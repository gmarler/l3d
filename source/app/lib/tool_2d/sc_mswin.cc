#include "sc_mswin.h"
#include <stdlib.h>

static HDC hdcMem;

#define DIB_24
#define DIBS 
l3d_screen_mswin::l3d_screen_mswin(int xsize, int ysize) :
  l3d_screen(xsize,ysize)
{

// figure out depth and create accoringly
// for now, just 8 bpp
#ifndef DIB_24
  int bytespp=1;

   l3d_screen_info_indexed *si;
      sinfo = si = new l3d_screen_info_indexed(256,
					  (1<<8)-1,  // bits_per_red
					  (1<<8)-1,  // bits_per_green
					  (1<<8)-1);  // bits_per_blue

      si->palette_num_used = 10; // windows reserves colors 0-9 and 246-255
      /* Windows: allocate space for bitmapinfo and initialize timer */

      {
         /* allocate space for bitmapinfo struct */
         hBitmapInfo = GlobalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,
             sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
         if (hBitmapInfo == NULL) {
            MessageBox(NULL,"Can't allocate space for bitmap info",
                            "Fatal error", MB_OK);
            exit(-1);
         }
         pBitmapInfo = (LPBITMAPINFO) GlobalLock(hBitmapInfo);
   
         /* initialize bitmap info; the actual bitmap is created later within
          * the blit_screen() method
          */
         pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
         pBitmapInfo->bmiHeader.biWidth = xsize;
         pBitmapInfo->bmiHeader.biHeight = ysize;
         pBitmapInfo->bmiHeader.biPlanes = 1;
         pBitmapInfo->bmiHeader.biBitCount = 8;
         pBitmapInfo->bmiHeader.biCompression = BI_RGB;

         pBitmapInfo->bmiHeader.biSizeImage = 0;
         pBitmapInfo->bmiHeader.biClrUsed = 0;
         pBitmapInfo->bmiHeader.biClrImportant = 0;
         pBitmapInfo->bmiHeader.biSizeImage = xsize*ysize;

      }

#ifndef DIBS
   buffer = new unsigned char [xsize*ysize*bytespp];
   sinfo->p_screenbuf = buffer;
#endif

#else
// DIB24

  int bytespp=4;

   l3d_screen_info_rgb *si;
      sinfo = si = new l3d_screen_info_rgb(0xFF0000, // red mask
					   0x00FF00, // green mask
					   0x0000FF, // blue mask
					   bytespp,  // bytes per pixel
                                           3); // bytes per RGB-spec

      /* Windows: allocate space for bitmapinfo and initialize timer */

      {
         /* allocate space for bitmapinfo struct */
         hBitmapInfo = GlobalAlloc(LMEM_ZEROINIT | LMEM_MOVEABLE,
             sizeof(BITMAPINFOHEADER) + (sizeof(RGBQUAD) * 256));
         if (hBitmapInfo == NULL) {
            MessageBox(NULL,"Can't allocate space for bitmap info",
                            "Fatal error", MB_OK);
            exit(-1);
         }
         pBitmapInfo = (LPBITMAPINFO) GlobalLock(hBitmapInfo);
   
         /* initialize bitmap info; the actual bitmap is created later within
          * the blit_screen() method
          */
         pBitmapInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
         pBitmapInfo->bmiHeader.biWidth = xsize;
         pBitmapInfo->bmiHeader.biHeight = ysize;
         pBitmapInfo->bmiHeader.biPlanes = 1;
         pBitmapInfo->bmiHeader.biBitCount = 32;
         pBitmapInfo->bmiHeader.biCompression = BI_RGB;

         pBitmapInfo->bmiHeader.biSizeImage = 0;
         pBitmapInfo->bmiHeader.biClrUsed = 0;
         pBitmapInfo->bmiHeader.biClrImportant = 0;
      }


#endif

   bufsize = xsize * ysize * bytespp; // used for clearing screen buf
}

l3d_screen_mswin::~l3d_screen_mswin(void) {
      /* Windows: discard bitmap info, custom palette, and timer */

      GlobalUnlock(hBitmapInfo);
      GlobalFree(hBitmapInfo);

      DeleteObject(hpal);

      KillTimer(hDisplaywnd, TIMER_ENGINE_ID);

  delete[] buffer;

#ifdef DIBS
  DeleteObject(hBitmap);
#endif
}

void l3d_screen_mswin::open_screen(void) {
         /* timer initialization: periodically updates the 3D world
          * every 50 milliseconds
          */
//           if(!SetTimer(hDisplaywnd, TIMER_ENGINE_ID, 50, NULL)) {
//              MessageBox(NULL, "Can't allocate timer!", "Fatal error", MB_OK);
//              exit(-1);
//           }

         /* determination of client window size (needed for StretchBlt) */
         {
            RECT r;
   
            GetClientRect(hDisplaywnd, &r);
            winsize_x = r.right;
            winsize_y = r.bottom;
         } 

#ifdef DIBS
  hdcMem = CreateCompatibleDC(NULL);
  SelectPalette(hdcMem, hpal, FALSE);
#ifndef DIB_24
  hBitmap = CreateDIBSection(hdcMem, pBitmapInfo,
    DIB_PAL_COLORS, (LPVOID *)&buffer, NULL, 0);
#else
  hBitmap = CreateDIBSection(hdcMem, pBitmapInfo,
    DIB_RGB_COLORS, (LPVOID *)&buffer, NULL, 0);
#endif
  sinfo->p_screenbuf = buffer;
  SelectObject(hdcMem, hBitmap);
#endif

}

void l3d_screen_mswin::close_screen(void) {

   // FIXME: free colormap, etc !
}

void l3d_screen_mswin::refresh_palette(void) {

  l3d_screen_info_indexed *si;

//  if ((si = dynamic_cast<l3d_screen_info_indexed *>(sinfo))) {
  if(0)  {

   /* Windows: set palette by defining and "realizing" a LOGPALETTE */
   
      PALETTEENTRY ape[si->palette_size];
      HDC hdc;
      LOGPALETTE FAR* plgpl;
   
// delete old palette???

      /* According to the Windows API help, if the bitmap image data is
       * defined in terms of palette entries (as opposed to explicit RGB
       * colors), then the pBitmapInfo->bmiColors member is an array of
       * unsigned-int indices into the currently-realized logical palette
       * (as opposed to an array of explicit RGBQUAD colors). Therefore we
       * cast the bmiColors member to unsigned int *.
       */
      unsigned short int *prgbq = (unsigned short int *)pBitmapInfo->bmiColors;
   
      plgpl = (LOGPALETTE*)
              FARmalloc(sizeof(LOGPALETTE) + si->palette_size * sizeof(PALETTEENTRY));
      if(!plgpl) {
         MessageBox(NULL,"Can't allocate memory for palette!", 
                         "Fatal error", MB_OK);
         exit(-1);
      }
      plgpl->palNumEntries = si->palette_size;
      plgpl->palVersion = 0x300;
   
    int idx;

    for(idx=0; idx<si->palette_size; idx++) {

            /* define the logical palette entry */
            ape[idx].peRed = plgpl->palPalEntry[idx].peRed =
              (int) (( (float)si->palette[idx].red /
			    si->palette_max_red ) * 255);
            ape[idx].peGreen = plgpl->palPalEntry[idx].peGreen =
              (int) (( (float)si->palette[idx].green /
			    si->palette_max_green) * 255);
            ape[idx].peBlue = plgpl->palPalEntry[idx].peBlue =
              (int) (( (float)si->palette[idx].blue /
			    si->palette_max_blue) * 255);
printf("assigning palette %d with %d %d %d\n", idx,
  ape[idx].peRed,
  ape[idx].peGreen,
  ape[idx].peBlue
);
#ifdef DIBS_RGB
  pBitmapInfo->bmiColors[idx].rgbRed =
              (int) (( (float)si->palette[idx].red /
			    si->palette_max_red ) * 255);
  pBitmapInfo->bmiColors[idx].rgbGreen =
              (int) (( (float)si->palette[idx].green /
			    si->palette_max_green) * 255);
  pBitmapInfo->bmiColors[idx].rgbBlue =
              (int) (( (float)si->palette[idx].blue /
			    si->palette_max_blue) * 255);
  pBitmapInfo->bmiColors[idx].rgbReserved =0;
#else
/*

            ape[idx].peRed = plgpl->palPalEntry[idx].peRed = idx;
            ape[idx].peGreen = plgpl->palPalEntry[idx].peGreen = 0;
            ape[idx].peBlue = plgpl->palPalEntry[idx].peBlue =0;
            ape[idx].peFlags = plgpl->palPalEntry[idx].peFlags = PC_RESERVED;
*/

            /* assign this logical palette index to the appropriate member
             * in pBitmapInfo->bmiColors
             */
            *prgbq++ = idx; 
#endif
   }

      hpal = CreatePalette(plgpl);  /* create the palette */
      if (hpal==NULL) {
         MessageBox(NULL, "Couldn't create palette!", "Fatal error", MB_OK);
         exit(-1);
      }

      FARfree(plgpl);
   }
#ifdef DIBS
  DeleteObject(hBitmap); // del old bitmap with old palette
  SelectPalette(hdcMem, hpal, FALSE);
#ifndef DIB_24
  hBitmap = CreateDIBSection(hdcMem, pBitmapInfo,
    DIB_PAL_COLORS, (LPVOID *)&buffer, NULL, 0);
#else
  hBitmap = CreateDIBSection(hdcMem, pBitmapInfo,
    DIB_RGB_COLORS, (LPVOID *)&buffer, NULL, 0);
#endif
  sinfo->p_screenbuf = buffer;
  SelectObject(hdcMem, hBitmap);
#endif
}
