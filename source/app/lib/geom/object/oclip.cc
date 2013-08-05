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

#include <stdio.h>
#include <stdlib.h>
#include "oclip.h"
#include "../../tool_os/memman.h"

l3d_object_clippable::~l3d_object_clippable() {}

int l3d_object_clippable::clip_to_plane(l3d_plane& clip_plane)
{
  //  printf("oclip to plane\n");
  l3d_polygon_3d_node *n = nonculled_polygon_nodes;
  while(n) {
    l3d_polygon_3d_clippable *pc;
    pc = dynamic_cast<l3d_polygon_3d_clippable*>(n->polygon);
    if(pc) {
      //        printf("clipping a poly\n");
      if(pc->clip_to_plane(clip_plane)) {
        //          printf("survived\n");
        // polygon survived: do nothing
      } else {
        // take this polygon out of the list of nonculled_polygon_nodes
        //          printf("culled\n");
        if(n->prev) {
          n->prev->next = n->next;
        } else {
          nonculled_polygon_nodes = n->next;
        }

        if(n->next) {
          n->next->prev = n->prev;
        }
      }
    }
    n = n->next;
  }
  if(nonculled_polygon_nodes) return 1; else return 0;
}

void l3d_object_clippable::make_current_geometry_permanent(void)
{
  //  printf("makign clipping perm\n");
  // make the clipped geometry permanent (move transformed
  // into original)
  vertices->num_fixed_items =
    vertices->num_fixed_items +
    vertices->num_varying_items;
  vertices->max_varying_items =
    vertices->max_varying_items -
    vertices->num_varying_items;
  vertices->num_varying_items = 0;

  int ivtx;
  for(ivtx=0;
      ivtx<vertices->num_fixed_items;
      ivtx++)
  {
    (*vertices)[ivtx].original = (*vertices)[ivtx].transformed;
  }


  //  printf("making clip_iv to iv\n");
  l3d_polygon_3d_node *n = nonculled_polygon_nodes;
  while(n) {
    *(n->polygon->ivertices) = *(n->polygon->clip_ivertices);
    n = n->next;
  }


  //  	    init_nonculled_list() in l3d_object, which we call at the beg.
  //  	    of each frame, will blindly relink together all polygons again,
  //  	    ** INCLUDING** those which we completely clipped away above!
  //  	    so we ahve to also bake-in the currenet state of the nonculled_list
  //  	    so that the next call to init_nonculled_list creates the current
  //  	    list of polys and no more.

  //  printf("creating new culled poly list\n");
  n = nonculled_polygon_nodes;
  l3d_list<l3d_polygon_3d*> new_polygon_list; // contains only surviving polys
  while(n) {
    new_polygon_list[new_polygon_list.next_index()] = n->polygon;
    n=n->next;
  }

  for(int i=0; i<polygons.num_items; i++) {
    int found=0;

    for(int j=0; j<new_polygon_list.num_items; j++) {
      if(polygons[i] == new_polygon_list[j]) { found = 1; }
    }

    if(!found) {
      //        printf("deleting poly %p, %p %p %p %p\n",
      //               polygons[i],
      //               polygons[i]->ivertices,
      //               polygons[i]->clip_ivertices,
      //               polygons[i]->temp_clip_ivertices,
      //               polygons[i]->iv_items_factory
      //               );
      delete polygons[i];
    }
    delete ( & polygons[i] );
  }
  for(int i=polygons.num_items; i<polygons.max_items; i++) {
    delete ( & polygons[i] );
  }
  polygons.num_items = 0;
  polygons.max_items = 0;

  polygons = new_polygon_list;

  //    // clean up any unused polygons
  //    for(int i=polygons.num_items; i<polygons.max_items; i++) {
  //      if(polygons[i]) {
  //        int poly_still_used = 0;
  //        for(int iused=0; iused<polygons.num_items; iused++) {
  //          if(polygons[iused] == polygons[i]) { poly_still_used = 1; }
  //        }
  //        if(!poly_still_used) {
  //          l3d_polygon_3d *p = polygons[i];
  //          delete polygons[i];
  //          for(int iused=polygons.num_items; iused<polygons.max_items; iused++) {
  //            if(polygons[iused] == p) { polygons[iused] = NULL; }
  //          }
  //        }
  //      }
  //      delete ( & polygons[i] );
  //    }
  //    polygons.max_items = polygons.num_items;

  //- clean up any unused vertices, and rewrite polygon definitions
  //- to use new vertex indices

  // the order of these loops is important!!!
  // we rewrite the vertex lists IN-PLACE. but the new vertex indices
  // will always be LESS THAN OR EQUAL TO the original. this means that
  // a replaced vertex index will never be touched again by a following
  // iteration of the loop.

  l3d_two_part_list<l3d_coordinate> *new_vlist;
  new_vlist = new l3d_two_part_list<l3d_coordinate> (1);
  *new_vlist = *vertices;
  new_vlist->num_fixed_items = 0;
  for(ivtx=0; ivtx<vertices->num_fixed_items; ivtx++) {
    int vtx_found = 0;
    for(int ipoly=0; ipoly<polygons.num_items && !vtx_found; ipoly++) {
      for(int ivtx_poly=0;
          ivtx_poly<polygons[ipoly]->ivertices->num_items && !vtx_found;
          ivtx_poly++)
      {
        if((*polygons[ipoly]->ivertices)[ivtx_poly].ivertex == ivtx) {
          //- vertex is used: keep it.
          vtx_found = 1;

          int new_ivtx = new_vlist->num_fixed_items;
          new_vlist->num_fixed_items++;
          assert(new_vlist->num_fixed_items <= vertices->num_fixed_items);

          (*new_vlist)[new_ivtx] = (*vertices)[ivtx];
          //- replace all occurrences in all polys of "ivtx" with "new_ivtx"
          for(int ipoly_repl=0; ipoly_repl<polygons.num_items; ipoly_repl++) {
            for(int ivtx_poly_repl=0;
                ivtx_poly_repl<polygons[ipoly_repl]->ivertices->num_items;
                ivtx_poly_repl++)
            {
              if((*polygons[ipoly_repl]->ivertices)[ivtx_poly_repl].ivertex
                  == ivtx)
              {
                (*polygons[ipoly_repl]->ivertices)[ivtx_poly_repl].ivertex
                = new_ivtx;
              }
            }
          }
        }
      }
    }
  }

  new_vlist->num_varying_items = 0;
  new_vlist->max_varying_items = 0;

  //    printf("after pruning unused verts: fixed from %d to %d, new max varying %d, old max %d\nnew list num %d max %d old num %d max %d",
  //           new_vlist->num_fixed_items,
  //           vertices->num_fixed_items,
  //           new_vlist->max_varying_items,
  //           vertices->max_varying_items,
  //           new_vlist->list->num_items,
  //           new_vlist->list->max_items,
  //           vertices->list->num_items,
  //           vertices->list->max_items
  //  );

  for(int i=new_vlist->list->max_items - 1;
      i >= new_vlist->num_fixed_items;
      i--)
  {
    //      printf("deleting unused vertex: lnum %d lmax %d\n",
    //             new_vlist->list->num_items,
    //             new_vlist->list->max_items
    //  );
    delete ( & (*new_vlist)[i] );
    new_vlist->list->max_items--;
  }

  new_vlist->list->num_items = new_vlist->num_fixed_items;

  //    printf("after deleting unused verts: new max varying %d\n",
  //           new_vlist->max_varying_items);

  l3d_two_part_list<l3d_coordinate> *old_vlist;

  old_vlist = vertices;
  vertices = new_vlist;

  delete old_vlist;

  //    printf("self stats\n");
  //    printf("polygons: %d/%d\n", polygons.num_items, polygons.max_items);
  //    printf("vertices: %d/%d\n", vertices->list->num_items, vertices->list->max_items);
  //    printf("poly node pool: %d/%d\n", polygon_node_pool.num_items, polygon_node_pool.max_items);
}





