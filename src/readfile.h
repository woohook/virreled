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

/*functie care returneaza 1 daca i se transmite un caracter de delimitare si 0 daca nu*/
int verdel(char s)
{char a[6]={' ','\r','\n','\t',':'};
int i,sem=0;
  for(i=0;i<=4;i++){
    if(s==a[i]){sem=1;break;}
  }
return sem;}


/*functie care returneaza 1 daca i se transmite o litera mica si 0 daca nu*/
int vercrl(char s)
{char a[28]={'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z'};
int i,sem=0;
  for(i=0;i<=25;i++){
    if(s==a[i]){sem=1;break;}
  }
return sem;}


/*functie care returneaza 1 daca i se transmite o cifra sau '.' si 0 daca nu*/
int vercrcf(char s)
{char a[13]={'0','1','2','3','4','5','6','7','8','9','.'};
int i,sem=0;
  for(i=0;i<=10;i++){
    if(s==a[i]){sem=1;break;}
  }
return sem;}


/*functie care se uita sa vada ce e un sir de caractere: intreg(0), cuvant(1), real(2) sau altceva(3) ...*/
int detsir(char *s)
{int i=1,sem=3,crp=0; /*crp - variabila pentru numarat punctele*/
  if(vercrl(s[0])){sem=1; /*adica daca primul caracter e o litera*/
    while((!verdel(s[i]))&&s[i]){if(!vercrl(s[i])){sem=3;return sem;} i++;}
  return sem;}
    if((vercrcf(s[0]))||(s[0]=='-')){sem=2; /*adica daca primul caracter e o cifra*/
	if(s[0]=='.'){crp++;}
      while((!verdel(s[i]))&&s[i]){
	if(s[i]=='.'){crp++;}
	if(!vercrcf(s[i])){sem=3;return sem;} i++;
      }if((crp>=2)||((crp==1)&&(i==1))){sem=3;return sem;}
    if((!crp)&&(s[0]!='-')){sem=0;}
    return sem;}
return sem;}


/*functie care citeste un cuvant s din fisierul fis; lincr - linia curenta*/
int fisgetw(FILE *fis,char *s,int *lincr)
{int i,sem=0;
  static int sem2=0;if(sem2){(*lincr)++;sem2=0;}
s[0]=getc(fis);if(s[0]=='\n'){(*lincr)++;}
  while((!(feof(fis)))&&(verdel(s[0]))){s[0]=getc(fis);if(s[0]=='\n'){(*lincr)++;}}
  /*sarit peste caracterele de dinaintea cuvantului*/
i=1;
  while((!(feof(fis)))&&(i<MAXWLG)&&(!verdel(s[i-1])))
  {s[i]=getc(fis);i++;} if(!verdel(s[i-1])){sem=1;}
  if(s[i-1]=='\n'){sem2=1;} s[i-1]='\0';
return sem;} /*1 daca s-a depasit lungimea maxima a cuvantului sau sfarsitul fisierului si 0 daca nu*/


/*for readtrack()*/
int identcom(char *s)
{if(strcmp(s,"objtypes")==0){return 1;} /*pt. declarat nr. de tipuri de obiecte*/
 if(strcmp(s,"objects")==0){return 2;} /*pt. declarat nr. de obiecte*/
 if(strcmp(s,"background")==0){return 3;}
 if(strcmp(s,"clfactors")==0){return 4;}
 if(strcmp(s,"accel")==0){return 5;}
 if(strcmp(s,"brake")==0){return 6;} /*acceleration and brake torques/wheel*/
 if(strcmp(s,"spring")==0){return 7;}
 if(strcmp(s,"damper")==0){return 8;} /*suspension coefficients*/
/*friction and trailer joint eliminated, added drag and downforce coefficients:*/
 if(strcmp(s,"drag")==0){return 9;}
 if(strcmp(s,"downforce")==0){return 10;}
 if(strcmp(s,"camera")==0){return 11;} /*camera position*/
 if(strcmp(s,"ambient")==0){return 12;} /*ambient light*/
 if(strcmp(s,"headlight")==0){return 13;} /*forward light*/
 if(strcmp(s,"directional")==0){return 14;} /*intensity of light with fixed direction*/
 if(strcmp(s,"lightdir")==0){return 15;} /*direction of light*/
 if(strcmp(s,"fullbright")==0){return 16;}
 return 0;}


/*functie care afiseaza un mesaj de eroare si iese din program daca e cazul*/
void afermex(char *numefis,int lincr,char *s,int cdvr)
{/*cdvr - conditie de verificare (0-intreg,1-cuvant,2-real)*/
if((cdvr==0)&&(detsir(s)!=0)){printf("Error: '%s' line %d - integer expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
if((cdvr==1)&&(detsir(s)!=1)){printf("Error: '%s' line %d - word expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
if((cdvr==2)&&(detsir(s)!=2)&&(detsir(s)!=0)){printf("Error: '%s' line %d - number expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
}


/*find number of triangles and vertices*/
int findnf(char *numefis)
{int nvert,nrfaces;
FILE *fis;

if(!(fis=fopen(numefis,"r"))){printf("File '%s' could not be open\r\n",numefis);exit(1);}

fscanf(fis,"%d",&nvert);
fscanf(fis,"%d",&nrfaces);

fclose(fis);
return nrfaces;}


/*functie care afla coordonatele varfurilor triunghiurilor*/
void faces(tria *face,char *numefis)
{int lincr=0,i,j,nrfaces,nvert;
FILE *fis;
REALN *x,*y,*z;
char s[MAXWLG];

if(!(fis=fopen(numefis,"r"))){printf("File '%s' could not be open\r\n",numefis);exit(1);}

fscanf(fis,"%d",&nvert);
fscanf(fis,"%d",&nrfaces);

if(!(x=(REALN *)malloc((nvert+1)*sizeof(REALN)))){printf("Out of memory"); exit(1);}
if(!(y=(REALN *)malloc((nvert+1)*sizeof(REALN)))){printf("Out of memory"); exit(1);}
if(!(z=(REALN *)malloc((nvert+1)*sizeof(REALN)))){printf("Out of memory"); exit(1);}

for(i=1;i<=nvert;i++){
    fisgetw(fis,s,&lincr); x[i]=atof(s);
    fisgetw(fis,s,&lincr); y[i]=atof(s);
    fisgetw(fis,s,&lincr); z[i]=atof(s);
} /*aflat coordonatele punctelor*/

for(i=1;i<=nrfaces;i++){
  fisgetw(fis,s,&lincr); /*sarit peste "f"*/
  fscanf(fis,"%d",&j);
    face[i].x1=x[j];face[i].y1=y[j];face[i].z1=z[j];
  fscanf(fis,"%d",&j);
    face[i].x2=x[j];face[i].y2=y[j];face[i].z2=z[j];
  fscanf(fis,"%d",&j);
    face[i].x3=x[j];face[i].y3=y[j];face[i].z3=z[j];

  face[i].red=255;
  face[i].green=200;
  face[i].blue=0;

  face[i].cull=0; /*no culling and no fullbright if not specified*/
}

free(x);free(y);free(z);
fclose(fis);}


/*function which reads colors of triangles
*bred,*bgreen,*bblue - background color*/
void readcolor(tria *face,int nrfaces,char *numefis)
{int i,j,colors,fstart,fend,fred,fgreen,fblue; /*colors - number of colors*/
/*fstart si fend - first and last triangle with color(fred,fgreen,fblue)*/
FILE *fis;
char s[MAXWLG];

if(!(fis=fopen(numefis,"r"))){printf("File '%s' could not be open\r\n",numefis);exit(1);}

fscanf(fis,"%d",&colors); /*found number of colors*/

for(i=1;i<=nrfaces;i++){face[i].red=255;face[i].green=255;face[i].blue=255;}

for(j=1;j<=colors;j++){
fscanf(fis,"%d %d %d %d %d",&fstart,&fend,&fred,&fgreen,&fblue);
  for(i=fstart;i<=fend;i++){
    face[i].red=fred;
    face[i].green=fgreen;
    face[i].blue=fblue;
  }
}

j=fisgetw(fis,s,&i);
if(identcom(s)==16){ /*fullbright*/
  fscanf(fis,"%d",&colors);
  for(j=1;j<=colors;j++){
    fscanf(fis,"%d %d",&fstart,&fend);
    for(i=fstart;i<=fend;i++){
      face[i].cull=((face[i].cull)&1)+2;
    }
  }
}

fclose(fis);
}


/*function which reads the reference points (i.e. triangle vertices) of an object type*/
void readref(refpo *rep,char *numefis)
{int i,j,lincr,nvert,ntri; /*number of vertices and triangles*/
REALN *x,*y,*z;
char s[MAXWLG];
FILE *fis;

if(!(fis=fopen(numefis,"r"))){printf("File '%s' could not be open\r\n",numefis);exit(1);}

fscanf(fis,"%d",&nvert);
fscanf(fis,"%d",&ntri);
/*if(ntri>=(MAXCTR-1)){printf("File '%s' - too many triangles\r\n",numefis);exit(1);}*/
if(!(rep->p1=(T3REALD *)malloc((ntri+1)*sizeof(T3REALD)))){printf("Out of memory"); exit(1);}
if(!(rep->p2=(T3REALD *)malloc((ntri+1)*sizeof(T3REALD)))){printf("Out of memory"); exit(1);}
if(!(rep->p3=(T3REALD *)malloc((ntri+1)*sizeof(T3REALD)))){printf("Out of memory"); exit(1);}

rep->ntr=ntri; /*number of triangles*/

if(!(x=(REALN *)malloc((nvert+1)*sizeof(REALN)))){printf("Out of memory"); exit(1);}
if(!(y=(REALN *)malloc((nvert+1)*sizeof(REALN)))){printf("Out of memory"); exit(1);}
if(!(z=(REALN *)malloc((nvert+1)*sizeof(REALN)))){printf("Out of memory"); exit(1);}

for(i=1;i<=nvert;i++){
    fisgetw(fis,s,&lincr); x[i]=atof(s);
    fisgetw(fis,s,&lincr); y[i]=atof(s);
    fisgetw(fis,s,&lincr); z[i]=atof(s);
}

for(i=1;i<=ntri;i++){
  fisgetw(fis,s,&lincr); /*skip "f"*/
  fscanf(fis,"%d",&j);
    rep->p1[i][0]=x[j]; rep->p1[i][1]=y[j]; rep->p1[i][2]=z[j];
  fscanf(fis,"%d",&j);
    rep->p2[i][0]=x[j]; rep->p2[i][1]=y[j]; rep->p2[i][2]=z[j];
  fscanf(fis,"%d",&j);
    rep->p3[i][0]=x[j]; rep->p3[i][1]=y[j]; rep->p3[i][2]=z[j];
}

free(x); free(y); free(z);
fclose(fis);
}


/*order vertices, so triangles can be culled correctly*/
void ordercl(tria *face,char *numefis)
{int i,j,nrcmd,nf1,nf2;
REALN x,y,z,dx,dy,dz,a,b,c,d,prodscal,tmp;
char vis; /*'v'-visible from (x,y,z); 'i'-not visible*/
FILE *fis;

if(!(fis=fopen(numefis,"r"))){printf("File '%s' could not be open\r\n",numefis);exit(1);}

fscanf(fis,"%d",&nrcmd);

for(i=1;i<=nrcmd;i++){
  fscanf(fis,"%d %d %c %f %f %f",&nf1,&nf2,&vis,&x,&y,&z);

  for(j=nf1;j<=nf2;j++){
    findplan(face,j,&a,&b,&c,&d);
     dx=face[j].x1-x;
     dy=face[j].y1-y;
     dz=face[j].z1-z;
    prodscal=a*dx+b*dy+c*dz;
    switch(vis){
      case 'v': face[j].cull=((face[j].cull)&2)+1;
        if(prodscal<0){
          tmp=face[j].x1; face[j].x1=face[j].x2; face[j].x2=tmp;
          tmp=face[j].y1; face[j].y1=face[j].y2; face[j].y2=tmp;
          tmp=face[j].z1; face[j].z1=face[j].z2; face[j].z2=tmp;
        } break;

      case 'i': face[j].cull=((face[j].cull)&2)+1;
        if(prodscal>=0){
          tmp=face[j].x1; face[j].x1=face[j].x2; face[j].x2=tmp;
          tmp=face[j].y1; face[j].y1=face[j].y2; face[j].y2=tmp;
          tmp=face[j].z1; face[j].z1=face[j].z2; face[j].z2=tmp;
        } break;

      default: break;
    }
  }
}

fclose(fis);
}


/*function which finds the center and size of the object*/
void eval_obj(tria *face,sgob *objs)
{int i,nrfaces;
REALD xmin,xmax,ymin,ymax,zmin,zmax,lenx,leny,lenz;

nrfaces=objs->nfa;

xmin=xmax=face[1].x1;
ymin=ymax=face[1].y1;
zmin=zmax=face[1].z1;

for(i=1;i<=nrfaces;i++){
if(xmin>face[i].x1){xmin=face[i].x1;}
if(xmin>face[i].x2){xmin=face[i].x2;}
if(xmin>face[i].x3){xmin=face[i].x3;}
if(xmax<face[i].x1){xmax=face[i].x1;}
if(xmax<face[i].x2){xmax=face[i].x2;}
if(xmax<face[i].x3){xmax=face[i].x3;}

if(ymin>face[i].y1){ymin=face[i].y1;}
if(ymin>face[i].y2){ymin=face[i].y2;}
if(ymin>face[i].y3){ymin=face[i].y3;}
if(ymax<face[i].y1){ymax=face[i].y1;}
if(ymax<face[i].y2){ymax=face[i].y2;}
if(ymax<face[i].y3){ymax=face[i].y3;}

if(zmin>face[i].z1){zmin=face[i].z1;}
if(zmin>face[i].z2){zmin=face[i].z2;}
if(zmin>face[i].z3){zmin=face[i].z3;}
if(zmax<face[i].z1){zmax=face[i].z1;}
if(zmax<face[i].z2){zmax=face[i].z2;}
if(zmax<face[i].z3){zmax=face[i].z3;}
}

lenx=xmax-xmin;
leny=ymax-ymin;
lenz=zmax-zmin;
objs->xcen=(xmax+xmin)/2;
objs->ycen=(ymax+ymin)/2;
objs->zcen=(zmax+zmin)/2;
objs->radius=sqrt(lenx*lenx+leny*leny+lenz*lenz)/2;
}


/*function which reads the vehicle; must be called AFTER readtrack()
nrtyp,nrobt - number of object types and objects given by readtrack()*/
sgob *readvehicle(char *numefis,sgob *objs,int *nrtyp,int *nrobt,vhc *car)
{int err,lincr=1; /*lincr-current line*/
char s[MAXWLG]; /*a word*/
FILE *fis;
int i,k,nto,nob; /*number of object types and number of objects*/
REALN tx,ty,tz, /*initial translations*/
      radius,density,elk,mu;

nto=*nrtyp;
nob=*nrobt;

if(!(fis=fopen(numefis,"r"))){printf("Error: File %s could not be open\r\n",numefis);exit(1);}

s[0]='1';while(s[0]){

if(!(err=fisgetw(fis,s,&lincr))){afermex(numefis,lincr,s,1);}

switch(identcom(s)){
  case 1: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); (*nrtyp)+=atoi(s);
          if(!(fceglob=(tria **)realloc(fceglob,((*nrtyp)+1)*sizeof(tria *)))){printf("Out of memory"); exit(1);}
          if(!(refglob=(refpo *)realloc(refglob,((*nrtyp)+1)*sizeof(refpo)))){printf("Out of memory"); exit(1);}
          for(i=nto+1;i<=(*nrtyp);i++){
            err=fisgetw(fis,s,&lincr); /*file with triangles*/
            refglob[i].nfa=findnf(s);
            if(!(fceglob[i]=(tria *)malloc((refglob[i].nfa+1)*sizeof(tria)))){printf("Out of memory"); exit(1);}
            faces(fceglob[i],s);
              err=fisgetw(fis,s,&lincr); /*file with colors*/
              readcolor(fceglob[i],refglob[i].nfa,s);
            err=fisgetw(fis,s,&lincr); /*file with data for backface culling*/
            ordercl(fceglob[i],s);
          }
          break;

  case 2: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0);
          car->nob=atoi(s); nob+=car->nob; (*nrobt)=nob;
          if(!(objs=(sgob *)realloc(objs,(nob+1)*sizeof(sgob)))){printf("Out of memory"); exit(1);}
          for(i=(nob-car->nob+1);i<=nob;i++){
              err=fisgetw(fis,s,&lincr); afermex(numefis,lincr,s,0);
              objs[i].otyp=nto+atoi(s);
              if(objs[i].otyp>(*nrtyp)){
                printf("Error: there is no object type '%d'\r\n",objs[i].otyp-nto);exit(1);
              }
              objs[i].ntr=0; /*no collision triangles in vehicle, only spheres*/
              objs[i].nfa=refglob[objs[i].otyp].nfa;
              objs[i].vx[0]=objs[i].vy[0]=objs[i].vz[0]=0;
              objs[i].vx[1]=objs[i].vy[2]=objs[i].vz[3]=1;
              objs[i].vx[2]=objs[i].vx[3]=0;
              objs[i].vy[1]=objs[i].vy[3]=0;
              objs[i].vz[1]=objs[i].vz[2]=0;
              eval_obj(fceglob[objs[i].otyp],&objs[i]);

              k=i-nob+car->nob; /*1<=k<=car->nob*/
              car->oid[k]=i;
              err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); car->ofc[k]=atoi(s);
              switch(k){
              case 1: if(car->ofc[k]!=1){printf("Error: '%s' line %d - second number must be 1\r\n",numefis,lincr); exit(1);}
                      break;
              case 2: if(car->ofc[k]==1){printf("Error: '%s' line %d - second number must not be 1\r\n",numefis,lincr); exit(1);}
                      break;
              default: if(car->ofc[k]<=2){printf("Error: '%s' line %d - second number must be > 2\r\n",numefis,lincr); exit(1);}
                      break;
            }
            /*^object's function; 1-car; 2-trailer; 3,4,5,6-car wheel; 7-trailer wheel*/
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ty=atof(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tz=atof(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); radius=atof(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); density=atof(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); elk=atof(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); mu=atof(s);

            translat(&objs[i],tx,ty,tz);

            car->bid[k]=addPart(radius,density,elk,mu,0.5); /*0.5-normal friction coef.*/
            setPartPos(car->bid[k],tx,ty,tz);
          }
          /*(DGLOBpart[car->bid[1]].im)*=7;*/ /*increased inertial moment of car body*/
          (DGLOBpart[car->bid[1]].im)=0.50; /*quick fix for low cars - to be modified later*/
          break;

  case 5: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->accel=atof(s);
          break;

  case 6: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->brake=atof(s);
          break;

  case 7: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->spring=atof(s);
          break;

  case 8: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->damper=atof(s);
          break;

  case 9: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->drag=atof(s);
          break;

  case 10: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->downforce=atof(s);
          break;

  case 11: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->camh=atof(s);
           err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->caml=atof(s);
           err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); car->camd=atof(s);
           break;

  default: if(s[0]){printf("Error: '%s' line %d - word '%s' not recognized\r\n",numefis,lincr,s);exit(1);}
}
}
fclose(fis);


/*set joints*/
car->nj=0; /*number of joints*/
for(i=1;i<=car->nob;i++){
  if(car->ofc[i]>=3){

      k=1; if(car->ofc[i]==7){k=2;}

      (car->nj)++;
      car->jid[car->nj]=addJointRelPos(car->bid[k],car->bid[i],0.0,1.0,0.0);
      car->jfc[car->nj]=car->ofc[i];
      (car->jid[car->nj])->kt[0]=0.5*car->spring;
      (car->jid[car->nj])->kt[1]=0.5*car->spring;
      (car->jid[car->nj])->kt[2]=0.5*car->spring;
      (car->jid[car->nj])->ct[0]=0.5*car->damper;
      (car->jid[car->nj])->ct[1]=0.5*car->damper;
      (car->jid[car->nj])->ct[2]=0.5*car->damper;
      glob2loc(DGLOBpart[car->bid[i]].pos,car->jax[car->nj],&DGLOBpart[car->bid[k]]);
      (car->nj)++;
      car->jid[car->nj]=addJointRelPos(car->bid[k],car->bid[i],0.0,-1.0,0.0);
      car->jfc[car->nj]=car->ofc[i];
      (car->jid[car->nj])->kt[0]=0.5*car->spring;
      (car->jid[car->nj])->kt[1]=0.5*car->spring;
      (car->jid[car->nj])->kt[2]=0.5*car->spring;
      (car->jid[car->nj])->ct[0]=0.5*car->damper;
      (car->jid[car->nj])->ct[1]=0.5*car->damper;
      (car->jid[car->nj])->ct[2]=0.5*car->damper;
      glob2loc(DGLOBpart[car->bid[i]].pos,car->jax[car->nj],&DGLOBpart[car->bid[k]]);
  }
}
/*^set joints*/

return objs;}


/*function which reads the track; nrobt - number of objects*/
sgob *readtrack(char *numefis,int *nrobt,int *nrtyp,pixcol *backcol,lightpr *light)
{int err,lincr=1; /*lincr-current line*/
char s[MAXWLG]; /*a word*/
FILE *fis;
int i,j,
    nto=0,nob=0, /*number of object types and number of objects; nob=(*nrobt) */
    bred=130,bgreen=160,bblue=200; /*background color*/
sgob *objs,objtmp;
REALN tx,ty,tz,rx,ry,rz, /*initial translations and rotations of the object*/
      fred=1.0,fgreen=1.0,fblue=1.0, /*color multiplication factors*/
      ix,jx,kx,
      iy,jy,ky,
      iz,jz,kz, /*rotation matrix (temporary)*/
      len,
      p1[3],p2[3],p3[3];

objs=&objtmp; /*ca sa nu "warning"*/

light->ambient=0.5;
light->headlight=0.3;
light->directional=0.5;
light->dx=-0.5;
light->dy=1;
light->dz=1; /*set default values for the light*/

if(!(fis=fopen(numefis,"r"))){printf("Error: File %s could not be open\r\n",numefis);exit(1);}

s[0]='1';while(s[0]){

if(!(err=fisgetw(fis,s,&lincr))){afermex(numefis,lincr,s,1);}

switch(identcom(s)){
  case 3: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); bred=atoi(s);
          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); bgreen=atoi(s);
          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); bblue=atoi(s);
          break;

  case 4: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); fred=atof(s);
          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); fgreen=atof(s);
          err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); fblue=atof(s);
          break;

  case 1: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); (*nrtyp)=nto=atoi(s);
          if(!(fceglob=(tria **)malloc((nto+1)*sizeof(tria *)))){printf("Out of memory"); exit(1);}
          if(!(refglob=(refpo *)malloc((nto+1)*sizeof(refpo)))){printf("Out of memory"); exit(1);}
          for(i=1;i<=nto;i++){
            err=fisgetw(fis,s,&lincr); /*file with triangles (rendering)*/
            refglob[i].nfa=findnf(s);
            if(!(fceglob[i]=(tria *)malloc((refglob[i].nfa+1)*sizeof(tria)))){printf("Out of memory"); exit(1);}
            faces(fceglob[i],s);
              err=fisgetw(fis,s,&lincr); /*file with colors*/
              readcolor(fceglob[i],refglob[i].nfa,s);
            err=fisgetw(fis,s,&lincr); /*file with triangles (collision)*/
            readref(&refglob[i],s);
              err=fisgetw(fis,s,&lincr); /*file with data for backface culling*/
              ordercl(fceglob[i],s);
          }
          break;

  case 2: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); (*nrobt)=nob=atoi(s);
          if(!(objs=(sgob *)malloc((nob+1)*sizeof(sgob)))){printf("Out of memory"); exit(1);}
          for(i=1;i<=nob;i++){
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0);
              objs[i].otyp=atoi(s);
              if(objs[i].otyp>nto){
                printf("Error: there is no object type '%d'\r\n",objs[i].otyp);exit(1);
              }
              objs[i].ntr=refglob[objs[i].otyp].ntr;
              objs[i].nfa=refglob[objs[i].otyp].nfa;
              objs[i].vx[0]=objs[i].vy[0]=objs[i].vz[0]=0;
              objs[i].vx[1]=objs[i].vy[2]=objs[i].vz[3]=1;
              objs[i].vx[2]=objs[i].vx[3]=0;
              objs[i].vy[1]=objs[i].vy[3]=0;
              objs[i].vz[1]=objs[i].vz[2]=0;
              eval_obj(fceglob[objs[i].otyp],&objs[i]);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tx=atof(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ty=atof(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); tz=atof(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); rz=atof(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); ry=atof(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); rx=atof(s);
              rotatz(&objs[i],0,0,rz);
              rotaty(&objs[i],0,0,ry);
              rotatx(&objs[i],0,0,rx);
              translat(&objs[i],tx,ty,tz);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,0); objs[i].lev=atoi(s);
            err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); objs[i].mu=atof(s); /*friction*/


            ix=objs[i].vx[1]-objs[i].vx[0];
            jx=objs[i].vx[2]-objs[i].vx[0];
            kx=objs[i].vx[3]-objs[i].vx[0];
              iy=objs[i].vy[1]-objs[i].vy[0];
              jy=objs[i].vy[2]-objs[i].vy[0];
              ky=objs[i].vy[3]-objs[i].vy[0];
            iz=objs[i].vz[1]-objs[i].vz[0];
            jz=objs[i].vz[2]-objs[i].vz[0];
            kz=objs[i].vz[3]-objs[i].vz[0]; /*unit vectors of the local axes in global system*/

            for(j=1;j<=objs[i].ntr;j++){
                tx=refglob[objs[i].otyp].p1[j][0];
                ty=refglob[objs[i].otyp].p1[j][1];
                tz=refglob[objs[i].otyp].p1[j][2];
              p1[0]=objs[i].vx[0]+tx*ix+ty*jx+tz*kx;
              p1[1]=objs[i].vy[0]+tx*iy+ty*jy+tz*ky;
              p1[2]=objs[i].vz[0]+tx*iz+ty*jz+tz*kz;
                tx=refglob[objs[i].otyp].p2[j][0];
                ty=refglob[objs[i].otyp].p2[j][1];
                tz=refglob[objs[i].otyp].p2[j][2];
              p2[0]=objs[i].vx[0]+tx*ix+ty*jx+tz*kx;
              p2[1]=objs[i].vy[0]+tx*iy+ty*jy+tz*ky;
              p2[2]=objs[i].vz[0]+tx*iz+ty*jz+tz*kz;
                tx=refglob[objs[i].otyp].p3[j][0];
                ty=refglob[objs[i].otyp].p3[j][1];
                tz=refglob[objs[i].otyp].p3[j][2];
              p3[0]=objs[i].vx[0]+tx*ix+ty*jx+tz*kx;
              p3[1]=objs[i].vy[0]+tx*iy+ty*jy+tz*ky;
              p3[2]=objs[i].vz[0]+tx*iz+ty*jz+tz*kz; /*updated positions of triangles*/

              addTriangle(p1[0],p1[1],p1[2],p2[0],p2[1],p2[2],p3[0],p3[1],p3[2]);
            }

          }
          break;

  case 12: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light->ambient=atof(s); break;

  case 13: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light->headlight=atof(s); break;

  case 14: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light->directional=atof(s); break;

  case 15: err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light->dx=atof(s);
           err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light->dy=atof(s);
           err=fisgetw(fis,s,&lincr);afermex(numefis,lincr,s,2); light->dz=atof(s);
           break;

  default: if(s[0]){printf("Error: '%s' line %d - word '%s' not recognized\r\n",numefis,lincr,s);exit(1);}
}
}
fclose(fis);

for(i=1;i<=nto;i++){
  for(j=1;j<=refglob[i].nfa;j++){
    len=fceglob[i][j].red*fred;
    fceglob[i][j].red=(int)len; if(fceglob[i][j].red>255){fceglob[i][j].red=255;}
    len=fceglob[i][j].green*fgreen;
    fceglob[i][j].green=(int)len; if(fceglob[i][j].green>255){fceglob[i][j].green=255;}
    len=fceglob[i][j].blue*fblue;
    fceglob[i][j].blue=(int)len; if(fceglob[i][j].blue>255){fceglob[i][j].blue=255;}
  }
}

backcol->red=bred; backcol->green=bgreen; backcol->blue=bblue;

len=sqrt(light->dx*light->dx+light->dy*light->dy+light->dz*light->dz);
light->dx/=len;
light->dy/=len;
light->dz/=len; /*normalized light direction*/

return objs;}
