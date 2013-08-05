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

#ifndef _SI_IDX_H
#define _SI_IDX_H
#include "../tool_os/memman.h"

#include "scrinfo.h"

struct l3d_rgb {
  int red, green, blue;
};

class l3d_screen_info_indexed : public l3d_screen_info {
    int *light_table;
    int *fog_table;

  public:
    l3d_screen_info_indexed(int palette_size, int max_red, int max_green, int max_blue);
    virtual ~l3d_screen_info_indexed(void);

    int palette_size;
    l3d_rgb *palette;
    int get_palette_size(void) { return palette_size; }
    int palette_num_used;
    int palette_max_red, palette_max_green, palette_max_blue;
    unsigned long ext_to_native(int red, int green, int blue);
    void compute_light_table(void);
    void compute_fog_table(void);
    void light_native(unsigned char *pcolor, int intensity);
    void fog_native(unsigned char *pcolor, int intensity);
};

#endif
