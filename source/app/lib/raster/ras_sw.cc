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

#include "ras_sw.h"
#include "../system/sys_dep.h"
#include <string.h>
#include <stdlib.h>
#include "../tool_os/memman.h"

l3d_rasterizer_2d_imp * l3d_rasterizer_2d_sw_imp_factory::create
(int xs, int ys, l3d_screen_info *si)
{
  return new l3d_rasterizer_2d_sw_imp(xs,ys,si);
}

l3d_rasterizer_2d_sw_imp::l3d_rasterizer_2d_sw_imp
(int xs, int ys, l3d_screen_info *si)
    : l3d_rasterizer_2d_imp(xs,ys,si)
{
}

void l3d_rasterizer_2d_sw_imp::clear_buffer(void) {
  memset(sinfo->p_screenbuf, 0x00, (size_t)xsize*ysize*sinfo->bytes_per_pixel);
}

//sbegin drawline
void l3d_rasterizer_2d_sw_imp::draw_line(int x0, int y0,
    int x1, int y1, unsigned long col)
{
  l3d_real fx,fy,m;
  int x,y,tmp,dx,dy;

  dx = x1 - x0;
  dy = y1 - y0;

  if(abs( ABS_CAST dx) > abs( ABS_CAST dy)) { //- a "mostly horizontal" line
    //- ensure (x0,y0) is horizontally smaller than (x1,y1)
    if(x1<x0) { tmp=x0;x0=x1;x1=tmp; tmp=y0;y0=y1;y1=tmp;}
    fy = int_to_l3d_real(y0);
    m = l3d_divrr( int_to_l3d_real(dy), int_to_l3d_real(dx) );
    for(x=x0; x<=x1; x++) {
      draw_point(x,
                 SW_RAST_Y_REVERSAL(ysize,
                                    l3d_real_to_int
                                    (fy+float_to_l3d_real(0.5))),
                 col);
      fy = fy + m;
    }
  } //- mostly horizontal line
  else { //- mostly vertical line
    //- ensure (x0,y0) is vertically smaller than (x1,y1)
    if(y1<y0) { tmp=x0;x0=x1;x1=tmp; tmp=y0;y0=y1;y1=tmp;}
    fx = int_to_l3d_real(x0);
    if( !(int_to_l3d_real(dy) )) return; //- degenerate: line is just a point
    m = l3d_divrr( int_to_l3d_real(dx), int_to_l3d_real(dy) );
    for(y=y0; y<=y1; y++) {
      draw_point(l3d_real_to_int(fx+float_to_l3d_real(0.5)),
                 SW_RAST_Y_REVERSAL(ysize,y),
                 col);
      fx = fx + m;
    }
  } //- mostly vertical line
}
//send drawline

void l3d_rasterizer_2d_sw_imp::draw_polygon_flatshaded
(const l3d_polygon_2d_flatshaded *p_poly)
{
  l3d_real x0,y0,x1,y1,x2,y2,x3,
  left_x_start,left_y_start,left_x_end,left_y_end,left_dx,left_x,leftedge,
  right_x_start,right_y_start,right_x_end,right_y_end,right_dx,right_x,rightedge;
  int left_ceily_start, left_ceily_end,
  right_ceily_start, right_ceily_end, scanline;
  l3d_real top_y, bottom_y;
  int point_on_right=0;
  int left_idx, right_idx, top_y_idx, bottom_y_idx;

  int clipleftedge, cliprightedge;
  int maxy_upper, iceil_y0, iceil_y1, iceil_y2;

  int i;

  //- convenience macro for accessing the vertex coordinates. Notice
  //- that we access the clipped vertex index list (not the original),
  //- and the transformed coordinate (not the original). This means
  //- we draw the clipped version of the transformed polygon.
#define VTX(i) ((**(p_poly->vlist))[ (*(p_poly->clip_ivertices))[i].ivertex ].transformed)

  //-------------------------------------------------------------------------
  //- STEP 1: Find top and bottom vertices
  //-------------------------------------------------------------------------

  top_y = VTX(0).Y_;
  top_y_idx = 0;
  bottom_y = top_y;
  bottom_y_idx = top_y_idx;
  for(i=0; i<p_poly->clip_ivertices->num_items; i++) {

    if(VTX(i).Y_ < top_y) {
      top_y = VTX(i).Y_;
      top_y_idx = i;
    }
    if(VTX(i).Y_ > bottom_y) {
      bottom_y = VTX(i).Y_;
      bottom_y_idx = i;
    }
  }

  left_idx = top_y_idx;
  right_idx = top_y_idx;

  //-------------------------------------------------------------------------
  //- STEP 2: Create empty left edge
  //-------------------------------------------------------------------------
  left_x_start = VTX(top_y_idx).X_;
  left_y_start = VTX(top_y_idx).Y_;
  left_ceily_start=iceil(left_y_start);
  left_ceily_end=left_ceily_start;

  //-------------------------------------------------------------------------
  //- STEP 3: Create empty right edge
  //-------------------------------------------------------------------------
  right_x_start=left_x_start;
  right_y_start=left_y_start;
  right_ceily_start=left_ceily_start;
  right_ceily_end=right_ceily_start;

  //-------------------------------------------------------------------------
  //- STEP 4: Loop from top y to bottom y
  //-------------------------------------------------------------------------
  scanline = left_ceily_start;

  while(scanline < ysize) {

    //-------------------------------------------------------------------------
    //- STEP 5: Find next left-edge of non-zero height (if needed)
    //-------------------------------------------------------------------------
    // if needed, find next left-edge whose ceily_end > current scanline
    // this is a while and not an if because we can have edges of effectively
    // zero height, which don't count; we need to search until (if) we find
    // an edge of non-zero height.
    while( left_ceily_end - scanline <= 0 ) {
      if (left_idx == bottom_y_idx) return;  //- done
      left_idx = p_poly->next_clipidx_left
                 (left_idx, p_poly->clip_ivertices->num_items);
      left_y_end= VTX(left_idx).Y_;
      left_ceily_end = iceil(left_y_end);

      if(left_ceily_end - scanline) {  //- found next vertex
        //-------------------------------------------------------------------
        //- STEP 5a: Initialize left-x variables
        //-------------------------------------------------------------------
        left_x_end= VTX(left_idx).X_;
        left_dx = l3d_divrr(left_x_end-left_x_start,left_y_end-left_y_start);
        left_x = left_x_start +  //- sub-pixel correction
                 l3d_mulrr(int_to_l3d_real(left_ceily_start)-left_y_start , left_dx);
      } else {  //- did not find next vertex: last failed end = new start
        left_x_start = VTX(left_idx).X_;
        left_y_start = VTX(left_idx).Y_;
        left_ceily_start = iceil(left_y_start);
      }
    }

    //-------------------------------------------------------------------------
    //- STEP 6: Find next right-edge of non-zero height (if needed)
    //-------------------------------------------------------------------------
    //- if needed, find next right-edge whose ceily_end > current scanline
    //- this is a while and not an if, as desribed above...
    while(right_ceily_end - scanline <= 0 ) {
      if (right_idx == bottom_y_idx) return;  //- done
      right_idx = p_poly->next_clipidx_right
                  (right_idx,p_poly->clip_ivertices->num_items);
      right_y_end=VTX(right_idx).Y_;
      right_ceily_end = iceil(right_y_end);
      if(right_ceily_end - scanline) {  //- found next vertex
        //-------------------------------------------------------------------
        //- STEP 6a: Initialize right-x variables
        //-------------------------------------------------------------------
        right_x_end=VTX(right_idx).X_;
        right_dx =
          l3d_divrr(right_x_end-right_x_start,right_y_end-right_y_start);
        right_x = right_x_start +  //- sub-pixel correction
                  l3d_mulrr(int_to_l3d_real(right_ceily_start)-right_y_start , right_dx);
      } else {  //- did not find next vertex: last failed end = new start
        right_x_start = VTX(right_idx).X_;
        right_y_start = VTX(right_idx).Y_;
        right_ceily_start = iceil(right_y_start);
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

    //- clamp edge values to screen
    if (left_ceily_end > ysize) left_ceily_end = ysize;
    if (right_ceily_end > ysize) right_ceily_end = ysize;

    //-------------------------------------------------------------------
    //- STEP 7: Loop until left and/or right edge is finished
    //-------------------------------------------------------------------
    while ( (scanline < left_ceily_end) && (scanline < right_ceily_end) ) {
      //   (scanline < ysize) )
      //    if (left_x > right_x) return; // due to slivers, left_x can be < right_x
      clipleftedge = iceil(left_x); // round up to find the next pixel
      //whose upper-left corner is TO THE RIGHT OF the real-valued left edge.
      //This is the first pixel INSIDE the polygon.
#ifdef FIXED_POINT_MATH
      //- due to numerical inaccuracy with fixed-point math we also
      //- perform this (otherwise unnecessary) check here to clamp the
      //- edge values to the screen borders
      clipleftedge = (clipleftedge < 0) ? 0 : clipleftedge;
#endif
      cliprightedge = iceil(right_x); // round up to find the next pixel
      // whose upper-left corner is TO THE RIGHT OF the real-valued right
      // edge. This is the first pixel NOT inside the polygon, so we draw
      // up to BUT NOT INCLUDING cliprightedge.

#ifdef FIXED_POINT_MATH
      //- due to numerical inaccuracy with fixed-point math we also
      //- perform this (otherwise unnecessary) check here to clamp the
      //- edge values to the screen borders
      cliprightedge = (cliprightedge > xsize) ? xsize : cliprightedge;
#endif

      // if the segment is (horizontally) visible, draw it.

#ifdef FIXED_POINT_MATH
      //- due to numerical inaccuracy with fixed-point math we also
      //- perform this (otherwise unnecessary) check here to clamp the
      //- edge values to the screen borders
      if( (left_x<int_to_l3d_real(xsize   )) && (right_x>int_to_l3d_real(0))
          &&  (clipleftedge <= cliprightedge) && (scanline>0) )
#endif
      {
        // previously this for-loop was only used for ultra-safe;
        // normally we used to use a command of the form
        //
        //            memset(fpScreenbuf+clipleftedge+(scanline)*xsize,
        //            p_poly->final_color,
        //                   (size_t) (cliprightedge - clipleftedge));
        //
        // to set the color. but with deeper color depths and multiple
        // bytes per pixel we cannot in general use the (faster) memset
        // because we are not setting the entire region to ONE color
        // value.

        //-------------------------------------------------------------------
        //- STEP 8: Draw horizontal span
        //-------------------------------------------------------------------

//            unsigned char *pix;
//            unsigned char *endpix;
//            pix = address_of_point(clipleftedge,
//                                   SW_RAST_Y_REVERSAL(ysize,scanline));
//            endpix = address_of_point(cliprightedge,
//                                      SW_Rscanline);
          
//            for(;pix<endpix;)
//            { // *pix++=255;
//              printf("pixel at %p (%d,%d)\n",
//                     pix, (endpix-pix)/sinfo->bytes_per_pixel, scanline);
//              draw_point_at_paddress(&pix, p_poly->final_color);
//            }


          for(register char unsigned *pix =
                address_of_point(clipleftedge,
                                 SW_RAST_Y_REVERSAL(ysize,scanline));
              pix < address_of_point(cliprightedge, 
                                     SW_RAST_Y_REVERSAL(ysize,scanline));
             )
          { // *pix++=255;
            assert((pix - address_of_point(clipleftedge, SW_RAST_Y_REVERSAL(ysize,scanline))) / sinfo->bytes_per_pixel < cliprightedge);
            draw_point_at_paddress(&pix, p_poly->final_color);
          }
//          for(register int ii =clipleftedge;
//              ii < cliprightedge;
//              ii++)
//            { // *pix++=255;
//              draw_point(ii,
//                         SW_RAST_Y_REVERSAL(ysize,scanline),
//                         p_poly->final_color);
//            }
      }

      //-------------------------------------------------------------------
      //- STEP 9: Increase y, and step x-values on left and right edges
      //-------------------------------------------------------------------
      scanline++;
      left_x += left_dx;
      right_x += right_dx;
    }

    //-------------------------------------------------------------------
    //- STEP 10: Continue loop, looking for next edge of non-zero height
    //-------------------------------------------------------------------

    //- for the left and/or right segment(s) which just completed drawing
    //- initialize xxx_start = xxx_end, to begin next segment. xxx_end is
    //- then searched for in the next iteration (the while() loops)
    if ( left_ceily_end - scanline <= 0 ) {
      left_x_start=left_x_end;
      left_y_start=left_y_end;
      left_ceily_start=left_ceily_end;
    }
    if ( right_ceily_end - scanline <= 0 ) {
      right_x_start=right_x_end;
      right_y_start=right_y_end;
      right_ceily_start=right_ceily_end;
    }
  }
}
#undef VTX(i)
