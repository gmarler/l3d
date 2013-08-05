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
#include "../lib/pipeline/pi_wor.h"
#include "../lib/system/fact0_2.h"

#include "shapes.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

class my_world:public l3d_world {
  public:
    my_world(void);
    void update_all(void);

    // no destructor necessary for objects created in constructor;
    // base class destructor takes care of these.
};

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

void my_world::update_all(void) {
  l3d_world::update_all();
}

my_world::my_world(void)
    : l3d_world(640,400)
{


  l3d_screen_info *si = screen->sinfo;

  camera->VRP.set(0,0,-50,0);
  camera->near_z = float_to_l3d_real(5.5);
  camera->far_z = int_to_l3d_real(500);

  int i,j,k,onum=0, prev_onum;

  i=10; j=0; k=20;
  k=0;

  //- create a pyramid
  pyramid *orig_pyr;
  objects[onum=objects.next_index()] = orig_pyr = new pyramid();
  l3d_polygon_3d_flatshaded *p;
  for(int pnum=0; pnum<objects[onum]->polygons.num_items; pnum++) {
    p = dynamic_cast<l3d_polygon_3d_flatshaded *>
        (objects[onum]->polygons[pnum]);
    if(p) {
      p->final_color = si->ext_to_native
                       (rand()%si->ext_max_red,
                        rand()%si->ext_max_green,
                        rand()%si->ext_max_blue);
    }
  }
  orig_pyr->dx = 0;

  //- copy the pyramid
  pyramid *copy_pyr;
  prev_onum = onum;
  objects[onum=objects.next_index()] = copy_pyr =
                                         new pyramid();
  *objects[onum] = *objects[prev_onum];
  copy_pyr->dx = float_to_l3d_real(0.25);
  copy_pyr->maxx=int_to_l3d_real(5);
  copy_pyr->minx=int_to_l3d_real(0);

  //- define a clip plane
  l3d_plane clip_plane;
  clip_plane.a = int_to_l3d_real(-1);
  clip_plane.b = int_to_l3d_real(-1);
  clip_plane.c = int_to_l3d_real(-1);
  clip_plane.d = int_to_l3d_real(3);

  //- split the first copy on one side of the plane
  orig_pyr->reset();
  orig_pyr->clip_to_plane(clip_plane);
  orig_pyr->make_current_geometry_permanent();

  //- split the second copy on the other side of the plane
  clip_plane.a = -clip_plane.a;
  clip_plane.b = -clip_plane.b;
  clip_plane.c = -clip_plane.c;
  clip_plane.d = -clip_plane.d;
  copy_pyr->reset();
  copy_pyr->clip_to_plane(clip_plane);
  copy_pyr->make_current_geometry_permanent();

  screen->refresh_palette();
}

