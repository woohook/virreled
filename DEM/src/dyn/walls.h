/*
Copyright (C) 2013-2014 Victor Matei Petrescu

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


typedef struct _wall{
REALD pos; /*position*/
REALD speed; /*specified speed*/
REALD force;
REALD spforce; /*specified force*/
REALD k; /*elastic coef.*/
REALD mu; /*friction coef.*/
} wall;


/*GLOBAL VARIABLE for walls ---------------*/
wall DGLOBwall[6]; /*[0] x,[1] y,[2] z,[3] -x,[4] -y,[5] -z*/
/*GLOBAL VARIABLE for walls ---------------*/


void setWallPos(REALD x1,REALD y1,REALD z1,REALD x2,REALD y2,REALD z2)
{int i;
DGLOBwall[0].pos=x1;
DGLOBwall[1].pos=y1;
DGLOBwall[2].pos=z1;
DGLOBwall[3].pos=x2;
DGLOBwall[4].pos=y2;
DGLOBwall[5].pos=z2;
for(i=0;i<=5;i++){
  DGLOBwall[i].speed=0.0;
  DGLOBwall[i].spforce=0.0;
}
}


/*sets optimal positions for walls; some particles must be declared before*/
void setOptWallPos()
{int i;
REALD dmax=0.0,d,x0,y0,z0,
      xmin,ymin,zmin,xmax,ymax,zmax;

xmin=xmax=DGLOBpart[1].pos[0];
ymin=ymax=DGLOBpart[1].pos[1];
zmin=zmax=DGLOBpart[1].pos[2];

for(i=2;i<=DGLOBnrpart;i++){
  x0=DGLOBpart[i].pos[0];
  y0=DGLOBpart[i].pos[1];
  z0=DGLOBpart[i].pos[2];
  d=2.0*DGLOBpart[i].radius;
  if(x0<xmin){xmin=x0;}
  if(x0>xmax){xmax=x0;}
  if(y0<ymin){ymin=y0;}
  if(y0>ymax){ymax=y0;}
  if(z0<zmin){zmin=z0;}
  if(z0>zmax){zmax=z0;}
  if(d>dmax){dmax=d;}
}

xmin-=0.5*dmax; ymin-=0.5*dmax; zmin-=0.5*dmax;
xmax+=0.5*dmax; ymax+=0.5*dmax; zmax+=0.5*dmax;

setWallPos(xmin,ymin,zmin,xmax,ymax,zmax);
}


void setWallK(REALD x1,REALD y1,REALD z1,REALD x2,REALD y2,REALD z2)
{DGLOBwall[0].k=x1;
 DGLOBwall[1].k=y1;
 DGLOBwall[2].k=z1;
 DGLOBwall[3].k=x2;
 DGLOBwall[4].k=y2;
 DGLOBwall[5].k=z2;
}


void setWallMu(REALD x1,REALD y1,REALD z1,REALD x2,REALD y2,REALD z2)
{DGLOBwall[0].mu=x1;
 DGLOBwall[1].mu=y1;
 DGLOBwall[2].mu=z1;
 DGLOBwall[3].mu=x2;
 DGLOBwall[4].mu=y2;
 DGLOBwall[5].mu=z2;
}


/*wall speed is distance travelled in 1 timestep*/
void setWallSpeed(REALD x1,REALD y1,REALD z1,REALD x2,REALD y2,REALD z2)
{DGLOBwall[0].speed=x1;
 DGLOBwall[1].speed=y1;
 DGLOBwall[2].speed=z1;
 DGLOBwall[3].speed=x2;
 DGLOBwall[4].speed=y2;
 DGLOBwall[5].speed=z2;
}


/*set force to very large value to make wall always move*/
void setWallForce(REALD x1,REALD y1,REALD z1,REALD x2,REALD y2,REALD z2)
{DGLOBwall[0].spforce=x1;
 DGLOBwall[1].spforce=y1;
 DGLOBwall[2].spforce=z1;
 DGLOBwall[3].spforce=x2;
 DGLOBwall[4].spforce=y2;
 DGLOBwall[5].spforce=z2;
}


/*solve contact between particle and the 6 walls; acv[], ac - same as in addContTr()*/
void solveContWalls(particle *part)
{int i,j,found;
REALD x=0,y=0,z=0, /*point of contact*/
      ac,acv[3],
      x0,y0,z0, /*centre of particle*/
      r1,k1,frep=0,ff,
      vl1[3],vl2[3],vlr,
      vln1[3], /*normal component of speed of part. 1 in point of contact*/
      f1[3],m1[3]; /*same as in addCont()*/
wall *w;

x0=part->pos[0]; y0=part->pos[1]; z0=part->pos[2];
r1=part->radius;

for(i=0;i<=5;i++){

  w=&DGLOBwall[i]; /*same as "DGLOBwall+i"*/
  k1=w->k;
  found=0;

  switch(i){
    case 0: /*x*/
      x=x0-r1;
      if(x<(w->pos)){ found=1;
        frep=k1*((w->pos)-x);
        y=y0; z=z0;
      } break;

    case 1: /*y*/
      y=y0-r1;
      if(y<(w->pos)){ found=1;
        frep=k1*((w->pos)-y);
        x=x0; z=z0;
      } break;

    case 2: /*z*/
      z=z0-r1;
      if(z<(w->pos)){ found=1;
        frep=k1*((w->pos)-z);
        x=x0; y=y0;
      } break;

    case 3: /*-x*/
      x=x0+r1;
      if(x>(w->pos)){ found=1;
        frep=k1*(x-(w->pos));
        y=y0; z=z0;
      } break;

    case 4: /*-y*/
      y=y0+r1;
      if(y>(w->pos)){ found=1;
        frep=k1*(y-(w->pos));
        x=x0; z=z0;
      } break;

    case 5: /*-z*/
      z=z0+r1;
      if(z>(w->pos)){ found=1;
        frep=k1*(z-(w->pos));
        x=x0; y=y0;
      } break;

    default: return;
  } /*found point of contact and calculated elastic force (frep)*/

  if(found==0){continue;}

  w->force+=frep;

  acv[0]=x-x0; acv[1]=y-y0; acv[2]=z-z0;
  ac=sqrt(acv[0]*acv[0]+acv[1]*acv[1]+acv[2]*acv[2]);

  crossProd(part->rots,acv,vl1);

  for(j=0;j<=2;j++){
    vl1[j]+=part->tras[j]; /*added translation speed*/
    f1[j]=-frep*acv[j]/ac; /*elastic force*/
  }

  /*add normal friction*/
  vlr=(vl1[0]*acv[0]+vl1[1]*acv[1]+vl1[2]*acv[2])/(ac*ac);
  for(j=0;j<=2;j++){vln1[j]=vlr*acv[j];}

  vlr=normVec(vln1);
  if(vlr>FRICTIONVEL){
    ff=frep*part->mu2;
    for(j=0;j<=2;j++){
      f1[j]-=(ff*vln1[j]);
    }
  }

  /*add tangent friction and apply moment*/
  crossProd(acv,vl1,vl2);
  crossProd(vl2,acv,vl1);
  vlr=1/(ac*ac);
  vl1[0]*=vlr; vl1[1]*=vlr; vl1[2]*=vlr;

  vlr=normVec(vl1);
  if(vlr>FRICTIONVEL){
    ff=frep*w->mu;
    for(j=0;j<=2;j++){
      vl2[j]=-vl1[j];
      f1[j]+=(ff*vl2[j]);
    }
    crossProd(acv,vl2,m1);
    for(j=0;j<=2;j++){
      m1[j]*=ff;
      part->rotac[j]+=(m1[j]/part->im);
    }
  }

  /*apply force*/
  for(j=0;j<=2;j++){
    part->traac[j]+=(f1[j]/part->mass);
  }

  #if RCONTACTS==1
  f1[0]=x; f1[1]=y; f1[2]=z;
  for(j=0;j<=2;j++){
    m1[j]=acv[j]/ac; /*end of function, f1 and m1 no longer needed*/
  }
  addContact(f1,m1);
  #endif
}
}


/*wfp=1: calculate pressures on wall instead of forces*/
void solveAllContWalls(int wfp)
{int i;
REALD wa[3];

for(i=0;i<=5;i++){DGLOBwall[i].force=0;}

omp_set_num_threads(DGLOBthreads);

#pragma omp parallel for
for(i=1;i<=DGLOBnrpart;i++){
  solveContWalls(&DGLOBpart[i]);
} /*forces on walls were also calculated here*/

if(wfp==1){
  wa[0]=(DGLOBwall[4].pos-DGLOBwall[1].pos)*(DGLOBwall[5].pos-DGLOBwall[2].pos);
  wa[1]=(DGLOBwall[3].pos-DGLOBwall[0].pos)*(DGLOBwall[5].pos-DGLOBwall[2].pos);
  wa[2]=(DGLOBwall[3].pos-DGLOBwall[0].pos)*(DGLOBwall[4].pos-DGLOBwall[1].pos);
  for(i=0;i<=5;i++){(DGLOBwall[i].force)/=wa[i%3];}
  /*converted forces to pressures*/
}

/*move walls as specified*/
for(i=0;i<=2;i++){
  if(DGLOBwall[i].force<DGLOBwall[i].spforce){
    DGLOBwall[i].pos+=DGLOBwall[i].speed;
  }else{
    DGLOBwall[i].pos-=DGLOBwall[i].speed;
  }
}

for(i=3;i<=5;i++){
  if(DGLOBwall[i].force<DGLOBwall[i].spforce){
    DGLOBwall[i].pos-=DGLOBwall[i].speed;
  }else{
    DGLOBwall[i].pos+=DGLOBwall[i].speed;
  }
}

}
