#include <stdio.h>
#include <stdlib.h>
#include <time.h>


/*initialize random numbers*/
void inrnd()
{int i,t,r;
t=time(NULL); if(t<0){t=-t;} t=t%16384;
for(i=1;i<=t;i++){
  r=rand();
}
}


int irand1(int b)
{int a;
a=rand();
b=b-5+a%11;
if(b>255){b=255;}else{if(b<0){b=0;}}
return b;}


int main()
{int i,j,n,tc1,tc2,red,green,blue;
char name[30];
FILE *f1,*f2;

inrnd();

printf("Input file (*.col): "); scanf("%s",name);
if(!(f1=fopen(name,"r"))){printf("Could not open '%s'\r\n",name);}
printf("Output file (*.col): "); scanf("%s",name);
if(!(f2=fopen(name,"w"))){printf("Could not open '%s'\r\n",name);}

fscanf(f1,"%d",&n);
for(i=1;i<=n;i++){
  fscanf(f1,"%d %d %d %d %d",&tc1,&tc2,&red,&green,&blue);
}
rewind(f1);

fprintf(f2,"%d\r\n",tc2);

fscanf(f1,"%d",&n);

for(i=1;i<=n;i++){
  fscanf(f1,"%d %d %d %d %d",&tc1,&tc2,&red,&green,&blue);
  for(j=tc1;j<=tc2;j++){
    fprintf(f2,"%d %d %d %d %d\r\n",j,j,irand1(red),irand1(green),irand1(blue));
  }
}

fclose(f1); fclose(f2);

return 0;}
