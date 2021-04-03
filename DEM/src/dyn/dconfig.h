#define MAXWLG 51
/*maximum word length*/

#define PI 3.1415926536

#define REALD float
/*float or double*/

#define RCONTACTS 0
/*1-create contact structures; 0-don't (only for rendering)*/

#define FRICTIONVEL 0.001
/*minimum relative velocity needed to mobilize friction*/

#define ROTFVEL 0.5
/*relative velocity needed to mobilize max. friction in joint*/

REALD GRAVX=-9.81;
REALD GRAVY=0.0;
REALD GRAVZ=0.0; /*gravity*/

void setGravity(REALD gx,REALD gy,REALD gz)
{GRAVX=gx; GRAVY=gy; GRAVZ=gz;}

#define ABS(_x) ((_x) < 0 ? -(_x) : (_x))
