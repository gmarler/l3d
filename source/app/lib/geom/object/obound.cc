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

#include "obound.h"
#include "../boundvol/boundsph.h"
#include "../../tool_os/memman.h"

l3d_object_clippable_boundable::l3d_object_clippable_boundable
(int num_fixed_vertices)
    :
    l3d_object_clippable(num_fixed_vertices),
    l3d_object(num_fixed_vertices)
{};

void l3d_object_clippable_boundable::transform(const l3d_matrix &m) {
  l3d_object_clippable::transform(m);

  //- the bounding sphere MAY have been transformed before the geometry,
  //- if we used it as part of a culling operation, in which case the transform
  //- stage of the sphere will already equal the newly updated object's
  //- transform stage. OR, the bounding sphere may NOT have been transformed
  //- yet, if the bounding sphere is to be used LATER for collision detection.
  //- thus we must check to see if the bounding sphere has already been
  //- transformed; if not, then we transform it.
  if(bounding_sphere.transform_stage != transform_stage) {
    assert(bounding_sphere.transform_stage == transform_stage - 1);
    bounding_sphere.transform(m);
    assert(bounding_sphere.transform_stage==transform_stage || (!printf("sph %d stage %d\n",bounding_sphere.transform_stage, transform_stage)));
  }
}

void l3d_object_clippable_boundable::reset(void) {
  l3d_object_clippable::reset();

  bounding_sphere.reset();
  assert(bounding_sphere.transform_stage==transform_stage || (!printf("sph %d stage %d\n",bounding_sphere.transform_stage, transform_stage)));

}

