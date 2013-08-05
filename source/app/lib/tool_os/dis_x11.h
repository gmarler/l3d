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

#ifndef _DIS_X11_H
#define _DIS_X11_H
#include "../tool_os/memman.h"

#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include "dispatch.h"
#include "../tool_2d/sc_x11.h"

class l3d_dispatcher_x11 : public l3d_dispatcher {
  public:
    void start(void);
};

class l3d_dispatcher_factory_x11 : public l3d_dispatcher_factory {
  public:
    l3d_dispatcher *create(void) {
      return new l3d_dispatcher_x11;
    }
};

#endif
