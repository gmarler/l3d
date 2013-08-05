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

#include "ras3z_mes.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include "../tool_os/memman.h"

///////////////////////////////////////////////////////////////////////
l3d_rasterizer_3d_imp * l3d_rasterizer_3d_zbuf_mesa_imp_factory::create
(int xs, int ys, l3d_screen_info *si)
{
  return new l3d_rasterizer_3d_zbuf_mesa_imp(xs,ys,si);
}

l3d_rasterizer_3d_zbuf_mesa_imp::
l3d_rasterizer_3d_zbuf_mesa_imp(int xs, int ys, l3d_screen_info *si):
    l3d_rasterizer_3d_imp(xs,ys,si),
    l3d_rasterizer_2d_imp(xs,ys,si),
    l3d_rasterizer_2d_mesa_imp(xs,ys,si),
    l3d_rasterizer_3d_mesa_imp(xs,ys,si)
{
  glEnable(GL_DEPTH_TEST);
  //    glEnable(GL_FOG);
  //    GLfloat fogColor[4] = {1.0,0.0,0.0,1.0};
  //    glFogi(GL_FOG_MODE, GL_LINEAR);
  //    glFogfv(GL_FOG_COLOR, fogColor);
  //    glFogf(GL_FOG_DENSITY, 1.00);
  //    glHint(GL_FOG_HINT, GL_DONT_CARE);
  //    glFogf(GL_FOG_START, 20.0);
  //    glFogf(GL_FOG_END, 100.0);
}

l3d_rasterizer_3d_zbuf_mesa_imp::
~l3d_rasterizer_3d_zbuf_mesa_imp(void)
{
}

void l3d_rasterizer_3d_zbuf_mesa_imp::clear_buffer(void)
{
  l3d_rasterizer_2d_mesa_imp::clear_buffer();
  glClear(GL_DEPTH_BUFFER_BIT);
}


