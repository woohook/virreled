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


typedef struct _inlist{
int i;
struct _inlist *next;
} inlist; /*list of integers for storing numbers of particles*/


/*GLOBAL VARIABLES for hash space --------------------------------*/
int DGLOBnx,DGLOBny,DGLOBnz;
REALD DGLOBx0,DGLOBy0,DGLOBz0,DGLOBd;
inlist *DGLOBhstart;
int DGLOBthreads=1;
/*GLOBAL VARIABLES for hash space --------------------------------*/


void createHashSpace(REALD x0,REALD y0,REALD z0,int nx,int ny,int nz,REALD d)
{int i;

DGLOBx0=x0; DGLOBy0=y0; DGLOBz0=z0;
DGLOBnx=nx; DGLOBny=ny; DGLOBnz=nz;
DGLOBd=d;

if(!(DGLOBhstart=(inlist *)malloc((nx*ny*nz+1)*sizeof(inlist)))){printf("Out of memory"); exit(1);}

for(i=0;i<=nx*ny*nz;i++){DGLOBhstart[i].next=0;}
}


/*creates optimal hash table; all particles must be declared before*/
void createOptHashSpace()
{int nx,ny,nz,i;
REALD x0,y0,z0,dmax=0.0,d,
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

x0=xmin-0.5*dmax;
y0=ymin-0.5*dmax;
z0=zmin-0.5*dmax;

nx=(int)((xmax-xmin+dmax)/dmax)+1;
ny=(int)((ymax-ymin+dmax)/dmax)+1;
nz=(int)((zmax-zmin+dmax)/dmax)+1;

createHashSpace(x0,y0,z0,nx,ny,nz,dmax);

printf("Hash:x0=%1.3f,y0=%1.3f,z0=%1.3f,nx=%d,ny=%d,nz=%d,d=%1.3f\r\n",x0,y0,z0,nx,ny,nz,dmax);
}


void insert1Part(int nrp)
{int i,j,k;
particle *part;
inlist *hstart,*h;

part=&DGLOBpart[nrp];

  i=(int)(floor((part->pos[0]-DGLOBx0)/DGLOBd)+1); if(i<1){i=1;} if(i>DGLOBnx){i=DGLOBnx;}
  j=(int)(floor((part->pos[1]-DGLOBy0)/DGLOBd)+1); if(j<1){j=1;} if(j>DGLOBny){j=DGLOBny;}
  k=(int)(floor((part->pos[2]-DGLOBz0)/DGLOBd)+1); if(k<1){k=1;} if(k>DGLOBnz){k=DGLOBnz;}

  hstart=&DGLOBhstart[((i-1)*DGLOBny+j-1)*DGLOBnz+k];
  if(!(h=(inlist *)malloc(sizeof(inlist)))){printf("Out of memory"); exit(1);}

  #pragma omp critical
  {h->next=hstart->next;
  hstart->next=h;}
  h->i=nrp;
}


/*insert all particles into hash table*/
void fillHashSpace()
{int nrp;

omp_set_num_threads(DGLOBthreads);

#pragma omp parallel for
for(nrp=1;nrp<=DGLOBnrpart;nrp++){
  insert1Part(nrp);
}
}


void clear1Hash(int i)
{inlist *ctstart,*cont1,*cont2;

  ctstart=&DGLOBhstart[i];
  cont1=ctstart->next;
  while(cont1!=0){
    cont2=cont1;
    cont1=cont1->next;
    free(cont2);
  }
  ctstart->next=0;
}


void clearHashSpace()
{int i,n;

n=DGLOBnx*DGLOBny*DGLOBnz;

omp_set_num_threads(DGLOBthreads);

#pragma omp parallel for
for(i=1;i<=n;i++){
  clear1Hash(i);
}
}


void freeHashSpace()
{clearHashSpace(); free(DGLOBhstart);}


void printHashSpace()
{int i,n;
inlist *hstart,*h;

n=DGLOBnx*DGLOBny*DGLOBnz;

for(i=1;i<=n;i++){
  printf("%d: ",i);
  hstart=&DGLOBhstart[i];
  h=hstart->next;
  while(h!=0){
    printf("%d ",h->i);
    h=h->next;
  } printf("\r\n");
}
}


/*fillHashSpace() must be called before, at each time step*/
void solveHash1Part(int np)
{int i,j,k,ip,jp,kp,ipm,jpm,kpm,ip0,jp0,kp0;
particle *part;
inlist *h2start,*h2;

part=&DGLOBpart[np];

  i=(int)(floor((part->pos[0]-DGLOBx0)/DGLOBd)+1); if(i<1){i=1;} if(i>DGLOBnx){i=DGLOBnx;}
  j=(int)(floor((part->pos[1]-DGLOBy0)/DGLOBd)+1); if(j<1){j=1;} if(j>DGLOBny){j=DGLOBny;}
  k=(int)(floor((part->pos[2]-DGLOBz0)/DGLOBd)+1); if(k<1){k=1;} if(k>DGLOBnz){k=DGLOBnz;}

  if(i==DGLOBnx){ipm=0;}else{ipm=1;}
  if(j==DGLOBny){jpm=0;}else{jpm=1;}
  if(k==DGLOBnz){kpm=0;}else{kpm=1;}
  if(i==1){ip0=0;}else{ip0=-1;}
  if(j==1){jp0=0;}else{jp0=-1;}
  if(k==1){kp0=0;}else{kp0=-1;}
  for(ip=ip0;ip<=ipm;ip++){
  for(jp=jp0;jp<=jpm;jp++){
  for(kp=kp0;kp<=kpm;kp++){
    h2start=&DGLOBhstart[((i+ip-1)*DGLOBny+j+jp-1)*DGLOBnz+k+kp];
    h2=h2start->next;
    while(h2!=0){
      solveContPart(np,h2->i);
      h2=h2->next;
    }
  }}}

}


void solveHashContPart()
{int np;

omp_set_num_threads(DGLOBthreads);

#pragma omp parallel for
for(np=1;np<=DGLOBnrpart;np++){
  solveHash1Part(np);
}

}


void solveAllContTrPar()
{int i,n=0;
triangle *tr,*trstart,*trs[101];

trstart=DGLOBtrstart;
tr=trstart->next;
while(tr!=0){
  n++;
  trs[n]=tr;
  tr=tr->next;
}

omp_set_num_threads(DGLOBthreads);

#pragma omp parallel for
for(i=1;i<=n;i++){
  solveContTr(trs[i]);
}

}


void stepSim1Part(int i,REALD tstep,REALD ts2)
{int j;
REALD rs,rots[3];
particle *part;

part=&DGLOBpart[i];

if(part->rotflag==1){
  for(j=0;j<=2;j++){rots[j]=part->rots[j]+0.5*tstep*part->rotac[j];}
  rs=sqrt(rots[0]*rots[0]+rots[1]*rots[1]+rots[2]*rots[2]);
  pRotAB(part,rots[0],rots[1],rots[2],rs*tstep);
}

  for(j=0;j<=2;j++){
    part->pos[j]+=(part->tras[j]*tstep+part->traac[j]*ts2);
    part->tras[j]+=(part->traac[j]*tstep);
  }

  if(part->rotflag==1){
    for(j=0;j<=2;j++){
      part->rots[j]+=(part->rotac[j]*tstep);
    }
  }

  part->traac[0]=GRAVX; part->rotac[0]=0.0;
  part->traac[1]=GRAVY; part->rotac[1]=0.0;
  part->traac[2]=GRAVZ; part->rotac[2]=0.0;
}


void stepSimPar(REALD tstep)
{int i;
REALD ts2;

ts2=tstep*tstep/2;

omp_set_num_threads(DGLOBthreads);

#pragma omp parallel for
for(i=1;i<=DGLOBnrpart;i++){
  stepSim1Part(i,tstep,ts2);
}
}
