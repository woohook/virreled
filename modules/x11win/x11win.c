// SPDX-License-Identifier: GPL-3.0-or-later

#include <X11/Xlib.h>

Display*             g_display              = 0;
int                  g_screen               = 0;
Visual*              g_visual               = CopyFromParent;
int                  g_width                = 320;
int                  g_height               = 240;
Window               g_window               = 0;
unsigned long        g_windowAttributesMask = 0;
XSetWindowAttributes g_windowAttributes;

void createMainWindow()
{
  g_window = XCreateWindow(g_display, RootWindow(g_display, g_screen),
                           50, 50, g_width, g_height, 0, CopyFromParent, InputOutput,
                           g_visual, g_windowAttributesMask, &g_windowAttributes);

  XMapWindow(g_display, g_window);
  XSync(g_display, g_window);
}

void window_initialize()
{
  g_windowAttributes.background_pixel = 127;
  g_windowAttributesMask |= CWBackPixel;

  g_display = XOpenDisplay(NULL);
  g_screen = DefaultScreen(g_display);
}

void window_processFrame()
{
  if(g_window == 0)
  {
    createMainWindow();
  }
  else
  {
    XSync (g_display,0);
  }
}

void window_deinitialize()
{
  XDestroyWindow(g_display, g_window);
  XSync(g_display, 0);
  XCloseDisplay(g_display);
  g_display = 0;
  g_window  = 0;
}
