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

#include "plane.h"
#include "../../tool_os/memman.h"

void l3d_plane::align_with_point_normal(const l3d_point& point,
                                        const l3d_vector& normal)
{

  // this normal MUST BE NORMALIZED!!!! (for collision det.)

  a = normal.a[0];
  b = normal.a[1];
  c = normal.a[2];

  // based on the plane equation ax + by + cz + d=0, we have
  // d = -ax - by -cz

  d = -a * point.X_ - b * point.Y_ - c * point.Z_;
}

void l3d_plane::align_with_points(const l3d_point& p0,
                                  const l3d_point& p1,
                                  const l3d_point& p2)
{
  align_with_point_normal(p0, normalized(cross(p1-p0, p2-p0)));
}

int l3d_plane::intersect_with_segment(const l3d_point& p0,
                                      const l3d_point& p1)
{
  l3d_real t_top, t_bottom,
  dx, dy, dz;


  t_bottom = (l3d_mulrr(a,(dx=p1.X_ - p0.X_)) +
              l3d_mulrr(b,(dy=p1.Y_ - p0.Y_)) +
              l3d_mulrr(c,(dz=p1.Z_ - p0.Z_)));
  if(l3d_abs(t_bottom) <= 0)
  {
    return 0;
  }


  t_top = -1*(d +
              l3d_mulrr(a,p0.X_) +
              l3d_mulrr(b,p0.Y_) +
              l3d_mulrr(c,p0.Z_));

//    /*
//    if ( (t_top < 0 && t_bottom > 0) || (t_top > 0 && t_bottom < 0) ||
//         (l3d_abs(t_top) > l3d_abs(t_bottom) ) )
//  {
//      // t, the intersection time, must be positive (i.e. top/bottom must
//      // have same sign) and must be 0<t<1 (i.e. !top>bottom)
//      return 0;
//  }
//    */

  intersection_t = l3d_divrr(t_top, t_bottom);

  intersection.X_ = l3d_mulrr(dx,intersection_t) + p0.X_;
  intersection.Y_ = l3d_mulrr(dy,intersection_t) + p0.Y_;
  intersection.Z_ = l3d_mulrr(dz,intersection_t) + p0.Z_;
  intersection.W_ = int_to_l3d_real(1);

  // return the negative of the sign of t_top, i.e., return the
  // unaltered sign of the plane equation at p0. this is used by
  // the caller to see in which direction the intersection occurred.
  // (from p0 to p1 or from p1 to p0). if 0, no intersection occurred
  // and we jumped out of the routine earlier.

  return (t_top>0 ? CROSS_BACK_TO_FRONT : CROSS_FRONT_TO_BACK);
}

