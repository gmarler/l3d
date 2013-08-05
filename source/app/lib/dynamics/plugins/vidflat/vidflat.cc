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
#include "../../../geom/polygon/p3_cflat.h"
#include "../../../dynamics/plugins/plugenv.h"
#include "../../../tool_2d/si_idx.h"
#include "../../../tool_2d/si_rgb.h"
#include <stdlib.h>
#include <string.h>
#include "vidflat.h"
#include "../../../tool_os/memman.h"

l3d_plugin_videoscape_flatshaded_mesh::l3d_plugin_videoscape_flatshaded_mesh
(l3d_plugin_environment *env)
{
  plugin_env = env;
}

l3d_plugin_videoscape_flatshaded_mesh::~l3d_plugin_videoscape_flatshaded_mesh(void) {
  delete plugin_env;
}

extern "C" {

  void constructor(l3d_object *target, void *data) {
    l3d_plugin_environment *env = (l3d_plugin_environment *)data;
    l3d_plugin_videoscape_flatshaded_mesh *mesh;

    target->plugin_data = mesh = new l3d_plugin_videoscape_flatshaded_mesh(env);

    char rest_parms[4096];
    strncpy(rest_parms, (char *)env->data, sizeof(rest_parms));
    char *tok;

    float posx,posy,posz;
    float xaxis_x, xaxis_y, xaxis_z,
    yaxis_x, yaxis_y, yaxis_z,
    zaxis_x, zaxis_y, zaxis_z;
    l3d_matrix position, orientation;
    strcpy(mesh->mesh_fname,"");
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

    FILE *fp;
    fp = fopen(mesh->mesh_fname, "rt");

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

        l3d_polygon_3d_flatshaded_clippable *p;
        int polygon_idx = target->polygons.next_index();
        target->polygons[polygon_idx] =
          p =
          new l3d_polygon_3d_flatshaded_clippable(numv);
        target->polygons[polygon_idx]->vlist = &target->vertices;

        //sbegin random_color
        int r,g,b;
    
        unsigned long col;
        l3d_screen_info_indexed *si_idx;
        l3d_screen_info_rgb *si_rgb;
        if (si_idx = dynamic_cast<l3d_screen_info_indexed *>( env->sinfo )) {
          col = rand() % si_idx->get_palette_size();
        } else if (si_rgb = dynamic_cast<l3d_screen_info_rgb *>(env->sinfo)) {
    
          r = rand() % ((si_rgb->red_mask) >> (si_rgb->red_shift));
          g = rand() % ((si_rgb->green_mask) >> (si_rgb->green_shift));
          b = rand() % ((si_rgb->blue_mask) >> (si_rgb->blue_shift));
    
          col =  r<<si_rgb->red_shift |
                 g<<si_rgb->green_shift |
                 b<<si_rgb->blue_shift;
        }
        p->final_color = col;
        //send random_color

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

        }

        // delete temp swap vertex
        target->polygons[polygon_idx]->ivertices->num_items =
          target->polygons[polygon_idx]->ivertices->num_items - 1;

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
  }

  void update(l3d_object *target) {
    l3d_plugin_videoscape_flatshaded_mesh *mesh;
    mesh = (l3d_plugin_videoscape_flatshaded_mesh *)target->plugin_data;

    // mesh specific update
    // mesh->xxx = yyy
    // also update target->modeling_xform if necessary
  }

  void destructor(l3d_object *target) {
    delete (l3d_plugin_videoscape_flatshaded_mesh *) target->plugin_data;
  }

  void copy_data(l3d_object *target, l3d_object *copy_target) {
    l3d_plugin_videoscape_flatshaded_mesh *mesh;
    mesh = (l3d_plugin_videoscape_flatshaded_mesh *) target->plugin_data;

    l3d_plugin_environment *new_env;
    l3d_plugin_videoscape_flatshaded_mesh *new_mesh;

    new_env = mesh->plugin_env->clone();
    new_env->data = mesh->plugin_env->data; // a const character string of parms
    new_mesh = new l3d_plugin_videoscape_flatshaded_mesh(new_env);

    strcpy(new_mesh->mesh_fname, mesh->mesh_fname);

    copy_target->plugin_data = (void *)new_mesh;
  }
}
