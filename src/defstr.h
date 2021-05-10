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

typedef REALD T3REALD[3]; /*looks strange, but 'T3REALD x' is now 'REALD x[3]'*/

typedef struct _refpo
{int ntr; /*number of triangles (collision)*/
 int nfa; /*number of triangles (rendering)*/
/*REALD p1[MAXCTR][3];
REALD p2[MAXCTR][3]; p[0]=x, p[1]=y, p[2]=z
REALD p3[MAXCTR][3]; vertices of triangles (collision) in local system of object type*/
T3REALD *p1;
T3REALD *p2;
T3REALD *p3;
} refpo; /*reference points of an object type*/


typedef struct _sgob
{int otyp; /*number of object type in the global array (fceglob)*/
 int ntr; /*number triangles (collision)*/
 int nfa; /*number of triangles (rendering)*/
 REALN radius; /*radius of sphere in which the object is included*/
REALD xcen;
REALD ycen;
REALD zcen; /*coordinates of the object's centre in global coordinates*/
REALN xmin;
REALN ymin;
REALN zmin;
REALN xmax;
REALN ymax;
REALN zmax;
REALD vx[4];
REALD vy[4];
REALD vz[4]; /*directions of local axes in global coordinates - v[1]=i, v[2]=j, v[3]=k
               and origin of local system, in global coord. - v[0];
               in local coord. vx[1]=1, vy[1]=0, vz[1]=0; vx[2]=0, vy[2]=1, vz[2]=0 etc.
               rotation and translation of object only affect vx, vy, vz, ref. points
               and centres; triangles are calculated from vx, vy and vz at rendering*/
int lev; /*level*/
REALN mu; /*friction*/
} sgob; /*3d object*/


typedef struct _vhc
{int nob; /*number of objects in vehicle*/
int nj; /*number of joints*/

int oid[21]; /*number of object (rendering and collision) in 'objs' array from 'virreled.c'*/
int bid[21]; /*particles*/
joint *jid[21]; /*joints*/
REALD jax[21][3]; /*joint rotation points*/

int jfc[21]; /*function of object attached to joint i (see 'ofc' below)*/
int ofc[21]; /*function of object i: 1-car; 2-trailer; 3-motor wheel; 4-steering wheel;
                    5-motor and steering wheel; 6-passive wheel; 7-trailer wheel*/

int cmd_accelerate;
int cmd_brake;
int cmd_handbrake;
int cmd_turn;
int cmd_mode;

REALN accel;
REALN brake; /*acceleration and brake torques/wheel*/
REALN spring;
REALN damper; /*suspension coefficients*/

REALN drag;
REALN downforce; /*these are multiplied with (speed^2)*/

REALN camh;
REALN caml;
REALN camd; /*camera position*/

REALN vrx,vrxmr, /*rot. speed*/
      arx,arxmr, /*rot. acceleration*/
      speed,dspeed,rotspeed;
} vhc; /*vehicle*/


tria **fceglob; /*array with triangles and colors of object types*/
refpo *refglob; /*array with reference points of object types*/
