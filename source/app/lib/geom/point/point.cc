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

#include "point.h"
#include "../../tool_os/memman.h"

//- Global pool of temporary variables for point operations.
//
// When declaring expressions using overloaded point operators, many temp
// objects might be created. To avoid the large overhead of dynamically
// creating and destroying temporary objects when evaluating expressions,
// we have declared all overloaded operator functions to return
// *references* to point objects instead of returning new (i.e.
// to-be-created) point objects. Returning a reference means that
// the operator itself must make sure that a non-temporary variable exists
// whose reference can be returned.
//
// The l3d_get_point_temp function serves exactly this purpose. It gets
// the next free temporary point from a global pool (statically declared)
// of l3d_point_objects. Whenever an operator needs to return a (reference
// to) a new result l3d_point object, it grabs one from the pre-allocated
// pool, instead of allowing a new one to be created dynamically (which
// would occur if we returned a point object and not a reference to one).
//
// This will crash and burn majorly if we have an expression which needs
// more than l3d_max_point_temp number of temporaries.

const int l3d_max_point_temp = 15;
l3d_point &l3d_get_point_temp(void) {
  static int nbuf=0;
  static l3d_point buf[l3d_max_point_temp];

  if(nbuf>=l3d_max_point_temp) nbuf=0;
  return buf[nbuf++];
}
