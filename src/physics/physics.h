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

int object_turning = 0;
void physics_object_preprocess(vhc* human)
{
  object_turning = (object_turning - human->cmd_turn) % 256;
  forward(human->bid[1], 6.0f*human->cmd_accelerate, object_turning, human->cmd_handbrake);
}

void physics_vehicle_preprocess(vhc* car)
{
  if(car->nob == 1)
  {
    physics_object_preprocess(car);
    return;
  }

  for(int i=1;i<=(car->nj);i++){
    if(((car->jfc[i])==4)||((car->jfc[i])==5)){
      rotjoint(car->jid[i],car->jax[i],-1.4*tan(1.7*car->vrx));
    }
  } /*rotate joints to steer instead of applying moment*/
}

void physics_vehicle_process(vhc* car)
{
  int i, j;
  REALN pin=0,bkf=0,hbkf=0,
        mass,drc,
        *vel,vloc[3],aloc[3];
  particle *part;

  if(car->nob == 1)
  {
    return;
  }

pin=((float)car->cmd_accelerate)*car->accel;

bkf=((float)car->cmd_brake)*car->brake;
hbkf=2.0*((float)car->cmd_handbrake)*car->brake;

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
}

void physics_vehicle_postprocess(vhc* car)
{
  if(car->nob == 1)
  {
    return;
  }

  for(int i=1;i<=(car->nj);i++){
    if(((car->jfc[i])==4)||((car->jfc[i])==5)){
      rotjoint(car->jid[i],car->jax[i],1.4*tan(1.7*car->vrx));
    }
  }
}

/*run 1 simulation step; tstep - time step
neartr[][] - near triangles to check for contacts; nnt - number of near triangles*/
void runsim(REALN tstep,triangle **neartr,int nnt)
{
  int i,j;

for(vhc* car = &g_vehicles[0]; car < &g_vehicles[g_vehicles_count]; ++car)
{
  physics_vehicle_process(car);
}

for(i=1;i<=nnt;i++){solveContTr(neartr[i]);}

/*solveAllContPart();*/
for(vhc* car = &g_vehicles[0]; car < &g_vehicles[g_vehicles_count]; ++car)
{
  for(vhc* car2 = &g_vehicles[1]; car2 < &g_vehicles[g_vehicles_count]; ++car2)
  {
    if(car == car2)
    {
      continue;
    }

    for(i = 1; i <= car->nob; ++i)
    {
      for(j = 1; j <= car2->nob; ++j)
      {
        solveContPart(car->bid[i], car2->bid[j]);
      }
    }
  }
}

solveAllJoint();

stepSim(tstep);

#if RCONTACTS==1
rmContacts();
#endif
}


/*run nsteps simulation steps*/
void runsteps(sgob *objs,REALN tstep,int nsteps)
{int i,j,nnt=0;
triangle *neartr[MAXCTR],*trstart,*tr;
REALN *pos,d,dmin,dx,dy,dz;
particle *part;

trstart=DGLOBtrstart;

tr=trstart->next;
while(tr!=0){
 for(int k=0; k<g_vehicles_count; ++k)
 {
  vhc* car = &g_vehicles[k];
  pos=DGLOBpart[car->bid[1]].pos;

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
    break;
  }
 }
 tr=tr->next;
}

for(int k=0; k<g_vehicles_count; ++k)
{
  vhc* car = &g_vehicles[k];
  physics_vehicle_preprocess(car);
}

for(i=1;i<=nsteps;i++){
  runsim(tstep,neartr,nnt);
}

for(int k=0; k<g_vehicles_count; ++k)
{
  vhc* car = &g_vehicles[k];
  physics_vehicle_postprocess(car);
}

for(int k=0; k<g_vehicles_count; ++k)
{
  vhc* car = &g_vehicles[k];
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
