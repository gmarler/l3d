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

#include "w_pcoll.h"
#include "../object/ocoll.h"
#include "../polygon/p_portal.h"
#include <string.h>
#include "../polygon/p3_cltex.h"
#include "../polygon/p3_cflat.h"
#include "../polygon/p_cport.h"
#include "../../raster/ras_swl.h"
#include "../../view/camcol.h"
#include "../../dynamics/plugins/plugenv.h"
#include "../../system/fact0_4.h"
#include "../../tool_os/memman.h"

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>

void l3d_world_portal_textured_lightmapped_obj_colldet::update_all(void) {
  l3d_world_portal_textured_lightmapped_obj::update_all();
  l3d_sector *sector;
  sector = current_sector;
  l3d_polygon_3d_node *n;

  // initialize object
  sector->reset();
  // position all vertices of object into world space
  if (sector->num_xforms) {
    sector->transform(sector->modeling_xform);
  }

  // init polys for all objects contained in the sector
  l3d_polygon_3d_node *n_objects=0;
  for(int i=0; i<sector->objects->num_items; i++) {
    (*sector->objects)[i]->reset();

    // position object into world-space if necessary
    if((*sector->objects)[i]->num_xforms) {
      (*sector->objects)[i]->transform
      ((*sector->objects)[i]->modeling_xform);
    }

    if((*sector->objects)[i]->nonculled_polygon_nodes) {
      l3d_polygon_3d_node *n_temp;
      n_temp = (*sector->objects)[i]->nonculled_polygon_nodes;
      while(n_temp->next) {
        // set sector and object polys to be at same transform stage
        // (since objects may have been ADDITIONALLY transformed
        // by their own matrices yielding a higher transform stage
        // which would cause later transformations to be falsely skipped)
        n_temp->polygon->set_transform_stage(sector->transform_stage);
        n_temp=n_temp->next;
      }
      n_temp->next = n_objects;
      if(n_objects) { n_objects->prev = n_temp; } // dont forget the rev link!
      // reassign head of list
      n_objects = (*sector->objects)[i]->nonculled_polygon_nodes;
      n_objects->prev = NULL;
    }
  }

  // merge sector and all object poly lists
  if(n_objects) {
    l3d_polygon_3d_node *n_temp;
    n_temp = sector->nonculled_polygon_nodes;
    while(n_temp->next) {
      n_temp=n_temp->next;
    }
    n_temp->next = n_objects;
    n_objects->prev = n_temp;
  }


  int some_collision_occurred = 1;
  int do_sound = 0;

  l3d_camera_collidable *cam;
  cam = dynamic_cast<l3d_camera_collidable *>(camera);

  if(cam) {
    l3d_point old_position = cam->old_position;
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

    cam->prevent_collision = 1;

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
        if(poly3 && cam &&
            !(dynamic_cast<l3d_polygon_3d_portal *>(n->polygon)))
        {

          // cam->old_position is initialized for the first time
          // outside of this loop in camera_collidable::update

          //      while(cam->prevent_collision_with(& poly3->plane)) {

          // remember since cam->prevent_collision is true, the
          // following also corrects the collision

          while(cam->detect_collision_with(poly3)) {
            do_sound = 1;
            //              printf("collision with %p\n", poly3);

            // reduce velocities in the direction of the bounce
            const l3d_real damping_factor = float_to_l3d_real(0.0);
            cam->VFW_velocity += l3d_mulrr(dot(cam->bounce, cam->VFW),
                                           damping_factor);
            cam->VRI_velocity += l3d_mulrr(dot(cam->bounce, cam->VRI),
                                           damping_factor);
            cam->VUP_velocity += l3d_mulrr(dot(cam->bounce, cam->VUP),
                                           damping_factor);

            // the camera's position might have just changed:
            cam->VRP = cam->new_position;

            // DO NOT DO THE FOLLOWING LINE!!!
            //cam->old_position = cam->VRP;
            // we cannot update the old_position yet because old_position
            // is assumed to be valid (not colliding with anything).
            // but the newly calculated position is an INTERMEDIATE
            // position which might be invalid w.r.t. some polygons.
            // assigning it to old_value now might push it outside
            // of some polys it actually collides with, and after it
            // is pushed outside of a poly, collision is never done
            // with the backside of a poly (which is correct). we must
            // thus prevent being pushed outside of a poly, and do this
            // by not updating old_pos.


            some_collision_occurred = 1;
          }
        }

        n=n->next;
      }
    }

    // now the current position must be valid

    // now we detect collisions with portals, but we dont want to
    // correct these (otherwise we could never cross one!)
    cam->prevent_collision = 0;
    // displace camera 0.5 along VFW for portal crossing
    l3d_point new_pos_with_portal_displacement
    (cam->new_position.X_ + l3d_mulrr(float_to_l3d_real(5.0),
                                      cam->VFW.X_),

     cam->new_position.Y_ + l3d_mulrr(float_to_l3d_real(5.0),
                                      cam->VFW.Y_),

     cam->new_position.Z_ + l3d_mulrr(float_to_l3d_real(5.0),
                                      cam->VFW.Z_),
     int_to_l3d_real(1) ),

    original__new_pos = cam->new_position;


    n = sector->nonculled_polygon_nodes;
    while(n) {
      l3d_polygon_3d_portal_collidable *portal;

      portal = dynamic_cast<l3d_polygon_3d_portal_collidable *>(n->polygon);
      if(portal)
      {

        cam->new_position = new_pos_with_portal_displacement;

        if(cam->detect_collision_with(portal) == 1) {
          // must be 1 ==> moving collision, not static
          current_sector = portal->connected_sector;
          break;
        }
      }

      n=n->next;
    }

    cam->new_position = original__new_pos;


  } // if cam

  n = sector->nonculled_polygon_nodes;
  while(n) {
    l3d_polygon_3d_collidable *poly3;

    poly3 = dynamic_cast<l3d_polygon_3d_collidable *>(n->polygon);
    if(poly3 && cam) {

      // cam->old_position is initialized for the first time
      // outside of this loop in camera_collidable::update
      if (poly3->plane.side_of_point(cam->VRP) < 0) {
        //              printf("HEY! after coll we are on wrong side of plane %f %f %f %f\n",
        //                   poly3->plane.a,
        //                   poly3->plane.b,
        //                   poly3->plane.c,
        //                   poly3->plane.d);
        //        printf("VRP  %f %f %f %f\n",
        //           cam->VRP.X_,
        //           cam->VRP.Y_,
        //           cam->VRP.Z_,
        //           cam->VRP.W_
        //);
      }

    }

    n=n->next;
  }



  //  cam->old_position_already_saved = 0;

  if(do_sound) {
    char cmd[] = "play count=1 volume=64 sound=crash.au\n";
    sound_client->send_command(cmd);
  }
}

l3d_world_portal_textured_lightmapped_obj_colldet::
l3d_world_portal_textured_lightmapped_obj_colldet
(int xsize, int ysize)
    : l3d_world_portal_textured_lightmapped_obj (xsize, ysize)
{
  delete camera; //from base class constructor
  camera = new l3d_camera_collidable();

  // must reassign all variables which pointed to anything in the
  // camera since we just annihilated the old camera and replaced
  // it with a collidable camera
  rasterizer_3d_imp->fovx = &(camera->fovx);
  rasterizer_3d_imp->fovy = &(camera->fovy);


  sound_client = factory_manager_v_0_4.sound_client_factory->create();
  sound_client->connect("127.0.0.1 5556"); //FIXME: parameterize

  //    // define 3d polygon for 3d clip window
  //      vlist = new l3d_two_part_list<l3d_coordinate>(1);


  //      clipwin->vlist = &vlist;

}

l3d_world_portal_textured_lightmapped_obj_colldet::
~l3d_world_portal_textured_lightmapped_obj_colldet(void)
{
  delete sound_client;
}

//  void l3d_world_portal_textured_lightmapped_obj_colldet::
//  load_from_file(const char *fname) {
//    FILE *fp;

//    int linesize=1536;
//    char line[1536];

//    int tex_count, snum;

//    fp = fopen("world.dat", "rb");

//    fgets(line,sizeof(line),fp);
//    sscanf(line,"%d", &tex_count);

//    for(int i=0; i<tex_count; i++) {
//      fgets(line,sizeof(line),fp);
//      while(line[strlen(line)-1] == '\n') { line[strlen(line)-1] = '\0'; }

//      texture_loader->load(line);
//      int next_tex_index;
//      next_tex_index = tex_data.next_index();
//      tex_data[next_tex_index].width = texture_loader->width;
//      tex_data[next_tex_index].height = texture_loader->height;
//      tex_data[next_tex_index].data = texture_loader->data;
//    }

//    screen->refresh_palette();

//    fgets(line,linesize,fp);
//    sscanf(line,"%d",&snum);

//    l3d_sector *current_sector;

//    for(int onum=0; onum<snum; onum++) {
//      char sname[80];
//      char keyword[80];
//      char plugin_name[1024];
//      char rest_parms[4096];
//      int numv, nump;
//      fgets(line, linesize, fp);
//      strcpy(keyword,"");
//      strcpy(sname,"");
//      strcpy(plugin_name,"");
//      strcpy(rest_parms,"");
//      sscanf(line,"%s %s %s\n",keyword, sname, plugin_name);

//      //- get rest parameters all in one string
//      char *tok;
//      tok=strtok(line, " \n\r\t"); //- skip keyword
//      tok=strtok(NULL, " \n\r\t"); //- skip object name
//      tok=strtok(NULL, " \n\r\t"); //- skip plugin name
//      tok=strtok(NULL, "\n\r\t"); //- rest of the line until newline
//      if(tok) { strncpy(rest_parms, tok, sizeof(rest_parms)); }


//      if(strcmp(keyword,"SECTOR")==0) {

//        printf("reading sector %s\n",sname);

//        fgets(line,linesize,fp);
//        sscanf(line, "%d %d", &numv, &nump);

//        l3d_sector *sector;
//        int new_onum;
//        objects[new_onum=objects.next_index()] = sector = new l3d_sector(numv);
//        current_sector = sector;

//        strcpy(objects[new_onum]->name, sname);

//        for(int v=0; v<numv; v++) {
//          int vn;
//          float vx, vy, vz;

//          fgets(line, linesize, fp);
//          sscanf(line, "%d %f %f %f", &vn, &vx, &vy, &vz);
//          (*(objects[new_onum]->vertices))[vn].original.
//            set(float_to_l3d_real(vx),
//                float_to_l3d_real(vy),
//                float_to_l3d_real(vz),
//                int_to_l3d_real(1));
//        }

//        for(int p=0; p<nump; p++) {
//          int p_idx = objects[new_onum]->polygons.next_index();

//          char *s;
//          fgets(line, linesize, fp);
//          s = strtok(line, " ");
//          printf("new line: %s\n",line);
//          if(strcmp(s,"GEOMPOLY")==0) {
//            printf("geompoly\n");
//            int num_vtx;
//            l3d_polygon_3d_textured_lightmapped_collidable *pt;

//            s = strtok(NULL, " ");
//            sscanf(s, "%d", &num_vtx);
//            objects[new_onum]->polygons[p_idx] = pt =
//              new l3d_polygon_3d_textured_lightmapped_collidable(num_vtx,scache);
//            objects[new_onum]->polygons[p_idx]->vlist =
//              &objects[new_onum]->vertices;

//            for(int vtx=0; vtx<num_vtx; vtx++) {
//              s = strtok(NULL, " ");
//              int iv;
//              sscanf(s, "%d", &iv);
//              (*(objects[new_onum]->polygons[p_idx]->ivertices))
//                [objects[new_onum]->polygons[p_idx]->ivertices->next_index()].
//                ivertex = iv;
//            }

//            pt->fovx = &(camera->fovx);
//            pt->fovy = &(camera->fovy);
//            pt->screen_xsize = &(screen->xsize);
//            pt->screen_ysize = &(screen->ysize);

//            int tex_id = 1;
//            pt->texture = new l3d_texture;
//            pt->texture->tex_data = & tex_data[tex_id];
//            pt->texture->owns_tex_data = false;

//            pt->texture->O.original.set
//              (

//               (**pt->vlist)[ (*pt->ivertices)[0].ivertex ].original.X_,
//               (**pt->vlist)[ (*pt->ivertices)[0].ivertex ].original.Y_,
//               (**pt->vlist)[ (*pt->ivertices)[0].ivertex ].original.Z_,
//               int_to_l3d_real(1) );
//            pt->texture->U.original.set
//              (
//               (**pt->vlist)[ (*pt->ivertices)[1].ivertex ].original.X_,
//               (**pt->vlist)[ (*pt->ivertices)[1].ivertex ].original.Y_,
//               (**pt->vlist)[ (*pt->ivertices)[1].ivertex ].original.Z_,
//               int_to_l3d_real(1) );

//            l3d_point V_point(
//                              (**pt->vlist)[ (*pt->ivertices)[pt->ivertices->num_items-1].ivertex ].original.X_,
//                              (**pt->vlist)[ (*pt->ivertices)[pt->ivertices->num_items-1].ivertex ].original.Y_,
//                              (**pt->vlist)[ (*pt->ivertices)[pt->ivertices->num_items-1].ivertex ].original.Z_,
//                              int_to_l3d_real(1) );
//            l3d_vector
//              U_vector = pt->texture->U.original - pt->texture->O.original,
//              V_vector = V_point - pt->texture->O.original,
//              U_cross_V_vector = cross(U_vector, V_vector),
//              V_vector_new = normalized(cross(U_cross_V_vector, U_vector)) *
//              sqrt(dot(U_vector,U_vector));

//            pt->texture->V.original = V_vector_new + pt->texture->O.original;

//            objects[new_onum]->polygons[p_idx]->compute_center();
//            objects[new_onum]->polygons[p_idx]->compute_sfcnormal();

//            // although we don't have the surface yet (we only create it when
//            // it is needed), we can compute its size and orientation beforehand.
//            // in fact we must do so, otherwise if we compute the size and
//            // orientation only when it is being drawn, the coords are already
//            // transformed and we would compute an OUV in camera space,
//            // and to transform this back into object space (so that we can
//            // reuse instead of recompute the OUV) we would have to apply
//            // the reverse camera transformation, meaning we would always
//            // need to keep track of the current transformation matrix so that
//            // "latecomers" to the pipeline could also be transformed. by
//            // computing the OUV ahead of time in object coords then we just
//            // use them and transform them as normal.

//            pt->compute_surface_orientation_and_size(); // also assigns u/v's

//            pt->plane.align_with_point_normal(pt->center.original, normalized(pt->sfcnormal.original - pt->center.original));

//          } else if(strcmp(s,"GEOMPOLY_TEX")==0) {

//            printf("geompoly_tex\n");
//            int num_vtx;
//            l3d_polygon_3d_textured_lightmapped_collidable *pt;

//            s = strtok(NULL, " ");
//            sscanf(s, "%d", &num_vtx);
//            objects[new_onum]->polygons[p_idx] = pt = new l3d_polygon_3d_textured_lightmapped_collidable(num_vtx, scache);
//            objects[new_onum]->polygons[p_idx]->vlist = &objects[new_onum]->vertices;

//            for(int vtx=0; vtx<num_vtx; vtx++) {
//              s = strtok(NULL, " ");
//              int iv;
//              sscanf(s, "%d", &iv);
//              (*(objects[new_onum]->polygons[p_idx]->ivertices))[
//                                                                 objects[new_onum]->polygons[p_idx]->ivertices->next_index()].ivertex = iv;
//            }
//            s = strtok(NULL, " ");


//            pt->fovx = &(camera->fovx);
//            pt->fovy = &(camera->fovy);
//            pt->screen_xsize = &(screen->xsize);
//            pt->screen_ysize = &(screen->ysize);


//            int tex_id;
//            sscanf(s, "%d", &tex_id);
//            pt->texture = new l3d_texture;
//            pt->texture->tex_data = & tex_data[tex_id];
//            pt->texture->owns_tex_data = false;
//            float x,y,z;
//            s = strtok(NULL, " ");
//            sscanf(s,"%f", &x);
//            s = strtok(NULL, " ");
//            sscanf(s,"%f", &y);
//            s = strtok(NULL, " ");
//            sscanf(s,"%f", &z);

//            pt->texture->O.original.set(float_to_l3d_real(x),
//                                        float_to_l3d_real(y),
//                                        float_to_l3d_real(z),
//                                        int_to_l3d_real(1));
//            printf("found O %f %f %f\n", x,y,z);
//            s = strtok(NULL, " ");
//            printf("%s\n",s);
//            sscanf(s,"%f", &x);
//            s = strtok(NULL, " ");
//            printf("%s\n",s);
//            sscanf(s,"%f", &y);
//            s = strtok(NULL, " ");
//            printf("%s\n",s);
//            sscanf(s,"%f", &z);
//            printf("found U %f %f %f\n", x,y,z);
//            pt->texture->U.original.set(float_to_l3d_real(x),
//                                        float_to_l3d_real(y),
//                                        float_to_l3d_real(z),
//                                        int_to_l3d_real(1));
//            s = strtok(NULL, " ");
//            printf("%s\n",s);
//            sscanf(s,"%f", &x);
//            s = strtok(NULL, " ");
//            printf("%s\n",s);
//            sscanf(s,"%f", &y);
//            s = strtok(NULL, " \n");
//            printf("%s\n",s);
//            sscanf(s,"%f", &z);
//            printf("found V %f %f %f\n", x,y,z);
//            pt->texture->V.original.set(float_to_l3d_real(x),
//                                        float_to_l3d_real(y),
//                                        float_to_l3d_real(z),
//                                        int_to_l3d_real(1));

//            objects[new_onum]->polygons[p_idx]->compute_center();
//            objects[new_onum]->polygons[p_idx]->compute_sfcnormal();



//            // although we don't have the surface yet (we only create it when
//            // it is needed), we can compute its size and orientation beforehand.
//            // in fact we must do so, otherwise if we compute the size and
//            // orientation only when it is being drawn, the coords are already
//            // transformed and we would compute an OUV in camera space,
//            // and to transform this back into object space (so that we can
//            // reuse instead of recompute the OUV) we would have to apply
//            // the reverse camera transformation, meaning we would always
//            // need to keep track of the current transformation matrix so that
//            // "latecomers" to the pipeline could also be transformed. by
//            // computing the OUV ahead of time in object coords then we just
//            // use them and transform them as normal.

//            pt->compute_surface_orientation_and_size();

//            pt->plane.align_with_point_normal(pt->center.original, normalized(pt->sfcnormal.original - pt->center.original));


//            printf("done geomtex\n");
//          } else {
//            printf("portal\n");
//            int num_vtx;

//            s = strtok(NULL, " ");
//            sscanf(s, "%d", &num_vtx);
//            objects[new_onum]->polygons[p_idx] = new l3d_polygon_3d_portal_collidable(num_vtx);
//            objects[new_onum]->polygons[p_idx]->vlist = &objects[new_onum]->vertices;
//            for(int vtx=0; vtx<num_vtx; vtx++) {
//              s = strtok(NULL, " ");
//              int iv;
//              sscanf(s, "%d", &iv);
//              (*(objects[new_onum]->polygons[p_idx]->ivertices))[
//                                                                 objects[new_onum]->polygons[p_idx]->ivertices->next_index()].ivertex = iv;
//            }

//            objects[new_onum]->polygons[p_idx]->compute_center();
//            objects[new_onum]->polygons[p_idx]->compute_sfcnormal();

//            s = strtok(NULL," \r\n");
//            char *s2 = s + strlen(s)-1;
//            while(*s2=='\r' || *s2=='\n') { *s2=0; s2--; }
//            printf("%d\n", *s2);
//            l3d_polygon_3d_portal *pp; // no need to be collidable since we only look at the portal part
//            pp = dynamic_cast<l3d_polygon_3d_portal *>(objects[new_onum]->polygons[p_idx]);
//            if(pp) { strcpy(pp->connected_sector_name, s); }
//          }

//        }
//      } else if(strcmp(keyword,"ACTOR")==0) {
//        int new_onum;
//        objects[new_onum = objects.next_index()]
//          = (*current_sector->objects)[current_sector->objects->next_index()]
//          = new l3d_object_clippable_boundable(100);
//        // will be deleted and recreated by by object
//        // (max fixed vtx count)

//        strcpy(objects[new_onum]->name, sname);
//        strcpy(objects[new_onum]->plugin_name, plugin_name);
//        objects[new_onum]->parent = current_sector;

//        if(strlen(plugin_name)) {
//          objects[new_onum]->plugin_loader =
//            factory_manager_v_0_2.plugin_loader_factory->create();
//          objects[new_onum]->plugin_loader->load(plugin_name);
//          objects[new_onum]->plugin_constructor =
//            (void (*)(l3d_object *, void *))
//            objects[new_onum]->plugin_loader->find_symbol("constructor");
//          objects[new_onum]->plugin_update =
//            (void (*)(l3d_object *))
//            objects[new_onum]->plugin_loader->find_symbol("update");
//          objects[new_onum]->plugin_destructor =
//            (void (*)(l3d_object *))
//            objects[new_onum]->plugin_loader->find_symbol("destructor");
//          objects[new_onum]->plugin_copy_data =
//            (void (*)(l3d_object *, l3d_object *))
//            objects[new_onum]->plugin_loader->find_symbol("copy_data");

//          l3d_plugin_environment *e = new l3d_plugin_environment
//            (texture_loader, screen->sinfo, scache, rest_parms);

//          if(objects[new_onum]->plugin_constructor) {
//            (*objects[new_onum]->plugin_constructor) (objects[new_onum],e);
//          }
//        }
//      }
//    }

//    for(int ii=0; ii<objects.num_items; ii++) {
//      for(int pi=0; pi<objects[ii]->polygons.num_items; pi++) {
//        l3d_polygon_3d_portal *p; // no need to be collidable since we only look at the portal part
//        if ((p = dynamic_cast<l3d_polygon_3d_portal *>(objects[ii]->polygons[pi]))) {
//          p->connected_sector = sector_by_name(p->connected_sector_name);
//        }
//      }
//    }

//  }

void l3d_world_portal_textured_lightmapped_obj_colldet::
load_from_file(const char *fname)
{
  FILE *fp;

  int linesize=1536;
  char line[1536];
  int tex_count, snum;

  fp = fopen("world.dat", "rb");

  fgets(line,sizeof(line),fp);
  sscanf(line,"%d", &tex_count);

  for(int i=0; i<tex_count; i++) {
    fgets(line,sizeof(line),fp);
    while(line[strlen(line)-1] == '\n') { line[strlen(line)-1] = '\0'; }

    texture_loader->load(line);
    int next_tex_index;
    next_tex_index = tex_data.next_index();
    tex_data[next_tex_index].width = texture_loader->width;
    tex_data[next_tex_index].height = texture_loader->height;
    tex_data[next_tex_index].data = texture_loader->data;
  }

  screen->refresh_palette();


  fgets(line,linesize,fp);
  sscanf(line,"%d",&snum);

  l3d_sector *current_sector=NULL;

  for(int onum=0; onum<snum; onum++) {
    char sname[80];
    char keyword[80];
    char plugin_name[1024];
    char rest_parms[4096];
    int numv, nump;
    fgets(line, linesize, fp);
    strcpy(keyword,"");
    strcpy(sname,"");
    strcpy(plugin_name,"");
    strcpy(rest_parms,"");
    sscanf(line,"%s %s %s\n",keyword, sname, plugin_name);

    //- get rest parameters all in one string
    char *tok;
    tok=strtok(line, " \n\r\t"); //- skip keyword
    tok=strtok(NULL, " \n\r\t"); //- skip object name
    tok=strtok(NULL, " \n\r\t"); //- skip plugin name
    tok=strtok(NULL, "\n\r\t"); //- rest of the line until newline
    if(tok) { strncpy(rest_parms, tok, sizeof(rest_parms)); }

    if(strcmp(keyword,"SECTOR")==0) {

      //printf("reading sector %s\n",sname);

      fgets(line,linesize,fp);
      sscanf(line, "%d %d", &numv, &nump);

      l3d_sector *sector;
      int new_onum;
      objects[new_onum=objects.next_index()] = sector = new l3d_sector(numv);
      current_sector = sector;

      strcpy(objects[new_onum]->name, sname);

      for(int v=0; v<numv; v++) {
        int vn;
        float vx, vy, vz;

        fgets(line, linesize, fp);
        sscanf(line, "%d %f %f %f", &vn, &vx, &vy, &vz);
        (*(objects[new_onum]->vertices))[vn].original.set(float_to_l3d_real(vx),
            float_to_l3d_real(vy),
            float_to_l3d_real(vz),
            int_to_l3d_real(1));
      }

      for(int p=0; p<nump; p++) {
        int p_idx = objects[new_onum]->polygons.next_index();

        char *s;
        fgets(line, linesize, fp);
        s = strtok(line, " ");
        //printf("new line: %s\n",line);
        if(strcmp(s,"GEOMPOLY")==0) {
          //printf("geompoly\n");
          int num_vtx;
          l3d_polygon_3d_textured_lightmapped_collidable *pt;

          s = strtok(NULL, " ");
          sscanf(s, "%d", &num_vtx);
          objects[new_onum]->polygons[p_idx] =
            pt =
            new l3d_polygon_3d_textured_lightmapped_collidable(num_vtx,scache);
          objects[new_onum]->polygons[p_idx]->vlist =
            &objects[new_onum]->vertices;
          for(int vtx=0; vtx<num_vtx; vtx++) {
            s = strtok(NULL, " ");
            int iv;
            sscanf(s, "%d", &iv);
            (*(objects[new_onum]->polygons[p_idx]->ivertices))[
              objects[new_onum]->polygons[p_idx]->ivertices->next_index()].ivertex = iv;

          }

          pt->fovx = &(camera->fovx);
          pt->fovy = &(camera->fovy);
          pt->screen_xsize = &(screen->xsize);
          pt->screen_ysize = &(screen->ysize);

          int tex_id = 0;
          pt->texture = new l3d_texture;
          pt->texture->tex_data = & tex_data[tex_id];
          pt->texture->owns_tex_data = false;

          pt->texture->O.original.set
          (

            (**pt->vlist)[ (*pt->ivertices)[0].ivertex ].original.X_,
            (**pt->vlist)[ (*pt->ivertices)[0].ivertex ].original.Y_,
            (**pt->vlist)[ (*pt->ivertices)[0].ivertex ].original.Z_,
            int_to_l3d_real(1) );
          pt->texture->U.original.set
          (
            (**pt->vlist)[ (*pt->ivertices)[1].ivertex ].original.X_,
            (**pt->vlist)[ (*pt->ivertices)[1].ivertex ].original.Y_,
            (**pt->vlist)[ (*pt->ivertices)[1].ivertex ].original.Z_,
            int_to_l3d_real(1) );

          l3d_point V_point(
            (**pt->vlist)[ (*pt->ivertices)[pt->ivertices->num_items-1].ivertex ].original.X_,
            (**pt->vlist)[ (*pt->ivertices)[pt->ivertices->num_items-1].ivertex ].original.Y_,
            (**pt->vlist)[ (*pt->ivertices)[pt->ivertices->num_items-1].ivertex ].original.Z_,
            int_to_l3d_real(1) );
          l3d_vector
          U_vector = pt->texture->U.original - pt->texture->O.original,
                     V_vector = V_point - pt->texture->O.original,
                                U_cross_V_vector = cross(U_vector, V_vector),
                                                   V_vector_new = normalized(cross(U_cross_V_vector, U_vector)) *
                                                                  sqrt(dot(U_vector,U_vector));

          pt->texture->V.original = V_vector_new + pt->texture->O.original;

          objects[new_onum]->polygons[p_idx]->compute_center();
          objects[new_onum]->polygons[p_idx]->compute_sfcnormal();

          // although we don't have the surface yet (we only create it when
          // it is needed), we can compute its size and orientation beforehand.
          // in fact we must do so, otherwise if we compute the size and
          // orientation only when it is being drawn, the coords are already
          // transformed and we would compute an OUV in camera space,
          // and to transform this back into object space (so that we can
          // reuse instead of recompute the OUV) we would have to apply
          // the reverse camera transformation, meaning we would always
          // need to keep track of the current transformation matrix so that
          // "latecomers" to the pipeline could also be transformed. by
          // computing the OUV ahead of time in object coords then we just
          // use them and transform them as normal.

          pt->compute_surface_orientation_and_size(); // also assigns u/v's

          pt->plane.align_with_point_normal(pt->center.original, normalized(pt->sfcnormal.original - pt->center.original));

        } else if(strcmp(s,"GEOMPOLY_TEX")==0) {

          //printf("geompoly_tex\n");
          int num_vtx;
          l3d_polygon_3d_textured_lightmapped_collidable *pt;

          s = strtok(NULL, " ");
          sscanf(s, "%d", &num_vtx);
          objects[new_onum]->polygons[p_idx] = 
            pt = 
            new l3d_polygon_3d_textured_lightmapped_collidable
            (num_vtx, scache);
          objects[new_onum]->polygons[p_idx]->vlist = &objects[new_onum]->vertices;

          for(int vtx=0; vtx<num_vtx; vtx++) {
            s = strtok(NULL, " ");
            int iv;
            sscanf(s, "%d", &iv);
            (*(objects[new_onum]->polygons[p_idx]->ivertices))[
              objects[new_onum]->polygons[p_idx]->ivertices->next_index()].ivertex = iv;
          }
          s = strtok(NULL, " ");


          pt->fovx = &(camera->fovx);
          pt->fovy = &(camera->fovy);
          pt->screen_xsize = &(screen->xsize);
          pt->screen_ysize = &(screen->ysize);


          int tex_id;
          sscanf(s, "%d", &tex_id);
          pt->texture = new l3d_texture;
          pt->texture->tex_data = & tex_data[tex_id];
          pt->texture->owns_tex_data = false;
          float x,y,z;
          s = strtok(NULL, " ");
          sscanf(s,"%f", &x);
          s = strtok(NULL, " ");
          sscanf(s,"%f", &y);
          s = strtok(NULL, " ");
          sscanf(s,"%f", &z);

          pt->texture->O.original.set(float_to_l3d_real(x),
                                      float_to_l3d_real(y),
                                      float_to_l3d_real(z),
                                      int_to_l3d_real(1));
          //printf("found O %f %f %f\n", x,y,z);
          s = strtok(NULL, " ");
          //printf("%s\n",s);
          sscanf(s,"%f", &x);
          s = strtok(NULL, " ");
          //printf("%s\n",s);
          sscanf(s,"%f", &y);
          s = strtok(NULL, " ");
          //printf("%s\n",s);
          sscanf(s,"%f", &z);
          //printf("found U %f %f %f\n", x,y,z);
          pt->texture->U.original.set(float_to_l3d_real(x),
                                      float_to_l3d_real(y),
                                      float_to_l3d_real(z),
                                      int_to_l3d_real(1));
          s = strtok(NULL, " ");
          //printf("%s\n",s);
          sscanf(s,"%f", &x);
          s = strtok(NULL, " ");
          //printf("%s\n",s);
          sscanf(s,"%f", &y);
          s = strtok(NULL, " \n");
          //printf("%s\n",s);
          sscanf(s,"%f", &z);
          //printf("found V %f %f %f\n", x,y,z);
          pt->texture->V.original.set(float_to_l3d_real(x),
                                      float_to_l3d_real(y),
                                      float_to_l3d_real(z),
                                      int_to_l3d_real(1));

          objects[new_onum]->polygons[p_idx]->compute_center();
          objects[new_onum]->polygons[p_idx]->compute_sfcnormal();



          // although we don't have the surface yet (we only create it when
          // it is needed), we can compute its size and orientation beforehand.
          // in fact we must do so, otherwise if we compute the size and
          // orientation only when it is being drawn, the coords are already
          // transformed and we would compute an OUV in camera space,
          // and to transform this back into object space (so that we can
          // reuse instead of recompute the OUV) we would have to apply
          // the reverse camera transformation, meaning we would always
          // need to keep track of the current transformation matrix so that
          // "latecomers" to the pipeline could also be transformed. by
          // computing the OUV ahead of time in object coords then we just
          // use them and transform them as normal.

          pt->compute_surface_orientation_and_size();

          pt->plane.align_with_point_normal(pt->center.original, normalized(pt->sfcnormal.original - pt->center.original));


          //printf("done geomtex\n");
        } else {
          //printf("portal\n");
          int num_vtx;

          s = strtok(NULL, " ");
          sscanf(s, "%d", &num_vtx);
          objects[new_onum]->polygons[p_idx] = new l3d_polygon_3d_portal_collidable(num_vtx);
          objects[new_onum]->polygons[p_idx]->vlist = &objects[new_onum]->vertices;
          for(int vtx=0; vtx<num_vtx; vtx++) {
            s = strtok(NULL, " ");
            int iv;
            sscanf(s, "%d", &iv);
            (*(objects[new_onum]->polygons[p_idx]->ivertices))[
              objects[new_onum]->polygons[p_idx]->ivertices->next_index()].ivertex = iv;
          }

          objects[new_onum]->polygons[p_idx]->compute_center();
          objects[new_onum]->polygons[p_idx]->compute_sfcnormal();

          s = strtok(NULL," \r\n");
          char *s2 = s + strlen(s)-1;
          while(*s2=='\r' || *s2=='\n') { *s2=0; s2--; }
          //printf("%d\n", *s2);
          l3d_polygon_3d_portal *pp;
          pp = dynamic_cast<l3d_polygon_3d_portal *>(objects[new_onum]->polygons[p_idx]);
          if(pp) { strcpy(pp->connected_sector_name, s); }
        }

      }
    } else if(strcmp(keyword,"ACTOR")==0) {
      int new_onum;
      assert(current_sector);
      objects[new_onum = objects.next_index()]
      = (*current_sector->objects)[current_sector->objects->next_index()]
        = new l3d_object_clippable_boundable_collidable(100);
      // will be deleted and recreated by by object
      // (max fixed vtx count)

      strcpy(objects[new_onum]->name, sname);
      strcpy(objects[new_onum]->plugin_name, plugin_name);
      objects[new_onum]->parent = current_sector;

      if(strlen(plugin_name)) {
        objects[new_onum]->plugin_loader =
          factory_manager_v_0_4.plugin_loader_factory->create();
        objects[new_onum]->plugin_loader->load(plugin_name);
        objects[new_onum]->plugin_constructor =
          (void (*)(l3d_object *, void *))
          objects[new_onum]->plugin_loader->find_symbol("constructor");
        objects[new_onum]->plugin_update =
          (void (*)(l3d_object *))
          objects[new_onum]->plugin_loader->find_symbol("update");
        objects[new_onum]->plugin_destructor =
          (void (*)(l3d_object *))
          objects[new_onum]->plugin_loader->find_symbol("destructor");
        objects[new_onum]->plugin_copy_data =
          (void (*)(const l3d_object *, l3d_object *))
          objects[new_onum]->plugin_loader->find_symbol("copy_data");

        l3d_plugin_environment *e = new l3d_plugin_environment
                                    (texture_loader, screen->sinfo, scache, rest_parms);

        if(objects[new_onum]->plugin_constructor) {
          (*objects[new_onum]->plugin_constructor) (objects[new_onum],e);
        }
      }
    } else if(strcmp(keyword,"CAMERA")==0) {

      float posx,posy,posz;
      float xaxis_x, xaxis_y, xaxis_z,
      yaxis_x, yaxis_y, yaxis_z,
      zaxis_x, zaxis_y, zaxis_z;
      char *tok;

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

      assert(current_sector);
      this->current_sector = current_sector;
      //      printf("camera assigned to %s\n", current_sector->name);
      camera->VRP.set(float_to_l3d_real(posx),
                      float_to_l3d_real(posy),
                      float_to_l3d_real(posz),
                      int_to_l3d_real(1));
      camera->calculate_viewing_xform();
    }
  }

  for(int ii=0; ii<objects.num_items; ii++) {
    for(int pi=0; pi<objects[ii]->polygons.num_items; pi++) {
      l3d_polygon_3d_portal *p;
      if ((p = dynamic_cast<l3d_polygon_3d_portal *>(objects[ii]->polygons[pi]))) {
        p->connected_sector = sector_by_name(p->connected_sector_name);
      }
    }
  }

}

