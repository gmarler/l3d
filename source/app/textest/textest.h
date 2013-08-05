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

#ifndef __TEXTEST_H
#define __TEXTEST_H

#include "../lib/geom/world/world.h"
#include "../lib/pipeline/pi_wor.h"
#include "../lib/geom/texture/texture.h"
#include "../lib/geom/texture/tl_ppm.h"

class my_world:public l3d_world {
  public:
    my_world(void);
    void update_all(void);

    virtual ~my_world(void) { delete texture_loader; }
    // no destructor necessary for objects created in constructor;
    // base class destructor takes care of these.
    l3d_texture_loader *texture_loader;
};

#endif
