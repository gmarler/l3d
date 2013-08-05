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

#include "poly3.h"
#include <string.h>
#include "../../tool_os/memman.h"
// for memcpy

//sbegin init_xform
void l3d_polygon_3d::init_transformed(void) {
  sfcnormal.reset();
  center.reset();
}
//send init_xform


//
// Compute surface normal of the given l3d_polygon
// Facet's center must already be computed! (facet_compute_center)
//
// Since facets are stored in clockwise order (left hand rule), we
// compute the outward-pointing surface normal as v0v1 cross v0v2.
// Note that the vector v0v1 is computed as v1-v0, and v0v2 is computed as
// v2-v0. For instance the vector v0v1 from v0=(0,0,0) to v1=(5,6,7) is
// v1-v0 = (5,6,7)
//
//     v1*
//      /
//     /
//  v0*-.
//       `-v2*
//

//sbegin sfcnormal
void l3d_polygon_3d::compute_sfcnormal() {
  const l3d_real collinear_epsilon = float_to_l3d_real(0.999); // cos(1 deg)
  int found_noncollinear_points = 0;
  int p0,p1,p2;
  for(p0=0; p0<ivertices->num_items; p0++) {
    p1 = p0 + 1;
    if(p1 >= ivertices->num_items) { p1 = 0; }
    p2 = p1 + 1;
    if(p2 >= ivertices->num_items) { p2 = 0; }

    l3d_vector v1 =
      (**vlist)[(*ivertices)[p1].ivertex].original -
      (**vlist)[(*ivertices)[p0].ivertex].original;

    l3d_vector v2 =
      (**vlist)[(*ivertices)[p2].ivertex].original -
      (**vlist)[(*ivertices)[p0].ivertex].original;

    // compute the surface normal. This result will be RELATIVE TO THE ORIGIN,
    // since the two vectors above are also relative to the origin (by
    // subtracting v1-v0 we get the vector, RELATIVE TO THE ORIGIN, from v0
    // to v1.) Perhaps it's clearer to say the vector is just "RELATIVE"; it
    // represents a displacement in a certain direction.

    // We then take the ORIGIN-RELATIVE surface normal vector just computed
    // and add it to the facet's center, so that the surface normal is
    // now defined RELATIVE TO THE FACET CENTER. WE MUST DO
    // THIS BECAUSE aFacet.sfcnormal GETS TRANSFORMED BY THE CAMERA
    // VIEWING MATRIX, JUST LIKE aFacet.center GETS TRANSFORMED BY THE
    // VIEWING MATRIX. If we did not translate aFacet.sfcnormal to be
    // relative to aFacet.center, then aFacet.sfcnormal after the viewing
    // transformation would be relative to the _origin_after_the_viewing_
    // _transformation_, which we could compute but which we do not.
    // It is more logical to define aFacet.sfcnormal relative to
    // aFacet.center, and to apply the camera transform accordingly to
    // both aFacet.center and aFacet.sfcnormal.
    //

    //- if angle between vectors is too close to 0 or 180 deg, they are
    //- almost in a straight line, so we choose the next 3 points
    if(l3d_abs(dot(normalized(v1),normalized(v2)))
        < int_to_l3d_real(collinear_epsilon))
    {
      sfcnormal.original = center.original + normalized(cross(v1,v2));
      found_noncollinear_points = 1;
    }
    // else {
    //       printf("rejecting noncollinear %f\n",
    //              l3d_abs(dot(normalized(v1),normalized(v2))));
    //
    // }
  }

  assert(found_noncollinear_points);
}
//send sfcnormal

//
// Compute center of the given facet by averaging X, Y, and Z coordinates
// for all vertices

//sbegin ctr
void l3d_polygon_3d::compute_center() {

  // note: this routine operates on the UNTRANSFORMED, UNCLIPPED
  // coordinates. Thus it is meant to be called at polygon creation time;
  // the center will then be transformed as needed as the polygon survives
  // culling tests.

  l3d_real xsum=0, ysum=0, zsum=0;
  register int i;

  for (i=0; i<ivertices->num_items; i++) {
    // note use of ivertices instead of clip_ivertices
    // note use of original instead of transformed
    xsum += (**vlist)[(*ivertices)[i].ivertex].original.X_;
    ysum += (**vlist)[(*ivertices)[i].ivertex].original.Y_;
    zsum += (**vlist)[(*ivertices)[i].ivertex].original.Z_;
  }

  center.original.set(
    l3d_divri(xsum, i),
    l3d_divri(ysum, i),
    l3d_divri(zsum, i),
    int_to_l3d_real(1) );
}
//send ctr


// FIXME FIXME common functions!

void l3d_polygon_3d::clip_segment_to_near_z
(l3d_polygon_ivertex *ivertex, l3d_real zclip, int idx0, int idx1)
{
  l3d_real x0,y0,z0,  x1,y1,z1,  t,  clip_x, clip_y, clip_z;

  // x0,y0,z0 behind zclip; x1,y1,z1 in front of zclip

  assert((**vlist)[ (*ivertices)[idx1].ivertex ].transformed.W_ == int_to_l3d_real(1));
  assert((**vlist)[ (*ivertices)[idx0].ivertex ].transformed.W_ == int_to_l3d_real(1));

  if((**vlist)[ (*ivertices)[idx0].ivertex ].transformed.Z_ > zclip) {
    x0 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.X_;
    y0 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.Y_;
    z0 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.Z_;

    x1 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.X_;
    y1 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.Y_;
    z1 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.Z_;
  }
  else {
    x0 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.X_;
    y0 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.Y_;
    z0 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.Z_;

    x1 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.X_;
    y1 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.Y_;
    z1 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.Z_;
  }

  t = l3d_divrr( zclip-z0, z1-z0);

  clip_x = x0 + l3d_mulrr( x1-x0 , t );
  clip_y = y0 + l3d_mulrr( y1-y0 , t );
  clip_z = zclip; // could also be computed with same formula above as proof

  int new_idx = (*vlist)->next_varying_index();

  (**vlist)[ new_idx ].transform_stage = 0;
  (**vlist)[ new_idx ].original.X_ =
    (**vlist)[ new_idx ].transformed.X_ = clip_x;
  (**vlist)[ new_idx ].original.Y_ =
    (**vlist)[ new_idx ].transformed.Y_ = clip_y;
  (**vlist)[ new_idx ].original.Z_ =
    (**vlist)[ new_idx ].transformed.Z_ = clip_z;
  (**vlist)[ new_idx ].original.W_ =
    (**vlist)[ new_idx ].transformed.W_ = int_to_l3d_real(1);

  ivertex->ivertex = new_idx;
}

//
// clip facet against a given near z-plane, possibly adding new vertices
//

int l3d_polygon_3d::clip_near_z(l3d_real zclip) {
  int idx0,idx1;
  int crossedge0_idx0,crossedge0_idx1,crossedge1_idx0,crossedge1_idx1;
  int newedge_ivertex0, newedge_ivertex1;
  int i;
  int dir;
#define FIRST_CROSS_FRONT_TO_BACK 0
#define FIRST_CROSS_BACK_TO_FRONT 1

  l3d_polygon_ivertex new_ivertex0, new_ivertex1;

  // for each edge of the polygon, compare it with the z-clip plane.
  // there are 3 cases:
  // 1. both endpoints of edge > zclip. keep edge.
  // 2. both endpoints of edge <=zclip. discard edge.
  // 3. one endpoint of edge <=zclip, the other > zclip. clip edge.
  //    for convex polygons, if this occurs, it will occur with exactly
  //    two edges. we clip these two edges off and insert a new edge to
  //    "close" the polygon. we say <=zclip and not <zclip since this
  //    ensures that the edge to be clipped MUST be partially > zclip.
  //
  // since the ivertex array is an ordered (clockwise, left-hand-rule)
  // list of vertices, the edges can be obtained by traversing this
  // array in order. since we know that IF there is a crossing, there will
  // be two crossings, we can use the following logic: start with 1st edge.
  // while no crossing, add edges to final edge list. if crossing, clip
  // crossed edge, loop until we find the other crossing, clip this edge,
  // add a new edge. if there are no crossings we just loop through the
  // whole array and add all vertices (edges).
  //

  idx0=0;
  idx1=next_clipidx_right(idx0, clip_ivertices->num_items);

  temp_clip_ivertices->num_items = 0;

  // search for 1st crossing edge, meanwhile building up the new
  // polygon
  while(
    !( ((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed.Z_ >  zclip
        && (**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed.Z_ <=zclip)
       ||
       ((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed.Z_ <=zclip
        && (**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed.Z_ > zclip)))
  {
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
      (*clip_ivertices)[idx0].ivertex;
    idx0=idx1;

    // did we loop through all vertices without finding a z-plane cross?
    if(idx0==0) {

      // all points (and thus the current point) are <=clipz...
      if ((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed.Z_ <=zclip)
        return 0;

      // all points (and thus the current point) are >  clipz...
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
  if ((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed.Z_ > zclip
      && (**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed.Z_ <=zclip)
  { dir = FIRST_CROSS_FRONT_TO_BACK; } else { dir = FIRST_CROSS_BACK_TO_FRONT; }


  crossedge0_idx0 = idx0;
  crossedge0_idx1 = idx1;

  idx0=idx1;
  idx1=next_clipidx_right(idx0, clip_ivertices->num_items);

  while(
    !( ((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed.Z_ > zclip
        && (**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed.Z_ <=zclip)
       ||
       ((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed.Z_ <=zclip
        && (**vlist)[ (*clip_ivertices)[idx1].ivertex ].transformed.Z_ > zclip)))
  {
    idx0=idx1;
    // did we loop through all vertices without finding 2nd z-plane cross?
    if(idx0==crossedge0_idx0) {
      fprintf(stderr,"shouldn't be here! can't find 2nd crossing edge\n");
      return 0;
    }

    // continue looking for 2nd z-plane cross
    idx1=next_clipidx_right(idx0, clip_ivertices->num_items);
  }

  crossedge1_idx0 = idx0;
  crossedge1_idx1 = idx1;

  clip_segment_to_near_z(&new_ivertex0, zclip,
                         crossedge0_idx0,crossedge0_idx1);
  newedge_ivertex0 = new_ivertex0.ivertex;

  clip_segment_to_near_z(&new_ivertex1, zclip,
                         crossedge1_idx0,crossedge1_idx1);
  newedge_ivertex1 = new_ivertex1.ivertex;

  {
    idx0=idx1; idx1=next_clipidx_right(idx0, clip_ivertices->num_items);
  }

  temp_clip_ivertices->num_items = 0;

  if(dir==FIRST_CROSS_FRONT_TO_BACK) {
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

//sbegin transform
void l3d_polygon_3d::transform(const l3d_matrix &m, int count) {
  register int i;
  for(i=0; i<clip_ivertices->num_items; i++) {
    (**vlist)[ (*clip_ivertices)[i].ivertex ].transform(m, count);
  }

  // we don't bother to apply the "count" parameter to the sfcnormal and center
  // vectors since these are not shared components and thus cannot possibly
  // have already been transformed.

  sfcnormal.transform(m, count);
  center.transform(m, count);

  // descendants should also transform any other relevant information
  // such as surface or vertex normals

}
//send transform

void l3d_polygon_3d::set_transform_stage(int count) {
  register int i;
  for(i=0; i<clip_ivertices->num_items; i++) {
    (**vlist)[ (*clip_ivertices)[i].ivertex ].transform_stage = count;
  }

  sfcnormal.transform_stage = count;
  center.transform_stage = count;

  // descendants should also transform any other relevant information
  // such as surface or vertex normals

}


l3d_polygon_2d* l3d_polygon_3d::clone(void) {
  return new l3d_polygon_3d(*this);
}

l3d_polygon_3d::l3d_polygon_3d(const l3d_polygon_3d &r) {
  //  l3d_polygon_3d::l3d_polygon_3d(-1); // param is ignored anyway

  //  iv_items_factory = new l3d_polygon_ivertex_items_factory;
  //  ^^ error! why? what if the right hand side polygon has a different
  //     factory? ask the factory to clone itself, too!!!!!

  // iv_items_factory = r.iv_items_factory;
  // ^^ error! why? the new polygon must have ITS OWN COPY of the
  //    factory, not a reference to the old factory!

  iv_items_factory = r.iv_items_factory->clone();
  init(DEFAULT_IVERTICES_PER_POLY);

  vlist = r.vlist;
  *ivertices = *r.ivertices;
  *clip_ivertices = *r.clip_ivertices;
  *temp_clip_ivertices = *r.temp_clip_ivertices;

  sfcnormal = r.sfcnormal;
  center = r.center;
  zvisible = r.zvisible;
}

// virtual
void l3d_polygon_3d::after_camera_transform(void) {
}
