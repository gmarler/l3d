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

#ifndef __POLY3_H
#define __POLY3_H
#include "../../tool_os/memman.h"

#include "polygon.h"

class l3d_rasterizer_3d;

class l3d_polygon_3d :
      virtual public l3d_polygon_2d
{
  public:
  l3d_polygon_3d(void) : l3d_polygon_2d() {};
    // this constructor is called as a base class constructor by derived
    // classes to get an "empty" l3d_polygon which is then initialized
    // within the derived class's constructor

    l3d_polygon_3d(int num_pts) : l3d_polygon_2d(num_pts) {} ;
    // this constructor is called by clients to get a "real" l3d_polygon,
    // creating the vertices and all

    virtual ~l3d_polygon_3d(void) {};

    l3d_coordinate sfcnormal;  // normal vector to the facet
    l3d_coordinate center;  // center point of the facet
    void compute_sfcnormal();
    void compute_center();
    l3d_real zvisible; // average z value after viewing transformation (for
    // painters' algorithm)
    virtual void init_transformed(void); // initialize all "transformed"
    // components to be the "initial" value
    virtual void transform(const l3d_matrix &m, int count);
    virtual void set_transform_stage(int count);
    virtual void after_camera_transform(void);
    void camera_transform(const l3d_matrix &m, int count) {
      transform(m, count);
      after_camera_transform();
    }
    virtual void clip_segment_to_near_z
    (l3d_polygon_ivertex *ivertex,
     l3d_real zclip, int idx0, int idx1);
    virtual int clip_near_z(l3d_real zclip);
    virtual void draw(l3d_rasterizer_3d *r) {}
    l3d_polygon_3d(const l3d_polygon_3d &r);
    l3d_polygon_2d *clone(void); // overridden virtual with relaxed return type
    // oops, gcc 2.95 doesn't support relaxing the return type, so we return
    // from the more general superclass type declared in the superclass
};

//sbegin polynode
struct l3d_polygon_3d_node {
  l3d_polygon_3d *polygon;
  l3d_polygon_3d_node *prev,*next;
};
//send polynode

#include "../../raster/rast3.h"

#endif
