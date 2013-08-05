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

#include "pi_lwor.h"

#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include "../geom/world/w_porlotex.h"
#include "../tool_os/memman.h"

void l3d_pipeline_world_lightmapped::key_event(int c) {
  l3d_pipeline_world::key_event(c);

  l3d_world_portal_textured_lightmapped_obj *wtlo;
  wtlo = dynamic_cast<l3d_world_portal_textured_lightmapped_obj *>(world);

  switch(c) {
  case 'p': case 'P':
      // place lamp
      if (wtlo) { wtlo->place_lamp(wtlo->current_sector, NULL); }
      break;

  case 'R': case 'r':
      // reset surface cache
      if(wtlo) {
        wtlo->reset_surface_cache();
      }
      break;


  }
}
