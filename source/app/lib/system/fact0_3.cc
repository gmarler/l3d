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

#include "fact0_3.h"

#include <stdio.h>

#include "../tool_os/dis_x11.h"
#include "../tool_os/dis_mesa.h"
#include "../tool_2d/sc_x11.h"
#include "../tool_2d/sc_x11sh.h"
#include "../tool_2d/sc_mesa.h"
#include "../raster/ras3_sw.h"
#include "../raster/ras3_mes.h"
#include "../dynamics/plugins/pl_linux.h"
#include "../raster/ras3z_sw.h"
#include "../raster/ras3z_mes.h"
#include "../tool_2d/sc_zmesa.h"
#include "../tool_os/memman.h"

l3d_factory_manager_v_0_3 factory_manager_v_0_3;

l3d_factory_manager_v_0_3::l3d_factory_manager_v_0_3(void) :
    l3d_factory_manager_v_0_2()
{
}

l3d_factory_manager_v_0_3::~l3d_factory_manager_v_0_3(void)
{
}

int l3d_factory_manager_v_0_3::choose_factories(void) {

  char input[80];
  int i;

  printf("which configuration?\n");
  printf("1. Software X11\n");
  printf("2. Software X11 with SHM extension\n");
  printf("3. Mesa (with 3DFX h/w accel if installed with Mesa)\n");
  printf("4. Software X11 with z buffer and fog\n");
  printf("5. Software X11 with SHM, z buffer, and fog\n");
  printf("6. Mesa with z buffer\n");
  gets(input);
  sscanf(input,"%d", &i);
  switch(i) {
    case 1:
      screen_factory = new l3d_screen_factory_x11;
      ras_2d_imp_factory = new l3d_rasterizer_2d_sw_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_x11;
      ras_3d_imp_factory = new l3d_rasterizer_3d_sw_imp_factory;
      plugin_loader_factory = new l3d_plugin_loader_linux_factory;
      break;
    case 2:
      screen_factory = new l3d_screen_factory_x11_shm;
      ras_2d_imp_factory = new l3d_rasterizer_2d_sw_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_x11;
      ras_3d_imp_factory = new l3d_rasterizer_3d_sw_imp_factory;
      plugin_loader_factory = new l3d_plugin_loader_linux_factory;
      break;
    case 3:
      screen_factory = new l3d_screen_factory_mesa;
      ras_2d_imp_factory = new l3d_rasterizer_2d_mesa_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_mesa;
      ras_3d_imp_factory = new l3d_rasterizer_3d_mesa_imp_factory;
      plugin_loader_factory = new l3d_plugin_loader_linux_factory;
      break;
    case 4:
      screen_factory = new l3d_screen_factory_x11;
      ras_2d_imp_factory = new l3d_rasterizer_2d_sw_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_x11;
      ras_3d_imp_factory = new l3d_rasterizer_3d_zbuf_sw_imp_factory;
      plugin_loader_factory = new l3d_plugin_loader_linux_factory;
      break;
    case 5:
      screen_factory = new l3d_screen_factory_x11_shm;
      ras_2d_imp_factory = new l3d_rasterizer_2d_sw_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_x11;
      ras_3d_imp_factory = new l3d_rasterizer_3d_zbuf_sw_imp_factory;
      plugin_loader_factory = new l3d_plugin_loader_linux_factory;
      break;
    case 6:
      screen_factory = new l3d_screen_factory_zbuf_mesa;
      ras_2d_imp_factory = new l3d_rasterizer_2d_mesa_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_mesa;
      ras_3d_imp_factory = new l3d_rasterizer_3d_zbuf_mesa_imp_factory;
      plugin_loader_factory = new l3d_plugin_loader_linux_factory;
      break;
  }

  // for backwards-compatibility
  factory_manager_v_0_2 = *this;
  factory_manager_v_0_2.should_delete_factories = 0;

  return i;
}





