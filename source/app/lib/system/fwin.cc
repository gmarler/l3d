#include "fwin.h"

#include "../tool_os/dis_mswn.h"
#include "../tool_2d/sc_mswin.h"
#include "../raster/ras_sw.h"
#include "../tool_os/dis_mesa.h"
#include "../tool_2d/sc_wogl.h"
#include "../raster/ras_mesa.h"
#include "../tool_os/memman.h"

#include <stdio.h>

l3d_factory_manager_v_0_1 factory_manager_v_0_1;

l3d_factory_manager_v_0_1::l3d_factory_manager_v_0_1(void) {
  screen_factory = NULL;
  ras_2d_imp_factory = NULL;
  dispatcher_factory = NULL;
}

l3d_factory_manager_v_0_1::~l3d_factory_manager_v_0_1(void) {
  if(screen_factory) delete screen_factory;
  if(ras_2d_imp_factory) delete ras_2d_imp_factory;
  if(dispatcher_factory) delete dispatcher_factory;
}

int l3d_factory_manager_v_0_1::choose_factories(void) {
  char input[80];
  int i;

  printf("which configuration?\n");
  printf("1. Software MS-Windows DIB\n");
  printf("2. OpenGL (with hardware acceleration, if present)\n");
  gets(input);
  sscanf(input,"%d", &i);
  switch(i) {
    case 1:
      screen_factory = new l3d_screen_factory_mswin;
      ras_2d_imp_factory = new l3d_rasterizer_2d_sw_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_mswin;
      break;
    case 2:
      screen_factory = new l3d_screen_factory_wogl;
      ras_2d_imp_factory = new l3d_rasterizer_2d_mesa_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_mesa;
      break;
  }
  return i;
}


