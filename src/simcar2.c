/*
Copyright (C) 2010-2014 Victor Matei Petrescu

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int quit = 0;

#include "time/time.h"
#include "display/display.h"

#include "physics/dconfig.h"
#include "physics/datastr.h"
#include "physics/transpart.h"
#include "physics/findcont.h"

#include "config.h"

unsigned int g_screen_width  = 800;
unsigned int g_screen_height = 600;

#include "render32.h"

#include "defstr.h"
#include "trans.h"
#include "camera.h"
#include "readfile.h"
#include "physics/physics.h"

void handle_key_event(unsigned int,int);

/*print time*/
void tmformat(REALN tm,char *s)
{int hr,mn,sc;
hr=(int)(tm/3600.0);
mn=(int)(tm/60.0-hr*60.0);
sc=(int)(tm-mn*60.0);
sprintf(s,"%2d:%2d:%2d",hr,mn,sc);
if(hr<10){s[0]='0';}
if(mn<10){s[3]='0';}
if(sc<10){s[6]='0';}
}

REALN af=0,bf=0,hbf=0; /*acceleration, brake and handbrake factors*/
int turn, /*-1: left; 0: no turn; 1: right*/
    dmode; /*1 forward, -1 reverse*/
REALN vrotc,vrcmax,rotc; /*rot. speed and rotation of camera*/
int camflag=2; /*number of objects and of object types*/

int run(const char* pCarFile, const char* pTrackFile)
{char numefis[MAXWLG],s[10];

int i,
    t0frame; /*t0frame - moment when image starts to be displayed*/

pixcol backcol; /*culoarea fundalului*/
REALN  zfog,zmax; /*zfog,zmax - distanta de la care incepe ceatza, respectiv de la care nu se mai vede nimic*/
lightpr light;

sgob *objs,camera; /*objects*/
int nob,nto,ntotrk;

vhc car; /*vehicle*/

REALN tframe=0,xan=0,/*tframe-time necessary for display; xan-number of displayed images*/
      timp,dstr; /*total time, distance traveled*/

/*for game*/
REALN vrx,vrxmax,vrxmr, /*rot. speed*/
      arx,arxmax,arxmr, /*rot. acceleration*/
      vrot3, /*rot. speed of level 3 objects*/
      realstep, /*real time step (s)*/
      speed,dspeed,rotspeed,acc;
int nstepsf; /*number of simulation steps/frame*/
/*for game^*/


zfog=80;
zmax=120; /*visibility (m)*/

camera.vx[0]=0; camera.vy[0]=0; camera.vz[0]=0;
camera.vx[1]=1; camera.vy[1]=0; camera.vz[1]=0;
camera.vx[2]=0; camera.vy[2]=1; camera.vz[2]=0;
camera.vx[3]=0; camera.vy[3]=0; camera.vz[3]=1; /*set camera parameters*/

initSDE();
setGravity(-9.81,0.0,0.0);


strcpy(numefis,pTrackFile);
objs=readtrack(numefis,&nob,&nto,&backcol,&light); /*read objects from file*/

ntotrk=nto;
if(ntotrk==4){zfog=240; zmax=360;}else{zfog=80; zmax=120;}

strcpy(numefis,pCarFile);
objs=readvehicle(numefis,objs,&nto,&nob,&car); /*read vehicle from file*/

window_create(0,0,g_screen_width,g_screen_height);
window_set_key_handler(handle_key_event);

vrx=0; arx=0;
vrxmr=vrxmax=0.36;
arxmr=arxmax=vrxmax/1.5;
turn=0;
dmode=1;
vrot3=1.57;
vrcmax=0.79;
vrotc=0;
rotc=0;

timp=0,dstr=0; /*pornit cronometru*/
tframe=0.5; /*assuming 2 frames/second*/

setcamg(&camera,&car,camflag);

while(!quit)
{
  time_process_frame();
  tframe = time_get_step_duration() / 1000.0;
  xan++;

if(speed<10){vrxmr=vrxmax; arxmr=arxmax;}
else{
	vrxmr=vrxmax/(0.1*speed);
	arxmr=arxmax/(0.1*speed);
}

switch(turn){
  case 0: if(vrx>0){arx=-arxmr*1.5;}else{if(vrx<0){arx=arxmr*1.5;}else{arx=0;}}
          if(fabs(vrx)<2.25*tframe*arx){arx=0; vrx=0;}
          break;
  case -1: if(vrx>-vrxmr){arx=-arxmr; if(vrx>0){arx*=1.5;}}else{arx=0;}
           break;
  case 1: if(vrx<vrxmr){arx=arxmr; if(vrx<0){arx*=1.5;}}else{arx=0;}
          break;
  default: break;
}

vrx+=arx*tframe;
if(vrx>vrxmr){vrx=vrxmr;}
if(vrx<-vrxmr){vrx=-vrxmr;}


/*simulation*/
nstepsf=(int)(tframe/STIMESTEP)+1; /*number of simulation steps/frame*/
realstep=tframe/nstepsf; /*simulation time step*/

speed=0.1/realstep; /*decrease simulation speed if < 10fps*/
if(nstepsf>(int)speed){nstepsf=(int)speed;}


  runsteps(objs,nob,&car,realstep,nstepsf,vrx,af,bf,hbf);
  timp += tframe;


for(i=1;i<=nob;i++){
  if(objs[i].lev==3){
    rotab(&objs[i],objs[i].vx[0],objs[i].vy[0],objs[i].vz[0],objs[i].vx[3],objs[i].vy[3],objs[i].vz[3],vrot3*tframe);
  }
}

rdspeed(&car,&speed,&rotspeed,&dspeed);
acc=dspeed/tframe;

tmformat(timp,s);
//switch(dmode){
//  case 1: sprintf(textglob,"%3.0f km/h   %s",speed*3.6,s);
//          break;
//  case -1: sprintf(textglob,"%3.0f km/h-R %s",speed*3.6,s);
//          break;
//  default: break;
//}


setcamg(&camera,&car,camflag);

rotc+=vrotc*tframe; if(camflag==2){rotc=0; vrotc=0;}
if(rotc){rotatx(&camera,objs[car.oid[1]].vy[0],objs[car.oid[1]].vz[0],rotc);}

odis(objs,nob,backcol,zfog,zmax,&camera,&light); /*display image*/

dstr+=(speed*tframe);

    window_process_events();
}


printf("\r\n\r\n\r\n**********************************************\r\n");
printf("\r\nDistance: %1.2f km\r\nTime: %1.2f seconds\r\n",dstr/1000,timp);
printf("Average speed: %1.2f km/h\r\n",3.6*dstr/timp);
printf("Average framerate: %1.2f f/s\r\n\r\n",xan/timp);
printf("**********************************************\r\n\r\n");

for(i=1;i<=nto;i++){
  free(fceglob[i]);
}

for(i=1;i<=ntotrk;i++){
  free(refglob[i].p1);
  free(refglob[i].p2);
  free(refglob[i].p3);
}
free(fceglob); free (refglob); free(objs);

/* printf("Press ENTER: ");getchar();printf("\r\n"); */

quitSDE();

odis(0,0,backcol,0,0,0,0); /*freed static variables from odis() in "camera.h"*/

window_destroy();

return 0;}

void handle_key_event(unsigned int key, int press)
{
  switch(key)
  {
#ifdef _WIN32
    case 27: quit = 1; break;
    case 38: if(press) af=dmode; else af=0; break;
    case 40: bf=press; break;
    case 32: hbf=press; break;
    case 37: if(press) turn=-1; else if(turn==-1) turn = 0; break;
    case 39: if(press) turn=1;  else if(turn==1)  turn = 0; break;
    case 82: if(press) dmode=-dmode; else af=0; break;
    case 67: if(press) camflag++; if(camflag>3){camflag=1;} rotc=0; vrotc=0; break;
    case 78: if(press) vrotc=-vrcmax; else vrotc = 0; break;
    case 77: if(press) vrotc=vrcmax;  else vrotc = 0; break;
#else
    case 65307: quit = 1; break;
    case 65362: if(press) af=dmode; else af=0; break;
    case 65364: bf=press; break;
    case ' ':   hbf=press; break;
    case 65361: if(press) turn=-1; else if(turn==-1) turn = 0; break;
    case 65363: if(press) turn=1;  else if(turn==1)  turn = 0; break;
    case 'r':   if(press) dmode=-dmode; else af=0; break;
    case 'c':   if(press) camflag++; if(camflag>3){camflag=1;} rotc=0; vrotc=0; break;
    case 'n':   if(press) vrotc=-vrcmax; else vrotc = 0; break;
    case 'm':   if(press) vrotc=vrcmax;  else vrotc = 0; break;
#endif
    default: break;
  }
}

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  g_hInstance = hInstance;
  char pCarFile[256];
  char pTrackFile[256];
  char* argv[3] = {0,pCarFile,pTrackFile};
  int argc = sscanf(lpCmdLine, "%255s %255s", &pCarFile, &pTrackFile) + 1;

#else
int main(int argc,char *argv[])
{
#endif

  if(argc<=2){printf("Error: Input files not specified\r\nExample: ./simcar cars/car1 tracks/track1\r\n");exit(1);}
  if(argc>=4){printf("Error: Too many arguments\r\n");exit(1);}

  return run(argv[1], argv[2]);
}
