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

#ifndef __VECTOR_H
#define __VECTOR_H
#include "../tool_os/memman.h"

#include <math.h>
#include <stdio.h>
#include "../system/sys_dep.h"
#include "math_fix.h"
#include "sincos.h"
#include "../geom/point/point.h"

//
// Class l3d_vector: a 3d vector in homogeneous coordinates
// of the form (x,y,z,w). May be thought of as a vector or
// as a point in 3D space.
//

class l3d_vector {
  public:

    // constructors and set-routines
    l3d_vector(void);
    l3d_vector(l3d_real a0, l3d_real a1, l3d_real a2, l3d_real a3);
    l3d_vector(int a0, int a1, int a2, int a3);
    // l3d_vector(const l3d_vector &right);  // copy constructor
    const l3d_vector &set(l3d_real a0, l3d_real a1, l3d_real a2, l3d_real a3);
    const l3d_vector &set(int a0, int a1, int a2, int a3);

    // operators
    // const l3d_vector &operator = (const l3d_vector &right);  // assmt
    //   l3d_vector operator + (const l3d_vector &right);
    //   l3d_vector operator - (l3d_vector &right);
    //   const l3d_vector operator * (const l3d_real aScalar);

    // other operations
    //   l3d_vector cross(const l3d_vector &r); // vector cross product
    //   l3d_vector normalized(void);          // normalized (length=1) of vector
    //   const l3d_real dot(const l3d_vector &r); // vector dot product

    // data
    void print(void)
    {
      printf("%f %f %f %f\n",
             l3d_real_to_float(a[0]),
             l3d_real_to_float(a[1]),
             l3d_real_to_float(a[2]),
             l3d_real_to_float(a[3]));
    }
    l3d_real a[4]; // the 4 components (x,y,z,w) of the vector
};

//
//- useful defines for accessing matrix elements occurring in standard
//- positions for l3d_vector:
//
#define X_ a[0]
#define Y_ a[1]
#define Z_ a[2]
#define W_ a[3]

#define U_ a[0]
#define V_ a[1]
#define N_ a[2]

//- function for grabbing a temporary vector object from a global
//- pre-allocated pool of temporary objects.
// this is to reduce overhead of temporary object creation which
// occurs when using overloaded operators.
extern l3d_vector &l3d_get_vector_temp(void);


// l3d_vector default constructor
inline l3d_vector::l3d_vector(void) {
  a[0] = a[1] = a[2] = a[3] = 0;
}


// l3d_vector constructor given a set of initializing l3d_real values
inline l3d_vector::l3d_vector(l3d_real a0, l3d_real a1, l3d_real a2, l3d_real a3) {
  a[0] = a0;
  a[1] = a1;
  a[2] = a2;
  a[3] = a3;
}

// l3d_vector constructor given a set of initializing int values
inline l3d_vector::l3d_vector(int a0, int a1, int a2, int a3) {
  a[0] = int_to_l3d_real(a0);
  a[1] = int_to_l3d_real(a1);
  a[2] = int_to_l3d_real(a2);
  a[3] = int_to_l3d_real(a3);
}

//  /*
//  // l3d_vector copy constructor
//  inline l3d_vector::l3d_vector(const l3d_vector &right) {
//     a[0] = right.a[0];
//     a[1] = right.a[1];
//     a[2] = right.a[2];
//     a[3] = right.a[3];
//  }
//  */

//  /*
//  // l3d_vector assignment operator
//  inline const l3d_vector &l3d_vector::operator = (const l3d_vector &right) {
//     a[0] = right.a[0];
//     a[1] = right.a[1];
//     a[2] = right.a[2];
//     a[3] = right.a[3];

//     return *this;
//  }
//  */

// l3d_vector "set" operator given a set of mtype values
inline const l3d_vector &l3d_vector::set(l3d_real a0, l3d_real a1, l3d_real a2, l3d_real a3) {
  a[0] = a0;
  a[1] = a1;
  a[2] = a2;
  a[3] = a3;

  return *this;
}

// l3d_vector "set" operator given a set of int values
inline const l3d_vector &l3d_vector::set(int a0, int a1, int a2, int a3) {
  a[0] = int_to_l3d_real(a0);
  a[1] = int_to_l3d_real(a1);
  a[2] = int_to_l3d_real(a2);
  a[3] = int_to_l3d_real(a3);

  return *this;
}


// l3d_vector scalar multiplication
inline const l3d_vector &operator * (const l3d_vector& l,
                                     const l3d_real aScalar)
{
  l3d_vector &res = l3d_get_vector_temp();
  res.set(
    l3d_mulrr(l.a[0], /* TIMES */ aScalar),
    l3d_mulrr(l.a[1], /* TIMES */ aScalar),
    l3d_mulrr(l.a[2], /* TIMES */ aScalar),
    l3d_mulrr(l.a[3], /* TIMES */ aScalar));

  return res;
}

// l3d_vector 3D vector cross product operation - returns a vector
inline const l3d_vector& cross(const l3d_vector &l, const l3d_vector &r) {
  l3d_vector &res = l3d_get_vector_temp();

   #define V(q) l.a[(q-1)]
   #define W(q) r.a[(q-1)]

  res.set(
    l3d_mulrr(V(2),W(3)) - l3d_mulrr(V(3),W(2)),
    l3d_mulrr(V(3),W(1)) - l3d_mulrr(V(1),W(3)),
    l3d_mulrr(V(1),W(2)) - l3d_mulrr(V(2),W(1)),
    int_to_l3d_real(0) );
  // theoretically we could also multiply the fourth component of the
  // vector in the cross product to be 100% "correct", but this component
  // will always be 0 for vectors in 3D, thus we skip it and assign
  // 0 to the fourth component explicitly.

   #undef V
   #undef W

  return res;
}

// l3d_vector dot product operation - returns a scalar
inline const l3d_real dot(const l3d_vector &l, const l3d_vector &r) {
  return l3d_mulrr(l.a[0],r.a[0])
         + l3d_mulrr(l.a[1],r.a[1])
         + l3d_mulrr(l.a[2],r.a[2])
         + l3d_mulrr(l.a[3],r.a[3]) ;

  // homogeneous W component should always be 0 for vectors in 3D, thus
  // the contribution of this component should be zero
}

// l3d_vector normalizing operation - returns vector pointing in the same
// direction with magnitude 1

inline const l3d_vector& normalized(const l3d_vector &l) {
  l3d_vector &res = l3d_get_vector_temp();
  l3d_real   mm_vec_magnitude;

  // get current magnitude of vector
  mm_vec_magnitude = l3d_sqrt( dot(l,l) );
  //    mm_vec_magnitude =
  //      l3d_sqrt( l3d_mulrr(l.a[0] , l.a[0]) +
  // 	       l3d_mulrr(l.a[1] , l.a[1]) +
  // 	       l3d_mulrr(l.a[2] , l.a[2])  );

  // magnitude must not be zero, since division by 0 is illegal. if we
  // are using fixed point division, dividing by small values (e.g.
  // < 0.01) is the same as division by zero due to loss of precision.
  // We therefore check before division of the magnitude is not
  // "too small"

  if ((mm_vec_magnitude > float_to_l3d_real(0.01))
      || (mm_vec_magnitude < float_to_l3d_real(-0.01)))
  {

    // divide X, Y, and Z components of vector by magnitude, resulting
    //    in an overall magnitude of 1 for the vector

    res.set(
      l3d_divrr(l.a[0],mm_vec_magnitude),
      l3d_divrr(l.a[1],mm_vec_magnitude),
      l3d_divrr(l.a[2],mm_vec_magnitude),
      l3d_divrr(l.a[3],mm_vec_magnitude)); // a[3] should already be 0 in 3D
  }
  //
  // if we get here, we couldn't normalize the vector for some reason.
  // for debugging the following block could be uncommented.
  //
  //      else {
  //          printf("couldn't normalize vector\n");
  //      }

  return res;
}

inline const l3d_vector& operator + (const l3d_vector &l,
                                     const l3d_vector &right)
{
  l3d_vector& res = l3d_get_vector_temp();
  res.set(
    l.X_ + right.X_,
    l.Y_ + right.Y_,
    l.Z_ + right.Z_,
    l.W_ + right.W_); // w component should always be 0 for vectors
  // in 3D space
  return res;
}

inline const l3d_vector& operator - (const l3d_vector &l,
                                     const l3d_vector &right)
{
  l3d_vector& res = l3d_get_vector_temp();
  res.set(
    l.X_ - right.X_,
    l.Y_ - right.Y_,
    l.Z_ - right.Z_,
    l.W_ - right.W_); // w component should always be 0 for vectors in 3D

  return res;
}

// point-vector operations
inline const l3d_point& operator + (const l3d_point &l,
                                    const l3d_vector &right)
{
  l3d_point& res = l3d_get_point_temp();
  res.set(
    l.X_ + right.X_,
    l.Y_ + right.Y_,
    l.Z_ + right.Z_,
    l.W_ + right.W_); // w component should always be 0 for vectors in 3D

  return res;
}

inline const l3d_point& operator + (const l3d_vector &l,
                                    const l3d_point &right)
{
  l3d_point& res = l3d_get_point_temp();
  res.set(
    l.X_ + right.X_,
    l.Y_ + right.Y_,
    l.Z_ + right.Z_,
    l.W_ + right.W_); // w component should always be 0 for vectors in 3D

  return res;
}

inline const l3d_vector& operator - (const l3d_point &l,
                                     const l3d_point &right)
{
  l3d_vector& res = l3d_get_vector_temp();
  res.set(
    l.X_ - right.X_,
    l.Y_ - right.Y_,
    l.Z_ - right.Z_,
    l.W_ - right.W_); // w component should end up being 0; the diff
  // between 2 points in 3D -> both w are 1 -> resulting
  // w for the vector is 0

  return res;
}

inline const l3d_point& operator - (const l3d_point &l,
                                    const l3d_vector &right)
{
  l3d_point& res = l3d_get_point_temp();
  res.set(
    l.X_ - right.X_,
    l.Y_ - right.Y_,
    l.Z_ - right.Z_,
    l.W_ - right.W_); // w component should always be 0 for vectors in 3D

  return res;
}

#endif
