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

#ifndef __RAS_MESA_H
#define __RAS_MESA_H
#include "../tool_os/memman.h"

#include <GL/glut.h>
#include "rasteriz.h"

class l3d_rasterizer_2d_mesa_imp :
      virtual public l3d_rasterizer_2d_imp
{
  public:
    l3d_rasterizer_2d_mesa_imp(int xs, int ys, l3d_screen_info *si);

    void clear_buffer(void);
    void draw_point(int x, int y, unsigned long col);
    void draw_line(int x0, int y0, int x1, int y1, unsigned long col);
    void draw_polygon_flatshaded(const l3d_polygon_2d_flatshaded *p_poly);
};

class l3d_rasterizer_2d_mesa_imp_factory :
  public l3d_rasterizer_2d_imp_factory {
  public:
    l3d_rasterizer_2d_imp *create(int xs, int ys, l3d_screen_info *si); // virt
};

#endif
