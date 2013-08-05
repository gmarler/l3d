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

#include "ras3z_sw.h"
#include "../system/sys_dep.h"
#include <string.h>
#include <stdlib.h>
#include "../tool_os/memman.h"

///////////////////////////////////////////////////////////////////////////

l3d_rasterizer_3d_imp * l3d_rasterizer_3d_zbuf_sw_imp_factory::create
(int xs, int ys, l3d_screen_info *si)
{
  return new l3d_rasterizer_3d_zbuf_sw_imp(xs,ys,si);
}

l3d_rasterizer_3d_zbuf_sw_imp::l3d_rasterizer_3d_zbuf_sw_imp
(int xs, int ys, l3d_screen_info *si)
    :
    l3d_rasterizer_3d_sw_imp(xs,ys,si),
    l3d_rasterizer_3d_imp(xs,ys,si),
    l3d_rasterizer_2d_sw_imp(xs,ys,si),
    l3d_rasterizer_2d_imp(xs,ys,si)
{
  zbuf = new l3d_fixed[xs*ys];
  zbuf_size = xs*ys;
  long register int i;
  for(i=0; i<zbuf_size; i++) {
    zbuf[i] = 0;
  }
}

l3d_rasterizer_3d_zbuf_sw_imp::~l3d_rasterizer_3d_zbuf_sw_imp(void) {
  delete [] zbuf;
}

void l3d_rasterizer_3d_zbuf_sw_imp::clear_buffer(void) {
  l3d_rasterizer_3d_sw_imp::clear_buffer();

  // clear z buffer as well
  long register int i;

  for(i=0; i<zbuf_size; i++) {
    zbuf[i] = 0;
  }
}
//  /*****************************************************************************
//   * Draw a texture-mapped polygon with perspective correction
//   *****************************************************************************/

#define Z_MULT_FACTOR 512
#define Z_ADD_FACTOR -0.42
//
// Z_ADD_FACTOR is added to Z just before we compute 1/z:
// zi = Z_MULT_FACTOR / (z + Z_ADD_FACTOR)

// to avoid overflow, zi * ui and zi * vi < MAXINT in fixed

// MAXZVAL < Z_MULT_FACTOR / (z + Z_ADD_FACTOR)
// MAXZVAL * (z + Z_ADD_FACTOR) < Z_MULT_FACTOR
// z + Z_ADD_FACTOR < Z_MULT_FACTOR / MAXZVAL
// Z_ADD_FACTOR < Z_MULT_FACTOR / MAXZVAL - z
// due to near z-plane clipping minimum z is 0.5
// Z_ADD_FACTOR < Z_MULT_FACTOR / MAXZVAL - MIN_CLIP_Z
//
// now MAXZVAL depends on what zi will be multiplied with.
// zi is mult with ui and vi, and the result must be < max_fix_int (131072)
// assuming ui and vi are in the range 0 to 16, this means zi can have a
// max of 131072 / 16 = 8192.
//
// this means Z_ADD_FACTOR < 512 / 8192 - 0.5 = -0.4375


//- convenience macro for accessing the vertex coordinates. Notice
//- that we access the clipped vertex index list (not the original),
//- and the transformed coordinate (not the original). This means
//- we draw the clipped version of the transformed polygon.
#define VTX(i) ((**(p_poly->vlist))[ (*(p_poly->clip_ivertices))[i].ivertex ].transformed)

#include "../math/fix_lowp.h"
#if 0
#define l3d_fixed float
#define l3d_real_to_fixed(x) (x)
#define fixfixdiv(a,b) ((a)/(b))
#define fixfixmul(a,b) ((a)*(b))
#define int2fix(a) ( (float)(a))
#define fix2int(a) ( (int)(a))
#define fix2float(a) (a)
#define float2fix(a) (a)
#define iceil_fix(a) ( (int)ceil((double)(a)) )

#endif

void l3d_rasterizer_3d_zbuf_sw_imp::draw_polygon_textured
(const l3d_polygon_3d_textured *p_poly)
{

  l3d_fixed x0,y0,x1,y1,x2,y2,x3;
  l3d_fixed top_y, bottom_y;
  int point_on_right=0;
  int left_idx, right_idx, top_y_idx, bottom_y_idx;

  int maxy_upper, iceil_fix_y0, iceil_fix_y1, iceil_fix_y2;

  l3d_point *vtemp;

  int i;

  int scanline;

  //- variables for the left edge, incremented BETWEEN scanlines
  //- (inter-scanline) 

  l3d_fixed volatile
  left_x,                     left_ui,       left_vi,       left_zi,
  left_x_start, left_y_start, left_ui_start, left_vi_start, left_zi_start,
  left_x_end,   left_y_end,   left_ui_end,   left_vi_end,   left_zi_end,
  left_dx_dy,                 left_dui_dy,   left_dvi_dy,   left_dzi_dy;
  int
  left_ceilx,
  left_ceilx_start, left_ceilx_end,
  left_ceily_start, left_ceily_end;

  //- variables for the right edge, incremented BETWEEN scanlines 
  //- (inter-scanline) 

  l3d_fixed volatile
  right_x,                    right_ui,      right_vi,      right_zi,
  right_x_start,right_y_start,right_ui_start,right_vi_start,right_zi_start,
  right_x_end,  right_y_end,  right_ui_end,  right_vi_end,  right_zi_end,
  right_dx_dy,                right_dui_dy,  right_dvi_dy,  right_dzi_dy;

  int
  right_ceilx,
  right_ceilx_start, right_ceilx_end,
  right_ceily_start, right_ceily_end;

  //- variables incremented WITHIN one scanline (intra-scanline) 

  l3d_fixed volatile
  u,v,z,
  ui, vi, zi, dui_dx, dvi_dx, dzi_dx,

  // for linear interp.
  u_left, u_right, du_dx,
  v_left, v_right, dv_dx,
  inv_dx, denom, inv_run_dx,

  run_x, run_x_start, run_x_end,
  u_run_end,  v_run_end, du_dx_run, dv_dx_run;

  long int cur_x;

  //     {
  //       int a;
  //       for(a=0;a<(*p_poly->clip_ivertices).num_items;a++) {
  //         printf("ivtx %d at cam-spc ",a);
  //         (**p_poly->vlist)[(*p_poly->clip_ivertices)[a].ivertex].
  //           transformed_intermediates[1].print();
  //         printf("-->with tex coord ");
  //         ((l3d_polygon_ivertex_textured *)&((*p_poly->clip_ivertices)[a]))
  //           ->tex_coord.print();


  //       }
  //     }

  top_y = l3d_real_to_fixed(VTX(0).Y_);
  top_y_idx = 0;
  bottom_y = top_y;
  bottom_y_idx = top_y_idx;
  for(i=0; i<p_poly->clip_ivertices->num_items; i++) {
    if(l3d_real_to_fixed(VTX(i).Y_) < top_y) {
      top_y = l3d_real_to_fixed(VTX(i).Y_);
      top_y_idx = i;
    }
    if(l3d_real_to_fixed(VTX(i).Y_) > bottom_y) {
      bottom_y = l3d_real_to_fixed(VTX(i).Y_);
      bottom_y_idx = i;
    }
  }

  left_idx = top_y_idx;
  right_idx = top_y_idx;

  left_x_start=l3d_real_to_fixed(VTX(top_y_idx).X_);
  left_ceilx_start=iceil_fix(left_x_start);
  left_y_start=l3d_real_to_fixed(VTX(top_y_idx).Y_);
  left_ceily_start=iceil_fix(left_y_start);
  left_ceily_end=left_ceily_start;
  left_zi_start = fixfixdiv(int2fix(Z_MULT_FACTOR),
                            l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                               & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.Z_)
                            + float2fix(Z_ADD_FACTOR));
  left_zi_end = left_zi_start;
  left_ui_start =
    fixfixmul(
      l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                         & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.X_),
      left_zi_start);
  left_ui_end = left_ui_start;
  left_vi_start = fixfixmul(
                    l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                       & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.Y_),
                    left_zi_start);
  left_vi_end = left_vi_start;

  right_x_start=left_x_start;
  right_y_start=left_y_start;
  right_ceily_start=left_ceily_start;
  right_ceily_end=right_ceily_start;
  right_ui_start = left_ui_start;
  right_vi_start = left_vi_start;
  right_zi_start = left_zi_start;

  scanline = left_ceily_start;
  int oneshot=1;

  //   printf("poly starting scanline %d ",scanline);
  while(scanline < ysize) {
    // if needed, find next left-edge whose ceily_end > current scanline
    while( left_ceily_end - scanline <= 0 )
      // technically this SHOULD never be < 0, but can be if (incorrectly)
      // the start > end
    {
      if (left_idx == bottom_y_idx) {
        //           printf("stopped poly at scanline %d\n", scanline);
        return;  // done 
      }
      left_idx = p_poly->next_clipidx_left(left_idx,p_poly->clip_ivertices->num_items);
      left_y_end=l3d_real_to_fixed(VTX(left_idx).Y_);
      left_ceily_end = iceil_fix(left_y_end);
      if(left_ceily_end - scanline) {  // found next vertex
#define MIN_EDGE_HEIGHT float2fix(0.005)
#define MIN_EDGE_WIDTH float2fix(0.005)
        //         if(left_y_end - int2fix(scanline) > MIN_EDGE_HEIGHT) {  // found next vertex 
        // must check against min_edge_height because if edge height is
        // TOO small then the d???_dy variables will divide by a very
        // small dy, which causes a floating point overflow
        left_x_end=l3d_real_to_fixed(VTX(left_idx).X_);
        left_ceilx_end=iceil_fix(left_x_end);

        left_zi_end = fixfixdiv(int2fix(Z_MULT_FACTOR),
                                l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                                   & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.Z_)
                                + float2fix(Z_ADD_FACTOR));
        left_ui_end = fixfixmul(
                        l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                           & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.X_),
                        left_zi_end);
        left_vi_end = fixfixmul(
                        l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                           & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.Y_),
                        left_zi_end);

        if(left_y_end - left_y_start >= MIN_EDGE_HEIGHT) {
          left_dx_dy = fixfixdiv(left_x_end-left_x_start,left_y_end-left_y_start);
          left_dui_dy = fixfixdiv(left_ui_end - left_ui_start,
                                  left_y_end  - left_y_start);
          left_dvi_dy = fixfixdiv(left_vi_end - left_vi_start,
                                  left_y_end  - left_y_start);
          left_dzi_dy = fixfixdiv(left_zi_end - left_zi_start,
                                  left_y_end  - left_y_start);
        }
        else
        {
          left_dx_dy =
            left_dui_dy =
              left_dvi_dy =
                left_dzi_dy = int2fix(0);
        }

        left_x = left_x_start +  // sub-pixel correction 
                 fixfixmul(int2fix(left_ceily_start)-left_y_start , left_dx_dy);
        left_ui = left_ui_start + //- sub-pixel correction
                  fixfixmul(int2fix(left_ceily_start)-left_y_start,
                            left_dui_dy);
        left_vi = left_vi_start + //- sub-pixel correction
                  fixfixmul(int2fix(left_ceily_start)-left_y_start,
                            left_dvi_dy);
        left_zi = left_zi_start + //- sub-pixel correction
                  fixfixmul(int2fix(left_ceily_start)-left_y_start,
                            left_dzi_dy);
      } else {  // did not find next vertex: last failed end = new start
        left_x_start = l3d_real_to_fixed(VTX(left_idx).X_);
        left_ceilx_start=iceil_fix(left_x_start);
        left_y_start = l3d_real_to_fixed(VTX(left_idx).Y_);
        left_ceily_start = iceil_fix(left_y_start);
        left_zi_start = fixfixdiv(int2fix(Z_MULT_FACTOR),
                                  l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                                     & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.Z_)
                                  + float2fix(Z_ADD_FACTOR));
        left_ui_start = fixfixmul(
                          l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                             & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.X_),
                          left_zi_start);
        left_vi_start = fixfixmul(
                          l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                             & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.Y_),
                          left_zi_start); // assert failed? fixfixmul 3.278870 51150.048767 = -94429.658630, should be 167714.341419

      }
    }

    // if needed, find next right-edge whose ceily_end > current scanline
    while(right_ceily_end - scanline <= 0 )
      // technically this SHOULD never be < 0, but can be if (incorrectly)
      // the start > end
    {
      if (right_idx == bottom_y_idx) {
        //           printf("stopped poly at scanline %d\n", scanline);
        return;  // done
      }
      right_idx = p_poly->next_clipidx_right(right_idx, p_poly->clip_ivertices->num_items);
      right_y_end=l3d_real_to_fixed(VTX(right_idx).Y_);
      right_ceily_end = iceil_fix(right_y_end);
      if(right_ceily_end - scanline) {  // found next vertex
        //         if(right_y_end - int2fix(scanline) > MIN_EDGE_HEIGHT) {  // found next vertex
        // must check against min_edge_height because if edge height is
        // TOO small then the d???_dy variables will divide by a very
        // small dy, which causes a floating point overflow

        right_x_end=l3d_real_to_fixed(VTX(right_idx).X_);
        right_ceilx_end = iceil_fix(right_x_end);
        right_zi_end = fixfixdiv(int2fix(Z_MULT_FACTOR),
                                 l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                                    & ((*(p_poly->clip_ivertices))[right_idx]))->tex_coord.Z_)
                                 + float2fix(Z_ADD_FACTOR));
        right_ui_end = fixfixmul(
                         l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                            & ((*(p_poly->clip_ivertices))[right_idx]))->tex_coord.X_),
                         right_zi_end);
        right_vi_end = fixfixmul(
                         l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                            & ((*(p_poly->clip_ivertices))[right_idx]))->tex_coord.Y_),
                         right_zi_end);


        if(right_y_end-right_y_start>=MIN_EDGE_HEIGHT) {
          right_dx_dy =
            fixfixdiv(right_x_end-right_x_start,right_y_end-right_y_start);
          right_dui_dy = fixfixdiv(right_ui_end - right_ui_start,
                                   right_y_end  - right_y_start);
          right_dvi_dy = fixfixdiv(right_vi_end - right_vi_start,
                                   right_y_end  - right_y_start);
          right_dzi_dy = fixfixdiv(right_zi_end - right_zi_start,
                                   right_y_end  - right_y_start);
        }
        else
        {
          right_dx_dy =
            right_dui_dy =
              right_dvi_dy =
                right_dzi_dy = int2fix(0);
        }

        right_x = right_x_start +  // sub-pixel correction
                  fixfixmul(int2fix(right_ceily_start)-right_y_start , right_dx_dy);

        right_ui = right_ui_start + //- sub-pixel correction
                   fixfixmul(int2fix(right_ceily_start)-right_y_start,
                             right_dui_dy);
        right_vi = right_vi_start + //- sub-pixel correction
                   fixfixmul(int2fix(right_ceily_start)-right_y_start,
                             right_dvi_dy);
        right_zi = right_zi_start + //- sub-pixel correction
                   fixfixmul(int2fix(right_ceily_start)-right_y_start,
                             right_dzi_dy);

      } else {  // did not find next vertex: last failed end = new start
        right_x_start = l3d_real_to_fixed(VTX(right_idx).X_);
        right_ceilx_start = iceil_fix(right_x_start);
        right_y_start = l3d_real_to_fixed(VTX(right_idx).Y_);
        right_ceily_start = iceil_fix(right_y_start);

        right_zi_start = fixfixdiv(int2fix(Z_MULT_FACTOR),
                                   l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                                      & ((*(p_poly->clip_ivertices))[right_idx]))->tex_coord.Z_)
                                   +float2fix(Z_ADD_FACTOR));
        right_ui_start = fixfixmul(
                           l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                              & ((*(p_poly->clip_ivertices))[right_idx]))->tex_coord.X_),
                           right_zi_start);
        right_vi_start = fixfixmul(
                           l3d_real_to_fixed(((l3d_polygon_ivertex_textured *)
                                              & ((*(p_poly->clip_ivertices))[right_idx]))->tex_coord.Y_),
                           right_zi_start);
      }
    }


    // lack of this check caused segfault in chk2dclip!!! : the next found
    // edge can extend far underneath the screen boundaries! we didn't
    // want to clamp the ceily_end values earlier, since the actual inv-slopes
    // for stepping in X must be calculated in terms of the UNCLIPPED
    // actual ceily_end values. here we can clamp these values to scanline,
    // but ONLY because we know that when one of these hits scanline, the
    // loop exits. if drawing were to continue afterwards somehow, we would
    // NOT be able to clamp these values, because the initialization of
    // the values for the next iteration depends on the actual (non-clamped)
    // ending value of left_ceily_end.
    //
    // perhaps this means a new variable, "left_ceily_end_clamped_to_screen"
    // and "right_ceily_end_clamped_to_screen", whose sole purpose is to
    // stop either the left or the right hand drawing when the left or right
    // edge is finished, but which do not necessarily represent the
    // actual ceily_end of the left or right edge.
    //
    // the other alternative, commented out below, was to check in the inner
    // scanline loop to see if we hit scanline. but this requires a compare
    // at every scanline!

    if (left_ceily_end > ysize) left_ceily_end = ysize;
    if (right_ceily_end > ysize) right_ceily_end = ysize;

//      /*
//      printf("u,v,z left %f,%f %f : %f %f %f\n",
//             ((l3d_polygon_ivertex_textured *)
//              & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.X_,
//             ((l3d_polygon_ivertex_textured *)
//              & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.Y_,
//             ((l3d_polygon_ivertex_textured *)
//              & ((*(p_poly->clip_ivertices))[left_idx]))->tex_coord.Z_,
//             fix2float(left_ui) / fix2float(left_zi),
//             fix2float(left_vi) / fix2float(left_zi),
//             Z_MULT_FACTOR / fix2float(left_zi)

//             );
//      printf("u,v,z right %f,%f %f\n",
//             ((l3d_polygon_ivertex_textured *)
//              & ((*(p_poly->clip_ivertices))[right_idx]))->tex_coord.X_,
//             ((l3d_polygon_ivertex_textured *)
//              & ((*(p_poly->clip_ivertices))[right_idx]))->tex_coord.Y_,
//             ((l3d_polygon_ivertex_textured *)
//              & ((*(p_poly->clip_ivertices))[right_idx]))->tex_coord.Z_
//             );
//      */

    while ( (scanline < left_ceily_end) && (scanline < right_ceily_end) )
    {
      if (left_x > right_x) {
        //           printf("BAILING stopped poly at scanline %d\n", scanline);
        //           return; // due to slivers, left_x can be < right_x
      }
      else {
        // no scanline clipping or bounds checking needed here due to 
        // analytical clipping 
        left_ceilx = iceil_fix(left_x);
        right_ceilx = iceil_fix(right_x);

        // if the segment is (horizontally) visible, draw it.

        if(right_x - left_x > MIN_EDGE_WIDTH) {
          dui_dx = fixfixdiv(right_ui - left_ui , right_x  - left_x);
          dvi_dx = fixfixdiv(right_vi - left_vi , right_x  - left_x);
          dzi_dx = fixfixdiv(right_zi - left_zi , right_x  - left_x);
        } else {
          // a segment exactly one pixel wide (left_x == right_x)
          dui_dx =
            dvi_dx =
              dzi_dx = int2fix(0);
        }

        ui = left_ui + //- sub-pixel correction
             fixfixmul(int2fix(left_ceilx) - left_x, dui_dx);
        vi = left_vi + //- sub-pixel correction
             fixfixmul(int2fix(left_ceilx) - left_x, dvi_dx);
        zi = left_zi + //- sub-pixel correction
             fixfixmul(int2fix(left_ceilx) - left_x, dzi_dx);

//          /*
//                   printf("sl %d left vi %f delta %f (%f-%f), right vi %f del %f (%f-%f)\n",
//                 scanline,
//                 fix2float(left_vi),
//                 fix2float(left_dvi_dy),
//                 fix2float(left_vi_start),
//                 fix2float(left_vi_end),
//                 fix2float(right_vi),
//                 fix2float(right_dvi_dy),
//                 fix2float(right_vi_start),
//                 fix2float(right_vi_end)

//          );
//          */

//          /*
//          if(oneshot) {
//            oneshot=0;

//          printf("start: u v z %f %f %f\n",
//           fix2float(left_ui_start)/fix2float(left_zi_start),
//           fix2float(left_vi_start)/fix2float(left_zi_start),
//           1.0/fix2float(left_zi_start));

//                   printf("scanline %d ui/zi %f vi/zi %f zi %f dzidy l %f r %f\n",
//                 scanline,
//                 fix2float(ui)/fix2float(zi),
//                 fix2float(vi)/fix2float(zi),
//                 fix2float(zi),
//                 fix2float(left_dzi_dy),
//                 fix2float(right_dzi_dy)
//                 );

//        }
//          */
        cur_x=left_ceilx;

        // linear interp
#define LINEAR_RUN_SHIFT 4
#define LINEAR_RUN_SIZE (1<<LINEAR_RUN_SHIFT)
        // correct:      u =  ui/zi;  // start of run 
        //               v =  vi/zi;
        z = fixfixdiv(int2fix(1), zi);
        u = fixfixmul(ui,z);
        //                       printf("hey! first real u %f\n", fix2float(u));
        v = fixfixmul(vi,z); // assert failed? fixfixmul 37.126770 -4096.000000 = 110072.750000, should be -152071.250000

        //            printf("hey! first real v %f\n", fix2float(v));


        l3d_fixed *zbuf_ptr;

        unsigned char *pix;
        for(pix=sinfo->p_screenbuf+(left_ceilx + SW_RAST_Y_REVERSAL(ysize, scanline)*xsize)*sinfo->bytes_per_pixel,
            zbuf_ptr = zbuf + (left_ceilx + scanline*xsize)
                       ;
            pix< sinfo->p_screenbuf+(right_ceilx+ SW_RAST_Y_REVERSAL(ysize,
                                     scanline)*xsize)*sinfo->bytes_per_pixel;
           )
        {
          run_x_end = int2fix(cur_x) +
                      int2fix(LINEAR_RUN_SIZE);
          if (run_x_end > int2fix(right_ceilx)) {
            run_x_end = int2fix(right_ceilx);
          }
          denom = fixfixdiv
                  (int2fix(1) ,
                   zi + fixfixmul(dzi_dx, run_x_end-int2fix(cur_x)));
          u_run_end=fixfixmul
                    (ui + fixfixmul(dui_dx,(run_x_end-int2fix(cur_x))),
                     denom) ;
          v_run_end=fixfixmul
                    (vi + fixfixmul(dvi_dx,(run_x_end-int2fix(cur_x))),
                     denom) ; // assert failed? fixfixmul 36.053711 -16384.000000 = -66416.000000, should be -590704.000000

          inv_run_dx = fixfixdiv
                       (int2fix(1),
                        (run_x_end-int2fix(cur_x)));
          du_dx_run = fixfixmul((u_run_end - u) ,inv_run_dx);
          dv_dx_run = fixfixmul((v_run_end - v) ,inv_run_dx);
          //               denom = 1.0/(zi+zi_xs*(run_x_end-cur_x));
          //               u_run_end=(ui+ui_xs*(run_x_end-cur_x)) * denom;
          //               v_run_end=(vi+vi_xs*(run_x_end-cur_x)) * denom;
          //               du_dx_run = (u_run_end-u) / (run_x_end-cur_x);
          //               dv_dx_run = (v_run_end-v) / (run_x_end-cur_x);

          for(run_x = int2fix(cur_x);
              run_x < run_x_end;
              run_x+=int2fix(1))
          {
            unsigned char *texel =
              p_poly->texture->tex_data->data +
              ((fix2int(l3d_mulri(v,p_poly->texture->tex_data->height-1))&(p_poly->texture->tex_data->height-1))*(p_poly->texture->tex_data->width)
               + (fix2int(l3d_mulri(u,p_poly->texture->tex_data->width-1))&(p_poly->texture->tex_data->width-1)) ) *
              sinfo->bytes_per_pixel;

            // inner loop slower due to z-buf and fog!
            //sbegin zbuf
            if(zi > *zbuf_ptr) {
              *zbuf_ptr = zi;

              for(register int b=0; b<sinfo->bytes_per_pixel;b++) {
                *pix++ =  *texel++;
              }

              //                   int fog = fix2int(fixintmul(fixfixdiv(int2fix(1),zi),
              //                                     Z_MULT_FACTOR*4));
              int fog = int2fix(Z_MULT_FACTOR) - fixintmul(zi,64);
              fog = fog >> 14;
              if(fog<0) fog=0;
              if(fog>MAX_LIGHT_LEVELS) fog=MAX_LIGHT_LEVELS;
              //                   sinfo->light_native(pix - sinfo->bytes_per_pixel, MAX_LIGHT_LEVELS - fog);
              sinfo->fog_native(pix - sinfo->bytes_per_pixel, fog);
            } else {
              pix+= sinfo->bytes_per_pixel;
              texel+= sinfo->bytes_per_pixel;
            }
            zbuf_ptr++;
            //send zbuf
            u += du_dx_run;
            v += dv_dx_run;
            zi += dzi_dx;
            //                 printf("u,v %f %f\n",fix2float(u),fix2float(v));
          }

          cur_x += LINEAR_RUN_SIZE;
#if 0
          ui += dui_dx<<LINEAR_RUN_SHIFT;
          vi += dvi_dx<<LINEAR_RUN_SHIFT;
          zi += dzi_dx<<LINEAR_RUN_SHIFT;
#else
ui += l3d_mulri(dui_dx, LINEAR_RUN_SIZE);
          vi += l3d_mulri(dvi_dx, LINEAR_RUN_SIZE);
          // zi += l3d_mulri(dzi_dx, LINEAR_RUN_SIZE);
          // ^ no longer needed since zi increased in inner loop for
          // z buffer
#endif
        }
      }

      scanline++;
      left_x += left_dx_dy;
      right_x += right_dx_dy;

      left_ui += left_dui_dy;
      left_vi += left_dvi_dy;
      left_zi += left_dzi_dy;
      right_ui += right_dui_dy;
      right_vi += right_dvi_dy;
      right_zi += right_dzi_dy;
    }

//      /* for the left and/or right segment(s) which just completed drawing
//       * initialize xxx_start = xxx_end, to begin next segment. xxx_end is
//       * then searched for in the next iteration (the while() loops)
//       */
    if ( left_ceily_end - scanline <= 0 ) {
      // technically this SHOULD never be < 0, but can be if (incorrectly)
      // the start > end
      left_x_start=left_x_end;
      left_y_start=left_y_end;
      left_ceily_start=left_ceily_end;
      left_ui_start=left_ui_end;
      left_vi_start=left_vi_end;
      left_zi_start=left_zi_end;

    }
    if ( right_ceily_end - scanline <= 0 ) {
      // technically this SHOULD never be < 0, but can be if (incorrectly)
      // the start > end
      right_x_start=right_x_end;
      right_y_start=right_y_end;
      right_ceily_start=right_ceily_end;
      right_ui_start=right_ui_end;
      right_vi_start=right_vi_end;
      right_zi_start=right_zi_end;


    }
  }

}
#include "../math/fix_prec.h"

//  #ifdef DELME
//  void l3d_rasterizer_3d_zbuf_sw_imp::draw_polygon_textured_lightmapped
//  (const l3d_polygon_3d_textured_lightmapped *p_poly)
//  {
//      l3d_texture *old_tex;

//      if(p_poly->surface==NULL) {
//        // compute_surface_orientation_and_size();
//        p_poly->surface = p_poly->get_scache()->combine_lightmap_and_texmap
//  	(p_poly->surface_xsize, p_poly->surface_ysize, p_poly);
//      }

//        p_poly->surface->O = p_poly->surface_orientation.O;
//        p_poly->surface->U = p_poly->surface_orientation.U;
//        p_poly->surface->V = p_poly->surface_orientation.V;

//      /*
//  printf("Old O: %f,%f,%f,%f\n",
//         texture->O.original.X_,
//         texture->O.original.Y_,
//         texture->O.original.Z_,
//         texture->O.original.W_);
//  printf("New O: %f,%f,%f,%f\n",
//         new_tex->O.original.X_,
//         new_tex->O.original.Y_,
//         new_tex->O.original.Z_,
//         new_tex->O.original.W_);
//  printf("Old V: %f,%f,%f,%f\n",
//         texture->V.original.X_,
//         texture->V.original.Y_,
//         texture->V.original.Z_,
//         texture->V.original.W_);
//  printf("New V: %f,%f,%f,%f\n",
//         new_tex->V.original.X_,
//         new_tex->V.original.Y_,
//         new_tex->V.original.Z_,
//         new_tex->V.original.W_);
//  printf("Old U: %f,%f,%f,%f\n",
//         texture->U.original.X_,
//         texture->U.original.Y_,
//         texture->U.original.Z_,
//         texture->U.original.W_);
//  printf("New U: %f,%f,%f,%f\n",
//         new_tex->U.original.X_,
//         new_tex->U.original.Y_,
//         new_tex->U.original.Z_,
//         new_tex->U.original.W_);
//      */

//      old_tex = p_poly->texture;
//      p_poly->texture = p_poly->surface;
//      draw_polygon_textured(p_poly);
//      p_poly->texture = old_tex;
//    }

//  #endif
