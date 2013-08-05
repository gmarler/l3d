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

#include "ocoll.h"

l3d_object_clippable_boundable_collidable::
l3d_object_clippable_boundable_collidable
(int num_fixed_vertices)
    :
    l3d_object_clippable_boundable(num_fixed_vertices),
    l3d_object_clippable(num_fixed_vertices),
    l3d_object(num_fixed_vertices)
{
  //  old_position_already_saved = 0;
};

void l3d_object_clippable_boundable_collidable::reset(void) {
  //  old_position_already_saved = 0;
  l3d_object_clippable_boundable::reset();

  l3d_point pos;
  pos.set(modeling_xform.a[0][3],
          modeling_xform.a[1][3],
          modeling_xform.a[2][3],
          modeling_xform.a[3][3]);
  l3d_collidable::old_position = pos;
  l3d_collidable::new_position = pos;

}
