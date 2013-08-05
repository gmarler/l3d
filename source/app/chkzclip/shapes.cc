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

#include "shapes.h"

#include <stdlib.h>
#include <string.h>
unsigned char tex_quilt[TEX_HEIGHT][TEX_WIDTH];

Tpyramid::Tpyramid(void) {
  num_vertices = 4;
  vertices[0].set(Float2MT(0.),Float2MT(0.),Float2MT(0.),Float2MT(1.));
  vertices[1].set(Float2MT(1.0),Float2MT(0.),Float2MT(0.),Float2MT(1.));
  vertices[2].set(Float2MT(0.),Float2MT(1.),Float2MT(0.),Float2MT(1.));
  vertices[3].set(Float2MT(0.),Float2MT(0.),Float2MT(1.),Float2MT(1.));

  num_facets = 4;
  facets[0].ivertices[0]=0; facets[0].ivertices[1]=1; facets[0].ivertices[2]=3; facets[0].num_pts=3;
  facets[1].ivertices[0]=2; facets[1].ivertices[1]=3; facets[1].ivertices[2]=1; facets[1].num_pts=3;
  facets[2].ivertices[0]=0; facets[2].ivertices[1]=2; facets[2].ivertices[2]=1;facets[2].num_pts=3;
  facets[3].ivertices[0]=3; facets[3].ivertices[1]=2; facets[3].ivertices[2]=0;facets[3].num_pts=3;
  facet_compute_center(facets[0]);facet_compute_sfcnormal(facets[0]);
  facet_compute_center(facets[1]);facet_compute_sfcnormal(facets[1]);
  facet_compute_center(facets[2]);facet_compute_sfcnormal(facets[2]);
  facet_compute_center(facets[3]);facet_compute_sfcnormal(facets[3]);

  facets[0].icolor = rand() % 8;
  facets[1].icolor = rand() % 8;
  facets[2].icolor = rand() % 8;
  facets[3].icolor = rand() % 8;

  facets[0].is_textured =
    facets[1].is_textured =
      facets[2].is_textured =
        facets[3].is_textured = 0;

  num_xforms = 2;
  xtheta=0;
  modeling_xforms[0] = rotx(xtheta);
  modeling_xforms[1].set
  ( Float2MT(1.), Float2MT(0.), Float2MT(0.), Float2MT(0.),
    Float2MT(0.), Float2MT(1.), Float2MT(0.), Float2MT(0.),
    Float2MT(0.), Float2MT(0.), Float2MT(1.), Float2MT(0.),
    Float2MT(0.), Float2MT(0.), Float2MT(0.), Float2MT(1.) );

  modeling_xform=
    modeling_xforms[1] | modeling_xforms[0];
}

Tpyramid::~Tpyramid(void) { }

int Tpyramid::update(void) {
  xtheta += 10; if (xtheta>359) xtheta-=360;
  //   modeling_xforms[0]=rotx(xtheta);
}

void Ttorus::load(char *fname) {
  FILE *fp;
  char line[256];
  int vnum;
  float x,y,z;
  int v1,v2,v3;

  fp = fopen(fname, "rt");
  if(!fp) return;

  fgets(line, 256, fp);
  if(strncmp(line,"vertices",8)!=0) {
    fprintf(stderr,"file %s: expected keyword 'vertices', got '%s'\n",
            fname, line);
    fclose(fp);
    return;
  }
  num_facets=0; num_vertices=0;

  fgets(line, 256, fp);
  while( (strncmp(line,"faces",5) != 0) && !feof(fp)) {
    sscanf(line, "%d %f %f %f", &vnum, &x, &y, &z);
    vertices[vnum].set(Float2MT(x),Float2MT(y),Float2MT(z),Float2MT(1.));
    fprintf(stderr,"read vertex %d: %f %f %f\n",vnum,x,y,z);
    num_vertices++;
    fgets(line, 256, fp);
  }

  if(strncmp(line,"faces",5)!=0) {
    fprintf(stderr,"file %s: expected keyword 'faces'\n",fname);
    fclose(fp);
    return;
  }

  fgets(line, 256, fp);
  while(!feof(fp)) {
    sscanf(line, "%d %d %d", &v1, &v2, &v3);
    facets[num_facets].ivertices[0] = v1;
    facets[num_facets].ivertices[1] = v2;
    facets[num_facets].ivertices[2] = v3;
    facets[num_facets].num_pts = 3;
    facet_compute_center(facets[num_facets]);
    facet_compute_sfcnormal(facets[num_facets]);
    facets[num_facets].icolor = rand()%8;
    facets[num_facets].is_textured = 0;
    fprintf(stderr,"read face %d: %d %d %d\n",num_facets,v1,v2,v3);
    num_facets++;
    fgets(line, 256, fp);
  }

  fclose(fp);
}

Ttorus::Ttorus(void) {
  load("torus.dat");

  num_xforms = 1;
  modeling_xforms[0].set
  ( Float2MT(1.), Float2MT(0.), Float2MT(0.), Float2MT(0.),
    Float2MT(0.), Float2MT(1.), Float2MT(0.), Float2MT(0.),
    Float2MT(0.), Float2MT(0.), Float2MT(1.), Float2MT(0.),
    Float2MT(0.), Float2MT(0.), Float2MT(0.), Float2MT(1.) );
  modeling_xform = modeling_xforms[0] ;
}

int Ttorus::update(void) {
  return 0;
}

Ttorus::~Ttorus(void) {
}

Tcube::Tcube(void) {
  int i;

  Ttexture *t1, *t2;

  t1 = new Ttexture;
  t1->read_from_ppm("tex.ppm");
  t2 = new Ttexture;
  t2->read_from_ppm("tex2.ppm");
  t2->map_to_palette(t1->_plocal_palette);


  num_vertices = 8;
  vertices[0].set(Float2MT(0.),Float2MT(0.),Float2MT(0.),Float2MT(1.));
  vertices[1].set(Float2MT(1.0),Float2MT(0.),Float2MT(0.),Float2MT(1.));
  vertices[2].set(Float2MT(0.),Float2MT(1.),Float2MT(0.),Float2MT(1.));
  vertices[3].set(Float2MT(1.),Float2MT(1.),Float2MT(0.),Float2MT(1.));
  vertices[4].set(Float2MT(0.),Float2MT(0.),Float2MT(1.),Float2MT(1.));
  vertices[5].set(Float2MT(1.0),Float2MT(0.),Float2MT(1.),Float2MT(1.));
  vertices[6].set(Float2MT(0.),Float2MT(1.),Float2MT(1.),Float2MT(1.));
  vertices[7].set(Float2MT(1.),Float2MT(1.),Float2MT(1.),Float2MT(1.));

  num_facets = 6;
  i=0;
  facets[i].ivertices[3]=0; facets[i].ivertices[2]=2; facets[i].ivertices[1]=3;
  facets[i].ivertices[0]=1; i++;
  facets[i].ivertices[3]=1; facets[i].ivertices[2]=3; facets[i].ivertices[1]=7;
  facets[i].ivertices[0]=5; i++;
  facets[i].ivertices[3]=7; facets[i].ivertices[2]=6; facets[i].ivertices[1]=4;
  facets[i].ivertices[0]=5; i++;
  facets[i].ivertices[3]=6; facets[i].ivertices[2]=2; facets[i].ivertices[1]=0;
  facets[i].ivertices[0]=4; i++;
  facets[i].ivertices[3]=6; facets[i].ivertices[2]=7; facets[i].ivertices[1]=3;
  facets[i].ivertices[0]=2; i++;
  facets[i].ivertices[3]=5; facets[i].ivertices[2]=4; facets[i].ivertices[1]=0;
  facets[i].ivertices[0]=1; i++;

  facet_compute_center(facets[0]);facet_compute_sfcnormal(facets[0]);
  facet_compute_center(facets[1]);facet_compute_sfcnormal(facets[1]);
  facet_compute_center(facets[2]);facet_compute_sfcnormal(facets[2]);
  facet_compute_center(facets[3]);facet_compute_sfcnormal(facets[3]);
  facet_compute_center(facets[4]);facet_compute_sfcnormal(facets[4]);
  facet_compute_center(facets[5]);facet_compute_sfcnormal(facets[5]);

  facets[0].icolor = rand() % 8;
  facets[1].icolor = rand() % 8;
  facets[2].icolor = rand() % 8;
  facets[3].icolor = rand() % 8;
  facets[5].icolor = rand() % 8;
  facets[5].icolor = rand() % 8;

  facets[0].num_pts =
    facets[1].num_pts =
      facets[2].num_pts =
        facets[3].num_pts =
          facets[4].num_pts =
            facets[5].num_pts = 4;

  facets[0].is_textured =
    facets[1].is_textured =
      facets[2].is_textured =
        facets[3].is_textured =
          facets[4].is_textured =
            facets[5].is_textured = 1;

  for(i=0; i<6; i++) {
    facets[i].O = vertices[facets[i].ivertices[1]];
    //      facets[i].O.set(0,0,0, 1);
    //      facets[i].U.set(facets[i].O.X_+1, facets[i].O.Y_, facets[i].O.Z_, 1.0);
    facets[i].U = vertices[facets[i].ivertices[2]]; // - facets[i].O;
    /* .set(
            vertices[facets[i].ivertices[1]].X_,
            vertices[facets[i].ivertices[1]].Y_,
            vertices[facets[i].ivertices[1]].Z_,
            1.0);
    */
    //      facets[i].V.set(facets[i].O.X_, facets[i].O.Y_+1, facets[i].O.Z_, 1.0);
    facets[i].V = vertices[facets[i].ivertices[0]]; //  - facets[i].O;
    /*.set(
             vertices[facets[i].ivertices[3]].X_,
             vertices[facets[i].ivertices[3]].Y_,
             vertices[facets[i].ivertices[3]].Z_,
             1.0);
    */
    //   facets[i].texmap = &(tex_quilt);
    facets[i].texmap = t1;

    /*
    for(int ii=0; ii<64; ii++) {
       for(int jj=0; jj<64; jj++) {
          facets[i].texmap[jj][ii] = (rand()%8)*16+15;
          printf("%d,%d:%d facet\n", ii,jj,facets[i].texmap[jj][ii]);
  }
  }
    */
  }

  num_xforms = 2;
  modeling_xforms[0].set
  ( Float2MT(15.), Float2MT(0.), Float2MT(0.), Float2MT(0.),
    Float2MT(0.), Float2MT(15.), Float2MT(0.), Float2MT(0.),
    Float2MT(0.), Float2MT(0.), Float2MT(15.), Float2MT(0.),
    Float2MT(0.), Float2MT(0.), Float2MT(0.), Float2MT(1.) );
  modeling_xforms[1].set
  ( Float2MT(1.), Float2MT(0.), Float2MT(0.), Float2MT(0.),
    Float2MT(0.), Float2MT(1.), Float2MT(0.), Float2MT(0.),
    Float2MT(0.), Float2MT(0.), Float2MT(1.), Float2MT(0.),
    Float2MT(0.), Float2MT(0.), Float2MT(0.), Float2MT(1.) );

  modeling_xform=
    modeling_xforms[1] | modeling_xforms[0];
}

Tcube::~Tcube(void) {
}

int Tcube::update(void) {

  return 0;
  if(!xrotating && !yrotating && !zrotating ) {
    xgoal=(rand()%35)*10;
    ygoal=(rand()%35)*10;
    zgoal=(rand()%35)*10;
    xrotating=rand()%2;
    yrotating=1-xrotating;
    zrotating=rand()%2;
  }

  if(xrotating) {
    xtheta+=10; if(xtheta>359) xtheta-=360;
    if(xtheta-xgoal < 10) {
      xrotating=rand()%2;
      if(xrotating) {
        xgoal=(rand()%35)*10;
      }
    }
  }

  if(yrotating) {
    ytheta+=10; if(ytheta>359) ytheta-=360;
    if(ytheta-ygoal<10) {
      yrotating=rand()%2;
      if(yrotating) {
        ygoal=(rand()%35)*10;
      }
    }
  }

  if(zrotating) {
    ztheta+=10; if(ztheta>359) ztheta-=360;
    if(ztheta-zgoal < 10) {
      zrotating=rand()%2;
      if(zrotating) {
        zgoal=(rand()%35)*10;
      }
    }
  }

  modeling_xforms[0]=rotx(xtheta) | (roty(ytheta) | rotz(ztheta));
  modeling_xform=
    modeling_xforms[2] | (modeling_xforms[1] |
                          modeling_xforms[0]);
  ;
}

Torbitcube::Torbitcube(void) {
  xorbit=yorbit=zorbit=0;
  xorbiting=rand()%2; yorbiting=rand()%2; zorbiting=rand()%2;
  xogoal=(rand()%35)*10; yogoal=(rand()%35)*10; zorbiting=(rand()%35)*10;
  num_xforms = 5;

  modeling_xforms[2].set
  ( Float2MT(1.), Float2MT(0.), Float2MT(0.), Float2MT(rand()%3+1.),
    Float2MT(0.), Float2MT(1.), Float2MT(0.), Float2MT(rand()%3+1.),
    Float2MT(0.), Float2MT(0.), Float2MT(1.), Float2MT(rand()%3+1.),
    Float2MT(0.), Float2MT(0.), Float2MT(0.), Float2MT(1.) );
  modeling_xforms[3] = rotx(xorbit) | (roty(yorbit) | rotz(zorbit));
  modeling_xform =
    modeling_xforms[2] | (modeling_xforms[1] |
                          modeling_xforms[0])
    ;
}

Torbitcube::~Torbitcube(void) { }

int Torbitcube::update(void) {
  Tcube::update();

  if(!xorbiting && !yorbiting && !zorbiting ) {
    xogoal=(rand()%35)*10;
    yogoal=(rand()%35)*10;
    zogoal=(rand()%35)*10;
    xorbiting=rand()%2;
    yorbiting=1-xorbiting;
    zorbiting=rand()%2;
  }

  if(xorbiting) {
    xorbit+=10; if(xorbit>359) xorbit-=360;
    if(xorbit-xogoal < 10) {
      xorbiting=rand()%2;
      if(xorbiting) {
        xogoal=(rand()%35)*10;
      }
    }
  }

  if(yorbiting) {
    yorbit+=10; if(yorbit>359) yorbit-=360;
    if(yorbit-yogoal<10) {
      yorbiting=rand()%2;
      if(yorbiting) {
        yogoal=(rand()%35)*10;
      }
    }
  }

  if(zorbiting) {
    zorbit+=10; if(zorbit>359) zorbit-=360;
    if(zorbit-zogoal < 10) {
      zorbiting=rand()%2;
      if(zorbiting) {
        zogoal=(rand()%35)*10;
      }
    }
  }

  modeling_xforms[3]=rotx(xorbit) | (roty(yorbit) | rotz(zorbit));
  modeling_xform =
    modeling_xforms[4] | (
      modeling_xforms[3] | (modeling_xforms[2] | (modeling_xforms[1] |
                            modeling_xforms[0]) ))
    ;
}

Tmountains::Tmountains(void) {

  int i,j,v,f;

  Ttexture *t1, *t2;

  t1 = new Ttexture;
  t1->read_from_ppm("tex.ppm");
  t2 = new Ttexture;
  t2->read_from_ppm("tex2.ppm");
  t2->map_to_palette(t1->_plocal_palette);

  num_vertices = (TRIANGLES_PER_STRIP/2 + 1) * (NUM_STRIPS+1);
  num_facets = TRIANGLES_PER_STRIP * NUM_STRIPS;

  v = 0;

  // Assign vertices:
  for(i=0; i<NUM_STRIPS+1; i++) {
    for(j=0; j<TRIANGLES_PER_STRIP/2 + 1; j++) {
      /* randoM:       vertices[v].set(Int2MT(j*10), Int2MT(-14+rand()%12), Int2MT(i*10), Int2MT(1)); */
      /* flat */         vertices[v].set(Int2MT(j*10), Int2MT(-14), Int2MT(i*10), Int2MT(1));
      //         if (rand()%2) { deltay[v] = Int2MT(1); } else { deltay[v] = Int2MT(-1); }
      deltay[v] = Float2MT( (10.-rand()%20)/10. );
      //       vertices[v++].set(Int2MT(j*100), Int2MT(i*100), Int2MT(0), Int2MT(1));
      v++;
    }
  }

  f = 0;

  // Assign facets:
  for(i=0; i<NUM_STRIPS; i++) {
    for(j=0; j<TRIANGLES_PER_STRIP; j++) {
      int sqnum = j / 2;

      if (j%2==1) {
        facets[f].ivertices[0] = sqnum + i*(TRIANGLES_PER_STRIP/2+1);
        facets[f].ivertices[1] = sqnum + (TRIANGLES_PER_STRIP/2+1) + 1 + i*(TRIANGLES_PER_STRIP/2+1);
        facets[f].ivertices[2] = sqnum + 1 + i*(TRIANGLES_PER_STRIP/2+1);
      } else {
        facets[f].ivertices[0] = sqnum + TRIANGLES_PER_STRIP/2+1 + i*(TRIANGLES_PER_STRIP/2+1);
        facets[f].ivertices[1] = sqnum + TRIANGLES_PER_STRIP/2+1 + 1 + i*(TRIANGLES_PER_STRIP/2+1);
        facets[f].ivertices[2] = sqnum + i*(TRIANGLES_PER_STRIP/2+1);
      }

      facet_compute_center(facets[f]);facet_compute_sfcnormal(facets[f]);
      facets[f].icolor = 3;
      facets[f].is_textured = 1;
      //   facets[f].texmap = &(tex_quilt);
      facets[f].texmap = t1;
      facets[f].tex_vertices[0].set(0,TEX_HEIGHT -1,0,0);
      facets[f].tex_vertices[1].set(TEX_WIDTH -1,0,0,0);
      facets[f].tex_vertices[2].set(TEX_WIDTH -1,TEX_HEIGHT -1,0,0);
      facets[f].num_pts = 3;
      f++;
    }
  }

  num_xforms = 1;

  modeling_xforms[0].set
  ( Float2MT(1.), Float2MT(0.), Float2MT(0.), Float2MT(0.),
    Float2MT(0.), Float2MT(1.), Float2MT(0.), Float2MT(0.),
    Float2MT(0.), Float2MT(0.), Float2MT(1.), Float2MT(0.),
    Float2MT(0.), Float2MT(0.), Float2MT(0.), Float2MT(1.) );
  modeling_xform = modeling_xforms[0];
}

int Tmountains::update(void) {
  int i,j,v;

  return;

  v = 0;
  // Assign vertices:
  for(i=0; i<NUM_STRIPS+1; i++) {
    for(j=0; j<TRIANGLES_PER_STRIP/2 + 1; j++) {
      //printf("updating mtn %d\n",v);
      vertices[v].Y_ = vertices[v].Y_ + deltay[v];
      if( (vertices[v].Y_ < Int2MT(-14)) || (vertices[v].Y_ > Int2MT(-2)) ) {
        deltay[v] = -deltay[v];
      }
      v++;
    }
  }

  for(i=0;i<NUM_STRIPS * TRIANGLES_PER_STRIP; i++) {
    facet_compute_center(facets[i]);
    facet_compute_sfcnormal(facets[i]);


  }

}
// non-member


void init_textures(void) {
  int i,j,ii,jj, col;
  col = 15;
  for(i=0; i<TEX_WIDTH; i+=1) {
    for(j=0;j<TEX_HEIGHT;j+=1) {
      tex_quilt[j][i]=(rand()%7) * 16 + 15;
      printf("%d,%d: %d\n", i, j, tex_quilt[j][i]);
    }
  }
}

/*
void init_textures(void) {
   int i,j,ii,jj, col;
   col = 0;
   for(i=0; i<TEX_WIDTH; i+=4) {
      for(j=0;j<TEX_HEIGHT;j+=4,col+=1) {
         for(ii=i; ii<i+4; ii++) {
            for(jj=j; jj<j+4; jj++) {
               tex_quilt[jj][ii]=(rand()%8) * 16 + 15;
            }
         }
      }
   }
}
*/
