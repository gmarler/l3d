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

#include <unistd.h>
#include <stdio.h>
#include <vector>
#include <string>

struct vertex {
  float x,y,z;

  int operator==(const vertex &r) {
    return(
            -0.001 < x-r.x && x-r.x < 0.001 &&
            -0.001 < y-r.y && y-r.y < 0.001 &&
            -0.001 < z-r.z && z-r.z < 0.001
          );
  }
};

main(int argc, char **argv)
{

  if(argc != 2) {
    printf("usage: %s videoscape_file.obj\n", argv[0]);
    exit(-1);
  }
  FILE *fp;

  unsigned long oid;

  std::vector<vertex> vidscape_vertices;

  // find the videoscape file we just created

  char vidscape_line[1024];

  float float1, float2, float3;
  int found=0;

  char blender_work_fname[1024];
  sprintf(blender_work_fname,argv[1]);
  fp = fopen(blender_work_fname, "rt");
  int lineno = 1;
  int num_vertices=0;

  float xsum=0.0, ysum=0.0, zsum=0.0;
  float xc,yc,zc;
  float x0_0,y0_0,z0_0  ,  x0_1,y0_1,z0_1,
  x1_0,y1_0,z1_0  ,  x1_1,y1_1,z1_1,
  x2_0,y2_0,z2_0  ,  x2_1,y2_1,z2_1;

  int prev_v_vtxcount=0, prev_v_vtx0_0, prev_v_vtx0_1;
  int overlapping_edge_count=0;

  vertex pos; // position of object
  vertex x_axis, y_axis, z_axis; // orientation of object (vectors)

  while(!feof(fp) && !found) {
    fgets(vidscape_line, 1024, fp);
    if(!feof(fp)) {

#define VIDSC_VTXCOUNT_LINENO 2

      if(lineno>VIDSC_VTXCOUNT_LINENO &&
          lineno<=VIDSC_VTXCOUNT_LINENO + num_vertices)
      {
        vertex v;
        sscanf(vidscape_line, "%f %f %f", &float1,&float2,&float3);

        v.x =float1;
        v.y =float2;
        v.z =float3;
        vidscape_vertices.push_back(v);
      }
      if(lineno == VIDSC_VTXCOUNT_LINENO) {

        // this edge contains the id
        sscanf(vidscape_line, "%d", &num_vertices);
      }



      int sc;
      int v_vtxcount=0, v_vtx0_0=0, v_vtx0_1=0;
      unsigned long v_color;
      char v_color_str[255];
      if((lineno > VIDSC_VTXCOUNT_LINENO + num_vertices + 1)
          && (sc=sscanf(vidscape_line,"%d %d %d %s",
                        &v_vtxcount, &v_vtx0_0, &v_vtx0_1, v_color_str)==4)
          && (v_vtxcount==2)
        ) // could also check dist. betw vertices
      {
        // we found a 2-vertex edge. does it overlap with the previuos one?
        // if so add it to the count, if not, re-init the count.

        if(prev_v_vtxcount==2
            &&((vidscape_vertices[v_vtx0_0]==vidscape_vertices[prev_v_vtx0_0]
                && vidscape_vertices[v_vtx0_1]==vidscape_vertices[prev_v_vtx0_1])
               ||(vidscape_vertices[v_vtx0_0]==vidscape_vertices[prev_v_vtx0_1]
                  && vidscape_vertices[v_vtx0_1]==vidscape_vertices[prev_v_vtx0_0]))
          ) {
          overlapping_edge_count++;
        } else {
          overlapping_edge_count=1;
        }

        prev_v_vtxcount = v_vtxcount;
        prev_v_vtx0_0 = v_vtx0_0;
        prev_v_vtx0_1 = v_vtx0_1;
      }

      if(overlapping_edge_count==4)
      {
        // we found the start-of-transmission flag (4 overlapping vertices)

        int x_v0, x_v1,
        y_v0, y_v1,
        z_v0, z_v1;

        // read x y and z axis orientations
        fgets(vidscape_line, 1024, fp);
        sscanf(vidscape_line, "%d %d %d", &v_vtxcount, &x_v0, &x_v1);
        fgets(vidscape_line, 1024, fp);
        sscanf(vidscape_line, "%d %d %d", &v_vtxcount, &y_v0, &y_v1);
        fgets(vidscape_line, 1024, fp);
        sscanf(vidscape_line, "%d %d %d", &v_vtxcount, &z_v0, &z_v1);

        // extract position and orientation
        vertex x0,x1,y0,y1,z0,z1;
        if ( vidscape_vertices[x_v0] == vidscape_vertices[y_v0] ) {
          // the intersection point of the axes is the origin
          x0 = vidscape_vertices[x_v0];
          y0 = vidscape_vertices[y_v0];
          x1 = vidscape_vertices[x_v1];
          y1 = vidscape_vertices[y_v1];
        } else if(vidscape_vertices[x_v0] == vidscape_vertices[y_v1]) {
          x0 = vidscape_vertices[x_v0];
          y0 = vidscape_vertices[y_v1];
          x1 = vidscape_vertices[x_v1];
          y1 = vidscape_vertices[y_v0];
        } else if(vidscape_vertices[x_v1] == vidscape_vertices[y_v0]) {
          x0 = vidscape_vertices[x_v1];
          y0 = vidscape_vertices[y_v0];
          x1 = vidscape_vertices[x_v0];
          y1 = vidscape_vertices[y_v1];
        } else if(vidscape_vertices[x_v1] == vidscape_vertices[y_v1]) {
          x0 = vidscape_vertices[x_v1];
          y0 = vidscape_vertices[y_v1];
          x1 = vidscape_vertices[x_v0];
          y1 = vidscape_vertices[y_v0];
        } else {
          fprintf(stderr,"fatal error: origin not found\n");
        }

        if ( vidscape_vertices[x_v0] == vidscape_vertices[z_v0] ) {
          // the intersection point of the axes is the origin
          z0 = vidscape_vertices[z_v0];
          z1 = vidscape_vertices[z_v1];
        } else if(vidscape_vertices[x_v0] == vidscape_vertices[z_v1]) {
          z0 = vidscape_vertices[z_v1];
          z1 = vidscape_vertices[z_v0];
        } else if(vidscape_vertices[x_v1] == vidscape_vertices[z_v0]) {
          z0 = vidscape_vertices[z_v0];
          z1 = vidscape_vertices[z_v1];
        } else if(vidscape_vertices[x_v1] == vidscape_vertices[z_v1]) {
          z0 = vidscape_vertices[z_v1];
          z1 = vidscape_vertices[z_v0];
        } else {
          fprintf(stderr,"fatal error: origin not found\n");
        }

        assert(x0==y0 && x0==z0);
        pos = x0;
        x_axis.x = x1.x - x0.x;
        x_axis.y = x1.y - x0.y;
        x_axis.z = x1.z - x0.z;
        y_axis.x = y1.x - x0.x;
        y_axis.y = y1.y - x0.y;
        y_axis.z = y1.z - x0.z;
        z_axis.x = z1.x - x0.x;
        z_axis.y = z1.y - x0.y;
        z_axis.z = z1.z - x0.z;

        // read in (discard) the x-marker for the x-axis
        int dummy1,dummy2,dummy3;
        fgets(vidscape_line, 1024, fp);
        sscanf(vidscape_line, "%d %d %d", &dummy1, &dummy2, &dummy3);
        fgets(vidscape_line, 1024, fp);
        sscanf(vidscape_line, "%d %d %d", &dummy1, &dummy2, &dummy3);

        found = 1;
        int done = 0;
        int shift=0;
        oid = 0;
        while(!feof(fp) && !done) {
          fgets(vidscape_line, 1024, fp);
          if(!feof(fp)) {
            sscanf(vidscape_line,
                   "%d %d %d",
                   &v_vtxcount, &v_vtx0_0, &v_vtx0_1);
            if(v_vtxcount != 2) {
              done=1;
            } else {
              // get the next bit of the oid



              // was this a "0-bit" edge (x), or a "1-bit" edge (y/z)?
              int bit = !
                        (((vidscape_vertices[v_vtx0_0].x==
                           vidscape_vertices[x_v0].x &&
                           vidscape_vertices[v_vtx0_0].y==
                           vidscape_vertices[x_v0].y &&
                           vidscape_vertices[v_vtx0_0].z==
                           vidscape_vertices[x_v0].z)
                          &&
                          (vidscape_vertices[v_vtx0_1].x==
                           vidscape_vertices[x_v1].x &&
                           vidscape_vertices[v_vtx0_1].y==
                           vidscape_vertices[x_v1].y &&
                           vidscape_vertices[v_vtx0_1].z==
                           vidscape_vertices[x_v1].z))
                         ||
                         ((vidscape_vertices[v_vtx0_0].x==
                           vidscape_vertices[x_v1].x &&
                           vidscape_vertices[v_vtx0_0].y==
                           vidscape_vertices[x_v1].y &&
                           vidscape_vertices[v_vtx0_0].z==
                           vidscape_vertices[x_v1].z)
                          &&
                          (vidscape_vertices[v_vtx0_1].x==
                           vidscape_vertices[x_v0].x &&
                           vidscape_vertices[v_vtx0_1].y==
                           vidscape_vertices[x_v0].y &&
                           vidscape_vertices[v_vtx0_1].z==
                           vidscape_vertices[x_v0].z)));
              oid |= bit<<shift;
              shift++;
              //printf("building oid %d\n",oid&0x1);
              //printf("building oid after %lu\n",oid);
            }
          }
        }
        printf("%lu\n", oid);
      }

      lineno++;
    }
  }
  fclose(fp);

  if(found) {
    printf("%f %f %f\n", pos.x, pos.y, pos.z); // position
    printf("%f %f %f\n", x_axis.x, y_axis.x, z_axis.x); // orientation
    printf("%f %f %f\n", x_axis.y, y_axis.y, z_axis.y); // orientation
    printf("%f %f %f\n", x_axis.z, y_axis.z, z_axis.z); // orientation
  }
}

