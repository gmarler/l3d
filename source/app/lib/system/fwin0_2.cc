#include "fwin0_2.h"
 
#include "../tool_os/dis_mswn.h"
#include "../tool_2d/sc_mswin.h"
#include "../raster/ras3_sw.h"
#include "../raster/ras3_mes.h"
#include "../dynamics/plugins/pl_linux.h"
#include <stdio.h>

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
    ras_3d_imp_factory = new l3d_rasterizer_3d_mesa_imp_factory;
    plugin_loader_factory = new l3d_plugin_loader_linux_factory;
    break;
  }

  return i;
}
