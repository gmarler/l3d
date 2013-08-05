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

#ifndef __SCREEN_H
#define __SCREEN_H
#include "../tool_os/memman.h"

/*
 ****************************************************************************
 *
 * SCREEN.H
 *
 * Defines class Tscreen, which contains the hardware-dependent
 * routines for manipulating the physical display.
 *
 ****************************************************************************
 */

#include "sys_dep.h"
#include "rasteriz.h"
#include "object3d.h"

#ifdef DOS_OS

/*
 * MSDOS hardware ports for manipulating the video card
 */

/*
 * ports for controlling the palette: palette in, palette out,
 * palette RGB data port.
 *
 * First we send the palette index (0-255) to PORT_IPALIDX, to read
 * the a palette entry, or to PORT_OPALIDX, write a palette entry.
 * Then we send the red, green, and blue intensities (each 0-255)
 * for this palette entry, in the order R G B, to PORT_PALRGB.
 */
   #define PORT_IPALIDX 0x3C7
   #define PORT_OPALIDX 0x3C8
   #define PORT_PALRGB 0x3C9

/*
 * port for waiting for vertical trace
 * We wait until bit 0x08 is 0, then wait until it's nonzero, then
 * a vtrace just occurred.
 */
   #define PORT_VTRACE 0x3DA

#elif defined  WINDOWS_OS

/*
* Windows-specific constants
*/

/* The Windows timer-ID we use to periodically update our 3D world */
   #define TIMER_ENGINE_ID 1

#elif defined X11_OS

/*
* X11-specific constants
*/

#endif
/*
 * Class Tscreen
 *
 * Routines for setting the state of the physical screen and for drawing
 * pixels and lines on the screen. Pixel and line drawing routines simply
 * call the appropriate routines in the contained Trasterizer object.
 *
 * All drawing operations are done in an offscren buffer. blit_screen()
 * then copies the offscreen buffer to the physical screen. The actual
 * pixel drawing and blitting routines are in the Trasterizer object.
 *
 * A resolution of 320x200 with 256 colors is used. In DOS this corresponds
 * to VGA mode 13H. In Windows, a 320x200 pixel offscreen buffer is used
 * and is StretchBlt'd into whatever size the client window is.
 */

class Tscreen {
  private:
    Trasterizer *fpRasterizer; /* rasterizer object containing rasterizing */
    /* algorighms */
      #ifdef DOS_OS
    char FARPTR *screen;    /* DOS: pointer to physical display memory */
      #elif defined WINDOWS_OS
HANDLE hBitmapInfo;     /* Windows: handle to bitmap to be Blt'd */
    LPBITMAPINFO FARPTR pBitmapInfo;/*: ptr to bitmap info struct  */
      #elif defined X11_OS
unsigned char *screen;           /* X11: pointer to data buffer for XImage */
    Visual *vis;            /* X11: Visual (visual info about X server) */
    Colormap cmap;          /* X11: colormap (palette) */
    XImage *ximg;           /* X11: XImage which will be actually shown */
    GC gc;                  /* X11: graphics context */
    int depth, bytespp, scanline_pad; /* X11: info about X server */
    unsigned long col_cells[256]; /* X11: cells for colormap entries */
      #endif

  protected:
    void wait_vtrace(void);    /* wait for vertical trace */
      #if (defined DOS_OS) || (defined X11_OS)
    void set_palette(int idx, int r, int g, int b);   /* set DOS palette entry */
    void get_palette(int idx, int &r, int &g, int &b);/* get DOS palette entry */
      #endif
    void close_screen(void);   /* closes screen */
    T3dobject view_win_obj;
  public:
      #ifdef WINDOWS_OS
    HWND hDisplaywnd;          /* Windows: ptr to client (target) window */
    HPALETTE hpal;             /*        : handle to logical palette */
    int winsize_x, winsize_y;  /*        : size of client window */
      #elif defined X11_OS
    Display *dpy;           /* X11: Display (connection to X server) */
    Window w;               /* X11: Window */
      #endif
    void init_palette(void);   /* initialize palette */
    void init_palette(Tpalette *ppal);   /* initialize palette */

    Tscreen(void);                 /* constructor */
    virtual ~Tscreen(void);        /* destructor */
    int xsize, ysize;              /* size of offscr.buf in Trasterizer */
    T3dfacet view_win;
    Trasterizer *get_rasterizer(); /* for external access to rasterizer */
    void open_screen(void);        /* initialize screen for output */
    void clear_screenbuf(void);    /* clear rasterizer's screen buffer */
    void blit_screen(void);        /* copy rasterizer scrn buf 2 phys scrn */
    void blit_screen(char FARPTR *buf);   /* copy "buf" to physical screen */
    void putpixel(int x, int y, int col); /* plot pixel */
    void drawline(int x0, int y0, int x1, int y1, int col);
    /* draw line frm (x0,y0)-(x1,y1) */
    void pointcloud(const T3dfacet *aFacet);/* draw triangle's vertices    */
    void fillpoly(const T3dfacet *aFacet);/* draw filled triangle   */
    void texpoly (const T3dfacet *aFacet);/* draw textured triangle */
    void linepoly(const T3dfacet *aFacet);/* draw outlined triangle */
};

/*
 * For speed reasons, the following routines are declared inline.
 */

/*****************************************************************************
 * Get a pointer to the rasterizer object.
 *****************************************************************************/
inline Trasterizer *Tscreen::get_rasterizer() {
  return fpRasterizer;
}

/*****************************************************************************
 * Plot a pixel, by calling the rasterizer's putpixel routine.
 *****************************************************************************/
inline void Tscreen::putpixel(int x, int y, int col) {
  fpRasterizer->putpixel(fpRasterizer->get_screenbuf(), x, y, col);
}

/*****************************************************************************
 * Draw a line, by calling the rasterizer's drawline routine.
 *****************************************************************************/
inline void Tscreen::drawline(int x0, int y0, int x1, int y1, int col) {
  fpRasterizer->drawline(fpRasterizer->get_screenbuf(), x0, y0, x1, y1, col);
}

/*****************************************************************************
 * Clear the rasterizer's offscreen screen buffer.
 *****************************************************************************/
inline void Tscreen::clear_screenbuf(void) {
  fpRasterizer->clear_screenbuf();
}

/*****************************************************************************
 * Copy the rasterizer's offscreen buffer to the physical screen.
 *****************************************************************************/
inline void Tscreen::blit_screen(void) {
   #ifdef DOS_OS

  /* DOS blitting: copy offscreen buffer directly to video memory */

      #ifdef BORLAND16_DOS
  memcpy(screen, fpRasterizer->get_screenbuf(), (size_t)SCREEN_XSIZE*SCREEN_YSIZE);
      #elif defined DJGPP_DOS
movedata(_my_ds(), (int)(fpRasterizer->get_screenbuf()), _dos_ds, 0xa0000, (size_t)SCREEN_XSIZE*SCREEN_YSIZE);
      #endif
   #elif defined WINDOWS_OS

  /* Windows blitting: create a bitmap and compatible offscreen DC from
  * the offscreen buffer, and use the OS's StretchBlt routine to blit
  * the bitmap from the offscreen DC into the onscreen DC.
  */

  {
    HDC hdc;
    HBITMAP hScreenbmp;
    HDC hdcMemory;
    HBITMAP hbmpOld;
    HPALETTE hpalOld;

    /* get handle to the display context of the (already existing)
    * display window
    */
    hdc = GetDC(hDisplaywnd);

    /* select our custom palette into the screen DC */
    hpalOld = SelectPalette(hdc, hpal, FALSE);

    /* create a compatible (with the screen) offscreen display context */
    hdcMemory = CreateCompatibleDC(hdc);

    /* create offscreen bitmap from the rasterizer's offscreen buffer.
    * pBitmapInfo was already initialized in open_screen()
    */
    hScreenbmp = CreateDIBitmap(hdc,
                                (BITMAPINFOHEADER FARPTR*) pBitmapInfo,
                                CBM_INIT, fpRasterizer->get_screenbuf(), pBitmapInfo,
                                DIB_PAL_COLORS);

    /* select the bitmap into the offscreen memory display context */
    hbmpOld = (HBITMAP)SelectObject(hdcMemory, hScreenbmp);

    /* blit the bitmap from the offscreen DC into the onscreen DC */

    /* if desired, we could use StretchBlt here, to stretch a small
    * rasterization area (say 320*200) to fill out a big window (say
    * 800*600), but it's slower than BitBlt... the StretchBlt code is:

    StretchBlt(hdc, 0,0,winsize_x,winsize_y,
    hdcMemory, 0,0,xsize,ysize,
    SRCCOPY);
    */

    BitBlt(hdc, 0,0, xsize, ysize,
           hdcMemory, 0,0,
           SRCCOPY);

    /* release selected objects and display contexts */
    SelectObject(hdcMemory, hbmpOld);
    DeleteObject(hScreenbmp);
    SelectPalette(hdc, hpalOld,FALSE);
    DeleteDC(hdcMemory);
    ReleaseDC(hDisplaywnd,hdc);
  }
   #elif defined X11_OS
memcpy(screen, fpRasterizer->get_screenbuf(), (size_t)SCREEN_XSIZE*SCREEN_YSIZE);
  XPutImage(dpy, w, gc, ximg,
            0,0,0,0,  /* source x,y; destination x,y */
            SCREEN_XSIZE, SCREEN_YSIZE);
   #endif
}

/*****************************************************************************
 * Copy a given buffer "buf" to the physical screen.
 *****************************************************************************/
inline void Tscreen::blit_screen(char FARPTR *buf) {
   #ifdef DOS_OS
  memcpy(screen, buf, (size_t)SCREEN_XSIZE*SCREEN_YSIZE);
   #elif defined WINDOWS_OS
  {
    HDC hdc;
    HBITMAP hScreenbmp;
    HDC hdcMemory;
    HBITMAP hbmpOld;
    HPALETTE hpalOld;

    /* get handle to the display context of the (already existing)
    * display window
    */
    hdc = GetDC(hDisplaywnd);

    /* select our custom palette into the screen DC */
    hpalOld = SelectPalette(hdc, hpal, FALSE);

    /* create a compatible (with the screen) offscreen display context */
    hdcMemory = CreateCompatibleDC(hdc);

    /* create offscreen bitmap from "buf"
    * pBitmapInfo was already initialized in open_screen()
    */
    hScreenbmp = CreateDIBitmap(hdc,
                                (BITMAPINFOHEADER FARPTR*) pBitmapInfo,
                                CBM_INIT, buf, pBitmapInfo, DIB_PAL_COLORS);

    /* select the bitmap into the offscreen memory display context */
    hbmpOld = (HBITMAP)SelectObject(hdcMemory, hScreenbmp);

    /* blit the bitmap from the offscreen DC into the onscreen DC */
    StretchBlt(hdc, 0,0,winsize_x,winsize_y,
               hdcMemory, 0,0,xsize,ysize,
               SRCCOPY);

    /* release selected objects and display contexts */
    SelectObject(hdcMemory, hbmpOld);
    DeleteObject(hScreenbmp);
    SelectPalette(hdc, hpalOld,FALSE);
    DeleteDC(hdcMemory);
    ReleaseDC(hDisplaywnd,hdc);
  }
   #elif defined X11_OS
memcpy(screen, buf, (size_t)SCREEN_XSIZE*SCREEN_YSIZE);
  XPutImage(dpy, w, gc, ximg,
            0,0,0,0,  /* source x,y; destination x,y */
            SCREEN_XSIZE, SCREEN_YSIZE);
   #endif
}

/*****************************************************************************
 * Draw a polygon's vertex points
 *****************************************************************************/
inline void Tscreen::pointcloud(const T3dfacet *aFacet) {
  fpRasterizer->pointcloud(aFacet);
}

/*****************************************************************************
 * Texture map a polygon by calling the rasterizer's polygon filling routine
 *****************************************************************************/
inline void Tscreen::texpoly(const T3dfacet *aFacet) {
  fpRasterizer->texpoly(aFacet);
}

/*****************************************************************************
 * Fill a polygon by calling the rasterizer's polygon filling routine
 *****************************************************************************/
inline void Tscreen::fillpoly(const T3dfacet *aFacet) {
  fpRasterizer->fillpoly(aFacet);
}

/*****************************************************************************
 * Draw a wireframe polygon by calling the rasterizer's polygon routine
 *****************************************************************************/
inline void Tscreen::linepoly(const T3dfacet *aFacet) {
  fpRasterizer->linepoly(aFacet);
}
#endif
