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

#include "w_oct.h"

#include "../object/object3d.h"
#include "../polygon/p_portal.h"
#include <string.h>
#include "../polygon/p3_ltex.h"
#include "../polygon/p3_cflat.h"
#include "../polygon/p_cport.h"
#include "../../raster/ras_swl.h"
#include "../../view/camcol.h"

#include "../../system/fact0_2.h"
#include "../../tool_os/memman.h"

void l3d_world_octree::draw_all(void) {
  l3d_world_portal_textured_lightmapped_obj::draw_all();
  return;

  //  	  int pi, idxv,idxc;
  //  	  (*clipwin->vlist)->num_varying_items = 0;
  //  	  clipwin->clip_ivertices->num_items=0;

  //  	  for(pi=0; pi<screen->view_win->clip_ivertices->num_items; pi++) {
  //  	    idxc = clipwin->clip_ivertices->next_index();
  //  	    idxv = (*clipwin->vlist)->next_varying_index();
  //  	    (**clipwin->vlist)[idxv].transformed =
  //  	      (**screen->view_win->vlist)[(*screen->view_win->clip_ivertices)[pi].ivertex].
  //  	      transformed;

  //  	    // set the z (distance from origin of proj. plane)
  //  	    // for the initial clip win

  //  	    (**clipwin->vlist)[idxv].transformed.X_ -= screen->xsize / 2;
  //  	    (**clipwin->vlist)[idxv].transformed.Y_ = screen->ysize/2 -
  //  	      (**clipwin->vlist)[idxv].transformed.Y_;
  //  	    (**clipwin->vlist)[idxv].transformed.Z_ =
  //  	      l3d_mulri(camera->fovx, screen->xsize);

  //  	    (*clipwin->clip_ivertices) [idxc].ivertex = idxv;
  //  	  }

  //  	  current_sector->render(clipwin,rasterizer,camera,
  //  				 screen->xsize, screen->ysize);

}

l3d_world_octree::l3d_world_octree
(int xsize, int ysize)
    : l3d_world_portal_textured_lightmapped_obj(xsize, ysize)
{
}

l3d_world_octree::
~l3d_world_octree(void)
{}

void l3d_world_octree::load_from_file(const char *fname) {
  FILE *fp;

  int linesize=1536;
  char line[1536];
  int snum;

  screen->sinfo->ext_to_native(0,0,0); //- allocate black bg color

  fp = fopen("world.dat", "rb");
  fgets(line,linesize,fp);
  sscanf(line,"%d",&snum);

  l3d_sector *current_sector=NULL;

  for(int onum=0; onum<snum; onum++) {
    char sname[80];
    char keyword[80];
    char plugin_name[1024];
    int numv, nump;
    fgets(line, linesize, fp);
    strcpy(keyword,"");
    strcpy(sname,"");
    strcpy(plugin_name,"");
    sscanf(line,"%s %s",keyword, sname);

    if(strcmp(keyword,"SECTOR")==0) {
      int num_read_items;
      int contains_geometry;
      int node_minx, node_miny, node_minz,
      node_maxx, node_maxy, node_maxz;

      printf("reading sector %s\n",sname);

      num_read_items =
        sscanf(line,"%s %s %d %d %d %d %d %d\n",keyword, sname,
               &node_minx, &node_miny, &node_minz,
               &node_maxx, &node_maxy, &node_maxz);
      if(num_read_items == 8) {
        contains_geometry = 1;
        printf("sector contains geometry, node bounding box "
               "%d %d %d, %d %d %d\n",
               node_minx, node_miny, node_minz,
               node_maxx, node_maxy, node_maxz);
      } else {
        contains_geometry = 0;
        printf("sector is an empty container node\n");
      }

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

#undef DEF_LTEX
#ifdef DEF_LTEX

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

          int tex_id = 1;
          pt->texture = new l3d_texture;
          pt->texture->tex_data = & tex_data[tex_id];

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

          pt->compute_surface_orientation_and_size();

          pt->plane.align_with_point_normal(pt->center.original, normalized(pt->sfcnormal.original - pt->center.original));

#else

printf("geompoly\n");
          int num_vtx;
          l3d_polygon_3d_flatshaded_clippable *pf;

          s = strtok(NULL, " ");
          sscanf(s, "%d", &num_vtx);
          objects[new_onum]->polygons[p_idx] = pf =
                                                 new l3d_polygon_3d_flatshaded_clippable(num_vtx);
          objects[new_onum]->polygons[p_idx]->vlist = &objects[new_onum]->vertices;

          for(int vtx=0; vtx<num_vtx; vtx++) {
            s = strtok(NULL, " ");
            int iv;
            sscanf(s, "%d", &iv);
            (*(objects[new_onum]->polygons[p_idx]->ivertices))[
              objects[new_onum]->polygons[p_idx]->ivertices->next_index()].ivertex = iv;

          }

          pf->final_color =
            screen->sinfo->ext_to_native
            (rand() % screen->sinfo->ext_max_red,
             rand() % screen->sinfo->ext_max_green,
             rand() % screen->sinfo->ext_max_blue);
          if(pf->final_color==0) {
            // color 0 probably means that in indexed mode, palette overflow.
            // so just choose any random color in that case. we are assigning
            // colors randomly anyway.
            pf->final_color = rand();
          }

          objects[new_onum]->polygons[p_idx]->compute_center();
          objects[new_onum]->polygons[p_idx]->compute_sfcnormal();

          pf->plane.align_with_point_normal(pf->center.original, normalized(pf->sfcnormal.original - pf->center.original));


#endif

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

          pt->compute_surface_orientation_and_size();

          pt->plane.align_with_point_normal(pt->center.original, normalized(pt->sfcnormal.original - pt->center.original));


          printf("done geomtex\n");
        } else {
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
          l3d_polygon_3d_portal *pp; // should be clippable since we also define the plane of the portal poly
          pp = dynamic_cast<l3d_polygon_3d_portal *>(objects[new_onum]->polygons[p_idx]);
          if(pp) {
            strcpy(pp->connected_sector_name, s);
            pp->plane.align_with_point_normal(pp->center.original, normalized(pp->sfcnormal.original - pp->center.original));
          }
        }

      }

      if(contains_geometry) {
        int nodex, nodey, nodez;
        char node_name[256];
        l3d_sector *node;
        l3d_sector *clipped_sector_part;

        printf("about to do all nodes\n");
        for(nodez=node_minz; nodez<=node_maxz; nodez++) {
          printf("in for z nodez %d node_minz %d node_maxz %d\n",nodez,node_minz,node_maxz);
          for(nodey=node_miny; nodey<=node_maxy; nodey++) {
            printf("in for y\n");
            for(nodex=node_minx; nodex<=node_maxx; nodex++) {
              printf("in for x\n");
              sprintf(node_name,"N%03d%03d%03d",nodex,nodey,nodez);
              printf("s by name\n");
              node = sector_by_name(node_name);
              // note that the containing node MUST ALREADY EXIST, this is why
              // in the world file all nodes are written first, THEN all
              // geometry-containing sectors. we could also have solved this
              // with a second loop through all objects, but it is clearer to
              // impose an ordering constraint on the world file and process
              // both nodes and geometry-sectors in one pass.

              // create sector, which will be clipped to this node
              printf("about to create new geom\n");
              assert(current_sector);
              clipped_sector_part = new l3d_sector
                                    (current_sector->vertices->num_fixed_items);

              printf("about to copy geom\n");

              // copy the complete original geometry into the clipped-sector
              *clipped_sector_part = *current_sector;
              clipped_sector_part->reset();

              // clip the clipped-sector to the current node
              int iclippoly;
              for(iclippoly=0; iclippoly<node->polygons.num_items; iclippoly++) {
                l3d_polygon_3d_clippable *pc_clip;
                pc_clip = dynamic_cast<l3d_polygon_3d_clippable*>
                          (node->polygons[iclippoly]);
                if(pc_clip) {
                  l3d_polygon_3d_node *n = clipped_sector_part->nonculled_polygon_nodes;
                  printf("about to clip geom\n");
                  while(n) {
                    l3d_polygon_3d_clippable *pc;
                    pc = dynamic_cast<l3d_polygon_3d_clippable*>
                         (n->polygon);
                    if(pc) {
                      if(pc->clip_to_plane(pc_clip->plane)) {
                        // polygon survived: do nothing
                      } else {
                        // take this polygon out of the list of nonculled_polygon_nodes
                        if(n->prev) {
                          n->prev->next = n->next;
                        } else {
                          clipped_sector_part->nonculled_polygon_nodes = n->next;
                        }

                        if(n->next) {
                          n->next->prev = n->prev;
                        }
                      }
                    }
                    n = n->next;
                  }
                }
              }

              printf("makign clipping perm\n");
              // make the clipped geometry permanent (move transformed
              // into original)
              clipped_sector_part->vertices->num_fixed_items =
                clipped_sector_part->vertices->num_fixed_items +
                clipped_sector_part->vertices->num_varying_items;
              clipped_sector_part->vertices->max_varying_items =
                clipped_sector_part->vertices->max_varying_items -
                clipped_sector_part->vertices->num_varying_items;
              clipped_sector_part->vertices->num_varying_items = 0;

              int ivtx;
              for(ivtx=0;
                  ivtx<clipped_sector_part->vertices->num_fixed_items;
                  ivtx++)
              {
                (*clipped_sector_part->vertices)[ivtx].original =
                  (*clipped_sector_part->vertices)[ivtx].transformed;
              }


              printf("making clip_iv to iv\n");
              l3d_polygon_3d_node *n = clipped_sector_part->nonculled_polygon_nodes;
              while(n)
              {
                *(n->polygon->ivertices) = *(n->polygon->clip_ivertices);
                n = n->next;
              }


              //  	    init_nonculled_list() in l3d_object, which we call at the beg.
              //  	    of each frame, will blindly relink together all polygons again,
              //  	    ** INCLUDING** those which we completely clipped away above!
              //  	    so we ahve to also bake-in the currenet state of the nonculled_list
              //  	    so that the next call to init_nonculled_list creates the current
              //  	    list of polys and no more.

              printf("creating new culled poly list\n");
              n = clipped_sector_part->nonculled_polygon_nodes;
              l3d_list<l3d_polygon_3d*> new_polygon_list; // contains only surviving polys
              while(n) {
                new_polygon_list[new_polygon_list.next_index()] = n->polygon;
                n=n->next;
              }
              clipped_sector_part->polygons = new_polygon_list;


              n = clipped_sector_part->nonculled_polygon_nodes;

              printf("assigning clipped geom to node\n");
              if(n) {
                // if anything is left after clipping, then
                // assign this clipped-sector to this node's object list

                (*node->objects)[node->objects->next_index()] =
                  clipped_sector_part;
                printf("node %s contains part of object\n", node->name);
              } else {
                printf("node %s contains no part of object\n", node->name);
              }

            }
          }
        }
      }

      printf("done all nodes\n");

    } else if(strcmp(keyword,"OBJECT")==0) {
      sscanf(line,"%s %s %s\n",keyword, sname, plugin_name);

      int new_onum;
      assert(current_sector);
      objects[new_onum = objects.next_index()]
      = (*current_sector->objects)[current_sector->objects->next_index()]
        = new l3d_object_clippable_boundable(100);
      // will be deleted and recreated  by object
      // (max fixed vtx count)

      strcpy(objects[new_onum]->name, sname);
      strcpy(objects[new_onum]->plugin_name, plugin_name);
      objects[new_onum]->parent = current_sector;

      if(strlen(plugin_name)) {
        objects[new_onum]->plugin_loader =
          factory_manager_v_0_2.plugin_loader_factory->create();
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

        if(objects[new_onum]->plugin_constructor) {
          (*objects[new_onum]->plugin_constructor) (objects[new_onum],
              texture_loader);
        }
      }
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

  screen->refresh_palette();
}
