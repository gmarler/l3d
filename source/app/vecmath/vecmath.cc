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
#include "../lib/geom/point/point.h"
#include "../lib/math/vector.h"

int main(int argc, char **argv) {
  l3d_point p1(int_to_l3d_real(5),
               int_to_l3d_real(5),
               int_to_l3d_real(4),
               int_to_l3d_real(1)),
  p2(int_to_l3d_real(2),
     int_to_l3d_real(7),
     int_to_l3d_real(3),
     int_to_l3d_real(1));
  l3d_vector v1,v2,v3;
  l3d_real scalar;

  printf("Point P1 = (%f,%f,%f,%f)\n",
         l3d_real_to_float(p1.X_),
         l3d_real_to_float(p1.Y_),
         l3d_real_to_float(p1.Z_),
         l3d_real_to_float(p1.W_));
  printf("Point P2 = (%f,%f,%f,%f)\n",
         l3d_real_to_float(p2.X_),
         l3d_real_to_float(p2.Y_),
         l3d_real_to_float(p2.Z_),
         l3d_real_to_float(p2.W_));

  v1 = p2 - p1;
  printf("The vector V1 from P1 to P2 = P2 - P1 = (%f,%f,%f,%f)\n",
         l3d_real_to_float(v1.X_),
         l3d_real_to_float(v1.Y_),
         l3d_real_to_float(v1.Z_),
         l3d_real_to_float(v1.W_));

  v2.set(int_to_l3d_real(10),
         int_to_l3d_real(10),
         int_to_l3d_real(10),
         int_to_l3d_real(0));
  printf("A different vector V2 = (%f,%f,%f,%f)\n",
         l3d_real_to_float(v2.X_),
         l3d_real_to_float(v2.Y_),
         l3d_real_to_float(v2.Z_),
         l3d_real_to_float(v2.W_));

  p1 = p1 + v2;
  printf("Moving from P1 in the direction of V2 = P1 + V2 = (%f,%f,%f,%f)\n",
         l3d_real_to_float(p1.X_),
         l3d_real_to_float(p1.Y_),
         l3d_real_to_float(p1.Z_),
         l3d_real_to_float(p1.W_));

  v1 = v1 + v2;
  printf("The combined displacement of V1 and V2 = V1 + V2 = (%f,%f,%f,%f)\n",
         l3d_real_to_float(v1.X_),
         l3d_real_to_float(v1.Y_),
         l3d_real_to_float(v1.Z_),
         l3d_real_to_float(v1.W_));

  v1.set(int_to_l3d_real(1),
         int_to_l3d_real(0),
         int_to_l3d_real(0),
         int_to_l3d_real(0));
  v2.set(int_to_l3d_real(0),
         int_to_l3d_real(1),
         int_to_l3d_real(0),
         int_to_l3d_real(0));
  v3 = cross(v1,v2);
  printf("The cross product of (%f,%f,%f,%f) and (%f,%f,%f,%f) "
         "is (%f,%f,%f,%f)\n",
         l3d_real_to_float(v1.X_),
         l3d_real_to_float(v1.Y_),
         l3d_real_to_float(v1.Z_),
         l3d_real_to_float(v1.W_),
         l3d_real_to_float(v2.X_),
         l3d_real_to_float(v2.Y_),
         l3d_real_to_float(v2.Z_),
         l3d_real_to_float(v2.W_),
         l3d_real_to_float(v3.X_),
         l3d_real_to_float(v3.Y_),
         l3d_real_to_float(v3.Z_),
         l3d_real_to_float(v3.W_));

  v1.set(int_to_l3d_real(1),
         int_to_l3d_real(0),
         int_to_l3d_real(0),
         int_to_l3d_real(0));
  v2.set(int_to_l3d_real(0),
         int_to_l3d_real(1),
         int_to_l3d_real(0),
         int_to_l3d_real(0));
  v3 = cross(v2,v1);
  printf("The cross product of (%f,%f,%f,%f) and (%f,%f,%f,%f) "
         "is (%f,%f,%f,%f)\n",
         l3d_real_to_float(v2.X_),
         l3d_real_to_float(v2.Y_),
         l3d_real_to_float(v2.Z_),
         l3d_real_to_float(v2.W_),
         l3d_real_to_float(v1.X_),
         l3d_real_to_float(v1.Y_),
         l3d_real_to_float(v1.Z_),
         l3d_real_to_float(v1.W_),
         l3d_real_to_float(v3.X_),
         l3d_real_to_float(v3.Y_),
         l3d_real_to_float(v3.Z_),
         l3d_real_to_float(v3.W_));
  v1.set(int_to_l3d_real(1),
         int_to_l3d_real(0),
         int_to_l3d_real(0),
         int_to_l3d_real(0));
  v2.set(int_to_l3d_real(1),
         int_to_l3d_real(1),
         int_to_l3d_real(0),
         int_to_l3d_real(0));
  scalar = dot(v1,v2);
  printf("The dot product of (%f,%f,%f,%f) and (%f,%f,%f,%f) "
         "is %f\n",
         l3d_real_to_float(v1.X_),
         l3d_real_to_float(v1.Y_),
         l3d_real_to_float(v1.Z_),
         l3d_real_to_float(v1.W_),
         l3d_real_to_float(v2.X_),
         l3d_real_to_float(v2.Y_),
         l3d_real_to_float(v2.Z_),
         l3d_real_to_float(v2.W_),
         scalar);
}
