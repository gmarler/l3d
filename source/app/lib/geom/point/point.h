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

#ifndef __POINT_H
#define __POINT_H
#include "../../tool_os/memman.h"

#include <math.h>
#include <stdio.h>
#include "../../system/sys_dep.h"

//
// Class l3d_point: a 3d point  in homogeneous coordinates
// of the form (x,y,z,w).
//

class l3d_point  {
  public:

    // constructors and set-routines
    l3d_point(void);
    l3d_point(l3d_real a0, l3d_real a1, l3d_real a2, l3d_real a3);
    l3d_point(int a0, int a1, int a2, int a3);
    l3d_point(const l3d_point &right);  // copy constructor
    const l3d_point &set(l3d_real a0, l3d_real a1, l3d_real a2, l3d_real a3);
    const l3d_point  &set(int a0, int a1, int a2, int a3);

    // operators
    const l3d_point &operator = (const l3d_point &right);  // assmt

    void print(void)
    {
      printf("%f %f %f %f\n",
             l3d_real_to_float(a[0]),
             l3d_real_to_float(a[1]),
             l3d_real_to_float(a[2]),
             l3d_real_to_float(a[3]));
    }

    // data
    l3d_real a[4]; // the 4 components (x,y,z,w) of the point
};

//
//- useful defines for accessing matrix elements occurring in standard
//- positions for l3d_point:
//
#define X_ a[0]
#define Y_ a[1]
#define Z_ a[2]
#define W_ a[3]

//- function for grabbing a temporary point object from a global
//- pre-allocated pool of temporary objects.
// this is to reduce overhead of temporary object creation which
// occurs when using overloaded operators.
extern l3d_point &l3d_get_point_temp(void);


// l3d_point  default constructor
inline l3d_point::l3d_point(void) {
  a[0] = a[1] = a[2] = a[3] = 0;
}


// l3d_point constructor given a set of initializing l3d_real values
inline l3d_point::l3d_point(l3d_real a0, l3d_real a1, l3d_real a2, l3d_real a3) {
  a[0] = a0;
  a[1] = a1;
  a[2] = a2;
  a[3] = a3;
}

// l3d_point constructor given a set of initializing int values
inline l3d_point::l3d_point(int a0, int a1, int a2, int a3) {
  a[0] = int_to_l3d_real(a0);
  a[1] = int_to_l3d_real(a1);
  a[2] = int_to_l3d_real(a2);
  a[3] = int_to_l3d_real(a3);
}

// l3d_point copy constructor
inline l3d_point::l3d_point(const l3d_point &right) {
  a[0] = right.a[0];
  a[1] = right.a[1];
  a[2] = right.a[2];
  a[3] = right.a[3];
}

// l3d_point assignment operator
inline const l3d_point &l3d_point::operator = (const l3d_point &right) {
  a[0] = right.a[0];
  a[1] = right.a[1];
  a[2] = right.a[2];
  a[3] = right.a[3];

  return *this;
}

// l3d_point "set" operator given a set of mtype values
inline const l3d_point &l3d_point::set(l3d_real a0, l3d_real a1, l3d_real a2, l3d_real a3) {
  a[0] = a0;
  a[1] = a1;
  a[2] = a2;
  a[3] = a3;

  return *this;
}

// l3d_point "set" operator given a set of int values
inline const l3d_point &l3d_point::set(int a0, int a1, int a2, int a3) {
  a[0] = int_to_l3d_real(a0);
  a[1] = int_to_l3d_real(a1);
  a[2] = int_to_l3d_real(a2);
  a[3] = int_to_l3d_real(a3);

  return *this;
}


// l3d_point scalar multiplication
inline const l3d_point &operator * (const l3d_point& l,
                                    const l3d_real aScalar)
{
  l3d_point &res = l3d_get_point_temp();
  res.set(
    l3d_mulrr(l.a[0], /* TIMES */ aScalar),
    l3d_mulrr(l.a[1], /* TIMES */ aScalar),
    l3d_mulrr(l.a[2], /* TIMES */ aScalar),
    l3d_mulrr(l.a[3], /* TIMES */ aScalar));

  return res;
}

#endif
