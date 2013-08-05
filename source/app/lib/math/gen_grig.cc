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

#include<stdio.h>
#include<math.h>

/*****************************************************************************
 * Helper program - generates sin/cos lookup table declarations to be
 * cut and pasted into sincos.cc
 *****************************************************************************/
main() {
  int theta;
  double rad;

  printf("Mtype cos_lu[360] = {\n");
  for(theta=0; theta<359; theta++) {
    rad = theta*PI/180.0;
    printf("   Float2MT(%f),\n", cos(rad) );
  }
  rad = theta*PI/180.0;
  printf("   Float2MT(%f)\n", cos(rad) );
  printf("}\n");

  printf("Mtype sin_lu[360] = {\n");
  for(theta=0; theta<359; theta++) {
    rad = theta*PI/180.0;
    printf("   Float2MT(%f),\n", sin(rad) );
  }
  rad = theta*PI/180.0;
  printf("   Float2MT(%f)\n", sin(rad) );
  printf("}\n");

}
