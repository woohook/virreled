// SPDX-License-Identifier: GPL-3.0-or-later

#include <GL/gl.h>

extern int g_width;
extern int g_height;

float g_cam_x = 1.0;
float g_cam_y = 0.75;
float g_cam_z = 4.0;
float g_cam_rx = 0;
float g_cam_ry = 0;
float g_cam_rz = 0;

void model_render(float cam_x, float cam_y, float cam_z, float rotX, float rotY, float rotZ);

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

  glEnable (GL_DEPTH_TEST);
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_CULL_FACE);
  glCullFace (GL_BACK);
  glFrontFace (GL_CCW);

  model_render(g_cam_x, g_cam_y, g_cam_z, g_cam_rx, g_cam_ry, g_cam_rz);

  glFlush();
}

void scene_deinitialize()
{
}
