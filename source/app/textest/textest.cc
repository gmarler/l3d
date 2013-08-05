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

#include "textest.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

#include "../lib/geom/object/object3d.h"
#include "../lib/tool_2d/screen.h"
#include "../lib/tool_os/dispatch.h"
#include "../lib/raster/rasteriz.h"
#include "../lib/tool_2d/scrinfo.h"
#include "../lib/geom/world/world.h"
#include "../lib/geom/texture/texture.h"
#include "../lib/geom/texture/tl_ppm.h"

#include "shapes.h"

void my_world::update_all(void) {
  l3d_world::update_all();
}

my_world::my_world(void)
    : l3d_world(400,300)
{
  rasterizer_3d_imp->fovx = &(camera->fovx);
  rasterizer_3d_imp->fovy = &(camera->fovy);
  rasterizer_3d_imp->screen_xsize = &(screen->xsize);
  rasterizer_3d_imp->screen_ysize = &(screen->ysize);

  l3d_screen_info *si = screen->sinfo;
  texture_loader = new l3d_texture_loader_ppm(screen->sinfo);

  camera->VRP.set(0,0,-50,0);
  camera->near_z = float_to_l3d_real(5.5);
  camera->far_z = int_to_l3d_real(500);

  int i,j,k,onum;

  i=10; j=0; k=20;
  k=0;

  for(i=1; i<10; i+=5) {
    pyramid *p;
    objects[onum = objects.next_index()] = p = new pyramid(texture_loader);
    p->dx_theta = i-1;

    if (objects[onum]==NULL) exit;
    objects[onum]->modeling_xforms[1].set
    ( int_to_l3d_real(1), int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(-i),
      int_to_l3d_real(0), int_to_l3d_real(1), int_to_l3d_real(0), int_to_l3d_real(i/2),
      int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(1), int_to_l3d_real(i),
      int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(1) );
    objects[onum]->modeling_xform =
      objects[onum]->modeling_xforms[1] |
      objects[onum]->modeling_xforms[0] ;
    onum++;
  }

  screen->refresh_palette();
  screen->sinfo->compute_light_table();
  screen->sinfo->compute_fog_table();
}

