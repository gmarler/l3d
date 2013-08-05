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

#include "rast3.h"
#include "../tool_os/memman.h"

l3d_rasterizer_3d_imp::l3d_rasterizer_3d_imp
(int xs, int ys, l3d_screen_info *si) :
    l3d_rasterizer_2d_imp(xs,ys,si)
{}

void l3d_rasterizer_3d_imp::draw_polygon_textured
(const l3d_polygon_3d_textured *p_poly)
{}

void l3d_rasterizer_3d_imp::draw_polygon_textured_lightmapped
(const l3d_polygon_3d_textured_lightmapped *p_poly)
{}



