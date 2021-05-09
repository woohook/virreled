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

typedef struct _particle{
REALD radius;
REALD density;
REALD mass;
REALD im; /*inertial moment*/
REALD k; /*elastic coef. (N/m)*/
REALD mu; /*tangent friction coef.*/
REALD mu2; /*normal friction coef.*/
REALD pos[3]; /*position: pos[0]=x; pos[1]=y; pos[2]=z*/
REALD tras[3]; /*speed of translation*/
REALD rots[3]; /*speed of rotation*/
REALD traac[3];
REALD rotac[3]; /*accelerations*/

REALD vx[3];
REALD vy[3];
REALD vz[3]; /*directions of local axes in global coordinates - v[0]=i, v[1]=j, v[2]=k*/

char rotflag; /*1-rotational; 0-not*/
REALD chforce; /*cohesion force*/
} particle;


typedef struct _triangle{
REALD p1[3]; /*x,y,z of point p1*/
REALD p2[3];
REALD p3[3];
REALD length; /*length of longest side*/
struct _triangle *next;
} triangle; /*perfectly stiff triangle*/


#if RCONTACTS==1

typedef struct _contact{
REALD pos[3]; /*position: pos[0]=x; pos[1]=y; pos[2]=z*/
REALD dir[3]; /*direction of contact*/
struct _contact *next;
} contact;

#endif


typedef struct _joint{
int np[2]; /*numbers of the 2 particles*/
REALD pos11[3]; /*posij[3] - position of joint attached to particle i*/
REALD pos22[3]; /*calculated in the system of particle j*/
REALD kt[3]; /*elastic coef.*/
REALD ct[3]; /*damping*/
struct _joint *next;
} joint;


/*cross product between a[3] and b[3]*/
void crossProd(REALD *a,REALD *b,REALD *c)
{c[0]=a[1]*b[2]-a[2]*b[1];
 c[1]=a[2]*b[0]-a[0]*b[2];
 c[2]=a[0]*b[1]-a[1]*b[0];
}


/*set length of vector[3] to 1.0 and return original length*/
REALD normVec(REALD *vec)
{REALD len;
len=sqrt(vec[0]*vec[0]+vec[1]*vec[1]+vec[2]*vec[2]);
vec[0]/=len;
vec[1]/=len;
vec[2]/=len;
return len;}


void trPlan(triangle *tr,REALD *a,REALD *b,REALD *c,REALD *d)
{REALD a1,b1,c1,a2,b2,c2;

  a1=(tr->p1[0])-(tr->p2[0]); b1=(tr->p1[1])-(tr->p2[1]); c1=(tr->p1[2])-(tr->p2[2]);
  a2=(tr->p1[0])-(tr->p3[0]); b2=(tr->p1[1])-(tr->p3[1]); c2=(tr->p1[2])-(tr->p3[2]);

  *a=b1*c2-b2*c1; *b=a2*c1-a1*c2; *c=a1*b2-a2*b1;
  *d=(*a)*(tr->p1[0])+(*b)*(tr->p1[1])+(*c)*(tr->p1[2]); /*ax+by+cz=d*/
}


/*check whether C(xc,yc,zc) is inside triangle tr; a,b,c-from trPlan()*/
int checkTr(triangle *tr,REALD a,REALD b,REALD c,REALD xc,REALD yc,REALD zc)
{REALD x1,y1,x2,y2,x3,y3,x,y,
       l1,l2,m1,m2,p; /*p - cross product*/
char s1,s2,s3; /*signs of the 3 cross products*/
int cr1,cr2;

a=ABS(a); b=ABS(b); c=ABS(c);

if((a>=b)&&(a>=c)){
  cr1=1; cr2=2; x=yc; y=zc;
}else{if(b>=c){
  cr1=0; cr2=2; x=xc; y=zc;
}else{
  cr1=0; cr2=1; x=xc; y=yc;
}}

x1=tr->p1[cr1]; y1=tr->p1[cr2];
x2=tr->p2[cr1]; y2=tr->p2[cr2];
x3=tr->p3[cr1]; y3=tr->p3[cr2];

  l1=x2-x1;l2=x-x1;
  m1=y2-y1;m2=y-y1;
  p=l1*m2-l2*m1; if(p>0){s1=1;}else{s1=2;}

  l1=x3-x2;l2=x-x2;
  m1=y3-y2;m2=y-y2;
  p=l1*m2-l2*m1; if(p>0){s2=1;}else{s2=2;}

  l1=x1-x3;l2=x-x3;
  m1=y1-y3;m2=y-y3;
  p=l1*m2-l2*m1; if(p>0){s3=1;}else{s3=2;}

  if((s1==s2)&&(s1==s3)){return 1;}

return 0;}


/*transform position from local system of particle to global system*/
void loc2glob(REALD *posl,REALD *posg,particle *part)
{posg[0]=(part->pos[0]) + posl[0]*part->vx[0] + posl[1]*part->vx[1] + posl[2]*part->vx[2];
 posg[1]=(part->pos[1]) + posl[0]*part->vy[0] + posl[1]*part->vy[1] + posl[2]*part->vy[2];
 posg[2]=(part->pos[2]) + posl[0]*part->vz[0] + posl[1]*part->vz[1] + posl[2]*part->vz[2];
}


/*transform speed or force from local system to global system*/
void rtloc2glob(REALD *posl,REALD *posg,particle *part)
{posg[0]=posl[0]*part->vx[0] + posl[1]*part->vx[1] + posl[2]*part->vx[2];
 posg[1]=posl[0]*part->vy[0] + posl[1]*part->vy[1] + posl[2]*part->vy[2];
 posg[2]=posl[0]*part->vz[0] + posl[1]*part->vz[1] + posl[2]*part->vz[2];
}


/*transform position from global system to local system*/
void glob2loc(REALD *posg,REALD *posl,particle *part)
{REALD x,y,z;
x=posg[0]-part->pos[0];
y=posg[1]-part->pos[1];
z=posg[2]-part->pos[2];
  posl[0]=x*part->vx[0] + y*part->vy[0] + z*part->vz[0];
  posl[1]=x*part->vx[1] + y*part->vy[1] + z*part->vz[1];
  posl[2]=x*part->vx[2] + y*part->vy[2] + z*part->vz[2];
}


/*transform speed or force from global system to local system*/
void rtglob2loc(REALD *posg,REALD *posl,particle *part)
{posl[0]=posg[0]*part->vx[0] + posg[1]*part->vy[0] + posg[2]*part->vz[0];
 posl[1]=posg[0]*part->vx[1] + posg[1]*part->vy[1] + posg[2]*part->vz[1];
 posl[2]=posg[0]*part->vx[2] + posg[1]*part->vy[2] + posg[2]*part->vz[2];
}


/*GLOBAL VARIABLES---GLOBAL VARIABLES---GLOBAL VARIABLES---GLOBAL VARIABLES---GLOBAL VARIABLES---------*/
particle *DGLOBpart;
int DGLOBnrpartmem; /*nr. of particles for which memory was allocated*/
int DGLOBnrpart; /*nr. of particles*/

REALD DGLOBchdist; /*cohesion distance for all particles*/

triangle *DGLOBtrstart;

#if RCONTACTS==1
contact *DGLOBctstart;
#endif

joint *DGLOBjtstart;
/*GLOBAL VARIABLES---GLOBAL VARIABLES---GLOBAL VARIABLES---GLOBAL VARIABLES---GLOBAL VARIABLES---------*/


void initSDE()
{DGLOBnrpartmem=100;
 DGLOBnrpart=0;

 DGLOBchdist=0.0;

if(!(DGLOBpart=(particle *)malloc((DGLOBnrpartmem+1)*sizeof(particle)))){printf("Out of memory"); exit(1);}

if(!(DGLOBtrstart=(triangle *)malloc(sizeof(triangle)))){printf("Out of memory"); exit(1);}
DGLOBtrstart->next=0;

#if RCONTACTS==1
if(!(DGLOBctstart=(contact *)malloc(sizeof(contact)))){printf("Out of memory"); exit(1);}
DGLOBctstart->next=0;
#endif

if(!(DGLOBjtstart=(joint *)malloc(sizeof(joint)))){printf("Out of memory"); exit(1);}
DGLOBjtstart->next=0;
}


/*set position of particle*/
void setPartPos(int i,REALD x,REALD y,REALD z)
{particle *part;
 part=&DGLOBpart[i];

 part->pos[0]=x;
 part->pos[1]=y;
 part->pos[2]=z;
 part->tras[0]=part->tras[1]=part->tras[2]=0;
 part->rots[0]=part->rots[1]=part->rots[2]=0;
 part->traac[0]=part->traac[1]=part->traac[2]=0;
 part->rotac[0]=part->rotac[1]=part->rotac[2]=0;

part->vx[0]=1.0;
part->vy[0]=0.0;
part->vz[0]=0.0;
part->vx[1]=0.0;
part->vy[1]=1.0;
part->vz[1]=0.0;
part->vx[2]=0.0;
part->vy[2]=0.0;
part->vz[2]=1.0;

}

forward(int i, REALD speed, int turn, int jump)
{
 particle *part;
 part=&DGLOBpart[i];

// part->pos[0]=x;
// part->pos[1]=y;
// part->pos[2]=z;
if(jump > 0 && part->tras[0] > -0.0125F && part->tras[0] < 0.0125F)
{
  part->tras[0] = 8.F;
}
//part->tras[1]=
part->tras[2]=0;
// part->rots[0]=
part->rots[1]=part->rots[2]=0;
 part->traac[0]=part->traac[1]=part->traac[2]=0;
 part->rotac[0]=part->rotac[1]=part->rotac[2]=0;
 float angle = (float)turn*PI / 128.0F;

part->vx[0]=1.0;
part->vy[0]=0.0;
part->vz[0]=0.0;
part->vx[1]=0.0;
part->vy[1]=cos(angle);
part->vz[1]=sin(angle);
part->vx[2]=0.0;
part->vy[2]=-sin(angle);
part->vz[2]=cos(angle);

// part->tras[0]=x;
part->tras[1]=-sin(angle)*speed;
part->tras[2]=cos(angle)*speed;
}

/*set translation speed of particle*/
void setPartSpeed(int i,REALD x,REALD y,REALD z)
{particle *part;
 part=&DGLOBpart[i];
 part->tras[0]=x; part->tras[1]=y; part->tras[2]=z;
}


/*set rotation speed of particle*/
void setPartRotSpeed(int i,REALD x,REALD y,REALD z)
{particle *part;
 part=&DGLOBpart[i];
 part->rots[0]=x; part->rots[1]=y; part->rots[2]=z;
}


/*adds new particle and returns its number (new total number of particles)*/
int addPart(REALD radius,REALD density,REALD k,REALD mu,REALD mu2)
{particle *part;

DGLOBnrpart++;
if(DGLOBnrpart>=DGLOBnrpartmem){
  DGLOBnrpartmem+=100;
  if(!(DGLOBpart=(particle *)realloc(DGLOBpart,(DGLOBnrpartmem+1)*sizeof(particle)))){
    printf("Out of memory"); exit(1);
  }
}

part=&DGLOBpart[DGLOBnrpart];

part->radius=radius;
part->density=density;
part->mass=part->density*4.0*PI*(part->radius*part->radius*part->radius)/3.0;
part->im=part->density*8.0*PI*(part->radius*part->radius*part->radius*part->radius*part->radius)/15.0;
part->k=k; /*elastic coef.*/
part->mu=mu; /*tangent friction*/
part->mu2=mu2;

setPartPos(DGLOBnrpart,0.0,0.0,0.0);

part->chforce=0.0;
part->rotflag=1; /*default: rotational, no cohesion*/

return DGLOBnrpart;}


/*set rotation flag*/
void setPartRot(int i,char flag)
{particle *part;
 part=&DGLOBpart[i];
 part->rotflag=flag;
}


/*set cohesion force*/
void setPartChForce(int i,REALD chforce)
{particle *part;
 part=&DGLOBpart[i];
 part->chforce=chforce;
}


triangle *addTriangle(REALD x1,REALD y1,REALD z1,REALD x2,REALD y2,REALD z2,REALD x3,REALD y3,REALD z3)
{REALD l,lmax;
triangle *trstart,*tr;
trstart=DGLOBtrstart;
if(!(tr=(triangle *)malloc(sizeof(triangle)))){printf("Out of memory"); exit(1);}
tr->next=trstart->next;
trstart->next=tr;
tr->p1[0]=x1; tr->p1[1]=y1; tr->p1[2]=z1;
tr->p2[0]=x2; tr->p2[1]=y2; tr->p2[2]=z2;
tr->p3[0]=x3; tr->p3[1]=y3; tr->p3[2]=z3;
lmax=(x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)+(z2-z1)*(z2-z1);
l=(x3-x1)*(x3-x1)+(y3-y1)*(y3-y1)+(z3-z1)*(z3-z1); if(l>lmax){lmax=l;}
l=(x3-x2)*(x3-x2)+(y3-y2)*(y3-y2)+(z3-z2)*(z3-z2); if(l>lmax){lmax=l;}
tr->length=sqrt(lmax); /*length of longest side*/
return tr;}


/*removes all triangles (identical to rmContacts(), except for the first 2 lines)*/
void rmTriangles()
{triangle *ctstart,*cont1,*cont2;
ctstart=DGLOBtrstart;
cont1=ctstart->next;
while(cont1!=0){
  cont2=cont1;
  cont1=cont1->next;
  free(cont2);
}
ctstart->next=0;
}


#if RCONTACTS==1

void addContact(REALD *pos,REALD *dir)
{int i;
contact *ctstart,*cont;
  ctstart=DGLOBctstart;
if(!(cont=(contact *)malloc(sizeof(contact)))){printf("Out of memory"); exit(1);}
cont->next=ctstart->next;
ctstart->next=cont;
for(i=0;i<=2;i++){
  cont->dir[i]=dir[i]; cont->pos[i]=pos[i];
}
} /*contacts are only for visualisation*/


/*removes all contacts*/
void rmContacts()
{contact *ctstart,*cont1,*cont2;
ctstart=DGLOBctstart;
cont1=ctstart->next;
while(cont1!=0){
  cont2=cont1;
  cont1=cont1->next;
  free(cont2);
}
ctstart->next=0;
}

#endif


/*adds joint between particles np1 and np2*/
joint *addJoint(int np1,int np2,REALD x,REALD y,REALD z)
{REALD posg[3];
joint *jtstart,*jt;
particle *part;
  jtstart=DGLOBjtstart;
  part=DGLOBpart;
if(np1>DGLOBnrpart){printf("Particle %d not found - couldn't add joint\r\n",np1); exit(1);}
if(np2>DGLOBnrpart){printf("Particle %d not found - couldn't add joint\r\n",np2); exit(1);}
posg[0]=x; posg[1]=y; posg[2]=z;
  if(!(jt=(joint *)malloc(sizeof(joint)))){printf("Out of memory"); exit(1);}
  jt->next=jtstart->next;
  jtstart->next=jt;
  jt->np[0]=np1; jt->np[1]=np2;
glob2loc(posg,jt->pos11,&part[np1]);
glob2loc(posg,jt->pos22,&part[np2]);
return jt;}


/*adds joint at x,y,z in local system of particle np2*/
joint *addJointRelPos(int np1,int np2,REALD x,REALD y,REALD z)
{REALD posl[3],posg[3];
joint *jtstart,*jt;
particle *part;
  jtstart=DGLOBjtstart;
  part=DGLOBpart;
if(np1>DGLOBnrpart){printf("Particle %d not found - couldn't add joint\r\n",np1); exit(1);}
if(np2>DGLOBnrpart){printf("Particle %d not found - couldn't add joint\r\n",np2); exit(1);}
posl[0]=x; posl[1]=y; posl[2]=z;
loc2glob(posl,posg,&part[np2]);
  if(!(jt=(joint *)malloc(sizeof(joint)))){printf("Out of memory"); exit(1);}
  jt->next=jtstart->next;
  jtstart->next=jt;
  jt->np[0]=np1; jt->np[1]=np2;
glob2loc(posg,jt->pos11,&part[np1]);
glob2loc(posg,jt->pos22,&part[np2]);
return jt;}


/*removes all joints (identical to rmContacts(), except for the first 2 lines)*/
void rmJoints()
{joint *ctstart,*cont1,*cont2;
ctstart=DGLOBjtstart;
cont1=ctstart->next;
while(cont1!=0){
  cont2=cont1;
  cont1=cont1->next;
  free(cont2);
}
ctstart->next=0;
}


void quitSDE()
{free(DGLOBpart);
rmTriangles();
free(DGLOBtrstart);
#if RCONTACTS==1
rmContacts();
free(DGLOBctstart);
#endif
rmJoints();
free(DGLOBjtstart);
}
