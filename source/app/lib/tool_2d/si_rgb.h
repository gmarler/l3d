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

#ifndef _SI_RGB_H
#define _SI_RGB_H
#include "../tool_os/memman.h"

#include "scrinfo.h"

class l3d_screen_info_rgb : public l3d_screen_info {
  private:
    int *red_light_table, *green_light_table, *blue_light_table,
    *red_fog_table, *green_fog_table, *blue_fog_table;
  public:
    l3d_screen_info_rgb(unsigned long red_mask, unsigned long green_mask,
                        unsigned long blue_mask, char bytes_per_pixel, char bytes_per_rgb);
    virtual l3d_screen_info_rgb::~l3d_screen_info_rgb(void);

    char bytes_per_rgb;
    unsigned long red_mask, green_mask, blue_mask;
    int red_shift, green_shift, blue_shift,
    red_max, green_max, blue_max;
    void compute_color_resolution();

    unsigned long ext_to_native(int red, int green, int blue);
    void compute_light_table(void);
    void compute_fog_table(void);
    void light_native(unsigned char *pcolor, int intensity);
    void fog_native(unsigned char *pcolor, int intensity);


};

#endif
