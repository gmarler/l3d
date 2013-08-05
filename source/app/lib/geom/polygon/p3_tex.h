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

#ifndef __P3_TEX_H
#define __P3_TEX_H
#include "../../tool_os/memman.h"

#define __ACTIVE__P3_TEX_H

#include "poly3.h"
#include "p3_clip.h"
#include "../texture/texture.h"

class l3d_polygon_ivertex_textured : public l3d_polygon_ivertex {
  public:
    l3d_point tex_coord;
    virtual ~l3d_polygon_ivertex_textured(void) {
      // printf("iv_tex dest\n");
    }

    virtual l3d_polygon_ivertex& operator= (const l3d_polygon_ivertex &r) {
      //        printf("virtual ivertex textured assignment\n");
      ivertex = r.ivertex;
      assert(dynamic_cast<l3d_polygon_ivertex_textured *>( (l3d_polygon_ivertex *) (&r)));
      tex_coord = ((l3d_polygon_ivertex_textured *) &r)->tex_coord;
      //    printf("was virtual ivertex textured assignment of Z %f\n",
      //           tex_coord.Z_);
      // WARNING! The above cast assumes we ONLY assign itextured_vertex objects
      // to other itextured_vertex objects. An attempt to combine two different
      // ivertex subclasses together via assignment will certainly be disastrous
      // because of the hard cast above!

      // The only reason we do this at all is to have an "ivertex" compatible
      // virtual assignment interface, so that all the assignment operations
      // in l3d_polygon_2d will also work transparently with "ivertex_texture"
      // objects. The idea is NOT to mix and match different types of
      // "ivertex" objects, only to allow existing assignments in l3d_polygon_2d
      // to work with a wide range of ivertex objects.

      return *this;
    }

    int clip_to_plane(l3d_plane& clip_plane);

};

class l3d_polygon_ivertex_textured_items_factory :
      public l3d_polygon_ivertex_items_factory
{
    l3d_polygon_ivertex *create(void) {
      return new l3d_polygon_ivertex_textured;
    }

    // virtual
    l3d_polygon_ivertex_items_factory *clone(void);
};

class l3d_polygon_3d_textured :
      virtual public l3d_polygon_3d,
      virtual public l3d_polygon_3d_clippable,
      virtual public l3d_texture_computer
{
  public:
    l3d_polygon_3d_textured(void) :
    l3d_polygon_2d(),
    l3d_polygon_3d()
    {
    }

    l3d_polygon_3d_textured(int num_pts);

    mutable l3d_texture *texture;
    void assign_tex_coords_from_tex_space(l3d_texture_space const &tex_space);

    void init_transformed(void);
    void transform(const l3d_matrix &m, int count);
    void draw(l3d_rasterizer_3d *r ) {
      r->draw_polygon_textured(this);
    }

    l3d_polygon_3d_textured(const l3d_polygon_3d_textured &r);
    virtual ~l3d_polygon_3d_textured(void);
    l3d_polygon_2d *clone(void);

    /* virtual */ void clip_segment_to_edge_2d
    (l3d_polygon_ivertex *ivertex,
     const l3d_point *pC0, const l3d_point *pC1,
     int crossedge_idx0,int crossedge_idx1);
    /* virtual */ int clip_to_edge_2d
    (const l3d_point *pPoint1, const l3d_point *pPoint2);
    /* virtual */ void after_camera_transform(void);
    /* virtual */ void clip_segment_to_near_z
    (l3d_polygon_ivertex *ivertex,
     l3d_real zclip, int idx0, int idx1);
    /* virtual */ int clip_near_z(l3d_real zclip);
    /* virtual */ int clip_to_plane(l3d_plane& clip_plane);

    // l3d_fixed dui_dx, dvi_dx, dzi_dx, dui_dy, dvi_dy, dzi_dy;
    //  void compute_projected_tex_deltas(l3d_real fovx, l3d_real fovy);
};

#undef __ACTIVE__P3_TEX_H

#endif
