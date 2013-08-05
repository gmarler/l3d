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

#include "sc_x11.h"
#include <stdlib.h>
#include "../tool_os/memman.h"

l3d_screen_x11::l3d_screen_x11(int xsize, int ysize) :
    l3d_screen(xsize,ysize)
{

  XPixmapFormatValues *pixmap_formats;
  int i, count;

  // establish connection to X server

  dpy = XOpenDisplay(NULL);

  // find out the supported depths of this server and take the default depth
  int* depths, numdepths;
  depths = XListDepths(dpy, DefaultScreen(dpy), &numdepths);
  depth = depths[0];
  for(i=0;i<numdepths;i++) {
    printf("got X depth %d\n", depths[i]);
  }
  XFree(depths);

  // get pixmap information about this depth
   pixmap_formats = XListPixmapFormats(dpy, &count);
   for(i=0; i<count; i++) {
     printf("getting depth %d\n",pixmap_formats[i].depth);
     if(pixmap_formats[i].depth == depth) {
       bytespp      = pixmap_formats[i].bits_per_pixel / BITS_PER_BYTE;
       scanline_pad = pixmap_formats[i].scanline_pad;
       break;
     }
   }
   XFree(pixmap_formats);


  printf("default depth of display %d\n", depth);
  printf("bytes per pixel: %d\n", bytespp);

  bufsize = xsize * ysize * bytespp; // used for clearing screen buf

  // create and map (display) an X window for output

  vis = DefaultVisual(dpy,0);
  w = XCreateWindow(dpy,                        // display
                    DefaultRootWindow(dpy),     // parent
                    100, 100,                   // x, y position
                    xsize, ysize,               // width, height
                    0,                          // border width
                    depth,                      // depth (we use max. possible)
                    CopyFromParent,             // visual class (TrueColor etc)
                    vis,                        // visual
                    0, NULL);                   // valuemask, window attributes
  XStoreName(dpy, w, "l3d application");
  XMapWindow(dpy, w);

  switch(vis->c_class) {
      // constants for visual classes are defined in /usr/include/X11/X.h

      XVisualInfo vi;
      int result;

    case PseudoColor:
      result=
        XMatchVisualInfo(dpy, DefaultScreen(dpy), depth, PseudoColor, &vi);
      if(result) {
        printf("visual is PseudoColor (indexed),\n");
        printf("colormapsize %d,\n", vi.colormap_size);
        printf("bits per rgb %d\n",vis->bits_per_rgb);

        col_cells = new unsigned long[vi.colormap_size];
        sinfo = new l3d_screen_info_indexed(vi.colormap_size,
                                            (1<<vis->bits_per_rgb)-1,
                                            (1<<vis->bits_per_rgb)-1,
                                            (1<<vis->bits_per_rgb)-1);
        printf("is indexed\n");
      } else {
        printf("no information for PseudoColor visual\n");
      }


      break;

    case TrueColor:
      result=
        XMatchVisualInfo(dpy, DefaultScreen(dpy), depth, TrueColor, &vi);
      if(result) {
        printf("visual is TrueColor, %d bytes per pix, %d bytes per rgb\n",
               bytespp,
               vi.depth / BITS_PER_BYTE);
        col_cells = NULL;
        sinfo = new l3d_screen_info_rgb(vis->red_mask,
                                        vis->green_mask,
                                        vis->blue_mask,
                                        bytespp,
                                        vi.depth / BITS_PER_BYTE);
        // FIXME: byte order!!!!!!!!
        // pbuf->byte_order =
      } else {
        printf("Couldn't get visual information, XMatchVisualInfo\n");
        exit(-1);
      }
      break;

    case StaticColor: printf("unsupported visual StaticColor\n");break;
    case GrayScale:   printf("unsupported visual GrayScale\n");break;
    case StaticGray:  printf("unsupported visual StaticGray\n");break;
    case DirectColor: printf("unsupported visual DirectColor\n");break;
  }

  // creation of ximage and offscreen buffer cannot take place here,
  // in the constructor, because:
  //
  //    in derived classes, this base class constructor will be completely
  //    executed before the child constructor gets to execute. if the child
  //    constructor however wants to create the ximage and buffer differently
  //    (as is the case with shm), it is too late - the base class constructor
  //    has already created the wrong one.
  //
  // thus the creation of the ximage and the buffer is moved into
  // the factory method create(). this implies also that if for some
  // reason an x11_screen is created directly, without going through the
  // factory method, that the caller had better also explitcly call
  // the routines to create the ximg and the buffer - in fact, the caller
  // must do everything which the factory method does.


  XSelectInput(dpy, w, KeyPressMask);
  // gc is not needed now, but later when blitting with XPutImage
  gc = DefaultGC(dpy, DefaultScreen(dpy));

}

l3d_screen_x11::~l3d_screen_x11(void) {
  delete sinfo;
  XDestroyImage(ximg);
  //delete [] buffer; // automatically done by XDestroyImage ! See man page...
  if(col_cells) delete [] col_cells;
  XCloseDisplay(dpy);
}

void l3d_screen_x11::open_screen(void) {
}

void l3d_screen_x11::close_screen(void) {

  // FIXME: free colormap, etc !
}

void l3d_screen_x11::refresh_palette(void) {

  l3d_screen_info_indexed *si;
  if ((si = dynamic_cast<l3d_screen_info_indexed *>(sinfo))) {

    int idx;

    cmap = XCreateColormap(dpy, w, vis, AllocNone);

    // we allocate 2^depth cells for the palette: the max. displayable
    // colors

    XAllocColorCells(dpy, cmap, TRUE, NULL, 0, col_cells,
                     (int)pow(2.0, (double)depth));

    for(idx=0; idx<si->palette_size; idx++) {
      XColor xcolor;

      xcolor.pixel = idx;
      // X color model is by definition 0-65535, thus scale all values to
      // be in this range
      xcolor.red = (int) (( (float)si->palette[idx].red /
                            si->palette_max_red ) * 65535);
      xcolor.green = (int) (( (float)si->palette[idx].green /
                              si->palette_max_green ) * 65535);
      xcolor.blue = (int)( ( (float)si->palette[idx].blue /
                             si->palette_max_blue ) * 65535);
      xcolor.flags = DoRed | DoGreen | DoBlue;
      XAllocColor(dpy, cmap, &xcolor);
      XStoreColor(dpy, cmap, &xcolor);
    }
    XSetWindowColormap(dpy, w, cmap);
  }

}

XImage *l3d_screen_x11::create_ximage(void) {
  return XCreateImage(dpy, vis, depth,
                      ZPixmap,         // format XYPixmap or ZPixmap
                      0,               // pixels to ignore at beg. of scanline
                      (char *)buffer,  // pointer to image data
                      xsize, ysize,    // width, height
                      scanline_pad,    // bitmap pad: each scanline is a
                      //   multiple of scanline_pad bits long
                      0);              // bytes_per_line: 0 = contiguious (i.e.
  //   autocompute

}

void l3d_screen_x11::create_buffer(void) {
  buffer = new unsigned char [xsize*ysize*bytespp];
  sinfo->p_screenbuf = buffer;
}
