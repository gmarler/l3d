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
#include <stdio.h>

#include "math/matrix.h"

int main(int argc, char *argv[]) {
  l3d_matrix a, b, c;
  l3d_point p;

  a.set(2,0,0,0,
        0,2,0,0,
        0,0,2,0,
        0,0,0,1);

  b.set(float_to_l3d_real(1.1),
        float_to_l3d_real(2.2),
        float_to_l3d_real(3.3),
        float_to_l3d_real(4.4),
        float_to_l3d_real(5.5),
        float_to_l3d_real(6.6),
        float_to_l3d_real(7.7),
        float_to_l3d_real(8.8),
        float_to_l3d_real(9.9),
        float_to_l3d_real(10.10),
        float_to_l3d_real(11.11),
        float_to_l3d_real(12.12),
        float_to_l3d_real(13.13),
        float_to_l3d_real(14.14),
        float_to_l3d_real(15.15),
        float_to_l3d_real(16.16)) ;

  p.set(5,6,7,1);

  printf("matrix a is:\n");
  a.print();

  printf("\nmatrix b is:\n");
  b.print();

  printf("\nmatrix a|b is:\n");
  (a|b).print();

  printf("\nmatrix c=a|b is:\n");
  c = a | b;
  c.print();

  printf("\nmatrix b|a is:\n");
  (b|a).print();

  printf("\npoint p is:\n");
  p.print();

  printf("\na|p is:\n");
  (a|p).print();

  printf("\na|b|p is:\n");
  (a|b|p).print();

  printf("\nc|p is:\n");
  (c|p).print();

  printf("\nb|a|p is:\n");
  (b|a|p).print();

  printf("\nmatrix c*2 is:\n");
  (c * int_to_l3d_real(2)).print();

  printf("\nmatrix c*2|p is:\n");
  (c*int_to_l3d_real(2)|p).print();
}
