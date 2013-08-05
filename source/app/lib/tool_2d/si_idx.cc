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

#include "si_idx.h"

#include <stdio.h>
#include <limits.h>
#include "../tool_os/memman.h"

l3d_screen_info_indexed::l3d_screen_info_indexed(int palette_size,
    int max_red, int max_green, int max_blue)
{

  this->palette = new l3d_rgb[palette_size];
  this->palette_size = palette_size;
  this->palette_num_used = 0;

  this->palette_max_red = max_red;
  this->palette_max_green = max_green;
  this->palette_max_blue = max_blue;

  this->bytes_per_pixel = 1;

  light_table = new int[palette_size * NUM_LIGHT_LEVELS];
  fog_table = new int[palette_size * NUM_LIGHT_LEVELS];

  //- initial reasonable default values for external max rgb values; these
  //- can be overridden just before actually reading rgb values from an
  //- external source
  ext_max_red = 255;
  ext_max_green = 255;
  ext_max_blue = 255;

  // mainly needed because often at the beginning a zero-color will be
  // allocated (ext_to_native(0,0,0)) before we start reading textures
  // from an external source and before we know what the ext_max ranges
  // are. with 0,0,0 as a color this is unproblematic because no
  // matter what the ext_max ranges are, black is black.
}

l3d_screen_info_indexed::~l3d_screen_info_indexed() {
  delete [] palette;
  delete [] light_table;
  delete [] fog_table;
}

unsigned long l3d_screen_info_indexed::ext_to_native(int red,
    int green, int blue)
{
  int i;
  unsigned long result=0;
  int found = 0;

  int red_scaled = red*palette_max_red / ext_max_red,
                   green_scaled = green*palette_max_green / ext_max_green,
                                  blue_scaled = blue*palette_max_blue / ext_max_blue;

  for(i=0; i<palette_num_used && !found; i++) {
    if (    (palette[i].red == red_scaled)
            && (palette[i].green == green_scaled)
            && (palette[i].blue == blue_scaled) )
    {
      found = 1;
      result = i;
    }
  }

  if ( !found ) {
    if (palette_num_used >= palette_size) {
      printf("palette overflow; generate common palette for all textures\n");
      result = 0;
    }
    else {
      palette[palette_num_used].red = red_scaled;
      palette[palette_num_used].green = green_scaled;
      palette[palette_num_used].blue = blue_scaled;
      result = palette_num_used;
      palette_num_used++;
    }
  }

  return result;
}

void l3d_screen_info_indexed::compute_light_table(void)
{
  int c,i;
  int unlit_r, unlit_g, unlit_b;
  int lit_r, lit_g, lit_b;

  int *tableptr;
  tableptr = light_table;

  for(c=0; c<palette_num_used; c++) {
    unlit_r = palette[c].red;
    unlit_g = palette[c].green;
    unlit_b = palette[c].blue;

    for(i=0; i<=MAX_LIGHT_LEVELS; i++) {
      // between 0 and given intensity
      lit_r = unlit_r * i / MAX_LIGHT_LEVELS;
      lit_g = unlit_g * i / MAX_LIGHT_LEVELS;
      lit_b = unlit_b * i / MAX_LIGHT_LEVELS;

      // now search for the nearest existing color in the palette.
      int existing_c;
      int closest_existing_c=0;
      int smallest_distance=INT_MAX;
      int squared_distance=0;
      for(existing_c=0; existing_c < palette_num_used; existing_c++) {
        squared_distance =
          (palette[existing_c].red - lit_r) *
          (palette[existing_c].red - lit_r)
          +
          (palette[existing_c].green - lit_g) *
          (palette[existing_c].green - lit_g)
          +
          (palette[existing_c].blue - lit_b) *
          (palette[existing_c].blue - lit_b) ;

        if(squared_distance < smallest_distance) {
          closest_existing_c = existing_c;
          smallest_distance = squared_distance;
        }
      }

      // if the nearest existing color isn't close enough, and if
      // we have a free extra entry, then create a new palette entry
      // with the exact needed color.

      if(squared_distance > 300
          && palette_num_used < palette_size)
      {
        palette[palette_num_used].red = lit_r * palette_max_red / ext_max_red;
        palette[palette_num_used].red = lit_g * palette_max_red / ext_max_red;
        palette[palette_num_used].red = lit_b * palette_max_red / ext_max_red;
        closest_existing_c = palette_num_used;

        palette_num_used++;
      }

      *tableptr++ = closest_existing_c;

    }
  }
}

void l3d_screen_info_indexed::compute_fog_table(void)
{
  int c,i;
  int unlit_r, unlit_g, unlit_b;
  int lit_r, lit_g, lit_b;

  int *tableptr;
  tableptr = fog_table;

  for(c=0; c<palette_num_used; c++) {
    unlit_r = palette[c].red;
    unlit_g = palette[c].green;
    unlit_b = palette[c].blue;

    for(i=0; i<=MAX_LIGHT_LEVELS; i++) {
      // between normal intensity and white
      lit_r = unlit_r + (int) (((float)i/MAX_LIGHT_LEVELS)*
              (palette_max_red - unlit_r));
      lit_g = unlit_g + (int) (((float)i/MAX_LIGHT_LEVELS)*
              (palette_max_green - unlit_g));
      lit_b = unlit_b + (int) (((float)i/MAX_LIGHT_LEVELS)*
              (palette_max_blue - unlit_b));

      // now search for the nearest existing color in the palette.
      int existing_c;
      int closest_existing_c=0;
      int smallest_distance=INT_MAX;
      for(existing_c=0; existing_c < palette_num_used; existing_c++) {
        int squared_distance =
          (palette[existing_c].red - lit_r) *
          (palette[existing_c].red - lit_r)
          +
          (palette[existing_c].green - lit_g) *
          (palette[existing_c].green - lit_g)
          +
          (palette[existing_c].blue - lit_b) *
          (palette[existing_c].blue - lit_b) ;

        if(squared_distance < smallest_distance) {
          closest_existing_c = existing_c;
          smallest_distance = squared_distance;
        }
      }

      *tableptr++ = closest_existing_c;

      // if the nearest existing color isn't close enough, and if
      // we have a free extra entry, then create a new palette entry
      // with the exact needed color.
    }
  }
}

void l3d_screen_info_indexed::light_native(unsigned char *pcolor, int intensity)
{
  // with a palette we assume the bytes_per_pixel is one
  *pcolor = light_table[ (*pcolor) * NUM_LIGHT_LEVELS + intensity ];
}

void l3d_screen_info_indexed::fog_native(unsigned char *pcolor, int intensity)
{
  // with a palette we assume the bytes_per_pixel is one
  *pcolor = fog_table[ (*pcolor) * NUM_LIGHT_LEVELS + intensity ];
}
