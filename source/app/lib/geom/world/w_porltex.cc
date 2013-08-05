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

#include "w_porltex.h"
#include "../object/object3d.h"
#include "../polygon/p_portal.h"
#include <string.h>
#include "../polygon/p3_ltex.h"
#include "../polygon/p3_cflat.h"
#include "../polygon/p_cport.h"
#include "../../raster/ras_swl.h"
#include "../../view/camcol.h"
#include "../../geom/frustum/vfrust.h"
#include "../../tool_os/memman.h"

int l3d_world_portal_textured_lightmapped::compute_light_level
(l3d_real u, l3d_real v)
{
  l3d_real x,y,z;

  // from tex space u,v coords, convert to world space x,y,z

  x = O.X_ + l3d_mulri(U.X_, u) + l3d_mulri(V.X_, v);
  y = O.Y_ + l3d_mulri(U.Y_, u) + l3d_mulri(V.Y_, v);
  z = O.Z_ + l3d_mulri(U.Z_, u) + l3d_mulri(V.Z_, v);

  // x,y,z are now the world-space coordinates of the lumel under
  // consideration

  l3d_point aLight(0,0,0,1); // light at current camera position = origin
  // (since we only rasterize sublightmaps when we have placed the light
  // by interactive positioning via key 'P')

  // we define 2 vectors: L, the vector from the facet (more precisely from
  // the facet's center) to the light source, and N, the facet's normal
  // vector. Note that vector FROM facet center TO light source is
  // computed as light_source - facet_center and not vice versa.
  // also note that the surface normal vector is defined as starting at the
  // (transformed) facet center and not at the origin. (If we defined N
  // just as aFacet.xfrm_sfcnormal.X_, Y_, Z_), this would implicitly
  // assume that N starts at the origin, which is not the case.)

  l3d_vector L(aLight.X_ - x ,
               aLight.Y_ - y ,
               aLight.Z_ - z ,
               int_to_l3d_real(0)),

  N(cross(U,V)); // correct if portalizer returns correct UVs

  //N(cross(V,U)); // U,V not V,U is correct but as long as the UV
  // are mirrored we correct for this here FIXME

  l3d_real intensity;

  // calculate f_att attenuation factor to reduce sharp fall-off of
  // light intensity based on distance; this makes light more gradually
  // dim as objects are further from the light source. (Foley/van Dam/
  // Feiner/Hughes, Equation 16.8)

  l3d_real f_att=l3d_divrr(int_to_l3d_real(1) ,
                           (float_to_l3d_real(0.1)+float_to_l3d_real(0.02*sqrt(l3d_real_to_float(dot(L,L))))));
  if (f_att>int_to_l3d_real(1)) f_att=int_to_l3d_real(1);

  // calculate intensity value (0-15) for facet
  intensity = l3d_mulrr ( l3d_mulrr(int_to_l3d_real(MAX_LIGHT_LEVELS/2) , f_att) ,
                          dot(normalized(N),normalized(L)) );
  if (intensity>int_to_l3d_real(MAX_LIGHT_LEVELS/2)) { intensity = int_to_l3d_real(MAX_LIGHT_LEVELS/2); }

  if (intensity<int_to_l3d_real(0))  { intensity = int_to_l3d_real(0);  }

  // apply intensity value (brightness) to existing facet color
  // we store both the intensity and colored intensity to save computation
  // time later
  return l3d_real_to_int(255);
  return l3d_real_to_int(intensity);
}


l3d_world_portal_textured_lightmapped::l3d_world_portal_textured_lightmapped
(int xsize, int ysize)
    : l3d_world_portal_textured (xsize, ysize)
{
  scache = new l3d_surface_cache(screen->sinfo);

}

l3d_world_portal_textured_lightmapped::
~l3d_world_portal_textured_lightmapped(void)
{
  delete scache;
}

void l3d_world_portal_textured_lightmapped::load_from_file(const char *fname) {
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

      printf("reading sector %s\n",sname);

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
        printf("new line: %s\n",line);
        if(strcmp(s,"GEOMPOLY")==0) {
          printf("geompoly\n");
          int num_vtx;
          l3d_polygon_3d_textured_lightmapped *pt;

          s = strtok(NULL, " ");
          sscanf(s, "%d", &num_vtx);
          objects[new_onum]->polygons[p_idx] = pt =
                                                 new l3d_polygon_3d_textured_lightmapped(num_vtx,scache);
          objects[new_onum]->polygons[p_idx]->vlist = &objects[new_onum]->vertices;

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

          pt->compute_surface_orientation_and_size(); // also computes uv's for ivertices

          pt->plane.align_with_point_normal(pt->center.original, normalized(pt->sfcnormal.original - pt->center.original));

        } else if(strcmp(s,"GEOMPOLY_TEX")==0) {

          printf("geompoly_tex\n");
          int num_vtx;
          l3d_polygon_3d_textured_lightmapped *pt;

          s = strtok(NULL, " ");
          sscanf(s, "%d", &num_vtx);
          objects[new_onum]->polygons[p_idx] = pt = new l3d_polygon_3d_textured_lightmapped(num_vtx, scache);
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
          printf("found O %f %f %f\n", x,y,z);
          s = strtok(NULL, " ");
          printf("%s\n",s);
          sscanf(s,"%f", &x);
          s = strtok(NULL, " ");
          printf("%s\n",s);
          sscanf(s,"%f", &y);
          s = strtok(NULL, " ");
          printf("%s\n",s);
          sscanf(s,"%f", &z);
          printf("found U %f %f %f\n", x,y,z);
          pt->texture->U.original.set(float_to_l3d_real(x),
                                      float_to_l3d_real(y),
                                      float_to_l3d_real(z),
                                      int_to_l3d_real(1));
          s = strtok(NULL, " ");
          printf("%s\n",s);
          sscanf(s,"%f", &x);
          s = strtok(NULL, " ");
          printf("%s\n",s);
          sscanf(s,"%f", &y);
          s = strtok(NULL, " \n");
          printf("%s\n",s);
          sscanf(s,"%f", &z);
          printf("found V %f %f %f\n", x,y,z);
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

          pt->compute_surface_orientation_and_size(); // also computes uv's for iertices

          pt->plane.align_with_point_normal(pt->center.original, normalized(pt->sfcnormal.original - pt->center.original));


          printf("done geomtex\n");
        } else if(strcmp(s,"PORTALPOLY")==0) {
          printf("portal\n");
          int num_vtx;

          s = strtok(NULL, " ");
          sscanf(s, "%d", &num_vtx);
          objects[new_onum]->polygons[p_idx] = new l3d_polygon_3d_portal(num_vtx);
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
          printf("%d\n", *s2);
          l3d_polygon_3d_portal *pp;
          pp = dynamic_cast<l3d_polygon_3d_portal *>(objects[new_onum]->polygons[p_idx]);
          if(pp) { strcpy(pp->connected_sector_name, s); }
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
      printf("%f %f %f, %f %f %f, %f %f %f, %f %f %f\n",
             posx, posy, posz, xaxis_x, xaxis_y, xaxis_z,
             yaxis_x, yaxis_y, yaxis_z,
             zaxis_x, zaxis_y, zaxis_z);
      assert(current_sector);
      this->current_sector = current_sector;
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

  screen->sinfo->compute_light_table();
  screen->sinfo->compute_fog_table();
}


void l3d_world_portal_textured_lightmapped::place_lamp
(l3d_sector *sector, l3d_polygon_3d_clippable *clipwin)
{
  static int recurselevel = 0;

  if(recurselevel > 4) return;
  recurselevel ++;
  int iObj, iFacet;

  l3d_vector facet_to_cam_vec;
  l3d_vector N;

  l3d_polygon_3d_clippable *pc;

  // init polys for all objects contained in the sector
  for(int i=0; i<sector->objects->num_items; i++) {
    (*sector->objects)[i]->init_nonculled_list();
    (*sector->objects)[i]->vertices->num_varying_items = 0;
    (*sector->objects)[i]->transform_stage = 0;

    for(register int ivtx=0; ivtx<(*sector->objects)[i]->vertices->num_fixed_items; ivtx++) {
      (*((*sector->objects)[i]->vertices))[ivtx].reset();
    }

    l3d_polygon_3d_node *n;
    n = (*sector->objects)[i]->nonculled_polygon_nodes;
    while(n) {
      n->polygon->init_clip_ivertices();
      n->polygon->init_transformed();
      n=n->next;
    }

    // position all vertices of object into world space

    // (*sector->objects)[i] transform
    if ((*sector->objects)[i]->num_xforms) {
      (*sector->objects)[i]->transform((*sector->objects)[i]->modeling_xform);
    }

    // sector transform
    if (sector->num_xforms) {
      (*sector->objects)[i]->transform(sector->modeling_xform);
    }
  }

  // now concatenate all polys in all objects into one big object-polys
  // list. IMPORTANT!!! after this operation, the nonculled_polygon_nodes
  // CANNOT be used anymore within the context of one single object,
  // because the last node of an object's nonculled poly list points to
  // the first node of the NEXT object's nonculled list, because this
  // is exactly what the concatenation does.

  l3d_polygon_3d_node *n_objects=0;

  for(int i=0; i<sector->objects->num_items; i++) {
    // init_nonculled_list was already called in the first loop,
    // where we placed all polys into world space. otherwise, we
    // must call init_nonculled list here.
    // (*objects)[i]->init_nonculled_list();

    // prepend this object's polys to the overall list of all polys in all
    // objects
    if((*sector->objects)[i]->nonculled_polygon_nodes) {
      l3d_polygon_3d_node *n_temp;
      n_temp = (*sector->objects)[i]->nonculled_polygon_nodes;
      while(n_temp->next) {
        n_temp=n_temp->next;
      }

      // assign current poly list to end of this object's poly list
      n_temp->next = n_objects;
      if(n_objects) { n_objects->prev = n_temp; } // dont forget the rev link!
      // reassign the overall poly list to be the new, longer list
      n_objects = (*sector->objects)[i]->nonculled_polygon_nodes;

      // the head element of the list can have no previous element
      n_objects->prev = NULL;
    }
  }

  // initialize object
  sector->init_nonculled_list();
  sector->vertices->num_varying_items = 0;
  sector->transform_stage = 0;
  for(register int ivtx=0; ivtx<sector->vertices->num_fixed_items; ivtx++) {
    (*(sector->vertices))[ivtx].reset();
  }

  l3d_polygon_3d_node *n;
  n = sector->nonculled_polygon_nodes;
  while(n) {
    n->polygon->init_clip_ivertices();
    n->polygon->init_transformed();
    n=n->next;
  }


  // position all vertices of object into world space

  if (sector->num_xforms) {
    sector->transform(sector->modeling_xform);
  }

  // cull/clip polys in world space

  // copy object for mirror usage
  //    l3d_object copied_sector_in_wc(*sector);
  //    l3d_object copied_camera(camera);
  //    l3d_matrix reversal_xform (-1.0,0.0,0.0,0.0,
  //				0.0,-1.0,0.0,0.0,
  //				0.0,0.0,-1.0,0.0,
  //				0.0,0.0,0.0,1.0);

  // at this point, the sector geometry and the geometry of all
  // contained objects are in world coordinates and have any transformations
  // already applied. now we dump all polys from all objects and the sector
  // into one big list - from now on, there is no distinction between
  // sector polys and contained-object-polys. they are just polys from now
  // on.
  l3d_polygon_3d_node *last_sector_node;
  {
    l3d_polygon_3d_node *n;

    // for all object polygons and their vlists: set the transform stage
    // to be the same as for the sector. this is because the object maybe
    // underwent more or fewer transformations due to its own modeling
    // transformation status (yes or no), meaning that till now the
    // transform stage for the objectpolys and the sectorpolys might be
    // different. now, we must set them the same since they are one list.

    n = n_objects;
    while(n) {
      n->polygon->set_transform_stage(sector->transform_stage);
      n = n->next;
    }

    n = sector->nonculled_polygon_nodes;
    if(n) {
      while(n->next) { n = n->next; }
      last_sector_node = n;
      n->next = n_objects; // append object polys to sector poly list
      if(n_objects) { n_objects->prev = n; }
    } else {
      last_sector_node=NULL;
      sector->nonculled_polygon_nodes = n_objects;
    }
  }


  // transform surviving polys to camera space

  sector->camera_transform(camera->viewing_xform);

  // clip all geometry against current portal in 3D
  if(clipwin) {

    l3d_point origin(0,0,0,1), p0, p1;
    l3d_plane clip_plane;

    int survived=1;
    int idx0,idx1;

    idx0 = clipwin->clip_ivertices->num_items-1;
    idx1 = 0;

    while(1) {
      p1 = (**(clipwin->vlist))[(*clipwin->clip_ivertices)[idx0].ivertex].transformed;
      p0 = (**(clipwin->vlist))[(*clipwin->clip_ivertices)[idx1].ivertex].transformed;
      clip_plane.align_with_points( origin, p0, p1);

      sector->clip_to_plane(clip_plane);

      idx0 = idx1;
      if(idx0==clipwin->clip_ivertices->num_items-1) break;
      idx1++;
    }}



  n = sector->nonculled_polygon_nodes;
  while(n) {
    N.set(n->polygon->sfcnormal.transformed.X_ - n->polygon->center.transformed.X_,
          n->polygon->sfcnormal.transformed.Y_ - n->polygon->center.transformed.Y_,
          n->polygon->sfcnormal.transformed.Z_ - n->polygon->center.transformed.Z_,
          int_to_l3d_real(0));

    facet_to_cam_vec.set (0 - n->polygon->center.transformed.X_,
                          0 - n->polygon->center.transformed.Y_,
                          0 - n->polygon->center.transformed.Z_,
                          int_to_l3d_real(0));

    /* dot product explicitly formulated so no fixed point overflow */
    /* (facet_to_cam_vec is very large) */
    /* (use of the inline function "dot" uses default fixed precision) */
    //#include "../lib/math/fix_lowp.h"
    //#define BACKFACE_EPS float_to_l3d_real(0.0)
    //      if(( l3d_mulrr(FX_CVT(N.X_), FX_CVT(facet_to_cam_vec.X_)) +
    //	   l3d_mulrr(FX_CVT(N.Y_), FX_CVT(facet_to_cam_vec.Y_)) +
    //	   l3d_mulrr(FX_CVT(N.Z_), FX_CVT(facet_to_cam_vec.Z_)) > BACKFACE_EPS)
    /* equivalent to: if(N.dot(facet_to_cam_vec) > 0 ) */
    if(	 (dot(N,facet_to_cam_vec) > 0 ) )
      //#include "../lib/math/fix_prec.h"
    {
      // polygon is not culled
    } else {
      // take this polygon out of the list of nonculled_polygon_nodes
      if(n->prev) {
        n->prev->next = n->next;
      } else {
        sector->nonculled_polygon_nodes = n->next;
      }

      if(n->next) {
        n->next->prev = n->prev;
      }
    }


    n = n->next;
  }




  //// light all lumels

  n = sector->nonculled_polygon_nodes;
  while(n) {

    l3d_polygon_3d_textured_lightmapped *pt;
    l3d_polygon_3d_portal *p;

    if ( (p=dynamic_cast<l3d_polygon_3d_portal *>(n->polygon)) ) {
      place_lamp(p->connected_sector, p);
    } else if ((pt = dynamic_cast<l3d_polygon_3d_textured_lightmapped *>
                     (n->polygon)))
    {
      l3d_screen_info *si = new l3d_screen_info_indexed(MAX_LIGHT_LEVELS,
                            0, 0, 0);
      si->p_screenbuf = pt->lightmap->tex_data->data;

      l3d_rasterizer_2d_sw_lighter_imp *rast_imp = new
          l3d_rasterizer_2d_sw_lighter_imp(pt->lightmap->tex_data->width,
                                           pt->lightmap->tex_data->height,si);
      l3d_rasterizer_2d rast(rast_imp);

      // set up the O,U,V for the rasterizer: these must be
      // O U and V for the lightmap resolution, NOT necess. for the
      // entire surface (i.e. tiled texture) resolution (though these
      // are the same if the lightmap is applied to the entire
      // tiled texture and not just to the used part)

      rast_imp->O = pt->surface_orientation.O.transformed;
      rast_imp->U = pt->surface_orientation.U.transformed - rast_imp->O;
      rast_imp->V = pt->surface_orientation.V.transformed - rast_imp->O;

      l3d_polygon_2d_flatshaded *lightmap_poly;
      l3d_two_part_list<l3d_coordinate> *vlist;
      vlist = new l3d_two_part_list<l3d_coordinate>
              (pt->clip_ivertices->num_items);

      int i;
      lightmap_poly = new l3d_polygon_2d_flatshaded
                      (pt->clip_ivertices->num_items);
      lightmap_poly->vlist = &vlist;
      for(i=0; i< pt->clip_ivertices->num_items; i++) {
        l3d_point tex_coord;
        tex_coord =
          world_to_tex_matrix(pt->surface_orientation)
          | camera->inverse_viewing_xform
          | (**pt->vlist) [ (*pt->clip_ivertices)[i].ivertex ].transformed;


        if(tex_coord.W_ != int_to_l3d_real(1)) {
          l3d_real invw = l3d_divrr(int_to_l3d_real(1), tex_coord.W_);
          tex_coord.X_ = l3d_mulrr(tex_coord.X_, invw);
          tex_coord.Y_ = l3d_mulrr(tex_coord.Y_, invw);
          tex_coord.Z_ = l3d_mulrr(tex_coord.Z_, invw);
          tex_coord.W_ = int_to_l3d_real(1);
        }

        l3d_real ul,vl;
        ul = l3d_mulri(tex_coord.X_, pt->lightmap->tex_data->width);
        vl = l3d_mulri(tex_coord.Y_, pt->lightmap->tex_data->height);

        ul = ul + float_to_l3d_real(0.5);
        vl = vl + float_to_l3d_real(0.5);
        ul = (int)ul;
        vl = (int)vl;

        l3d_coordinate vtx;
        vtx.transformed.set
        (ul, vl,
         int_to_l3d_real(0),
         int_to_l3d_real(1));
        // we don't care about the vtx.original value since this
        // vertex is just used once, here, immediately, to draw
        // the lightmap

        printf("gonna draw uv vtx %f %f\n", vtx.transformed.X_, vtx.transformed.Y_);
        (*vlist)[i] = vtx;
        (*lightmap_poly->clip_ivertices)
        [lightmap_poly->clip_ivertices->next_index()].ivertex =
          //	       pt->clip_ivertices->num_items-1  - i;
          i;
      }

      lightmap_poly->final_color = 255;
      lightmap_poly->draw(&rast);

      delete vlist;
      delete lightmap_poly;
      delete si;
      delete rast_imp;
    }
    n = n->next;
  }

  recurselevel--;
}


#ifdef PORTAL_LIGHT



// back face culling: elimination of back-facing facets
n = sector->nonculled_polygon_nodes;
while(n) {

  // FIXME: this could actually be done BEFORE the camera transform
  // by applying the inverse camera transform to get the camera
  // position in world space, doing frustum clipping there
  l3d_point origin(0,0,0,1), p0, p1;
  l3d_plane clip_plane;

  int survived=1;
  int idx0,idx1;

  idx0 = clipwin->clip_ivertices->num_items-1;
  idx1 = 0;

  while(survived) {
    p1 = (**(clipwin->vlist))[(*clipwin->clip_ivertices)[idx0].ivertex].transformed;
    p0 = (**(clipwin->vlist))[(*clipwin->clip_ivertices)[idx1].ivertex].transformed;

    p0.X_ -= screen->xsize / 2;
    p0.Y_ = screen->ysize/2 - p0.Y_;
    p0.Z_ = l3d_mulri(camera->fovx, screen->xsize);
    p1.X_ -= screen->xsize / 2;
    p1.Y_ = screen->ysize/2 - p1.Y_;
    p1.Z_ = l3d_mulri(camera->fovx, screen->xsize);

    clip_plane.align_with_points( origin, p0, p1);

    pc = dynamic_cast<l3d_polygon_3d_clippable *>(n->polygon);
    if(pc) if( ! (survived=pc->clip_to_plane(clip_plane)) ) break;

    idx0 = idx1;
    if(idx0==clipwin->clip_ivertices->num_items-1) break;
    idx1++;
  }

  N.set(n->polygon->sfcnormal.transformed.X_ - n->polygon->center.transformed.X_,
        n->polygon->sfcnormal.transformed.Y_ - n->polygon->center.transformed.Y_,
        n->polygon->sfcnormal.transformed.Z_ - n->polygon->center.transformed.Z_,
        int_to_l3d_real(0));

  facet_to_cam_vec.set (0 - n->polygon->center.transformed.X_,
                        0 - n->polygon->center.transformed.Y_,
                        0 - n->polygon->center.transformed.Z_,
                        int_to_l3d_real(0));

  /* dot product explicitly formulated so no fixed point overflow */
  /* (facet_to_cam_vec is very large) */
  /* (use of the inline function "dot" uses default fixed precision) */
  //#include "../lib/math/fix_lowp.h"
  //#define BACKFACE_EPS float_to_l3d_real(0.0)
  //      if(( l3d_mulrr(FX_CVT(N.X_), FX_CVT(facet_to_cam_vec.X_)) +
  //	   l3d_mulrr(FX_CVT(N.Y_), FX_CVT(facet_to_cam_vec.Y_)) +
  //	   l3d_mulrr(FX_CVT(N.Z_), FX_CVT(facet_to_cam_vec.Z_)) > BACKFACE_EPS)
  /* equivalent to: if(N.dot(facet_to_cam_vec) > 0 ) */
  if(
    survived

    &&

    (dot(N,facet_to_cam_vec) > 0 )
    //#include "../lib/math/fix_prec.h"

    &&

    n->polygon->clip_near_z(camera->near_z))

  {
    // polygon is not culled
  } else {
    // take this polygon out of the list of nonculled_polygon_nodes
    if(n->prev) {
      n->prev->next = n->next;
    } else {
      sector->nonculled_polygon_nodes = n->next;
    }

    if(n->next) {
      n->next->prev = n->prev;
    }
  }


  n = n->next;
}

// project surviving polygons
sector->apply_perspective_projection
(*camera, screen->xsize, screen->ysize);

//     /////////MIRRRRRRRRRROOOOOOOOOOOOORRRRRRRRR
//     // transform surviving polys to camera space

//     sector->transform(camera.viewing_xform);

//     // back face culling: elimination of back-facing facets
//     n = sector->nonculled_polygon_nodes;
//     while(n) {
//       N.set(n->polygon->sfcnormal.transformed.X_ - n->polygon->center.transformed.X_,
// 	    n->polygon->sfcnormal.transformed.Y_ - n->polygon->center.transformed.Y_,
// 	    n->polygon->sfcnormal.transformed.Z_ - n->polygon->center.transformed.Z_,
// 	    int_to_l3d_real(0));

//       facet_to_cam_vec.set (0 - n->polygon->center.transformed.X_,
// 			    0 - n->polygon->center.transformed.Y_,
// 			    0 - n->polygon->center.transformed.Z_,
// 			    int_to_l3d_real(0));

//       /* dot product explicitly formulated so no fixed point overflow */
//       /* (facet_to_cam_vec is very large) */
//       /* (use of the inline function "dot" uses default fixed precision) */
//       //#include "../lib/math/fix_lowp.h"
//       //#define BACKFACE_EPS float_to_l3d_real(0.0)
//       //      if(( l3d_mulrr(FX_CVT(N.X_), FX_CVT(facet_to_cam_vec.X_)) +
//       //	   l3d_mulrr(FX_CVT(N.Y_), FX_CVT(facet_to_cam_vec.Y_)) +
//       //	   l3d_mulrr(FX_CVT(N.Z_), FX_CVT(facet_to_cam_vec.Z_)) > BACKFACE_EPS)
//          /* equivalent to: if(N.dot(facet_to_cam_vec) > 0 ) */
//       if((dot(N,facet_to_cam_vec) > 0 )
//       //#include "../lib/math/fix_prec.h"

// 	 &&

// 	 n->polygon->clip_near_z(camera.near_z))

// 	{
// 	  // polygon is not culled
// 	} else {
// 	  // take this polygon out of the list of nonculled_polygon_nodes
// 	  if(n->prev) {
// 	    n->prev->next = n->next;
// 	  } else {
// 	    sector->nonculled_polygon_nodes = n->next;
// 	  }

// 	  if(n->next) {
// 	    n->next->prev = n->prev;
// 	  }
// 	}


//       n = n->next;
//     }
//     /////////MIRRRRRRRRRROOOOOOOOOOOOORRRRRRRRR

n = sector->nonculled_polygon_nodes;
while(n) {
  pc = dynamic_cast<l3d_polygon_3d_clippable *>(n->polygon);

  if (pc || (!pc && ( n->polygon->clip_to_polygon_2d(clipwin)))) {


    // polygon survived all culling tests and will be lit.
    l3d_polygon_3d_portal *p;
    if ( (p=dynamic_cast<l3d_polygon_3d_portal *>(n->polygon)) ) {
      render_sector(p->connected_sector, p);
    } else {
      l3d_polygon_3d_textured_lightmapped *pt;
      pt = dynamic_cast<l3d_polygon_3d_textured_lightmapped *>(n->polygon);
      if(pt) {

        l3d_screen_info *si = new l3d_screen_info_indexed(MAX_LIGHT_LEVELS,
                              0, 0, 0);
        si->p_screenbuf = pt->lightmap->tex_data->data;

        l3d_rasterizer_2d_sw_lighter_imp *rast_imp = new
            l3d_rasterizer_2d_sw_lighter_imp(pt->lightmap->tex_data->width,
                                             pt->lightmap->tex_data->height,si);
        l3d_rasterizer_2d rast(rast_imp);

        // set up the O,U,V for the rasterizer: these must be
        // O U and V for the lightmap resolution, NOT necess. for the
        // entire surface (i.e. tiled texture) resolution (though these
        // are the same if the lightmap is applied to the entire
        // tiled texture and not just to the used part)

        rast_imp->O = pt->surface_orientation.O.transformed;
        rast_imp->U = pt->surface_orientation.U.transformed - rast_imp->O;
        rast_imp->V = pt->surface_orientation.V.transformed - rast_imp->O;

        l3d_polygon_2d_flatshaded *lightmap_poly;
        l3d_two_part_list<l3d_coordinate> *vlist;
        vlist = new l3d_two_part_list<l3d_coordinate>
                (pt->clip_ivertices->num_items);

        int i;
        lightmap_poly = new l3d_polygon_2d_flatshaded
                        (pt->clip_ivertices->num_items);
        lightmap_poly->vlist = &vlist;
        for(i=0; i< pt->clip_ivertices->num_items; i++) {
          // from texture computer
          O = pt->surface_orientation.O.transformed;
          U = pt->surface_orientation.U.transformed-O;
          V = pt->surface_orientation.V.transformed-O;

          uv_from_world((**pt->vlist)[(*pt->clip_ivertices)[i].ivertex].
                        transformed_intermediates[1]);
          // transformed_intermedate #1 is the camera-space coord just
          // before perspective projection

          l3d_coordinate vtx;
          vtx.transformed.set
          (l3d_mulri(u, pt->lightmap->tex_data->width),
           l3d_mulri(v, pt->lightmap->tex_data->height),
           int_to_l3d_real(0),
           int_to_l3d_real(1));
          // we don't care about the vtx.original value since this
          // vertex is just used once, here, immediately, to draw
          // the lightmap

          (*vlist)[i] = vtx;
          (*lightmap_poly->clip_ivertices)
          [lightmap_poly->clip_ivertices->next_index()].ivertex =
            pt->clip_ivertices->num_items-1  - i;
        }

        lightmap_poly->final_color = 15;
        lightmap_poly->draw(&rast);


        for(i=0; i< pt->clip_ivertices->num_items; i++) {
          // from texture computer
          O = pt->surface_orientation.O.transformed;
          U = pt->surface_orientation.U.transformed-O;
          V = pt->surface_orientation.V.transformed-O;

          uv_from_world((**pt->vlist)[(*pt->clip_ivertices)[i].ivertex].
                        transformed_intermediates[1]);
          // transformed_intermedate #1 is the camera-space coord just
          // before perspective projection

          l3d_coordinate vtx;
          vtx.transformed.set
          (l3d_mulri(u, pt->lightmap->tex_data->width),
           l3d_mulri(v, pt->lightmap->tex_data->height),
           int_to_l3d_real(0),
           int_to_l3d_real(1));
          // we don't care about the vtx.original value since this
          // vertex is just used once, here, immediately, to draw
          // the lightmap

          (*vlist)[i] = vtx;
          (*lightmap_poly->clip_ivertices)
          [i].ivertex = i;
        }

        lightmap_poly->final_color = 10;
        lightmap_poly->draw(&rast);

        pt->lightmap->tex_data->data [ 31*32 ] = 15;

        delete vlist;
        delete lightmap_poly;
        delete si;
        delete rast_imp;
      }
    }
  }
  n = n->next;
}
}
#endif
