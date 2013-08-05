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

#ifndef __OCOLL_H
#define __OCOLL_H

#include "obound.h"
#include "../../dynamics/colsph.h"

class l3d_object_clippable_boundable_collidable:
      virtual public l3d_object_clippable_boundable,
      virtual public l3d_collidable_sphere
{
    // protected:
    //  int old_position_already_saved;

  public:
    l3d_object_clippable_boundable_collidable(int num_fixed_vertices);

    void reset(void); //- overridden virtual
};

#endif
