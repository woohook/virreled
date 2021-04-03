/*
Copyright (C) 2014 Victor Matei Petrescu

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
#include <omp.h>
#include <time.h>

#include "src/dyn/dconfig.h"
#include "src/dyn/datastr.h"
#include "src/dyn/transpart.h"
#include "src/dyn/findcont.h"
#include "src/dyn/hash.h"
#include "src/dyn/walls.h"
#include "src/dyn/partgroup.h"


#include <SDL.h>
#include "src/demogfx/confgrf.h"
#include "src/demogfx/grafstr2.h"


void wallforce2text()
{int i;
char s[3]={'x','y','z'};
for(i=0;i<=5;i++){
  sprintf(textglob[5-i],"wall %c%d: frc=%1.3f, pos=%1.3f",s[i%3],i/3,DGLOBwall[i].force,DGLOBwall[i].pos);
}
}


int main()
{int i,crfail=0,dtime;
/*joint *jt;*/

SDL_Event event;
SDL_Surface *screen;
REALN zfog=15.0,zmax=30.0,
      vtz=0.0,vty=0.0,vtx=0.0,vrotx=0.0, /*translation and rotation speeds of camera*/
      lookx=0.0,looky=0.0,lookz=1.0, /*direction in which camera is looking*/
      tframe=0.05,
      simtime;
sgob *objs,camera; /*objects*/
int nob,nxf; /*number of objects*/
tria xface[101]; /*triangles not associated with any object*/

printf("Threads: "); scanf("%d",&DGLOBthreads);

int quit=0;

initSDE();

i=addPartGroup(0.75,1.00,0.0,-5.0,0.0,10.0,5.0,5.0);

printf("%d particles\r\n",DGLOBnrpart);

setGroupParam(i,27.0,1e5,1000.0,1000.0,0.5,1,0.0);
             /* ds  k   mu  c    mu2 r chf*/
/*setWallPos(0.0,-5.0,0.0,10.0,5.0,10.0);*/
setOptWallPos();
setWallK(1e4,1e4,1e4,1e4,1e4,1e4);
setWallMu(1.0,1.0,1.0,1.0,1.0,1.0);
setWallSpeed(0.0,0.0,0.0,0.0,0.0,0.0);
setWallForce(1e6,1e6,1e6,1e6,1e6,1e6);

setGravity(-9.81,0.0,0.0);

createtypes(); /*create object types*/

if(!(objs=(sgob *)malloc((DGLOBnrpart*10)*sizeof(sgob)))){printf("Out of memory"); exit(1);}

setcam(&camera,0,0,-5);

/*Initialize SDL*/
if(SDL_Init(SDL_INIT_VIDEO)<0){printf("Couldn't initialize SDL: %s\n", SDL_GetError());SDL_Quit();return 0;}
/*Initialize display*/
screen=SDL_SetVideoMode(SCREENWIDTH,SCREENHEIGHT,CLBITS,SDLSCREENFLAG);
if(screen == NULL){printf("Couldn't set requested video mode: %s\n",SDL_GetError());SDL_Quit();return 0;}
printf("Set %dx%dx%d\n",(screen->pitch)/(screen->format->BytesPerPixel),SCREENHEIGHT,screen->format->BitsPerPixel);
/*Set window title*/
SDL_WM_SetCaption("DEM", "DEM");
/* Enable Unicode translation */
SDL_EnableUNICODE(1);
/*SDL initialized*/

createOptHashSpace();

simtime=0.0;
quit=0;

while(!quit){

while(SDL_PollEvent(&event)){
switch(event.type){

  case SDL_KEYDOWN:
    switch(event.key.keysym.sym){
      case SDLK_a: vty=-5.0; break;
      case SDLK_d: vty=5.0; break;
      case SDLK_w: vtx=5.0; break;
      case SDLK_s: vtx=-5.0; break;
      case SDLK_LEFT: vrotx=1.0; break;
      case SDLK_RIGHT: vrotx=-1.0; break;
      case SDLK_UP: vtz=5.0; break;
      case SDLK_DOWN: vtz=-5.0; break;
      case SDLK_ESCAPE: quit=1; break;

      case SDLK_h: freeHashSpace(); createOptHashSpace(); break;
      case SDLK_p: setOptWallPos(); break;

      case SDLK_1: /*phase 1*/
        setWallPos(0.0,-7.0,-2.0,10.0,2.0,7.0);
        break;


      default: break;
  } break;

  case SDL_KEYUP:
    switch(event.key.keysym.sym){
      case SDLK_a: vty=0.0; break;
      case SDLK_d: vty=0.0; break;
      case SDLK_w: vtx=0.0; break;
      case SDLK_s: vtx=0.0; break;
      case SDLK_LEFT: vrotx=0.0; break;
      case SDLK_RIGHT: vrotx=0.0; break;
      case SDLK_UP: vtz=0.0; break;
      case SDLK_DOWN: vtz=0.0; break;
      default: break;
  } break;

case SDL_QUIT: quit=1; break;
}
}

dtime=SDL_GetTicks();
for(crfail=1;crfail<=20;crfail++){

solveAllContTrPar();
solveAllContWalls(1);

fillHashSpace();

solveHashContPart();
/*solveAllContPart()*/;

solveAllJoint();

stepSimPar(0.0001); simtime+=0.0001;

#if RCONTACTS==1
rmContacts();
#endif

clearHashSpace();
}
dtime=SDL_GetTicks()-dtime;


nob=convpart(objs,xface,&nxf);
/*crfail++;if(crfail>=2){printf("Here\r\n");exit(1);}*/


rotatx(&camera,camera.vy[0],camera.vz[0],vrotx*tframe);
lookx=camera.vx[3]-camera.vx[0];
looky=camera.vy[3]-camera.vy[0];
lookz=camera.vz[3]-camera.vz[0];
translat(&camera,vtz*lookx*tframe,vtz*looky*tframe,vtz*lookz*tframe);
lookx=camera.vx[2]-camera.vx[0];
looky=camera.vy[2]-camera.vy[0];
lookz=camera.vz[2]-camera.vz[0];
translat(&camera,vty*lookx*tframe,vty*looky*tframe,vty*lookz*tframe);
translat(&camera,vtx*tframe,0.0,0.0);

wallforce2text();
sprintf(textglob[6],"%1.2f ms/step, %1.2f s - press '1' after 2.0 s",(float)dtime/20.0,simtime);

odis(screen,objs,nob,xface,nxf,zfog,zmax,&camera); /*objs[0].nfa - nr. of objects*/;

}


SDL_Quit();

quitSDE();

free(objs);
for(i=1;i<=2;i++){free(fceglob[i]);} /*2 object types created with 'createtypes()' in 'readmods.h'*/
free(fceglob); free(nrfglob);

return 0;}
