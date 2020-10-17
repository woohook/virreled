// SPDX-License-Identifier: GPL-3.0-or-later

#include <X11/Xlib.h>
#include <GL/glx.h>

extern Display* g_display;
extern int      g_screen;
extern int      g_width;
extern int      g_height;
extern Window   g_window;
extern Visual*  g_visual;

extern unsigned long        g_windowAttributesMask;
extern XSetWindowAttributes g_windowAttributes;

XVisualInfo* g_visualinfo    = 0;
GLXContext   g_rendercontext = 0;

void render_initialize()
{
  int visualAttributes[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE,16,
                            GLX_RED_SIZE,4, GLX_GREEN_SIZE,4, GLX_BLUE_SIZE,4, None};
  g_visualinfo           = glXChooseVisual(g_display, g_screen, visualAttributes);
  g_visual               = g_visualinfo->visual;

  g_windowAttributes.colormap = XCreateColormap (g_display, RootWindow(g_display,g_screen),
                                                 g_visualinfo->visual, AllocNone);
  g_windowAttributesMask      |= CWColormap;

}

void render_processFrame()
{
  if(g_window == 0)
  {
    return;
  }

  if(g_rendercontext == 0)
  {
    g_rendercontext = glXCreateContext(g_display, g_visualinfo, 0, GL_TRUE);
    glXMakeCurrent(g_display, g_window, g_rendercontext);
    glViewport(0, 0, g_width, g_height);

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
  }

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
  glXSwapBuffers(g_display, g_window);
}

void render_deinitialize()
{
  glXDestroyContext(g_display, g_rendercontext);
}
