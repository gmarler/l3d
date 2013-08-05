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

#ifndef __SC_X11SH_H
#define __SC_X11SH_H
#include "../tool_os/memman.h"

#include "sc_x11.h"
#include <X11/extensions/XShm.h>
#include <sys/shm.h>
#include <sys/ipc.h>

class l3d_screen_x11_shm : public l3d_screen_x11 {
  protected:
    XShmSegmentInfo shminfo;

  public:
    l3d_screen_x11_shm(int xsize, int ysize)
    : l3d_screen_x11(xsize,ysize) {}
    virtual ~l3d_screen_x11_shm(void);
    XImage *create_ximage(void);
    void create_buffer(void);
    void blit_screen(void);
};

class l3d_screen_factory_x11_shm : public l3d_screen_factory {
  public:
    l3d_screen *create(int xsize, int ysize) {
      l3d_screen_x11_shm *s;
      s = new l3d_screen_x11_shm(xsize,ysize);

      /* create offscreen buffer and ximage */

      s->ximg = s->create_ximage();
      s->create_buffer();

      return s;
    }
};

#endif
