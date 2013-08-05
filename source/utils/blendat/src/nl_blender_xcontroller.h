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

#ifndef __NL_BLENDER_XCONTROLLER_H
#define __NL_BLENDER_XCONTROLLER_H

#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <unistd.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <vector>
#include <string>

#include "nl_blender_controller.h"

class blender_config;

class blender_xcontroller : public blender_controller {
  protected:
    Display *dpy;
    int recursive_find_blender_window(Window w);

  public:
    Window blender_w;

    blender_xcontroller(Display *dpy, blender_config *c);
    virtual ~blender_xcontroller(void);
    void find_blender_window(void);
    void press_key(int code);
    void release_key(int code);
    void read_properties(void);
    void determine_selected_face_nums(void);
};

#endif
