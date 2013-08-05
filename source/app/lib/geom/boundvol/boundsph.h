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

#ifndef __BOUNDSPH_H
#define __BOUNDSPH_H
#include "../../tool_os/memman.h"

#include "../../system/sys_dep.h"
#include "../plane/plane.h"
#include "../../math/matrix.h"
#include "../vertex/coord.h"
class l3d_object;

class l3d_bounding_sphere {
  public:
    int transform_stage;
    virtual void reset(void);
    virtual void transform(const l3d_matrix &xform);

    l3d_real radius;
    l3d_coordinate center;
    l3d_real distance_to_plane(const l3d_plane & plane) const;
    void compute_around_object(const l3d_object *object);
};

#endif
