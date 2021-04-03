/*
Copyright (C) 2007-2012 Victor Matei Petrescu

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
#include <time.h>

typedef struct _obi
{int tip;
double x;
double y;
double z;
double tt;
int lev;
} obi;


/*initialize random numbers*/
void inrnd()
{int i,t,r;
t=time(NULL); if(t<0){t=-t;} t=t%16384;
for(i=1;i<=t;i++){
  r=rand();
}
}


int main()
{int i,j,*nr,*tip,*obtyp,cr,
     nob=0,tno, /*number of road objects, total number of objects*/
     oc,
     nral=3000; /*number of objects for which to allocate memory*/
double ttcr=0,xcr=-1.2,ycr=0,zcr=0, /*current coordinates*/
       *tt,*x,*y,*z,axdis,
       val=1.2,brd; /*distance from vehicle to road*/
float  freq[10];
int h4l;
obi *wan;

FILE *fis,*ofs;
char numefis[20];

if(!(nr=(int *)malloc((nral+1)*sizeof(int)))){printf("Out of memory"); exit(1);}
if(!(tip=(int *)malloc((nral+1)*sizeof(int)))){printf("Out of memory"); exit(1);}
if(!(obtyp=(int *)malloc((nral+1)*sizeof(int)))){printf("Out of memory"); exit(1);}

if(!(tt=(double *)malloc((nral+1)*sizeof(double)))){printf("Out of memory"); exit(1);}
if(!(x=(double *)malloc((nral+1)*sizeof(double)))){printf("Out of memory"); exit(1);}
if(!(y=(double *)malloc((nral+1)*sizeof(double)))){printf("Out of memory"); exit(1);}
if(!(z=(double *)malloc((nral+1)*sizeof(double)))){printf("Out of memory"); exit(1);}

if(!(wan=(obi *)malloc((nral+1)*sizeof(obi)))){printf("Out of memory"); exit(1);}

inrnd();

printf("\r\nInput file: ");scanf("%s",numefis);
if(!(fis=fopen(numefis,"r"))){printf("Error: Could not open file '%s'\r\n",numefis);exit(1);}
printf("Output file: ");scanf("%s",numefis);
if(!(ofs=fopen(numefis,"w"))){printf("Error: Could not open file '%s'\r\n",numefis);exit(1);}


freq[0]=100.0;
printf("\r\nFrequencies of objects on the sides of the track:\r\n\r\n");
printf("House 1 (max. %1.2f): ",freq[0]); scanf("%f",&freq[1]); freq[1]=freq[0]-freq[1];
printf("House 2 (max. %1.2f): ",freq[1]); scanf("%f",&freq[2]); freq[2]=freq[1]-freq[2];
printf("Tree 1 (max. %1.2f): ",freq[2]); scanf("%f",&freq[3]); freq[3]=freq[2]-freq[3];
printf("Tree 2 (max. %1.2f): ",freq[3]); scanf("%f",&freq[4]); freq[4]=freq[3]-freq[4];
printf("Tree 3 (max. %1.2f): ",freq[4]); scanf("%f",&freq[5]); freq[5]=freq[4]-freq[5];
if(freq[5]<0){printf("\r\nError: Sum of frequencies > 100%% \r\n\r\n");exit(1);}
printf("Houses on sides of road with 4 lanes (0/1): "); scanf("%d",&h4l);


fprintf(ofs,"background 170 200 255\r\nclfactors 1.0 1.0 1.0\r\n");
fprintf(ofs,"objtypes 49\r\n");
fprintf(ofs,"data/steer.geo data/steer.col data/noref.ref data/nocl.cld\r\n");
fprintf(ofs,"data/speed.geo data/speed.col data/noref.ref data/nocl.cld\r\n");
fprintf(ofs,"data/dmod.geo data/dmod.col data/noref.ref data/nocl.cld\r\n");
fprintf(ofs,"data/roads.geo data/road.col data/roads.ref data/road.cld\r\n");
fprintf(ofs,"data/roadlw.geo data/road.col data/roadlw.ref data/road.cld\r\n");
fprintf(ofs,"data/roadln.geo data/road.col data/roadln.ref data/road.cld\r\n");
fprintf(ofs,"data/roadrw.geo data/road.col data/roadrw.ref data/road.cld\r\n");
fprintf(ofs,"data/roadrn.geo data/road.col data/roadrn.ref data/road.cld\r\n");
fprintf(ofs,"data/road25.geo data/road.col data/road25.ref data/road.cld\r\n");
fprintf(ofs,"data/road50.geo data/road.col data/road50.ref data/road.cld\r\n");
fprintf(ofs,"data/road75.geo data/road.col data/road75.ref data/road.cld\r\n");
fprintf(ofs,"data/tunnb.geo data/tunnb.col data/tunnb.ref data/tunnb.cld\r\n");
fprintf(ofs,"data/tunn.geo data/tunn.col data/tunn.ref data/tunn.cld\r\n");
fprintf(ofs,"data/bridgec.geo data/bridgec.col data/bridgstr.ref data/bridgec.cld\r\n");
fprintf(ofs,"data/bridges.geo data/bridges.col data/bridges.ref data/bridges.cld\r\n");
fprintf(ofs,"data/start.geo data/start.col data/start.ref data/nocl.cld\r\n");
fprintf(ofs,"data/tree1.geo data/tree1.col data/tree.ref data/tree.cld\r\n");
fprintf(ofs,"data/tree2.geo data/tree2.col data/tree.ref data/tree.cld\r\n");
fprintf(ofs,"data/house1.geo data/house1.col data/house1.ref data/house1.cld\r\n");
fprintf(ofs,"data/house2.geo data/house2.col data/house2.ref data/house2.cld\r\n");
fprintf(ofs,"data/trs.geo data/trs.col data/trs.ref data/trs.cld\r\n");
fprintf(ofs,"data/wind.geo data/wind.col data/wind.ref data/wind.cld\r\n");
fprintf(ofs,"data/prop.geo data/prop.col data/noref.ref data/nocl.cld\r\n");
fprintf(ofs,"data/tree3.geo data/tree3.col data/tree.ref data/nocl.cld\r\n");
fprintf(ofs,"data/glider.geo data/glider.col data/noref.ref data/nocl.cld\r\n");
fprintf(ofs,"data/bridgescl.geo data/bridges.col data/bridgec.ref data/bridgescl.cld\r\n");
fprintf(ofs,"data/loop4r.geo data/loopblue.col data/loop4r.ref data/loop4r.cld\r\n");
fprintf(ofs,"data/loop2r.geo data/loopgreen.col data/loop2r.ref data/loop2r.cld\r\n");
fprintf(ofs,"data/loop1r.geo data/loopred.col data/loop1r.ref data/loop1r.cld\r\n");
fprintf(ofs,"data/loop4l.geo data/loopblue.col data/loop4l.ref data/loop4l.cld\r\n");
fprintf(ofs,"data/loop2l.geo data/loopgreen.col data/loop2l.ref data/loop2l.cld\r\n");
fprintf(ofs,"data/loop1l.geo data/loopred.col data/loop1l.ref data/loop1l.cld\r\n");
fprintf(ofs,"data/corkl.geo data/cork.col data/corkl.ref data/corkl.cld\r\n");
fprintf(ofs,"data/corkr.geo data/cork.col data/corkr.ref data/corkr.cld\r\n");
fprintf(ofs,"data/road11.geo data/road.col data/road11.ref data/road.cld\r\n");
fprintf(ofs,"data/road12.geo data/road.col data/road12.ref data/road.cld\r\n");
fprintf(ofs,"data/roadhill.geo data/roadhill.col data/roads.ref data/roadhill.cld\r\n");
fprintf(ofs,"data/roadhl25.geo data/road.col data/roadhl25.ref data/road.cld\r\n");
fprintf(ofs,"data/roadhr25.geo data/road.col data/roadhr25.ref data/road.cld\r\n");
fprintf(ofs,"data/roadhl50.geo data/road.col data/roadhl50.ref data/road.cld\r\n");
fprintf(ofs,"data/roadhr50.geo data/road.col data/roadhr50.ref data/road.cld\r\n");
fprintf(ofs,"data/roadhl75.geo data/road.col data/roadhl75.ref data/road.cld\r\n");
fprintf(ofs,"data/roadhr75.geo data/road.col data/roadhr75.ref data/road.cld\r\n");
fprintf(ofs,"data/wroads.geo data/wroad.col data/wroads.ref data/wroad.cld\r\n");
fprintf(ofs,"data/wroadl.geo data/wroad.col data/wroadl.ref data/wroad.cld\r\n");
fprintf(ofs,"data/roadch.geo data/roadch.col data/roadch.ref data/roadch.cld\r\n");
fprintf(ofs,"data/ter.geo data/ter.col data/ter.ref data/ter.cld\r\n");
fprintf(ofs,"data/ter1.geo data/ter1.col data/ter1.ref data/ter1.cld\r\n");
fprintf(ofs,"data/ter2.geo data/ter2.col data/ter2.ref data/ter2.cld\r\n");
cr=0;
while(!feof(fis)){
  cr++;
  fscanf(fis,"%d",&nr[cr]);
  fscanf(fis,"%d",&tip[cr]);
  if((nr[cr]>0)&&(!feof(fis))){nob+=nr[cr];}
} cr--;

oc=1;

wan[oc].tip=16; wan[oc].x=-val; wan[oc].y=0;wan[oc].z=20; wan[oc].tt=0;wan[oc].lev=2;

oc++;


  wan[oc].tip=21; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=5;

  for(j=1;j<=cr;j++){
    for(i=1;i<=nr[j];i++){ oc++;
      switch(tip[j]){
        case 0: wan[oc].tip=4; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                break;
        case 3: wan[oc].tip=5; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=9.7448*sin(ttcr)-1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)+1.9383*sin(ttcr); ttcr+=-0.3926991;
                 break;
        case 4: wan[oc].tip=6; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=9.0031*sin(ttcr)-3.7292*cos(ttcr); zcr+=9.0031*cos(ttcr)+3.7292*sin(ttcr); ttcr+=-0.7853982;
                 break;
        case 1: wan[oc].tip=7; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=9.7448*sin(ttcr)+1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)-1.9383*sin(ttcr); ttcr+=0.3926991;
                 break;
        case 2: wan[oc].tip=8; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=9.0031*sin(ttcr)+3.7292*cos(ttcr); zcr+=9.0031*cos(ttcr)-3.7292*sin(ttcr); ttcr+=0.7853982;
                 break;
        case 5: wan[oc].tip=9; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=0.8; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                break;
        case 6: wan[oc].tip=10; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=1.6; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                break;
        case 7: wan[oc].tip=11; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=2.4; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                break;
        case 8:xcr-=0.8; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                wan[oc].tip=9; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                break;
        case 9:xcr-=1.6; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                wan[oc].tip=10; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                break;
        case 10:xcr-=2.4; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                wan[oc].tip=11; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                break;
        case 11:wan[oc].tip=12; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=0; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                break;
        case 12:wan[oc].tip=13; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=0; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                break;
        case 13:xcr+=0; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                wan[oc].tip=12; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                break;
        case 14: wan[oc].tip=14; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=2.4; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                break;
        case 15: wan[oc].tip=15; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=0; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                break;
        case 16:xcr-=2.4; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                wan[oc].tip=14; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                break;
        case 17:wan[oc].tip=26; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=2.4; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                break; /*bridge climb, not start (added in version 4.0.0)*/
        case 18:xcr-=2.4; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                wan[oc].tip=26; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                break; /*bridge descend*/
        case 19:wan[oc].tip=27; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=0; ycr+=5*cos(ttcr); zcr-=5*sin(ttcr); ttcr+=0;
                break; /*loop*/
        case 20:wan[oc].tip=28; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=0; ycr+=5*cos(ttcr); zcr-=5*sin(ttcr); ttcr+=0;
                break; /*loop*/
        case 21:wan[oc].tip=29; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=0; ycr+=5*cos(ttcr); zcr-=5*sin(ttcr); ttcr+=0;
                break; /*loop*/
        case 22:wan[oc].tip=30; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=0; ycr-=5*cos(ttcr); zcr+=5*sin(ttcr); ttcr+=0;
                break; /*loop*/
        case 23:wan[oc].tip=31; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=0; ycr-=5*cos(ttcr); zcr+=5*sin(ttcr); ttcr+=0;
                break; /*loop*/
        case 24:wan[oc].tip=32; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=0; ycr-=5*cos(ttcr); zcr+=5*sin(ttcr); ttcr+=0;
                break; /*loop*/
        case 25: wan[oc].tip=33; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr-0.12533;wan[oc].lev=1;
                xcr+=0; ycr+=0; zcr+=0; ttcr+=0;
                break; /*corkskrew left*/
        case 26: wan[oc].tip=34; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+0.12533;wan[oc].lev=1;
                xcr+=0; ycr+=0; zcr+=0; ttcr+=0;
                break; /*corkskrew right*/
        case 27: wan[oc].tip=35; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=7.0711; ycr+=7.0711*sin(ttcr); zcr+=7.0711*cos(ttcr); ttcr+=0;
                break;
        case 28: wan[oc].tip=36; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=4.4713; ycr+=8.9447*sin(ttcr); zcr+=8.9447*cos(ttcr); ttcr+=0;
                break;
        case 29:xcr-=7.0711; ycr+=7.0711*sin(ttcr); zcr+=7.0711*cos(ttcr); ttcr+=0;
                wan[oc].tip=35; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                break;
        case 30:xcr-=4.4713; ycr+=8.9447*sin(ttcr); zcr+=8.9447*cos(ttcr); ttcr+=0;
                wan[oc].tip=36; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                break;
        case 31:wan[oc].tip=37; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                break;
        case 32:ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                wan[oc].tip=37; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                break;
        case 33: wan[oc].tip=39; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=9.7448*sin(ttcr)+1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)-1.9383*sin(ttcr); ttcr+=0.3926991;
                 xcr+=0.8;
                 break;
        case 34: wan[oc].tip=38; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=9.7448*sin(ttcr)-1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)+1.9383*sin(ttcr); ttcr+=-0.3926991;
                 xcr+=0.8;
                 break;
        case 35: ycr+=9.7448*sin(ttcr)+1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)-1.9383*sin(ttcr); ttcr+=0.3926991;
                 xcr-=0.8;
                 wan[oc].tip=38; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                 break;
        case 36: ycr+=9.7448*sin(ttcr)-1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)+1.9383*sin(ttcr); ttcr+=-0.3926991;
                 xcr-=0.8;
                 wan[oc].tip=39; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                 break;
        case 37: wan[oc].tip=41; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=9.7448*sin(ttcr)+1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)-1.9383*sin(ttcr); ttcr+=0.3926991;
                 xcr+=1.6;
                 break;
        case 38: wan[oc].tip=40; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=9.7448*sin(ttcr)-1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)+1.9383*sin(ttcr); ttcr+=-0.3926991;
                 xcr+=1.6;
                 break;
        case 39: ycr+=9.7448*sin(ttcr)+1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)-1.9383*sin(ttcr); ttcr+=0.3926991;
                 xcr-=1.6;
                 wan[oc].tip=40; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                 break;
        case 40: ycr+=9.7448*sin(ttcr)-1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)+1.9383*sin(ttcr); ttcr+=-0.3926991;
                 xcr-=1.6;
                 wan[oc].tip=41; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                 break;
        case 41: wan[oc].tip=43; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=9.7448*sin(ttcr)+1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)-1.9383*sin(ttcr); ttcr+=0.3926991;
                 xcr+=2.4;
                 break;
        case 42: wan[oc].tip=42; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=9.7448*sin(ttcr)-1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)+1.9383*sin(ttcr); ttcr+=-0.3926991;
                 xcr+=2.4;
                 break;
        case 43: ycr+=9.7448*sin(ttcr)+1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)-1.9383*sin(ttcr); ttcr+=0.3926991;
                 xcr-=2.4;
                 wan[oc].tip=42; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                 break;
        case 44: ycr+=9.7448*sin(ttcr)-1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)+1.9383*sin(ttcr); ttcr+=-0.3926991;
                 xcr-=2.4;
                 wan[oc].tip=43; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                 break;
        case 45: wan[oc].tip=44; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                 break;
        case 47: wan[oc].tip=45; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                 ycr+=9.7448*sin(ttcr)-1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)+1.9383*sin(ttcr); ttcr+=-0.3926991;
                 xcr+=0.0;
                 break;
        case 46: ycr+=9.7448*sin(ttcr)+1.9383*cos(ttcr); zcr+=9.7448*cos(ttcr)-1.9383*sin(ttcr); ttcr+=0.3926991;
                 xcr-=0.0;
                 wan[oc].tip=45; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                 break;
        case 48: wan[oc].tip=46; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                xcr+=0.0; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                break;
        case 49:xcr-=0.0; ycr+=10*sin(ttcr); zcr+=10*cos(ttcr); ttcr+=0;
                wan[oc].tip=46; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr+3.1415927;wan[oc].lev=1;
                break;
        case 50: wan[oc].tip=47; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                ycr+=40*sin(ttcr); zcr+=40*cos(ttcr); ttcr+=0;
                break;
        case 51: wan[oc].tip=48; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                ycr+=40*sin(ttcr); zcr+=40*cos(ttcr); ttcr+=0;
                break;
        case 52: wan[oc].tip=49; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=1;
                ycr+=40*sin(ttcr); zcr+=40*cos(ttcr); ttcr+=0;
                break;

        default: printf("Error: Unknown type '%d'\r\n",tip[j]);exit(1);
      }
      x[oc]=xcr; y[oc]=ycr; z[oc]=zcr; tt[oc]=ttcr; obtyp[oc]=tip[j];
    }
  }
  oc++;
  wan[oc].tip=21; wan[oc].x=xcr; wan[oc].y=ycr;wan[oc].z=zcr; wan[oc].tt=-ttcr;wan[oc].lev=5;
  oc++;
  wan[oc].tip=16; wan[oc].x=xcr; wan[oc].y=ycr-20*sin(ttcr);wan[oc].z=zcr-20*cos(ttcr); wan[oc].tt=-ttcr;wan[oc].lev=2;

tno=oc;

for(i=6;i<=oc-4;i+=2){
if((obtyp[i]==11)||(obtyp[i]==12)||(obtyp[i]==13)||(obtyp[i]==14)||(obtyp[i]==15)||(obtyp[i]==16)||(obtyp[i]==17)||(obtyp[i]==18)){continue;}
if((obtyp[i+1]==11)||(obtyp[i+1]==12)||(obtyp[i+1]==13)||(obtyp[i+1]==14)||(obtyp[i+1]==15)||(obtyp[i+1]==16)||(obtyp[i+1]==17)||(obtyp[i+1]==18)){continue;}
if((obtyp[i-1]==11)||(obtyp[i-1]==12)||(obtyp[i-1]==13)||(obtyp[i-1]==14)||(obtyp[i-1]==15)||(obtyp[i-1]==16)||(obtyp[i-1]==17)||(obtyp[i-1]==18)){continue;}
if((obtyp[i]==19)||(obtyp[i+1]==19)||(obtyp[i-1]==19)||(obtyp[i+2]==19)||(obtyp[i-2]==19)){continue;}
if((obtyp[i]==20)||(obtyp[i+1]==20)||(obtyp[i-1]==20)||(obtyp[i+2]==20)||(obtyp[i-2]==20)){continue;}
if((obtyp[i]==21)||(obtyp[i+1]==21)||(obtyp[i-1]==21)||(obtyp[i+2]==21)||(obtyp[i-2]==21)){continue;}
if((obtyp[i]==22)||(obtyp[i+1]==22)||(obtyp[i-1]==22)||(obtyp[i+2]==22)||(obtyp[i-2]==22)){continue;}
if((obtyp[i]==23)||(obtyp[i+1]==23)||(obtyp[i-1]==23)||(obtyp[i+2]==23)||(obtyp[i-2]==23)){continue;}
if((obtyp[i]==24)||(obtyp[i+1]==24)||(obtyp[i-1]==24)||(obtyp[i+2]==24)||(obtyp[i-2]==24)){continue;}
if((obtyp[i]==25)||(obtyp[i+1]==25)||(obtyp[i-1]==25)||(obtyp[i+2]==25)||(obtyp[i-2]==25)){continue;}
if((obtyp[i]==26)||(obtyp[i+1]==26)||(obtyp[i-1]==26)||(obtyp[i+2]==26)||(obtyp[i-2]==26)){continue;}
if((obtyp[i+3]==25)||(obtyp[i-3]==25)){continue;}
if((obtyp[i+3]==26)||(obtyp[i-3]==26)){continue;}
if((obtyp[i]==27)||(obtyp[i+1]==27)||(obtyp[i-1]==27)){continue;}
if((obtyp[i]==28)||(obtyp[i+1]==28)||(obtyp[i-1]==28)){continue;}
if((obtyp[i]==29)||(obtyp[i+1]==29)||(obtyp[i-1]==29)){continue;}
if((obtyp[i]==30)||(obtyp[i+1]==30)||(obtyp[i-1]==30)){continue;}
if((obtyp[i]==48)||(obtyp[i-1]==48)||(obtyp[i+1]==48)){continue;}
if((obtyp[i]==49)||(obtyp[i-1]==49)||(obtyp[i+1]==49)){continue;}
if((obtyp[i]==50)||(obtyp[i-1]==50)||(obtyp[i+1]==50)){continue;}
if((obtyp[i]==51)||(obtyp[i-1]==51)||(obtyp[i+1]==51)){continue;}
if((obtyp[i]==52)||(obtyp[i-1]==52)||(obtyp[i+1]==52)){continue;}

j=rand(); j=j%100;

    if((obtyp[i]==6)||(obtyp[i]==7)||(obtyp[i]==9)||(obtyp[i]==10)){goto TREE1;}
    if((obtyp[i-1]==6)||(obtyp[i-1]==7)||(obtyp[i-1]==9)||(obtyp[i-1]==10)){goto TREE1;}
    if((obtyp[i+1]==6)||(obtyp[i+1]==7)||(obtyp[i+1]==9)||(obtyp[i+1]==10)){goto TREE1;}
    if((obtyp[i]==33)||(obtyp[i]==34)||(obtyp[i]==35)||(obtyp[i]==36)){goto TREE1;}
    if((obtyp[i-1]==33)||(obtyp[i-1]==34)||(obtyp[i-1]==35)||(obtyp[i-1]==36)){goto TREE1;}
    if((obtyp[i+1]==33)||(obtyp[i+1]==34)||(obtyp[i+1]==35)||(obtyp[i+1]==36)){goto TREE1;}
    if((obtyp[i]==37)||(obtyp[i]==38)||(obtyp[i]==39)||(obtyp[i]==40)){goto TREE1;}
    if((obtyp[i-1]==37)||(obtyp[i-1]==38)||(obtyp[i-1]==39)||(obtyp[i-1]==40)){goto TREE1;}
    if((obtyp[i+1]==37)||(obtyp[i+1]==38)||(obtyp[i+1]==39)||(obtyp[i+1]==40)){goto TREE1;}
    if((obtyp[i]==41)||(obtyp[i]==42)||(obtyp[i]==43)||(obtyp[i]==44)){goto TREE1;}
    if((obtyp[i-1]==41)||(obtyp[i-1]==42)||(obtyp[i-1]==43)||(obtyp[i-1]==44)){goto TREE1;}
    if((obtyp[i+1]==41)||(obtyp[i+1]==42)||(obtyp[i+1]==43)||(obtyp[i+1]==44)){goto TREE1;}
    brd=0;
    if((obtyp[i-1]==5)||(obtyp[i]==5)||(obtyp[i+1]==5)||(obtyp[i-1]==8)||(obtyp[i]==8)||(obtyp[i+1]==8)){goto TREE1;}
      if((j>freq[1])&&(j<freq[0])){cr=19;}
      else{if((j>freq[2])&&(j<freq[1])){cr=20;}
      else{goto TREE1;}}
    axdis=5.5;
    if((obtyp[i]==45)||(obtyp[i]==46)||(obtyp[i]==47)){axdis+=2.5; if(!h4l){continue;}}
    tno++;
    wan[tno].tip=cr; wan[tno].x=x[i]-brd; wan[tno].y=y[i]+axdis*sin(tt[i]-1.57);wan[tno].z=z[i]+axdis*cos(tt[i]-1.57);
    wan[tno].tt=-tt[i]+3.1416;wan[tno].lev=2;
  goto CONT;
TREE1:
  if((j>freq[3])&&(j<freq[2])){cr=17;}
  else{if((j>freq[4])&&(j<freq[3])){cr=18;}
  else{if((j>freq[5])&&(j<freq[4])){cr=24;}
  else{if((j>freq[1])&&(j<freq[0])){cr=17;}
  else{if((j>freq[2])&&(j<freq[1])){cr=18;}
  else{goto CONT;}}}}}
brd=0;
if((obtyp[i]==5)||(obtyp[i-1]==5)||(obtyp[i+1]==5)||(obtyp[i]==8)||(obtyp[i-1]==8)||(obtyp[i+1]==8)){brd=0.2;}
if((obtyp[i]==6)||(obtyp[i-1]==6)||(obtyp[i+1]==6)||(obtyp[i]==9)||(obtyp[i-1]==9)||(obtyp[i+1]==9)){brd=0.4;}
if((obtyp[i]==7)||(obtyp[i-1]==7)||(obtyp[i+1]==7)||(obtyp[i]==10)||(obtyp[i-1]==10)||(obtyp[i+1]==10)){brd=0.6;}
if((obtyp[i]>=33)||(obtyp[i-1]>=33)||(obtyp[i+1]>=33)){brd=0.6;}
if((obtyp[i]>=45)||(obtyp[i-1]>=45)||(obtyp[i+1]>=45)){brd=0.0;}
axdis=5.3;
if((obtyp[i]==45)||(obtyp[i]==46)||(obtyp[i]==47)){axdis+=2.5;}
tno++;
wan[tno].tip=cr; wan[tno].x=x[i]-brd; wan[tno].y=y[i]+axdis*sin(tt[i]-1.57);wan[tno].z=z[i]+axdis*cos(tt[i]-1.57);
wan[tno].tt=tt[i];wan[tno].lev=2;

CONT:

j=rand(); j=j%100;

    if((obtyp[i]==6)||(obtyp[i]==7)||(obtyp[i]==9)||(obtyp[i]==10)){goto TREE2;}
    if((obtyp[i-1]==6)||(obtyp[i-1]==7)||(obtyp[i-1]==9)||(obtyp[i-1]==10)){goto TREE2;}
    if((obtyp[i+1]==6)||(obtyp[i+1]==7)||(obtyp[i+1]==9)||(obtyp[i+1]==10)){goto TREE2;}
    if((obtyp[i]==33)||(obtyp[i]==34)||(obtyp[i]==35)||(obtyp[i]==36)){goto TREE2;}
    if((obtyp[i-1]==33)||(obtyp[i-1]==34)||(obtyp[i-1]==35)||(obtyp[i-1]==36)){goto TREE2;}
    if((obtyp[i+1]==33)||(obtyp[i+1]==34)||(obtyp[i+1]==35)||(obtyp[i+1]==36)){goto TREE2;}
    if((obtyp[i]==37)||(obtyp[i]==38)||(obtyp[i]==39)||(obtyp[i]==40)){goto TREE2;}
    if((obtyp[i-1]==37)||(obtyp[i-1]==38)||(obtyp[i-1]==39)||(obtyp[i-1]==40)){goto TREE2;}
    if((obtyp[i+1]==37)||(obtyp[i+1]==38)||(obtyp[i+1]==39)||(obtyp[i+1]==40)){goto TREE2;}
    if((obtyp[i]==41)||(obtyp[i]==42)||(obtyp[i]==43)||(obtyp[i]==44)){goto TREE2;}
    if((obtyp[i-1]==41)||(obtyp[i-1]==42)||(obtyp[i-1]==43)||(obtyp[i-1]==44)){goto TREE2;}
    if((obtyp[i+1]==41)||(obtyp[i+1]==42)||(obtyp[i+1]==43)||(obtyp[i+1]==44)){goto TREE2;}
    brd=0;
    if((obtyp[i-1]==5)||(obtyp[i]==5)||(obtyp[i+1]==5)||(obtyp[i-1]==8)||(obtyp[i]==8)||(obtyp[i+1]==8)){goto TREE2;}
      if((j>freq[1])&&(j<freq[0])){cr=19;}
      else{if((j>freq[2])&&(j<freq[1])){cr=20;}
      else{goto TREE2;}}
    axdis=5.5;
    if((obtyp[i]==45)||(obtyp[i]==46)||(obtyp[i]==47)){axdis+=2.5; if(!h4l){continue;}}
    tno++;
    wan[tno].tip=cr; wan[tno].x=x[i]-brd; wan[tno].y=y[i]-axdis*sin(tt[i]-1.57);wan[tno].z=z[i]-axdis*cos(tt[i]-1.57);
    wan[tno].tt=-tt[i];wan[tno].lev=2;
  continue;
TREE2:
  if((j>freq[3])&&(j<freq[2])){cr=17;}
  else{if((j>freq[4])&&(j<freq[3])){cr=18;}
  else{if((j>freq[5])&&(j<freq[4])){cr=24;}
  else{if((j>freq[1])&&(j<freq[0])){cr=17;}
  else{if((j>freq[2])&&(j<freq[1])){cr=18;}
  else{continue;}}}}}
brd=0;
if((obtyp[i]==5)||(obtyp[i-1]==5)||(obtyp[i+1]==5)||(obtyp[i]==8)||(obtyp[i-1]==8)||(obtyp[i+1]==8)){brd=0.2;}
if((obtyp[i]==6)||(obtyp[i-1]==6)||(obtyp[i+1]==6)||(obtyp[i]==9)||(obtyp[i-1]==9)||(obtyp[i+1]==9)){brd=0.4;}
if((obtyp[i]==7)||(obtyp[i-1]==7)||(obtyp[i+1]==7)||(obtyp[i]==10)||(obtyp[i-1]==10)||(obtyp[i+1]==10)){brd=0.6;}
if((obtyp[i]>=33)||(obtyp[i-1]>=33)||(obtyp[i+1]>=33)){brd=0.6;}
if((obtyp[i]>=45)||(obtyp[i-1]>=45)||(obtyp[i+1]>=45)){brd=0.0;}
axdis=5.3;
if((obtyp[i]==45)||(obtyp[i]==46)||(obtyp[i]==47)){axdis+=2.5;}
tno++;
wan[tno].tip=cr; wan[tno].x=x[i]-brd; wan[tno].y=y[i]-axdis*sin(tt[i]-1.57);wan[tno].z=z[i]-axdis*cos(tt[i]-1.57);
wan[tno].tt=tt[i];wan[tno].lev=2;
}

fprintf(ofs,"objects %d\r\n",tno);

for(i=1;i<=tno;i++){
  fprintf(ofs,"%d %1.4f %1.4f %1.4f 0 0 %1.4f %d 0\r\n",wan[i].tip,wan[i].x,wan[i].y,wan[i].z,wan[i].tt,wan[i].lev);
}

printf("\r\nTrack generated.\r\n\r\n");

fclose(fis);fclose(ofs);

free(nr); free(tip); free(obtyp);
free(tt); free(x); free(y); free(z);
free(wan);

return 0;}
