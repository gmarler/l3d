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

#ifndef __PI_WOR_H
#define __PI_WOR_H
#include "../tool_os/memman.h"

#include "pipeline.h"
#include "../geom/world/world.h"

class l3d_pipeline_world : public l3d_pipeline {
  protected:
    l3d_world *world;
  public:

  l3d_pipeline_world(l3d_world *w) { world = w; }
    virtual ~l3d_pipeline_world(void) {};
    void key_event(int ch); // from dispatcher
    void update_event(void); // from dispatcher
    void draw_event(void);   // from dispatcher
};

#endif
