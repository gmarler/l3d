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
#include "../lib/geom/polygon/p3_flat.h"
#include "../lib/tool_2d/screen.h"
#include "../lib/tool_os/dispatch.h"
#include "../lib/raster/rast3.h"
#include "../lib/tool_2d/scrinfo.h"
#include "../lib/geom/world/world.h"
#include "../lib/geom/world/w_back.h"
#include "../lib/system/fact0_2.h"
#include "../lib/pipeline/pi_wor.h"
#include "../lib/dynamics/plugins/plugenv.h"

#include "shapes.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

class my_world:public l3d_world_backface {
  public:
    my_world(void);
    // no destructor necessary for objects created in constructor;
    // base class destructor takes care of these.
};

my_world::my_world(void)
    : l3d_world_backface(640,400)
{
  l3d_screen_info *si = screen->sinfo;

  camera->VRP.set(0,0,-50,0);
  camera->near_z = float_to_l3d_real(5.5);
  camera->far_z = int_to_l3d_real(500);

  int i,j,k,onum=0;

  i=10; j=0; k=20;
  k=0;

  //- create some pyramid objects
  for(i=1; i<200; i+=20) {
    objects[onum=objects.next_index()] = new pyramid();
    l3d_screen_info_indexed *si_idx;
    l3d_screen_info_rgb *si_rgb;

    l3d_polygon_3d_flatshaded *p;
    for(int pnum=0; pnum<objects[onum]->polygons.num_items; pnum++) {
      p = dynamic_cast<l3d_polygon_3d_flatshaded *>(objects[onum]->polygons[pnum]);
      if(p) {
        p->final_color = si->ext_to_native
                         (rand()%si->ext_max_red,
                          rand()%si->ext_max_green,
                          rand()%si->ext_max_blue);
      }
    }



    if (objects[onum]==NULL) exit;
    objects[onum]->modeling_xforms[1].set
    ( int_to_l3d_real(1), int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(i),
      int_to_l3d_real(0), int_to_l3d_real(1), int_to_l3d_real(0), int_to_l3d_real(1),
      int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(1), int_to_l3d_real(1),
      int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(1) );
    objects[onum]->modeling_xform =
      objects[onum]->modeling_xforms[1] |
      objects[onum]->modeling_xforms[0] ;
  }

  //sbegin plugin
  //- create a plugin object

  objects[onum=objects.next_index()] = new l3d_object(10);
  //- max 10 fixed vertices, can be overridden by plug-in if desired
  //- by redefining the vertex list

  objects[onum]->plugin_loader =
    factory_manager_v_0_2.plugin_loader_factory->create();
  objects[onum]->plugin_loader->load("../lib/dynamics/plugins/pyramid/pyramid.so");
  objects[onum]->plugin_constructor =
    (void (*)(l3d_object *, void *))
    objects[onum]->plugin_loader->find_symbol("constructor");
  objects[onum]->plugin_update =
    (void (*)(l3d_object *))
    objects[onum]->plugin_loader->find_symbol("update");
  objects[onum]->plugin_destructor =
    (void (*)(l3d_object *))
    objects[onum]->plugin_loader->find_symbol("destructor");
  objects[onum]->plugin_copy_data =
    (void (*)(const l3d_object *, l3d_object *))
    objects[onum]->plugin_loader->find_symbol("copy_data");

  l3d_plugin_environment *e = new l3d_plugin_environment
                              (NULL, screen->sinfo, NULL, (void *)"");

  if(objects[onum]->plugin_constructor) {
    (*objects[onum]->plugin_constructor) (objects[onum],e);
  }
  //send plugin

  screen->refresh_palette();
}

main() {
  l3d_dispatcher *d;
  l3d_pipeline_world *p;
  my_world *w;

  factory_manager_v_0_2.choose_factories();
  d = factory_manager_v_0_2.dispatcher_factory->create();

  // plug our custom behavior pipeline into the dispatcher
  w = new my_world();
  p  = new l3d_pipeline_world(w);
  d->pipeline = p;// polymorphic assignment
  d->event_source = w->screen;

  // start dispatcher with the screen from the pipeline
  d->start();

  // clean up
  delete d;
  delete p;
  delete w;
}
