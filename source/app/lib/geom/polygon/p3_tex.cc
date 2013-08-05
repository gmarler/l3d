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

#include "p3_tex.h"
#include "../../tool_os/memman.h"

l3d_polygon_ivertex_items_factory *
l3d_polygon_ivertex_textured_items_factory::clone(void) {
  return new l3d_polygon_ivertex_textured_items_factory;
}

void l3d_polygon_3d_textured::init_transformed(void) {
  l3d_polygon_3d::init_transformed();

  texture->O.reset();
  texture->U.reset();
  texture->V.reset();

  plane.align_with_point_normal(center.transformed,
                                normalized(sfcnormal.transformed
                                           - center.transformed));

}

void l3d_polygon_3d_textured::transform(const l3d_matrix &m, int count) {
  l3d_polygon_3d::transform(m, count);

  // we don't bother to apply the "count" parameter to the O,U, and V
  // vectors since these are not shared components and thus cannot possibly
  // have already been transformed.
  texture->O.transform(m, count);
  texture->U.transform(m, count);
  texture->V.transform(m, count);

  plane.align_with_point_normal(center.transformed,
                                normalized(sfcnormal.transformed
                                           - center.transformed));
}

l3d_polygon_3d_textured::l3d_polygon_3d_textured(int num_pts) :
    l3d_polygon_2d(),
    l3d_polygon_3d()
{
  texture = NULL;
  if(iv_items_factory) { delete iv_items_factory; }
  iv_items_factory = new l3d_polygon_ivertex_textured_items_factory;
  init(num_pts); // uses the factory to create ivertices
}

l3d_polygon_3d_textured::~l3d_polygon_3d_textured(void) {
  if(texture) {
    delete texture;
  }
}

l3d_polygon_2d* l3d_polygon_3d_textured::clone(void) {
  return new l3d_polygon_3d_textured(*this);
}

l3d_polygon_3d_textured::l3d_polygon_3d_textured
(const l3d_polygon_3d_textured &r)
    : l3d_polygon_3d(r),
    l3d_polygon_2d(r)
{
  texture = new l3d_texture; // FIXME: this should actually be a call to a
  // virtual clone operation, in case derived texture objects are cloned

  assert(r.texture);
  *texture = *r.texture; // only copies texture orientation, not texture data
  // itself; pointer texture->tex_data still points to same tex_data
  // object
  texture->owns_tex_data = false;

  //    // the base class copy constructors have faithfully created copies
  //    // of this object based on their knowledge of what must be copied
  //    // from the original base class object to the new object.

  //    // unfortunately, the base classes don't know anything about
  //    // textured ivertices, and thus could not copy these correctly.
  //    // they copied the ivertices, but into plain ivertex lists, not
  //    // textured ivertex lists. since we are in a constructor here,
  //    // unfortunately the base class constructors are called BEFORE
  //    // we can (in this derived class) change the factory to be a
  //    // textured ivertex factory. This means we have to let the base
  //    // class constructors do their (partially incorrect) work first;
  //    // only then does the derived class constructor get a chance to
  //    // correct the work be recopying the ivertices into textured ivertex
  //    // lists. we could simply avoid copying the ivertices completely and
  //    // completely reduplicate the base class copying operation here, but
  //    // this leads to the danger that changes in the base class copy semantics
  //    // would need to be maintained here also - and who is able to look
  //    // in all derived class when a base class changes? so, we take the safer
  //    // but slower route here of correcting the work afterwards. since
  //    // objects won't normally be copied during execution of the main program
  //    // logic (which is time-critical), this inefficiency can be tolerated.

  //    int i;
  //    for(i=0; i<ivertices->num_items; i++) {

  //    }
  //    if(iv_items_factory) { delete iv_items_factory; }
  //    iv_items_factory = new l3d_polygon_ivertex_textured_items_factory;
  //    init(num_pts); // uses the factory to create ivertices

}



void l3d_polygon_3d_textured::clip_segment_to_edge_2d
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
#ifdef FFIX
  l3d_fixed ui_intersect,vi_intersect,zi_intersect;
#else
l3d_real ui_intersect,vi_intersect,zi_intersect;
#endif

  denom =
    l3d_mulrr(l3d_fix_new_prec
              ((**vlist)[ (*clip_ivertices)[crossedge_idx1].ivertex ].transformed.X_ -
               (**vlist)[ (*clip_ivertices)[crossedge_idx0].ivertex ].transformed.X_ )
              , /* times */
              l3d_fix_new_prec(pC1->Y_ - pC0->Y_) )
    -
    l3d_mulrr(l3d_fix_new_prec
              ((**vlist)[ (*clip_ivertices)[crossedge_idx1].ivertex ].transformed.Y_ -
               (**vlist)[ (*clip_ivertices)[crossedge_idx0].ivertex ].transformed.Y_)
              , /* times */
              l3d_fix_new_prec(pC1->X_ - pC0->X_ ) );

  //   if (denom > float_to_l3d_real(-0.01) && denom < float_to_l3d_real(0.01))
  //   if above line is used instead of ==0, polys disappear sometimes when
  //   clipped! for the same reason we use an epsilon for the "side_of_point"
  //   test.
  if (denom == 0) {
    ivertex->ivertex= -1;
    return;
  }


  t=
    l3d_divrr(l3d_mulrr(l3d_fix_new_prec(pC1->X_ - pC0->X_)
                        , /* times */
                        l3d_fix_new_prec
                        ((**vlist)[(*clip_ivertices)[crossedge_idx0].ivertex].transformed.Y_
                         -pC0->Y_)
                       )
              +
              l3d_mulrr(l3d_fix_new_prec(pC1->Y_ - pC0->Y_)
                        , /* times */
                        l3d_fix_new_prec(pC0->X_ -
                                         (**vlist)[(*clip_ivertices)[crossedge_idx0].ivertex].transformed.X_)
                       )


              , /* divby */
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

#define Z_ADD_FACTOR -0.42
  // see ras3_sw.cc for derivation of Z_ADD_FACTOR
#ifdef FFIX
  l3d_fixed ui_start, ui_end, vi_start, vi_end, zi_start, zi_end;
  zi_start = fixfixdiv(
               int2fix(1),
               l3d_real_to_fixed(
                 ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx0]))
                 ->tex_coord.Z_) + float2fix(Z_ADD_FACTOR));
  zi_end = fixfixdiv(
             int2fix(1),
             l3d_real_to_fixed(
               ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx1]))
               ->tex_coord.Z_) + float2fix(Z_ADD_FACTOR));
  ui_start = fixfixmul(l3d_real_to_fixed(
                         ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx0]))
                         ->tex_coord.X_),
                       zi_start);
  ui_end = fixfixmul(l3d_real_to_fixed(
                       ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx1]))
                       ->tex_coord.X_),
                     zi_end);
  vi_start = fixfixmul(l3d_real_to_fixed(
                         ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx0]))
                         ->tex_coord.Y_),
                       zi_start);
  vi_end = fixfixmul(l3d_real_to_fixed(
                       ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx1]))
                       ->tex_coord.Y_),
                     zi_end);

  ui_intersect = ui_start + fixfixmul(l3d_real_to_fixed(t), ui_end - ui_start);
  vi_intersect = vi_start + fixfixmul(l3d_real_to_fixed(t), vi_end - vi_start);
  zi_intersect = zi_start + fixfixmul(l3d_real_to_fixed(t), zi_end - zi_start);


  l3d_fixed newu,newv,newz;
  newz = fixfixdiv(int2fix(1),zi_intersect);
  newu = fixfixmul(ui_intersect,newz);
  newv = fixfixmul(vi_intersect,newz);
  newz = newz - float2fix(Z_ADD_FACTOR);

  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.X_ = fixed_to_l3d_real(newu);
  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.Y_ = fixed_to_l3d_real(newv);
  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.Z_ = fixed_to_l3d_real(newz);
  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.W_ =
    int_to_l3d_real(1);
#else
  l3d_real ui_start, ui_end, vi_start, vi_end, zi_start, zi_end;

  zi_start = l3d_divrr(
               int_to_l3d_real(1),
               ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx0]))
               ->tex_coord.Z_);
  zi_end = l3d_divrr(
             int_to_l3d_real(1),
             ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx1]))
             ->tex_coord.Z_);
  ui_start = l3d_mulrr(
               ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx0]))
               ->tex_coord.X_,
               zi_start);
  ui_end = l3d_mulrr(
             ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx1]))
             ->tex_coord.X_,
             zi_end);
  vi_start = l3d_mulrr(
               ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx0]))
               ->tex_coord.Y_,
               zi_start);
  vi_end = l3d_mulrr(
             ((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx1]))
             ->tex_coord.Y_,
             zi_end);

  ui_intersect = ui_start + l3d_mulrr((t), ui_end - ui_start);
  vi_intersect = vi_start + l3d_mulrr((t), vi_end - vi_start);
  zi_intersect = zi_start + l3d_mulrr((t), zi_end - zi_start);


  l3d_real newu,newv,newz;
  newz = l3d_divrr(int_to_l3d_real(1),zi_intersect);
  newu = l3d_mulrr(ui_intersect,newz);
  newv = l3d_mulrr(vi_intersect,newz);

  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.X_ = (newu);
  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.Y_ = (newv);
  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.Z_ = (newz);
  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.W_ =
    int_to_l3d_real(1);
#endif


  /*
  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord =
     (((l3d_polygon_ivertex_textured *)&((*clip_ivertices)[crossedge_idx0]))
      ->tex_coord);

  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.Y_ = int_to_l3d_real(1);
  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.Z_ = int_to_l3d_real(1);
  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.W_ = 
    int_to_l3d_real(1);
  */

  return;
}

int l3d_polygon_3d_textured::clip_to_edge_2d
(const l3d_point *pPoint1, const l3d_point *pPoint2)
{
  int idx0,idx1;
  int crossedge0_idx0,crossedge0_idx1,crossedge1_idx0,crossedge1_idx1;
  int newedge_ivertex0, newedge_ivertex1;
  int i;
  l3d_polygon_ivertex_textured new_ivertex0, new_ivertex1;

  // assumption: this polygon has ALREADY been clipped and is being
  // FURTHER clipped; i.e.:
  // assumption: this polygon has ALREADY had its clip_ivertices and its
  // num_clip_pts initialized (probably in 3D)

  idx0 = 0;
  idx1 = next_clipidx_right(idx0, clip_ivertices->num_items);
  /* search for 1st crossing edge FROM outside idx0 TO inside idx1 */
  while( ! (side_of_point(&((**vlist)[(*clip_ivertices)[idx0].ivertex].transformed),
                          pPoint1, pPoint2) < 1
            &&
            side_of_point(&((**vlist)[(*clip_ivertices)[idx1].ivertex].transformed),
                          pPoint1, pPoint2) == 1 )
       )
  {
    //printf("%d %d\n",idx0,idx1);
    idx0=idx1;

    /* did we loop through all vertices without finding an edge cross? */
    if(idx0 == 0) {
      /* all points (and thus the current point) are "outside"... */
      if(side_of_point(&((**vlist)[(*clip_ivertices)[idx0].ivertex].transformed),
                       pPoint1,pPoint2) == -1 ) // not < 1 )
      {
        return 0;
      }

      /* all points (and thus the current point) are "inside"... */
      else
      {
        return 1;
      }
    }

    /* still looking for the crossing edge */
    idx1=next_clipidx_right(idx0, clip_ivertices->num_items);
  }

  crossedge0_idx0 = idx0;
  crossedge0_idx1 = idx1;

  idx0=idx1;
  idx1=next_clipidx_right(idx0, clip_ivertices->num_items);

  /* search for next crossing edge, FROM inside idx0 TO outside idx1 */
  while( !

         (/*side_of_point(&((**vlist)[(*clip_ivertices)[idx0].ivertex].transformed),
               pPoint1,pPoint2) == 1
               && */
           side_of_point(&((**vlist)[(*clip_ivertices)[idx1].ivertex].transformed),
                         pPoint1,pPoint2) < 1 )
       )
  {
    //printf("%d %d\n",idx0,idx1);
    idx0=idx1;
    /* did we loop through all vertices without finding 2nd z-plane cross? */
    if(idx0==crossedge0_idx0) {
      fprintf(stderr,"shouldn't be here! can't find 2nd crossing edge\n");
      return 0;
    }

    /* continue looking for 2nd z-plane cross */
    idx1=next_clipidx_right(idx0, clip_ivertices->num_items);
  }

  crossedge1_idx0 = idx0;
  crossedge1_idx1 = idx1;

  clip_segment_to_edge_2d(&new_ivertex0,
                          pPoint1, pPoint2,
                          crossedge0_idx0,crossedge0_idx1);
  if ( (newedge_ivertex0 = new_ivertex0.ivertex) == -1)
  {
    return 0; // intersection not on segment! bail out...
  }

  clip_segment_to_edge_2d(&new_ivertex1,
                          pPoint1, pPoint2,
                          crossedge1_idx0,crossedge1_idx1);
  if ( (newedge_ivertex1 = new_ivertex1.ivertex) == -1 )
  {
    return 0; // intersection not on segment! bail out...
  }

  int new_ivtx;

  temp_clip_ivertices->num_items = 0;
  (*temp_clip_ivertices)[new_ivtx=temp_clip_ivertices->next_index()] =
    new_ivertex0;

  for(i=crossedge0_idx1;
      i!=crossedge1_idx0;
      i=next_clipidx_right(i,clip_ivertices->num_items))
  {
    (*temp_clip_ivertices)[new_ivtx=temp_clip_ivertices->next_index()] =
      (*clip_ivertices)[i];



    //        printf("copying ivtx %d, new u v x y z %f %f %f %f %f\nold u v x y z %f %f %f %f %f\n",
    //               i,
    //               ((l3d_polygon_ivertex_textured *)&(*temp_clip_ivertices)[new_ivtx].ivertex)->tex_coord.X_,
    //               ((l3d_polygon_ivertex_textured *)&(*temp_clip_ivertices)[new_ivtx].ivertex)->tex_coord.Y_,
    //               (**vlist)[(*temp_clip_ivertices)[new_ivtx].ivertex].transformed.X_,
    //               (**vlist)[(*temp_clip_ivertices)[new_ivtx].ivertex].transformed.Y_,
    //               (**vlist)[(*temp_clip_ivertices)[new_ivtx].ivertex].transformed.Z_,
    //               ((l3d_polygon_ivertex_textured *)&(*clip_ivertices)[i].ivertex)->tex_coord.X_,
    //               ((l3d_polygon_ivertex_textured *)&(*clip_ivertices)[i].ivertex)->tex_coord.Y_,
    //               (**vlist)[(*clip_ivertices)[i].ivertex].transformed.X_,
    //               (**vlist)[(*clip_ivertices)[i].ivertex].transformed.Y_,
    //               (**vlist)[(*clip_ivertices)[i].ivertex].transformed.Z_);
  }


  (*temp_clip_ivertices)[new_ivtx=temp_clip_ivertices->next_index()].ivertex =
    (*clip_ivertices)[crossedge1_idx0].ivertex;
  ((l3d_polygon_ivertex_textured *)&(*temp_clip_ivertices)[new_ivtx].ivertex)->tex_coord =
    ((l3d_polygon_ivertex_textured *)&(*clip_ivertices)[crossedge1_idx0].ivertex)->tex_coord ;


  (*temp_clip_ivertices)[new_ivtx=temp_clip_ivertices->next_index()] =
    new_ivertex1;

  swap_temp_and_main_clip_lists();

  return 1;
}

// virtual
void l3d_polygon_3d_textured::after_camera_transform(void) {
  int i;

  l3d_polygon_3d::after_camera_transform();

  for(i=0; i<clip_ivertices->num_items; i++) {
    assert( dynamic_cast<l3d_polygon_ivertex_textured *>( & ((*(clip_ivertices))[i])) );

    ((l3d_polygon_ivertex_textured *)
     & ((*(clip_ivertices))[i]))->tex_coord.Z_ =

       (**(vlist))[ (*(clip_ivertices))[i].ivertex ].
       transformed_intermediates[1].Z_;
    // transformed_intermediate number 1 is camera-space coord
    // just before perspective projection

    //      printf("assigned %f to clip_ivtx %d\n",

    //      ((l3d_polygon_ivertex_textured *)
    //       & ((*(clip_ivertices))[i]))->tex_coord.Z_,
    //             i);

  }

}

void l3d_polygon_3d_textured::clip_segment_to_near_z
(l3d_polygon_ivertex *ivertex, l3d_real zclip, int idx0, int idx1)
{
  l3d_real x0,y0,z0,  x1,y1,z1,  t,  clip_x, clip_y, clip_z;
  l3d_real u0,v0,     u1,v1;

  /* x0,y0,z0 behind zclip; x1,y1,z1 in front of zclip */

  if((**vlist)[ (*ivertices)[idx0].ivertex ].transformed.Z_ > 0) {
    x0 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.X_;
    y0 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.Y_;
    z0 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.Z_;
    u0 = ((l3d_polygon_ivertex_textured *)&((*ivertices)[idx1]))
         ->tex_coord.X_;
    v0 = ((l3d_polygon_ivertex_textured *)&((*ivertices)[idx1]))
         ->tex_coord.Y_;

    x1 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.X_;
    y1 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.Y_;
    z1 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.Z_;
    u1 = ((l3d_polygon_ivertex_textured *)&((*ivertices)[idx0]))
         ->tex_coord.X_;
    v1 = ((l3d_polygon_ivertex_textured *)&((*ivertices)[idx0]))
         ->tex_coord.Y_;
  }
  else {
    x0 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.X_;
    y0 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.Y_;
    z0 = (**vlist)[ (*ivertices)[idx0].ivertex ].transformed.Z_;
    u0 = ((l3d_polygon_ivertex_textured *)&((*ivertices)[idx0]))
         ->tex_coord.X_;
    v0 = ((l3d_polygon_ivertex_textured *)&((*ivertices)[idx0]))
         ->tex_coord.Y_;

    x1 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.X_;
    y1 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.Y_;
    z1 = (**vlist)[ (*ivertices)[idx1].ivertex ].transformed.Z_;
    u1 = ((l3d_polygon_ivertex_textured *)&((*ivertices)[idx1]))
         ->tex_coord.X_;
    v1 = ((l3d_polygon_ivertex_textured *)&((*ivertices)[idx1]))
         ->tex_coord.Y_;
  }

  t = l3d_divrr( zclip-z0, z1-z0);

  clip_x = x0 + l3d_mulrr( x1-x0 , t );
  clip_y = y0 + l3d_mulrr( y1-y0 , t );
  clip_z = zclip; /* could also be computed with same formula above as proof */

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
  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.X_ =
    u0 + l3d_mulrr(t, u1-u0);

  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.Y_ =
    v0 + l3d_mulrr(t, v1-v0);

  ((l3d_polygon_ivertex_textured *)ivertex)->tex_coord.Z_ =
    clip_z;

}

/*****************************************************************************
 * clip facet against a given near z-plane, possibly adding new vertices
 *****************************************************************************/

int l3d_polygon_3d_textured::clip_near_z(l3d_real zclip) {
  /*
  for(int i=0; i<num_clip_pts; i++) {
   if ( (**vlist)[(*clip_ivertices)[i].ivertex].transformed.Z_ < 0.5) return 0;
}
  return 1;
  */
  /*
  man this crap SUCKS: it assumes it is the FIRST clipping operation and
  cavalierly pumps FROM ivertices INTO clip_ivertices. what if there
  were other 3d clips beforehand?

    2 errors:

  1. it must use existing clip_ivertices and not assume it is the first clip.
  2. it must save the old number of num_clip_pts when creating its new clip
   list, and pass this old_num_clip_pts to next_idx_right, just like
  everyone else, to loop through the vertices of the old polygon.

  its amazing this worked at all - nonchalantly calling next_idx_right while
  the num_clip_pts, upon which next_idx_right was based, were being changed
  during the calling loop!
  */



#if 0
  printf("BEFORE clipping:\n");
  for(int i=0; i<num_clip_pts; i++) {
    printf("ivertex %d: %f %f %f\n",
           i,
           (**vlist)[clip_ivertices[i].ivertex].transformed.X_,
           (**vlist)[clip_ivertices[i].ivertex].transformed.Y_,
           (**vlist)[clip_ivertices[i].ivertex].transformed.Z_);
  }
#endif




  int idx0,idx1;
  int crossedge0_idx0,crossedge0_idx1,crossedge1_idx0,crossedge1_idx1;
  int newedge_ivertex0, newedge_ivertex1;
  int i;
  int dir;
#define FIRST_CROSS_FRONT_TO_BACK 0
#define FIRST_CROSS_BACK_TO_FRONT 1

  l3d_polygon_ivertex_textured new_ivertex0, new_ivertex1;

  /* for each edge of the polygon, compare it with the z-clip plane.
   * there are 3 cases:
   * 1. both endpoints of edge > zclip. keep edge.
   * 2. both endpoints of edge <=zclip. discard edge.
   * 3. one endpoint of edge <=zclip, the other > zclip. clip edge.
   *    for convex polygons, if this occurs, it will occur with exactly
   *    two edges. we clip these two edges off and insert a new edge to
   *    "close" the polygon. we say <=zclip and not <zclip since this 
   *    ensures that the edge to be clipped MUST be partially > zclip.
   *
   * since the ivertex array is an ordered (clockwise, left-hand-rule)
   * list of vertices, the edges can be obtained by traversing this
   * array in order. since we know that IF there is a crossing, there will
   * be two crossings, we can use the following logic: start with 1st edge.
   * while no crossing, add edges to final edge list. if crossing, clip
   * crossed edge, loop until we find the other crossing, clip this edge,
   * add a new edge. if there are no crossings we just loop through the
   * whole array and add all vertices (edges).
   */

  idx0=0;
  idx1=next_clipidx_right(idx0, clip_ivertices->num_items);

  temp_clip_ivertices->num_items = 0;

  /* search for 1st crossing edge, meanwhile building up the new
  * polygon */
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

    /* did we loop through all vertices without finding a z-plane cross? */
    if(idx0==0) {

      /* all points (and thus the current point) are <=clipz... */
      if ((**vlist)[ (*clip_ivertices)[idx0].ivertex ].transformed.Z_ <=zclip)
        return 0;

      /* all points (and thus the current point) are >  clipz... */
      else
      {
        //            num_clip_pts = num_pts;
        // do nothing - the polygon remains unchanged, the changes in
        // temp_clip_ivertices simply remain until they are overwritten
        // by the next clipping operation.
        return 1;
      }

    }

    /* still looking for the crossing edge */
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
    /* did we loop through all vertices without finding 2nd z-plane cross? */
    if(idx0==crossedge0_idx0) {
      fprintf(stderr,"shouldn't be here! can't find 2nd crossing edge\n");
      return 0;
    }

    /* continue looking for 2nd z-plane cross */
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
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
      new_ivertex1;
#if 0
    printf("new first vtx: idx %d x %f y %f z %f\n",
           newedge_ivertex1,
           (**vlist)[newedge_ivertex1].transformed.X_,
           (**vlist)[newedge_ivertex1].transformed.Y_,
           (**vlist)[newedge_ivertex1].transformed.Z_);
#endif

    for(i=crossedge1_idx1;
        i!=crossedge0_idx0;
        i=next_clipidx_right(i,clip_ivertices->num_items))
    {
      (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
        (*clip_ivertices)[i];

    }
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
      (*clip_ivertices)[crossedge0_idx0];


    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
      new_ivertex0;
#if 0
    printf("new 2nd vtx: idx %d x %f y %f z %f\n",
           newedge_ivertex0,
           (**vlist)[newedge_ivertex0].transformed.X_,
           (**vlist)[newedge_ivertex0].transformed.Y_,
           (**vlist)[newedge_ivertex0].transformed.Z_);
#endif

    swap_temp_and_main_clip_lists();
  } else {
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
      new_ivertex0;
#if 0
    printf("new first vtx: idx %d x %f y %f z %f\n",
           newedge_ivertex0,
           (**vlist)[newedge_ivertex0].transformed.X_,
           (**vlist)[newedge_ivertex0].transformed.Y_,
           (**vlist)[newedge_ivertex0].transformed.Z_);
#endif

    for(i=crossedge0_idx1;
        i!=crossedge1_idx0;
        i=next_clipidx_right(i, clip_ivertices->num_items))
    {
      (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
        (*clip_ivertices)[i];

    }
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
      (*clip_ivertices)[crossedge1_idx0];

    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
      new_ivertex1;
#if 0
    printf("new 2nd vtx: idx %d x %f y %f z %f\n",
           newedge_ivertex1,
           (**vlist)[newedge_ivertex1].transformed.X_,
           (**vlist)[newedge_ivertex1].transformed.Y_,
           (**vlist)[newedge_ivertex1].transformed.Z_);
#endif

    swap_temp_and_main_clip_lists();
  }

#if 0
  printf("AFTER clipping:\n");
  for(int i=0; i<num_clip_pts; i++) {
    printf("ivertex %d: %f %f %f\n",
           i,
           (**vlist)[(*clip_ivertices)[i].ivertex].transformed.X_,
           (**vlist)[(*clip_ivertices)[i].ivertex].transformed.Y_,
           (**vlist)[(*clip_ivertices)[i].ivertex].transformed.Z_);
  }
#endif

  return 1;
}

void l3d_polygon_3d_textured::assign_tex_coords_from_tex_space
(l3d_texture_space const &tex_space)
{

  l3d_matrix world_to_tex_space = world_to_tex_matrix(tex_space);

  for(int vtx=0; vtx<ivertices->num_items; vtx++) {
    l3d_polygon_ivertex_textured *iv;
    iv = (l3d_polygon_ivertex_textured *)&((*ivertices)[vtx]);

    l3d_point world_point =
      (**vlist)[(*ivertices)[vtx].ivertex].original;

    iv->tex_coord =
      world_to_tex_space
      |
      world_point;
    iv->tex_coord.X_ =
      l3d_divrr(iv->tex_coord.X_, iv->tex_coord.W_);
    iv->tex_coord.Y_ =
      l3d_divrr(iv->tex_coord.Y_, iv->tex_coord.W_);
    //- ** DO NOT ** compute z now, nor try to compute 1/z or u/z or v/z.
    //- the values here are the true 3D u and v values. LATER in CAMERA SPACE
    //- we use cam-space z for 1/z,u/z,v/z, so that the z value is guaranteed
    //- to be reasonable and not astronomical.
    iv->tex_coord.W_ = int_to_l3d_real(1);

    //      printf("GPTEX: vtx %d: just computed uv of %f %f %f %f from %f %f %f %f\n",
    //             vtx,
    //             iv->tex_coord.X_,
    //             iv->tex_coord.Y_,
    //             iv->tex_coord.Z_,
    //             iv->tex_coord.W_,
    //             world_point.X_,
    //             world_point.Y_,
    //             world_point.Z_,
    //             world_point.W_);
    //      world_to_tex_space.print();

  }
}

int l3d_polygon_3d_textured::clip_to_plane(l3d_plane& clip_plane) {
  //  printf("p3 tex clip to plane\n");
  int idx0,idx1;
  int crossedge0_idx0,crossedge0_idx1,crossedge1_idx0,crossedge1_idx1;
  int newedge_ivertex0, newedge_ivertex1;
  l3d_polygon_ivertex_textured new_ivertex0, new_ivertex1;

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

  //     printf("we found a crossing edge, I think. \npoint %f %f %f %f side %f \npoint %f %f %f %f side %f\nplane %f %f %f %f\ntolerance %f\n",
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

  //- compute new texture coordinate clipped in 3D against the plane
  l3d_polygon_ivertex_textured *ivtx0, *ivtx1;
  l3d_vector tex_dif;

  ivtx0 = dynamic_cast<l3d_polygon_ivertex_textured *>
          ( & (*clip_ivertices)[crossedge0_idx0] );
  ivtx1 = dynamic_cast<l3d_polygon_ivertex_textured *>
          ( & (*clip_ivertices)[crossedge0_idx1] );
  assert(ivtx0 && ivtx1);
  new_ivertex0.ivertex = newedge_ivertex0;
  tex_dif = (ivtx1->tex_coord - ivtx0->tex_coord)
            * clip_plane.intersection_t;
  new_ivertex0.tex_coord = ivtx0->tex_coord + tex_dif;
  //     new_ivertex0.tex_coord.X_ = ivtx0->tex_coord.X_ + tex_dif.X_;
  //     new_ivertex0.tex_coord.Y_ = ivtx0->tex_coord.Y_ + tex_dif.Y_;
  //     new_ivertex0.tex_coord.Z_ = ivtx0->tex_coord.Z_ + tex_dif.Z_;
  //     new_ivertex0.tex_coord.W_ = ivtx0->tex_coord.W_ + tex_dif.W_;
  //     printf("tex dif %f %f %f %f\n",
  //            tex_dif.X_, tex_dif.Y_, tex_dif.Z_, tex_dif.W_);
  //     printf("ivtx0 tex coord %f %f %f %f\n",
  //            ivtx0->tex_coord.X_,
  //            ivtx0->tex_coord.Y_,
  //            ivtx0->tex_coord.Z_,
  //            ivtx0->tex_coord.W_);
  //     printf("ivtx1 tex coord %f %f %f %f\n",
  //            ivtx1->tex_coord.X_,
  //            ivtx1->tex_coord.Y_,
  //            ivtx1->tex_coord.Z_,
  //            ivtx1->tex_coord.W_);
  //     printf("intersection time %f\n",
  //            clip_plane.intersection_t);
  //     printf("new_ivertex0 %f %f %f %f\n",
  //            new_ivertex0.tex_coord.X_,
  //            new_ivertex0.tex_coord.Y_,
  //            new_ivertex0.tex_coord.Z_,
  //            new_ivertex0.tex_coord.W_);

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

  //- compute new texture coordinate clipped in 3D against the plane
  ivtx0 = dynamic_cast<l3d_polygon_ivertex_textured *>
          ( & (*clip_ivertices)[crossedge1_idx0] );
  ivtx1 = dynamic_cast<l3d_polygon_ivertex_textured *>
          ( & (*clip_ivertices)[crossedge1_idx1] );
  assert(ivtx0 && ivtx1);
  new_ivertex1.ivertex = newedge_ivertex1;
  tex_dif = (ivtx1->tex_coord - ivtx0->tex_coord)
            * clip_plane.intersection_t;
  new_ivertex1.tex_coord = ivtx0->tex_coord + tex_dif;
  //     new_ivertex1.tex_coord.X_ = ivtx0->tex_coord.X_ + tex_dif.X_;
  //     new_ivertex1.tex_coord.Y_ = ivtx0->tex_coord.Y_ + tex_dif.Y_;
  //     new_ivertex1.tex_coord.Z_ = ivtx0->tex_coord.Z_ + tex_dif.Z_;
  //     new_ivertex1.tex_coord.W_ = ivtx0->tex_coord.W_ + tex_dif.W_;

  //     printf("tex dif %f %f %f %f\n",
  //            tex_dif.X_, tex_dif.Y_, tex_dif.Z_, tex_dif.W_);
  //     printf("ivtx0 tex coord %f %f %f %f\n",
  //            ivtx0->tex_coord.X_,
  //            ivtx0->tex_coord.Y_,
  //            ivtx0->tex_coord.Z_,
  //            ivtx0->tex_coord.W_);
  //     printf("ivtx1 tex coord %f %f %f %f\n",
  //            ivtx1->tex_coord.X_,
  //            ivtx1->tex_coord.Y_,
  //            ivtx1->tex_coord.Z_,
  //            ivtx1->tex_coord.W_);
  //     printf("intersection time %f\n",
  //            clip_plane.intersection_t);
  //     printf("new_ivertex1 %f %f %f %f\n",
  //            new_ivertex1.tex_coord.X_,
  //            new_ivertex1.tex_coord.Y_,
  //            new_ivertex1.tex_coord.Z_,
  //            new_ivertex1.tex_coord.W_);


  {
    idx0=idx1; idx1=next_clipidx_right(idx0, clip_ivertices->num_items);
  }

  temp_clip_ivertices->num_items = 0;

  if(dir==CROSS_FRONT_TO_BACK) {
    //      (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
    //	newedge_ivertex1;
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] = new_ivertex1;

    for(i=crossedge1_idx1;
        i!=crossedge0_idx0;
        i=next_clipidx_right(i,clip_ivertices->num_items))
    {
      (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
        (*clip_ivertices)[i];
    }
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
      (*clip_ivertices)[crossedge0_idx0];

    //        (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
    //  	newedge_ivertex0;
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] = new_ivertex0;

    swap_temp_and_main_clip_lists();
  } else {
    //        (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
    //  	newedge_ivertex0;
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] = new_ivertex0;

    for(i=crossedge0_idx1;
        i!=crossedge1_idx0;
        i=next_clipidx_right(i, clip_ivertices->num_items))
    {
      (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
        (*clip_ivertices)[i];

    }
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] =
      (*clip_ivertices)[crossedge1_idx0];

    //        (*temp_clip_ivertices)[temp_clip_ivertices->next_index()].ivertex =
    //  	newedge_ivertex1;
    (*temp_clip_ivertices)[temp_clip_ivertices->next_index()] = new_ivertex1;

    swap_temp_and_main_clip_lists();
  }

  return 1;
}


//    void l3d_polygon_3d_textured::compute_projected_tex_deltas(l3d_real fovx, l3d_real fovy)
//    {
//      l3d_real k,ki;
//      l3d_vector O_vec, V_vec, U_vec;

//      l3d_vector O_x_V, O_x_U, V_x_U;
//      O_vec.set(texture->O.transformed.X_,
//                texture->O.transformed.Y_,
//                texture->O.transformed.Z_,
//                int_to_l3d_real(0));
//      U_vec = texture->U.transformed - texture->O.transformed;
//      V_vec = texture->V.transformed - texture->O.transformed;

//      O_x_V = cross(O_vec, V_vec);
//      O_x_U = cross(O_vec, U_vec);
//      V_x_U = cross(V_vec, U_vec);
//      k = dot(O_vec, V_x_U);
//      ki = l3d_divrr(int_to_l3d_real(512), k);

//      dui_dx = l3d_real_to_fixed(l3d_divrr(l3d_mulrr(ki, O_x_V.X_),l3d_mulri(fovx,400)));
//      dui_dy = l3d_real_to_fixed(l3d_divrr(l3d_mulrr(ki, O_x_V.Y_),l3d_mulri(fovy,-300)));

//      dvi_dx = l3d_real_to_fixed(l3d_divrr(l3d_mulrr(ki, O_x_U.X_),l3d_mulri(fovx,400)));
//      dvi_dy = l3d_real_to_fixed(l3d_divrr(l3d_mulrr(ki, O_x_U.Y_),l3d_mulri(fovy,-300)));

//      dzi_dx = l3d_real_to_fixed(l3d_divrr(l3d_mulrr(ki, V_x_U.X_),l3d_mulri(fovx,400)));
//      dzi_dy = l3d_real_to_fixed(l3d_divrr(l3d_mulrr(ki, V_x_U.Y_),l3d_mulri(fovy,-300)));
//    };

