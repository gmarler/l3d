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

#ifndef __FACT0_2_H
#define __FACT0_2_H
#include "../tool_os/memman.h"

#include "factorys.h"
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
