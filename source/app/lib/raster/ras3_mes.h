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

#ifndef __RAS3_MES_H
#define __RAS3_MES_H
#include "../tool_os/memman.h"

#include <GL/glut.h>
#include "rast3.h"
#include "ras_mesa.h"

////////////////////////////////////////////////////////////////////

//  /*
//  static const GLubyte space[] = 
//  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//  static const GLubyte digits[][13] = {
//    {0x0,0x0,0x18,0x3C,0x66,0xC3,0xC3,0xC3,0xC3,0xC3,0x66,0x3C,0x18},
//    {0x0,0x0,0xC,0x3C,0x6C,0xC,0xC,0xC,0xC,0xC,0xC,0x7F,0x7F},
//    {0x0,0x0,0x3C,0x66,0xC6,0x6,0xC,0x18,0x30,0x60,0xC0,0xFF,0xFF},
//    {0x0,0x0,0x7E,0xC3,0x3,0x6,0xC,0x38,0xC,0x6,0x3,0xC3,0x7E},
//    {0x0,0x0,0x1E,0x36,0x66,0xC6,0xC6,0xFF,0xFF,0x6,0x6,0x6,0x6},
//    {0x0,0x0,0xFE,0xC0,0xC0,0xC0,0xFE,0xFF,0x3,0x3,0xC3,0xFF,0x7E},
//    {0x0,0x0,0x7E,0xC0,0xC0,0xC0,0xFE,0xFF,0xC3,0xC3,0xC3,0xFF,0x7E},
//    {0x0,0x0,0xFF,0x3,0x3,0x6,0xC,0x18,0x30,0x60,0x60,0x60,0x60},
//    {0x0,0x0,0x7E,0xC3,0xC3,0xC3,0xC3,0x7E,0xC3,0xC3,0xC3,0xFF,0x7E},
//    {0x0,0x0,0x7E,0xC3,0xC3,0xC3,0xC3,0x7F,0x3,0x3,0xC3,0xFF,0x7E}
//  };


//  static const GLubyte letters[][13] = {
//  {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18}, 
//  {0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe}, 
//  {0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e}, 
//  {0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce, 0xfc}, 
//  {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff}, 
//  {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xff}, 
//  {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e}, 
//  {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3}, 
//  {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e}, 
//  {0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06}, 
//  {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3}, 
//  {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0}, 
//  {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7, 0xc3}, 
//  {0x00, 0x00, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3}, 
//  {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e}, 
//  {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe}, 
//  {0x00, 0x00, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c}, 
//  {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe}, 
//  {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7, 0x7e}, 
//  {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff}, 
//  {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3}, 
//  {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3}, 
//  {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3}, 
//  {0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3}, 
//  {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3}, 
//  {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff}
//  };

//  */


// extern const GLubyte period[];
// extern const GLubyte space[];
// extern const GLubyte digits[][13];
// extern const GLubyte letters[][13];

class l3d_rasterizer_3d_mesa_imp :
      virtual public l3d_rasterizer_2d_mesa_imp,
      virtual public l3d_rasterizer_3d_imp
{
  private:

    GLuint fontOffset;// for text
    void make_raster_font(void);

  protected:
    void texture_setup(const l3d_polygon_3d_textured *p_poly);
    void ogl_texturing_between_glbegin_end
    (const l3d_polygon_3d_textured *p_poly);

    l3d_real ui_xs, ui_ys, ui_c, vi_xs, vi_ys, vi_c, zi_xs, zi_ys, zi_c,
    ui, vi, zi;

    unsigned long text_color;

  public:
    l3d_rasterizer_3d_mesa_imp(int xs, int ys, l3d_screen_info *si);

    // we normally would not need to redefine these methods (defined
    // originally in l3d_rasterizer_2d_mesa_imp), but since the
    // 3d mesa rasterizer implementation redefines the origin to be
    // at the center of the screen (since the LAST step for perspective
    // correction MUST be division by w done by the hardware, thereby
    // requiring us to define the origin to be at the center), we have
    // to redefine all routines accordingly to work with a screen-center
    // origin.

    void draw_point(int x, int y, unsigned long col);
    void draw_line(int x0, int y0, int x1, int y1, unsigned long col);
    void draw_polygon_flatshaded(const l3d_polygon_2d_flatshaded *p_poly);

    // draw a textured polygon
    void draw_polygon_textured(const l3d_polygon_3d_textured *p_poly);
    void draw_polygon_textured_lightmapped
    (const l3d_polygon_3d_textured_lightmapped *p_poly);

    void draw_text(int x, int y, const char *string);
    void set_text_color(int red, int green, int blue);

    void clear_buffer(void);
};


class l3d_rasterizer_3d_mesa_imp_factory :
  public l3d_rasterizer_3d_imp_factory {
  public:
    l3d_rasterizer_3d_imp *create(int xs, int ys, l3d_screen_info *si); // virt
};

#endif





