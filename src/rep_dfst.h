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

#define MAXWLG 51

typedef struct _refpo
{int nfa; /*number of triangles (for rendering)*/
} refpo; /*reference points of an object type*/


typedef struct _sgob
{int otyp; /*number of object type in the global array*/
 int nref; /*number of reference points - max. MAXREF*/
 int nfa; /*number of triangles*/
 REALN radius; /*radius of sphere in which the object is included*/
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
int lev; /*level*/
} sgob; /*3d object*/


typedef struct _vhc
{int nob; /*number of objects in vehicle*/

int oid[MAXCTR]; /*number of object (rendering and collision) in 'objs' array from 'simcar.c'*/
int ofc[MAXCTR]; /*object function: 1-car; 2-passive wheel; 3-motor wheel; 4-steering wheel;
                    5-motor and steering wheel*/
REALN camh;
REALN caml;
REALN camd; /*camera position*/
} vhc; /*vehicle*/


tria **fceglob; /*array with triangles and colors of object types*/
refpo *refglob; /*array with reference points of object types*/

