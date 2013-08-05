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
#include "../../../geom/object/object3d.h"
#include "../../../geom/texture/texload.h"
#include "../../../geom/polygon/p3_ltex.h"
#include "../../../dynamics/plugins/plugenv.h"
#include <stdlib.h>
#include <string.h>
#include "vidmesh.h"
#include "../../../tool_os/memman.h"

l3d_plugin_videoscape_mesh::l3d_plugin_videoscape_mesh
(l3d_plugin_environment *env)
{
  plugin_env = env;
  tex_data = NULL;
}

l3d_plugin_videoscape_mesh::~l3d_plugin_videoscape_mesh(void) {
  if(tex_data) delete tex_data;
  delete plugin_env;
}

extern "C" {

  void constructor(l3d_object *target, void *data) {
    l3d_plugin_environment *env = (l3d_plugin_environment *)data;
    l3d_plugin_videoscape_mesh *mesh;

    target->plugin_data = mesh = new l3d_plugin_videoscape_mesh(env);
    l3d_texture_loader *l = env->texloader;

    // mesh-specific initialization
    // mesh->xxx = xxx

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
    l3d_plugin_videoscape_mesh *mesh;
    mesh = (l3d_plugin_videoscape_mesh *)target->plugin_data;

    // mesh specific update
    // mesh->xxx = yyy
    // also update target->modeling_xform if necessary
  }

  void destructor(l3d_object *target) {
    delete (l3d_plugin_videoscape_mesh *) target->plugin_data;
  }

  void copy_data(l3d_object *target, l3d_object *copy_target) {
    l3d_plugin_videoscape_mesh *mesh;
    mesh = (l3d_plugin_videoscape_mesh *) target->plugin_data;

    l3d_plugin_environment *new_env;
    l3d_plugin_videoscape_mesh *new_mesh;

    new_env = mesh->plugin_env->clone();
    new_env->data = mesh->plugin_env->data; // a const character string of parms
    new_mesh = new l3d_plugin_videoscape_mesh(new_env);

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
