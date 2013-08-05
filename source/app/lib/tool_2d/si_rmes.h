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

#ifndef _SI_RMES_H
#define _SI_RMES_H
#include "../tool_os/memman.h"

#include <GL/glut.h>
#include "si_rgb.h"

// ScreenInfo, RBG-Mesa

class l3d_screen_info_rgb_mesa : public l3d_screen_info_rgb {
  public:
    l3d_screen_info_rgb_mesa(unsigned long red_mask, unsigned long green_mask,
                             unsigned long blue_mask, char bytes_per_pixel):
    l3d_screen_info_rgb(red_mask, green_mask, blue_mask, bytes_per_pixel,
                        3)
    {
      // OpenGL/Mesa always uses a 32 bit RGBA color model for its RGB,
      // regardless of the actual physical screen depth.

      this->red_mask =   0x000000FF;
      this->green_mask = 0x0000FF00;
      this->blue_mask =  0x00FF0000;
      this->bytes_per_pixel = 4;
      this->bytes_per_rgb = 3;

      compute_color_resolution();
    };

    void set_color(unsigned long col) {
      glColor4f( (float)(col & red_mask)   / red_mask,
                 (float)(col & green_mask) / green_mask,
                 (float)(col & blue_mask)  / blue_mask,
                 1.0 );
    }

    // unused for mesa
    void compute_light_table(void) {};
    void compute_fog_table(void) {} ;
    void light_native(unsigned char *pcolor, int intensity) {} ;
    void fog_native(unsigned char *pcolor, int intensity)  {};
};


#endif
