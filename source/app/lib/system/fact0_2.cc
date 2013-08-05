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

#include "fact0_2.h"

#include "../tool_os/dis_x11.h"
#include "../tool_os/dis_mesa.h"
#include "../tool_2d/sc_x11.h"
#include "../tool_2d/sc_x11sh.h"
#include "../tool_2d/sc_mesa.h"
#include "../raster/ras3_sw.h"
#include "../raster/ras3_mes.h"
#include "../dynamics/plugins/pl_linux.h"

#include <stdio.h>
#include "../tool_os/memman.h"

l3d_factory_manager_v_0_2 factory_manager_v_0_2;

l3d_factory_manager_v_0_2::l3d_factory_manager_v_0_2(void) :
    l3d_factory_manager_v_0_1()
{
  ras_3d_imp_factory = NULL;
  plugin_loader_factory = NULL;
}

l3d_factory_manager_v_0_2::~l3d_factory_manager_v_0_2(void)
{
  if(should_delete_factories) {
    if (ras_3d_imp_factory) {
      delete ras_3d_imp_factory;
      ras_3d_imp_factory = NULL;
    }
    if (plugin_loader_factory) {
      delete plugin_loader_factory;
      plugin_loader_factory = NULL;
    }
  }
}

int l3d_factory_manager_v_0_2::choose_factories(void) {
  int i = l3d_factory_manager_v_0_1::choose_factories();

  switch(i) {
    case 1:
      ras_3d_imp_factory = new l3d_rasterizer_3d_sw_imp_factory;
      plugin_loader_factory = new l3d_plugin_loader_linux_factory;
      break;
    case 2:
      ras_3d_imp_factory = new l3d_rasterizer_3d_sw_imp_factory;
      plugin_loader_factory = new l3d_plugin_loader_linux_factory;
      break;
    case 3:
      ras_3d_imp_factory = new l3d_rasterizer_3d_mesa_imp_factory;
      plugin_loader_factory = new l3d_plugin_loader_linux_factory;
      break;
  }

  return i;
}





