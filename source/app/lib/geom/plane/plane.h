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

#ifndef __PLANE_H
#define __PLANE_H
#include "../../tool_os/memman.h"

#include "../../system/sys_dep.h"
#include "../../math/vector.h"
#include "../point/point.h"
#include "../../dynamics/collide.h"

#define EPSILON_PLANE float_to_l3d_real(0.001)
//#define EPSILON_INTERSECT_DENOM float_to_l3d_real(0.0)
#define CROSS_FRONT_TO_BACK 1
#define CROSS_BACK_TO_FRONT -1


class l3d_plane
{
  public:
    l3d_real a,b,c,d;
    void align_with_point_normal(const l3d_point& point,
                                 const l3d_vector& normal);
    // the expected input normal is a normalized offset from the origin, not
    // an absolute 3d-world location of the "tip" of the vector

    void align_with_points(const l3d_point& p0,
                           const l3d_point& p1,
                           const l3d_point& p2);
    // the expected input points are evaluated such that the plane normal is
    // computed by crossing p1-p0 is with p2-p0 (this determines
    // the direction of the normal and must be considered when passing
    // in the points, otherwise if the points come in a random order,
    // the normal will also point randomly either one way or the other)

    l3d_real side_of_point(const l3d_point& p) const {
      return (l3d_mulrr(a,p.X_) +
              l3d_mulrr(b,p.Y_) +
              l3d_mulrr(c,p.Z_) +
              d);
    }

    l3d_real intersection_t;
    l3d_point intersection;
    int intersect_with_segment(const l3d_point& p0,
                               const l3d_point& p1);
};

#endif
