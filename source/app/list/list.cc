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

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "../lib/datastr/list.h"

class l3d_polygon_ivertex {
  public:
    int ivertex;

    virtual l3d_polygon_ivertex& operator= (const l3d_polygon_ivertex &r) {
      printf("virtual ivertex assignment\n");
      ivertex = r.ivertex;
      return *this;
    }


    virtual ~l3d_polygon_ivertex(void) {printf("iv dest\n");}// destructor MUST be virtual here since
    // l3d_polygon destroys the l3d_polygon_vertex without necessarily
    // knowing what concrete type they are (since l3d_polygon descendants
    // can override create_vertices to create new concrete l3d_polygon_vertex
    // types)
};

class my_factory : public l3d_list_item_factory<l3d_polygon_ivertex> {
    l3d_polygon_ivertex *create(void) {
      printf("my create\n");
      return new l3d_polygon_ivertex;
    };
};

main()  {
  int i;
  my_factory f;

  l3d_two_part_list<l3d_polygon_ivertex> list(5, &f);
  l3d_two_part_list<l3d_polygon_ivertex> list2(1);

  list[0].ivertex = 11;
  list[1].ivertex = 12;
  list[2].ivertex = 13;
  list[3].ivertex = 14;
  list[4].ivertex = 15;

  list.num_varying_items = 0;
  list[list.next_varying_index()].ivertex = 16;
  list[list.next_varying_index()].ivertex = 17;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list.num_varying_items = 0;
  list[list.next_varying_index()].ivertex = 18;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list[list.next_varying_index()].ivertex = 19;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list.num_varying_items = 0;
  list[list.next_varying_index()].ivertex = 20;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list[list.next_varying_index()].ivertex = 21;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list[list.next_varying_index()].ivertex = 22;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list[list.next_varying_index()].ivertex = 23;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list[list.next_varying_index()].ivertex = 24;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list[list.next_varying_index()].ivertex = 25;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list[list.next_varying_index()].ivertex = 26;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list[list.next_varying_index()].ivertex = 27;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list[list.next_varying_index()].ivertex = 28;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list[list.next_varying_index()].ivertex = 29;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }
  list[list.next_varying_index()].ivertex = 30;
  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }

  for(i=0; i<list.num_fixed_items + list.num_varying_items; i++) {
    printf("%d: %d\n", i, list[i].ivertex);
  }


  printf("bout to assign\n");
  list2 = list;
  list = list2;
  list2 = list;
  list = list2;
  list2 = list;
  list = list2;
  list2 = list;
  list = list2;
  list2 = list;
  list = list2;
  list2 = list;
  list = list2;
  list2 = list;
  list = list2;
  printf("after to assign\n");
  for(i=0; i<list2.num_fixed_items + list2.num_varying_items; i++) {
    printf("2: %d: %d\n", i, list[i].ivertex);
  }
}

