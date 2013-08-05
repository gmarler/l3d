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

#ifndef __P3_COLL_H
#define __P3_COLL_H
#include "../../tool_os/memman.h"

#define __ACTIVE__P3_COLL_H

#include "p3_clip.h"
#include "../plane/plane.h"
#include "../../dynamics/collide.h"

class l3d_polygon_3d_collidable :
      virtual public l3d_polygon_3d_clippable,
      virtual public l3d_collidable
{
  public:
  l3d_polygon_3d_collidable(void) : l3d_polygon_3d_clippable() {};
    // this constructor is called as a base class constructor by derived
    // classes to get an "empty" l3d_polygon which is then initialized
    // within the derived class's constructor

    l3d_polygon_3d_collidable(int num_pts) :
    l3d_polygon_3d(num_pts),
    l3d_polygon_2d(num_pts),
    l3d_polygon_3d_clippable(num_pts)
  {} ;
    // this constructor is called by clients to get a "real" l3d_polygon,
    // creating the vertices and all

    int contains_point(const l3d_point &point);

    l3d_polygon_3d_collidable(const l3d_polygon_3d_collidable &r);
    l3d_polygon_2d *clone(void);
};

#undef __ACTIVE__P3_COLL_H

#endif
