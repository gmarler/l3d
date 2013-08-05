#ifndef __FWIN0_2_H
#define __FWIN0_2_H

#include "fwin.h"
#include "../raster/rast3.h"
#include "../dynamics/plugins/plugload.h"

class l3d_factory_manager_v_0_2 : public l3d_factory_manager_v_0_1 {
public:
  l3d_factory_manager_v_0_2(void);
  virtual ~l3d_factory_manager_v_0_2(void);
  l3d_rasterizer_3d_imp_factory *ras_3d_imp_factory;
  l3d_plugin_loader_factory *plugin_loader_factory;
  int choose_factories(void);// doesn´t need to be virtual
};

extern l3d_factory_manager_v_0_2 factory_manager_v_0_2;

#endif
