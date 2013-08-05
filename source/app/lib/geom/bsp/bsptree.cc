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

#include "bsptree.h"
#include "halfspace.h"
#include "../object/obound.h"
#include "../../dynamics/plugins/vidmesh/vidmesh.h"
#include "../../tool_os/memman.h"

l3d_bsptree::l3d_bsptree(void) {
  root = NULL;
}

void l3d_bsptree::_recursive_delete_tree(l3d_halfspace *root) {
  if(root) {
    assert(root->children.num_items == 2);
    _recursive_delete_tree(root->children[0]);
    _recursive_delete_tree(root->children[1]);
    if(root->object) {
      delete root->object;
    }
  }
}

l3d_bsptree::~l3d_bsptree(void) {
  _recursive_delete_tree(root);
}

void l3d_bsptree::print_stats(l3d_halfspace *tree) {
  static int level = 0;
  level++;
  if(tree) {
    printf("%-*.*sLEVEL %d ", level*2, level*2, "", level);
    if(tree->object) {
      printf("%d polys, obj %p\n", tree->object->polygons.num_items, tree->object);
    } else {
      printf("NO OBJECT (0 polys)\n");
    }
    printf("%-*.*sleft child:\n", level*2, level*2, "");
    print_stats(tree->children[0]);
    printf("%-*.*sright child:\n", level*2, level*2, "");
    print_stats(tree->children[1]);
  }
  level--;
}

void l3d_bsptree::compute_for_object(l3d_halfspace *root,
                                     l3d_object_clippable_boundable *rest_of_obj)
{
  static int recurselevel=0;

  //    printf("bsp level %d with %d polys\n",
  //           recurselevel, rest_of_obj->polygons.num_items);
  recurselevel++;

  if(rest_of_obj->polygons.num_items == 1) {
    // polygons in this half space
    root->object = rest_of_obj;

    // plane defining this half-space
    l3d_polygon_3d_clippable *pc;
    pc = dynamic_cast<l3d_polygon_3d_clippable *>
         ((rest_of_obj->polygons)[0]);
    assert(pc);
    root->plane = pc->plane;

    // leaf node: no children
    root->children.num_items = 0;
    int cnum;
    root->children[cnum=root->children.next_index()] = NULL;
    root->children[cnum=root->children.next_index()] = NULL;

  }
  else {
    rest_of_obj->reset();

    int split_poly_num = rand() % rest_of_obj->polygons.num_items;
    l3d_polygon_3d_clippable *split_poly;
    split_poly = dynamic_cast<l3d_polygon_3d_clippable *>
                 ((rest_of_obj->polygons) [split_poly_num]);
    assert(split_poly);
    root->plane = split_poly->plane;

    l3d_object_clippable_boundable *left_obj, *right_obj;
    left_obj = new l3d_object_clippable_boundable(1);
    *left_obj = *rest_of_obj;
    right_obj = new l3d_object_clippable_boundable(1);
    *right_obj = *rest_of_obj;

    // bound the current geometry
    root->object = rest_of_obj;
    root->bounding_sphere.compute_around_object(root->object);

    // exactly one poly, the split poly, in this half space
    root->object->reset();
    l3d_polygon_3d_node *n;
    n = root->object->nonculled_polygon_nodes;
    while(n) {
      l3d_polygon_3d_clippable *pc;
      pc = dynamic_cast<l3d_polygon_3d_clippable*>(n->polygon);
      assert(pc);
      if(pc==split_poly) {
        root->object->nonculled_polygon_nodes = n;
        n->next = NULL;
        n->prev = NULL;
        break;
      }
      n = n->next;
    }
    assert(n); // the poly MUST have been found!
    root->object->make_current_geometry_permanent();

    // remove splitting poly from left obj and right obj
    left_obj->reset();
    //      printf("starting left %d\n",
    //             left_obj->polygons.num_items);

    int poly_i;
    poly_i = 0;
    n = left_obj->nonculled_polygon_nodes;
    while(n) {
      l3d_polygon_3d_clippable *pc;
      pc = dynamic_cast<l3d_polygon_3d_clippable*>(n->polygon);
      assert(pc);
      if(poly_i == split_poly_num) {
        //          printf("removed left\n");
        // take this polygon out of the list of nonculled_polygon_nodes
        if(n->prev) {
          n->prev->next = n->next;
        } else {
          left_obj->nonculled_polygon_nodes = n->next;
        }
        if(n->next) {
          n->next->prev = n->prev;
        }
        break;
      }
      n = n->next;
      poly_i++;
    }
    right_obj->reset();
    //      printf("starting right %d\n",
    //             right_obj->polygons.num_items);
    poly_i = 0;
    n = right_obj->nonculled_polygon_nodes;
    while(n) {
      l3d_polygon_3d_clippable *pc;
      pc = dynamic_cast<l3d_polygon_3d_clippable*>(n->polygon);
      assert(pc);
      if(poly_i==split_poly_num) {
        //          printf("removed right\n");
        // take this polygon out of the list of nonculled_polygon_nodes
        if(n->prev) {
          n->prev->next = n->next;
        } else {
          right_obj->nonculled_polygon_nodes = n->next;
        }
        if(n->next) {
          n->next->prev = n->prev;
        }
        break;
      }
      n = n->next;
      poly_i++;
    }

    // clip remaining geometry against splitting plane
    l3d_plane other_plane;
    other_plane.a = -split_poly->plane.a;
    other_plane.b = -split_poly->plane.b;
    other_plane.c = -split_poly->plane.c;
    other_plane.d = -split_poly->plane.d;

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

    left_obj->clip_to_plane(other_plane);
    right_obj->clip_to_plane(split_poly->plane);

    // make clip permanent
    left_obj->make_current_geometry_permanent();
    right_obj->make_current_geometry_permanent();

    //      printf("did split left %d right %d\n",
    //             left_obj->polygons.num_items,
    //             right_obj->polygons.num_items);

    // create new left and right child tree nodes and insert into tree

    l3d_halfspace *left, *right;
    if(left_obj->polygons.num_items) {
      left = new l3d_halfspace;
      left->object = left_obj;
      left->children.num_items = 0;
      left->bounding_sphere.compute_around_object(left_obj);
      //        printf("left boundsph %f %f %f, %f \n",
      //               left->bounding_sphere.center.transformed.X_,
      //               left->bounding_sphere.center.transformed.Y_,
      //               left->bounding_sphere.center.transformed.Z_,
      //               left->bounding_sphere.radius);

    } else {
      left = NULL;
    }
    if(right_obj->polygons.num_items) {
      right = new l3d_halfspace;
      right->object = right_obj;
      right->children.num_items = 0;
      right->bounding_sphere.compute_around_object(right_obj);
      //        printf("right boundsph %f %f %f, %f \n",
      //               right->bounding_sphere.center.transformed.X_,
      //               right->bounding_sphere.center.transformed.Y_,
      //               right->bounding_sphere.center.transformed.Z_,
      //               right->bounding_sphere.radius);

    } else {
      right = NULL;
    }

    root->children.num_items = 0;
    int cnum;
    root->children[cnum=root->children.next_index()] = left;
    root->children[cnum=root->children.next_index()] = right;

    if(left) { compute_for_object(left, left_obj); }
    if(right) { compute_for_object(right, right_obj); }
  }

  recurselevel--;

}

void l3d_bsptree::_permanently_merge_objects
(l3d_object *dest, l3d_object *src)
{
  if(!src) return;

  dest->reset();
  assert ( { int result = 1; for(int i=0;i<dest->polygons.num_items; i++) {for(int iv=0; iv<dest->polygons[i]->ivertices->num_items; iv++) { result = result && ( (*dest->polygons[i]->ivertices)[iv].ivertex <(*dest->polygons[i]->vlist)->num_fixed_items ); } } result; } );
  l3d_polygon_3d_node *n;
  n = src->nonculled_polygon_nodes;
  while(n) {
    assert ( { int result = 1; for(int i=0;i<dest->polygons.num_items; i++) {for(int iv=0; iv<dest->polygons[i]->ivertices->num_items; iv++) { result = result && ( (*dest->polygons[i]->ivertices)[iv].ivertex <(*dest->polygons[i]->vlist)->num_fixed_items ); } } result; } );
    l3d_polygon_2d *poly;
    poly = n->polygon->clone(); //- warning: does not clone texture

    //- point texture in cloned polygon to the destination mesh's texture,
    //- otherwise the cloned polygon still points to the original mesh's tex,
    //- which is a problem when the original mesh is deleted
    l3d_polygon_3d_textured *ptex;
    ptex = dynamic_cast<l3d_polygon_3d_textured *> (poly);
    if(ptex) {
      l3d_plugin_videoscape_mesh *mesh =
        (l3d_plugin_videoscape_mesh *)dest->plugin_data;
      assert(mesh->tex_data != ptex->texture->tex_data);
      ptex->texture->tex_data = mesh->tex_data;
    }
    poly->ivertices->num_items = 0;
    int ivtx;
    for(ivtx=0; ivtx<poly->clip_ivertices->num_items; ivtx++) {
      int new_ivtx;
      int found_new_ivtx=0;
      for(new_ivtx=0;
          new_ivtx<dest->vertices->num_fixed_items &&
          ! found_new_ivtx;
          new_ivtx++)
      {
        if( (*dest->vertices)[new_ivtx].original.X_ ==
            (**poly->vlist)[(*poly->clip_ivertices)[ivtx].ivertex].original.X_
            &&  (*dest->vertices)[new_ivtx].original.Y_ ==
            (**poly->vlist)[(*poly->clip_ivertices)[ivtx].ivertex].original.Y_
            &&  (*dest->vertices)[new_ivtx].original.Z_ ==
            (**poly->vlist)[(*poly->clip_ivertices)[ivtx].ivertex].original.Z_
          )
        {
          found_new_ivtx = 1;
          int next_idx = poly->ivertices->next_index();
          (*poly->ivertices)[next_idx].ivertex = new_ivtx;
        }
      }
      if(!found_new_ivtx) {
        dest->vertices->next_varying_index();
        dest->vertices->num_varying_items --;
        dest->vertices->max_varying_items --;
        dest->vertices->num_fixed_items ++;
        (*dest->vertices)
        [dest->vertices->num_fixed_items - 1].original
        =
          (**poly->vlist)[(*poly->clip_ivertices)[ivtx].ivertex].original ;

        int next_idx = poly->ivertices->next_index();
        (*poly->ivertices)[next_idx].ivertex =
          dest->vertices->num_fixed_items - 1;
      }

      assert ( (*poly->ivertices)[ivtx].ivertex < dest->vertices->num_fixed_items);
      assert ( { int result = 1; for(int iv=0; iv<ivtx; iv++) { result = result && ( (*poly->ivertices)[iv].ivertex <dest->vertices->num_fixed_items ); printf("while assigning: iv %d is %d/%d",iv,(*poly->ivertices)[iv].ivertex,dest->vertices->num_fixed_items); } result; } );

    }
    assert ( { int result = 1; for(int iv=0; iv<poly->ivertices->num_items; iv++) { result = result && ( (*poly->ivertices)[iv].ivertex <dest->vertices->num_fixed_items ); printf("ivtx %d is %d / %d\n",iv, (*poly->ivertices)[iv].ivertex, dest->vertices->num_fixed_items);} result; } );

    poly->vlist = &dest->vertices;
    assert ( { int result = 1; for(int iv=0; iv<poly->ivertices->num_items; iv++) { result = result && ( (*poly->ivertices)[iv].ivertex <(*poly->vlist)->num_fixed_items ); } result; } );
    int pnum = dest->polygons.next_index();
    l3d_polygon_3d_clippable *poly3;
    poly3 = dynamic_cast<l3d_polygon_3d_clippable *> (poly);
    assert(poly3);
    dest->polygons[pnum] = poly3;
    poly3->compute_center();
    poly3->compute_sfcnormal();
    poly3->plane.align_with_point_normal
    (poly3->center.original,
     normalized(poly3->sfcnormal.original - poly3->center.original));

    assert ( { int result = 1; for(int i=0;i<dest->polygons.num_items; i++) {for(int iv=0; iv<dest->polygons[i]->ivertices->num_items; iv++) { result = result && ( (*dest->polygons[i]->ivertices)[iv].ivertex <(*dest->polygons[i]->vlist)->num_fixed_items ); } } result; } );
    n = n->next;
  }

  assert ( { int result = 1; for(int i=0;i<dest->polygons.num_items; i++) {for(int iv=0; iv<dest->polygons[i]->ivertices->num_items; iv++) { result = result && ( (*dest->polygons[i]->ivertices)[iv].ivertex <(*dest->polygons[i]->vlist)->num_fixed_items ); } } result; } );
}


void l3d_bsptree::push_polys_to_children
(l3d_halfspace *tree,
 l3d_object_clippable_boundable *unsplit_obj_from_parent)
{
  static int lvl=0;
  lvl++;
  //    printf("%-*.*s level %d, ihave %d polys, from parent %d\n",lvl*2,lvl*2,"",lvl, tree->object->polygons.num_items, unsplit_obj_from_parent?unsplit_obj_from_parent->polygons.num_items:0);
  if(tree) {

    //- define front and back clipping planes

    l3d_plane front_clip_plane, back_clip_plane;
    back_clip_plane.a = -tree->plane.a;
    back_clip_plane.b = -tree->plane.b;
    back_clip_plane.c = -tree->plane.c;
    back_clip_plane.d = -tree->plane.d;
    front_clip_plane.a = tree->plane.a;
    front_clip_plane.b = tree->plane.b;
    front_clip_plane.c = tree->plane.c;
    front_clip_plane.d = tree->plane.d;

    l3d_object_clippable_boundable *back_obj;
    back_obj = new l3d_object_clippable_boundable
               (tree->object->vertices->num_fixed_items);
    int should_delete_back_obj = 1;
    assert(tree->object);
    *back_obj = *tree->object;
    assert(back_obj->polygons.num_items == 1);
    //- uncomment the following to cause all polys to end up in the
    //- right children in the tree (be sure to comment out the similar
    //- assignment below to front_obj->polygons.num_items = 0)
    //-
    back_obj->polygons.num_items = 0;
    if(unsplit_obj_from_parent) {
      unsplit_obj_from_parent->reset();
      unsplit_obj_from_parent->clip_to_plane(back_clip_plane);
      _permanently_merge_objects(back_obj, unsplit_obj_from_parent);
    }

    if(back_obj->polygons.num_items > 0) {
      back_obj->reset();

      if(tree->children[0]) {
        push_polys_to_children(tree->children[0], back_obj);
      } else {
        //- there previously were no polys in the back half space, but
        //- now due to a poly pushed down from the parent, there are.
        //- so create a new left child.
        tree->children[0] = new l3d_halfspace;
        tree->children[0]->object = back_obj;
        tree->children[0]->children.num_items = 0;
        tree->children[0]->children[tree->children[0]->children.next_index()]
        = NULL;
        tree->children[0]->children[tree->children[0]->children.next_index()]
        = NULL;
        tree->children[0]->bounding_sphere.compute_around_object(back_obj);
        // no assignment to tree->children[0]->plane; its a leaf node with no
        // splitting plane

        should_delete_back_obj = 0;
      }
    }
    else {
      if(tree->children[0]) {
        push_polys_to_children(tree->children[0], NULL);
      }
    }

    if(should_delete_back_obj) { delete back_obj; }

    l3d_object_clippable_boundable *front_obj;
    front_obj = new l3d_object_clippable_boundable
                (tree->object->vertices->num_fixed_items);
    int should_delete_front_obj = 1;
    assert(tree->object);
    *front_obj = *tree->object;
    assert(front_obj->polygons.num_items == 1);
    //- uncomment the following to cause all polys to end up in the
    //- left children in the tree (be sure to comment out the similar
    //- assignment above to back_obj->polygons.num_items = 0)
    //-
    //- front_obj->polygons.num_items = 0;
    if(unsplit_obj_from_parent) {
      unsplit_obj_from_parent->reset();
      unsplit_obj_from_parent->clip_to_plane(front_clip_plane);
      _permanently_merge_objects(front_obj, unsplit_obj_from_parent);
    }
    if(front_obj->polygons.num_items > 0) {
      front_obj->reset();
      if(tree->children[1]) {
        push_polys_to_children(tree->children[1], front_obj);
      } else {
        //- there previously were no polys in the front half space, but
        //- now due to a poly pushed down from the parent, there are.
        //- so create a new right child.
        tree->children[1] = new l3d_halfspace;
        tree->children[1]->object = front_obj;
        tree->children[1]->children.num_items = 0;
        tree->children[1]->children[tree->children[1]->children.next_index()]
        = NULL;
        tree->children[1]->children[tree->children[1]->children.next_index()]
        = NULL;
        tree->children[1]->bounding_sphere.compute_around_object(front_obj);
        // no assignment to tree->children[1]->plane; its a leaf node with no
        // splitting plane

        should_delete_front_obj = 0;
      }
    }
    else {
      if(tree->children[1]) {
        push_polys_to_children(tree->children[1], NULL);
      }
    }
    if(should_delete_front_obj) { delete front_obj; }

    assert(tree->object);
    delete tree->object;
    tree->object = NULL;
  }
  //    printf("%-*.*s EXITING LEVEL\n",lvl*2,lvl*2,"");
  lvl--;
}


void l3d_bsptree::make_videoscape_meshes(l3d_halfspace *tree) {
  static int level = 0;
  static int filecounter=0;
  level++;
  if(tree) {
    //      printf("%-*.*s", level*2, level*2, "");
    if(tree->object) {
      //        printf(" writing %d polys\n", tree->object->polygons.num_items);
      FILE *fp;
      char fname[512];
      sprintf(fname,"vid%d.obj", filecounter);
      fp = fopen(fname,"wb");
      fprintf(fp,"3DG1\n");
      fprintf(fp,"%d\n", tree->object->vertices->num_fixed_items);
      for(int i=0; i<tree->object->vertices->num_fixed_items; i++) {
        fprintf(fp, "%f %f %f\n",
                l3d_real_to_float( (*tree->object->vertices)[i].original.X_),
                l3d_real_to_float( (*tree->object->vertices)[i].original.Z_),
                l3d_real_to_float( (*tree->object->vertices)[i].original.Y_));
      }
      for(int ip=0; ip<tree->object->polygons.num_items; ip++) {
        fprintf(fp,"%d ", (*tree->object->polygons[ip]->ivertices).num_items);
        for(int iv = (*tree->object->polygons[ip]->ivertices).num_items-1;
            iv >= 0;
            iv--)
        {
          fprintf(fp,"%d ",
                  (*tree->object->polygons[ip]->ivertices)[iv].ivertex);
        }
        fprintf(fp,"0xf0f0f0\n");
      }
      fclose(fp);
      filecounter++;
    } else {
      //        printf(" NO OBJ\n");
    }
    make_videoscape_meshes(tree->children[0]);
    make_videoscape_meshes(tree->children[1]);
  }
  level--;
}

