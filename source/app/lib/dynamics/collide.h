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

#ifndef __COLLIDE_H
#define __COLLIDE_H

#include "../tool_os/memman.h"
#include "../math/vector.h"
#include "../geom/point/point.h"

class l3d_collidable {
  public:
    l3d_point old_position, new_position;
    //  l3d_point collision;
    l3d_vector bounce;
    int prevent_collision;

    virtual int detect_collision_with(l3d_collidable *target) { return 0; };
};


#endif
