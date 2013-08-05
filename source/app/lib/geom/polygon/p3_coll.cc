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

#include "p3_coll.h"
#include "../../tool_os/memman.h"

int l3d_polygon_3d_collidable::contains_point(const l3d_point& point) {
  int collindex[2];
  int inside_side; // normal clockwise=1, reversed ccw=-1

  if(l3d_abs(plane.a) > l3d_abs(plane.b) &&
      l3d_abs(plane.a) > l3d_abs(plane.c))
  {
    //- smallest variation in the a (x) direction (largest normal component);
    //- drop it
    collindex[0] = 2; //- z axis becomes new 2D x axis
    collindex[1] = 1; //- y axis becomes new 2D y axis
    //- NOTE: look at LHS system. drop x. you must use "y" as the vertical
    //- axis and "z" as horiz. in order for left-right orientation to stay
    //- the same.
    // (for the same reason the one rotation matrix looks weird
    // with a negated term)

    //- if the dir. of the normal vector component you dropped is positive,
    //- the orientation of the orthogonally-projected poly is clockwise
    //- (the normal case for a LHS coordinate system) else counterclockwise.
    //- BUT!! side_of_point assumes SCREEN-SPACE coords, i.e. +ve y axis is
    //- DOWN, but here we are working in 3D space with one axis dropped,
    //- giving an orthographically projected point with +ve y axis going UP.
    //- so we flip our interpretation of "inside" and "outside" from the
    //- result returned by side_of_point: we treat -1 as inside for normal
    //- clockwise polys, and treat +1 as inside for reversed counterclockwise
    //- polys.

    if(plane.a > 0) inside_side = -1; else inside_side = 1;
  } else if(l3d_abs(plane.b) > l3d_abs(plane.a) &&
            l3d_abs(plane.b) > l3d_abs(plane.c) )
  {
    //- smallest variation in the b (y) direction (largest normal component);
    //- drop it
    collindex[0] = 0; //- x axis becomes new 2D x axis
    collindex[1] = 2; //- z axis becomes new 2D y axis

    //- if the dir. of the normal vector component you dropped is positive,
    //- the orientation of the orthogonally-projected poly is clockwise
    //- (the normal case for a LHS coordinate system) else counterclockwise.
    //- BUT!! side_of_point assumes SCREEN-SPACE coords, i.e. +ve y axis is
    //- DOWN, but here we are working in 3D space with one axis dropped,
    //- giving an orthographically projected point with +ve y axis going UP.
    //- so we flip our interpretation of "inside" and "outside" from the
    //- result returned by side_of_point: we treat -1 as inside for normal
    //- clockwise polys, and treat +1 as inside for reversed counterclockwise
    //- polys.
    if(plane.b > 0) inside_side = -1; else inside_side = 1;

  } else {
    //- smallest variation in the c (z) direction (largest normal component);
    //- drop it

    collindex[0] = 0; //- x axis becomes new 2D x axis
    collindex[1] = 1; //- y axis becomes new 2D y axis

    //- here, because the +ve z-axis is pointing into the screen, then
    //- if plane.c > 0 then the polygon is pointing AWAY from the
    //- viewer, meaning that the orientation of the polygon is reversed
    //- (i.e. clockwise), so normally inside_side would be -1. BUT,
    //- due to the reversed-y system expected by side_of_point(),
    //- inside_side gets negated again, giving 1.
    if(plane.c > 0) inside_side = 1; else inside_side = -1;
  }

  int idx0, idx1;
  idx0 = 0;
  idx1 = next_clipidx_right(idx0, clip_ivertices->num_items);
  l3d_point ortho_point, ortho_p1, ortho_p2;

  ortho_point.set(point.a[collindex[0]],
                  point.a[collindex[1]],
                  int_to_l3d_real(0),
                  int_to_l3d_real(1));

  int status=1; // inside/outside status

  int done=0;
  while(!done) {
    ortho_p1.set
    ( (**vlist)[(*clip_ivertices)[idx0].ivertex].transformed.a[collindex[0]],
      (**vlist)[(*clip_ivertices)[idx0].ivertex].transformed.a[collindex[1]],
      int_to_l3d_real(0),
      int_to_l3d_real(1) );
    ortho_p2.set
    ( (**vlist)[(*clip_ivertices)[idx1].ivertex].transformed.a[collindex[0]],
      (**vlist)[(*clip_ivertices)[idx1].ivertex].transformed.a[collindex[1]],
      int_to_l3d_real(0),
      int_to_l3d_real(1) );

    if ( side_of_point(&ortho_point, &ortho_p1, &ortho_p2) != inside_side ) {
      status = 0;
      done = 1;
    }

    if(idx1==0) {
      done = 1;
    } else {
      idx0 = idx1;
      idx1 = next_clipidx_right(idx0, clip_ivertices->num_items);
    }
  }

  return status;
}

l3d_polygon_2d* l3d_polygon_3d_collidable::clone(void) {
  return new l3d_polygon_3d_collidable(*this);
}

l3d_polygon_3d_collidable::l3d_polygon_3d_collidable
(const l3d_polygon_3d_collidable &r)
    : l3d_polygon_3d_clippable(r),
    l3d_collidable(r)
{
}
