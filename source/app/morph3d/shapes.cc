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

pyramid::pyramid(void) :
    l3d_object(4)
{
  //sbegin keydef
  keyframes[0] = new l3d_two_part_list<l3d_coordinate>(4);
  (*keyframes[0])[0].original.set
  (float_to_l3d_real(0.),
   float_to_l3d_real(0.),
   float_to_l3d_real(0.),
   float_to_l3d_real(1.));
  (*keyframes[0])[1].original.set
  (float_to_l3d_real(10.0),
   float_to_l3d_real(0.),
   float_to_l3d_real(0.),
   float_to_l3d_real(1.));
  (*keyframes[0])[2].original.set
  (float_to_l3d_real(0.),
   float_to_l3d_real(10.),
   float_to_l3d_real(0.),
   float_to_l3d_real(1.));
  (*keyframes[0])[3].original.set
  (float_to_l3d_real(0.),
   float_to_l3d_real(0.),
   float_to_l3d_real(10.),
   float_to_l3d_real(1.));

  keyframes[1] = new l3d_two_part_list<l3d_coordinate>(4);
  (*keyframes[1])[0].original.set
  (float_to_l3d_real(5.),
   float_to_l3d_real(5.),
   float_to_l3d_real(5.),
   float_to_l3d_real(1.));
  (*keyframes[1])[1].original.set
  (float_to_l3d_real(-10.),
   float_to_l3d_real(10.),
   float_to_l3d_real(0.),
   float_to_l3d_real(1.));
  (*keyframes[1])[2].original.set
  (float_to_l3d_real(10.),
   float_to_l3d_real(0.),
   float_to_l3d_real(0.),
   float_to_l3d_real(1.));
  (*keyframes[1])[3].original.set
  (float_to_l3d_real(10.0),
   float_to_l3d_real(15.0),
   float_to_l3d_real(0.),
   float_to_l3d_real(1.));
  //send keydef

  int pi;
  pi = polygons.next_index();
  polygons[pi] = new l3d_polygon_3d_flatshaded(3);
  printf("before: %p\n", polygons[pi]->vlist);
  polygons[pi]->vlist = &vertices;
  printf("after: %p\n", polygons[pi]->vlist);
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=0;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=1;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=3;
  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();

  pi = polygons.next_index();
  polygons[pi] = new l3d_polygon_3d_flatshaded(3);
  polygons[pi]->vlist = &vertices;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=2;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=3;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=1;
  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();

  pi = polygons.next_index();
  polygons[pi] = new l3d_polygon_3d_flatshaded(3);
  polygons[pi]->vlist = &vertices;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=0;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=2;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=1;
  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();

  pi = polygons.next_index();
  polygons[pi] = new l3d_polygon_3d_flatshaded(3);
  polygons[pi]->vlist = &vertices;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=3;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=2;
  (*polygons[pi]->ivertices)[polygons[pi]->ivertices->next_index()].ivertex=0;
  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();

  num_xforms = 2;
  modeling_xforms[0] = l3d_mat_rotx(0);
  modeling_xforms[1].set
  ( float_to_l3d_real(1.), float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(0.),
    float_to_l3d_real(0.), float_to_l3d_real(1.), float_to_l3d_real(0.), float_to_l3d_real(0.),
    float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(1.), float_to_l3d_real(0.),
    float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(1.) );

  modeling_xform=
    modeling_xforms[1] | modeling_xforms[0];

  currently_interpolating = false;
  keyframe_no=0;
  vertices = keyframes[keyframe_no];
}

pyramid::~pyramid(void) {
  for(register int i=0; i<polygons.num_items; i++) { delete polygons[i]; }
  for(int i=0; i<num_keyframes; i++) {
    delete keyframes[i];
  }
}

int pyramid::update(void) {
  //sbegin interplist
  if(currently_interpolating) {
    vertices = interp.list;
    if(! interp.step()) {
      currently_interpolating = false;
    }
    //send interplist
    //sbegin nextframe
  } else {
    keyframe_no++;
    if(keyframe_no >= num_keyframes) { keyframe_no = 0; }
    int next_keyframe = keyframe_no + 1;
    if(next_keyframe >= num_keyframes) { next_keyframe = 0; }

    vertices = keyframes[keyframe_no];
    interp.start( *keyframes[keyframe_no], *keyframes[next_keyframe],
                  rand()%100 + 50, 3);

    currently_interpolating = true;
  }
  //send nextframe
}


