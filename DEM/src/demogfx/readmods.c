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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <SDL.h>
#include "confgrf.h"
#include "grafstr.h"


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


/*functie care afiseaza un mesaj de eroare si iese din program daca e cazul*/
void afermex(char *numefis,int lincr,char *s,int cdvr)
{/*cdvr - conditie de verificare (0-intreg,1-cuvant,2-real)*/
if((cdvr==0)&&(detsir(s)!=0)){printf("Error: '%s' line %d - integer expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
if((cdvr==1)&&(detsir(s)!=1)){printf("Error: '%s' line %d - word expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
if((cdvr==2)&&(detsir(s)!=2)&&(detsir(s)!=0)){printf("Error: '%s' line %d - number expected, got '%s'\r\n",numefis,lincr,s);exit(1);}
}


int identcom(char *s)
{if(strcmp(s,"fullbright")==0){return 1;}
 return 0;}


/*functie care afla numarul de triunghiuri*/
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
{int err,lincr=0,i,j,nrfaces,nvert;
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
err=fisgetw(fis,s,&lincr); x[i]=atof(s);
err=fisgetw(fis,s,&lincr); y[i]=atof(s);
err=fisgetw(fis,s,&lincr); z[i]=atof(s);
} /*aflat coordonatele punctelor*/

for(i=1;i<=nrfaces;i++){
  err=fisgetw(fis,s,&lincr); /*sarit peste "f"*/
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
if(identcom(s)==1){ /*fullbright*/
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



/*function which reads the objects*/
void createtypes()
{int nto=2, /*number of object types*/
     i,nfa;
char s1[MAXWLG],s2[MAXWLG],s3[MAXWLG];


  if(!(fceglob=(tria **)malloc((nto+1)*sizeof(tria *)))){printf("Out of memory"); exit(1);}
  if(!(nrfglob=(int *)malloc((nto+1)*sizeof(int)))){printf("Out of memory"); exit(1);}
  for(i=1;i<=nto;i++){
    switch(i){
      case 1: strcpy(s1,"src/demogfx/model/ic.geo"); /*geometry*/
              strcpy(s2,"src/demogfx/model/ic.col"); /*colors*/
              strcpy(s3,"src/demogfx/model/ic.cld"); /*backface culling*/
              break;
      case 2: strcpy(s1,"src/demogfx/model/ct.geo");
              strcpy(s2,"src/demogfx/model/ct.col");
              strcpy(s3,"src/demogfx/model/nocl.cld");
              break;
      default: printf("Object type %d not found\r\n",i); exit(1);
    }
    nfa=findnf(s1);
    if(!(fceglob[i]=(tria *)malloc((nfa+1)*sizeof(tria)))){printf("Out of memory"); exit(1);}
    nrfglob[i]=nfa;
    faces(fceglob[i],s1);
    readcolor(fceglob[i],nfa,s2);
    ordercl(fceglob[i],s3);
  }
}
