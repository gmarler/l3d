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

#ifndef _COORD_H
#define _COORD_H
#include "../../tool_os/memman.h"

#include "../../geom/point/point.h"
const int max_transformed_intermediates = 2;
class l3d_coordinate {
  public:
  l3d_coordinate(void) { transform_stage = 0; }

    l3d_point original;
    l3d_point transformed;
    char transform_stage;

    l3d_point transformed_intermediates[max_transformed_intermediates];

    void reset(void) { transform_stage = 0; transformed = original; }
    void transform(const l3d_matrix &m, int count) {
      if (transform_stage != count) {
        transform_stage = count;
        transformed = m | transformed;
      }
    }
};

#endif
