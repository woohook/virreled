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


int main()
{int i,j,n,m;
float l,w,t,h,
      dl,dw,dh;
FILE *f;

printf("Length: "); scanf("%f",&l);
printf("Width: "); scanf("%f",&w);
printf("Thickness: "); scanf("%f",&t);
printf("Height of fence: "); scanf("%f",&h);
printf("n: "); scanf("%d",&n); dl=l/n; n++;
printf("m: "); scanf("%d",&m); dw=w/m; m++;
printf("Slope: "); scanf("%f",&dh); dh*=dl;

if(!(f=fopen("bridge.geo","w"))){printf("Could not open file 'bridge.geo'\r\n"); exit(1);}

fprintf(f,"%d %d\r\n",m*n+8,2*(m-1)*(n-1)+10);

for(i=1;i<=m;i++){
  for(j=1;j<=n;j++){
    fprintf(f,"%1.3f %1.3f %1.3f\r\n",(j-1)*dh,-0.5*w+(i-1)*dw,(j-1)*dl);
  }
}
fprintf(f,"%1.3f %1.3f %1.3f\r\n",-t,-0.5*w,0.0);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",-t+(n-1)*dh,-0.5*w,l);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",-t,0.5*w,0.0);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",-t+(n-1)*dh,0.5*w,l);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",h,-0.5*w,0.0);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",h+(n-1)*dh,-0.5*w,l);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",h,0.5*w,0.0);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",h+(n-1)*dh,0.5*w,l);


for(i=1;i<=(m-1);i++){
  for(j=1;j<=(n-1);j++){
    fprintf(f,"f %d %d %d\r\n",(i-1)*n+j,(i-1)*n+j+1,(i-1)*n+j+n+1);
    fprintf(f,"f %d %d %d\r\n",(i-1)*n+j,(i-1)*n+j+n,(i-1)*n+j+n+1);
  }
}
fprintf(f,"f %d %d %d\r\n",m*n+1,m*n+2,n);
fprintf(f,"f %d %d %d\r\n",m*n+1,1,n);
fprintf(f,"f %d %d %d\r\n",m*n+3,m*n+4,m*n);
fprintf(f,"f %d %d %d\r\n",m*n+3,(m-1)*n+1,m*n);

fprintf(f,"f %d %d %d\r\n",m*n+1,m*n+2,m*n+4);
fprintf(f,"f %d %d %d\r\n",m*n+1,m*n+3,m*n+4);

fprintf(f,"f %d %d %d\r\n",m*n+5,m*n+6,n);
fprintf(f,"f %d %d %d\r\n",m*n+5,1,n);
fprintf(f,"f %d %d %d\r\n",m*n+7,m*n+8,m*n);
fprintf(f,"f %d %d %d\r\n",m*n+7,(m-1)*n+1,m*n);


fclose(f);


if(!(f=fopen("bridge.col","w"))){printf("Could not open file 'bridge.col'\r\n"); exit(1);}

fprintf(f,"%d\r\n",(m-1)*(n-1)+2);

for(i=1;i<=(m-1);i++){
  for(j=1;j<=(n-1);j++){
    if((i+j)%2){fprintf(f,"%d %d 170 160 80\r\n",2*((i-1)*(n-1)+j-1)+1,2*((i-1)*(n-1)+j-1)+2);}
    else{fprintf(f,"%d %d 150 50 30\r\n",2*((i-1)*(n-1)+j-1)+1,2*((i-1)*(n-1)+j-1)+2);}
  }
}
fprintf(f,"%d %d 170 160 140\r\n",2*(m-1)*(n-1)+1,2*(m-1)*(n-1)+6);
fprintf(f,"%d %d 120 90 80\r\n",2*(m-1)*(n-1)+7,2*(m-1)*(n-1)+10);

fclose(f);


if(!(f=fopen("bridge.cld","w"))){printf("Could not open file 'bridge.cld'\r\n"); exit(1);}

fprintf(f,"1\r\n");
fprintf(f,"1 %d i %1.3f 0.0 %1.3f\r\n",2*(m-1)*(n-1)+6,0.5*(n-1)*dh-0.5*t,0.5*l);

fclose(f);


if(!(f=fopen("bridge.ref","w"))){printf("Could not open file 'bridge.ref'\r\n"); exit(1);}

fprintf(f,"12 8\r\n");
fprintf(f,"%1.3f %1.3f %1.3f\r\n",-t,-0.5*w,0.0);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",-t+(n-1)*dh,-0.5*w,l);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",-t+(n-1)*dh,0.5*w,l);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",-t,0.5*w,0.0);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",0.0,-0.5*w,0.0);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",(n-1)*dh,-0.5*w,l);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",(n-1)*dh,0.5*w,l);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",0.0,0.5*w,0.0);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",h,-0.5*w,0.0);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",h+(n-1)*dh,-0.5*w,l);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",h+(n-1)*dh,0.5*w,l);
fprintf(f,"%1.3f %1.3f %1.3f\r\n",h,0.5*w,0.0);

fprintf(f,"f 1 2 10\r\nf 1 9 10\r\nf 3 4 12\r\nf 3 11 12\r\n");
fprintf(f,"f 5 6 7\r\nf 5 8 7\r\nf 1 2 3\r\nf 1 4 3\r\n");

fclose(f);



return 0;}
