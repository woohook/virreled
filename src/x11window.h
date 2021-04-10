// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>

Display* g_display    = NULL;
int      g_screen     = 0;
Visual*  g_visual     = 0;
Window   g_window     = 0;
XImage*  g_image      = 0;
GC       g_gfxContext = 0;

char* g_imagedata = 0;

XWindowAttributes g_windowAttributes;

void (*g_key_event_handler)(KeySym,int) = 0;

void _x11_display_open()
{
  g_display = XOpenDisplay(NULL);
  if(g_display == NULL)
  {
    fprintf(stderr, "XOpenDisplay failed\n");
    exit(1);
  }

  g_screen = DefaultScreen(g_display);

  g_visual = DefaultVisual(g_display, 0);
  if(g_visual->class != TrueColor)
  {
    fprintf(stderr, "Not a TrueColor visual.\n");
    exit(43);
  }
}

void _x11_display_close()
{
  XCloseDisplay(g_display);

  g_screen  = 0;
  g_display = NULL;
}

void window_create(int x, int y, unsigned int width, unsigned int height)
{
  const unsigned int border_width = 1;

  _x11_display_open();

  g_window = XCreateSimpleWindow(g_display, RootWindow(g_display, g_screen), x, y, width, height, border_width,
                                 BlackPixel(g_display, g_screen), WhitePixel(g_display, g_screen));

  XSelectInput(g_display, g_window, ExposureMask | KeyPressMask | KeyReleaseMask);

  g_gfxContext = XCreateGC(g_display, g_window, 0, 0);

  XGetWindowAttributes(g_display, g_window, &g_windowAttributes);
  g_imagedata = (char*)malloc(g_windowAttributes.width*g_windowAttributes.height*4);
  memset(g_imagedata, 255, g_windowAttributes.width*g_windowAttributes.height*4);
  g_image = XCreateImage(g_display, g_visual, DefaultDepth(g_display, g_screen), ZPixmap, 0, g_imagedata, g_windowAttributes.width, g_windowAttributes.height, 32, 0);

  XMapWindow(g_display, g_window);
  XSync(g_display, g_window);
}

void window_flush()
{
  XPutImage(g_display, g_window, g_gfxContext, g_image, 0, 0, 0, 0, g_windowAttributes.width, g_windowAttributes.height);

  XFlush(g_display);
}

// x11_set_pixel(x, y, ((red&255)<<16) + ((green&255)<<8) + (blue&255));
void window_set_pixel(int x, int y, int color)
{
  g_imagedata[(y*g_windowAttributes.width + x)*4 + 0] = color&255;
  g_imagedata[(y*g_windowAttributes.width + x)*4 + 1] = (color>>8)&255;
  g_imagedata[(y*g_windowAttributes.width + x)*4 + 2] = (color>>16)&255;
}

void window_destroy()
{
  XDestroyWindow(g_display, g_window);
  XDestroyImage(g_image);
  XFreeGC(g_display, g_gfxContext);
  XSync(g_display, 0);

  _x11_display_close();

  g_window = 0;
}

void window_set_key_handler(void (*key_event_handler)(KeySym,int))
{
  g_key_event_handler = key_event_handler;
}

void window_process_events()
{
  XEvent event;

  if(!g_display)
  {
    return;
  }

  while (XPending(g_display))
  {
    XNextEvent(g_display,&event);
    switch(event.type)
    {
      case KeyPress:
      case KeyRelease:
      {
        if(g_key_event_handler != NULL)
        {
          KeySym key;
          XLookupString (&event.xkey,NULL,0,&key,0);
          g_key_event_handler(key, (event.type == KeyPress) ? 1 : 0);
        }
        break;
      }
      default: {
        break;
      }
    }
  }
}
