/*
Copyright (C) 2007-2011 Victor Matei Petrescu

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

/*set camera*/
void setcamg(sgob *camera,vhc *car,int flag)
{REALN *pos;
REALN x0,y0,z0,a,h,l,d;
particle *part;

part=&DGLOBpart[car->bid[1]];

pos=part->pos;

switch(flag){
  case 1: a=0.9;
          x0=pos[0]+25.0; y0=pos[1]; z0=pos[2]-10.0;
          camera->vx[0]=x0; camera->vy[0]=y0; camera->vz[0]=z0;
          camera->vx[1]=x0+cos(a); camera->vy[1]=y0; camera->vz[1]=z0+sin(a);
          camera->vx[2]=x0; camera->vy[2]=y0+1; camera->vz[2]=z0;
          camera->vx[3]=x0-sin(a); camera->vy[3]=y0; camera->vz[3]=z0+cos(a);
          break;

  case 2: h=car->camh; l=car->caml; d=car->camd;
          x0=pos[0]; y0=pos[1]; z0=pos[2];
          camera->vx[0]=x0; camera->vy[0]=y0; camera->vz[0]=z0;
          camera->vx[1]=x0+part->vx[0]; camera->vy[1]=y0+part->vy[0]; camera->vz[1]=z0+part->vz[0];
          camera->vx[2]=x0+part->vx[1]; camera->vy[2]=y0+part->vy[1]; camera->vz[2]=z0+part->vz[1];
          camera->vx[3]=x0+part->vx[2]; camera->vy[3]=y0+part->vy[2]; camera->vz[3]=z0+part->vz[2];
          translat(camera,h*part->vx[0]+d*part->vx[2]+l*part->vx[1],h*part->vy[0]+d*part->vy[2]+l*part->vy[1],h*part->vz[0]+d*part->vz[2]+l*part->vz[1]);
          break;

  case 3: x0=pos[0]+3.0; y0=pos[1]; z0=pos[2]-11.0;
          camera->vx[0]=x0; camera->vy[0]=y0; camera->vz[0]=z0;
          camera->vx[1]=x0+1; camera->vy[1]=y0; camera->vz[1]=z0;
          camera->vx[2]=x0; camera->vy[2]=y0+1; camera->vz[2]=z0;
          camera->vx[3]=x0; camera->vy[3]=y0; camera->vz[3]=z0+1;
          y0=part->vy[2]; z0=part->vz[2]; a=sqrt(y0*y0+z0*z0);
          if(fabs(a)>1e-5){y0/=a; z0/=a;}else{y0=0; z0=1;}
          h=camera->vy[3]-camera->vy[0]; d=camera->vz[3]-camera->vz[0];
          a=y0*h+z0*d;
          if(a>1){a=1;}else{if(a<-1){a=-1;}}
          if(y0>0){a=acos(a);}else{a=-acos(a);}
          rotatx(camera,pos[1],pos[2],-a);
          break;

  default: break;
}
}


/*function which displays the objcts which are closer than zmax
nob - total number of objects
cam - camera*/
void odis(sgob *objs,int nob,pixcol backcol,REALN zfog,REALN zmax,sgob *cam,lightpr *light)
{int i,j,focal;
unsigned int width,height;
unsigned long int area;
static int sem=0,nrfm=0; /*number of triangles for which memory has been allocated*/
static tria *face,*facedisp; /*triangles and displayed triangles in global system*/
static sgob *obdis; /*displayed objects (copies)*/
static REALN *distmin; /*Zbuffer for sending to display()*/
int nrfaces,nrdisp,crf, /*number of triangles and of displayed triangles, current triangle*/
    nobdis=0; /*number of displayed objects*/

lightpr rotlight; /*rotated light parameters*/

REALN tgh,tgv,zmin;
REALN x,y,z,ix,iy,iz,jx,jy,jz,kx,ky,kz; /*temporary variables for transformations*/

if(nob==0){free(face); free(facedisp); free(obdis); free(distmin); return;}
/*to free static variables, call odis(0,0,0,backcol,0,0,0,0)*/

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
  if(!(distmin=(REALN *)malloc(area*sizeof(REALN)))){printf("Out of memory");}
  if(!(face=(tria *)malloc(11*sizeof(tria)))){printf("Out of memory");}
  if(!(facedisp=(tria *)malloc(22*sizeof(tria)))){printf("Out of memory");}
  if(!(obdis=(sgob *)malloc((nob+1)*sizeof(sgob)))){printf("Out of memory");}
  nrfm=1; sem=1;}

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

x=light->dx;
y=light->dy;
z=light->dz;
rotlight.dx=x*ix+y*iy+z*iz;
rotlight.dy=x*jx+y*jy+z*jz;
rotlight.dz=x*kx+y*ky+z*kz; /*rotated light*/
rotlight.ambient=light->ambient;
rotlight.headlight=light->headlight;
rotlight.directional=light->directional;

nrfaces=0;
crf=0;
for(i=1;i<=nobdis;i++){
  if((obdis[i].ycen-obdis[i].radius)>(obdis[i].zcen+obdis[i].radius)*tgh){continue;}
  if((obdis[i].ycen+obdis[i].radius)<(obdis[i].zcen+obdis[i].radius)*(-tgh)){continue;}
  if((obdis[i].xcen-obdis[i].radius)>(obdis[i].zcen+obdis[i].radius)*tgv){continue;}
  if((obdis[i].xcen+obdis[i].radius)<(obdis[i].zcen+obdis[i].radius)*(-tgv)){continue;}
  if(((obdis[i].zcen-obdis[i].radius)<zmax)&&((obdis[i].zcen+obdis[i].radius)>0)){
    nrfaces+=obdis[i].nfa;
    if(nrfaces>nrfm){
      nrfm=nrfaces;
      if(!(face=(tria *)realloc(face,(nrfm+10)*sizeof(tria)))){printf("Out of memory");}
      if(!(facedisp=(tria *)realloc(facedisp,(2*nrfm+20)*sizeof(tria)))){printf("Out of memory");}
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

    for(j=1;j<=obdis[i].nfa;j++){
      face[j+crf]=fceglob[obdis[i].otyp][j]; /*added triangles*/
        x=fceglob[obdis[i].otyp][j].x1;
        y=fceglob[obdis[i].otyp][j].y1;
        z=fceglob[obdis[i].otyp][j].z1;
      face[j+crf].x1=obdis[i].vx[0]+x*ix+y*jx+z*kx;
      face[j+crf].y1=obdis[i].vy[0]+x*iy+y*jy+z*ky;
      face[j+crf].z1=obdis[i].vz[0]+x*iz+y*jz+z*kz;
        x=fceglob[obdis[i].otyp][j].x2;
        y=fceglob[obdis[i].otyp][j].y2;
        z=fceglob[obdis[i].otyp][j].z2;
      face[j+crf].x2=obdis[i].vx[0]+x*ix+y*jx+z*kx;
      face[j+crf].y2=obdis[i].vy[0]+x*iy+y*jy+z*ky;
      face[j+crf].z2=obdis[i].vz[0]+x*iz+y*jz+z*kz;
        x=fceglob[obdis[i].otyp][j].x3;
        y=fceglob[obdis[i].otyp][j].y3;
        z=fceglob[obdis[i].otyp][j].z3;
      face[j+crf].x3=obdis[i].vx[0]+x*ix+y*jx+z*kx;
      face[j+crf].y3=obdis[i].vy[0]+x*iy+y*jy+z*ky;
      face[j+crf].z3=obdis[i].vz[0]+x*iz+y*jz+z*kz; /*updated positions of triangles*/
    }
    crf=nrfaces;
  }
}

nrdisp=fclip(face,nrfaces,zmin,facedisp,zmax,tgh,tgv);

display(facedisp,nrdisp,distmin,width,height,focal,backcol,zfog,zmax,&rotlight);

x11_flush();
}
