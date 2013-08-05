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

#ifndef __SCACHE_H
#define __SCACHE_H
#include "../../tool_os/memman.h"

class l3d_texture;
class l3d_texture_data;

#include "../../tool_2d/scrinfo.h"
#include "../../datastr/list.h"


class l3d_polygon_3d_textured_lightmapped;


class l3d_surface {
  public:
    // for software lit polygons: combine tiled texture and surface,
    // unique for each polygon

    // for hardware lit polygons: two-pass render of texturemap and lightmap,
    // we have 2 surfaces, surface member points to texture once and lightmap
    // a second time.

    l3d_surface(void);
    const l3d_polygon_3d_textured_lightmapped *polygon;
    l3d_texture *surface;

    int age;

    virtual ~l3d_surface(void);
};

class l3d_surface_cache {
  protected:
    l3d_screen_info *sinfo;
    l3d_list<l3d_surface> *surfaces;

  public:
    l3d_surface_cache(l3d_screen_info *sinfo) {
      surfaces = new l3d_list<l3d_surface> (10);
      this->sinfo = sinfo;
    }

    virtual ~l3d_surface_cache(void) {
      int i;
      delete surfaces;
    }

    l3d_texture *combine_lightmap_and_texmap(int width,
        int height,
        const l3d_polygon_3d_textured_lightmapped *polygon);

    void reset(void) {
      delete surfaces;
      surfaces = new l3d_list<l3d_surface> (10);
    }

    l3d_texture_data *find_texdata(l3d_texture_data *texdata);
};

#include "../polygon/p3_tex.h"
#include "../polygon/p3_ltex.h"


#endif

