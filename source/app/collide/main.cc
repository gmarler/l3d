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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "../lib/geom/object/object3d.h"
#include "../lib/geom/polygon/p_flat.h"
#include "../lib/geom/polygon/p_portal.h"
#include "../lib/tool_2d/screen.h"
#include "../lib/tool_os/dispatch.h"
#include "../lib/raster/rasteriz.h"
#include "../lib/tool_2d/scrinfo.h"
#include "../lib/geom/world/world.h"
#include "../lib/system/fact0_4.h"
#include "../lib/geom/texture/texload.h"
#include "../lib/geom/texture/tl_ppm.h"
#include "../lib/pipeline/pi_lwor.h"
#include "../lib/geom/world/w_pcoll.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


main() {
  factory_manager_v_0_4.choose_factories();

  l3d_dispatcher *d;
  l3d_pipeline_world_lightmapped *p;
  l3d_world_portal_textured_lightmapped_obj_colldet w(320,240);
  void *startinfo;

  w.load_from_file("world.dat");
  //    w.current_sector = w.sector_by_name("S24");
  //    w.camera->VRP.set(0,0,0,0);
  //    w.camera->VFW.set(0,0,1,0);
  //    w.camera->VUP.set(0,1,0,0);
  w.camera->near_z = float_to_l3d_real(0.5);
  w.camera->far_z = int_to_l3d_real(5000);

  //- start background music
  char cmd[] = "play count=9999 volume=128 sound=bgmusic.au\n";
  w.sound_client->send_command(cmd);


  printf("current sector %s\n", w.current_sector->name);
  d = factory_manager_v_0_4.dispatcher_factory->create();

  // plug our custom behavior pipeline into the dispatcher
  p  = new l3d_pipeline_world_lightmapped(&w);
  d->pipeline = p;// polymorphic assignment
  d->event_source = w.screen;

  // start dispatcher with the screen
  d->start();

  // clean up
  delete p;
  delete d;

  //- stop background music
  char cmd2[] = "stop bgmusic.au\n";
  w.sound_client->send_command(cmd2);
}
