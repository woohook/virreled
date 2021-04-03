/*
Copyright (C) 2014 Victor Matei Petrescu

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
#include <string.h>
#include <time.h>
#include "readfile.h"


typedef struct _texture{
char name[101];
int red;
int green;
int blue;
} texture;


/*initialize random numbers*/
void inrnd()
{int i,t,r;
t=time(NULL); if(t<0){t=-t;} t=t%16384;
for(i=1;i<=t;i++){
  r=rand();
}
}


int irand1()
{int a;
a=rand();
a=70+a%140;
return a;}


int main()
{char s[MAXWLG];
int i,j,
    nrvert0=0,nrvert=0,nrtr0=0,nrtr=0,dn, /*nr. of vertices and triangles*/
    *n1,*n2,*n3,
    nrcol=0,tc1[10000],tc2[10000],
    nrtex=0; /*nr. of textures*/
float *x,*y,*z;
texture tex[10000];
FILE *f1,*fgeo,*fcol;



inrnd();
tc1[1]=1; /*first coloured triangle*/

if(!(x=(float *)malloc(sizeof(float)))){printf("Out of memory\r\n");exit (1);}
if(!(y=(float *)malloc(sizeof(float)))){printf("Out of memory\r\n");exit (1);}
if(!(z=(float *)malloc(sizeof(float)))){printf("Out of memory\r\n");exit (1);}

if(!(n1=(int *)malloc(sizeof(int)))){printf("Out of memory\r\n");exit (1);}
if(!(n2=(int *)malloc(sizeof(int)))){printf("Out of memory\r\n");exit (1);}
if(!(n3=(int *)malloc(sizeof(int)))){printf("Out of memory\r\n");exit (1);}


printf("Input file (.ac): "); scanf("%s",s);
if(!(f1=fopen(s,"r"))){printf("Could not open file '%s'\r\n",s); exit(1);}


while(!feof(f1)){
  i=0;
  while((i!=3)&&(!feof(f1))){i=fgetwnum(f1);} /*3-textures*/
  if(i!=3){break;}
  nrtex++;
  fscanf(f1,"%s",tex[nrtex].name);
  tex[nrtex].red=irand1();
  tex[nrtex].green=irand1();
  tex[nrtex].blue=irand1();
  for(j=1;j<=(nrtex-1);j++){
    if(strcmp(tex[j].name,tex[nrtex].name)==0){
      tex[nrtex]=tex[j];
    }
  }

  i=0;
  while((i!=1)&&(!feof(f1))){i=fgetwnum(f1);} /*1-vertices*/
  if(i!=1){break;}
  fscanf(f1,"%d",&dn);
  nrvert0=nrvert; nrvert+=dn;
  nrcol+=1; /*colours*/
  if(!(x=(float *)realloc(x,(nrvert+1)*sizeof(float)))){printf("Out of memory\r\n");exit (1);}
  if(!(y=(float *)realloc(y,(nrvert+1)*sizeof(float)))){printf("Out of memory\r\n");exit (1);}
  if(!(z=(float *)realloc(z,(nrvert+1)*sizeof(float)))){printf("Out of memory\r\n");exit (1);}
  for(i=nrvert0+1;i<=nrvert;i++){
    fscanf(f1,"%f %f %f",&z[i],&x[i],&y[i]);
  }

  while(((fgetwnum(f1))!=2)&&(!feof(f1))){;} /*2-triangles*/
  fscanf(f1,"%d",&dn);
  nrtr0=nrtr; nrtr+=dn;
  tc2[nrcol]=nrtr; tc1[nrcol+1]=nrtr+1; /*colours*/
  if(!(n1=(int *)realloc(n1,(nrtr+1)*sizeof(int)))){printf("Out of memory\r\n");exit (1);}
  if(!(n2=(int *)realloc(n2,(nrtr+1)*sizeof(int)))){printf("Out of memory\r\n");exit (1);}
  if(!(n3=(int *)realloc(n3,(nrtr+1)*sizeof(int)))){printf("Out of memory\r\n");exit (1);}
  for(i=nrtr0+1;i<=nrtr;i++){
    fskipw(f1,6); /*skip 6 words*/
    fscanf(f1,"%d",&n1[i]);n1[i]+=(nrvert0+1) ; fskipw(f1,2);
    fscanf(f1,"%d",&n2[i]);n2[i]+=(nrvert0+1) ; fskipw(f1,2);
    fscanf(f1,"%d",&n3[i]);n3[i]+=(nrvert0+1) ; fskipw(f1,2);
  }

}

fclose(f1); /*read input file*/


if(!(fgeo=fopen("out.geo","w"))){printf("Could not open file 'out.geo'\r\n"); exit(1);}

fprintf(fgeo,"%d %d\r\n",nrvert,nrtr);

for(i=1;i<=nrvert;i++){
  fprintf(fgeo,"%1.4f %1.4f %1.4f\r\n",x[i],y[i],z[i]);
}

for(i=1;i<=nrtr;i++){
  fprintf(fgeo,"f %d %d %d\r\n",n1[i],n2[i],n3[i]);
}

fclose(fgeo); /*wrote geometry file*/


if(!(fcol=fopen("out.col","w"))){printf("Could not open file 'out.col'\r\n"); exit(1);}

fprintf(fcol,"%d\r\n",nrcol);

for(i=1;i<=nrcol;i++){
  fprintf(fcol,"%d %d %d %d %d\r\n",tc1[i],tc2[i],tex[i].red,tex[i].green,tex[i].blue);
}

fclose(fcol);

printf("nrcol=%d, nrtex=%d\r\n'out.geo' and 'out.col' written\r\n",nrcol,nrtex);

return 0;}
