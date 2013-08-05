// L3D realtime 3D library, explained in book "Linux 3D Graphics Programming"
// Copyright (C) 2000  Norman Lin
// Contact: nlin@linux3dgraphicsprogramming.org (alt. nlin@geocities.com)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.

#include "ch2_app_xevent.h"
#include <stdio.h>
#include <stdlib.h>

const char *ch2_app_xevent::event_name(int e_type) const {
  static char text[80];

  switch (e_type) {

      //- event types from X11.h

    case KeyPress : sprintf(text, "KeyPress"); break;
    case KeyRelease : sprintf(text, "KeyRelease"); break;
    case ButtonPress : sprintf(text, "ButtonPress"); break;
    case ButtonRelease : sprintf(text, "ButtonRelease"); break;
    case MotionNotify : sprintf(text, "MotionNotify"); break;
    case EnterNotify : sprintf(text, "EnterNotify"); break;
    case LeaveNotify : sprintf(text, "LeaveNotify"); break;
    case FocusIn : sprintf(text, "FocusIn"); break;
    case FocusOut : sprintf(text, "FocusOut"); break;
    case KeymapNotify : sprintf(text, "KeymapNotify"); break;
    case Expose : sprintf(text, "Expose"); break;
    case GraphicsExpose : sprintf(text, "GraphicsExpose"); break;
    case NoExpose : sprintf(text, "NoExpose"); break;
    case VisibilityNotify : sprintf(text, "VisibilityNotify"); break;
    case CreateNotify : sprintf(text, "CreateNotify"); break;
    case DestroyNotify : sprintf(text, "DestroyNotify"); break;
    case UnmapNotify : sprintf(text, "UnmapNotify"); break;
    case MapNotify : sprintf(text, "MapNotify"); break;
    case MapRequest : sprintf(text, "MapRequest"); break;
    case ReparentNotify : sprintf(text, "ReparentNotify"); break;
    case ConfigureNotify : sprintf(text, "ConfigureNotify"); break;
    case ConfigureRequest : sprintf(text, "ConfigureRequest"); break;
    case GravityNotify : sprintf(text, "GravityNotify"); break;
    case ResizeRequest : sprintf(text, "ResizeRequest"); break;
    case CirculateNotify : sprintf(text, "CirculateNotify"); break;
    case CirculateRequest : sprintf(text, "CirculateRequest"); break;
    case PropertyNotify : sprintf(text, "PropertyNotify"); break;
    case SelectionClear : sprintf(text, "SelectionClear"); break;
    case SelectionRequest : sprintf(text, "SelectionRequest"); break;
    case SelectionNotify : sprintf(text, "SelectionNotify"); break;
    case ColormapNotify : sprintf(text, "ColormapNotify"); break;
    case ClientMessage : sprintf(text, "ClientMessage"); break;
    case MappingNotify : sprintf(text, "MappingNotify"); break;
    case LASTEvent : sprintf(text, "LASTEvent"); break;
  }

  return text;
}

ch2_app_xevent::~ch2_app_xevent(void) {
  if(screen) { delete [] screen; }
}

void ch2_app_xevent::create_window(void) {
  XPixmapFormatValues *pixmap_formats;
  int i, count;
  const int bits_per_byte = 8;

  //- establish connection to X server

  dpy = XOpenDisplay(NULL);


  // find out the supported depths of this server and take the default depth
  int* depths, numdepths;
  depths = XListDepths(dpy, DefaultScreen(dpy), &numdepths);
  depth = depths[0];
  for(i=0;i<numdepths;i++) {
    printf("got X depth %d\n", depths[i]);
  }
  XFree(depths);

  // get pixmap information about this depth
   pixmap_formats = XListPixmapFormats(dpy, &count);
   for(i=0; i<count; i++) {
     printf("getting depth %d\n",pixmap_formats[i].depth);
     if(pixmap_formats[i].depth == depth) {
       bytespp      = pixmap_formats[i].bits_per_pixel / 8; // 8 bits per byte
       scanline_pad = pixmap_formats[i].scanline_pad;
       break;
     }
   }
   XFree(pixmap_formats);


  printf("default depth of display %d\n", depth);
  printf("bytes per pixel: %d\n", bytespp);

  //- print out some information about the visual

  vis = DefaultVisual(dpy,0);
  switch(vis->c_class) {
    case PseudoColor: printf("Vis is pseudocolor\n");break;
    case StaticColor: printf("Vis is StaticColor\n");break;
    case GrayScale: printf("Vis is GrayScale\n");break;
    case StaticGray: printf("Vis is StaticGray\n");break;
    case DirectColor: printf("Vis is DirectColor\n");break;
    case TrueColor: printf("Vis is TrueColor\n");break;
  }

  //- create and map the window using max. possible depth

  w = XCreateWindow(dpy, DefaultRootWindow(dpy), 100, 100,
                    screen_xsize, screen_ysize,
                    0, // border
                    depth, CopyFromParent, vis, // depth, class, visual
                    0, NULL); // valuemask, attributes
  XStoreName(dpy, w,"hello_oo_event");
  XMapWindow(dpy, w);

  //- create XImage and offscreen buffer

  //sbegin create_screen
  screen = new char[screen_xsize*screen_ysize*bytespp];
  //send create_screen

  if (ImageByteOrder(dpy) == LSBFirst ) {
    printf("lsb first\n");
  } else {
    printf("msb first\n");
  }

  //sbegin create_ximage
  ximg = XCreateImage(dpy, vis, depth,
                      ZPixmap,   // format
                      0,  // offset from beg. of scanline
                      screen,
                      screen_xsize, screen_ysize,
                      scanline_pad,
                      0); // bytes_per_line: 0 = autocompute
  //send create_ximage

  XSetForeground(dpy, DefaultGC(dpy, 0), ~0);
  XSetBackground(dpy, DefaultGC(dpy, 0), 0);
}

void ch2_app_xevent::ask_to_be_notified_of_interesting_events(void) {
  XSelectInput(dpy, w, 0x00FFFFFF ^ PointerMotionHintMask);
}

void ch2_app_xevent::event_loop(void) {
  XEvent event;
  char ch;
  KeySym keysym;
  XComposeStatus xcompstat;

  int mouse_x, mouse_y;
  while(1) {
    XNextEvent(dpy, &event);
    char event_text[80];
    sprintf(event_text, "Event type %s              ", event_name(event.type));
    printf("%s\n", event_text);

    switch(event.type) {
      case KeyPress: {
          XLookupString(&event.xkey, &ch, 1, &keysym, &xcompstat);
          switch(ch) {
            case 'q': {
                exit(0);
              }
          }
        }
        break;

      case MotionNotify: {
          printf("x %d y %d\n", event.xmotion.x,event.xmotion.y);
          mouse_x = event.xmotion.x;
          mouse_y = event.xmotion.y;

          XEvent e;
          e.type = Expose;
          e.xexpose.send_event = TRUE;
          e.xexpose.display = dpy;
          e.xexpose.window = w;
          e.xexpose.x = e.xexpose.y = e.xexpose.width = e.xexpose.height =
                                        e.xexpose.count = 0;

          XSendEvent(dpy, w, TRUE, ExposureMask, &e);
        }
        break;

      case Expose: {
          if(event.xexpose.count==0) {
            XPutImage(dpy, w, DefaultGC(dpy,0), ximg,
                      0,0,0,0,  /* source x,y; destination x,y */
                      screen_xsize, screen_ysize);

            XDrawString(dpy, w, DefaultGC(dpy, 0), mouse_x, mouse_y, "hello!", 6);
          }
        }
        break;
    }

    XDrawImageString(dpy, w, DefaultGC(dpy, 0), 50, 50, event_text,
                     strlen(event_text));
  }
}
