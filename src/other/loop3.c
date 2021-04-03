/*
Copyright (C) 2013 Victor Matei Petrescu

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

#define PI 3.1415927


int main()
{int i,j,m,n,p,s,
     tv[51][3]; /*max. 50 triangles*/
float r,w,rij,dtt,ttij,
      x[51],y[51],z[51]; /*max. 50 vertices*/
FILE *f;


printf("Radius: "); scanf("%f",&r);
printf("Width: "); scanf("%f",&w);
printf("Number of steps: "); scanf("%d",&p);
s=0; while((s!=-1)&&(s!=1)){printf("Left (-1) or right (1): "); scanf("%d",&s);}

n=16; /*vertices*/
m=12; /*triangles*/

y[1]=y[12]=-0.5*w; y[2]=y[3]=0.5*w; y[4]=y[5]=0.3*w; y[6]=y[7]=0.1*w; y[8]=y[9]=-0.1*w; y[10]=y[11]=-0.3*w;
y[13]=y[16]=y[1]+0.3; y[14]=y[15]=y[2]-0.3;
z[1]=z[2]=0; z[3]=z[4]=z[7]=z[8]=z[11]=z[12]=1.0; z[5]=z[6]=z[9]=z[10]=0.7;
z[13]=z[14]=z[1]; z[15]=z[16]=z[3];/*max. z must always be 1*/
for(j=1;j<=12;j++){x[j]=0;}
for(j=13;j<=16;j++){x[j]=-0.3;}

tv[1][1]=1; tv[1][2]=2;  tv[1][3]=5;
tv[2][1]=1; tv[2][2]=10; tv[2][3]=5;
tv[3][1]=1; tv[3][2]=10; tv[3][3]=11;
tv[4][1]=1; tv[4][2]=12; tv[4][3]=11;
tv[5][1]=2; tv[5][2]=3;  tv[5][3]=4;
tv[6][1]=2; tv[6][2]=5;  tv[6][3]=4;
tv[7][1]=9; tv[7][2]=6;  tv[7][3]=7;
tv[8][1]=9; tv[8][2]=8;  tv[8][3]=7;
tv[9][1]=1; tv[9][2]=12;  tv[9][3]=16;
tv[10][1]=1; tv[10][2]=13;  tv[10][3]=16;
tv[11][1]=2; tv[11][2]=3;  tv[11][3]=15;
tv[12][1]=2; tv[12][2]=14;  tv[12][3]=15;

if(!(f=fopen("loop.geo","w"))){printf("Could not open file 'loop.geo'\r\n"); exit(1);}

dtt=PI/p;

fprintf(f,"%d %d\r\n",2*n*p+8,2*m*p+4);

for(i=1;i<=2*p;i++){
  for(j=1;j<=n;j++){
    ttij=(i-1+z[j])*dtt; rij=r-x[j];
    fprintf(f,"%1.4f %1.4f %1.4f\r\n",r-rij*cos(ttij),y[j]+s*w*ttij/(2*PI),rij*sin(ttij));
  }
}
fprintf(f,"-5.50 %1.2f 0.00\r\n",0.5*s*w-5.0);
fprintf(f,"-5.50 %1.2f 0.00\r\n",0.5*s*w-17.0);
fprintf(f,"1.50 %1.2f 0.00\r\n",0.5*s*w-17.0);
fprintf(f,"1.50 %1.2f 0.00\r\n",0.5*s*w-5.0);
fprintf(f,"-5.50 %1.2f 0.00\r\n",0.5*s*w+17.0);
fprintf(f,"-5.50 %1.2f 0.00\r\n",0.5*s*w+5.0);
fprintf(f,"1.50 %1.2f 0.00\r\n",0.5*s*w+5.0);
fprintf(f,"1.50 %1.2f 0.00\r\n",0.5*s*w+17.0);

for(i=1;i<=2*p;i++){
  for(j=1;j<=m;j++){
    fprintf(f,"f %d %d %d\r\n",(i-1)*n+tv[j][1],(i-1)*n+tv[j][2],(i-1)*n+tv[j][3]);
  }
}
fprintf(f,"f %d %d %d\r\n",2*n*p+1,2*n*p+2,2*n*p+3);
fprintf(f,"f %d %d %d\r\n",2*n*p+1,2*n*p+4,2*n*p+3);
fprintf(f,"f %d %d %d\r\n",2*n*p+5,2*n*p+6,2*n*p+7);
fprintf(f,"f %d %d %d\r\n",2*n*p+5,2*n*p+8,2*n*p+7);

fclose(f);

if(!(f=fopen("loop.col","w"))){printf("Could not open file 'loop.col'\r\n"); exit(1);}
fprintf(f,"%d\r\n",4*p+1);

for(i=1;i<=2*p;i++){
  j=(int)(150.0*sin((float)i*PI/(2.0*(float)p)));
  fprintf(f,"%d %d %d %d %d\r\n",(i-1)*m+1,i*m-4,70-(int)(0.1*j),70-(int)(0.1*j),70+(int)(1.2*j));
  fprintf(f,"%d %d 200 200 200\r\n",i*m-3,i*m);
}
fprintf(f,"%d %d 70 20 10",2*m*p+1,2*m*p+4);

fclose(f);

n=4; /*vertices*/
m=2; /*triangles*/

y[3]=y[2]; y[4]=y[1]; z[3]=z[4]=1.0;

tv[1][1]=1; tv[1][2]=2;  tv[1][3]=3;
tv[2][1]=1; tv[2][2]=4; tv[2][3]=3;

if(!(f=fopen("loop.ref","w"))){printf("Could not open file 'loop.ref'\r\n"); exit(1);}

dtt=2*PI/p;

fprintf(f,"%d %d\r\n",n*p+8,m*p+4);

for(i=1;i<=p;i++){
  for(j=1;j<=n;j++){
    ttij=(i-1+z[j])*dtt; rij=r-x[j];
    fprintf(f,"%1.4f %1.4f %1.4f\r\n",r-rij*cos(ttij),y[j]+s*w*ttij/(2*PI),rij*sin(ttij));
  }
}
fprintf(f,"-5.50 %1.2f 0.00\r\n",0.5*s*w-5.0);
fprintf(f,"-5.50 %1.2f 0.00\r\n",0.5*s*w-17.0);
fprintf(f,"1.50 %1.2f 0.00\r\n",0.5*s*w-17.0);
fprintf(f,"1.50 %1.2f 0.00\r\n",0.5*s*w-5.0);
fprintf(f,"-5.50 %1.2f 0.00\r\n",0.5*s*w+17.0);
fprintf(f,"-5.50 %1.2f 0.00\r\n",0.5*s*w+5.0);
fprintf(f,"1.50 %1.2f 0.00\r\n",0.5*s*w+5.0);
fprintf(f,"1.50 %1.2f 0.00\r\n",0.5*s*w+17.0);

for(i=1;i<=p;i++){
  for(j=1;j<=m;j++){
    fprintf(f,"f %d %d %d\r\n",(i-1)*n+tv[j][1],(i-1)*n+tv[j][2],(i-1)*n+tv[j][3]);
  }
}
fprintf(f,"f %d %d %d\r\n",n*p+1,n*p+2,n*p+3);
fprintf(f,"f %d %d %d\r\n",n*p+1,n*p+4,n*p+3);
fprintf(f,"f %d %d %d\r\n",n*p+5,n*p+6,n*p+7);
fprintf(f,"f %d %d %d\r\n",n*p+5,n*p+8,n*p+7);

fclose(f);

if(!(f=fopen("loop.cld","w"))){printf("Could not open file 'loop.cld'\r\n"); exit(1);}
fprintf(f,"0\r\n");
fclose(f);

printf("Files written\r\n");

return 0;}
