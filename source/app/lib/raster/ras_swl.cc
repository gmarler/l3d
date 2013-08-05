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

#include "ras_swl.h"
#include "../system/sys_dep.h"
#include <string.h>
#include <stdlib.h>
#include "../tool_os/memman.h"

int l3d_rasterizer_2d_sw_lighter_imp::compute_light_level(int lumel_x, int lumel_y)
{
  l3d_real x,y,z;

  l3d_real u,v;

  // convert raster lumel x,y coords (0..64,0..64) to texture space
  // u,v coords (0..1,0..1)

  u = l3d_divrr( int_to_l3d_real(lumel_x), xsize );
  v = l3d_divrr( int_to_l3d_real(lumel_y), ysize );

  // from tex space u,v coords, convert to world space x,y,z

  x = O.X_ + l3d_mulri(U.X_, u) + l3d_mulri(V.X_, v);
  y = O.Y_ + l3d_mulri(U.Y_, u) + l3d_mulri(V.Y_, v);
  z = O.Z_ + l3d_mulri(U.Z_, u) + l3d_mulri(V.Z_, v);

  // x,y,z are now the world-space coordinates of the lumel under
  // consideration

  l3d_vector aLight(0,0,0,1); // light at current camera position = origin
  // (since we only rasterize sublightmaps when we have placed the light
  // by interactive positioning via key 'P')

  // we define 2 vectors: L, the vector from the facet (more precisely from
  // the facet's center) to the light source, and N, the facet's normal
  // vector. Note that vector FROM facet center TO light source is
  // computed as light_source - facet_center and not vice versa.
  // also note that the surface normal vector is defined as starting at the
  // (transformed) facet center and not at the origin. (If we defined N
  // just as aFacet.xfrm_sfcnormal.X_, Y_, Z_), this would implicitly
  // assume that N starts at the origin, which is not the case.)

  l3d_vector L(aLight.X_ - x ,
               aLight.Y_ - y ,
               aLight.Z_ - z ,
               int_to_l3d_real(0)),

  N(cross(U,V));
  //   N(cross(V,U)); // U,V is correct but as long as the UV
  // are mirrored we correct for this here FIXME

  l3d_real intensity;

  // calculate f_att attenuation factor to reduce sharp fall-off of
  // light intensity based on distance; this makes light more gradually
  // dim as objects are further from the light source. (Foley/van Dam/
  // Feiner/Hughes, Equation 16.8)

  l3d_real f_att=l3d_divrr(int_to_l3d_real(1) ,
                           (float_to_l3d_real(0.1)+float_to_l3d_real(0.01*sqrt(l3d_real_to_float(dot(L,L))))));
  if (f_att>int_to_l3d_real(1)) f_att=int_to_l3d_real(1);

  l3d_real n_dot_l;
  n_dot_l = dot(normalized(N), normalized(L));
  //     n_dot_l = l3d_mulrr(n_dot_l, float_to_l3d_real(1.2));
  //     if(n_dot_l > int_to_l3d_real(1)) n_dot_l = int_to_l3d_real(1);
  //     if(n_dot_l < int_to_l3d_real(-1)) n_dot_l = int_to_l3d_real(-1);

  // calculate intensity value (0-15) for facet
  intensity = l3d_mulrr ( l3d_mulrr(int_to_l3d_real(128) , f_att) ,
                          n_dot_l);
  if (intensity>int_to_l3d_real(128)) { intensity = int_to_l3d_real(128); }
  if (intensity<int_to_l3d_real(2))  { intensity = int_to_l3d_real(2);  }

  // apply intensity value (brightness) to existing facet color
  // we store both the intensity and colored intensity to save computation
  // time later
  return l3d_real_to_int(intensity);
}


void l3d_rasterizer_2d_sw_lighter_imp::draw_polygon_flatshaded
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

#define XPAD 4
#define YPAD 1
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

  int first_scanline_done = 0;
  while(scanline <= ysize) {

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
      if(! first_scanline_done) {
        first_scanline_done = 1;

        int top_pad;
        top_pad = scanline -YPAD;
        if(top_pad < 0) top_pad = 0;
        int target = scanline;

        for(scanline=top_pad ; scanline < target; scanline++) {
          //          scanline--;
          //          left_x -= left_dx;
          //          right_x -= right_dx;


          clipleftedge = ifloor(left_x)-XPAD; // round down to find the next pixel
          //whose upper-left corner is TO THE RIGHT OF the real-valued left edge.
          //This is the first pixel INSIDE the polygon.

          clipleftedge = (clipleftedge < 0) ? 0 : clipleftedge;
          cliprightedge = iceil(right_x)+XPAD; // round up to find the next pixel
          // whose upper-left corner is TO THE RIGHT OF the real-valued right
          // edge. This is the first pixel NOT inside the polygon, so we draw
          // up to BUT NOT INCLUDING cliprightedge.

          cliprightedge = (cliprightedge > xsize-1) ? xsize-1 : cliprightedge;

          int lumel_x = clipleftedge;
          for(register char unsigned *pix =
                address_of_point(clipleftedge,
                                 SW_RAST_Y_REVERSAL(ysize,scanline));

              pix <= address_of_point(cliprightedge,
                                      SW_RAST_Y_REVERSAL(ysize,scanline));  // NOTE!
              // here we draw upto and including and past the last point, since
              // we want to fill the lightmap including BOTH borderss

              lumel_x++

             )
          { // *pix++=255;
            assert((pix - address_of_point(clipleftedge, SW_RAST_Y_REVERSAL(ysize,scanline))) / sinfo->bytes_per_pixel < cliprightedge);
            int light_value = compute_light_level(lumel_x, scanline);

            light_value += *pix;
            if(light_value > 255) light_value = 255;
            draw_point_at_paddress(&pix, light_value);
          }

          left_x+= left_dx;
          right_x+= right_dx;
        }

      }
      //      printf("scanline %d\n", scanline);
      //   (scanline < ysize) )
      //    if (left_x > right_x) return; // due to slivers, left_x can be < right_x

      // unlike with normal rasterization we round left down, right up, to
      // make sure we cover ALL lumels that in any way touch the poly.
      // we also do time-consuming clamping since this is not done in real-time

      clipleftedge = ifloor(left_x)-XPAD; // round down to find the next pixel
      //whose upper-left corner is TO THE RIGHT OF the real-valued left edge.
      //This is the first pixel INSIDE the polygon.

      clipleftedge = (clipleftedge < 0) ? 0 : clipleftedge;
      cliprightedge = iceil(right_x)+XPAD; // round up to find the next pixel
      // whose upper-left corner is TO THE RIGHT OF the real-valued right
      // edge. This is the first pixel NOT inside the polygon, so we draw
      // up to BUT NOT INCLUDING cliprightedge.

      cliprightedge = (cliprightedge > xsize-1) ? xsize-1 : cliprightedge;

      // if the segment is (horizontally) visible, draw it.

      //
      //         if( (left_x<int_to_l3d_real(xsize   )) && (right_x>int_to_l3d_real(0))
      //         &&  (clipleftedge <= cliprightedge) && (scanline>0) )
      //
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
        int lumel_x = clipleftedge;
        for(register char unsigned *pix =
              address_of_point(clipleftedge,
                               SW_RAST_Y_REVERSAL(ysize,scanline));

            pix <= address_of_point(cliprightedge, 
                                    SW_RAST_Y_REVERSAL(ysize,scanline));  // NOTE!
            // here we draw upto and including and past the last point, since
            // we want to fill the lightmap including BOTH borderss

            lumel_x++

           )
        { // *pix++=255;
          assert((pix - address_of_point(clipleftedge, SW_RAST_Y_REVERSAL(ysize,scanline))) / sinfo->bytes_per_pixel < cliprightedge);
          int light_value = compute_light_level(lumel_x, scanline);

          light_value += *pix;
          if(light_value > 255) light_value = 255;
          draw_point_at_paddress(&pix, light_value);
        }
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


  for(int bottom_pad=0; bottom_pad<YPAD; bottom_pad++) {
    if(scanline < ysize-1) {
      scanline++;
      left_x += left_dx;
      right_x += right_dx;


      clipleftedge = ifloor(left_x)-XPAD; // round down to find the next pixel
      //whose upper-left corner is TO THE RIGHT OF the real-valued left edge.
      //This is the first pixel INSIDE the polygon.

      clipleftedge = (clipleftedge < 0) ? 0 : clipleftedge;
      cliprightedge = iceil(right_x)+XPAD; // round up to find the next pixel
      // whose upper-left corner is TO THE RIGHT OF the real-valued right
      // edge. This is the first pixel NOT inside the polygon, so we draw
      // up to BUT NOT INCLUDING cliprightedge.

      cliprightedge = (cliprightedge > xsize-1) ? xsize-1 : cliprightedge;

      int lumel_x = clipleftedge;
      for(register char unsigned *pix =
            address_of_point(clipleftedge,
                             SW_RAST_Y_REVERSAL(ysize,scanline));

          pix <= address_of_point(cliprightedge, 
                                  SW_RAST_Y_REVERSAL(ysize,scanline));  // NOTE!
          // here we draw upto and including and past the last point, since
          // we want to fill the lightmap including BOTH borderss

          lumel_x++

         )
      { // *pix++=255;
        assert((pix - address_of_point(clipleftedge, SW_RAST_Y_REVERSAL(ysize,scanline))) / sinfo->bytes_per_pixel < cliprightedge);
        int light_value = compute_light_level(lumel_x, scanline);

        light_value += *pix;
        if(light_value > 255) light_value = 255;
        draw_point_at_paddress(&pix, light_value);
      }

      scanline++;
      left_x++;
      right_x++;
    }
  }


}
#undef VTX(i)
