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


#ifndef __WORLD_H
#define __WORLD_H
#include "../../tool_os/memman.h"

#include "../../tool_os/dispatch.h"
#include "../../view/camera.h"
#include "../../tool_2d/screen.h"
#include "../object/object3d.h"

#define MAX_VISIBLE_FACETS 5000
#define MAX_OBJECTS 12000

class l3d_world {

  public:
    l3d_world(int xsize, int ysize);
    virtual ~l3d_world(void);

    l3d_screen *screen;
    l3d_rasterizer_3d *rasterizer;
    l3d_rasterizer_2d_imp *rasterizer_2d_imp;
    l3d_rasterizer_3d_imp *rasterizer_3d_imp;
    l3d_camera *camera;   // camera to specify a view location and orientation

    // object database
    l3d_list<l3d_object *> objects;

    // pipeline related methods

    virtual void update_all(void); // call virtual method "update" for all obj
    virtual void draw_all(void); // the 3d rendering pipeline

    static int compare_facet_zdepth(const void *key, const void *elem);
    // used for qsort in painter's algorithm

    int should_display_status;
    // timing
    unsigned int framecount, first_tickcount, last_tickcount;
};

#endif
