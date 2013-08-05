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

#ifndef __TEXLOAD_H
#define __TEXLOAD_H
#include "../../tool_os/memman.h"

#include "../../tool_2d/scrinfo.h"
#include <stdio.h>

class l3d_texture_loader {
  protected:
    l3d_screen_info *si;
  public:
    l3d_texture_loader(l3d_screen_info *si) {
      this->si = si;
      data = NULL;
    }
    virtual ~l3d_texture_loader(void) {};
    // the texture loader is not responsible for destroying the loaded texture,
    // but instead the texture which called the loader

    virtual void load(const char *fname) = 0;
    unsigned char *data;
    int width, height;
};

#endif
