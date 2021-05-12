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

vhc g_vehicles[2];
int g_vehicles_count = 0;

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
int turning = 0;
int jump = 0;
int left = 0;
int fwd=-1;
REALN vrotc,vrcmax,rotc; /*rot. speed and rotation of camera*/
int camflag=2; /*number of objects and of object types*/
vhc* g_vehicle = 0;

int run(const char* pCarFile, const char* pTrackFile)
{char numefis[MAXWLG],s[10];

int i;

pixcol backcol; /*culoarea fundalului*/
REALN  zfog,zmax; /*zfog,zmax - distanta de la care incepe ceatza, respectiv de la care nu se mai vede nimic*/
lightpr light;

sgob *objs,camera; /*objects*/
int nob,nto,ntotrk;

vhc car; /*vehicle*/
car.cmd_turn=0;
car.cmd_mode=1;
car.cmd_accelerate = 0;
car.cmd_brake = 0;
car.cmd_handbrake = 0;
car.vrx=0; car.arx=0;
car.vrxmr=0.36;
car.arxmr=car.vrxmr/1.5;
car.speed = car.dspeed = car.rotspeed = 0;

g_vehicles[g_vehicles_count++] = car;
g_vehicles[g_vehicles_count++] = car;
g_vehicle = &g_vehicles[0];

REALN tframe=0,xan=0,/*tframe-time necessary for display; xan-number of displayed images*/
      timp,dstr; /*total time, distance traveled*/

/*for game*/
REALN vrot3, /*rot. speed of level 3 objects*/
      realstep; /*real time step (s)*/
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
objs=readvehicle(numefis,objs,&nto,&nob,&g_vehicles[0]); /*read vehicle from file*/

objs=readvehicle("cars/car1",objs,&nto,&nob,&g_vehicles[1]); /*read vehicle from file*/
setPartPos(g_vehicles[1].bid[1],0,-2,14);  // move next to car

window_create(0,0,g_screen_width,g_screen_height);
window_set_key_handler(handle_key_event);

vrot3=1.57;
vrcmax=0.79;
vrotc=0;
rotc=0;

timp=0,dstr=0; /*pornit cronometru*/
tframe=0.5; /*assuming 2 frames/second*/

setcamg(&camera,g_vehicle,camflag);

while(!quit)
{
  time_process_frame();
  tframe = time_get_step_duration() / 1000.0;
  xan++;

/*simulation*/
nstepsf=(int)(tframe/STIMESTEP)+1; /*number of simulation steps/frame*/
realstep=tframe/nstepsf; /*simulation time step*/

REALN simspeed=0.1/realstep; /*decrease simulation speed if < 10fps*/
if(nstepsf>(int)simspeed){nstepsf=(int)simspeed;}


  runsteps(objs,&g_vehicles[0],realstep,nstepsf,tframe);
  timp += tframe;


for(i=1;i<=nob;i++){
  if(objs[i].lev==3){
    rotab(&objs[i],objs[i].vx[0],objs[i].vy[0],objs[i].vz[0],objs[i].vx[3],objs[i].vy[3],objs[i].vz[3],vrot3*tframe);
  }
}

if(fwd>-0.125)
{
  left = (left + turning) % 256;
  forward(g_vehicles[1].bid[1],fwd,left,jump);
}

rdspeed(g_vehicle,&g_vehicle->speed,&g_vehicle->rotspeed,&g_vehicle->dspeed);

tmformat(timp,s);
//switch(dmode){
//  case 1: sprintf(textglob,"%3.0f km/h   %s",speed*3.6,s);
//          break;
//  case -1: sprintf(textglob,"%3.0f km/h-R %s",speed*3.6,s);
//          break;
//  default: break;
//}


setcamg(&camera,g_vehicle,camflag);

rotc+=vrotc*tframe; if(camflag==2){rotc=0; vrotc=0;}
if(rotc){rotatx(&camera,objs[g_vehicles[0].oid[1]].vy[0],objs[g_vehicles[0].oid[1]].vz[0],rotc);}

odis(objs,nob,backcol,zfog,zmax,&camera,&light); /*display image*/

dstr+=(g_vehicle->speed*tframe);

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

#ifdef _WIN32

#define KEY_ESCAPE 27
#define KEY_SPACE 32
#define KEY_ARROW_LEFT 37
#define KEY_ARROW_UP 38
#define KEY_ARROW_RIGHT 39
#define KEY_ARROW_DOWN 40
#define KEY_C 67
#define KEY_M 77
#define KEY_N 78
#define KEY_R 82

#else

#define KEY_ESCAPE 65307
#define KEY_SPACE ' '
#define KEY_ARROW_LEFT 65361
#define KEY_ARROW_UP 65362
#define KEY_ARROW_RIGHT 65363
#define KEY_ARROW_DOWN 65364
#define KEY_A 'a'
#define KEY_C 'c'
#define KEY_D 'd'
#define KEY_M 'm'
#define KEY_N 'n'
#define KEY_R 'r'
#define KEY_W 'w'
#define KEY_PLUS '+'
#define KEY_MINUS '-'

#endif

void handle_key_event(unsigned int key, int press)
{
  switch(key)
  {
    case KEY_ESCAPE: quit = 1; break;
    case KEY_ARROW_UP:
      if(press) {
        g_vehicle->cmd_accelerate = g_vehicle->cmd_mode;
      } else {
        g_vehicle->cmd_accelerate = 0;
      }
      break;
    case KEY_ARROW_DOWN: g_vehicle->cmd_brake = press; break;
    case KEY_SPACE:   jump = press; g_vehicle->cmd_handbrake = press; break;
    case KEY_ARROW_LEFT:
      if(press) {
        g_vehicle->cmd_turn = -1;
      } else {
        if(g_vehicle->cmd_turn==-1) {
          g_vehicle->cmd_turn = 0;
        }
      }
      break;
    case KEY_ARROW_RIGHT:
      if(press) {
        g_vehicle->cmd_turn = 1;
      } else {
        if(g_vehicle->cmd_turn==1) {
          g_vehicle->cmd_turn = 0;
        }
      }
      break;
    case KEY_R: if(press) g_vehicle->cmd_mode=-g_vehicle->cmd_mode; else g_vehicle->cmd_accelerate=0; break;
    case KEY_C: if(press) camflag++; if(camflag>3){camflag=1;} rotc=0; vrotc=0; break;
    case KEY_N: if(press) vrotc=-vrcmax; else vrotc = 0; break;
    case KEY_M: if(press) vrotc=vrcmax;  else vrotc = 0; break;
    case KEY_W: fwd = 6*press; break;
    case KEY_A: turning = press; break;
    case KEY_D: turning = -1*press; break;
    case KEY_PLUS: if(press) g_zoom = g_zoom - 0.1; break;
    case KEY_MINUS: if(press) g_zoom = g_zoom + 0.1; break;
    default: break;
  }
}

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
  g_hInstance = hInstance;
  char carFile[256];
  char trackFile[256];
  char screenWidth[5];
  char screenHeight[5];
  char* argv[5] = {0,carFile,trackFile, screenWidth, screenHeight};
  int argc = sscanf(lpCmdLine, "%255s %255s %4s %4s", &carFile, &trackFile, &screenWidth, &screenHeight) + 1;

#else
int main(int argc,char *argv[])
{
#endif
  char* pCarFile   = "cars/car2";
  char* pTrackFile = "tracks/track9";

  if(argc>=6){printf("Error: Too many arguments\nExample: ./virreled cars/car1 tracks/track1 800 600\r\n");exit(1);}

  if(argc > 1)
  {
    pCarFile = argv[1];
  }

  if(argc > 2)
  {
    pTrackFile = argv[2];
  }

  if(argc==5)
  {
    g_screen_width  = atoi(argv[3]);
    g_screen_height = atoi(argv[4]);
  }

  if(g_screen_width < 10) {printf("Error: Screen width less than 10 not supported!\n"); exit(1);}
  if(g_screen_width > 4096) {printf("Error: Screen width greater than 4096 not supported!\n"); exit(1);}
  if(g_screen_height < 10) {printf("Error: Screen height less than 10 not supported!\n"); exit(1);}
  if(g_screen_height > 4096) {printf("Error: Screen height greater than 4096 not supported!\n"); exit(1);}

  return run(pCarFile, pTrackFile);
}
