#ifndef __SC_MSWIN_H
#define __SC_MSWIN_H

#include "screen.h"
#include "windows.h"

class l3d_screen_mswin : public l3d_screen {
  protected:
  unsigned char *buffer;           // offscreen buffer data for DIB
  static const int TIMER_ENGINE_ID= 1;
  HANDLE hBitmapInfo;     /* Windows: handle to bitmap to be Blt'd */
  LPBITMAPINFO FARPTR pBitmapInfo;/*: ptr to bitmap info struct  */
  HBITMAP hBitmap;

    void close_screen(void);
  public:
    HWND hDisplaywnd;          /* Windows: ptr to client (target) window */
    HPALETTE hpal;             /*        : handle to logical palette */
    int winsize_x, winsize_y;  /*        : size of client window */

    l3d_screen_mswin(int xsize, int ysize);
    virtual ~l3d_screen_mswin(void);
    void refresh_palette(void);
    void open_screen(void);
    void blit_screen(void);
};

class l3d_screen_factory_mswin : public l3d_screen_factory {
 public:
  l3d_screen *create(int xsize, int ysize) {
    l3d_screen_mswin *s;
    s = new l3d_screen_mswin(xsize,ysize);
    return s;
  }
};

inline void l3d_screen_mswin::blit_screen(void) {
#ifdef DIBS
{
         HDC hdc;
         HPALETTE hpalOld,hpalOld_mem;

         /* get handle to the display context of the (already existing)
          * display window
          */
         hdc = GetDC(hDisplaywnd);

         /* select our custom palette into the screen DC */
         hpalOld = SelectPalette(hdc, hpal, FALSE);
         RealizePalette(hdc);
   
         /* select the bitmap into the offscreen memory display context */

         /* blit the bitmap from the offscreen DC into the onscreen DC */

         BitBlt(hdc, 0,0, xsize, ysize,
                    hdcMem, 0,0,
                    SRCCOPY);

         /* release selected objects and display contexts */
//         SelectObject(hdcMem, hbmpOld);
//         DeleteObject(hScreenbmp);
//         SelectPalette(hdcMem,hpalOld_mem, TRUE);
//         SelectPalette(hdc, hpalOld,FALSE);
//         DeleteDC(hdcMem);
         ReleaseDC(hDisplaywnd,hdc);
      }
#else
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
            CBM_INIT, buffer, pBitmapInfo,
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
#endif

}

#endif
