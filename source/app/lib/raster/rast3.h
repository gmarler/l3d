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

#ifndef __RAST3_H
#define __RAST3_H
#include "../tool_os/memman.h"

#include "rasteriz.h"
#include "../tool_2d/scrinfo.h"

#include "../geom/polygon/p3_flat.h"
#include "../geom/texture/texture.h"

//  /*
//  class l3d_polygon_3d_flatshaded :
//  virtual public l3d_polygon_2d_flatshaded,
//  virtual public l3d_polygon_3d;
//  */

class l3d_polygon_3d_textured;
class l3d_polygon_3d_textured_lightmapped;

class l3d_rasterizer_3d_imp :
      virtual public l3d_rasterizer_2d_imp,
      virtual public l3d_texture_computer
{
  public:
    l3d_rasterizer_3d_imp(int xs, int ys, l3d_screen_info *si);

    /* virtual */ void clear_buffer(void) {};

    virtual void draw_polygon_textured(const l3d_polygon_3d_textured *p_poly);
    virtual void draw_polygon_textured_lightmapped
    (const l3d_polygon_3d_textured_lightmapped *p_poly);
    virtual void draw_text(int x, int y, const char *string) {};
    virtual void set_text_color(int red, int green, int blue) {};
};


class l3d_rasterizer_3d : public l3d_rasterizer_2d {
  protected:
    l3d_rasterizer_3d_imp *imp3d;

  public:
    l3d_rasterizer_3d(l3d_rasterizer_2d_imp *i2,
                      l3d_rasterizer_3d_imp *i3) :
    l3d_rasterizer_2d(i2)
    {
      imp3d = i3;
    }
    virtual ~l3d_rasterizer_3d(void) {}

    // overridden from l3d_rasterizer_2d: we pass along the
    // request to the imp3d, not the imp2d! this is important
    // for mesa, where the imp2d has a lower-left origin,
    // but the imp3d has a center origin (required due to the
    // w-coordinate passing required for persp. correct textures)
    void draw_point( int x , int y ,  unsigned long col ) {
      imp3d->draw_point(x,y,col);
    }

    // overridden from l3d_rasterizer_2d: we pass along the
    // request to the imp3d, not the imp2d! this is important
    // for mesa, where the imp2d has a lower-left origin,
    // but the imp3d has a center origin (required due to the
    // w-coordinate passing required for persp. correct textures)
    /* virtual */ void draw_line( int x0 , int y0 , int x1 ,  int y1 , unsigned long col ) {
      imp3d->draw_line(x0,y0,x1,y1,col);
    }

    /* virtual */ void clear_buffer(void) {
      imp3d->clear_buffer();
    }

    /* virtual */ void draw_polygon_flatshaded( const l3d_polygon_3d_flatshaded
        *p_poly )
  { imp3d->draw_polygon_flatshaded(p_poly); }

    virtual void draw_polygon_textured(const l3d_polygon_3d_textured *p_poly)
  { imp3d->draw_polygon_textured(p_poly); }

    virtual void draw_polygon_textured_lightmapped
    (const l3d_polygon_3d_textured_lightmapped *p_poly)
  { imp3d->draw_polygon_textured_lightmapped(p_poly); }

    void draw_text(int x, int y, const char *string)
  { imp3d->draw_text(x, y, string); }


};

class l3d_rasterizer_3d_imp_factory {
  public:
    virtual l3d_rasterizer_3d_imp *create(int xs, int ys, l3d_screen_info *si)=0;
};

//  /*
//  a #include "../geom/polygon/poly3.h"
//  a #include "../geom/polygon/p3_flat.h"
//  */


#ifndef __ACTIVE__P3_TEX_H
#ifndef __ACTIVE__P3_CLIP_H
#include "../geom/polygon/p3_tex.h"
#include "../geom/polygon/p3_ltex.h"
#endif
#endif

#endif
