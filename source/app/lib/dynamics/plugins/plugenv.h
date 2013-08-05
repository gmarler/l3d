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

#ifndef __PLUGENV_H
#define __PLUGENV_H
#include "../../tool_os/memman.h"

#include "../../system/sys_dep.h"
#include "../../geom/object/object3d.h"
#include "../../geom/texture/texload.h"
#include "../../geom/polygon/p3_ltex.h"

// all data which a plugin needs to know about its environment in order
// to be able to initialize itself

class l3d_plugin_environment {
  public:
    l3d_texture_loader *texloader;
    l3d_screen_info *sinfo;
    l3d_surface_cache *scache;
    void *data;
    l3d_plugin_environment::l3d_plugin_environment(l3d_texture_loader *l,
        l3d_screen_info *sinfo,
        l3d_surface_cache *scache,
        void *data)
    {
      texloader = l;
      this->sinfo = sinfo;
      this->scache = scache;
      this->data = data;
    }

    virtual l3d_plugin_environment *clone(void) {
      l3d_plugin_environment *e = new l3d_plugin_environment
                                  (texloader, sinfo, scache, NULL);
      // e->data is separately copied by plugin itself
      return e;
    }
};


#endif
