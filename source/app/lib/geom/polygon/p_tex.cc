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

#include "p_tex.h"
#include "../../tool_os/memman.h"

l3d_polygon_ivertex * l3d_polygon_3d_textured::create_ivertices(int count) {
  return new l3d_polygon_ivertex_textured[count];
}

size_t l3d_polygon_3d_textured::sizeof_ivertex(void) {
  return sizeof(l3d_polygon_ivertex_textured);
}

void l3d_polygon_3d_textured::init_transformed(void) {
  l3d_polygon_3d::init_transformed();

  texture->O.transformed = texture->O.original;
  texture->U.transformed = texture->U.original;
  texture->V.transformed = texture->V.original;
}

void l3d_polygon_3d_textured::transform(const l3d_matrix &m, int count) {
  l3d_polygon_3d::transform(m, count);

  // we don't bother to apply the "count" parameter to the O,U, and V
  // vectors since these are not shared components and thus cannot possibly
  // have already been transformed.
  texture->O.transformed = m | texture->O.transformed;
  texture->U.transformed = m | texture->U.transformed;
  texture->V.transformed = m | texture->V.transformed;
}

FIXME DONT USE THIS FILE












