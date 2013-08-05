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

#ifndef __RAS3Z_SW_H
#define __RAS3Z_SW_H
#include "../tool_os/memman.h"

#include "ras3_sw.h"
#include "math.h"
#include "../system/sys_dep.h"

////////////////////////////////////////////////////////////////////////////

class l3d_rasterizer_3d_zbuf_sw_imp :
      virtual public l3d_rasterizer_3d_sw_imp
{
  private:
    l3d_fixed *zbuf;
    long int zbuf_size;
  public:
    l3d_rasterizer_3d_zbuf_sw_imp(int xs, int ys, l3d_screen_info *si);
    virtual ~l3d_rasterizer_3d_zbuf_sw_imp(void);

    /* virtual */ void draw_polygon_textured(const l3d_polygon_3d_textured *p_poly);

    // #ifdef DELME
    //  /* virtual */ void draw_polygon_textured_lightmapped
    //
    //    (const l3d_polygon_3d_textured_lightmapped *p_poly);
    //#endif

    /* virtual */
    void clear_buffer(void);
};

class l3d_rasterizer_3d_zbuf_sw_imp_factory :
      public l3d_rasterizer_3d_imp_factory
{
  public:
    l3d_rasterizer_3d_imp *create(int xs, int ys, l3d_screen_info *si);
};

#endif
