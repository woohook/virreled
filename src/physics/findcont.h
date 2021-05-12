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

/*adds moment in joint; moment is in local system of particle np1 or np2 (pls=1 or 2)*/
void addMomJoint(joint *jt,int pls,REALD x,REALD y,REALD z)
{int i,np1,np2,np;
REALD ml[3],mg[3]; /*moment in local and global system*/
particle *part;

part=DGLOBpart;

np1=jt->np[0]; np2=jt->np[1];
switch(pls){
  case 1: np=np1; break;
  case 2: np=np2; break;
  default: np=np1; break;
}

ml[0]=x; ml[1]=y; ml[2]=z;
rtloc2glob(ml,mg,&part[np]);

for(i=0;i<=2;i++){ /*apply moments to particles*/
  part[np1].rotac[i]+=(mg[i]/part[np1].im);
  part[np2].rotac[i]-=(mg[i]/part[np2].im);
}

}


/*add friction moment in joint*/
void addFrictionJoint(joint *jt,REALD frmom)
{int i,np1,np2;
REALD rots[3],m[3],len; /*relative rot. speed, moment*/
particle *part;

part=DGLOBpart;

np1=jt->np[0]; np2=jt->np[1];

for(i=0;i<=2;i++){rots[i]=part[np2].rots[i]-part[np1].rots[i];}
len=normVec(rots);

if(len>FRICTIONVEL){
  if(len<ROTFVEL){frmom*=(len/ROTFVEL);}
  for(i=0;i<=2;i++){ /*apply moments to particles*/
    m[i]=frmom*rots[i];
    part[np1].rotac[i]+=(m[i]/part[np1].im);
    part[np2].rotac[i]-=(m[i]/part[np2].im);
  }
}

}


/*calculates forces in joint*/
void solveJoint(joint *jt,particle *part)
{int i,np1,np2;
REALD pos21[3],pos1g[3],pos2g[3],
      f11[3],f1g[3],f2g[3],
      m1[3],m2[3],
      vl1[3],vl2[3];

np1=jt->np[0]; np2=jt->np[1];

/*elastic forces*/
loc2glob(jt->pos22,pos2g,&part[np2]);
glob2loc(pos2g,pos21,&part[np1]);
for(i=0;i<=2;i++){f11[i]=(jt->kt[i])*(pos21[i]-(jt->pos11[i]));}
rtloc2glob(f11,f1g,&part[np1]);
rtloc2glob(jt->pos11,pos1g,&part[np1]);
for(i=0;i<=2;i++){pos2g[i]-=(part[np2].pos[i]); f2g[i]=-f1g[i];}
crossProd(pos1g,f1g,m1);
crossProd(pos2g,f2g,m2);

for(i=0;i<=2;i++){ /*apply moments and forces*/
  part[np1].rotac[i]+=(m1[i]/part[np1].im);
  part[np2].rotac[i]+=(m2[i]/part[np2].im);
  part[np1].traac[i]+=(f1g[i]/part[np1].mass);
  part[np2].traac[i]+=(f2g[i]/part[np2].mass);
}

/*damping forces*/
crossProd(part[np1].rots,pos1g,vl1);
crossProd(part[np2].rots,pos2g,vl2);
for(i=0;i<=2;i++){
  vl1[i]+=part[np1].tras[i];
  vl2[i]+=part[np2].tras[i]; /*added translation speed*/
  vl2[i]-=vl1[i];
}
rtglob2loc(vl2,vl1,&part[np1]);
for(i=0;i<=2;i++){f11[i]=(jt->ct[i])*vl1[i];}
rtloc2glob(f11,f1g,&part[np1]);
for(i=0;i<=2;i++){f2g[i]=-f1g[i];}
crossProd(pos1g,f1g,m1);
crossProd(pos2g,f2g,m2);

for(i=0;i<=2;i++){ /*apply moments and forces*/
  part[np1].rotac[i]+=(m1[i]/part[np1].im);
  part[np2].rotac[i]+=(m2[i]/part[np2].im);
  part[np1].traac[i]+=(f1g[i]/part[np1].mass);
  part[np2].traac[i]+=(f2g[i]/part[np2].mass);
}
}


void solveAllJoint()
{
joint *jtstart,*jt;
particle *part;

part=DGLOBpart;
jtstart=DGLOBjtstart;

jt=jtstart->next;
while(jt!=0){solveJoint(jt,part); jt=jt->next;}
}


/*solve contact between particles np1 and np2, if they are close enough*/
void solveContPart(int np1,int np2)
{int i;
REALD a,b,c,d,r1,r2,ac,bc,acd,
      acv[3],bcv[3], /*components of vectors ac and bc*/
      k1,k2,frep,ff, /*elastic coefficients, elastic force, friction*/
      vl1[3],vl2[3],vlr, /*linear velocities of particles in point of contact*/
      vln1[3], /*normal component of speed of part. 1 relative to part. 2 in point of contact*/
      f1[3],f2[3], /*forces added to particles 1 and 2*/
      m1[3],m2[3]; /*moments added to particles 1 and 2*/
particle *part;

if(np1>=np2){return;} /*to avoid adding the same contact twice*/

part=DGLOBpart;

a=part[np2].pos[0]-part[np1].pos[0]; /*x2-x1*/
b=part[np2].pos[1]-part[np1].pos[1]; /*y2-y1*/
c=part[np2].pos[2]-part[np1].pos[2]; /*z2-z1*/
d=sqrt(a*a+b*b+c*c);
r1=part[np1].radius;
r2=part[np2].radius;

if(d<=(r1+r2)){
  k1=part[np1].k; k2=part[np2].k;
  ac=(k1*r1+k2*(d-r2))/(k1+k2);
  bc=d-ac;

  acd=-bc/d;
  bcv[0]=a*acd; bcv[1]=b*acd; bcv[2]=c*acd;
  acd=ac/d;
  acv[0]=a*acd; acv[1]=b*acd; acv[2]=c*acd;

  frep=k1*(r1-ac);

  crossProd(part[np1].rots,acv,vl1);
  crossProd(part[np2].rots,bcv,vl2);

  for(i=0;i<=2;i++){
    vl1[i]+=part[np1].tras[i];
    vl2[i]+=part[np2].tras[i]; /*added translation speed*/
    vl1[i]=vl1[i]-vl2[i];
    f2[i]=frep*acv[i]/ac;
    f1[i]=-f2[i]; /*f1, f2 - elastic forces*/
  }

  /*add normal friction*/
  vlr=(vl1[0]*acv[0]+vl1[1]*acv[1]+vl1[2]*acv[2])/(ac*ac);
  for(i=0;i<=2;i++){vln1[i]=vlr*acv[i];}

  vlr=normVec(vln1);
  if(vlr>FRICTIONVEL){
    ff=0.5*frep*(part[np1].mu2+part[np2].mu2);
    /*if(vlr<TRANSFVEL){ff*=(vlr/TRANSFVEL);}*/
    for(i=0;i<=2;i++){
      f1[i]-=(ff*vln1[i]);
      f2[i]+=(ff*vln1[i]);
    }
  }

  /*add tangent friction and apply moments*/
  for(i=0;i<=2;i++){
    vln1[i]*=vlr;
    vl1[i]-=vln1[i];
  }

  vlr=normVec(vl1);
  if(vlr>FRICTIONVEL){
    ff=0.5*frep*(part[np1].mu+part[np2].mu);
    if(vlr<TRANSFVEL){ff*=(vlr/TRANSFVEL);}
    for(i=0;i<=2;i++){
      vl2[i]=-vl1[i];
      f1[i]+=(ff*vl2[i]);
      f2[i]+=(ff*vl1[i]);
    }
    crossProd(acv,vl2,m1);
    crossProd(bcv,vl1,m2);
    for(i=0;i<=2;i++){
      m1[i]*=ff; m2[i]*=ff;
      part[np1].rotac[i]+=(m1[i]/part[np1].im);
      part[np2].rotac[i]+=(m2[i]/part[np2].im);
    }
  }

  /*apply forces*/
  for(i=0;i<=2;i++){
    part[np1].traac[i]+=(f1[i]/part[np1].mass);
    part[np2].traac[i]+=(f2[i]/part[np2].mass);
  }

  #if RCONTACTS==1
  for(i=0;i<=2;i++){
    f1[i]=part[np1].pos[i]+acv[i];
    m1[i]=acv[i]/ac; /*end of function, f1 and m1 no longer needed*/
  }
  addContact(f1,m1);
  #endif

}

}


void solveAllContPart()
{int i,j,nrpart;

nrpart=DGLOBnrpart;

for(i=1;i<=nrpart;i++){
  for(j=i+1;j<=nrpart;j++){
    solveContPart(i,j);
  }
}

}


/*solve contact with fixed point
acv[], ac - same as in addContTr()*/
void solveForceCt(particle *part,REALD *acv,REALD ac)
{int j;
REALD r1,k1,frep,ff,
      vl1[3],vl2[3],vlr,
      vln1[3], /*normal component of speed of part. 1 in point of contact*/
      f1[3],m1[3]; /*same as in addCont()*/

r1=part->radius;

k1=part->k;
frep=k1*(r1-ac);

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
  /*if(vlr<TRANSFVEL){ff*=(vlr/TRANSFVEL);}*/
  for(j=0;j<=2;j++){
    f1[j]-=(ff*vln1[j]);
  }
}

/*add tangent friction and apply moment*/
for(j=0;j<=2;j++){
  vln1[j]*=vlr;
  vl1[j]-=vln1[j];
}

vlr=normVec(vl1);
if(vlr>FRICTIONVEL){
  ff=frep*part->mu;
  if(vlr<TRANSFVEL){ff*=(vlr/TRANSFVEL);}
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


/*solve contacts between line p1[3]-p2[3] and 1 particle*/
void solveContLn(particle *part,REALD *p1,REALD *p2)
{REALD a,b,c,d,
       a2,b2,c2,abc2,
       x0,y0,z0, /*centre of particle*/
       x,y,z, /*contact point*/
       r1,ac,acv[3];

a=p2[0]-p1[0]; b=p2[1]-p1[1]; c=p2[2]-p1[2];
a2=a*a; b2=b*b; c2=c*c; abc2=a2+b2+c2;

  x0=part->pos[0]; y0=part->pos[1]; z0=part->pos[2];
  d=a*x0+b*y0+c*z0;
  x=((b2+c2)*p1[0]-a*(b*p1[1]+c*p1[2])+a*d)/abc2;
  y=((a2+c2)*p1[1]-b*(a*p1[0]+c*p1[2])+b*d)/abc2;
  z=((a2+b2)*p1[2]-c*(a*p1[0]+b*p1[1])+c*d)/abc2;

  acv[0]=x-x0; acv[1]=y-y0; acv[2]=z-z0;
  ac=sqrt(acv[0]*acv[0]+acv[1]*acv[1]+acv[2]*acv[2]);
  r1=part->radius;

  if(ac<=r1){
  if(((x>p1[0])&&(x<p2[0]))||((y>p1[1])&&(y<p2[1]))||((z>p1[2])&&(z<p2[2]))){
    solveForceCt(part,acv,ac);
  }}

}


/*solve contacts between triangle *tr and all particles*/
void solveContTr(triangle *tr)
{int i;
REALD a,b,c,d,
      a2,b2,c2,abc2,
      x0,y0,z0, /*centre of particle*/
      x,y,z, /*contact point*/
      r1,ac,acv[3];
particle *part;
int nrpart;

part=DGLOBpart;
nrpart=DGLOBnrpart;

trPlan(tr,&a,&b,&c,&d);
a2=a*a; b2=b*b; c2=c*c; abc2=a2+b2+c2;

for(i=1;i<=nrpart;i++){
  x0=part[i].pos[0]; y0=part[i].pos[1]; z0=part[i].pos[2];
  x=((b2+c2)*x0-a*(b*y0+c*z0)+a*d)/abc2;
  y=((a2+c2)*y0-b*(a*x0+c*z0)+b*d)/abc2;
  z=((a2+b2)*z0-c*(a*x0+b*y0)+c*d)/abc2;

  acv[0]=x-x0; acv[1]=y-y0; acv[2]=z-z0;
  ac=sqrt(acv[0]*acv[0]+acv[1]*acv[1]+acv[2]*acv[2]);
  r1=part[i].radius;

  if(ac<=r1){ /*if distance is lower than particle radius*/
  if(checkTr(tr,a,b,c,x,y,z)){ /*if point of contact is inside triangle*/
    solveForceCt(&part[i],acv,ac);
  }else{
    solveContLn(&part[i],tr->p1,tr->p2);
    solveContLn(&part[i],tr->p2,tr->p3);
    solveContLn(&part[i],tr->p3,tr->p1);
  }}
}

}


void solveAllContTr()
{triangle *tr,*trstart;

trstart=DGLOBtrstart;
tr=trstart->next;
while(tr!=0){
  solveContTr(tr);
  tr=tr->next;
}

}


/*run a simulation step;tstep - time step;constant accelerations assumed*/
void stepSim(REALD tstep)
{int i,j,nrpart;
REALD ts2,rs,rots[3];
particle *part;

nrpart=DGLOBnrpart;
part=DGLOBpart;

ts2=tstep*tstep/2;

for(i=1;i<=nrpart;i++){

  for(j=0;j<=2;j++){rots[j]=part[i].rots[j]+0.5*tstep*part[i].rotac[j];}
  rs=sqrt(rots[0]*rots[0]+rots[1]*rots[1]+rots[2]*rots[2]);
  pRotAB(&part[i],rots[0],rots[1],rots[2],rs*tstep);

  for(j=0;j<=2;j++){
    part[i].pos[j]+=(part[i].tras[j]*tstep+part[i].traac[j]*ts2);
    part[i].tras[j]+=(part[i].traac[j]*tstep);
    part[i].rots[j]+=(part[i].rotac[j]*tstep);
  }

  part[i].traac[0]=GRAVX; part[i].rotac[0]=0.0;
  part[i].traac[1]=GRAVY; part[i].rotac[1]=0.0;
  part[i].traac[2]=GRAVZ; part[i].rotac[2]=0.0;
}
}
