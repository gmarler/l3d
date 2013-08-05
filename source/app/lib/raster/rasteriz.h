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

#ifndef __RASTERIZ_H
#define __RASTERIZ_H
#include "../tool_os/memman.h"

#include "../tool_2d/scrinfo.h"

class l3d_polygon;
class l3d_polygon_2d_flatshaded;

// bridge design pattern: rasterizer and implementation are separate
// class hierarchies (rasterizer_2d, rasterizer_3d form the abstraction
// hierarchy; rasterizer_2d_mesa_imp, rasterizer_2d_sw_imp,
// rasterizer_3d_mesa_imp, rasterizer_3d_sw_imp form the implementation
// hierarchy)

class l3d_rasterizer_2d_imp {
  protected:
    int xsize, ysize;
    l3d_screen_info *sinfo;
  public:
    l3d_rasterizer_2d_imp(int xs, int ys, l3d_screen_info *si);
    virtual ~l3d_rasterizer_2d_imp(void) {};

    virtual void clear_buffer(void);
    virtual void draw_point( int x ,
                             int y ,
                             unsigned long col );
    virtual void draw_line( int x0 ,
                            int y0 ,
                            int x1 ,
                            int y1 ,
                            unsigned long col );
    virtual void draw_polygon_flatshaded( const l3d_polygon_2d_flatshaded
                                          *p_poly );
};

class l3d_rasterizer_2d {
  protected:
    l3d_rasterizer_2d_imp *imp2d;

  public:
    l3d_rasterizer_2d(l3d_rasterizer_2d_imp *i) {
      imp2d = i;
    }
    virtual ~l3d_rasterizer_2d(void) {}

    // actual rasterization

    virtual void clear_buffer(void)
  { imp2d->clear_buffer(); }

    virtual void draw_point( int x ,
                             int y ,
                             unsigned long col )
  { imp2d->draw_point(x,y,col); }

    virtual void draw_line( int x0 ,
                            int y0 ,
                            int x1 ,
                            int y1 ,
                            unsigned long col )
  { imp2d->draw_line(x0,y0,x1,y1,col);}

    virtual void draw_polygon_flatshaded( const l3d_polygon_2d_flatshaded
                                          *p_poly )
  { imp2d->draw_polygon_flatshaded(p_poly); }
};

class l3d_rasterizer_2d_imp_factory {
  public:
    virtual l3d_rasterizer_2d_imp *create(int xs, int ys, l3d_screen_info *si)=0;
};

#include "../geom/polygon/polygon.h"
#include "../geom/polygon/p_flat.h"

#endif

