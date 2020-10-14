// SPDX-License-Identifier: GPL-3.0-or-later

#include <X11/Xlib.h>

Display *g_display = 0;
int      g_screen  = 0;
Window   g_window  = 0;

void window_initialize()
{
  g_display = XOpenDisplay(NULL);
  g_screen = DefaultScreen(g_display);

  g_window = XCreateSimpleWindow(g_display, RootWindow(g_display, g_screen),50,50,300,200,0,4,127);

  XMapWindow(g_display, g_window);
  XSync(g_display, g_window);
}

void window_processFrame()
{
  XSync (g_display,0);
}

void window_deinitialize()
{
  XDestroyWindow(g_display, g_window);
  XSync(g_display, 0);
  XCloseDisplay(g_display);
  g_display = 0;
  g_window  = 0;
}
