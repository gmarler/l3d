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

#include "boundsph.h"
#include "../plane/plane.h"
#include "../object/object3d.h"
#include "../../tool_os/memman.h"

void l3d_bounding_sphere::reset(void) {
  transform_stage = 0;
  center.reset();
}

void l3d_bounding_sphere::transform(const l3d_matrix &xform) {
  transform_stage++;
  center.transform(xform, transform_stage);
}

l3d_real l3d_bounding_sphere::distance_to_plane(const l3d_plane & plane) const
{
  //- ensure the size of the plane's normal vector is 1, otherwise this
  //- computation doesn't work
  assert(sqrt(l3d_real_to_float(l3d_mulrr(plane.a,plane.a)+l3d_mulrr(plane.b,plane.b)+ l3d_mulrr(plane.c,plane.c))) < 1.01 && sqrt(l3d_real_to_float(l3d_mulrr(plane.a,plane.a)+ l3d_mulrr(plane.b,plane.b)+ l3d_mulrr(plane.c,plane.c))) > 0.99);

  return plane.side_of_point(center.transformed);
}

void l3d_bounding_sphere::compute_around_object(const l3d_object *object) {
  int i;
  center.original.set(0,0,0,1);

  l3d_point min, max;
  int first_vtx=1;

  for(i=0; i<object->vertices->num_fixed_items; i++) {
    int vtx_is_referenced=0;
    for(int pi=0; pi<object->polygons.num_items && !vtx_is_referenced;pi++) {
      for(int vi=0; vi<object->polygons[pi]->ivertices->num_items; vi++) {
        if((*object->polygons[pi]->ivertices)[vi].ivertex == i) {
          vtx_is_referenced = 1;
          break;
        }
      }
    }

    if(vtx_is_referenced) {
      if(first_vtx) {
        first_vtx = 0;
        min.X_ = (*object->vertices)[i].original.X_;
        max.X_ = (*object->vertices)[i].original.X_;
        min.Y_ = (*object->vertices)[i].original.Y_;
        max.Y_ = (*object->vertices)[i].original.Y_;
        min.Z_ = (*object->vertices)[i].original.Z_;
        max.Z_ = (*object->vertices)[i].original.Z_;
      } else {
        if ( (*object->vertices)[i].original.X_ < min.X_) {
          min.X_ = (*object->vertices)[i].original.X_;
        }
        if ( (*object->vertices)[i].original.X_ > max.X_) {
          max.X_ = (*object->vertices)[i].original.X_;
        }
        if ( (*object->vertices)[i].original.Y_ < min.Y_) {
          min.Y_ = (*object->vertices)[i].original.Y_;
        }
        if ( (*object->vertices)[i].original.Y_ > max.Y_) {
          max.Y_ = (*object->vertices)[i].original.Y_;
        }
        if ( (*object->vertices)[i].original.Z_ < min.Z_) {
          min.Z_ = (*object->vertices)[i].original.Z_;
        }
        if ( (*object->vertices)[i].original.Z_ > max.Z_) {
          max.Z_ = (*object->vertices)[i].original.Z_;
        }
      }
    }
  }

  center.original.X_ =
    l3d_divri(min.X_ + max.X_, 2);
  center.original.Y_ =
    l3d_divri(min.Y_ + max.Y_, 2);
  center.original.Z_ =
    l3d_divri(min.Z_ + max.Z_, 2);

  radius = int_to_l3d_real(0);
  for(i=0; i<object->vertices->num_fixed_items; i++) {
    int vtx_is_referenced=0;
    for(int pi=0; pi<object->polygons.num_items && !vtx_is_referenced;pi++) {
      for(int vi=0; vi<object->polygons[pi]->ivertices->num_items; vi++) {
        if((*object->polygons[pi]->ivertices)[vi].ivertex == i) {
          vtx_is_referenced = 1;
          break;
        }
      }
    }

    if(vtx_is_referenced) {
      //        printf("rad. comp %d, %f %f %f\n",
      //               i,
      //               (*object->vertices)[i].original.X_,
      //               (*object->vertices)[i].original.Y_,
      //               (*object->vertices)[i].original.Z_);

      l3d_real dist_squared;
      dist_squared = dot((*object->vertices)[i].original - center.original,
                         (*object->vertices)[i].original - center.original);

      if(dist_squared > radius) {
        radius = dist_squared;
      }
    }
  }
  //- the value computed in the above loop was the max squared distance,
  //- now take the square root to retrieve the actual distance
  radius = l3d_sqrt(radius);

  reset(); //- copy original into transformed
}





