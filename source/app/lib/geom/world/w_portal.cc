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

#include "../object/object3d.h"
#include "../polygon/p_flat.h"
#include "../polygon/p_portal.h"
#include "../../tool_2d/screen.h"
#include "../../tool_os/dispatch.h"
#include "../../raster/rasteriz.h"
#include "../../tool_2d/scrinfo.h"
#include "w_portal.h"
#include "../../system/fact0_2.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include "../../tool_os/memman.h"

void l3d_world_portal::update_all(void) {
  l3d_world::update_all();

  // BRUTE FORCE check and update of current_sector

  // we loop through ALL polygons (NOT the post-3d-clipping polygons!)
  // to check for 3d containment within the ORIGINAL sector

  int i;
  l3d_sector *s;
  int found=0;


  int portal_num=0;
  s = current_sector;
  while(!found) {

    // (re)init sector to 3D camera space

    // initialize object

    s->reset();

    //  #ifdef DELME
    //      s->init_nonculled_list();
    //      s->vertices->num_varying_items = 0;
    //      s->transform_stage = 0;
    //      for(register int ivtx=0; ivtx<s->vertices->num_fixed_items; ivtx++) {
    //        (*(s->vertices))[ivtx].reset();
    //      }

    //      l3d_polygon_3d_node *n;
    //      n = s->nonculled_polygon_nodes;
    //      while(n) {
    //        n->polygon->init_clip_ivertices();
    //        n->polygon->init_transformed();
    //        n=n->next;
    //      }
    //  #endif

    // position all vertices of object into world space

    if (s->num_xforms) {
      s->transform(s->modeling_xform);
    }

    // cull/clip polys in world space

    // transform surviving polys to camera space

    s->camera_transform(camera->viewing_xform);

    //////////////////////

    found=1;
    for(i=0; i<s->polygons.num_items && found; i++) {
      l3d_polygon_3d *p;
      l3d_vector N, facet_to_cam_vec;
      p = s->polygons[i];
      N.set(p->sfcnormal.transformed.X_ - p->center.transformed.X_,
            p->sfcnormal.transformed.Y_ - p->center.transformed.Y_,
            p->sfcnormal.transformed.Z_ - p->center.transformed.Z_,
            int_to_l3d_real(0));

      facet_to_cam_vec.set (0 - p->center.transformed.X_,
                            0 - p->center.transformed.Y_,
                            0.5 - p->center.transformed.Z_,
                            int_to_l3d_real(0));

      /* dot product explicitly formulated so no fixed point overflow */
      /* (facet_to_cam_vec is very large) */
      /* (use of the inline function "dot" uses default fixed precision) */
      // #include "../../math/fix_lowp.h"
      //#define BACKFACE_EPS float_to_l3d_real(0.1)
      //#define FX_CVT(x) (x)
      //      if(( l3d_mulrr(FX_CVT(N.X_), FX_CVT(facet_to_cam_vec.X_)) +
      //	   l3d_mulrr(FX_CVT(N.Y_), FX_CVT(facet_to_cam_vec.Y_)) +
      //	   l3d_mulrr(FX_CVT(N.Z_), FX_CVT(facet_to_cam_vec.Z_)) >BACKFACE_EPS)
      //         /* equivalent to: if(N.dot(facet_to_cam_vec) > 0 ) */
      //	 // #include "../../math/fix_prec.h"
      //	 )
      l3d_real d;
      d=dot(N,facet_to_cam_vec);
      // printf("p%d dot %f\n",i,d);
      if(d>-0.05)
        //         if(dot(N,facet_to_cam_vec) > 0 )

      {
        // polygon is not culled
      } else {
        found = 0;
      }
    }
    if (!found) {

      //      printf("NOT sector %s, fail %d\n",s->name, i);
      // find next portal

      if(s!=current_sector) portal_num++;

      int next_sector_found=0;
      l3d_polygon_3d_portal *por;
      while(portal_num<current_sector->polygons.num_items && !next_sector_found) {
        if ( (por = dynamic_cast<l3d_polygon_3d_portal *>
                    (current_sector->polygons[portal_num])) )
        {
          next_sector_found = 1;
          s = por->connected_sector;
        } else {
          portal_num++;
        }
      }

      if(!next_sector_found) {
        found = 1;// no more other sectors to look at....
        //	printf("in nirvana\n");
      }
    } else {
      current_sector = s;// found new sector
      //      printf("current_sector %s\n",current_sector->name);
    }
  }
}

l3d_world_portal::l3d_world_portal(int xsize, int ysize)
    : l3d_world(xsize, ysize)
{
  l3d_screen_info *si = screen->sinfo;

  camera->VRP.set(0,0,-50,0);
  camera->near_z = float_to_l3d_real(0.5);
  camera->far_z = int_to_l3d_real(5000);

  int i,j,k;

  i=10; j=0; k=20;
  k=0;

}

void l3d_world_portal::draw_all(void) {
  //      l3d_world::draw_all();
  //  screen->clear_screenbuf();
  //    render_sector(current_sector, screen->view_win);
  current_sector->render(screen->view_win,
                         rasterizer, camera,
                         screen->xsize, screen->ysize);

}

l3d_sector *l3d_world_portal::sector_by_name(const char *name) {
  int i;
  int found=0;
  l3d_sector *result = NULL;

  for (i=0; (i<objects.num_items) && (!found); i++) {
    if ( strcmp(objects[i]->name, name) == 0 ) {
      result = dynamic_cast<l3d_sector *> ( objects[i] );
      found=1;
    }
  }

  if(result==NULL) printf("NULL sector %s\n!",name);
  return result;

}

void l3d_world_portal::load_from_file(const char *fname) {
  FILE *fp;

  char line[8000];
  int snum;

  fp = fopen(fname, "rb");
  fgets(line, 800000,fp);
  sscanf(line,"%d",&snum);

  screen->sinfo->ext_to_native(0,0,0); //- allocate black background color

  for(int onum=0; onum<snum; onum++) {
    char sname[80];
    int numv, nump;
    fgets(line, 8000, fp);
    sscanf(line,"SECTOR %s\n", sname);
    //    printf("reading sector %s\n",sname);

    fgets(line, 8000,fp);
    sscanf(line, "%d %d", &numv, &nump);

    int new_onum;
    objects[new_onum=objects.next_index()] = new l3d_sector(numv);
    objects[new_onum]->num_xforms = 0;
    strcpy(objects[new_onum]->name, sname);

    for(int v=0; v<numv; v++) {
      int vn;
      float vx, vy, vz;

      fgets(line, 8000, fp);
      sscanf(line, "%d %f %f %f", &vn, &vx, &vy, &vz);
      (*(objects[new_onum]->vertices))[vn].original.set(float_to_l3d_real(vx),
          float_to_l3d_real(vy),
          float_to_l3d_real(vz),
          int_to_l3d_real(1));
    }

    for(int p=0; p<nump; p++) {
      int p_idx = objects[new_onum]->polygons.next_index();

      char *s;
      fgets(line, 8000, fp);
      s = strtok(line, " ");
      if(strcmp(s,"GEOMPOLY")==0) {
        int num_vtx;
        l3d_polygon_3d_flatshaded *poly;

        s = strtok(NULL, " ");
        sscanf(s, "%d", &num_vtx);
        objects[new_onum]->polygons[p_idx] = poly =
                                               new l3d_polygon_3d_flatshaded(num_vtx);
        objects[new_onum]->polygons[p_idx]->vlist = &(objects[new_onum]->vertices);

        for(int vtx=0; vtx<num_vtx; vtx++) {
          s = strtok(NULL, " ");
          int iv;
          sscanf(s, "%d", &iv);
          (*objects[new_onum]->polygons[p_idx]->ivertices)[
            objects[new_onum]->polygons[p_idx]->ivertices->next_index()].ivertex = iv;

          poly->final_color =
            screen->sinfo->ext_to_native
            (rand() % screen->sinfo->ext_max_red,
             rand() % screen->sinfo->ext_max_green,
             rand() % screen->sinfo->ext_max_blue);
        }

        objects[new_onum]->polygons[p_idx]->compute_center();
        objects[new_onum]->polygons[p_idx]->compute_sfcnormal();
      } else {
        int num_vtx;

        s = strtok(NULL, " ");
        sscanf(s, "%d", &num_vtx);
        objects[new_onum]->polygons[p_idx] = new l3d_polygon_3d_portal(num_vtx);
        objects[new_onum]->polygons[p_idx]->vlist = &(objects[new_onum]->vertices);
        for(int vtx=0; vtx<num_vtx; vtx++) {
          s = strtok(NULL, " ");
          int iv;
          sscanf(s, "%d", &iv);
          (*objects[new_onum]->polygons[p_idx]->ivertices)[
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

  for(int i=0; i<objects.num_items; i++) {
    for(int pi=0; pi<objects[i]->polygons.num_items; pi++) {
      l3d_polygon_3d_portal *p;
      if ((p = dynamic_cast<l3d_polygon_3d_portal *>(objects[i]->polygons[pi]))) {
        p->connected_sector = sector_by_name(p->connected_sector_name);
      }
    }
  }

  screen->refresh_palette();
}
