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

#ifndef __SECTOR_H
#define __SECTOR_H
#include "../../tool_os/memman.h"

#include "obound.h"
#include "../../view/camera.h"

class l3d_polygon_3d_portal_clippable;

class l3d_sector :
      public l3d_object_clippable_boundable
{
  public:
    l3d_sector(int num_fixed_vertices) :
    l3d_object(num_fixed_vertices),
    l3d_object_clippable(num_fixed_vertices),
    l3d_object_clippable_boundable(num_fixed_vertices)
    //    ,l3d_object_clippable(num_fixed_vertices),
    //    l3d_object_clippable_boundable(num_fixed_vertices)
    {
      objects = new l3d_list<l3d_object_clippable_boundable *>(1);
    };
    virtual ~l3d_sector(void);

    l3d_list<l3d_object_clippable_boundable *> *objects;
    int update(void); // virtual

    // for hierarchichal regions of space (bsp?) we could also
    // declare a series of sub-sectors of the current sector here.
    // in fact we could store sectors in the objects list since
    // sectors inherit from objects. for now though we assume
    // objects are simple, non-hierarchical objects

    virtual void render(l3d_polygon_2d *clipwin,
                        l3d_rasterizer_3d *rasterizer, l3d_camera *camera,
                        int xsize, int ysize);
    l3d_sector& operator= (const l3d_sector &r);

};

#include "../polygon/p3_clip.h"
#include "../polygon/p_cport.h"

#endif
