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

#include "../lib/geom/object/sector.h"
#include "../lib/geom/polygon/p3_flat.h"
#include "../lib/tool_2d/screen.h"
#include "../lib/tool_os/dispatch.h"
#include "../lib/raster/rast3.h"
#include "../lib/tool_2d/scrinfo.h"
#include "../lib/geom/world/w_bsp.h"
#include "../lib/system/fact0_2.h"
#include "../lib/pipeline/pi_wor.h"
#include "../lib/dynamics/plugins/plugenv.h"
#include "../lib/geom/texture/tl_ppm.h"
#include "../lib/dynamics/plugins/vidmesh/vidmesh.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include "../lib/geom/bsp/halfspace.h"
#include "../lib/geom/bsp/bsptree.h"

//  void print_stats(l3d_halfspace *tree) {
//    static int level = 0;
//    level++;
//    if(tree) {
//      printf("%-*.*s", level*2, level*2, "");
//      if(tree->object) {
//        printf("%d polys, obj %p\n", tree->object->polygons.num_items, tree->object);
//      } else {
//        printf(" NO OBJ\n");
//      }
//      stats(tree->children[0]);
//      stats(tree->children[1]);
//    }
//    level--;
//  }

//  void compute_bsp_tree(l3d_halfspace *root,
//                        l3d_object_clippable_boundable *rest_of_obj)
//  {
//    static int recurselevel=0;

//    printf("bsp level %d with %d polys\n",
//           recurselevel, rest_of_obj->polygons.num_items);
//    recurselevel++;

//    if(rest_of_obj->polygons.num_items == 1) {
//      // polygons in this half space
//      root->object = rest_of_obj;

//      // plane defining this half-space
//      l3d_polygon_3d_clippable *pc;
//      pc = dynamic_cast<l3d_polygon_3d_clippable *>
//        ((rest_of_obj->polygons)[0]);
//      assert(pc);
//      root->plane = pc->plane;

//      // leaf node: no children
//      root->children.num_items = 0;
//      int cnum;
//      root->children[cnum=root->children.next_index()] = NULL;
//      root->children[cnum=root->children.next_index()] = NULL;

//    }
//    else {
//      rest_of_obj->reset();

//      int split_poly_num = rand() % rest_of_obj->polygons.num_items;
//      l3d_polygon_3d_clippable *split_poly;
//      split_poly = dynamic_cast<l3d_polygon_3d_clippable *>
//        ((rest_of_obj->polygons) [split_poly_num]);
//      assert(split_poly);
//      root->plane = split_poly->plane;

//      l3d_object_clippable_boundable *left_obj, *right_obj;
//      left_obj = new l3d_object_clippable_boundable(1);
//      *left_obj = *rest_of_obj;
//      right_obj = new l3d_object_clippable_boundable(1);
//      *right_obj = *rest_of_obj;

//      // bound the current geometry
//      root->object = rest_of_obj;
//      root->bounding_sphere.compute_around_object(root->object);

//      // exactly one poly, the split poly, in this half space
//      root->object->reset();
//      l3d_polygon_3d_node *n;
//      n = root->object->nonculled_polygon_nodes;
//      while(n) {
//        l3d_polygon_3d_clippable *pc;
//        pc = dynamic_cast<l3d_polygon_3d_clippable*>(n->polygon);
//        assert(pc);
//        if(pc==split_poly) {
//          root->object->nonculled_polygon_nodes = n;
//          n->next = NULL;
//          n->prev = NULL;
//          break;
//        }
//        n = n->next;
//      }
//      assert(n); // the poly MUST have been found!
//      root->object->make_current_geometry_permanent();

//      // remove splitting poly from left obj and right obj
//      left_obj->reset();
//      printf("starting left %d\n",
//             left_obj->polygons.num_items);

//      int poly_i;
//      poly_i = 0;
//      n = left_obj->nonculled_polygon_nodes;
//      while(n) {
//        l3d_polygon_3d_clippable *pc;
//        pc = dynamic_cast<l3d_polygon_3d_clippable*>(n->polygon);
//        assert(pc);
//        if(poly_i == split_poly_num) {
//          printf("removed left\n");
//          // take this polygon out of the list of nonculled_polygon_nodes
//          if(n->prev) {
//            n->prev->next = n->next;
//          } else {
//            left_obj->nonculled_polygon_nodes = n->next;
//          }
//          if(n->next) {
//            n->next->prev = n->prev;
//          }
//          break;
//        }
//        n = n->next;
//        poly_i++;
//      }
//      right_obj->reset();
//      printf("starting right %d\n",
//             right_obj->polygons.num_items);
//      poly_i = 0;
//      n = right_obj->nonculled_polygon_nodes;
//      while(n) {
//        l3d_polygon_3d_clippable *pc;
//        pc = dynamic_cast<l3d_polygon_3d_clippable*>(n->polygon);
//        assert(pc);
//        if(poly_i==split_poly_num) {
//          printf("removed right\n");
//          // take this polygon out of the list of nonculled_polygon_nodes
//          if(n->prev) {
//            n->prev->next = n->next;
//          } else {
//            right_obj->nonculled_polygon_nodes = n->next;
//          }
//          if(n->next) {
//            n->next->prev = n->prev;
//          }
//          break;
//        }
//        n = n->next;
//        poly_i++;
//      }

//      // clip remaining geometry against splitting plane
//      l3d_plane other_plane;
//      other_plane.a = -split_poly->plane.a;
//      other_plane.b = -split_poly->plane.b;
//      other_plane.c = -split_poly->plane.c;
//      other_plane.d = -split_poly->plane.d;

//      printf("clipping left to %f %f %f %f\n",
//             other_plane.a,
//             other_plane.b,
//             other_plane.c,
//             other_plane.d);
//      printf("clipping right to %f %f %f %f\n",
//             split_poly->plane.a,
//             split_poly->plane.b,
//             split_poly->plane.c,
//             split_poly->plane.d);

//      left_obj->clip_to_plane(other_plane);
//      right_obj->clip_to_plane(split_poly->plane);

//      // make clip permanent
//      left_obj->make_current_geometry_permanent();
//      right_obj->make_current_geometry_permanent();

//      printf("did split left %d right %d\n",
//             left_obj->polygons.num_items,
//             right_obj->polygons.num_items);

//      // create new left and right child tree nodes and insert into tree

//      l3d_halfspace *left, *right;
//      if(left_obj->polygons.num_items) {
//        left = new l3d_halfspace;
//        left->object = left_obj;
//        left->children.num_items = 0;
//        left->bounding_sphere.compute_around_object(left_obj);
//        printf("left boundsph %f %f %f, %f \n",
//               left->bounding_sphere.center.transformed.X_,
//               left->bounding_sphere.center.transformed.Y_,
//               left->bounding_sphere.center.transformed.Z_,
//               left->bounding_sphere.radius);

//      } else {
//        left = NULL;
//      }
//      if(right_obj->polygons.num_items) {
//        right = new l3d_halfspace;
//        right->object = right_obj;
//        right->children.num_items = 0;
//        right->bounding_sphere.compute_around_object(right_obj);
//        printf("right boundsph %f %f %f, %f \n",
//               right->bounding_sphere.center.transformed.X_,
//               right->bounding_sphere.center.transformed.Y_,
//               right->bounding_sphere.center.transformed.Z_,
//               right->bounding_sphere.radius);

//      } else {
//        right = NULL;
//      }

//      root->children.num_items = 0;
//      int cnum;
//      root->children[cnum=root->children.next_index()] = left;
//      root->children[cnum=root->children.next_index()] = right;

//      if(left) { compute_bsp_tree(left, left_obj); }
//      if(right) { compute_bsp_tree(right, right_obj); }
//    }

//    recurselevel--;

//  }

//  void permanently_merge_objects(l3d_object *dest, l3d_object *src) {
//    if(!src) return;

//    dest->reset();
//    assert ( { int result = 1; for(int i=0;i<dest->polygons.num_items; i++) {for(int iv=0; iv<dest->polygons[i]->ivertices->num_items; iv++) { result = result && ( (*dest->polygons[i]->ivertices)[iv].ivertex <(*dest->polygons[i]->vlist)->num_fixed_items ); } } result; } );
//    l3d_polygon_3d_node *n;
//    n = src->nonculled_polygon_nodes;
//    while(n) {
//      assert ( { int result = 1; for(int i=0;i<dest->polygons.num_items; i++) {for(int iv=0; iv<dest->polygons[i]->ivertices->num_items; iv++) { result = result && ( (*dest->polygons[i]->ivertices)[iv].ivertex <(*dest->polygons[i]->vlist)->num_fixed_items ); } } result; } );
//      l3d_polygon_2d *poly;
//      poly = n->polygon->clone(); //- warning: does not clone texture

//      //- point texture in cloned polygon to the destination mesh's texture,
//      //- otherwise the cloned polygon still points to the original mesh's tex,
//      //- which is a problem when the original mesh is deleted
//      l3d_polygon_3d_textured *ptex;
//      ptex = dynamic_cast<l3d_polygon_3d_textured *> (poly);
//      if(ptex) {
//        l3d_plugin_videoscape_mesh *mesh =
//          (l3d_plugin_videoscape_mesh *)dest->plugin_data;
//        assert(mesh->tex_data != ptex->texture->tex_data);
//        ptex->texture->tex_data = mesh->tex_data;
//      }
//      poly->ivertices->num_items = 0;
//      int ivtx;
//      for(ivtx=0; ivtx<poly->clip_ivertices->num_items; ivtx++) {
//        int new_ivtx;
//        int found_new_ivtx=0;
//        for(new_ivtx=0;
//            new_ivtx<dest->vertices->num_fixed_items &&
//              ! found_new_ivtx;
//            new_ivtx++)
//          {
//            if( (*dest->vertices)[new_ivtx].original.X_ ==
//                (**poly->vlist)[(*poly->clip_ivertices)[ivtx].ivertex].original.X_
//                &&  (*dest->vertices)[new_ivtx].original.Y_ ==
//                (**poly->vlist)[(*poly->clip_ivertices)[ivtx].ivertex].original.Y_
//                &&  (*dest->vertices)[new_ivtx].original.Z_ ==
//                (**poly->vlist)[(*poly->clip_ivertices)[ivtx].ivertex].original.Z_
//                )
//              {
//                found_new_ivtx = 1;
//                int next_idx = poly->ivertices->next_index();
//                (*poly->ivertices)[next_idx].ivertex = new_ivtx;
//              }
//          }
//        if(!found_new_ivtx) {
//          dest->vertices->next_varying_index();
//          dest->vertices->num_varying_items --;
//          dest->vertices->max_varying_items --;
//          dest->vertices->num_fixed_items ++;
//          (*dest->vertices)
//            [dest->vertices->num_fixed_items - 1].original
//            =
//            (**poly->vlist)[(*poly->clip_ivertices)[ivtx].ivertex].original ;

//          int next_idx = poly->ivertices->next_index();
//          (*poly->ivertices)[next_idx].ivertex =
//            dest->vertices->num_fixed_items - 1;
//        }

//        assert ( (*poly->ivertices)[ivtx].ivertex < dest->vertices->num_fixed_items);
//        assert ( { int result = 1; for(int iv=0; iv<ivtx; iv++) { result = result && ( (*poly->ivertices)[iv].ivertex <dest->vertices->num_fixed_items ); printf("while assigning: iv %d is %d/%d",iv,(*poly->ivertices)[iv].ivertex,dest->vertices->num_fixed_items); } result; } );

//      }
//      assert ( { int result = 1; for(int iv=0; iv<poly->ivertices->num_items; iv++) { result = result && ( (*poly->ivertices)[iv].ivertex <dest->vertices->num_fixed_items ); printf("ivtx %d is %d / %d\n",iv, (*poly->ivertices)[iv].ivertex, dest->vertices->num_fixed_items);} result; } );

//      poly->vlist = &dest->vertices;
//      assert ( { int result = 1; for(int iv=0; iv<poly->ivertices->num_items; iv++) { result = result && ( (*poly->ivertices)[iv].ivertex <(*poly->vlist)->num_fixed_items ); } result; } );
//      int pnum = dest->polygons.next_index();
//      l3d_polygon_3d_clippable *poly3;
//      poly3 = dynamic_cast<l3d_polygon_3d_clippable *> (poly);
//      assert(poly3);
//      dest->polygons[pnum] = poly3;
//      poly3->compute_center();
//      poly3->compute_sfcnormal();
//      poly3->plane.align_with_point_normal
//        (poly3->center.original,
//         normalized(poly3->sfcnormal.original - poly3->center.original));

//      assert ( { int result = 1; for(int i=0;i<dest->polygons.num_items; i++) {for(int iv=0; iv<dest->polygons[i]->ivertices->num_items; iv++) { result = result && ( (*dest->polygons[i]->ivertices)[iv].ivertex <(*dest->polygons[i]->vlist)->num_fixed_items ); } } result; } );
//      n = n->next;
//    }

//    assert ( { int result = 1; for(int i=0;i<dest->polygons.num_items; i++) {for(int iv=0; iv<dest->polygons[i]->ivertices->num_items; iv++) { result = result && ( (*dest->polygons[i]->ivertices)[iv].ivertex <(*dest->polygons[i]->vlist)->num_fixed_items ); } } result; } );
//  }


//  void push_polys_to_children
//  (l3d_halfspace *tree,
//   l3d_object_clippable_boundable *unsplit_obj_from_parent)
//  {
//    static int lvl=0;
//    lvl++;
//    printf("%-*.*s level %d, ihave %d polys, from parent %d\n",lvl*2,lvl*2,"",lvl, tree->object->polygons.num_items, unsplit_obj_from_parent?unsplit_obj_from_parent->polygons.num_items:0);
//    if(tree) {

//      if(0 && !(tree->children[0] || tree->children[1])) { //- a leaf node
//        permanently_merge_objects(tree->object, unsplit_obj_from_parent);
//        printf("%-*.*s BINGO, leaf, ihave %d polys, from parent %d\n",lvl*2,lvl*2,"", tree->object->polygons.num_items, unsplit_obj_from_parent?unsplit_obj_from_parent->polygons.num_items:0);

//      } else {
//        //- define front and back clipping planes

//        l3d_plane front_clip_plane, back_clip_plane;
//        back_clip_plane.a = -tree->plane.a;
//        back_clip_plane.b = -tree->plane.b;
//        back_clip_plane.c = -tree->plane.c;
//        back_clip_plane.d = -tree->plane.d;
//        front_clip_plane.a = tree->plane.a;
//        front_clip_plane.b = tree->plane.b;
//        front_clip_plane.c = tree->plane.c;
//        front_clip_plane.d = tree->plane.d;

//        l3d_object_clippable_boundable *back_obj;
//        back_obj = new l3d_object_clippable_boundable(tree->object->vertices->num_fixed_items);
//        int should_delete_back_obj = 1;
//        assert(tree->object);
//        *back_obj = *tree->object;
//        assert(back_obj->polygons.num_items == 1);
//        back_obj->polygons.num_items = 0;
//        if(unsplit_obj_from_parent) {
//          unsplit_obj_from_parent->reset();
//          unsplit_obj_from_parent->clip_to_plane(back_clip_plane);
//          permanently_merge_objects(back_obj, unsplit_obj_from_parent);
//        }

//        if(back_obj->polygons.num_items > 0) {
//          back_obj->reset();

//          if(tree->children[0]) {
//            push_polys_to_children(tree->children[0], back_obj);
//          } else {
//            //- non-leaf node (determined earlier), so create new left child
//            tree->children[0] = new l3d_halfspace;
//            tree->children[0]->object = back_obj;
//            tree->children[0]->children.num_items = 0;
//            tree->children[0]->children[tree->children[0]->children.next_index()]
//              = NULL;
//            tree->children[0]->children[tree->children[0]->children.next_index()]
//              = NULL;
//            tree->children[0]->bounding_sphere.compute_around_object(back_obj);
//            // no assignment to tree->children[0]->plane; its a leaf node with no
//            // splitting plane

//            should_delete_back_obj = 0;
//          }
//        }
//        else {
//          if(tree->children[0]) {
//            push_polys_to_children(tree->children[0], NULL);
//          }
//        }

//        if(should_delete_back_obj) { delete back_obj; }

//        l3d_object_clippable_boundable *front_obj;
//        front_obj = new l3d_object_clippable_boundable(tree->object->vertices->num_fixed_items);
//        int should_delete_front_obj = 1;
//        assert(tree->object);
//        *front_obj = *tree->object;
//        assert(front_obj->polygons.num_items == 1);
//        if(unsplit_obj_from_parent) {
//          unsplit_obj_from_parent->reset();
//          unsplit_obj_from_parent->clip_to_plane(front_clip_plane);
//          permanently_merge_objects(front_obj, unsplit_obj_from_parent);
//        }
//        if(front_obj->polygons.num_items > 0) {
//          front_obj->reset();
//          if(tree->children[1]) {
//            push_polys_to_children(tree->children[1], front_obj);
//          } else {
//            //- non-leaf node (determined earlier), so create new right child
//            //- create new right child
//            tree->children[1] = new l3d_halfspace;
//            tree->children[1]->object = front_obj;
//            tree->children[1]->children.num_items = 0;
//            tree->children[1]->children[tree->children[1]->children.next_index()]
//              = NULL;
//            tree->children[1]->children[tree->children[1]->children.next_index()]
//              = NULL;
//            tree->children[1]->bounding_sphere.compute_around_object(front_obj);
//            // no assignment to tree->children[1]->plane; its a leaf node with no
//            // splitting plane

//            should_delete_front_obj = 0;
//          }
//        }
//        else {
//          if(tree->children[1]) {
//            push_polys_to_children(tree->children[1], NULL);
//          }
//        }
//        if(should_delete_front_obj) { delete front_obj; }

//        assert(tree->object);
//        delete tree->object;
//        tree->object = NULL;
//      }
//    }
//    printf("%-*.*s EXITING LEVEL\n",lvl*2,lvl*2,"");
//    lvl--;
//  }


//  void make_videoscape_meshes(l3d_halfspace *tree) {
//    static int level = 0;
//    static int filecounter=0;
//    level++;
//    if(tree) {
//      printf("%-*.*s", level*2, level*2, "");
//      if(tree->object) {
//        printf(" writing %d polys\n", tree->object->polygons.num_items);
//        FILE *fp;
//        char fname[512];
//        sprintf(fname,"vid%d.obj", filecounter);
//        fp = fopen(fname,"wb");
//        fprintf(fp,"3DG1\n");
//        fprintf(fp,"%d\n", tree->object->vertices->num_fixed_items);
//        for(int i=0; i<tree->object->vertices->num_fixed_items; i++) {
//          fprintf(fp, "%f %f %f\n",
//                  l3d_real_to_float( (*tree->object->vertices)[i].original.X_),
//                  l3d_real_to_float( (*tree->object->vertices)[i].original.Z_),
//                  l3d_real_to_float( (*tree->object->vertices)[i].original.Y_));
//        }
//        for(int ip=0; ip<tree->object->polygons.num_items; ip++) {
//          fprintf(fp,"%d ", (*tree->object->polygons[ip]->ivertices).num_items);
//          for(int iv = (*tree->object->polygons[ip]->ivertices).num_items-1;
//              iv >= 0;
//              iv--)
//            {
//              fprintf(fp,"%d ",
//                      (*tree->object->polygons[ip]->ivertices)[iv].ivertex);
//            }
//          fprintf(fp,"0xf0f0f0\n");
//        }
//        fclose(fp);
//        filecounter++;
//      } else {
//        printf(" NO OBJ\n");
//      }
//      make_videoscape_meshes(tree->children[0]);
//      make_videoscape_meshes(tree->children[1]);
//    }
//    level--;
//  }


class my_world:public l3d_world_bsptree {
  protected:
    l3d_texture_loader *texloader;
    l3d_surface_cache *scache;
  public:
    my_world(void);
    virtual ~my_world(void);
};

my_world::my_world(void)
    : l3d_world_bsptree(320,240)
{
  l3d_screen_info *si = screen->sinfo;

  camera->VRP.set(0,0,-50,0);
  camera->near_z = float_to_l3d_real(1.0);
  camera->far_z = int_to_l3d_real(50);

  //- for mesa rasterizer's reverse projection
  rasterizer_3d_imp->fovx = &(camera->fovx);
  rasterizer_3d_imp->fovy = &(camera->fovy);
  rasterizer_3d_imp->screen_xsize = &(screen->xsize);
  rasterizer_3d_imp->screen_ysize = &(screen->ysize);

  int i,j,k;

  texloader = new l3d_texture_loader_ppm(si);
  scache = new l3d_surface_cache(si);

  //- create a plugin object
  l3d_object_clippable_boundable *oc;

  oc =
    new l3d_object_clippable_boundable(10);
  //- max 10 fixed vertices, can be overridden by plug-in if desired
  //- by redefining the vertex list

  oc->plugin_loader =
    factory_manager_v_0_2.plugin_loader_factory->create();
  oc->plugin_loader->load("../lib/dynamics/plugins/vidmesh/vidmesh.so");
  oc->plugin_constructor =
    (void (*)(l3d_object *, void *))
    oc->plugin_loader->find_symbol("constructor");
  oc->plugin_update =
    (void (*)(l3d_object *))
    oc->plugin_loader->find_symbol("update");
  oc->plugin_destructor =
    (void (*)(l3d_object *))
    oc->plugin_loader->find_symbol("destructor");
  oc->plugin_copy_data =
    (void (*)(const l3d_object *, l3d_object *))
    oc->plugin_loader->find_symbol("copy_data");

  texloader = new l3d_texture_loader_ppm(si);
  scache = new l3d_surface_cache(si);

  char plugin_parms[4096];
  sprintf(plugin_parms, "%d %d %d  1 0 0 0 1 0 0 0 1 soup.obj soup.ppm soup.uv",
          0,0,0);

  l3d_plugin_environment *e =
     new l3d_plugin_environment
       (texloader, screen->sinfo, scache, (void *)plugin_parms);

  if(oc->plugin_constructor) {
    (*oc->plugin_constructor) (oc,e);
  }

  oc->bounding_sphere.compute_around_object(oc);

  // now, compute the bsp tree
  l3d_bsptree *tree;

  tree = new l3d_bsptree;
  tree->root = new l3d_halfspace;
  tree->root->bounding_sphere = oc->bounding_sphere;
  tree->root->object = NULL;
  tree->root->children.num_items = 0;
  tree->root->children[tree->root->children.next_index()] = 0;
  tree->root->children[tree->root->children.next_index()] = 0;

  tree->compute_for_object(tree->root, oc); // oc now belongs to the BSP tree and will be freed by the BSP tree

  the_tree = tree;

  screen->refresh_palette();
}

my_world::~my_world(void) {
  delete texloader;
  delete scache;
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
