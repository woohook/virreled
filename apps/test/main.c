//gcc -o testapp -lX11 main.c
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>

Display *g_display = 0;
int      g_screen  = 0;
Window   g_window  = 0;

int g_keepRunning = 1;

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

void input_initialize()
{
  XSetWindowAttributes attributes;
  attributes.event_mask = KeyPressMask | KeyReleaseMask;

  XChangeWindowAttributes(g_display, g_window, CWEventMask, &attributes);
}

void input_processEvent(XEvent event)
{
  switch(event.type) {
    case KeyPress: {
      KeySym key;
      XLookupString (&event.xkey,NULL,0,&key,0);
      switch (key) {
        case 'q':
          g_keepRunning = 0;
          break;
        default:
          break;
      }
      break;
    }
    default: {
      break;
    }
  }
}

void input_processFrame()
{
  XEvent event;
  while (XPending(g_display)) {
    XNextEvent(g_display,&event);
    input_processEvent(event);
  }
}

void input_deinitialize()
{
}

int main(int argc, char** argv)
{
  window_initialize();
  input_initialize();

  while (g_keepRunning) {
    input_processFrame();
    window_processFrame();
  }

  input_deinitialize();
  window_deinitialize();

  return 0;
}
