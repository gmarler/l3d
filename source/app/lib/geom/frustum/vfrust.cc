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

#include "vfrust.h"
#include "../../tool_os/memman.h"

void l3d_viewing_frustum::create_from_points
(const l3d_point & top_left,
 const l3d_point & top_right,
 const l3d_point & bot_right,
 const l3d_point & bot_left,
 const l3d_real  & near_z,
 const l3d_real  & far_z,
 const l3d_matrix& xform)
{
  l3d_point eye(0,0,0,1);

  top.align_with_points(xform|eye, xform|top_right, xform|top_left);
  bottom.align_with_points(xform|eye, xform|bot_left, xform|bot_right);
  left.align_with_points(xform|eye, xform|top_left, xform|bot_left);
  right.align_with_points(xform|eye, xform|bot_right, xform|top_right);

  l3d_point p1, p2, p3;

  p1.set(int_to_l3d_real(0), int_to_l3d_real(0), near_z, int_to_l3d_real(1));
  p2.set(int_to_l3d_real(1), int_to_l3d_real(0), near_z, int_to_l3d_real(1));
  p3.set(int_to_l3d_real(0), int_to_l3d_real(1), near_z, int_to_l3d_real(1));
  near.align_with_points(xform|p1,xform|p2,xform|p3);

  p1.set(int_to_l3d_real(0), int_to_l3d_real(1), far_z, int_to_l3d_real(1));
  p2.set(int_to_l3d_real(1), int_to_l3d_real(0), far_z, int_to_l3d_real(1));
  p3.set(int_to_l3d_real(0), int_to_l3d_real(0), far_z, int_to_l3d_real(1));
  far.align_with_points(xform|p1,xform|p2,xform|p3);
}

int l3d_viewing_frustum::intersects_sphere(const l3d_bounding_sphere & sphere)
{
  return
    (
      sphere.distance_to_plane(top) > -sphere.radius &&
      sphere.distance_to_plane(bottom) > -sphere.radius &&
      sphere.distance_to_plane(left) > -sphere.radius &&
      sphere.distance_to_plane(right) > -sphere.radius &&
      sphere.distance_to_plane(near) > -sphere.radius &&
      sphere.distance_to_plane(far) > -sphere.radius
    );
};


