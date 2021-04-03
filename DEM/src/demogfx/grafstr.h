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

#define REALD float
/*float or double*/

#define MAXWLG 51
/*maximum word length*/

/*draw points and lines*/
void mdraw(SDL_Surface *screen,int x,int y,char s);

/*draws 1 character*/
void drawchar1(SDL_Surface *screen,float *coord,int x,int y,int i,int h,float caz);

/*draws fragment of character*/
void drawchaux(SDL_Surface *screen,int x,int y,char a,int h,float caz);

/*draws 1 character; needs drawchar1() to work*/
void drawchar(SDL_Surface *screen,int x,int y,int h,char a,float caz);

/*draws text*/
void dtext(SDL_Surface *screen,char *s,float x,float y,float h);



#if CLBITS==16
extern char textglob[7][MAXWLG];

typedef struct _pixcol
{Uint8 red;Uint8 green;Uint8 blue; /*culoarea pixelului*/
Uint16 bright; /*culoarea afisata*/
} pixcol;

typedef struct _tria
{REALN x1; REALN y1;REALN z1;
REALN x2; REALN y2;REALN z2;
REALN x3; REALN y3;REALN z3;
int red; int green; int blue; /*culoarea triunghiului*/
Uint16 bright; /*culoarea afisata*/
unsigned char cull; /*first bit(&1):1-cull;0-no cull;second bit(&2):1-fullbright;0-usual*/
} tria;

typedef struct _tripf
{REALN x1; REALN y1;
REALN x2; REALN y2;
REALN x3; REALN y3;
} tripf;

typedef struct _trilim
{int imin;int imax;} trilim;

typedef struct _lightpr
{REALN ambient;
REALN headlight;
REALN directional;
REALN dx;
REALN dy;
REALN dz;
} lightpr; /*light parameters*/

#else

extern char textglob[7][MAXWLG];

typedef struct _pixcol
{int red;int green;int blue; /*culoarea pixelului*/
} pixcol;

typedef struct _tria
{REALN x1; REALN y1;REALN z1;
REALN x2; REALN y2;REALN z2;
REALN x3; REALN y3;REALN z3;
int red; int green; int blue; /*culoarea triunghiului*/
int redd; int greend; int blued;
unsigned char cull; /*first bit(&1):1-cull;0-no cull;second bit(&2):1-fullbright;0-usual*/
} tria;

typedef struct _tripf
{REALN x1; REALN y1;
REALN x2; REALN y2;
REALN x3; REALN y3;
} tripf;

typedef struct _trilim
{int imin;int imax;} trilim;

typedef struct _lightpr
{REALN ambient;
REALN headlight;
REALN directional;
REALN dx;
REALN dy;
REALN dz;
} lightpr; /*light parameters*/
#endif

/*functie care elimina triunghiurile care sunt in plus*/
int fclip(tria *face,int nrfaces,REALN zmin,tria *facedisp,REALN zmax,REALN tgh,REALN tgv);

/*functie care verifica daca punctul M(x,y,z) se afla in interiorul triunghiului ABC
A(x1,y1,z1);B(x2,y2,z2);c(x3,y3,z3)
(punctul M apartine planului ABC)
nu se mai foloseste functia asta*/
int vertri(REALN x1,REALN y1,REALN x2,REALN y2,REALN x3,REALN y3,REALN x,REALN y);


/*functie care determina ecuatia unui plan care trece prin 3 puncte
A(x1,y1,z1);B(x2,y2,z2);c(x3,y3,z3)
dupa care determina punctul de intersectie al planului cu o dreapta care trece prin
O(0,0,0) si F(xf,yf,zf)
nu se mai foloseste functia asta ci findplan()*/
REALN ccplan(REALN x1,REALN y1,REALN z1,REALN x2,REALN y2,REALN z2,REALN x3,REALN y3,REALN z3,REALN xf,REALN yf,REALN zf);

/*functie care determina ecuatia planului cu numarul de ordine 'i'
dupa care determina punctul de intersectie al planului cu o dreapta care trece prin
O(0,0,0) si F(xf,yf,zf)
nrfaces - numar total de triunghiuri*/
void findplan(tria *face,int i,REALN *a,REALN *b,REALN *c,REALN *d);

void displaysdl(SDL_Surface *screen,tria *face,int nrfaces,REALN *distmin,unsigned int width,unsigned int height,REALN focal,pixcol backcol,REALN zfog,REALN zmax,lightpr *light);

typedef struct _sgob
{int otyp; /*number of object type in the global array*/
 int nfa; /*number of triangles*/
 REALN radius; /*radius of sphere in which the object is included*/
 REALN scale;
REALN xcen;
REALN ycen;
REALN zcen; /*coordinates of the object's centre in global coordinates*/
REALN xmin;
REALN ymin;
REALN zmin;
REALN xmax;
REALN ymax;
REALN zmax;
REALN vx[4];
REALN vy[4];
REALN vz[4]; /*directions of local axes in global coordinates - v[1]=i, v[2]=j, v[3]=k
               and origin of local system, in global coord. - v[0];
               in local coord. vx[1]=1, vy[1]=0, vz[1]=0; vx[2]=0, vy[2]=1, vz[2]=0 etc.
               rotation and translation of object only affect vx, vy, vz, ref. points
               and centres; triangles are calculated from vx, vy and vz at rendering*/
} sgob; /*3d object*/

extern tria **fceglob; /*array with triangles and colors of object types*/
extern int *nrfglob; /*array with numbers of triangles of object types*/


/*convert particles into graphical objects; return nr. of objects*/
int convpart(sgob *objs,tria *xface,int *nxf);

/*set camera position; look at z*/
void setcam(sgob *camera,REALN x,REALN y,REALN z);

/*function which displays the objcts which are closer than zmax
nob - total number of objects
cam - camera
xface - triangles not associated with any object
nxf - nr. of triangles in xface*/
void odis(SDL_Surface *screen,sgob *objs,int nob,tria *xface,int nxf,REALN zfog,REALN zmax,sgob *cam);



/*functie care returneaza 1 daca i se transmite un caracter de delimitare si 0 daca nu*/
int verdel(char s);

/*functie care returneaza 1 daca i se transmite o litera mica si 0 daca nu*/
int vercrl(char s);

/*functie care returneaza 1 daca i se transmite o cifra sau '.' si 0 daca nu*/
int vercrcf(char s);

/*functie care se uita sa vada ce e un sir de caractere: intreg(0), cuvant(1), real(2) sau altceva(3) ...*/
int detsir(char *s);

/*functie care citeste un cuvant s din fisierul fis; lincr - linia curenta*/
int fisgetw(FILE *fis,char *s,int *lincr);

/*functie care afiseaza un mesaj de eroare si iese din program daca e cazul*/
void afermex(char *numefis,int lincr,char *s,int cdvr);

int identcom(char *s);

/*functie care afla numarul de triunghiuri*/
int findnf(char *numefis);

/*functie care afla coordonatele varfurilor triunghiurilor*/
void faces(tria *face,char *numefis);

/*function which reads colors of triangles
*bred,*bgreen,*bblue - background color*/
void readcolor(tria *face,int nrfaces,char *numefis);

/*order vertices, so triangles can be culled correctly*/
void ordercl(tria *face,char *numefis);

/*function which finds the center and size of the object*/
void eval_obj(tria *face,sgob *objs);

/*function which reads the objects*/
void createtypes();



/*function which translates an object by x,y and z*/
void translat(sgob *objs,REALD x,REALD y,REALD z);

/*functie care roteste un obiect in jurul unei axe paralele cu z care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatz(sgob *objs,REALD x,REALD y,REALD tt);

/*functie care roteste un obiect in jurul unei axe paralele cu y care intersecteaza
planul xOz intr-un punct de coordonate x si z*/
void rotaty(sgob *objs,REALD x,REALD z,REALD tt);

/*functie care roteste toate triunghiurile in jurul unei axe paralele cu x care intersecteaza
planul xOy intr-un punct de coordonate x si y*/
void rotatx(sgob *objs,REALD y,REALD z,REALD tt);

/*functie care roteste un obiect in jurul unei axe oarecare care trece prin A1(x,y,z) si B(xb,yb,zb)*/
void rotab(sgob *objs,REALD x,REALD y,REALD z,REALD xb,REALD yb,REALD zb,REALD tt);










#define PI 3.1415926536

#define RCONTACTS 0
/*1-create contact structures; 0-don't (only for rendering)*/

#define FRICTIONVEL 0.001
/*minimum relative velocity needed to mobilize friction*/

#define ABS(_x) ((_x) < 0 ? -(_x) : (_x))


typedef struct _particle{
REALD radius;
REALD density;
REALD mass;
REALD im; /*inertial moment*/
REALD k; /*elastic coef. (N/m)*/
REALD mu; /*tangent friction coef.*/
REALD c; /*tangent damping coef.*/
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



/*GLOBAL VARIABLES---GLOBAL VARIABLES---GLOBAL VARIABLES---GLOBAL VARIABLES---GLOBAL VARIABLES---------*/
extern particle *DGLOBpart;
extern int DGLOBnrpart; /*nr. of particles*/
extern triangle *DGLOBtrstart;
