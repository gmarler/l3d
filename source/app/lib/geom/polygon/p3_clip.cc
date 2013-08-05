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

#include "p3_clip.h"
#include "../../tool_os/memman.h"

int l3d_polygon_3d_clippable::clip_to_plane(l3d_plane& clip_plane) {
  int idx0,idx1;
  int crossedge0_idx0,crossedge0_idx1,crossedge1_idx0,crossedge1_idx1;
  int newedge_ivertex0, newedge_ivertex1;
  int i;
  int dir;

  //     /* for each edge of the polygon, compare it with the clip plane.
  //      * there are 3 cases:
  //      * 1. both endpoints of edge > clip. keep edge.
  //      * 2. both endpoints of edge <=clip. discard edge.
  //      * 3. one endpoint of edge <=clip, the other > clip. clip edge.
  //      *    for convex polygons, if this occurs, it will occur with exactly
  //      *    two edges. we clip these two edges off and insert a new edge to
  //      *    "close" the polygon. we say <=clip and not <clip since this
  //      *    ensures that the edge to be clipped MUST be partially > clip.
  //      *
  //      * since the ivertex array is an ordered (clockwise, left-hand-rule)
  //      * list of vertices, the edges can be obtained by traversing this
  //      * array in order. since we know that IF there is a crossing, there will
  //      * be two crossings, we can use the following logic: start with 1st edge.
  //      * while no crossing, add edges to final edge list. if crossing, clip
  //      * crossed edge, loop until we find the other crossing, clip this edge,
  //      * add a new edge. if there are no crossings we just loop through the
  //      * whole array and add all vertices (edges).
  //      */

  idx0=0;
  idx1=next_clipidx_right(idx0, clip_ivertices->num_items);

  temp_clip_ivertices->num_items = 0;
  int extends_behind_plane = 0;

  // search for 1st crossing edge, meanwhile building up the new polygon
  while(
    !( (clip_plane.side_of_point((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed) > EPSILON_PLANE
        && clip_plane.side_of_point((**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed) < -EPSILON_PLANE)
       ||
       (clip_plane.side_of_point((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed) < -EPSILON_PLANE
        && clip_plane.side_of_point((**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed) > EPSILON_PLANE)))

  {
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
      (*clip_ivertices)[idx0].ivertex;
    idx0=idx1;
    if (clip_plane.side_of_point((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed) < -EPSILON_PLANE) { extends_behind_plane = 1; }

    // did we loop through all vertices without finding a plane cross?
    if(idx0==0) {

      // we found no edge cross. we now must determine if all vertices
      // are in front of or behind the plane. WE **CANNOT** make this
      // comparison based on just one vertex alone, due to the epsilon
      // parameter (the thick plane). if the current vertex lies within
      // the epsilon tolerance, it tells us nothing about the position of
      // the polygon w.r.t. the plane; the current vertex is in the grey
      // area. this is why we kept track of the extends_behind_plane variable
      // DURING the crossing edge search. if at ANY point we earlier found
      // a vertex which was definitely behind the plane, then
      // extends_behind_plane is set to 1. then, here, once we found
      // no edge crosses, we check to see if extends_behind_plane is 1.
      // if so, then since there was no edge cross, then all points are
      // either in the epsilon tolerance area or are behind the plane.
      // if ALL points are in the grey epsilon area, then
      // extends_behind_plane remains 0, and the points are assumed to
      // be in front of the plane.

      if (extends_behind_plane) {
        return 0;
      }

      // all points (and thus the current point) are >  plane...
      else
      {
        //            num_clip_pts = num_pts;
        // do nothing - the polygon remains unchanged, the changes in
        // temp_clip_ivertices simply remain until they are overwritten
        // by the next clipping operation.
        return 1;
      }

    }

    // still looking for the crossing edge
    idx1=next_clipidx_right(idx0, clip_ivertices->num_items);
  }

  //     printf("p3 clip: we found a crossing edge, I think. \npoint %f %f %f %f side %f \npoint %f %f %f %f side %f\nplane %f %f %f %f\ntolerance %f\n",
  //            (**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed.X_,
  //            (**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed.Y_,
  //            (**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed.Z_,
  //            (**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed.W_,
  //            clip_plane.side_of_point((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed),

  //            (**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed.X_,
  //            (**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed.Y_,
  //            (**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed.Z_,
  //            (**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed.W_,
  //            clip_plane.side_of_point((**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed),

  //            clip_plane.a,clip_plane.b,clip_plane.c,clip_plane.d,
  //            EPSILON_PLANE);

  if (clip_plane.side_of_point((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed) > EPSILON_PLANE
      && clip_plane.side_of_point((**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed) < -EPSILON_PLANE)
  { dir = CROSS_FRONT_TO_BACK; } else { dir = CROSS_BACK_TO_FRONT; }

  crossedge0_idx0 = idx0;
  crossedge0_idx1 = idx1;

  idx0=idx1;
  idx1=next_clipidx_right(idx0, clip_ivertices->num_items);

  // notice that this comparison for finding the second crossing edge
  // is slightly different than for finding the first crossing edge.
  // this comes from the fact that the plane is thick. to qualify as
  // a FIRST crossing edge, the logic is strict: it must pass FROM
  // one side of the thickness TO the other side of the thickness.
  // for the SECOND crossing edge, we are laxer: it must only pass BACK
  // TO the original side of the thickness. i.e. it doesn't have to pass
  // FROM the opposite side TO the original side; it can also pass from
  // WITHIN the thick area back to the original side. the only important
  // thing for the second crossing edge is that it must cross back.
  while(
    !( (dir==CROSS_FRONT_TO_BACK
        && clip_plane.side_of_point((**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed) > EPSILON_PLANE)
       ||
       (dir==CROSS_BACK_TO_FRONT
        && clip_plane.side_of_point((**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed) < -EPSILON_PLANE)
     )
  )
  {
    idx0=idx1;
    // did we loop through all vertices without finding 2nd plane cross?
    if(idx0==crossedge0_idx0) {
      fprintf(stderr,"shouldn't be here! can't find 2nd crossing edge\n");
      return 0;
    }

    // continue looking for 2nd plane cross
    idx1=next_clipidx_right(idx0, clip_ivertices->num_items);
  }

  crossedge1_idx0 = idx0;
  crossedge1_idx1 = idx1;

  int new_idx;
  clip_plane.intersect_with_segment(
    (**vlist)[(*clip_ivertices)[crossedge0_idx0].ivertex].transformed,
    (**vlist)[(*clip_ivertices)[crossedge0_idx1].ivertex].transformed);
  new_idx = newedge_ivertex0 = (*vlist)->next_varying_index();
  (**vlist)[ new_idx ].transform_stage = 0;
  (**vlist)[ new_idx ].original.X_ =
    (**vlist)[ new_idx ].transformed.X_ = clip_plane.intersection.X_;
  (**vlist)[ new_idx ].original.Y_ =
    (**vlist)[ new_idx ].transformed.Y_ = clip_plane.intersection.Y_;
  (**vlist)[ new_idx ].original.Z_ =
    (**vlist)[ new_idx ].transformed.Z_ = clip_plane.intersection.Z_;
  (**vlist)[ new_idx ].original.W_ =
    (**vlist)[ new_idx ].transformed.W_ = int_to_l3d_real(1);

  clip_plane.intersect_with_segment(
    (**vlist)[(*clip_ivertices)[crossedge1_idx0].ivertex].transformed,
    (**vlist)[(*clip_ivertices)[crossedge1_idx1].ivertex].transformed);
  new_idx = newedge_ivertex1 = (*vlist)->next_varying_index();
  (**vlist)[ new_idx ].transform_stage = 0;
  (**vlist)[ new_idx ].original.X_ =
    (**vlist)[ new_idx ].transformed.X_ = clip_plane.intersection.X_;
  (**vlist)[ new_idx ].original.Y_ =
    (**vlist)[ new_idx ].transformed.Y_ = clip_plane.intersection.Y_;
  (**vlist)[ new_idx ].original.Z_ =
    (**vlist)[ new_idx ].transformed.Z_ = clip_plane.intersection.Z_;
  (**vlist)[ new_idx ].original.W_ =
    (**vlist)[ new_idx ].transformed.W_ = int_to_l3d_real(1);

  {
    idx0=idx1; idx1=next_clipidx_right(idx0, clip_ivertices->num_items);
  }

  temp_clip_ivertices->num_items = 0;

  if(dir==CROSS_FRONT_TO_BACK) {
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
      newedge_ivertex1;

    for(i=crossedge1_idx1;
        i!=crossedge0_idx0;
        i=next_clipidx_right(i,clip_ivertices->num_items))
    {
      (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
        (*clip_ivertices)[i].ivertex;
    }
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
      (*clip_ivertices)[crossedge0_idx0].ivertex;

    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
      newedge_ivertex0;

    swap_temp_and_main_clip_lists();
  } else {
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
      newedge_ivertex0;

    for(i=crossedge0_idx1;
        i!=crossedge1_idx0;
        i=next_clipidx_right(i, clip_ivertices->num_items))
    {
      (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
        (*clip_ivertices)[i].ivertex;

    }
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
      (*clip_ivertices)[crossedge1_idx0].ivertex;

    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
      newedge_ivertex1;

    swap_temp_and_main_clip_lists();
  }

  return 1;
}

void l3d_polygon_3d_clippable::init_transformed(void) {
  l3d_polygon_3d::init_transformed();
  plane.align_with_point_normal(center.transformed,
                                normalized(sfcnormal.transformed- center.transformed));

}

void l3d_polygon_3d_clippable::transform(const l3d_matrix &m, int count) {
  l3d_polygon_3d::transform(m,count);
  plane.align_with_point_normal(center.transformed,
                                normalized(sfcnormal.transformed-center.transformed));
}

//  int l3d_polygon_3d_clippable::contains_point(const l3d_point& point) {
//    int collindex[2];
//    int inside_side; // normal clockwise=1, reversed ccw=-1

//    if(l3d_abs(plane.a) > l3d_abs(plane.b) &&
//       l3d_abs(plane.a) > l3d_abs(plane.c))
//    {
//      // smallest variation in a (x) direction (largest normal component);
//      // drop it
//      collindex[0] = 2; // z
//      collindex[1] = 1; // y
//      // NOTE: look at LHS system. drop x. you must use "y" as the vertical
//      // axis and "z" as horiz in order for left-right orientation to stay
//      // the same. (for the same reason the one rotation matrix looks weird
//      // with a negated term)

//     // if the dir of the normal vector component you dropped is positive,
//     // the orientation of the ortho-proj poly is ccw (normal) else cw
//     // BUT!! side_of_point assumes SCREEN-SPACE coords, i.e. +ve y axis is
//     // DOWN, but here we are working in 3D space with one axis dropped,
//      // giving an orthographically projected point with +ve y axis going UP.
//      // so we flip our interpretation of "inside" and "outside" from the
//      // result returned by side_of_point: we treat -1 as inside for normal ccw
//      // polys, and treat +1 as inside for reversed cw polys.

//      if(plane.a > 0) inside_side = -1; else inside_side = 1;
//    } else if(l3d_abs(plane.b) > l3d_abs(plane.a) &&
//  	    l3d_abs(plane.b) > l3d_abs(plane.c) )
//    {
//      collindex[0] = 0; // x
//      collindex[1] = 2; // z

//     // if the dir of the normal vector component you dropped is positive,
//     // the orientation of the ortho-proj poly is ccw (normal) else cw
//     // BUT!! side_of_point assumes SCREEN-SPACE coords, i.e. +ve y axis is
//     // DOWN, but here we are working in 3D space with one axis dropped,
//      // giving an orthographically projected point with +ve y axis going UP.
//      // so we flip our interpretation of "inside" and "outside" from the
//      // result returned by side_of_point: we treat -1 as inside for normal ccw
//      // polys, and treat +1 as inside for reversed cw polys.
//      if(plane.b > 0) inside_side = -1; else inside_side = 1;

//    } else {
//      collindex[0] = 0;
//      collindex[1] = 1;

//      // here, because the +ve z-axis is pointing into the screen,
//      // if plane.c > 0 then the polygon is pointing AWAY from the
//      // viewer, meaning that the orientation of the polygon is reversed
//      // (i.e. clockwise), so normally inside_side would be -1. BUT,
//      // due to the reversed-y system expected by side_of_point(),
//      // inside_side gets negated again, giving 1.
//      if(plane.c > 0) inside_side = 1; else inside_side = -1;
//    }

//    int idx0, idx1;
//    idx0 = 0;
//    idx1 = next_clipidx_right(idx0, clip_ivertices->num_items);
//    l3d_point ortho_point, ortho_p1, ortho_p2;

//    ortho_point.set(point.a[collindex[0]],
//  		  point.a[collindex[1]],
//  		  int_to_l3d_real(0),
//  		  int_to_l3d_real(1));

//    int status=1; // inside/outside status

//    int done=0;
//    while(!done) {
//      ortho_p1.set
//        ( (**vlist)[(*clip_ivertices)[idx0].ivertex].transformed.a[collindex[0]],
//  	(**vlist)[(*clip_ivertices)[idx0].ivertex].transformed.a[collindex[1]],
//  	int_to_l3d_real(0),
//  	int_to_l3d_real(1) );
//      ortho_p2.set
//        ( (**vlist)[(*clip_ivertices)[idx1].ivertex].transformed.a[collindex[0]],
//  	(**vlist)[(*clip_ivertices)[idx1].ivertex].transformed.a[collindex[1]],
//  	int_to_l3d_real(0),
//  	int_to_l3d_real(1) );

//      if ( side_of_point(&ortho_point, &ortho_p1, &ortho_p2) != inside_side ) {
//        status = 0;
//        done = 1;
//      }

//      if(idx1==0) {
//        done = 1;
//      } else {
//        idx0 = idx1;
//        idx1 = next_clipidx_right(idx0, clip_ivertices->num_items);
//      }
//    }

//    return status;
//  }

l3d_polygon_2d* l3d_polygon_3d_clippable::clone(void) {
  return new l3d_polygon_3d_clippable(*this);
}

l3d_polygon_3d_clippable::l3d_polygon_3d_clippable
(const l3d_polygon_3d_clippable &r)
    : l3d_polygon_3d(r)
    //    l3d_collidable(r)
{
  plane = r.plane;
}
