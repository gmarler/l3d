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

#include "fact0_4.h"

#include <stdio.h>
#include "../tool_os/memman.h"
#include "../sound/scli_rplay.h"

l3d_factory_manager_v_0_4 factory_manager_v_0_4;

l3d_factory_manager_v_0_4::l3d_factory_manager_v_0_4(void) :
    l3d_factory_manager_v_0_3()
{
}

l3d_factory_manager_v_0_4::~l3d_factory_manager_v_0_4(void)
{
  //FIXME: in descendant classes, they might reassign this and
  //want to delete it themselves. the "should delete factories"
  //mechanism is oversimplified; what is needed is to know if
  //a particular factory is allowed to be deleted in the destructor
  //or not (ie if it has been replaced and is manged by a subclass or not)
  delete sound_client_factory;
}

int l3d_factory_manager_v_0_4::choose_factories(void) {
  printf("NOTE: rplay sound factory will be created\n");
  int i = l3d_factory_manager_v_0_3::choose_factories();

  sound_client_factory = new l3d_sound_client_rplay_factory;

  // for backwards-compatibility
  factory_manager_v_0_2 = *this;
  factory_manager_v_0_2.should_delete_factories = 0;
  factory_manager_v_0_3 = *this;
  factory_manager_v_0_3.should_delete_factories = 0;

  return i;
}

