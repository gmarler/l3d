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

#include "w_portex.h"
#include "../object/object3d.h"
#include "../polygon/p_portal.h"
#include <string.h>
#include "../../tool_os/memman.h"

l3d_world_portal_textured::l3d_world_portal_textured(int xsize, int ysize)
    : l3d_world_portal (xsize, ysize)
{
  l3d_screen_info *si = screen->sinfo;
  si->ext_to_native(0,0,0); //- allocate black bg color first
  texture_loader = new l3d_texture_loader_ppm(screen->sinfo);

  rasterizer_3d_imp->fovx = &(camera->fovx);
  rasterizer_3d_imp->fovy = &(camera->fovy);
  rasterizer_3d_imp->screen_xsize = &(screen->xsize);
  rasterizer_3d_imp->screen_ysize = &(screen->ysize);

}

void l3d_world_portal_textured::load_from_file(const char *fname) {
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

  for(int onum=0; onum<snum; onum++) {
    char sname[80];
    int numv, nump;
    fgets(line, linesize, fp);
    sscanf(line,"SECTOR %s\n", sname);
    //    printf("reading sector %s\n",sname);

    fgets(line,linesize,fp);
    sscanf(line, "%d %d", &numv, &nump);

    int new_onum;
    objects[new_onum=objects.next_index()] = new l3d_sector(numv);
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
      //      printf("new line: %s\n",line);
      if(strcmp(s,"GEOMPOLY")==0) {
        //printf("geompoly\n");
        int num_vtx;
        l3d_polygon_3d_textured *pt;

        s = strtok(NULL, " ");
        sscanf(s, "%d", &num_vtx);
        objects[new_onum]->polygons[p_idx] = pt =
                                               new l3d_polygon_3d_textured(num_vtx);
        objects[new_onum]->polygons[p_idx]->vlist = &objects[new_onum]->vertices;

        for(int vtx=0; vtx<num_vtx; vtx++) {
          s = strtok(NULL, " ");
          int iv;
          sscanf(s, "%d", &iv);
          (*(objects[new_onum]->polygons[p_idx]->ivertices))[
            objects[new_onum]->polygons[p_idx]->ivertices->next_index()].ivertex = iv;

        }

        /*
        pt->fovx = &(camera->fovx);
        pt->fovy = &(camera->fovy);
        pt->screen_xsize = &(screen->xsize);
        pt->screen_ysize = &(screen->ysize);
        */

        int tex_id = 1;
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

        pt->assign_tex_coords_from_tex_space(*pt->texture);

        objects[new_onum]->polygons[p_idx]->compute_center();
        objects[new_onum]->polygons[p_idx]->compute_sfcnormal();
      } else if(strcmp(s,"GEOMPOLY_TEX")==0) {

        //printf("geompoly_tex\n");
        int num_vtx;
        l3d_polygon_3d_textured *pt;

        s = strtok(NULL, " ");
        sscanf(s, "%d", &num_vtx);
        objects[new_onum]->polygons[p_idx] = pt = new l3d_polygon_3d_textured(num_vtx);
        objects[new_onum]->polygons[p_idx]->vlist = &objects[new_onum]->vertices;

        for(int vtx=0; vtx<num_vtx; vtx++) {
          s = strtok(NULL, " ");
          int iv;
          sscanf(s, "%d", &iv);
          (*(objects[new_onum]->polygons[p_idx]->ivertices))[
            objects[new_onum]->polygons[p_idx]->ivertices->next_index()].ivertex = iv;
        }
        s = strtok(NULL, " ");
        /*
        pt->fovx = &(camera->fovx);
        v	pt->fovy = &(camera->fovy);
        pt->screen_xsize = &(screen->xsize);
        pt->screen_ysize = &(screen->ysize);*/

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
        pt->assign_tex_coords_from_tex_space(*pt->texture);


        objects[new_onum]->polygons[p_idx]->compute_center();
        objects[new_onum]->polygons[p_idx]->compute_sfcnormal();
        //printf("done geomtex\n");
      } else {
        //printf("portal\n");
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
        //printf("%d\n", *s2);
        l3d_polygon_3d_portal *pp;
        pp = dynamic_cast<l3d_polygon_3d_portal *>(objects[new_onum]->polygons[p_idx]);
        if(pp) { strcpy(pp->connected_sector_name, s); }
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
  screen->sinfo->compute_light_table();
  screen->sinfo->compute_fog_table();
}


l3d_world_portal_textured::~l3d_world_portal_textured(void) {
  delete texture_loader;
}
