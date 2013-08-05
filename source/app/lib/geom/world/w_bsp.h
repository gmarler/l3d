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

#ifndef __W_BSP_H
#define __W_BSP_H
#include "../../tool_os/memman.h"

#include "../../tool_os/dispatch.h"
#include "../../view/camera.h"
#include "../../tool_2d/screen.h"
#include "../object/object3d.h"
#include "world.h"
#include "../frustum/vfrust.h"

class l3d_bsptree;
class l3d_halfspace;
class l3d_world_bsptree : public l3d_world
{
  protected:
    int visited_nodes;

  public:
    l3d_world_bsptree(int xsize, int ysize) :
    l3d_world(xsize,ysize) {};
    virtual ~l3d_world_bsptree(void);
    void render_bsp_tree(l3d_halfspace *tree, l3d_point campos,
                         l3d_viewing_frustum &frustum);

    /* virtual */ void draw_all(void); // the 3d rendering pipeline
    l3d_bsptree *the_tree;
};

#endif
