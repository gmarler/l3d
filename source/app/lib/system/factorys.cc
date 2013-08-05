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

#include "factorys.h"

#include "../tool_os/dis_x11.h"
#include "../tool_os/dis_mesa.h"
#include "../tool_2d/sc_x11.h"
#include "../tool_2d/sc_x11sh.h"
#include "../tool_2d/sc_mesa.h"
#include "../raster/ras_sw.h"
#include "../raster/ras_mesa.h"
#include <stdio.h>
#include "../tool_os/memman.h"

l3d_factory_manager_v_0_1 factory_manager_v_0_1;

l3d_factory_manager_v_0_1::l3d_factory_manager_v_0_1(void) {
  screen_factory = NULL;
  ras_2d_imp_factory = NULL;
  dispatcher_factory = NULL;
  should_delete_factories=1;
}

l3d_factory_manager_v_0_1::~l3d_factory_manager_v_0_1(void) {
  if(should_delete_factories) {
    if(screen_factory) {
      delete screen_factory;
      screen_factory = NULL;
    }
    if(ras_2d_imp_factory) {
      delete ras_2d_imp_factory;
      ras_2d_imp_factory = NULL;
    }
    if(dispatcher_factory) {
      delete dispatcher_factory;
      dispatcher_factory = NULL;
    }
  }
}

int l3d_factory_manager_v_0_1::choose_factories(void) {
  char input[80];
  int i;

  printf("which configuration?\n");
  printf("1. Software X11\n");
  printf("2. Software X11 with SHM extension\n");
  printf("3. Mesa (with 3DFX h/w accel if installed with Mesa)\n");
  gets(input);
  sscanf(input,"%d", &i);
  switch(i) {
    case 1:
      screen_factory = new l3d_screen_factory_x11;
      ras_2d_imp_factory = new l3d_rasterizer_2d_sw_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_x11;
      break;
    case 2:
      screen_factory = new l3d_screen_factory_x11_shm;
      ras_2d_imp_factory = new l3d_rasterizer_2d_sw_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_x11;
      break;
    case 3:
      screen_factory = new l3d_screen_factory_mesa;
      ras_2d_imp_factory = new l3d_rasterizer_2d_mesa_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_mesa;
      break;
  }
  return i;
}



