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

#ifndef _SCRINFO_H
#define _SCRINFO_H
#include "../tool_os/memman.h"

// a rasterizer talks only to the screen_info, to find out what it needs
// to know about the display in order to do rasterization.
//
// in general, anything (not just rasterization) which needs to query the
// screen in any way must be able to get its information solely through
// a screen_info object; furthermore, it is best if such queries always
// go through the abstract interface - meaning that the abstract interface
// for l3d_screen_info contains some entries which are not meaningful for
// all concrete type of l3d_screen_info.
//
// this is a fat interface.
//
// we could create a complicated inheritance graph of screen_info classes -
// those indexed or not indexed, those with or without buffer pointers,
// those with or without color setting routines - one subclass for every
// possible combination of screen_info features, but this is a bit
// too much IMHO - such a hierarchy does not bring any useable benefit,
// and there is no criteria I know of for deciding what groupings of
// features are logically useful (e.g. here I need an indexed color-settable
// screeninfo, there I need an rgb color-settable with bufferpointer,
// and there I need an indexed non-color-settable with bufferpointer).

#define MAX_LIGHT_LEVELS 255
#define NUM_LIGHT_LEVELS (MAX_LIGHT_LEVELS+1)

// max light levels should also be correct for mesa rasterizer, which
// uses the light levels not as a lookup but as a direct lightmap intensity,
// RGB of one byte.

class l3d_screen_info {
  public:
    int ext_max_red, ext_max_green, ext_max_blue;
    char bytes_per_pixel;
    virtual unsigned long ext_to_native(int red, int green, int blue)=0;
    virtual ~l3d_screen_info(void) {};
    // in case of palettes this is a map and possibly a palette alloc too
    // in case of rgb this is just a map to the rgb resolution

    // lookup tables for colors of light and fog
    virtual void compute_light_table(void)=0;
    virtual void compute_fog_table(void)=0;
    virtual void light_native(unsigned char *pcolor, int intensity)=0;
    virtual void fog_native(unsigned char *pcolor, int intensity)=0;

    virtual void set_color(unsigned long col ) {};
    // currently only needed by mesa screeninfo, where we must call a glColor
    // function to set the color; otherwise with software rasterization we
    // set the colors in the frame buffer directly

    unsigned char *p_screenbuf;
    // only needed by software screeninfo

};

#endif
