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

#include "ch2_app_x.h"
#include <stdio.h>
#include <stdlib.h>

void ch2_app_x::create_window(void) {

  // establish connection to X server

  dpy = XOpenDisplay(NULL);

  // create and map (display) an X window for output

  vis = DefaultVisual(dpy,0);
  w = XCreateWindow(dpy,                        // display
                    DefaultRootWindow(dpy),     // parent
                    100, 100,                   // x, y position
                    screen_xsize, screen_ysize, // width, height
                    0,                          // border width
                    CopyFromParent,             // depth (we use max. possible)
                    CopyFromParent,             // visual class (TrueColor etc)
                    vis,                        // visual
                    0, NULL);                   // valuemask, window attributes

  XMapWindow(dpy, w);
}

void ch2_app_x::ask_to_be_notified_of_interesting_events(void) {
  XSelectInput(dpy, w, KeyPressMask);
}

void ch2_app_x::event_loop(void) {
  XEvent event;
  char ch;
  KeySym keysym;
  XComposeStatus xcompstat;

  while(1) {
    if(XCheckWindowEvent(dpy,w,KeyPressMask,&event)) {
      XLookupString(&event.xkey, &ch, 1, &keysym, &xcompstat);
      switch(ch) {
        case 'q': {
            exit(0);
          }
      }
    }
  }
}
