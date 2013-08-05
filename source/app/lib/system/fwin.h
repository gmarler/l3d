#ifndef __FWIN_H
#define __FWIN_H

#include "../tool_2d/screen.h"
#include "../raster/rasteriz.h"
#include "../tool_os/dispatch.h"

class l3d_factory_manager_v_0_1 {
 public:
  l3d_factory_manager_v_0_1(void);
  virtual ~l3d_factory_manager_v_0_1(void);
  int should_delete_factories;
  l3d_screen_factory *screen_factory;
  l3d_rasterizer_2d_imp_factory *ras_2d_imp_factory;
  l3d_dispatcher_factory *dispatcher_factory;

  int choose_factories(void);
};

extern l3d_factory_manager_v_0_1 factory_manager_v_0_1;

#endif
