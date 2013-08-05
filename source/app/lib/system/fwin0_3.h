#ifndef __FWIN0_3_H
#define __FWIN0_3_H

#include "fwin0_2.h"

class l3d_factory_manager_v_0_3 : public l3d_factory_manager_v_0_2 {
 public:
  l3d_factory_manager_v_0_3(void);
  virtual ~l3d_factory_manager_v_0_3(void);
  int choose_factories(void);// doesn´t need to be virtual
};

extern l3d_factory_manager_v_0_3 factory_manager_v_0_3;

#endif
