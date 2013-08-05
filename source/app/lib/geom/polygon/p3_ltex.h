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

#ifndef __P3_LTEX_H
#define __P3_LTEX_H
#include "../../tool_os/memman.h"

#include "p3_tex.h"
#include "../surface/scache.h"

class l3d_polygon_3d_textured_lightmapped :
      virtual public l3d_polygon_3d_textured,
      virtual public l3d_texture_computer // must recompute O,U,V for lightmap
{
  protected:
    l3d_surface_cache *scache;
  public:
    mutable l3d_texture *surface;
    //- mutable since the draw routine for s/w rendering needs to assign
    //- this member (the tiled, lit surface) if the surface is not currently
    //- in the cache, though the poly is otherwise passed as const. logically
    //- the draw routine handles the polygon as a const, physically though
    //- it can change the mutable surface member

    l3d_surface_cache *get_scache(void) const { return scache; }

    l3d_texture_space surface_orientation;
    int surface_xsize, surface_ysize;// total surface size, rounded-up-to pow2

    l3d_polygon_3d_textured_lightmapped(int num_pts, l3d_surface_cache *scache);

    ~l3d_polygon_3d_textured_lightmapped(void) {
      //- DO NOT delete the surface! it belongs to the surface cache,
      //- or might not even have been created if h/w accel with 2-pass
      //- rendering was used

      delete lightmap;
    }

    //  int surface_used_u0, surface_used_v0,
    //    surface_used_u1, surface_used_v1; // total USED area in surface
    // (only the used part is lit, i.e. the lightmap is not applied to
    // the entire (tiled) surface, only to that part which is actually used)
    //l3d_texture_space lightmap_orientation;
    l3d_texture *lightmap;

    void compute_surface_orientation_and_size(void);
    //  void compute_surface_size_from_existing_uv_coords(void);

    void init_transformed(void);
    void transform(const l3d_matrix &m, int count);

    void draw(l3d_rasterizer_3d *r ) {
      r->draw_polygon_textured_lightmapped(this);
    }

    l3d_polygon_3d_textured_lightmapped
    (const l3d_polygon_3d_textured_lightmapped &r);
    l3d_polygon_2d *clone(void);

};

#endif
