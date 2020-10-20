// SPDX-License-Identifier: GPL-3.0-or-later

#include <GL/gl.h>

extern int g_width;
extern int g_height;

void scene_initialize()
{
}

void scene_processFrame()
{
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  const float angle  = 0.8f;
  const float ratio  = (float)g_height/(float)g_width;
  const float near   = 0.1f;
  const float far    = 100.0f;
  const float left   = -angle * near;
  const float right  =  angle * near;
  const float bottom = -angle * ratio * near;
  const float top    =  angle * ratio * near;
  glFrustum(left, right, bottom, top, near, far);

  glClearColor (0.0,0.3,0.0,0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float cam_x     = 1.0;  // left
  float cam_y     = 0.75;  // up
  float cam_z     = -2.0;  // forward/back (+/-)

  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
  glRotatef (180, 0,1,0);  // look forward towards +z
  glTranslatef (-cam_x,-cam_y,-cam_z);

  glEnable (GL_DEPTH_TEST);
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_CULL_FACE);
  glCullFace (GL_BACK);
  glFrontFace (GL_CCW);

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

  glFlush();
}

void scene_deinitialize()
{
}
