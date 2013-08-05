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


#include "sc_zmesa.h"
#include "../tool_os/memman.h"

l3d_screen_zbuf_mesa::l3d_screen_zbuf_mesa(int xsize, int ysize) :
    l3d_screen(xsize,ysize)
{
  int argc=1;
  char *argv[1];
  argv[0] = "a";

  Visual *vis;            // X11: Visual (visual info about X server) 
  int depth=0, bytespp=0, scanline_pad=0; // X11: info about X server 
  Display *dpy;           // X11: Display (connection to X server) 
  XPixmapFormatValues *pixmap_formats;
  int i, count;

  // establish connection to X server

  dpy = XOpenDisplay(NULL);

  // find out the supported depths of this server and take the max depth

  pixmap_formats = XListPixmapFormats(dpy, &count);
  for(i=0, depth=0; i<count; i++) {
    if(pixmap_formats[i].depth > depth) {
      depth        = pixmap_formats[i].depth;
      bytespp      = pixmap_formats[i].bits_per_pixel / BITS_PER_BYTE;
      scanline_pad = pixmap_formats[i].scanline_pad;
    }
  }
  XFree(pixmap_formats);
  printf("max depth of display %d\n", depth);
  printf("bytes per pixel: %d\n", bytespp);

  bufsize = xsize * ysize * bytespp; // used for clearing screen buf

  // create and map (display) an X window for output 

  vis = DefaultVisual(dpy,0);

  // FIXME: byte order!!!!!!!!
  // pbuf->byte_order =
  switch(vis->c_class) {
      // constants for visual classes are defined in /usr/include/X11/X.h

    case PseudoColor:
      sinfo = new l3d_screen_info_indexed_mesa(255, 65535, 65535, 65535);
      glutInit(&argc, argv);
      glutInitDisplayMode(GLUT_DOUBLE|GLUT_INDEX|GLUT_DEPTH);
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

      break;
    case TrueColor:
      sinfo = new l3d_screen_info_rgb_mesa(vis->red_mask, vis->green_mask,
                                           vis->blue_mask, bytespp);
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

      break;
    case StaticColor: printf("unsupported visual StaticColor\n");break;
    case GrayScale:   printf("unsupported visual GrayScale\n");break;
    case StaticGray:  printf("unsupported visual StaticGray\n");break;
    case DirectColor: printf("unsupported visual DirectColor\n");break;
  }

}

l3d_screen_zbuf_mesa::~l3d_screen_zbuf_mesa(void) {
}

void l3d_screen_zbuf_mesa::open_screen(void) {
}
