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

#include "polygon.h"
#include <string.h>
#include "../../tool_os/memman.h"
// for memcpy

l3d_polygon_ivertex_items_factory *
l3d_polygon_ivertex_items_factory::clone(void) {
  return new l3d_polygon_ivertex_items_factory;
}


//sbegin ctor_with_factory
l3d_polygon_2d::l3d_polygon_2d(int num_pts) {
  iv_items_factory = new l3d_polygon_ivertex_items_factory;
  init(num_pts);
}
//send ctor_with_factory

//sbegin init_lists_with_factory
void l3d_polygon_2d::init(int num_pts) {
  ivertices = new l3d_list<l3d_polygon_ivertex>
              (num_pts, iv_items_factory);
  clip_ivertices = new l3d_list<l3d_polygon_ivertex>
                   (num_pts, iv_items_factory);
  temp_clip_ivertices = new l3d_list<l3d_polygon_ivertex>
                        (num_pts, iv_items_factory);
}
//send init_lists_with_factory

l3d_polygon_2d::~l3d_polygon_2d(void) {
  //      printf("deleting ivertices %p in polygon_2d\n", ivertices);
  delete ivertices;// virtual destructor call
  //        printf("deleting clip_ivertices %p in polygon_2d\n", clip_ivertices);
  delete clip_ivertices;// virtual destructor call
  //        printf("deleting temp_clip_ivertices %p in polygon_2d\n", temp_clip_ivertices);
  delete temp_clip_ivertices;// virtual destructor call

  delete iv_items_factory;
}

void l3d_polygon_2d::clip_segment_to_edge_2d
(l3d_polygon_ivertex *ivertex,
 const l3d_point *pC0, const l3d_point *pC1,
 int crossedge_idx0,int crossedge_idx1)
{

  // redefine fixed point precision to avoid overflow during clipping calculation

#ifdef FIXED_POINT_MATH

#undef FX_PRECIS
#undef FX_PRECIS_QUOT
#undef FX_SIGN_MASK
#undef FX_ALLBITS_MASK
#undef FX_PRECIS_MASK
#undef FX_PRECIS_MAX
#undef l3d_fix_new_prec
#undef l3d_fix_norm_prec

#define FX_PRECIS 8
#define FX_PRECIS_QUOT 8

#define FX_SIGN_MASK    0x80000000L
#define FX_ALLBITS_MASK 0xFFFFFFFFL

  // binary: 0....0 0011 1111 1111 1111; accesses the precision part
#define FX_PRECIS_MASK 0x000000FFL
#define FX_PRECIS_MAX 256.0

#define l3d_fix_new_prec(x) ((x) >> (FX_PRECIS_DEFAULT-FX_PRECIS))
#define l3d_fix_norm_prec(x) ((x) << (FX_PRECIS_DEFAULT-FX_PRECIS))

#else

#define l3d_fix_new_prec(x) (x)
#define l3d_fix_norm_prec(x) (x)

#endif

  l3d_real t,x_intersect,y_intersect,denom;

  denom =
    l3d_mulrr(l3d_fix_new_prec
              ((**vlist)[ (*clip_ivertices)[crossedge_idx1].ivertex ].transformed.X_ -
               (**vlist)[ (*clip_ivertices)[crossedge_idx0].ivertex ].transformed.X_ )
              , // /* times */
              l3d_fix_new_prec(pC1->Y_ - pC0->Y_) )
    -
    l3d_mulrr(l3d_fix_new_prec
              ((**vlist)[ (*clip_ivertices)[crossedge_idx1].ivertex ].transformed.Y_ -
               (**vlist)[ (*clip_ivertices)[crossedge_idx0].ivertex ].transformed.Y_)
              , // /* times */
              l3d_fix_new_prec(pC1->X_ - pC0->X_ ) );

  //   if (denom > float_to_l3d_real(-0.01) && denom < float_to_l3d_real(0.01))
  //   if above line is used instead of ==0, polys disappear sometimes when
  //   clipped! for the same reason we use an epsilon for the "side_of_point"
  //   test.
  if (denom == 0) {
    printf("shouldnt be here: cannot clip segment to edge\n");
    ivertex->ivertex= -1;
    return;
  }


  t=
    l3d_divrr(l3d_mulrr(l3d_fix_new_prec(pC1->X_ - pC0->X_)
                        , // /* times */
                        l3d_fix_new_prec
                        ((**vlist)[(*clip_ivertices)[crossedge_idx0].ivertex].transformed.Y_
                         -pC0->Y_)
                       )
              +
              l3d_mulrr(l3d_fix_new_prec(pC1->Y_ - pC0->Y_)
                        , // /* times */
                        l3d_fix_new_prec(pC0->X_ -
                                         (**vlist)[(*clip_ivertices)[crossedge_idx0].ivertex].transformed.X_)
                       )


              , // /* divby */
              denom);

  //   if(t<0 || t>int_to_l3d_real(1)) return -1; // intersection is not on line (shoulnd't be here)

  x_intersect =
    (l3d_fix_new_prec
     ((**vlist)[(*clip_ivertices)[crossedge_idx0].ivertex].transformed.X_) )
    +
    l3d_mulrr(t,
              l3d_fix_new_prec
              ((**vlist)[(*clip_ivertices)[crossedge_idx1].ivertex].transformed.X_ -
               (**vlist)[(*clip_ivertices)[crossedge_idx0].ivertex].transformed.X_)
             );
  y_intersect =
    (l3d_fix_new_prec
     ((**vlist)[(*clip_ivertices)[crossedge_idx0].ivertex].transformed.Y_) )
    +
    l3d_mulrr(t,
              l3d_fix_new_prec
              ((**vlist)[(*clip_ivertices)[crossedge_idx1].ivertex].transformed.Y_ -
               (**vlist)[(*clip_ivertices)[crossedge_idx0].ivertex].transformed.Y_ )
             );

  int new_idx = (*vlist)->next_varying_index();
  (**vlist)[new_idx].original.X_ =
    (**vlist)[new_idx].transformed.X_ =
      l3d_fix_norm_prec(x_intersect);
  (**vlist)[new_idx].original.Y_ =
    (**vlist)[new_idx].transformed.Y_ =
      l3d_fix_norm_prec(y_intersect);

  // define precision back to normal
#include "../../math/fix_prec.h"

  ivertex->ivertex = new_idx;
  return;
}

int l3d_polygon_2d::clip_to_edge_2d
(const l3d_point *pPoint1, const l3d_point *pPoint2)
{
  int idx0,idx1;
  int crossedge0_idx0,crossedge0_idx1,crossedge1_idx0,crossedge1_idx1;
  int newedge_ivertex0, newedge_ivertex1;
  int i;
  l3d_polygon_ivertex new_ivertex0, new_ivertex1;

  // assumption: this polygon has ALREADY been clipped and is being
  // FURTHER clipped; i.e.:
  // assumption: this polygon has ALREADY had its clip_ivertices and its
  // num_clip_pts initialized (probably in 3D)

  int extends_outside_edge=0;
  idx0 = 0;
  idx1 = next_clipidx_right(idx0, clip_ivertices->num_items);
  //-  search for 1st crossing edge FROM outside idx0 TO inside idx1
  while( ! (side_of_point(&((**vlist)[(*clip_ivertices)[idx0].ivertex].transformed),
                          pPoint1, pPoint2) < 1
            &&
            side_of_point(&((**vlist)[(*clip_ivertices)[idx1].ivertex].transformed),
                          pPoint1, pPoint2) == 1 )
       )
  {
    //printf("%d %d\n",idx0,idx1);
    idx0=idx1;

    if(side_of_point
        (&((**vlist)[(*clip_ivertices)[idx0].ivertex].transformed),
         pPoint1,pPoint2) == -1 ) { extends_outside_edge = 1; }


    //- did we loop through all vertices without finding an edge cross?
    if(idx0 == 0) {
      //- all points (and thus the current point) are "outside"...

      // we found no edge cross. we now must determine if all vertices
      // are in front of or behind the edge. WE **CANNOT** make this
      // comparison based on just one vertex alone, due to the epsilon
      // parameter (the thick edge). if the current vertex lies within
      // the epsilon tolerance, it tells us nothing about the position of
      // the polygon w.r.t. the edge; the current vertex is in the grey
      // area. this is why we kept track of the extends_outside_edge variable
      // DURING the crossing edge search. if at ANY point we earlier found
      // a vertex which was definitely outside the edge, then
      // extends_outside_edge is set to 1. then, here, once we found
      // no edge crosses, we check to see if extends_outside_edge is 1.
      // if so, then since there was no edge cross, then all points are
      // either in the epsilon tolerance area or are outside the edge.
      // if ALL points are in the grey epsilon area, then
      // extends_outside_edge remains 0, and the points are assumed to
      // be in front of the edge.

      if(extends_outside_edge)
      {
        return 0;
      }

      //- all points (and thus the current point) are "inside"...
      else
      {
        return 1;
      }
    }

    //- still looking for the crossing edge
    idx1=next_clipidx_right(idx0, clip_ivertices->num_items);
  }

  crossedge0_idx0 = idx0;
  crossedge0_idx1 = idx1;

  idx0=idx1;
  idx1=next_clipidx_right(idx0, clip_ivertices->num_items);

  //- search for next crossing edge, FROM inside idx0 TO outside idx1
  while( !

         (
//  /*side_of_point(&((**vlist)[(*clip_ivertices)[idx0].ivertex].transformed),
//                 pPoint1,pPoint2) == 1
//                 && */
           side_of_point(&((**vlist)[(*clip_ivertices)[idx1].ivertex].transformed),
                         pPoint1,pPoint2) < 1 )
       )
  {
    //printf("%d %d\n",idx0,idx1);
    idx0=idx1;
    //- did we loop through all vertices without finding 2nd z-plane cross?
    if(idx0==crossedge0_idx0) {
      fprintf(stderr,"shouldn't be here! can't find 2nd crossing edge\n");
      return 0;
    }

    //- continue looking for 2nd cross
    idx1=next_clipidx_right(idx0, clip_ivertices->num_items);
  }

  crossedge1_idx0 = idx0;
  crossedge1_idx1 = idx1;

  clip_segment_to_edge_2d(&new_ivertex0,
                          pPoint1, pPoint2,
                          crossedge0_idx0,crossedge0_idx1);
  if ( (newedge_ivertex0 = new_ivertex0.ivertex) == -1)
  {
    //     printf("ints not on seg: bail\n");
    return 0; // intersection not on segment! bail out...
  }

  clip_segment_to_edge_2d(&new_ivertex1,
                          pPoint1, pPoint2,
                          crossedge1_idx0,crossedge1_idx1);
  if ( (newedge_ivertex1 = new_ivertex1.ivertex) == -1 )
  {
    //     printf("ints not on seg: bail\n");
    return 0; // intersection not on segment! bail out...
  }

  temp_clip_ivertices->num_items = 0;
  (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
    newedge_ivertex0;

  for(i=crossedge0_idx1;
      i!=crossedge1_idx0;
      i=next_clipidx_right(i,clip_ivertices->num_items))
  {
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
      (*clip_ivertices)[i].ivertex;
  }

  (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
    (*clip_ivertices)[crossedge1_idx0].ivertex;
  (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
    newedge_ivertex1;

  swap_temp_and_main_clip_lists();

  return 1;
}

//  /*****************************************************************************
//   * clip polygon against another polygon: both must be convex
//   *****************************************************************************/
int l3d_polygon_2d::clip_to_polygon_2d(const l3d_polygon_2d *pClipPoly)
{
  int idx0,idx1;

  // assumption: this polygon has ALREADY been clipped and is being
  // FURTHER clipped; i.e.:
  // assumption: this polygon has ALREADY had its clip_ivertices and its
  // num_clip_pts initialized. typically the first clip will be in 3d,
  // then several 2d clips against all portals

  if (   (clip_ivertices->num_items == 0)  // polygon is empty
         || (pClipPoly->clip_ivertices->num_items == 0) // clip poly is empty
     )
  {
    return 0;
  }

  idx0 = pClipPoly->clip_ivertices->num_items-1;
  idx1 = 0;
  while(1) {
    if(!clip_to_edge_2d(&((**(pClipPoly->vlist)) [
                            (*pClipPoly->clip_ivertices)[idx0].ivertex
                          ].transformed),
                        &((**(pClipPoly->vlist)) [
                            (*pClipPoly->clip_ivertices)[idx1].ivertex
                          ].transformed)) )
    {
      clip_ivertices->num_items=0;
      return 0;
    }

    idx0 = idx1;
    if(idx0==pClipPoly->clip_ivertices->num_items-1) break;
    idx1++;
  }

  return 1;
}

l3d_polygon_2d::l3d_polygon_2d(const l3d_polygon_2d &r) {

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
}

l3d_polygon_2d* l3d_polygon_2d::clone(void) {
  return new l3d_polygon_2d(*this);
}
