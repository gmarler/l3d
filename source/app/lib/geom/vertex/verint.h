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

#include "system/sys_dep.h"
#include "datastr/list.h"
#include "geom/vertex/coord.h"
#include "../../tool_os/memman.h"

class l3d_vertex_interpolator {
  public:
    int steps;
    int pos;
    int dimension;

    l3d_two_part_list<l3d_coordinate> *list;

    l3d_list<l3d_vector> *deltas;

    l3d_vertex_interpolator(void) {
      list = new l3d_two_part_list<l3d_coordinate> ( 1 );
      deltas = new l3d_list<l3d_vector> ( 1 );
    }

    ~l3d_vertex_interpolator(void) {
      delete list;
      delete deltas;
    }

    void start(const l3d_two_part_list<l3d_coordinate> & source,
               const l3d_two_part_list<l3d_coordinate> & dest,
               int steps, int dimension)
    {
      this->steps = steps;
      this->dimension = dimension;

      *list = source;

      deltas->num_items = 0;

      int i,j;
      for(i=0; i<list->num_fixed_items; i++) {
        for(j=0; j<dimension; j++) {
          deltas->next_index();
          (*deltas)[i].a[j] =
            l3d_divri(dest[i].original.a[j] - source[i].original.a[j],
                      steps);
        }
      }

      pos = 0;
    }

    bool step(void) {
      int i,j;
      for(i=0; i<list->num_fixed_items; i++) {
        for(j=0; j<dimension; j++) {
          (*list)[i].original.a[j] += (*deltas)[i].a[j];
        }
      }

      pos++;
      return pos < steps;
    }

};
