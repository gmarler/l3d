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

//sbegin header
#include <stdio.h>
#include <stdlib.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
//send header
//
//- empty line
//
//sbegin defines
#define SCREEN_XSIZE 320
#define SCREEN_YSIZE 200
//send defines

main() {
  //sbegin display_var
  Display *dpy;           // X11: Display (connection to X server)
  //send display_var
  //
  //- establish connection to X server

  //sbegin open_display
  dpy = XOpenDisplay(NULL);
  //send open_display

  //- create and map (display) an X window for output

  //sbegin win_var
  Visual *vis;            // X11: Visual (visual info about X server)
  Window w;               // X11: Window
  //send win_var
  //sbegin default_visual
  vis = DefaultVisual(dpy,0);
  //send default_visual
  //sbegin create_window
  w = XCreateWindow(dpy,                        //- display
                    DefaultRootWindow(dpy),     //- parent
                    100, 100,                   //- x, y position
                    SCREEN_XSIZE, SCREEN_YSIZE, //- width, height
                    0,                          //- border width
                    CopyFromParent,             //- depth (we use max. possible)
                    CopyFromParent,             //- visual class (TrueColor etc)
                    vis,                        //- visual
                    0, NULL);                   //- valuemask, window attributes
  XStoreName(dpy, w, "hello");
  //send create_window

  //sbegin map_window
  XMapWindow(dpy, w);
  //send map_window

  //sbegin select_input
  XSelectInput(dpy, w, KeyPressMask);
  //send select_input

  //sbegin main_loop
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
//send main_loop
