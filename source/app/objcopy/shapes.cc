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

#include "../lib/geom/polygon/p3_cflat.h"
#include <stdlib.h>
#include <string.h>

pyramid::pyramid(void) :
    l3d_object(4),
    l3d_object_clippable(4)
{
  (*vertices)[0].original.set(float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(1.));
  (*vertices)[1].original.set(float_to_l3d_real(10.0),float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(1.));
  (*vertices)[2].original.set(float_to_l3d_real(0.),float_to_l3d_real(10.),float_to_l3d_real(0.),float_to_l3d_real(1.));
  (*vertices)[3].original.set(float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(10.),float_to_l3d_real(1.));

  int pi;
  pi = polygons.next_index();
  polygons[pi] = new l3d_polygon_3d_flatshaded_clippable(3);
  //   printf("before: %p\n", polygons[pi]->vlist);
  polygons[pi]->vlist = &vertices;
  //   printf("after: %p\n", polygons[pi]->vlist);
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=0;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=1;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=3;
  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();

  pi = polygons.next_index();
  polygons[pi] = new l3d_polygon_3d_flatshaded_clippable(3);
  polygons[pi]->vlist = &vertices;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=2;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=3;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=1;
  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();

  pi = polygons.next_index();
  polygons[pi] = new l3d_polygon_3d_flatshaded_clippable(3);
  polygons[pi]->vlist = &vertices;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=0;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=2;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=1;
  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();

  pi = polygons.next_index();
  polygons[pi] = new l3d_polygon_3d_flatshaded_clippable(3);
  polygons[pi]->vlist = &vertices;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=3;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=2;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=0;
  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();

  num_xforms = 1;
  x=int_to_l3d_real(0);
  dx=int_to_l3d_real(0);
  modeling_xforms[0] = l3d_mat_translate
                       (dx,
                        int_to_l3d_real(0),
                        int_to_l3d_real(0));
  modeling_xform=
    modeling_xforms[0];
}

pyramid::~pyramid(void) {
  // polys deleted by parent class l3d_object
  //  for(register int i=0; i<polygons.num_items; i++) { delete polygons[i]; }
}

int pyramid::update(void) {
  x += dx;
  if(x<=minx || x>=maxx) {
    dx = -dx;
  }
  modeling_xforms[0]=l3d_mat_translate
                     (x,
                      int_to_l3d_real(0),
                      int_to_l3d_real(0));
  modeling_xform=
    modeling_xforms[0];
}

