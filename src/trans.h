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

/*function which translates an object by x,y and z*/
void translat(sgob *objs,REALN x,REALN y,REALN z)
{int i;

  objs->xcen+=x;
  objs->ycen+=y;
  objs->zcen+=z;

  for(i=0;i<=3;i++){
    objs->vx[i]+=x;
    objs->vy[i]+=y;
    objs->vz[i]+=z;
  }
}

/*functie care roteste un obiect in jurul unei axe paralele cu z care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatz(sgob *objs,REALN x,REALN y,REALN tt)
{int i;
REALN xtm,sintt,costt;

sintt=sin(tt);costt=cos(tt);

  xtm=objs->xcen;
  objs->xcen=x+(objs->xcen-x)*costt-(objs->ycen-y)*sintt;
  objs->ycen=y+(objs->ycen-y)*costt+(xtm-x)*sintt;

  for(i=0;i<=3;i++){
    xtm=objs->vx[i];
    objs->vx[i]=x+(objs->vx[i]-x)*costt-(objs->vy[i]-y)*sintt;
    objs->vy[i]=y+(objs->vy[i]-y)*costt+(xtm-x)*sintt;
  }
}

/*functie care roteste un obiect in jurul unei axe paralele cu y care intersecteaza
planul xOz intr-un punct de coordonate x si z*/
void rotaty(sgob *objs,REALN x,REALN z,REALN tt)
{int i;
REALN xtm,sintt,costt;

sintt=sin(tt);costt=cos(tt);

  xtm=objs->xcen;
  objs->xcen=x+(objs->xcen-x)*costt+(objs->zcen-z)*sintt;
  objs->zcen=z+(objs->zcen-z)*costt-(xtm-x)*sintt;

  for(i=0;i<=3;i++){
    xtm=objs->vx[i];
    objs->vx[i]=x+(objs->vx[i]-x)*costt+(objs->vz[i]-z)*sintt;
    objs->vz[i]=z+(objs->vz[i]-z)*costt-(xtm-x)*sintt;
  }
}


/*functie care roteste toate triunghiurile in jurul unei axe paralele cu x care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatx(sgob *objs,REALN y,REALN z,REALN tt)
{int i;
REALN ytm,sintt,costt;

sintt=sin(tt);costt=cos(tt);

  ytm=objs->ycen;
  objs->ycen=y+(objs->ycen-y)*costt-(objs->zcen-z)*sintt;
  objs->zcen=z+(objs->zcen-z)*costt+(ytm-y)*sintt;

  for(i=0;i<=3;i++){
    ytm=objs->vy[i];
    objs->vy[i]=y+(objs->vy[i]-y)*costt-(objs->vz[i]-z)*sintt;
    objs->vz[i]=z+(objs->vz[i]-z)*costt+(ytm-y)*sintt;
  }
}


/*functie care roteste un obiect in jurul unei axe oarecare care trece prin A1(x,y,z) si B(xb,yb,zb)*/
void rotab(sgob *objs,REALN x,REALN y,REALN z,REALN xb,REALN yb,REALN zb,REALN tt)
{int i;
REALN xtm,
sinalf=0,cosalf=0,sinbt=0,cosbt=0,sintt,costt,
len2,len1, /*lungimea segmentului AB si a proiectiei lui in planul xOy*/
abx,aby,abz, /*lungimile proiectiilor segmentului AB pe cele 3 axe*/
thres=1e-5; /*valoare minima admisa pentru len1*/

abx=xb-x; aby=yb-y; abz=zb-z;
len1=sqrt(abx*abx+aby*aby);
len2=sqrt(abx*abx+aby*aby+abz*abz);

if(len1>thres){sinalf=aby/len1; cosalf=abx/len1;
	       sinbt=len1/len2; cosbt=abz/len2;
	      }else{if(abz<0){tt=-tt;}}

sintt=sin(tt);costt=cos(tt);

	if(len1>thres){
/*1 - rotire cu (-alfa) in jurul axei z*/
  xtm=objs->xcen;
  objs->xcen=x+(objs->xcen-x)*cosalf+(objs->ycen-y)*sinalf;
  objs->ycen=y+(objs->ycen-y)*cosalf-(xtm-x)*sinalf;

  for(i=0;i<=3;i++){
    xtm=objs->vx[i];
    objs->vx[i]=x+(objs->vx[i]-x)*cosalf+(objs->vy[i]-y)*sinalf;
    objs->vy[i]=y+(objs->vy[i]-y)*cosalf-(xtm-x)*sinalf;
  }
  
/*2 - rotire cu (-beta) in jurul axei y*/
  xtm=objs->xcen;
  objs->xcen=x+(objs->xcen-x)*cosbt-(objs->zcen-z)*sinbt;
  objs->zcen=z+(objs->zcen-z)*cosbt+(xtm-x)*sinbt;

  for(i=0;i<=3;i++){
    xtm=objs->vx[i];
    objs->vx[i]=x+(objs->vx[i]-x)*cosbt-(objs->vz[i]-z)*sinbt;
    objs->vz[i]=z+(objs->vz[i]-z)*cosbt+(xtm-x)*sinbt;
  }
	}
/*3 - rotire cu teta in jurul axei z*/
  xtm=objs->xcen;
  objs->xcen=x+(objs->xcen-x)*costt-(objs->ycen-y)*sintt;
  objs->ycen=y+(objs->ycen-y)*costt+(xtm-x)*sintt;

  for(i=0;i<=3;i++){
    xtm=objs->vx[i];
    objs->vx[i]=x+(objs->vx[i]-x)*costt-(objs->vy[i]-y)*sintt;
    objs->vy[i]=y+(objs->vy[i]-y)*costt+(xtm-x)*sintt;
  }

	if(len1>thres){
/*4 - rotire cu beta in jurul axei y*/
  xtm=objs->xcen;
  objs->xcen=x+(objs->xcen-x)*cosbt+(objs->zcen-z)*sinbt;
  objs->zcen=z+(objs->zcen-z)*cosbt-(xtm-x)*sinbt;

  for(i=0;i<=3;i++){
    xtm=objs->vx[i];
    objs->vx[i]=x+(objs->vx[i]-x)*cosbt+(objs->vz[i]-z)*sinbt;
    objs->vz[i]=z+(objs->vz[i]-z)*cosbt-(xtm-x)*sinbt;
  }

/*5 - rotire cu alfa in jurul axei z*/
  xtm=objs->xcen;
  objs->xcen=x+(objs->xcen-x)*cosalf-(objs->ycen-y)*sinalf;
  objs->ycen=y+(objs->ycen-y)*cosalf+(xtm-x)*sinalf;

  for(i=0;i<=3;i++){
    xtm=objs->vx[i];
    objs->vx[i]=x+(objs->vx[i]-x)*cosalf-(objs->vy[i]-y)*sinalf;
    objs->vy[i]=y+(objs->vy[i]-y)*cosalf+(xtm-x)*sinalf;
  }
	}
}
