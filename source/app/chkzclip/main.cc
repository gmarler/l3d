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

#include "world.h"
#include "shapes.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>


class Tmyworld:public Tworld {
  public:
    Tmyworld::Tmyworld(void);
    /* virtual */ void Tmyworld::updateall(void);
    // no destructor necessary for objects created in constructor;
    // base class destructor takes care of these.
};

/* virtual */ void Tmyworld::updateall(void) {
  Tworld::updateall();
  light = camera.VRP;
}

Tmyworld::Tmyworld(void) {
  int i,j,k,onum=0;

  light.set(0,0,0,1);
  i=10; j=0; k=20;
  k=0;

  objects[onum] = new Tcube;
  if (objects[onum]==NULL) exit;
  objects[onum]->modeling_xforms[1].set
  ( Int2MT(1), Int2MT(0), Int2MT(0), Int2MT(0),
    Int2MT(0), Int2MT(1), Int2MT(0), Int2MT(0),
    Int2MT(0), Int2MT(0), Int2MT(1), Int2MT(0),
    Int2MT(0), Int2MT(0), Int2MT(0), Int2MT(1) );
  objects[onum]->modeling_xform =
    objects[onum]->modeling_xforms[1] | (
      objects[onum]->modeling_xforms[0] );
  onum++;

  num_objects = onum;
}

#if (defined DOS_OS) || (defined X11_OS)

int main(int argc, char *argv[])
{
  Tmyworld aWorld;
  init_textures();
  aWorld.camera.near_z = Int2MT(10);
  aWorld.camera.far_z = Int2MT(500);
  aWorld.texmap_flag=0;
  aWorld.event_loop();

  printf("done\n");
  printf("%d frames\n%d last, %d first\n%f fps\n",
         aWorld.framecount,
         aWorld.last_tickcount,aWorld.first_tickcount,
         aWorld.framecount /  ((aWorld.last_tickcount-aWorld.first_tickcount)/18.2));

  return 0;
}

#elif defined WINDOWS_OS

int WINAPI WinMain (HINSTANCE hInst,
HINSTANCE hPrevInstance,
char * lpszCmdParam,
int nCmdShow)
/*
int pascal WinMain(HINSTANCE hInst,
HINSTANCE hPrevInstance,
LPSTR lpszCmdParam,
int nCmdShow)
*/
{

  Tmyworld aWorld;

  static char Name[] = "S3D 3D Engine";
  HWND hwnd;
  MSG Msg;
  WNDCLASS W;

  printf("start\n");
  memset(&W, 0, sizeof(WNDCLASS));
  printf("memset\n");
  W.style = CS_HREDRAW | CS_VREDRAW;
  printf("style\n");
  W.lpfnWndProc = Tworld::WorldWndProc;
  printf("wndprc\n");
  W.hInstance = hInst;
  printf("hinst\n");
  //   W.hbrBackground = GetStockBrush(WHITE_BRUSH);
  W.lpszClassName = Name;
  printf("classname\n");
  W.cbWndExtra = sizeof(long);
  printf("extra\n");

  RegisterClass(&W);
  printf("registered\n");

  hwnd = CreateWindow(Name, Name, WS_OVERLAPPEDWINDOW,
                      10, 10, 600, 400, NULL, NULL, hInst, NULL);
  printf("created\n");


  SetWindowLong(hwnd, 0, (long)( (Tworld *)(&aWorld)) );
  printf("long\n");


  aWorld.screen.hDisplaywnd = hwnd;
  printf("hDisplaywnd\n");
  printf("aworld is %p\n", &aWorld);
  printf("aworld.screen is %p\n", &aWorld.screen);
  printf("aworld.screen.xsize is %d\n", aWorld.screen.xsize);
  printf("aworld.screen.ysize is %d\n", aWorld.screen.ysize);
  aWorld.texmap_flag=0;
  aWorld.screen.open_screen();
  printf("open screen\n");
  init_textures();
  printf("init textures\n");

  ShowWindow(hwnd, nCmdShow);
  printf("showwindow\n");
  UpdateWindow(hwnd);
  printf("updatewindow\n");


  while(GetMessage(&Msg, NULL, 0, 0)) {
    TranslateMessage(&Msg);
    DispatchMessage(&Msg);
  }
  return Msg.wParam;
}

main() {
  WinMain(0,0,0,1);
}

#endif
