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

#ifndef __P_TEX_H
#define __P_TEX_H
#include "../../tool_os/memman.h"

#include "poly3.h"
#include "../../raster/rast3.h"
#include "../texture/texture.h"

class l3d_polygon_ivertex_textured : public l3d_polygon_ivertex {
  public:
    l3d_vector tex_coord;
    virtual ~l3d_polygon_ivertex_textured(void) {printf("iv_tex dest\n");}
};

class l3d_polygon_3d_textured : public l3d_polygon_3d {
  public:
    l3d_polygon_3d_textured(int num_pts) :
    l3d_polygon_3d() { init(num_pts); }

    // override the factory methods to create textured ivertices
    virtual l3d_polygon_ivertex *create_ivertices(int count);
    virtual size_t sizeof_ivertex(void);

    l3d_texture *texture;

    void init_transformed(void);
    void transform(const l3d_matrix &m, int count);
    void draw(l3d_rasterizer_3d *r ) {
      r->draw_polygon_textured(this);
    }
};

FIXME DONT USE THIS FILE

#endif













