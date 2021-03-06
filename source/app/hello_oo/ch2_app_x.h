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

#ifndef __CH2_APP_X_H
#define __CH2_APP_X_H
#include "ch2_app.h"
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>

class ch2_app_x : public ch2_app {
  protected:
    const static int screen_xsize = 320;
    const static int screen_ysize = 200;
    Visual *vis;            // X11: Visual (visual info about X server)
    Display *dpy;           // X11: Display (connection to X server)
    Window w;               // X11: Window

    void create_window(void); // virtual
    void ask_to_be_notified_of_interesting_events(void); // virtual
    void event_loop(void); // virtual

  public:
  virtual ~ch2_app_x(void) {};
};
#endif
