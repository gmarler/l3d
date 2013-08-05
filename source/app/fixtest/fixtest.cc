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

#include <stdio.h>
#include "../lib/system/sys_dep.h"

int main(int argc, char **argv) {
  float delta, mindelta=1000, maxdelta=-1000;
  l3d_real x,y,z,fovx,fovy;
  int xsize=320,ysize=200;
  fovx = float_to_l3d_real(0.87);
  fovy = l3d_mulrr(fovx, float_to_l3d_real(1.3));

  for(z=int_to_l3d_real(-1001);
      z<int_to_l3d_real(1000);
      z+=int_to_l3d_real(10))
  {
    for(x=int_to_l3d_real(-1000);
        x<int_to_l3d_real(1000);
        x+=int_to_l3d_real(10))
    {

      int v;
      l3d_real ox,oy,nx,ny;
      ox =  x;
      nx =
        l3d_mulri(l3d_mulrr(l3d_divrr( x,z ),
                            fovx),
                  xsize)
        +
        int_to_l3d_real(xsize >> 1);


      float ofx,ofy,ofz,nfx,nfy;
      ofx =	   l3d_real_to_float(ox);
      ofz =	   l3d_real_to_float(z);
      nfx = ofx/ofz*l3d_real_to_float(fovx)*xsize + xsize/2;

      delta =       nfx - l3d_real_to_float(nx);
      if(delta < mindelta) mindelta = delta;
      if(delta > maxdelta) maxdelta = delta;


      // if(delta < -0.5 || delta > 0.5)
      /*
       {

      printf("DELTA %f : %f/%f to %f, ",
             nfx - l3d_real_to_float(nx),
      	   l3d_real_to_float(ox),
      	   l3d_real_to_float(z),
      	   l3d_real_to_float(nx));
          printf("F: %f \n", nfx);
       }
      */


    }
  }
  printf("min delta %f max delta %f\n", mindelta, maxdelta);
}

