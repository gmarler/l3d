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

#ifndef __RAS_SW_H
#define __RAS_SW_H
#include "../tool_os/memman.h"

#include "rasteriz.h"
#include "math.h"
#include "../system/sys_dep.h"

class l3d_rasterizer_2d_sw_imp :
      virtual public l3d_rasterizer_2d_imp
{
  protected:
    unsigned char *address_of_point(int x, int y);
    void draw_point_at_paddress(unsigned char **a, unsigned long col);

  public:
    l3d_rasterizer_2d_sw_imp(int xs, int ys, l3d_screen_info *si);
    void clear_buffer(void);
    void draw_point(int x, int y, unsigned long col);
    void draw_line(int x0, int y0, int x1, int y1, unsigned long col);
    void draw_polygon_flatshaded(const l3d_polygon_2d_flatshaded *p_poly);
};


class l3d_rasterizer_2d_sw_imp_factory :
      public l3d_rasterizer_2d_imp_factory
{
  public:
    l3d_rasterizer_2d_imp *create(int xs, int ys, l3d_screen_info *si); // virt
};

// inline functions
//sbegin dotadr
inline unsigned char *l3d_rasterizer_2d_sw_imp::address_of_point(int x, int y)
{
  return sinfo->p_screenbuf +(x + (y*xsize)) * sinfo->bytes_per_pixel;
}
//send dotadr

//sbegin drawpoint_adr
inline void l3d_rasterizer_2d_sw_imp::draw_point_at_paddress
(unsigned char **p, unsigned long col)
{
  register int i;
  unsigned long mask = MAX_BYTE;
  char shift = 0;

  // stuff the bytes in the unsigned long col into the screen buffer, in
  // little-endian order
  for(i=0; i<sinfo->bytes_per_pixel; i++) {
    **p = (col & mask) >> shift;
    (*p)++;
    mask <<= BITS_PER_BYTE;
    shift += BITS_PER_BYTE;
  }
}
//send drawpoint_adr

//sbegin drawpoint
inline void l3d_rasterizer_2d_sw_imp::draw_point
(int x, int y, unsigned long col)
{
  unsigned char *p = address_of_point(x,y);
  draw_point_at_paddress(&p, col);
}
//send drawpoint

#endif
