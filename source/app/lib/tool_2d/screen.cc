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
#include "screen.h"
#include "../tool_os/memman.h"

l3d_screen::l3d_screen(int xsize, int ysize) {

  this->xsize = xsize; this->ysize = ysize;

  //- initialize view-window to a rectangle

  vwin_vlist = new l3d_two_part_list<l3d_coordinate> ( 4 );

#define BORDER 3
  (*vwin_vlist)[0].transformed.set(int_to_l3d_real(BORDER),
                                   int_to_l3d_real(BORDER),
                                   int_to_l3d_real(0),
                                   int_to_l3d_real(0));
  (*vwin_vlist)[1].transformed.set(int_to_l3d_real(xsize-BORDER),
                                   int_to_l3d_real(BORDER),
                                   int_to_l3d_real(0),
                                   int_to_l3d_real(0));
  (*vwin_vlist)[2].transformed.set(int_to_l3d_real(xsize-BORDER),
                                   int_to_l3d_real(ysize-BORDER),
                                   int_to_l3d_real(0),
                                   int_to_l3d_real(0));
  (*vwin_vlist)[3].transformed.set(int_to_l3d_real(BORDER),
                                   int_to_l3d_real(ysize-BORDER),
                                   int_to_l3d_real(0),
                                   int_to_l3d_real(0));
  view_win = new l3d_polygon_2d(4);
  view_win->vlist = &vwin_vlist;

  (*(view_win->ivertices)) [view_win->ivertices->next_index()].ivertex = 0;
  (*(view_win->ivertices)) [view_win->ivertices->next_index()].ivertex = 1;
  (*(view_win->ivertices)) [view_win->ivertices->next_index()].ivertex = 2;
  (*(view_win->ivertices)) [view_win->ivertices->next_index()].ivertex = 3;
  view_win->init_clip_ivertices();
}

l3d_screen::~l3d_screen(void) {

  //- Close the physical screen
  close_screen();

  delete vwin_vlist;
  delete view_win;
}
