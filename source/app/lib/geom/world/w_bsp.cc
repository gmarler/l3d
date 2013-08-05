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

#include "w_bsp.h"
#include <stdlib.h>
#include <string.h>

#include "../object/sector.h"
#include "../polygon/polygon.h"
#include "../../tool_2d/screen.h"
#include "../../tool_os/dispatch.h"
#include "../../raster/rasteriz.h"
#include "../../tool_2d/scrinfo.h"
#include "../../system/fact0_2.h"
#include "w_bsp.h"
#include "../bsp/halfspace.h"
#include "../bsp/bsptree.h"
#include "../frustum/vfrust.h"
#include "../../tool_os/memman.h"

l3d_world_bsptree::~l3d_world_bsptree() {
  if(the_tree) delete the_tree;
}

void l3d_world_bsptree::render_bsp_tree
(l3d_halfspace *tree, l3d_point campos, l3d_viewing_frustum &frustum)
{
  visited_nodes++;

  if(tree) {

    if( tree->plane.side_of_point(campos) > 0 ) {
      if(tree->children[0]
          && frustum.intersects_sphere(tree->children[0]->bounding_sphere)
        )
      {
        render_bsp_tree(tree->children[0],campos,frustum);
      }
      //      else if (tree->children[0]) {
      //        printf("sphere %f %f %f rad %f culled\n",
      //               tree->children[0]->bounding_sphere.center.transformed.X_,
      //               tree->children[0]->bounding_sphere.center.transformed.Y_,
      //               tree->children[0]->bounding_sphere.center.transformed.Z_,
      //               tree->children[0]->bounding_sphere.radius);
      //      }

      if(tree->object) {
        tree->object->reset();
        assert(tree->object->bounding_sphere.transform_stage == tree->object->transform_stage);
        //- no world coord xforms allowed, bsp is static


        if(tree->object->clip_to_plane(frustum.top)
            && tree->object->clip_to_plane(frustum.bottom)
            && tree->object->clip_to_plane(frustum.left)
            && tree->object->clip_to_plane(frustum.right)
            && tree->object->clip_to_plane(frustum.near)
            && tree->object->clip_to_plane(frustum.far))
        {
          assert(tree->object->bounding_sphere.transform_stage == tree->object->transform_stage||!printf("sph %d tra %d\n",tree->object->bounding_sphere.transform_stage,tree->object->transform_stage));
          tree->object->camera_transform(camera->viewing_xform);
          assert(tree->object->bounding_sphere.transform_stage == tree->object->transform_stage||!printf("sph %d tra %d\n",tree->object->bounding_sphere.transform_stage,tree->object->transform_stage));
          tree->object->apply_perspective_projection
          (*camera, screen->xsize, screen->ysize);


          l3d_polygon_3d_node *n;
          n = tree->object->nonculled_polygon_nodes;
          while(n) {
            l3d_polygon_3d_clippable *poly =
              dynamic_cast<l3d_polygon_3d_clippable *>
              (n->polygon);
            assert(poly);
            if( poly->sfcnormal.transformed.Z_ -
                poly->center.transformed.Z_ < float_to_l3d_real(0.05) )
            {
              poly->draw(rasterizer);
            }
            n = n->next;
          }
        }
      }

      if(tree->children[1]
          && frustum.intersects_sphere(tree->children[1]->bounding_sphere)
        )
      {
        render_bsp_tree(tree->children[1],campos,frustum);
      }
      //      else if (tree->children[1]) {
      //        printf("sphere %f %f %f rad %f culled\n",
      //               tree->children[1]->bounding_sphere.center.transformed.X_,
      //               tree->children[1]->bounding_sphere.center.transformed.Y_,
      //               tree->children[1]->bounding_sphere.center.transformed.Z_,
      //               tree->children[1]->bounding_sphere.radius);
      //      }
    } else {
      if(tree->children[1]
          && frustum.intersects_sphere(tree->children[1]->bounding_sphere)
        )
      {
        render_bsp_tree(tree->children[1],campos,frustum);
      }
      //      else if (tree->children[1]) {
      //        printf("sphere %f %f %f rad %f culled\n",
      //               tree->children[1]->bounding_sphere.center.transformed.X_,
      //               tree->children[1]->bounding_sphere.center.transformed.Y_,
      //               tree->children[1]->bounding_sphere.center.transformed.Z_,
      //               tree->children[1]->bounding_sphere.radius);
      //      }

      if(tree->object) {
        tree->object->reset();
        //- no world coord xforms allowed, bsp is static

        if(tree->object->clip_to_plane(frustum.top)
            && tree->object->clip_to_plane(frustum.bottom)
            && tree->object->clip_to_plane(frustum.left)
            && tree->object->clip_to_plane(frustum.right)
            && tree->object->clip_to_plane(frustum.near)
            && tree->object->clip_to_plane(frustum.far))
        {
          tree->object->camera_transform(camera->viewing_xform);
          tree->object->apply_perspective_projection
          (*camera, screen->xsize, screen->ysize);

          l3d_polygon_3d_node *n;
          n = tree->object->nonculled_polygon_nodes;
          while(n) {
            l3d_polygon_3d_clippable *poly =
              dynamic_cast<l3d_polygon_3d_clippable *>
              (n->polygon);
            assert(poly);
            {
              if( poly->sfcnormal.transformed.Z_ -
                  poly->center.transformed.Z_ < float_to_l3d_real(0.05) )
              {
                poly->draw(rasterizer);
              }
            }
            n = n->next;
          }
        }
      }
      if(tree->children[0]
          && frustum.intersects_sphere(tree->children[0]->bounding_sphere)
        )
      {
        render_bsp_tree(tree->children[0],campos,frustum);
      }
      //      else if (tree->children[0]) {
      //        printf("sphere %f %f %f rad %f culled\n",
      //               tree->children[0]->bounding_sphere.center.transformed.X_,
      //               tree->children[0]->bounding_sphere.center.transformed.Y_,
      //               tree->children[0]->bounding_sphere.center.transformed.Z_,
      //               tree->children[0]->bounding_sphere.radius);
      //      }
    }
  }

}

void l3d_world_bsptree::draw_all(void) {
  int iObj, iFacet;

  // don't make this a l3d_list because we use qsort later

  rasterizer->clear_buffer();

  // make viewing frustum

  l3d_viewing_frustum frustum;

  l3d_point top_left, top_right, bot_right, bot_left;
  const int XOFF=2,YOFF=2;

  top_left.set(l3d_mulrr(l3d_divrr(int_to_l3d_real(XOFF - screen->xsize/2),
                                   l3d_mulri(camera->fovx,screen->xsize)),
                         camera->near_z),
               l3d_mulrr(l3d_divrr(int_to_l3d_real(screen->ysize/2 - YOFF),
                                   l3d_mulri(camera->fovy,screen->ysize)),
                         camera->near_z),
               int_to_l3d_real(camera->near_z),
               int_to_l3d_real(1));

  top_right.set(l3d_mulrr(l3d_divrr(int_to_l3d_real(screen->xsize-XOFF - screen->xsize/2),
                                    l3d_mulri(camera->fovx,screen->xsize)),
                          camera->near_z),
                l3d_mulrr(l3d_divrr(int_to_l3d_real(screen->ysize/2 - YOFF),
                                    l3d_mulri(camera->fovy,screen->ysize)),
                          camera->near_z),
                int_to_l3d_real(camera->near_z),
                int_to_l3d_real(1));


  bot_right.set(l3d_mulrr(l3d_divrr(int_to_l3d_real(screen->xsize-XOFF - screen->xsize/2),
                                    l3d_mulri(camera->fovx,screen->xsize)),
                          camera->near_z),
                l3d_mulrr(l3d_divrr(int_to_l3d_real(screen->ysize/2 - screen->ysize+YOFF),
                                    l3d_mulri(camera->fovy,screen->ysize)),
                          camera->near_z),
                camera->near_z,
                int_to_l3d_real(1));

  bot_left.set(l3d_mulrr(l3d_divrr(int_to_l3d_real(XOFF - screen->xsize/2),
                                   l3d_mulri(camera->fovx,screen->xsize)),
                         camera->near_z),
               l3d_mulrr(l3d_divrr(int_to_l3d_real(screen->ysize/2 - screen->ysize+YOFF),
                                   l3d_mulri(camera->fovy,screen->ysize)),
                         camera->near_z),
               camera->near_z,
               int_to_l3d_real(1));

  frustum.create_from_points
  (top_left, top_right, bot_right, bot_left,
   camera->near_z, camera->far_z,
   camera->inverse_viewing_xform);


  //- traverse the BSP tree

  l3d_point camera_world_pos = camera->VRP;

  visited_nodes = 0;
  render_bsp_tree(the_tree->root, camera_world_pos, frustum);

  char msg[256];
  sprintf(msg,"%d nodes visited", visited_nodes);
  rasterizer->draw_text(0,32,msg);
}
