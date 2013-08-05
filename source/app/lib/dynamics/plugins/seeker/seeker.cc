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

#include "../../../system/sys_dep.h"
#include "../../../geom/object/ocoll.h"
#include "../../../geom/object/sector.h"
#include "../../../geom/texture/texload.h"
#include "../../../geom/polygon/p3_ltex.h"
#include "../../../dynamics/plugins/plugenv.h"
#include <stdlib.h>
#include <string.h>
#include "seeker.h"

l3d_plugin_videoscape_mesh_seeker::l3d_plugin_videoscape_mesh_seeker
(l3d_plugin_environment *env)
{
  plugin_env = env;
  tex_data = NULL;
}

l3d_plugin_videoscape_mesh_seeker::~l3d_plugin_videoscape_mesh_seeker(void) {
  if(tex_data) delete tex_data;
  delete plugin_env;
}

extern "C" {

  void constructor(l3d_object *target, void *data) {
    l3d_object_clippable_boundable_collidable *obj;
    obj = dynamic_cast<l3d_object_clippable_boundable_collidable *> (target);
    assert(obj);

    obj->collision_sphere_radius = int_to_l3d_real(4);

    l3d_plugin_environment *env = (l3d_plugin_environment *)data;
    l3d_plugin_videoscape_mesh_seeker *mesh;

    target->plugin_data = mesh = new l3d_plugin_videoscape_mesh_seeker(env);
    l3d_texture_loader *l = env->texloader;

    // mesh-specific initialization
    // mesh->xxx = xxx
    mesh->tick_count = 0;
    mesh->max_ticks = rand() % 50 + 10;
    mesh->movement_state = states_array[rand()%7];

    char rest_parms[4096];
    strncpy(rest_parms, (char *)env->data, sizeof(rest_parms));
    char *tok;

    float posx,posy,posz;
    float xaxis_x, xaxis_y, xaxis_z,
    yaxis_x, yaxis_y, yaxis_z,
    zaxis_x, zaxis_y, zaxis_z;
    l3d_matrix position, orientation;
    strcpy(mesh->mesh_fname,"");
    strcpy(mesh->texcoord_fname,"");
    strcpy(mesh->texture_fname,"");
    position.set(int_to_l3d_real(1),
                 int_to_l3d_real(0),
                 int_to_l3d_real(0),
                 int_to_l3d_real(0),
                 int_to_l3d_real(0),
                 int_to_l3d_real(1),
                 int_to_l3d_real(0),
                 int_to_l3d_real(0),
                 int_to_l3d_real(0),
                 int_to_l3d_real(0),
                 int_to_l3d_real(1),
                 int_to_l3d_real(0),
                 int_to_l3d_real(0),
                 int_to_l3d_real(0),
                 int_to_l3d_real(0),
                 int_to_l3d_real(1));
    orientation = position;

    tok = strtok(rest_parms, " ");  if(tok) { sscanf(tok, "%f", &posx); }
    tok = strtok(NULL, " ");  if(tok) { sscanf(tok, "%f", &posy); }
    tok = strtok(NULL, " ");  if(tok) { sscanf(tok, "%f", &posz); }
    tok = strtok(NULL, " ");  if(tok) { sscanf(tok, "%f", &xaxis_x); }
    tok = strtok(NULL, " ");  if(tok) { sscanf(tok, "%f", &yaxis_x); }
    tok = strtok(NULL, " ");  if(tok) { sscanf(tok, "%f", &zaxis_x); }
    tok = strtok(NULL, " ");  if(tok) { sscanf(tok, "%f", &xaxis_y); }
    tok = strtok(NULL, " ");  if(tok) { sscanf(tok, "%f", &yaxis_y); }
    tok = strtok(NULL, " ");  if(tok) { sscanf(tok, "%f", &zaxis_y); }
    tok = strtok(NULL, " ");  if(tok) { sscanf(tok, "%f", &xaxis_z); }
    tok = strtok(NULL, " ");  if(tok) { sscanf(tok, "%f", &yaxis_z); }
    tok = strtok(NULL, " ");  if(tok) { sscanf(tok, "%f", &zaxis_z); }

    tok = strtok(NULL, " ");
    if(tok) { strncpy(mesh->mesh_fname, tok, sizeof(mesh->mesh_fname)); }
    tok = strtok(NULL, " ");
    if(tok) { strncpy(mesh->texture_fname, tok, sizeof(mesh->texture_fname)); }
    tok = strtok(NULL, " ");
    if(tok) { strncpy(mesh->texcoord_fname, tok, sizeof(mesh->texcoord_fname)); }

    l->load(mesh->texture_fname); // implies no sharing of common texture
    // ^data among objects, could be improved by having a texture
    // cache and reusing common texture data
    mesh->tex_data = new l3d_texture_data;
    mesh->tex_data->width = l->width;
    mesh->tex_data->height = l->height;
    mesh->tex_data->data = l->data;

    FILE *fp;
    FILE *fp_texcoord;
    fp = fopen(mesh->mesh_fname, "rt");
    fp_texcoord = fopen(mesh->texcoord_fname, "rt");
    char line[4096];
    if(fp) {
      fgets(line, sizeof(line), fp); // ID line
      fgets(line, sizeof(line), fp); // num vertices

      int num_vert;
      sscanf(line, "%d", &num_vert);
      int i;

      delete target->vertices;
      target->vertices =
        new l3d_two_part_list<l3d_coordinate> ( num_vert );

      // vertices

      for(i=0; i<num_vert; i++) {
        fgets(line, sizeof(line), fp); // vertex line
        float x,y,z;
        sscanf(line, "%f %f %f", &x, &y, &z);

        //- change from blender's right-handed +z-up system to a
        //- left-handed +y-up system
        (*target->vertices)[i].original.set
        (float_to_l3d_real(x),
         float_to_l3d_real(z),
         float_to_l3d_real(y),
         float_to_l3d_real(1.));
      }

      // polygons


      while(!feof(fp)) {
        fgets(line, sizeof(line), fp);
        if(feof(fp)) break;

        char *tok;

        int numv;
        tok = strtok(line, " \t\n\r");
        sscanf(tok, "%d", &numv);

        l3d_polygon_3d_textured *p;
        int polygon_idx = target->polygons.next_index();
        target->polygons[polygon_idx] = p =
                                          new l3d_polygon_3d_textured(numv);
        target->polygons[polygon_idx]->vlist = &target->vertices;

        // insert ivertex entries into list, for now in incorrect ccw order
        for(i=0; i<numv; i++) {
          int cur_iv=0;

          tok = strtok(NULL," \t\n\r");
          if(tok) {
            int ivtx;
            sscanf(tok,"%d", &ivtx);
            cur_iv=target->polygons[polygon_idx]->ivertices->next_index();
            (*(target->polygons[polygon_idx]->ivertices)) [cur_iv].ivertex
            = ivtx;

          }

          if(fp_texcoord) {
            // read tex coords (also in ccw order) from tex coord file
            // and insert them (for now in incorrect ccw order) into iv list
            char texcoord_line[1024];
            float u,v;
            fgets(texcoord_line, sizeof(texcoord_line), fp_texcoord);
            sscanf(texcoord_line, "%f %f", &u,&v);
            assert( dynamic_cast<l3d_polygon_ivertex_textured *>(&((*(target->polygons[polygon_idx]->ivertices))[cur_iv])));
            ((l3d_polygon_ivertex_textured *)
             (&((*(target->polygons[polygon_idx]->ivertices))[cur_iv])))
            ->tex_coord.X_ = float_to_l3d_real(u);
            ((l3d_polygon_ivertex_textured *)
             (&((*(target->polygons[polygon_idx]->ivertices))[cur_iv])))
            ->tex_coord.Y_ = float_to_l3d_real(1.0 - v);// VRML/blender reverses orientation of v axis
            // ((l3d_polygon_ivertex_textured *)
            //  (&((*(target->polygons[polygon_idx]->ivertices))[cur_iv])))
            //  ->tex_coord.Z_ = float_to_l3d_real( IS COMPUTED LATER IN CAM SPACE );
            ((l3d_polygon_ivertex_textured *)
             (&((*(target->polygons[polygon_idx]->ivertices))[cur_iv])))
            ->tex_coord.W_ = int_to_l3d_real(1);
          }
        }

        //- now reverse the list IN-PLACE; for this we need one temp swap
        //- variable, which we allocate (virtually from the list, i.e. we
        //- let the type of the temp object be allocated by the list itself)
        //- here:

        l3d_polygon_ivertex *
        swap_iv =
          &((*(target->polygons[polygon_idx]->ivertices))
            [target->polygons[polygon_idx]->ivertices->next_index()]);

        for(i=0; i<numv/2; i++) {
          *swap_iv = (*(target->polygons[polygon_idx]->ivertices))[i];
          (*(target->polygons[polygon_idx]->ivertices))[i] =
            (*(target->polygons[polygon_idx]->ivertices))[numv-1 - i];
          (*(target->polygons[polygon_idx]->ivertices))[numv-1 - i] =
            *swap_iv;

          //- don't swap the tex coords (i.e. swap them back)

          if(fp_texcoord) {
            ((l3d_polygon_ivertex_textured *)
             ( & (*(target->polygons[polygon_idx]->ivertices))[numv-1 - i]))
            ->tex_coord =
              ((l3d_polygon_ivertex_textured *)
               ( & (*(target->polygons[polygon_idx]->ivertices))[i]))
              ->tex_coord;

            ((l3d_polygon_ivertex_textured *)
             ( & (*(target->polygons[polygon_idx]->ivertices))[i]))
            ->tex_coord =
              ((l3d_polygon_ivertex_textured *) swap_iv)->tex_coord;
          }
        }

        // now shift all texture coords one earlier in the list
        // blender's vrml export seems to mess up the relationship
        // between coordinates and their texture coordinates by
        // displacing all tex coords by a position of 1 with respect to their
        // actual vertex, so we reset them back here

        if(fp_texcoord) {
          assert( dynamic_cast<l3d_polygon_ivertex_textured *>(swap_iv));
          assert( dynamic_cast<l3d_polygon_ivertex_textured *>( & (*(target->polygons[polygon_idx]->ivertices))[0] ));
          ((l3d_polygon_ivertex_textured *)swap_iv)->tex_coord =
            ((l3d_polygon_ivertex_textured *)
             ( & (*(target->polygons[polygon_idx]->ivertices))[0] ))
            ->tex_coord;
          for(i=0; i<numv-1; i++) {
            assert( dynamic_cast<l3d_polygon_ivertex_textured *>( & (*(target->polygons[polygon_idx]->ivertices))[i] ));
            assert( dynamic_cast<l3d_polygon_ivertex_textured *>( & (*(target->polygons[polygon_idx]->ivertices))[i+1] ));
            ((l3d_polygon_ivertex_textured *)
             ( & (*(target->polygons[polygon_idx]->ivertices))[i] ))
            ->tex_coord =
              ((l3d_polygon_ivertex_textured *)
               ( & (*(target->polygons[polygon_idx]->ivertices))[i+1] ))
              ->tex_coord;
          }
          assert( dynamic_cast<l3d_polygon_ivertex_textured *>( & (*(target->polygons[polygon_idx]->ivertices))[i] ));
          ((l3d_polygon_ivertex_textured *)
           ( & (*(target->polygons[polygon_idx]->ivertices))[i] ))->tex_coord =
             ((l3d_polygon_ivertex_textured *)swap_iv)->tex_coord;
        }

        // delete temp swap vertex
        target->polygons[polygon_idx]->ivertices->num_items =
          target->polygons[polygon_idx]->ivertices->num_items - 1;

        p->texture = new l3d_texture;
        p->texture->tex_data = mesh->tex_data;
        p->texture->owns_tex_data = false; // owned by the mesh, not the poly

        //- no texture coord file found, so assign default tex coordinates
        if(!fp_texcoord) {
          // assign the texture coord vectors -- only possible if we
          // have no manually assigned tex coords. this means that with
          // manually assigned tex coords we have no texture space, which
          // is correct (the manual assignment warps space nonlinearly).
          // this means that no light mapping is possible with manually
          // assigned tex coords since no texture space exists.

          p->texture->O = (**(target->polygons[polygon_idx]->vlist))
                          [(*(target->polygons[polygon_idx]->ivertices))[0].ivertex];
          p->texture->U = (**(target->polygons[polygon_idx]->vlist))
                          [(*(target->polygons[polygon_idx]->ivertices))[1].ivertex];
          p->texture->V = (**(target->polygons[polygon_idx]->vlist))
                          [(*(target->polygons[polygon_idx]->ivertices))
                           [p->ivertices->num_items - 1].ivertex];

          p->assign_tex_coords_from_tex_space(*p->texture);
        }

        target->polygons[polygon_idx]->compute_center();
        target->polygons[polygon_idx]->compute_sfcnormal();

        p->plane.align_with_point_normal(p->center.original, normalized(p->sfcnormal.original - p->center.original));

      }
      target->num_xforms = 2;

      orientation.set
      (float_to_l3d_real(xaxis_x),
       float_to_l3d_real(yaxis_x),
       float_to_l3d_real(zaxis_x),
       int_to_l3d_real(0),
       float_to_l3d_real(xaxis_y),
       float_to_l3d_real(yaxis_y),
       float_to_l3d_real(zaxis_y),
       int_to_l3d_real(0),
       float_to_l3d_real(xaxis_z),
       float_to_l3d_real(yaxis_z),
       float_to_l3d_real(zaxis_z),
       int_to_l3d_real(0),
       int_to_l3d_real(0),
       int_to_l3d_real(0),
       int_to_l3d_real(0),
       int_to_l3d_real(1));

      target->modeling_xforms[0] =
        orientation;

      position.set(int_to_l3d_real(1),
                   int_to_l3d_real(0),
                   int_to_l3d_real(0),
                   float_to_l3d_real(posx),
                   int_to_l3d_real(0),
                   int_to_l3d_real(1),
                   int_to_l3d_real(0),
                   float_to_l3d_real(posy),
                   int_to_l3d_real(0),
                   int_to_l3d_real(0),
                   int_to_l3d_real(1),
                   float_to_l3d_real(posz),
                   int_to_l3d_real(0),
                   int_to_l3d_real(0),
                   int_to_l3d_real(0),
                   int_to_l3d_real(1));


      target->modeling_xforms[1] = position;
      target->modeling_xform = target->modeling_xforms[1]  |
                               target->modeling_xforms[0];

      // object-specific initialization
    }

    if(fp) fclose(fp);
    if(fp_texcoord) fclose(fp_texcoord);
  }

  void update(l3d_object *target) {
    l3d_object_clippable_boundable_collidable *obj;
    obj = dynamic_cast<l3d_object_clippable_boundable_collidable *> (target);
    assert(obj);

    l3d_plugin_videoscape_mesh_seeker *mesh;
    mesh = (l3d_plugin_videoscape_mesh_seeker *)target->plugin_data;

    // mesh specific update
    // mesh->xxx = yyy

    l3d_point pos;
    l3d_vector move;

    // also update target->modeling_xform if necessary
    pos.set(target->modeling_xform.a[0][3],
            target->modeling_xform.a[1][3],
            target->modeling_xform.a[2][3],
            target->modeling_xform.a[3][3]);

    if(mesh->movement_state == NONE) {
    } else if(mesh->movement_state == AROUND_VRI) {
      l3d_vector rot;
      rot.set(target->modeling_xform.a[0][0],
              target->modeling_xform.a[1][0],
              target->modeling_xform.a[2][0],
              target->modeling_xform.a[3][0]);
      target->modeling_xform =
        target->modeling_xform | l3d_mat_rotu(rot, mesh->turning_velocity);
      // FIXME: reorthogonalize matrix here (normalize basis vectors)
    } else if(mesh->movement_state == AROUND_VUP) {
      l3d_vector rot;
      rot.set(target->modeling_xform.a[0][1],
              target->modeling_xform.a[1][1],
              target->modeling_xform.a[2][1],
              target->modeling_xform.a[3][1]);
      target->modeling_xform =
        target->modeling_xform | l3d_mat_rotu(rot, mesh->turning_velocity);
      // FIXME: reorthogonalize matrix here (normalize basis vectors)
    } else if(mesh->movement_state == AROUND_VFW) {
      l3d_vector rot;
      rot.set(target->modeling_xform.a[0][2],
              target->modeling_xform.a[1][2],
              target->modeling_xform.a[2][2],
              target->modeling_xform.a[3][2]);
      target->modeling_xform =
        target->modeling_xform | l3d_mat_rotu(rot, mesh->turning_velocity);
      // FIXME: reorthogonalize matrix here (normalize basis vectors)
    } else if(mesh->movement_state == ALONG_VFW) {
      move.set(target->modeling_xform.a[0][0],
               target->modeling_xform.a[1][0],
               target->modeling_xform.a[2][0],
               target->modeling_xform.a[3][0]);
      move.set(l3d_mulrr(mesh->moving_velocity, move.X_),
               l3d_mulrr(mesh->moving_velocity, move.Y_),
               l3d_mulrr(mesh->moving_velocity, move.Z_),
               int_to_l3d_real(0));
    } else if(mesh->movement_state == ALONG_VRI) {
      move.set(0,0,0,0);
      move.set(target->modeling_xform.a[0][1],
               target->modeling_xform.a[1][1],
               target->modeling_xform.a[2][1],
               target->modeling_xform.a[3][1]);
      move.set(l3d_mulrr(mesh->moving_velocity, move.X_),
               l3d_mulrr(mesh->moving_velocity, move.Y_),
               l3d_mulrr(mesh->moving_velocity, move.Z_),
               int_to_l3d_real(0));

    } else if(mesh->movement_state == ALONG_VUP) {
      move.set(0,0,0,0);
      move.set(target->modeling_xform.a[0][2],
               target->modeling_xform.a[1][2],
               target->modeling_xform.a[2][2],
               target->modeling_xform.a[3][2]);
      move.set(l3d_mulrr(mesh->moving_velocity, move.X_),
               l3d_mulrr(mesh->moving_velocity, move.Y_),
               l3d_mulrr(mesh->moving_velocity, move.Z_),
               int_to_l3d_real(0));
    }

    l3d_polygon_3d_node *n;
    l3d_sector *sector;
    sector = dynamic_cast<l3d_sector *> (target->parent);
    assert(sector);
    sector->reset();

    int some_collision_occurred = 1;

    //  obj->old_position = pos;
    // ^ already set earlier for ALL objects when they are reset (ocoll.cc)
    //   must have been set for all objects so that each obj here knows
    //   where the others are in world space

    pos = pos + move;
    obj->new_position = pos;

    // we know this pos is valid w.r.t. the polygons to be tested.
    // we cant say this for the intermediate positions since they
    // bounce around in intermediate posistions which while valid
    // for one poly might be suddenly invalid for another poly.

    // since we only check for coll w. front side of poly, a
    // bad intermediate bounce can push us on the wrong side of
    // a poly we should be on the inside of. but if we were orig.
    // outside the poly we should leave it be (this is the reason
    // we only check for coll w. front side of poly).

    // thus save the last VALID old position, and always use it also
    // as a frame of reference.

    // this means when we detect a collision we should also automatically
    // try and correct for it. note that this is a local correction; this
    // must be enclosed within a global while loop to check for global
    // non-collision (see below).

    obj->prevent_collision = 1;
    l3d_real explosion_factor = int_to_l3d_real(0.1);

    while(some_collision_occurred) {
      // we must use a while loop, imagine a very tight corner and
      // a fast-moving sphere which jumps past the tip of the corner.
      // it is invalid wrt several polys. by incrementally correcting
      // each poly individually accoring to its normal, we might push
      // it inside this poly but OUTSIDE another since the corner is so
      // tight. but by iterating we will push the sphere back, back,
      // back out of the corner until it finally collides with NONE
      // of the polys.

      some_collision_occurred = 0;

      n = sector->nonculled_polygon_nodes;
      while(n) {
        l3d_polygon_3d_collidable *poly3;

        poly3 = dynamic_cast<l3d_polygon_3d_collidable *>(n->polygon);
        if(poly3 &&
            !(dynamic_cast<l3d_polygon_3d_portal *>(n->polygon)))
        {
          // remember since obj->prevent_collision is true, the
          // following also corrects the collision

          //              printf("checking mesh at ");
          //              mesh->old_position.print();
          //              mesh->new_position.print();
          //              for(int iv=0; iv<poly3->clip_ivertices->num_items; iv++) {
          //                (**poly3->vlist)[(*poly3->clip_ivertices)[iv].ivertex].transformed.print();
          //              }
          while(obj->detect_collision_with(poly3)) {

            // reduce velocities in the direction of the bounce
            //              cam->VFW_velocity += dot(cam->bounce, cam->VFW);
            //              cam->VRI_velocity += dot(cam->bounce, cam->VRI);
            //              cam->VUP_velocity += dot(cam->bounce, cam->VUP);

            // the camera's position might have just changed:
            pos =
              obj->new_position =
                obj->new_position + obj->bounce * explosion_factor;
            some_collision_occurred = 1;
            explosion_factor = l3d_mulri(explosion_factor,2);
          }
        }

        n=n->next;
      }

      for(int iobj=0; iobj<sector->objects->num_items; iobj++) {
        if( (*sector->objects)[iobj] != target ) {
          l3d_object_clippable_boundable_collidable *obj2;
          obj2 = dynamic_cast<l3d_object_clippable_boundable_collidable *>
                 ( (*sector->objects)[iobj] );
          if(obj2) {
            if(obj->detect_collision_with(obj2)) {
              pos =
                obj->new_position =
                  obj->new_position + obj->bounce * explosion_factor;
              some_collision_occurred = 1;
              explosion_factor = l3d_mulri(explosion_factor,2);

            }
          }

        }
      }
    }


    // now the current position must be valid

    target->modeling_xform.a[0][3] = pos.a[0];
    target->modeling_xform.a[1][3] = pos.a[1];
    target->modeling_xform.a[2][3] = pos.a[2];
    target->modeling_xform.a[3][3] = pos.a[3];

    // now we detect collisions with portals, but we dont want to
    // correct these (otherwise we could never cross one!)
    obj->prevent_collision = 0;
    // displace camera 0.5 along VFW for portal crossing

    n = sector->nonculled_polygon_nodes;
    while(n) {
      l3d_polygon_3d_portal_collidable *portal;

      portal = dynamic_cast<l3d_polygon_3d_portal_collidable *>(n->polygon);
      if(portal) {
        if(obj->detect_collision_with(portal) == 1) {
          // must be 1 ==> moving collision, not static

          printf("CROSSING PORTAL INTO %p\n", portal->connected_sector);

          target->parent = portal->connected_sector;

          int idx;
          for(idx=0; idx<sector->objects->num_items; idx++) {
            if((*sector->objects)[idx] == target) {
              int idx2;
              for(idx2=idx; idx2<sector->objects->num_items-1; idx2++) {
                (*sector->objects)[idx2] = (*sector->objects)[idx2+1];
              }
              (*sector->objects)[idx2] = NULL;
              break;
            }
          }
          sector->objects->num_items--;

          sector = portal->connected_sector;
          sector->objects->next_index();
          assert(dynamic_cast<l3d_object_clippable_boundable *>(target));
          (*sector->objects)[sector->objects->num_items-1] =
            dynamic_cast<l3d_object_clippable_boundable *>(target);

          break;
        }
      }

      n=n->next;
    }

    //      target->viewing_xform.print();

    //      printf("VRI_tip ");
    //      target->VRI_tip.print();
    //      printf("VUP_tip ");
    //      target->VUP_tip.print();
    //      printf("VFW_tip ");
    //      target->VFW_tip.print();
    mesh->tick_count++;
    if(mesh->tick_count > mesh->max_ticks) {
      mesh->tick_count = 0;
      mesh->max_ticks = rand()%50 + 50;
      mesh->turning_velocity = rand()%5;
      mesh->moving_velocity = l3d_mulri(float_to_l3d_real(0.5),(rand()%5)+2);

      if(rand()%2) {
        mesh->movement_state = states_array[rand()%7];
        if(mesh->movement_state == NONE ||
           mesh->movement_state == ALONG_VFW ||
           mesh->movement_state == ALONG_VRI
           )
        {
          mesh->movement_state = ALONG_VUP;
        }
      } else {
        mesh->movement_state = ALONG_VUP;
      }
    }
  }

  void destructor(l3d_object *target) {
    delete (l3d_plugin_videoscape_mesh_seeker *) target->plugin_data;
  }

  void copy_data(l3d_object *target, l3d_object *copy_target) {
    l3d_plugin_videoscape_mesh_seeker *mesh;
    mesh = (l3d_plugin_videoscape_mesh_seeker *) target->plugin_data;

    l3d_plugin_environment *new_env;
    l3d_plugin_videoscape_mesh_seeker *new_mesh;

    new_env = mesh->plugin_env->clone();
    new_env->data = mesh->plugin_env->data; // a const character string of parms
    new_mesh = new l3d_plugin_videoscape_mesh_seeker(new_env);

    strcpy(new_mesh->mesh_fname, mesh->mesh_fname);
    strcpy(new_mesh->texture_fname, mesh->texture_fname);
    strcpy(new_mesh->texcoord_fname, mesh->texcoord_fname);

    l3d_texture_loader *l = new_mesh->plugin_env->texloader;
    l->load(new_mesh->texture_fname);
    new_mesh->tex_data = new l3d_texture_data;
    new_mesh->tex_data->width = l->width;
    new_mesh->tex_data->height = l->height;
    new_mesh->tex_data->data = l->data;

    //      printf("in vidmesh copy: made new texdata.data %p, orig was %p\n",
    //             new_mesh->tex_data->data,
    //             mesh->tex_data->data);
    for(int i=0; i<copy_target->polygons.num_items; i++) {
      l3d_polygon_3d_textured *pt;
      pt = dynamic_cast<l3d_polygon_3d_textured *>( copy_target->polygons[i]);
      if(pt) {
        pt->texture->tex_data = new_mesh->tex_data;
        pt->texture->owns_tex_data = false; // owned by the mesh, not the poly
      }
    }

    copy_target->plugin_data = (void *)new_mesh;
  }
}
