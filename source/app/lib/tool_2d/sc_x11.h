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

#ifndef __SC_X11_H
#define __SC_X11_H
#include "../tool_os/memman.h"

#include "screen.h"
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>

class l3d_screen_x11 : public l3d_screen {
  protected:
    Visual *vis;            // X11: Visual (visual info about X server)
    Colormap cmap;          // X11: colormap (palette)
    unsigned char *buffer;           // offscreen buffer data for ximage
    GC gc;                  // X11: graphics context
    int depth, bytespp, scanline_pad; // X11: info about X server
    unsigned long* col_cells; // X11: cells for colormap entries

    void close_screen(void);
  public:
    XImage *ximg;           // X11: XImage which will be actually shown
    virtual XImage *create_ximage(void);
    virtual void create_buffer(void);

    Display *dpy;           // X11: Display (connection to X server)
    Window w;               // X11: Window
    l3d_screen_x11(int xsize, int ysize);
    virtual ~l3d_screen_x11(void);
    void refresh_palette(void);
    void open_screen(void);
    void blit_screen(void);
};

class l3d_screen_factory_x11 : public l3d_screen_factory {
  public:
    l3d_screen *create(int xsize, int ysize) {
      l3d_screen_x11 *s;
      s = new l3d_screen_x11(xsize,ysize);

      // create offscreen buffer and ximage

      s->create_buffer();
      s->ximg = s->create_ximage();

      return s;
    }
};

inline void l3d_screen_x11::blit_screen(void) {
  XPutImage(dpy, w, gc, ximg,
            0,0,0,0,  /* source x,y; destination x,y */
            xsize, ysize);
}

#endif
