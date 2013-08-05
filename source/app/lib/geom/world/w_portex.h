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

#ifndef __W_PORTEX_H
#define __W_PORTEX_H
#include "../../tool_os/memman.h"

#include "w_portal.h"
#include "../texture/tl_ppm.h"

class l3d_world_portal_textured : public l3d_world_portal {
  protected:
    l3d_texture_loader *texture_loader;
    l3d_list<l3d_texture_data> tex_data;

  public:
    l3d_world_portal_textured(int xsize, int ysize);
    virtual ~l3d_world_portal_textured(void);
    void load_from_file(const char *fname); // virtual
};

#endif


