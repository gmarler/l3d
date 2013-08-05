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

#include "w_back.h"
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

l3d_world_backface::~l3d_world_backface() {
}

void l3d_world_backface::draw_all(void) {
  int iObj, iFacet;

  // don't make this a l3d_list because we use qsort later

  l3d_polygon_3d *plist[MAX_VISIBLE_FACETS]; int pnum=0;

  rasterizer->clear_buffer();

  // for all objects in the world, determine which facets are visible
  // and put these facets in the visible facets list
  //
  for(iObj=0; iObj<objects.num_items; iObj++) {

    objects[iObj]->reset();


    // position all vertices of object into world space

    if (objects[iObj]->num_xforms) {
      objects[iObj]->transform(objects[iObj]->modeling_xform);
    }

    // cull/clip polys in world space if desired

    // transform surviving polys to camera space

    objects[iObj]->camera_transform(camera->viewing_xform);

    // backface-culling and clip polys to near-z

    l3d_polygon_3d_node *n;

    //sbegin backface
    //- vector from facet center to camera (at 0,0,0), for backface culling
    l3d_vector facet_to_cam_vec;
    //- facet's sfc normal relative to origin (aFacet.sfc_normal is relative
    //- to facet center)
    l3d_vector N;
    n = objects[iObj]->nonculled_polygon_nodes;
    while(n) {
      N = n->polygon->sfcnormal.transformed - n->polygon->center.transformed;

      facet_to_cam_vec.set (0 - n->polygon->center.transformed.X_,
                            0 - n->polygon->center.transformed.Y_,
                            0 - n->polygon->center.transformed.Z_,
                            int_to_l3d_real(0));

      //- dot product explicitly formulated so no fixed point overflow
      //- (facet_to_cam_vec is very large)
      //- (use of the inline function "dot" uses default fixed precision)
#include "../../math/fix_lowp.h"
#define BACKFACE_EPS float_to_l3d_real(0.1)
      if(( l3d_mulrr(FX_CVT(N.X_), FX_CVT(facet_to_cam_vec.X_)) +
           l3d_mulrr(FX_CVT(N.Y_), FX_CVT(facet_to_cam_vec.Y_)) +
           l3d_mulrr(FX_CVT(N.Z_), FX_CVT(facet_to_cam_vec.Z_)) > BACKFACE_EPS)
          //- equivalent to: if(N.dot(facet_to_cam_vec) > BACKFACE_EPS )
#include "../../math/fix_prec.h"

          &&

          n->polygon->clip_near_z(camera->near_z))

      {
        //- polygon is not culled
      } else {

        //- take this polygon out of the list of nonculled_polygon_nodes
        //send backface
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

    // perspective projection of vertices only of surviving polys
    objects[iObj]->apply_perspective_projection(*camera,
        screen->xsize, screen->ysize);

    n = objects[iObj]->nonculled_polygon_nodes;

    while(n) {
      if ( n->polygon->clip_to_polygon_2d(screen->view_win)) {

        // polygon survived all culling tests and will be drawn.

        plist[pnum++] = n->polygon;
        assert(pnum<MAX_VISIBLE_FACETS || !printf("too many visible polys %d\n",pnum));

        /* Use the average of all z-values for depth sorting */
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


  /* Sort the visible facets in decreasing order of z-depth */
  qsort(plist, pnum, sizeof(l3d_polygon_3d *), compare_facet_zdepth);

  /* Display the facets in the sorted z-depth order (painter's algorithm) */

  for(int i=0; i<pnum; i++) {
    plist[i]->draw(rasterizer);
  }

  char msg[256];
  sprintf(msg,"%d POLYS DRAWN", pnum);
  rasterizer->draw_text(0,16,msg);
}
