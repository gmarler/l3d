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

#ifndef __VFRUST_H
#define __VFRUST_H
#include "../../tool_os/memman.h"
#include "../point/point.h"
#include "../plane/plane.h"
#include "../boundvol/boundsph.h"
#undef near
#undef far

class l3d_viewing_frustum {
  public:
    l3d_plane top, bottom, left, right, near, far;
    void create_from_points(const l3d_point & top_left,
                            const l3d_point & top_right,
                            const l3d_point & bot_right,
                            const l3d_point & bot_left,
                            const l3d_real  & near_z,
                            const l3d_real  & far_z,
                            const l3d_matrix& xform);
    int intersects_sphere(const l3d_bounding_sphere & sphere);
};

#endif
