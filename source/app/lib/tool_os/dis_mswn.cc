#include "dis_mswn.h"
#include "windows.h"
#include <stdlib.h>
#include <string.h>

#include "../pipeline/pipeline.h"


l3d_pipeline *l3d_dispatcher_mswin::last_pipeline;
l3d_event_source *l3d_dispatcher_mswin::last_event_source;

/*
int WINAPI WinMain (HINSTANCE hInst, 
	     HINSTANCE hPrevInstance,
	     char * lpszCmdParam,
	     int nCmdShow)
{
}
int pascal WinMain(HINSTANCE hInst,
   HINSTANCE hPrevInstance,
   LPSTR lpszCmdParam,
   int nCmdShow)
{

*/




void l3d_dispatcher_mswin::set_pipeline(l3d_pipeline *p) {
  l3d_dispatcher_mswin::last_pipeline = p;
  pipeline = p;
}

void l3d_dispatcher_mswin::start(void) {
  l3d_screen_mswin *screen;

  // FIXME!! dynamic cast not accepted by gcc...
  screen = (l3d_screen_mswin *)event_source;
  last_event_source = event_source;
  l3d_dispatcher_mswin::last_pipeline = pipeline;

   static char Name[] = "L3D 3D Engine";
   HWND hwnd;
   MSG Msg;
   WNDCLASS W;

printf("start\n");
   memset(&W, 0, sizeof(WNDCLASS));
printf("memset\n");
   W.style = CS_HREDRAW | CS_VREDRAW;
printf("style\n");
   W.lpfnWndProc = l3d_dispatcher_mswin::winproc;
printf("wndprc\n");
   W.hInstance = 0; // hInst;
printf("hinst\n");
//   W.hbrBackground = GetStockBrush(WHITE_BRUSH);
   W.lpszClassName = Name;
printf("classname\n");
   W.cbWndExtra = sizeof(long);
printf("extra\n");

   RegisterClass(&W);
printf("registered\n");

   hwnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW,
      10, 10, screen->xsize, screen->ysize, NULL, NULL, /* hInst */ 0, NULL);
printf("created\n");


//   SetWindowLong(hwnd, 0, (long)( (Tworld *)(&aWorld)) );
printf("long\n");

   screen->hDisplaywnd = hwnd;
printf("hDisplaywnd\n");

   ShowWindow(hwnd, 1 /* nCmdShow */ );
printf("showwindow\n");
   UpdateWindow(hwnd);
printf("updatewindow\n");

  screen->open_screen();
  screen->refresh_palette();

      {
  l3d_screen_mswin *screen;
  screen = (l3d_screen_mswin *)last_event_source;

         HPALETTE hpalOld;
         HDC hdc;
         UINT palChangedCount;

         /* if our window is activated, re-realize the window's palette */

         hdc = GetDC(hwnd);
         /* last param is FALSE (==not background palette) since this code is executed when our
          * app has the input focus */
         hpalOld = SelectPalette(hdc,screen->hpal, FALSE);
         palChangedCount = RealizePalette(hdc);
         SelectPalette(hdc,hpalOld, TRUE);
         ReleaseDC(hwnd,hdc);

         /* If any palette entries changed, repaint the window. */
         if (palChangedCount > 0) {
            InvalidateRect(hwnd, NULL, TRUE);
         }
      }

   while(GetMessage(&Msg, NULL, 0, 0)) {
      TranslateMessage(&Msg);
      DispatchMessage(&Msg);
   }
//   return Msg.wParam;
   return;
}

/* Windows event handling: the WindowProc for the "window" (in the Windows
 * definition of the word), which handles all incoming window messages
 * coming from the operating system
 *
 * This function must be static and cannot be a member function of a class
 * because class member functions have a hidden "this" pointer.
 */
 
/* static (declared static in header) */
LRESULT CALLBACK l3d_dispatcher_mswin::winproc(HWND hWindow, UINT Message,
   WPARAM wp, LPARAM lp)
{

   /* we intentionally avoid the use of message crackers here, since we
    * don't want to clutter the file with several of Windows-specific event
    * handling routines. Therefore we have an old-fashioned huge switch
    * statement here to handle all Windows messages of interest.
    */

   switch(Message) {

      /*********************
       * WM_DESTROY 
       *********************/
      case WM_DESTROY:
      {
         HDC hdc;

         /* the old palette has already been selected into window's DC (see message handlers for
          * WM_PALETTECHANGED and WM_QUERYNEWPALETTE) so that the upcoming destruction of the
          * custom palette (done in close_screen()) is unproblematic.
          */

         PostQuitMessage(0);
         return 0;
         /* break;    normally we would need this, but after a 'return' it's redundant */ 
      }
#ifdef NOTDEF

      /*********************
       * WM_SIZE
       *********************/
      case WM_SIZE:
      {
         Tworld *pWorld;

         /* we use the cbWndExtra bytes to store a pointer to the particular
          * Tworld object that this particular instance of a window points to.
          * (different instances in the same program could point to different
          * Tworlds, and since this ia a static function, we have no "this"
          * pointer to know from which Tworld this function was called)
          */
         pWorld = ((Tworld *)GetWindowLong(hWindow,0));

         /* determination of client window size */
         pWorld->screen.winsize_x = LOWORD(lp);
         pWorld->screen.winsize_y = HIWORD(lp);
      }
      return 0;
      /* break;    normally we would need this, but after a 'return' it's redundant */

#endif

      /*********************
       * WM_CHAR
       *********************/
       case WM_CHAR:
       {
  	last_pipeline->key_event(wp);  // will be overridden by application
          last_pipeline->update_event();   // will be overridden by application
          last_pipeline->draw_event();     // will be overridden by application
          return 0;
        }

//        /*********************
//         * WM_TIMER
//         *********************/
//        case WM_TIMER:
//        {
//          last_pipeline->update_event();   // will be overridden by application
//          last_pipeline->draw_event();     // will be overridden by application
//          return 0;
//        }

      /*********************
       * WM_PALETTECHANGED
       *********************/

      case WM_PALETTECHANGED:
      {
  l3d_screen_mswin *screen;
  screen = (l3d_screen_mswin *)last_event_source;
         HDC hdc;
         HPALETTE hpalOld;
         UINT palChangedCount;

         /* If this application changed the palette, ignore the message. */
         if ((HWND) wp == hWindow)
            return 0;

         /* if our window receives this msg, re-realize the window's (background) palette */

         hdc = GetDC(hWindow); 
         /* last param is TRUE (== background palette) since this code is only reached if our app
          * does NOT have the focus */
         hpalOld = SelectPalette(hdc,screen->hpal, TRUE);
         palChangedCount = RealizePalette(hdc);
         SelectPalette(hdc,hpalOld, TRUE);
         ReleaseDC(hWindow,hdc);

         /* If any palette entries changed, repaint the window. */

         if (palChangedCount > 0) {
            InvalidateRect(hWindow, NULL, TRUE);
         }

         return palChangedCount;
      }
      /* break;    normally we would need this, but after a 'return' it's redundant */

      /*********************
       * WM_QUERYNEWPALETTE
       *********************/
      case WM_QUERYNEWPALETTE:
      {
  l3d_screen_mswin *screen;
  screen = (l3d_screen_mswin *)last_event_source;

         HPALETTE hpalOld;
         HDC hdc;
         UINT palChangedCount;

         /* if our window is activated, re-realize the window's palette */

         hdc = GetDC(hWindow);
         /* last param is FALSE (==not background palette) since this code is executed when our
          * app has the input focus */
         hpalOld = SelectPalette(hdc,screen->hpal, FALSE);
         palChangedCount = RealizePalette(hdc);
         SelectPalette(hdc,hpalOld, TRUE);
         ReleaseDC(hWindow,hdc);

         /* If any palette entries changed, repaint the window. */
         if (palChangedCount > 0) {
            InvalidateRect(hWindow, NULL, TRUE);
         }

         return palChangedCount;
      }
      /* break;    normally we would need this, but after a 'return' it's redundant */

      /*********************
       * All other messages
       *********************/
      default:

          last_pipeline->update_event();   // will be overridden by application
          last_pipeline->draw_event();     // will be overridden by application

         return DefWindowProc(hWindow, Message, wp, lp);
         /* break;    normally we would need this, but after a 'return' it's redundant */
   }
}
