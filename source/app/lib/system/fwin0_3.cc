#include "fwin0_3.h"

#include <stdio.h>
#include "../tool_os/dis_mswn.h"
#include "../tool_2d/sc_mswin.h"
#include "../raster/ras_sw.h"
#include "../raster/ras3z_sw.h"
#include "../tool_os/dis_mesa.h"
#include "../tool_2d/sc_wogl.h"
#include "../tool_2d/sc_wzogl.h"
#include "../raster/ras3z_mes.h"
#include "../dynamics/plugins/pl_linux.h"
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
  int i;

  char input[80];

  printf("1. Software DIB rendering\n");
  printf("2. OpenGL\n");
  printf("3. OpenGL with Z buffer\n");
  gets(input);
  sscanf(input,"%d", &i);
  switch(i) {
    case 1:
      screen_factory = new l3d_screen_factory_mswin;
      ras_2d_imp_factory = new l3d_rasterizer_2d_sw_imp_factory;
      ras_3d_imp_factory = new l3d_rasterizer_3d_zbuf_sw_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_mswin;
      plugin_loader_factory = new l3d_plugin_loader_linux_factory;
    case 2:
      screen_factory = new l3d_screen_factory_wogl;
      ras_2d_imp_factory = new l3d_rasterizer_2d_mesa_imp_factory;
      dispatcher_factory = new l3d_dispatcher_factory_mesa;
      ras_3d_imp_factory = new l3d_rasterizer_3d_mesa_imp_factory;
      plugin_loader_factory = new l3d_plugin_loader_linux_factory;
      break;
    case 3:
      screen_factory = new l3d_screen_factory_zbuf_wogl;
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





