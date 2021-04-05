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
#include <SDL.h>
#include <time.h>

#include "dyn/dconfig.h"
#include "dyn/datastr.h"
#include "dyn/transpart.h"
#include "dyn/findcont.h"

#include "config.h"

SDL_Window *RGLOBwindow; /*for SDL 2*/
void SDL_UpdateRect(SDL_Surface *screen,int a,int b,int c,int d)
{SDL_UpdateWindowSurface(RGLOBwindow);}

#if CLBITS==16
  #include "render16.h"
#elif CLBITS==24
  #include "render32.h"
#elif CLBITS==32
  #include "render32.h"
#endif

#include "defstr.h"
#include "trans.h"
#include "camera.h"
#include "readfile.h"
#include "game.h"


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


int main(int argc,char *argv[])
{char numefis[MAXWLG],s[10];

int i,quit=0,
    t0frame; /*t0frame - moment when image starts to be displayed*/

SDL_Event event;
SDL_Surface *screen;


#if SOUND==1
/* Open the audio device */
SDL_AudioSpec *desired, *obtained;
SDL_AudioSpec *hardware_spec;
REALN volum[6]={0,0,0,0,0,0};
#endif


pixcol backcol; /*culoarea fundalului*/
REALN  zfog,zmax; /*zfog,zmax - distanta de la care incepe ceatza, respectiv de la care nu se mai vede nimic*/
lightpr light;

sgob *objs,camera; /*objects*/
int nob,nto,ntotrk,camflag=2; /*number of objects and of object types*/

vhc car; /*vehicle*/

REALN tframe=0,xan=0,/*tframe-time necessary for display; xan-number of displayed images*/
      timp,dstr; /*total time, distance traveled*/

/*for game*/
REALN vrx,vrxmax,vrxmr, /*rot. speed*/
      arx,arxmax,arxmr, /*rot. acceleration*/
      vrot3, /*rot. speed of level 3 objects*/
      vrotc,vrcmax,rotc, /*rot. speed and rotation of camera*/
      realstep, /*real time step (s)*/
      speed,dspeed,rotspeed,acc,
      af=0,bf=0,hbf=0; /*acceleration, brake and handbrake factors*/
int turn, /*-1: left; 0: no turn; 1: right*/
    dmode, /*1 forward, -1 reverse*/
    nstepsf; /*number of simulation steps/frame*/
/*for game^*/


zfog=80;
zmax=120; /*visibility (m)*/

camera.vx[0]=0; camera.vy[0]=0; camera.vz[0]=0;
camera.vx[1]=1; camera.vy[1]=0; camera.vz[1]=0;
camera.vx[2]=0; camera.vy[2]=1; camera.vz[2]=0;
camera.vx[3]=0; camera.vy[3]=0; camera.vz[3]=1; /*set camera parameters*/

if(argc<=2){printf("Error: Input files not specified\r\nExample: ./simcar cars/car1 tracks/track1\r\n");exit(1);}
if(argc>=4){printf("Error: Too many arguments\r\n");exit(1);}


initSDE();
setGravity(-9.81,0.0,0.0);


strcpy(numefis,argv[2]);
objs=readtrack(numefis,&nob,&nto,&backcol,&light); /*read objects from file*/

ntotrk=nto;
if(ntotrk==4){zfog=240; zmax=360;}else{zfog=80; zmax=120;}

strcpy(numefis,argv[1]);
objs=readvehicle(numefis,objs,&nto,&nob,&car); /*read vehicle from file*/

printf("\r\n");


#if SOUND==1
/* Allocate a desired SDL_AudioSpec */
desired = (SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
/* Allocate space for the obtained SDL_AudioSpec */
obtained = (SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec));
/* 22050Hz - FM Radio quality */
desired->freq=22050;
/* 16-bit signed audio */
desired->format=AUDIO_U8;
/* Mono */
desired->channels=1;
/* Large audio buffer reduces risk of dropouts but increases response time */
desired->samples=1024; /*increase if using computer faster than 400MHz*/
/* Our callback function */
desired->callback=my_audio_callback;
desired->userdata=volum;
#endif


/*Initialize SDL*/
if(SDL_Init(SDL_INIT_VIDEO)<0){printf("Couldn't initialize SDL: %s\n", SDL_GetError());SDL_Quit();return 0;}
/*Initialize display SDL2*/
RGLOBwindow = SDL_CreateWindow(SIMCVER,SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,SCREENWIDTH,SCREENHEIGHT,SDL_WINDOW_SHOWN);
if(RGLOBwindow==NULL){
  printf("Couldn't create window: %s\n",SDL_GetError());SDL_Quit();return 0;
}
screen=SDL_GetWindowSurface(RGLOBwindow);
/*SDL2^*/
printf("Set %dx%dx%d\n",(screen->pitch)/(screen->format->BytesPerPixel),SCREENHEIGHT,screen->format->BitsPerPixel);

SDL_ShowCursor(SDL_DISABLE);

#if SOUND==1
/* Open the audio device */
if ( SDL_OpenAudio(desired, obtained) < 0 ){
  fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
  exit(-1);
}
/* desired spec is no longer needed */
free(desired);
hardware_spec=obtained;

volum[2]=hardware_spec->format;
volum[3]=hardware_spec->channels;
volum[4]=hardware_spec->freq; /*pentru trimis la callback()*/

/* Start playing */
SDL_PauseAudio(0);
#endif


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
sprintf(textglob,"--READY--");
odis(screen,objs,nob,backcol,zfog,zmax,&camera,&light); /*display image*/
SDL_Delay(200);
sprintf(textglob,"---SET---");
odis(screen,objs,nob,backcol,zfog,zmax,&camera,&light); /*display image*/
SDL_Delay(200);


while(!quit){

/*t0frame=clock();*/
t0frame=SDL_GetTicks();
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
switch(dmode){
  case 1: sprintf(textglob,"%3.0f km/h   %s",speed*3.6,s);
          break;
  case -1: sprintf(textglob,"%3.0f km/h-R %s",speed*3.6,s);
          break;
  default: break;
}


#if SOUND==1
volum[1]=rotspeed; if (volum[1]>200){volum[1]=200;}
volum[5]=acc;
#endif


setcamg(&camera,&car,camflag);

rotc+=vrotc*tframe; if(camflag==2){rotc=0; vrotc=0;}
if(rotc){rotatx(&camera,objs[car.oid[1]].vy[0],objs[car.oid[1]].vz[0],rotc);}

odis(screen,objs,nob,backcol,zfog,zmax,&camera,&light); /*display image*/

dstr+=(speed*tframe);


while(SDL_PollEvent(&event)){
switch(event.type){

case SDL_KEYDOWN:
	switch(event.key.keysym.sym){
		case SDLK_q:
		case SDLK_UP: af=dmode;
		             break;
		case SDLK_a:
		case SDLK_DOWN: bf=1;
		             break;
		case SDLK_SPACE: hbf=1;
		             break;
		case SDLK_o:
		case SDLK_LEFT: turn=-1;
		             break;
		case SDLK_p:
		case SDLK_RIGHT: turn=1;
		             break;

		case SDLK_r: dmode=-dmode;
		             break;
		
		case SDLK_c: camflag++; if(camflag>3){camflag=1;}
		             rotc=0; vrotc=0;
		             break;

		case SDLK_n: vrotc=-vrcmax;
		               break;

		case SDLK_m: vrotc=vrcmax;
		               break;

		case SDLK_ESCAPE: quit=1;

		default: break;
	} break;
case SDL_KEYUP:
	switch(event.key.keysym.sym){
		case SDLK_q:
		case SDLK_UP: af=0;
		             break;
		case SDLK_a:
		case SDLK_DOWN: bf=0;
		             break;
		case SDLK_SPACE: hbf=0;
		             break;
		case SDLK_o:
		case SDLK_LEFT: if(turn==-1){turn=0;}
		             break;
		case SDLK_p:
		case SDLK_RIGHT: if(turn==1){turn=0;}
		             break;

		case SDLK_r: af=0;
		             break;

		case SDLK_n: vrotc=0;
		               break;

		case SDLK_m: vrotc=0;
		               break;

		default: break;
	} break;

case SDL_QUIT: quit=1;

default: break;
}
}

/*tframe=(REALN)(clock()-t0frame)/CLOCKS_PER_SEC;*/
tframe=(REALN)(SDL_GetTicks()-t0frame)/1000;
}


printf("\r\n\r\n\r\n**********************************************\r\n");
printf("\r\nDistance: %1.2f km\r\nTime: %1.2f seconds\r\n",dstr/1000,timp);
printf("Average speed: %1.2f km/h\r\n",3.6*dstr/timp);
printf("Average framerate: %1.2f f/s\r\n\r\n",xan/timp);
printf("**********************************************\r\n\r\n");

#if SOUND==1
/* Stop playing */
SDL_PauseAudio(1);
SDL_CloseAudio();
#endif

SDL_Quit();

#if SOUND==1
free(obtained);
#endif

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

odis(0,0,0,backcol,0,0,0,0); /*freed static variables from odis() in "camera.h"*/

return 0;}
