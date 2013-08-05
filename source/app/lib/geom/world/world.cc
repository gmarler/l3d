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


#include "world.h"
#include <stdlib.h>
#include <string.h>

#include "../object/object3d.h"
#include "../polygon/polygon.h"
#include "../../tool_2d/screen.h"
#include "../../tool_os/dispatch.h"
#include "../../raster/rasteriz.h"
#include "../../tool_2d/scrinfo.h"
#include "../../system/fact0_2.h"
#include "../../tool_os/memman.h"

l3d_world::l3d_world(int xsize, int ysize) {

  screen =
    factory_manager_v_0_2.screen_factory->create(xsize,ysize);
  rasterizer_2d_imp =
    factory_manager_v_0_2.ras_2d_imp_factory->
    create(xsize,ysize,screen->sinfo);
  rasterizer_3d_imp =
    factory_manager_v_0_2.ras_3d_imp_factory->
    create(xsize,ysize,screen->sinfo);
  rasterizer = new l3d_rasterizer_3d(rasterizer_2d_imp, rasterizer_3d_imp);

  // for compilation with a 64K DGROUP we must compute the sin/cos lookup
  // tables at runtime
  //
   #ifdef DGROUP64K
   #define PI 3.14159265
  int i;
  for(i=0; i<360; i++) {
    cos_lu[i] = Float2MT(cos(i*PI/180.));
    sin_lu[i] = Float2MT(sin(i*PI/180.));
  }
   #endif

  //#ifdef WINDOWS_OS
  // initizlize the physical screen */

  screen->open_screen();
  //#endif

  camera = new l3d_camera;

  should_display_status = 0;
}

l3d_world::~l3d_world() {
  int i;

  //- clean up world database
  for(i=0; i<objects.num_items; i++) {
    delete objects[i];
  }

  delete screen;
  delete rasterizer_2d_imp;
  delete rasterizer_3d_imp;
  delete rasterizer;
  delete camera;
}

int l3d_world::compare_facet_zdepth(const void *key, const void *elem) {

  l3d_real zdif =
    ( (*((l3d_polygon_3d **)elem))->zvisible - (*((l3d_polygon_3d **)key))->zvisible );

  if (zdif>0) return 1;
  else if(zdif<0) return -1;
  else return 0;
}

void l3d_world::draw_all(void) {
  int iObj, iFacet;

  //- don't make this array into a l3d_list because we use qsort later
  l3d_polygon_3d *plist[MAX_VISIBLE_FACETS]; int pnum=0;

  rasterizer->clear_buffer();

  // for all objects in the world, determine which facets are visible
  // and put these facets in the visible facets list
  //
  for(iObj=0; iObj<objects.num_items; iObj++) {

    //- reset object to its original position
    objects[iObj]->reset();

    //- position all vertices of object into world space
    if (objects[iObj]->num_xforms) {
      objects[iObj]->transform(objects[iObj]->modeling_xform);
    }

    // cull/clip polys in world space if desired

    //- transform polys to camera space
    objects[iObj]->camera_transform(camera->viewing_xform);

    //- clip polys to near-z
    l3d_polygon_3d_node *n;
    n = objects[iObj]->nonculled_polygon_nodes;
    while(n) {
      if(n->polygon->clip_near_z(camera->near_z))

      {
        //- polygon is not culled
      } else {

        //- take this polygon out of the list of nonculled_polygon_nodes
        if(n->prev) {
          n->prev->next = n->next;
        } else {
          objects[iObj]->nonculled_polygon_nodes = n->next;
        }

        if(n->next) {
          n->next->prev = n->prev;
        }
      }
      n = n->next;
    }

    //- perspective projection of vertices only of surviving polys
    objects[iObj]->apply_perspective_projection(*camera,
        screen->xsize, screen->ysize);

    n = objects[iObj]->nonculled_polygon_nodes;

    while(n) {
      if ( n->polygon->clip_to_polygon_2d(screen->view_win)) {

        //- polygon survived all culling tests and will be drawn.

        plist[pnum++] = n->polygon;
        assert(pnum<MAX_VISIBLE_FACETS || !printf("too many visible polys %d\n",pnum));

        //- Use the average of all z-values for depth sorting
        l3d_real zsum=int_to_l3d_real(0);
        register int ivtx;

        // don't make the mistake of using the clip_ivertices to
        // compute the depth, because some of the clip_ivertices resulted
        // from a 2D clipping operation and have no corresponding 3D point:
        // instead use ivertices, and transformed_intermediate number 1
        // to retrieve the cam space coordinates

        for(ivtx=0; ivtx<n->polygon->ivertices->num_items; ivtx++) {
          zsum += (**(n->polygon->vlist))[ (*n->polygon->ivertices)[ivtx].ivertex ].transformed_intermediates[1].Z_ ;
        }
        n->polygon->zvisible = l3d_divri(zsum, ivtx);
      }
      n = n->next;
    }
  }


  //- Sort the visible facets in decreasing order of z-depth
  qsort(plist, pnum, sizeof(l3d_polygon_3d *), compare_facet_zdepth);

  //- Display the facets in the sorted z-depth order (painter's algorithm)

  for(int i=0; i<pnum; i++) {
    plist[i]->draw(rasterizer);
  }

  if(should_display_status) {
    char msg[256];
    sprintf(msg,"%d polys drawn\n", pnum);
    rasterizer->draw_text(0,16,msg);
  }
}

// virtual
void l3d_world::update_all(void) {
  register int i;

  for(i=0; i<objects.num_items; i++) {
    objects[i]->update();
  }

  //- camera is separate object not in objects[] so it is separately updated
  camera->update();
}
