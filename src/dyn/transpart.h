/*
Copyright (C) 2012-2013 Victor Matei Petrescu

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

/*function which translates a particle by x,y and z*/
void pTrans(particle *part,REALD x,REALD y,REALD z)
{part->pos[0]+=x;
 part->pos[1]+=y;
 part->pos[2]+=z;
}

/*function which rotates a particle around an axis parallel to z which passes through its centre*/
void pRotZ(particle *part,REALD tt)
{int i;
REALD xtm,sintt,costt;

sintt=sin(tt);costt=cos(tt);

  for(i=0;i<=2;i++){
    xtm=part->vx[i];
    part->vx[i]=(part->vx[i])*costt-(part->vy[i])*sintt;
    part->vy[i]=(part->vy[i])*costt+(xtm)*sintt;
  }
}

/*function which rotates a particle around an axis parallel to y which passes through its centre*/
void pRotY(particle *part,REALD tt)
{int i;
REALD xtm,sintt,costt;

sintt=sin(tt);costt=cos(tt);

  for(i=0;i<=2;i++){
    xtm=part->vx[i];
    part->vx[i]=(part->vx[i])*costt+(part->vz[i])*sintt;
    part->vz[i]=(part->vz[i])*costt-(xtm)*sintt;
  }
}


/*function which rotates a particle around an axis parallel to x which passes through its centre*/
void pRotX(particle *part,REALD tt)
{int i;
REALD ytm,sintt,costt;

sintt=sin(tt);costt=cos(tt);

  for(i=0;i<=2;i++){
    ytm=part->vy[i];
    part->vy[i]=(part->vy[i])*costt-(part->vz[i])*sintt;
    part->vz[i]=(part->vz[i])*costt+(ytm)*sintt;
  }
}


/*function which rotates a particle around an axis with direction (abx,aby,abz)
which passes through its centre*/
void pRotAB(particle *part,REALD abx,REALD aby,REALD abz,REALD tt)
{int i;
REALD xtm,
sinalf=0,cosalf=0,sinbt=0,cosbt=0,sintt,costt,
len2,len1, /*lungimea segmentului AB si a proiectiei lui in planul xOy*/
thres=1e-5; /*valoare minima admisa pentru len1*/

len1=sqrt(abx*abx+aby*aby);
len2=sqrt(abx*abx+aby*aby+abz*abz);

if(len1>thres){sinalf=aby/len1; cosalf=abx/len1;
  sinbt=len1/len2; cosbt=abz/len2;
}else{if(abz<0){tt=-tt;}}

sintt=sin(tt);costt=cos(tt);

if(len1>thres){
/*1 - rotire cu (-alfa) in jurul axei z*/
  for(i=0;i<=2;i++){
    xtm=part->vx[i];
    part->vx[i]=(part->vx[i])*cosalf+(part->vy[i])*sinalf;
    part->vy[i]=(part->vy[i])*cosalf-(xtm)*sinalf;
  }

/*2 - rotire cu (-beta) in jurul axei y*/
  for(i=0;i<=2;i++){
    xtm=part->vx[i];
    part->vx[i]=(part->vx[i])*cosbt-(part->vz[i])*sinbt;
    part->vz[i]=(part->vz[i])*cosbt+(xtm)*sinbt;
  }
}
/*3 - rotire cu teta in jurul axei z*/
  for(i=0;i<=2;i++){
    xtm=part->vx[i];
    part->vx[i]=(part->vx[i])*costt-(part->vy[i])*sintt;
    part->vy[i]=(part->vy[i])*costt+(xtm)*sintt;
  }

if(len1>thres){
/*4 - rotire cu beta in jurul axei y*/
  for(i=0;i<=2;i++){
    xtm=part->vx[i];
    part->vx[i]=(part->vx[i])*cosbt+(part->vz[i])*sinbt;
    part->vz[i]=(part->vz[i])*cosbt-(xtm)*sinbt;
  }

/*5 - rotire cu alfa in jurul axei z*/
  for(i=0;i<=2;i++){
    xtm=part->vx[i];
    part->vx[i]=(part->vx[i])*cosalf-(part->vy[i])*sinalf;
    part->vy[i]=(part->vy[i])*cosalf+(xtm)*sinalf;
  }
}
}
