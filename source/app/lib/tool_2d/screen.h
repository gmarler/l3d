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


#ifndef __SCREEN_H
#define __SCREEN_H
#include "../tool_os/memman.h"

#include "../geom/polygon/polygon.h"
#include "si_rgb.h"
#include "si_idx.h"
#include "../tool_os/ev_src.h"

class l3d_screen : public l3d_event_source {
  protected:
  virtual void wait_vtrace(void) {} ;
    virtual void close_screen(void) {};
    l3d_two_part_list<l3d_coordinate> *vwin_vlist;
    int bufsize;
  public:
    l3d_screen(int xsize, int ysize);
    virtual ~l3d_screen(void);
    int xsize, ysize;
    l3d_polygon_2d *view_win;
    l3d_screen_info *sinfo;

    virtual void refresh_palette(void) { };
    virtual void open_screen(void)=0;
    virtual void blit_screen(void)=0;
};

class l3d_screen_factory {
  public:
    virtual l3d_screen *create(int xsize, int ysize)=0;
};

#endif
