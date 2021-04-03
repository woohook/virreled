/*
Copyright (C) 2007-2013 Victor Matei Petrescu

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

#include <math.h>
#include <SDL.h>
#include "confgrf.h"
#include "grafstr.h"

/*convert particles into graphical objects; return nr. of objects*/
int convpart(sgob *objs,tria *xface,int *nxf)
{int i,j,nrpart;
triangle *tr,*trstart;
particle *part;

#if RCONTACTS==1
REALD x[3],y[3],min,tmp;
int jmin;
contact *cont,*ctstart;

ctstart=DGLOBctstart;
#endif

nrpart=DGLOBnrpart;
trstart=DGLOBtrstart;
part=DGLOBpart;

*nxf=0;

for(i=1;i<=nrpart;i++){
  objs[i].otyp=1;
  objs[i].nfa=nrfglob[objs[i].otyp];
  objs[i].radius=part[i].radius;
  objs[i].scale=part[i].radius;

  objs[i].xcen=objs[i].vx[0]=part[i].pos[0];
  objs[i].ycen=objs[i].vy[0]=part[i].pos[1];
  objs[i].zcen=objs[i].vz[0]=part[i].pos[2];

  for(j=0;j<=2;j++){
    objs[i].vx[j+1]=part[i].pos[0]+part[i].vx[j];
    objs[i].vy[j+1]=part[i].pos[1]+part[i].vy[j];
    objs[i].vz[j+1]=part[i].pos[2]+part[i].vz[j];
  }
}

tr=trstart->next;
while(tr!=0){
  (*nxf)++;
  xface[*nxf].x1=tr->p1[0];
  xface[*nxf].y1=tr->p1[1];
  xface[*nxf].z1=tr->p1[2];
  xface[*nxf].x2=tr->p2[0];
  xface[*nxf].y2=tr->p2[1];
  xface[*nxf].z2=tr->p2[2];
  xface[*nxf].x3=tr->p3[0];
  xface[*nxf].y3=tr->p3[1];
  xface[*nxf].z3=tr->p3[2];
  xface[*nxf].red=50; xface[*nxf].green=50; xface[*nxf].blue=150;
  xface[*nxf].cull=0;
  tr=tr->next;
}

#if RCONTACTS==1
cont=ctstart->next;
i=nrpart;

while(cont!=0){
  i++;
  objs[i].otyp=2;
  objs[i].nfa=nrfglob[objs[i].otyp];
  objs[i].radius=1.0;
  objs[i].scale=1.0;

  objs[i].xcen=objs[i].vx[0]=cont->pos[0];
  objs[i].ycen=objs[i].vy[0]=cont->pos[1];
  objs[i].zcen=objs[i].vz[0]=cont->pos[2];

  objs[i].vx[3]=objs[i].vx[0]+cont->dir[0];
  objs[i].vy[3]=objs[i].vy[0]+cont->dir[1];
  objs[i].vz[3]=objs[i].vz[0]+cont->dir[2]; /*local z axis - contact direction*/

  jmin=0; min=fabs(cont->dir[0]);
  for(j=1;j<=2;j++){
    tmp=fabs(cont->dir[j]);
    if(tmp<min){jmin=j; min=tmp;}
  } /*jmin-axis with shortest projection of contact direction*/

  x[0]=x[1]=x[2]=0; x[jmin]=1;
  crossProd(cont->dir,x,y);
  normVec(y);
  objs[i].vx[2]=objs[i].vx[0]+y[0];
  objs[i].vy[2]=objs[i].vy[0]+y[1];
  objs[i].vz[2]=objs[i].vz[0]+y[2];

  crossProd(y,cont->dir,x);
  objs[i].vx[1]=objs[i].vx[0]+x[0];
  objs[i].vy[1]=objs[i].vy[0]+x[1];
  objs[i].vz[1]=objs[i].vz[0]+x[2];

  cont=cont->next;
}
#endif

return i;}


/*set camera position; look at z*/
void setcam(sgob *camera,REALN x,REALN y,REALN z)
{camera->vx[0]=x;
 camera->vy[0]=y;
 camera->vz[0]=z;
camera->vx[1]=x+1;
camera->vy[1]=y;
camera->vz[1]=z;
camera->vx[2]=x;
camera->vy[2]=y+1;
camera->vz[2]=z;
camera->vx[3]=x;
camera->vy[3]=y;
camera->vz[3]=z+1;
}


/*function which displays the objcts which are closer than zmax
nob - total number of objects
cam - camera
xface - triangles not associated with any object
nxf - nr. of triangles in xface*/
void odis(SDL_Surface *screen,sgob *objs,int nob,tria *xface,int nxf,REALN zfog,REALN zmax,sgob *cam)
{int i,j,focal;
unsigned int width,height;
unsigned long int area;
static int sem=0,nrfm=0,nobm=0; /*nr. of triangles/displayed objects for which memory was allocated*/
static tria *face,*facedisp; /*triangles and displayed triangles in global system*/
static sgob *obdis; /*displayed objects (copies)*/
static REALN *distmin; /*Zbuffer for sending to displaysdl()*/
int nrfaces,nrdisp,crf, /*number of triangles and of displayed triangles, current triangle*/
    nobdis=0; /*number of displayed objects*/

pixcol backcol;
lightpr *light,rotlight,tmplight; /*light and rotated light parameters*/

REALN tgh,tgv,zmin;
REALN x,y,z,ix,iy,iz,jx,jy,jz,kx,ky,kz,
      scale; /*temporary variables for transformations*/

light=&tmplight;

light->ambient=0.5;
light->headlight=0.3;
light->directional=0.5;
light->dx=-0.5;
light->dy=1;
light->dz=1; /*set default values for the light*/

backcol.red=70;
backcol.green=70;
backcol.blue=70;

width=SCREENWIDTH;
height=SCREENHEIGHT;
focal=(int)(width/(2*tan(FOV*0.008726646)));

area=(width+1)*(height+1);

tgh=(REALN)width/(2*(REALN)focal);
tgv=(REALN)height/(2*(REALN)focal);

#if ASPCOR==1
  focal=(int)((REALN)focal/WIDTHFACTOR);
  tgv*=WIDTHFACTOR;
#elif ASPCOR!=0
  printf("Error: ASPCOR should be set to 0 or 1 in 'src/config.h'\r\n"); exit(1);
#endif

zmin=1e-3;

if(!sem){
  if(!(distmin=(REALN *)malloc(area*sizeof(REALN)))){printf("Out of memory"); exit(1);}
  if(!(face=(tria *)malloc((nxf+10)*sizeof(tria)))){printf("Out of memory"); exit(1);}
  if(!(facedisp=(tria *)malloc((2*nxf+20)*sizeof(tria)))){printf("Out of memory"); exit(1);}
  if(!(obdis=(sgob *)malloc((nob+1)*sizeof(sgob)))){printf("Out of memory"); exit(1);}
  nrfm=1; nobm=nob; sem=1;}

for(i=1;i<=nxf;i++){face[i]=xface[i];}

if(nob>nobm){
  nobm=nob;
  if(!(obdis=(sgob *)realloc(obdis,(nob+1)*sizeof(sgob)))){printf("Out of memory"); exit(1);}
}

for(i=1;i<=nob;i++){
  if(objs[i].xcen-cam->vx[0]-objs[i].radius>(1.74*zmax)){continue;}
  if(objs[i].xcen-cam->vx[0]+objs[i].radius<(-1.74*zmax)){continue;}
  if(objs[i].ycen-cam->vy[0]-objs[i].radius>(1.74*zmax)){continue;}
  if(objs[i].ycen-cam->vy[0]+objs[i].radius<(-1.74*zmax)){continue;}
  if(objs[i].zcen-cam->vz[0]-objs[i].radius>(1.74*zmax)){continue;}
  if(objs[i].zcen-cam->vz[0]+objs[i].radius<(-1.74*zmax)){continue;}
  nobdis++;
  obdis[nobdis]=objs[i];
} /*copied objects*/

ix=cam->vx[1]-cam->vx[0];
jx=cam->vx[2]-cam->vx[0];
kx=cam->vx[3]-cam->vx[0];
  iy=cam->vy[1]-cam->vy[0];
  jy=cam->vy[2]-cam->vy[0];
  ky=cam->vy[3]-cam->vy[0];
    iz=cam->vz[1]-cam->vz[0];
    jz=cam->vz[2]-cam->vz[0];
    kz=cam->vz[3]-cam->vz[0]; /*unit vectors of the local axes of camera in global system*/

for(i=1;i<=nobdis;i++){
  for(j=0;j<=3;j++){
    obdis[i].vx[j]-=cam->vx[0];
    obdis[i].vy[j]-=cam->vy[0];
    obdis[i].vz[j]-=cam->vz[0];
  }
  obdis[i].xcen-=cam->vx[0];
  obdis[i].ycen-=cam->vy[0];
  obdis[i].zcen-=cam->vz[0]; /*translated object*/

  for(j=0;j<=3;j++){
    x=obdis[i].vx[j];
    y=obdis[i].vy[j];
    z=obdis[i].vz[j];
    obdis[i].vx[j]=x*ix+y*iy+z*iz;
    obdis[i].vy[j]=x*jx+y*jy+z*jz;
    obdis[i].vz[j]=x*kx+y*ky+z*kz;
  }
  x=obdis[i].xcen;
  y=obdis[i].ycen;
  z=obdis[i].zcen;
  obdis[i].xcen=x*ix+y*iy+z*iz;
  obdis[i].ycen=x*jx+y*jy+z*jz;
  obdis[i].zcen=x*kx+y*ky+z*kz; /*rotated objects*/
}

for(i=1;i<=nxf;i++){
  face[i].x1-=cam->vx[0]; face[i].y1-=cam->vy[0]; face[i].z1-=cam->vz[0];
  face[i].x2-=cam->vx[0]; face[i].y2-=cam->vy[0]; face[i].z2-=cam->vz[0];
  face[i].x3-=cam->vx[0]; face[i].y3-=cam->vy[0]; face[i].z3-=cam->vz[0];
    x=face[i].x1; y=face[i].y1; z=face[i].z1;
    face[i].x1=x*ix+y*iy+z*iz;
    face[i].y1=x*jx+y*jy+z*jz;
    face[i].z1=x*kx+y*ky+z*kz;
  x=face[i].x2; y=face[i].y2; z=face[i].z2;
  face[i].x2=x*ix+y*iy+z*iz;
  face[i].y2=x*jx+y*jy+z*jz;
  face[i].z2=x*kx+y*ky+z*kz;
    x=face[i].x3; y=face[i].y3; z=face[i].z3;
    face[i].x3=x*ix+y*iy+z*iz;
    face[i].y3=x*jx+y*jy+z*jz;
    face[i].z3=x*kx+y*ky+z*kz;
}

x=light->dx;
y=light->dy;
z=light->dz;
rotlight.dx=x*ix+y*iy+z*iz;
rotlight.dy=x*jx+y*jy+z*jz;
rotlight.dz=x*kx+y*ky+z*kz; /*rotated light*/
rotlight.ambient=light->ambient;
rotlight.headlight=light->headlight;
rotlight.directional=light->directional;

nrfaces=crf=nxf;

for(i=1;i<=nobdis;i++){
  if((obdis[i].ycen-obdis[i].radius)>(obdis[i].zcen+obdis[i].radius)*tgh){continue;}
  if((obdis[i].ycen+obdis[i].radius)<(obdis[i].zcen+obdis[i].radius)*(-tgh)){continue;}
  if((obdis[i].xcen-obdis[i].radius)>(obdis[i].zcen+obdis[i].radius)*tgv){continue;}
  if((obdis[i].xcen+obdis[i].radius)<(obdis[i].zcen+obdis[i].radius)*(-tgv)){continue;}
  if(((obdis[i].zcen-obdis[i].radius)<zmax)&&((obdis[i].zcen+obdis[i].radius)>0)){
    nrfaces+=obdis[i].nfa;
    if(nrfaces>nrfm){
      nrfm=nrfaces;
      if(!(face=(tria *)realloc(face,(nrfm+10)*sizeof(tria)))){printf("Out of memory"); exit(1);}
      if(!(facedisp=(tria *)realloc(facedisp,(2*nrfm+20)*sizeof(tria)))){printf("Out of memory"); exit(1);}
    }

    ix=obdis[i].vx[1]-obdis[i].vx[0];
    jx=obdis[i].vx[2]-obdis[i].vx[0];
    kx=obdis[i].vx[3]-obdis[i].vx[0];
      iy=obdis[i].vy[1]-obdis[i].vy[0];
      jy=obdis[i].vy[2]-obdis[i].vy[0];
      ky=obdis[i].vy[3]-obdis[i].vy[0];
        iz=obdis[i].vz[1]-obdis[i].vz[0];
        jz=obdis[i].vz[2]-obdis[i].vz[0];
        kz=obdis[i].vz[3]-obdis[i].vz[0]; /*unit vectors of the local axes in global system*/

    scale=obdis[i].scale;

    for(j=1;j<=obdis[i].nfa;j++){
      face[j+crf]=fceglob[obdis[i].otyp][j]; /*added triangles*/
        x=scale*fceglob[obdis[i].otyp][j].x1;
        y=scale*fceglob[obdis[i].otyp][j].y1;
        z=scale*fceglob[obdis[i].otyp][j].z1;
      face[j+crf].x1=obdis[i].vx[0]+x*ix+y*jx+z*kx;
      face[j+crf].y1=obdis[i].vy[0]+x*iy+y*jy+z*ky;
      face[j+crf].z1=obdis[i].vz[0]+x*iz+y*jz+z*kz;
        x=scale*fceglob[obdis[i].otyp][j].x2;
        y=scale*fceglob[obdis[i].otyp][j].y2;
        z=scale*fceglob[obdis[i].otyp][j].z2;
      face[j+crf].x2=obdis[i].vx[0]+x*ix+y*jx+z*kx;
      face[j+crf].y2=obdis[i].vy[0]+x*iy+y*jy+z*ky;
      face[j+crf].z2=obdis[i].vz[0]+x*iz+y*jz+z*kz;
        x=scale*fceglob[obdis[i].otyp][j].x3;
        y=scale*fceglob[obdis[i].otyp][j].y3;
        z=scale*fceglob[obdis[i].otyp][j].z3;
      face[j+crf].x3=obdis[i].vx[0]+x*ix+y*jx+z*kx;
      face[j+crf].y3=obdis[i].vy[0]+x*iy+y*jy+z*ky;
      face[j+crf].z3=obdis[i].vz[0]+x*iz+y*jz+z*kz; /*updated positions of triangles*/
    }
    crf=nrfaces;
  }
}

nrdisp=fclip(face,nrfaces,zmin,facedisp,zmax,tgh,tgv);

displaysdl(screen,facedisp,nrdisp,distmin,width,height,focal,backcol,zfog,zmax,&rotlight);

}
