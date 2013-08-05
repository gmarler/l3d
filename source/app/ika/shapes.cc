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

pyramid::pyramid(l3d_two_part_list<l3d_coordinate> *keyframe0,
                 l3d_two_part_list<l3d_coordinate> *keyframe1)
    :
    l3d_object(100)
{
  keyframes[0] = keyframe0;
  keyframes[1] = keyframe1;

  currently_interpolating = false;
  keyframe_no=0;
}

pyramid::~pyramid(void) {
  //- set vertices pointer to NULL to prevent parent l3d_object from trying
  //- to delete these vertices, because they do not belong to us
  vertices = NULL;
  // polygons deleted by parent class l3d_object
  //  for(register int i=0; i<polygons.num_items; i++) { delete polygons[i]; }
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
                  30, 3);

    currently_interpolating = true;
  }
  //send nextframe

  //- it is important to recalculate the normals because the mesa
  //- texture mapping reverse projection relies on correct normals!
  int i;
  for(i=0; i<polygons.num_items; i++) {
    polygons[i]->compute_center();
    polygons[i]->compute_sfcnormal();
  }
}


