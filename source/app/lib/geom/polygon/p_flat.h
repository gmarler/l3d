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

#ifndef __P_FLAT_H
#define __P_FLAT_H
#include "../../tool_os/memman.h"

#include "polygon.h"

class l3d_polygon_2d_flatshaded :
      virtual public l3d_polygon_2d
{
  public:
    unsigned long final_color;

    l3d_polygon_2d_flatshaded(void) {}; // to get an empty polygon for derived
    // classes

    l3d_polygon_2d_flatshaded(int num_pts) :
    l3d_polygon_2d(num_pts) {}; // to get an initialized polygon

    void draw(l3d_rasterizer_2d *r) {
      r->draw_polygon_flatshaded(this);
    }

    l3d_polygon_2d_flatshaded(const l3d_polygon_2d_flatshaded &r);
    virtual l3d_polygon_2d *clone(void);

};

#endif
