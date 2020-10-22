// SPDX-License-Identifier: GPL-3.0-or-later

#include <GL/gl.h>

void model_load()
{
}

void model_render()
{
  glBegin (GL_TRIANGLES);

  glColor3f (1,0,0);
  glVertex3f (0,-0.1,0);
  glVertex3f (3,0,0);
  glVertex3f (0,+0.1,0);
  glColor3f (0.7,0,0);
  glVertex3f (0,0,-0.1);
  glVertex3f (3,0,0);
  glVertex3f (0,0,+0.1);

  glColor3f (0,1,0);
  glVertex3f (-0.1,0,0);
  glVertex3f (0,3,0);
  glVertex3f (+0.1,0,0);
  glColor3f (0,0.7,0);
  glVertex3f (0,0,-0.1);
  glVertex3f (0,3,0);
  glVertex3f (0,0,+0.1);

  glColor3f (0,0,1);
  glVertex3f (0,-0.1,0);
  glVertex3f (0,0,3);
  glVertex3f (0,+0.1,0);
  glColor3f (0,0,0.7);
  glVertex3f (-0.1,0,0);
  glVertex3f (0,0,3);
  glVertex3f (+0.1,0,0);

  glEnd();
}
