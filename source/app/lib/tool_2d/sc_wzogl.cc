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


#include "sc_wzogl.h"
#include "../tool_os/memman.h"

l3d_screen_zbuf_wogl::l3d_screen_zbuf_wogl(int xsize, int ysize) :
    l3d_screen(xsize,ysize)
{
  int argc=1;
  char *argv[1];
  argv[0] = "a";

  int depth=0, bytespp=0, scanline_pad=0;
  int i, count;

  // find out the supported depths of this server and take the max depth

  depth = 24;
  bytespp = 3;
  printf("max depth of display %d\n", depth);
  printf("bytes per pixel: %d\n", bytespp);

  bufsize = xsize * ysize * bytespp; // used for clearing screen buf

  sinfo = new l3d_screen_info_rgb_mesa(0xFF0000, 0x00FF00,
                                           0x0000FF, bytespp);
      glutInit(&argc, argv);
      glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
      glutInitWindowSize(xsize,ysize);
      glutInitWindowPosition(100,100);
      glutCreateWindow("l3d");

      glClearColor(0.0,0.0,0.0,0.0);
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluOrtho2D(0, xsize, ysize, 0);  // (0,0) is thus at upper-left
      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      // CGDC notes 1998: offset by 0.375 this to use int coords for all primitives
      glTranslatef(0.375, 0.375, 0.0);
      glViewport(0,0,xsize,ysize);

}

l3d_screen_zbuf_wogl::~l3d_screen_zbuf_wogl(void) {
}

void l3d_screen_zbuf_wogl::open_screen(void) {
}
