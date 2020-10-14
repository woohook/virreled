// SPDX-License-Identifier: GPL-3.0-or-later

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>

extern Display *g_display;
extern Window   g_window;

extern int g_keepRunning;

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
