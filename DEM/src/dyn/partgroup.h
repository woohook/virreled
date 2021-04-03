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


typedef struct _partgroup{
int nfirst; /*first particle*/
int nlast; /*last particle*/
} partgroup;


/*---------------GLOBAL VARIABLES-----------------*/
partgroup DGLOBgroup[100]; /*DGLOBgstart[i] - first particle of group i>=1*/
int DGLOBlastg=0; /*last group created*/
/*---------------GLOBAL VARIABLES-----------------*/


/*generate random number r between min and max
with uniform distribution of r raised to the power of p*/
REALD ranpow(REALD min,REALD max)
{int a;
REALD p=-2.0;
min=pow(min,p); max=pow(max,p);
a=rand(); a=a%1000;
return pow((min+0.001*a*(max-min)),1/p);
}


int addPartGroup(REALD dmin,REALD dmax,REALD x0,REALD y0,REALD z0,REALD lx,REALD ly,REALD lz)
{int i,j,k,nx,ny,nz,n;
REALD r,x,y,z;

DGLOBlastg++;
DGLOBgroup[DGLOBlastg].nfirst=DGLOBnrpart+1;

nx=(int)(floor(0.707*lx/dmax+0.293));
ny=(int)(floor(ly/dmax));
nz=(int)(floor(lz/dmax));

for(i=0;i<=(nx-1);i++){
  for(j=0;j<=(ny-1);j++){
    for(k=0;k<=(nz-1);k++){
      r=0.5*ranpow(dmin,dmax);
      x=x0+0.5*dmax+1.414*dmax*i;
      y=y0+0.5*dmax+dmax*j;
      z=z0+0.5*dmax+dmax*k;
      n=addPart(r,1.0,1.0,1.0,1.0,1.0);
      setPartPos(n,x,y,z);
    }
  }
}

for(i=1;i<=(nx-1);i++){
  for(j=1;j<=(ny-1);j++){
    for(k=1;k<=(nz-1);k++){
      r=0.5*ranpow(dmin,dmax);
      x=x0-0.207*dmax+1.414*dmax*i;
      y=y0+dmax*j;
      z=z0+dmax*k;
      n=addPart(r,1.0,1.0,1.0,1.0,1.0);
      setPartPos(n,x,y,z);
    }
  }
}

DGLOBgroup[DGLOBlastg].nlast=DGLOBnrpart;

return DGLOBlastg;}


void setGroupParam(int ng,REALD density,REALD k,REALD mu,REALD c,REALD mu2,char rotflag,REALD chforce)
{int i;
particle *part;

for(i=DGLOBgroup[ng].nfirst;i<=DGLOBgroup[ng].nlast;i++){
  part=&DGLOBpart[i];
  part->density=density;

  part->mass=part->density*4.0*PI*(part->radius*part->radius*part->radius)/3.0;
  part->im=part->density*8.0*PI*(part->radius*part->radius*part->radius*part->radius*part->radius)/15.0;

  part->k=k;
  part->mu=mu;
  part->c=c;
  part->mu2=mu2;

  part->rotflag=rotflag;
  part->chforce=chforce;
}
}
