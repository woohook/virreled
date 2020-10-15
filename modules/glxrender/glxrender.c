// SPDX-License-Identifier: GPL-3.0-or-later

#include <X11/Xlib.h>
#include <GL/glx.h>

extern Display* g_display;
extern int      g_screen;
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
  }

  glClearColor (0.0,0.3,0.0,0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glFlush();
  glXSwapBuffers(g_display, g_window);
}

void render_deinitialize()
{
  glXDestroyContext(g_display, g_rendercontext);
}
