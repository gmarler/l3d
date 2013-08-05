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

#ifndef __SC_ZMESA_H
#define __SC_ZMESA_H
#include "../tool_os/memman.h"

#include "sc_mesa.h"

class l3d_screen_zbuf_mesa : public l3d_screen {
  public:
    l3d_screen_zbuf_mesa(int xsize, int ysize);
    void open_screen(void);
    void blit_screen(void);
    virtual ~l3d_screen_zbuf_mesa(void);
};

class l3d_screen_factory_zbuf_mesa : public l3d_screen_factory {
  public:
    l3d_screen *create(int xsize, int ysize) {
      return new l3d_screen_zbuf_mesa(xsize,ysize);
    }
};

inline void l3d_screen_zbuf_mesa::blit_screen(void) {
  glutSwapBuffers();
  glFlush();
}

#endif
