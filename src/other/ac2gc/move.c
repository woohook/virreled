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
{char n1[30],n2[30];
float x,y,z,dx,dy,dz,xmin,ymin,zmin,xmax,ymax,zmax;
int nv,nt,i,v1,v2,v3;
FILE *f1,*f2;

printf("Input file (*.geo): "); scanf("%s",n1);
printf("Output file (*.geo): "); scanf("%s",n2);

if(!(f1=fopen(n1,"r"))){printf("Could not open '%s'\r\n",n1); exit(1);}
if(!(f2=fopen(n2,"w"))){printf("Could not open '%s'\r\n",n2); exit(1);}

fscanf(f1,"%d %d",&nv,&nt);

fscanf(f1,"%f %f %f",&x,&y,&z);
xmin=xmax=x; ymin=ymax=y; zmin=zmax=z;
for(i=2;i<=nt;i++){
  fscanf(f1,"%f %f %f",&x,&y,&z);
  if(x<xmin){xmin=x;}
  if(x>xmax){xmax=x;}
  if(y<ymin){ymin=y;}
  if(y>ymax){ymax=y;}
  if(z<zmin){zmin=z;}
  if(z>zmax){zmax=z;}
}
printf("\r\nx: %1.2f...%1.2f\r\ny: %1.2f...%1.2f\r\nz: %1.2f...%1.2f\r\n\r\n",xmin,xmax,ymin,ymax,zmin,zmax);

rewind(f1);

printf("dx: "); scanf("%f",&dx);
printf("dy: "); scanf("%f",&dy);
printf("dz: "); scanf("%f",&dz);

fscanf(f1,"%d %d",&nv,&nt);
fprintf(f2,"%d %d\r\n",nv,nt);

for(i=1;i<=nv;i++){
  fscanf(f1,"%f %f %f",&x,&y,&z);
  x+=dx; y+=dy; z+=dz;
  fprintf(f2,"%1.3f %1.3f %1.3f\r\n",x,y,z);
}

for(i=1;i<=nt;i++){
  fscanf(f1,"%s %d %d %d",n1,&v1,&v2,&v3);
  fprintf(f2,"f %d %d %d\r\n",v1,v2,v3);
}

fclose(f1); fclose(f2);
return 0;}
