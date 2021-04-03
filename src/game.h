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


/*function which rotates a joint around an axis parallel to x
which passes through jax[3] in local system of particle 1 (pos11 modified)*/
void rotjoint(joint *jt,REALD *jax,REALD tt)
{REALD ytm,sintt,costt;

sintt=sin(tt);costt=cos(tt);

    ytm=jt->pos11[1];
    jt->pos11[1]=jax[1]+(jt->pos11[1]-jax[1])*costt-(jt->pos11[2]-jax[2])*sintt;
    jt->pos11[2]=jax[2]+(jt->pos11[2]-jax[2])*costt+(ytm-jax[1])*sintt;
}

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


/*run 1 simulation step; tstep - time step; af, bf - acceleration and brake factors
neartr[][] - near triangles to check for contacts; nnt - number of near triangles*/
void runsim(sgob *objs,int nob,vhc *car,REALN tstep,REALN af,REALN bf,REALN hbf,FILE *repf,REALN *timp,triangle **neartr,int nnt)
{int i,j,nobtr; /*nobtr-number of objects in the track*/

REALN pin=0,bkf=0,hbkf=0,
      mass,drc,
      *vel,vloc[3],aloc[3];
static int rsem=0; /*when rsem==REPSTEPS, save replay data*/
particle *part;

nobtr=nob-car->nob;

pin=af*car->accel;

bkf=bf*car->brake;
hbkf=2.0*hbf*car->brake;

/*drag*/
for(i=1;i<=car->nob;i++){
  vel=DGLOBpart[car->bid[i]].tras;
  mass=DGLOBpart[car->bid[i]].mass;
  if(i==1){drc=1.0;}else{drc=0.15;}
  for(j=0;j<=2;j++){
    (DGLOBpart[car->bid[i]].traac[j])-=(drc*car->drag*vel[j]*ABS(vel[j])/mass);
  }
}
/*drag^*/

/*downforce*/
part=&DGLOBpart[car->bid[1]];
vel=part->tras; /*speed in global coordinates*/
mass=part->mass;
rtglob2loc(vel,vloc,part);
aloc[0]=-(car->downforce*vloc[2]*vloc[2]/mass); aloc[1]=aloc[2]=0.0;
rtloc2glob(aloc,vloc,part); /*vloc now acceleration in global coordinates*/
for(j=0;j<=2;j++){
  part->traac[j]+=vloc[j];
}
/*downforce^*/

for(i=1;i<=(car->nj);i++){
  if(((car->jfc[i])==3)||((car->jfc[i])==5)){
    addMomJoint((car->jid[i]),2,0.0,0.5*pin,0.0);
  }
  if(((car->jfc[i])==4)||((car->jfc[i])==5)){
    addFrictionJoint((car->jid[i]),0.03*(car->brake)+1.0*bkf);
  }else{
    addFrictionJoint((car->jid[i]),0.03*(car->brake)+0.5*bkf);
  }
  if(((car->jfc[i])!=4)&&((car->jfc[i])!=5)){
    addFrictionJoint((car->jid[i]),0.5*hbkf); /*handbrake*/
  }
}

for(i=1;i<=nnt;i++){solveContTr(neartr[i]);}

/*solveAllContPart();*/

solveAllJoint();

stepSim(tstep);

#if RCONTACTS==1
rmContacts();
#endif

*timp+=tstep;


#if REPLAY==1
rsem++;
if(rsem>=REPSTEPS){
  for(i=1;i<=car->nob;i++){
    j=car->oid[i];
    part=&DGLOBpart[car->bid[i]];

    objs[j].vx[0]=objs[j].xcen=part->pos[0];
    objs[j].vy[0]=objs[j].ycen=part->pos[1];
    objs[j].vz[0]=objs[j].zcen=part->pos[2];

    objs[j].vx[1]=objs[j].vx[0]+part->vx[0];
    objs[j].vy[1]=objs[j].vy[0]+part->vy[0];
    objs[j].vz[1]=objs[j].vz[0]+part->vz[0];

    objs[j].vx[2]=objs[j].vx[0]+part->vx[1];
    objs[j].vy[2]=objs[j].vy[0]+part->vy[1];
    objs[j].vz[2]=objs[j].vz[0]+part->vz[1];

    objs[j].vx[3]=objs[j].vx[0]+part->vx[2];
    objs[j].vy[3]=objs[j].vy[0]+part->vy[2];
    objs[j].vz[3]=objs[j].vz[0]+part->vz[2];
  } /*calculated object positions for replay*/
  fprintf(repf,"%1.3f ",*timp);
  for(i=1;i<=car->nob;i++){
    j=car->oid[i];
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j].vx[0],objs[j].vy[0],objs[j].vz[0]);
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j].vx[1],objs[j].vy[1],objs[j].vz[1]);
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j].vx[2],objs[j].vy[2],objs[j].vz[2]);
    fprintf(repf,"%1.3f %1.3f %1.3f ",objs[j].vx[3],objs[j].vy[3],objs[j].vz[3]);
  }
  fprintf(repf,"\r\n");
  rsem=0;
}
#endif

}


/*run nsteps simulation steps*/
void runsteps(sgob *objs,int nob,vhc *car,REALN tstep,int nsteps,REALN vrx,REALN af,REALN bf,REALN hbf,FILE *repf,REALN *timp)
{int i,j,nnt=0;
triangle *neartr[MAXCTR],*trstart,*tr;
REALN *pos,d,dmin,dx,dy,dz;
particle *part;

trstart=DGLOBtrstart;
pos=DGLOBpart[car->bid[1]].pos;

tr=trstart->next;
while(tr!=0){
  dx=(pos[0]-(tr->p1[0])); dy=(pos[1]-(tr->p1[1])); dz=(pos[2]-(tr->p1[2]));
  dmin=dx*dx+dy*dy+dz*dz;
  dx=(pos[0]-(tr->p2[0])); dy=(pos[1]-(tr->p2[1])); dz=(pos[2]-(tr->p2[2]));
  d=dx*dx+dy*dy+dz*dz; if(d<dmin){dmin=d;}
  dx=(pos[0]-(tr->p3[0])); dy=(pos[1]-(tr->p3[1])); dz=(pos[2]-(tr->p3[2]));
  d=dx*dx+dy*dy+dz*dz; if(d<dmin){dmin=d;}
  dmin=sqrt(dmin);
  if(dmin<(5.0+0.5*(tr->length))){ /*if closer than ... to nearest vertex, add triangle to checklist*/
    nnt++;
    neartr[nnt]=tr;
  }
  tr=tr->next;
}

for(i=1;i<=(car->nj);i++){
  if(((car->jfc[i])==4)||((car->jfc[i])==5)){
    rotjoint(car->jid[i],car->jax[i],-1.4*tan(1.7*vrx));
  }
} /*rotate joints to steer instead of applying moment*/

for(i=1;i<=nsteps;i++){
  runsim(objs,nob,car,tstep,af,bf,hbf,repf,timp,neartr,nnt);
}

for(i=1;i<=(car->nj);i++){
  if(((car->jfc[i])==4)||((car->jfc[i])==5)){
    rotjoint(car->jid[i],car->jax[i],1.4*tan(1.7*vrx));
  }
}

for(i=1;i<=car->nob;i++){
  j=car->oid[i];
  part=&DGLOBpart[car->bid[i]];

  objs[j].vx[0]=objs[j].xcen=part->pos[0];
  objs[j].vy[0]=objs[j].ycen=part->pos[1];
  objs[j].vz[0]=objs[j].zcen=part->pos[2];

  objs[j].vx[1]=objs[j].vx[0]+part->vx[0];
  objs[j].vy[1]=objs[j].vy[0]+part->vy[0];
  objs[j].vz[1]=objs[j].vz[0]+part->vz[0];

  objs[j].vx[2]=objs[j].vx[0]+part->vx[1];
  objs[j].vy[2]=objs[j].vy[0]+part->vy[1];
  objs[j].vz[2]=objs[j].vz[0]+part->vz[1];

  objs[j].vx[3]=objs[j].vx[0]+part->vx[2];
  objs[j].vy[3]=objs[j].vy[0]+part->vy[2];
  objs[j].vz[3]=objs[j].vz[0]+part->vz[2];
} /*calculated object positions for rendering*/

}


/*function which determines speed of vehicle*/
void rdspeed(vhc *car,REALN *speed,REALN *rotspeed,REALN *dspeed)
{int i,n=0;
REALN *spe,*rot;
static REALN spe0[3];
REALN dspe[3];

spe=DGLOBpart[car->bid[1]].tras;

dspe[0]=spe[0]-spe0[0];
dspe[1]=spe[1]-spe0[1];
dspe[2]=spe[2]-spe0[2];

spe0[0]=spe[0]; spe0[1]=spe[1]; spe0[2]=spe[2];

*speed=sqrt(spe[0]*spe[0]+spe[1]*spe[1]+spe[2]*spe[2]);

*dspeed=sqrt(dspe[0]*dspe[0]+dspe[1]*dspe[1]+dspe[2]*dspe[2]);

*rotspeed=0;

for(i=1;i<=car->nob;i++){
  if((car->ofc[i]==3)||(car->ofc[i]==5)){ n++;
    rot=DGLOBpart[car->bid[i]].rots;
    (*rotspeed)+=sqrt(rot[0]*rot[0]+rot[1]*rot[1]+rot[2]*rot[2]);
  }
}
(*rotspeed)/=n; /*average rot. speed of motor wheels*/

}


#if SOUND==1

REALN sintr(REALN x)
{const REALN pi=3.1415926536;
REALN y;
x=(x/(2*pi)-floor(x/(2*pi)))*2*pi;
if(x<=pi/2){y=x/(pi/2);return y;}
if((x>pi/2)&&(x<=3*pi/2)){y=1-(x-pi/2)/(pi/2);return y;}
if(x>3*pi/2){y=-1+(x-3*pi/2)/(pi/2);return y;}
return 0;}


REALN sinst(REALN x)
{const REALN pi=3.1415926536;
REALN y;
x=(x/(2*pi)-floor(x/(2*pi)))*2*pi;
if(x<=pi){y=x/(pi/2);return y;}
if(x>pi){y=-1+(x-3*pi/2)/(pi/2);return y;}
return 0;}


REALN sinrd()
{REALN x;
x=rand();
x=(REALN)(((int)x)%100)/50-1;
return x;
}


/*audio callback function for SDL*/
void my_audio_callback(void *userdata, Uint8 *stream, int len)
{static REALD cr=0;
static int sem=0;
int freqtr,i,j1,j2,n=40;
REALN *volum,a,c,tmp;
Uint16 format;
Uint8 channels;
REALN coef;

static REALN freq[41]; /*frequencies of all sounds*/
REALN freqdo=0.3*261.6255653; /*first frequency*/



  if(sem==0){
    sem=1;
    tmp=1.06;
    freq[1]=freqdo;
    for(i=2;i<=40;i++){
      freq[i]=freq[i-1]*tmp;
    }
  } /*set frequencies*/

volum=(REALN *)userdata;

format=(Uint16)volum[2];
channels=(Uint8)volum[3];
freqtr=(int)volum[4];

switch(format){
	case AUDIO_U8: coef=1; break;
	case AUDIO_U16: coef=0.5; break;
	case AUDIO_S8: coef=1; break;
	case AUDIO_S16: coef=0.5; break;
	default: coef=0.5; break;
}
switch(channels){
	case 1: break;
	case 2: coef*=0.5;break;
	default: printf("Unsupported number of channels\r\n");exit(1);
}
coef=coef/freqtr;

/*SDL_Delay(10);*/

a=volum[1]/200; j1=(int)floor(a*(n-2)+1); j2=(int)floor(a*(n-2)+2);
a=a*(n-2)+1-j1;

  for(i=0;i<=len-1;i++){
    stream[i]=63;
    cr++;if(cr>1e6){cr=0;}
      c=0.85+0.15*sin(0.005*cr);
      stream[i]+=(Uint8)(10.0*c*(1.0-a)*sinst((freq[j1])*6.283185307*cr*coef));
      stream[i]+=(Uint8)(10.0*c*a*sinst((freq[j2])*6.283185307*cr*coef));
      if(volum[5]>20){
        stream[i]+=(Uint8)(40.0*c*a*sinrd());
      }

  }

}

#endif
