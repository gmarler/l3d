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

#ifndef __MATRIX_H
#define __MATRIX_H
#include "../tool_os/memman.h"

#include <stdio.h>
#include <math.h>
#include "../system/sys_dep.h"
#include "math_fix.h"
#include "sincos.h"
#include "vector.h"
#include "../geom/point/point.h"

//  /*
//   * Class l3d_matrix: a matrix in homogeneous coordinates of the form
//   *
//   * (a00  a01  a02  a03)
//   * (a10  a11  a12  a13)
//   * (a20  a21  a22  a23)
//   * (a30  a31  a32  a33)
//   *
//   * may be thought of as a transformation matrix used to rotate,
//   * scale, or translate a l3d_vector object.
//   *
//   */

class l3d_matrix {
  public:
    // constructors and set-routines
    l3d_matrix(void);  // default constructor -- set to zero
    l3d_matrix(l3d_real a00,l3d_real a01,l3d_real a02,l3d_real a03,
               l3d_real a10,l3d_real a11,l3d_real a12,l3d_real a13,
               l3d_real a20,l3d_real a21,l3d_real a22,l3d_real a23,
               l3d_real a30,l3d_real a31,l3d_real a32,l3d_real a33);
    l3d_matrix(int a00,int a01,int a02,int a03,
               int a10,int a11,int a12,int a13,
               int a20,int a21,int a22,int a23,
               int a30,int a31,int a32,int a33);
    l3d_matrix(const l3d_matrix &right);  // copy constructor
    const l3d_matrix &set(l3d_real a00,l3d_real a01,l3d_real a02,l3d_real a03,
                          l3d_real a10,l3d_real a11,l3d_real a12,l3d_real a13,
                          l3d_real a20,l3d_real a21,l3d_real a22,l3d_real a23,
                          l3d_real a30,l3d_real a31,l3d_real a32,l3d_real a33);
    const l3d_matrix &set(int a00,int a01,int a02,int a03,
                          int a10,int a11,int a12,int a13,
                          int a20,int a21,int a22,int a23,
                          int a30,int a31,int a32,int a33);

    // operators
    l3d_matrix &operator = (const l3d_matrix &right); // /* assignment      */
    //  l3d_matrix &operator + (const l3d_matrix &right);  /* addition        */
    //  l3d_matrix &operator - (const l3d_matrix &right);  /* subtraction     */
    //  l3d_matrix &operator * (const l3d_real aScalar);     /* scalar multiply */
    //  l3d_matrix &operator |(const l3d_matrix &right);/* composition w/matrix */
    //  l3d_vector &operator |(const l3d_vector &right);      /* composition w/vector */

    void print(void)
    {
      int i,j;
      for(j=0;j<4;j++) {
        for(i=0;i<4;i++) {
          printf("%f ", l3d_real_to_float(a[j][i]));
        }
        printf("\n");
      }
    };

    l3d_real a[4][4];  // actual matrix values
};

extern l3d_matrix &l3d_get_matrix_temp(void);


//- useful defines for accessing matrix elements occurring in standard
//- positions for l3d_vector:

#define X_ a[0]
#define Y_ a[1]
#define Z_ a[2]
#define W_ a[3]

#define U_ a[0]
#define V_ a[1]
#define N_ a[2]

//  /*****************************************************************************
//   * l3d_matrix default constructor
//   *****************************************************************************/
inline l3d_matrix::l3d_matrix() {

  //    /* technically we're being a little loose here with assignment of */
  //    /* uncasted "0"s... they should be int_to_l3d_real(0), but for speed we    */
  //    /* eliminate the (inline) function call.                          */
  a[0][0]=0; a[0][1]=0; a[0][2]=0; a[0][3]=0;
  a[1][0]=0; a[1][1]=0; a[1][2]=0; a[1][3]=0;
  a[2][0]=0; a[2][1]=0; a[2][2]=0; a[2][3]=0;
  a[3][0]=0; a[3][1]=0; a[3][2]=0; a[3][3]=0;
}

//  /*****************************************************************************
//   * l3d_matrix constructor given set of initializing l3d_real values
//   *****************************************************************************/
inline l3d_matrix::l3d_matrix(l3d_real a00,l3d_real a01,l3d_real a02,l3d_real a03,
                              l3d_real a10,l3d_real a11,l3d_real a12,l3d_real a13,
                              l3d_real a20,l3d_real a21,l3d_real a22,l3d_real a23,
                              l3d_real a30,l3d_real a31,l3d_real a32,l3d_real a33)
{
  a[0][0]=a00; a[0][1]=a01; a[0][2]=a02; a[0][3]=a03;
  a[1][0]=a10; a[1][1]=a11; a[1][2]=a12; a[1][3]=a13;
  a[2][0]=a20; a[2][1]=a21; a[2][2]=a22; a[2][3]=a23;
  a[3][0]=a30; a[3][1]=a31; a[3][2]=a32; a[3][3]=a33;

}

//  /*****************************************************************************
//   * l3d_matrix constructor given set of initializing int values
//   *****************************************************************************/
inline l3d_matrix::l3d_matrix(int a00,int a01,int a02,int a03,
                              int a10,int a11,int a12,int a13,
                              int a20,int a21,int a22,int a23,
                              int a30,int a31,int a32,int a33)
{
  a[0][0]=int_to_l3d_real(a00); a[0][1]=int_to_l3d_real(a01);
  a[0][2]=int_to_l3d_real(a02); a[0][3]=int_to_l3d_real(a03);

  a[1][0]=int_to_l3d_real(a10); a[1][1]=int_to_l3d_real(a11);
  a[1][2]=int_to_l3d_real(a12); a[1][3]=int_to_l3d_real(a13);

  a[2][0]=int_to_l3d_real(a20); a[2][1]=int_to_l3d_real(a21);
  a[2][2]=int_to_l3d_real(a22); a[2][3]=int_to_l3d_real(a23);

  a[3][0]=int_to_l3d_real(a30); a[3][1]=int_to_l3d_real(a31);
  a[3][2]=int_to_l3d_real(a32); a[3][3]=int_to_l3d_real(a33);
}

//  /*****************************************************************************
//   * l3d_matrix copy constructor
//   *****************************************************************************/
inline l3d_matrix::l3d_matrix(const l3d_matrix &right) {
  a[0][0]=right.a[0][0]; a[0][1]=right.a[0][1];
  a[0][2]=right.a[0][2]; a[0][3]=right.a[0][3];

  a[1][0]=right.a[1][0]; a[1][1]=right.a[1][1];
  a[1][2]=right.a[1][2]; a[1][3]=right.a[1][3];

  a[2][0]=right.a[2][0]; a[2][1]=right.a[2][1];
  a[2][2]=right.a[2][2]; a[2][3]=right.a[2][3];

  a[3][0]=right.a[3][0]; a[3][1]=right.a[3][1];
  a[3][2]=right.a[3][2]; a[3][3]=right.a[3][3];
}

//  /*****************************************************************************
//   * l3d_matrix assignment operator
//   *****************************************************************************/
inline l3d_matrix &l3d_matrix::operator = (const l3d_matrix &right) {
  a[0][0]=right.a[0][0]; a[0][1]=right.a[0][1];
  a[0][2]=right.a[0][2]; a[0][3]=right.a[0][3];

  a[1][0]=right.a[1][0]; a[1][1]=right.a[1][1];
  a[1][2]=right.a[1][2]; a[1][3]=right.a[1][3];

  a[2][0]=right.a[2][0]; a[2][1]=right.a[2][1];
  a[2][2]=right.a[2][2]; a[2][3]=right.a[2][3];

  a[3][0]=right.a[3][0]; a[3][1]=right.a[3][1];
  a[3][2]=right.a[3][2]; a[3][3]=right.a[3][3];

  return *this;
}

//  /*****************************************************************************
//   * l3d_matrix "set" operation given set of initializing l3d_real values
//   *****************************************************************************/
inline const l3d_matrix &l3d_matrix::set(l3d_real a00,l3d_real a01,l3d_real a02,l3d_real a03,
    l3d_real a10,l3d_real a11,l3d_real a12,l3d_real a13,
    l3d_real a20,l3d_real a21,l3d_real a22,l3d_real a23,
    l3d_real a30,l3d_real a31,l3d_real a32,l3d_real a33)
{
  a[0][0]=a00; a[0][1]=a01; a[0][2]=a02; a[0][3]=a03;
  a[1][0]=a10; a[1][1]=a11; a[1][2]=a12; a[1][3]=a13;
  a[2][0]=a20; a[2][1]=a21; a[2][2]=a22; a[2][3]=a23;
  a[3][0]=a30; a[3][1]=a31; a[3][2]=a32; a[3][3]=a33;

  return *this;
}

//  /*****************************************************************************
//   * l3d_matrix "set" operation given set of initializing int values
//   *****************************************************************************/
inline const l3d_matrix &l3d_matrix::set(int a00,int a01,int a02,int a03,
    int a10,int a11,int a12,int a13,
    int a20,int a21,int a22,int a23,
    int a30,int a31,int a32,int a33)
{
  a[0][0]=int_to_l3d_real(a00); a[0][1]=int_to_l3d_real(a01);
  a[0][2]=int_to_l3d_real(a02); a[0][3]=int_to_l3d_real(a03);

  a[1][0]=int_to_l3d_real(a10); a[1][1]=int_to_l3d_real(a11);
  a[1][2]=int_to_l3d_real(a12); a[1][3]=int_to_l3d_real(a13);

  a[2][0]=int_to_l3d_real(a20); a[2][1]=int_to_l3d_real(a21);
  a[2][2]=int_to_l3d_real(a22); a[2][3]=int_to_l3d_real(a23);

  a[3][0]=int_to_l3d_real(a30); a[3][1]=int_to_l3d_real(a31);
  a[3][2]=int_to_l3d_real(a32); a[3][3]=int_to_l3d_real(a33);

  return *this;
}


inline l3d_matrix& operator |(const l3d_matrix &l, const l3d_matrix &r) {
  l3d_matrix &res = l3d_get_matrix_temp();

  res.set(l3d_mulrr(l.a[0][0],r.a[0][0])
          +l3d_mulrr(l.a[0][1],r.a[1][0])
          +l3d_mulrr(l.a[0][2],r.a[2][0])
          +l3d_mulrr(l.a[0][3],r.a[3][0]),
          l3d_mulrr(l.a[0][0],r.a[0][1])
          +l3d_mulrr(l.a[0][1],r.a[1][1])
          +l3d_mulrr(l.a[0][2],r.a[2][1])
          +l3d_mulrr(l.a[0][3],r.a[3][1]),
          l3d_mulrr(l.a[0][0],r.a[0][2])
          +l3d_mulrr(l.a[0][1],r.a[1][2])
          +l3d_mulrr(l.a[0][2],r.a[2][2])
          +l3d_mulrr(l.a[0][3],r.a[3][2]),
          l3d_mulrr(l.a[0][0],r.a[0][3])
          +l3d_mulrr(l.a[0][1],r.a[1][3])
          +l3d_mulrr(l.a[0][2],r.a[2][3])
          +l3d_mulrr(l.a[0][3],r.a[3][3]),
          l3d_mulrr(l.a[1][0],r.a[0][0])
          +l3d_mulrr(l.a[1][1],r.a[1][0])
          +l3d_mulrr(l.a[1][2],r.a[2][0])
          +l3d_mulrr(l.a[1][3],r.a[3][0]),
          l3d_mulrr(l.a[1][0],r.a[0][1])
          +l3d_mulrr(l.a[1][1],r.a[1][1])
          +l3d_mulrr(l.a[1][2],r.a[2][1])
          +l3d_mulrr(l.a[1][3],r.a[3][1]),
          l3d_mulrr(l.a[1][0],r.a[0][2])
          +l3d_mulrr(l.a[1][1],r.a[1][2])
          +l3d_mulrr(l.a[1][2],r.a[2][2])
          +l3d_mulrr(l.a[1][3],r.a[3][2]),
          l3d_mulrr(l.a[1][0],r.a[0][3])
          +l3d_mulrr(l.a[1][1],r.a[1][3])
          +l3d_mulrr(l.a[1][2],r.a[2][3])
          +l3d_mulrr(l.a[1][3],r.a[3][3]),
          l3d_mulrr(l.a[2][0],r.a[0][0])
          +l3d_mulrr(l.a[2][1],r.a[1][0])
          +l3d_mulrr(l.a[2][2],r.a[2][0])
          +l3d_mulrr(l.a[2][3],r.a[3][0]),
          l3d_mulrr(l.a[2][0],r.a[0][1])
          +l3d_mulrr(l.a[2][1],r.a[1][1])
          +l3d_mulrr(l.a[2][2],r.a[2][1])
          +l3d_mulrr(l.a[2][3],r.a[3][1]),
          l3d_mulrr(l.a[2][0],r.a[0][2])
          +l3d_mulrr(l.a[2][1],r.a[1][2])
          +l3d_mulrr(l.a[2][2],r.a[2][2])
          +l3d_mulrr(l.a[2][3],r.a[3][2]),
          l3d_mulrr(l.a[2][0],r.a[0][3])
          +l3d_mulrr(l.a[2][1],r.a[1][3])
          +l3d_mulrr(l.a[2][2],r.a[2][3])
          +l3d_mulrr(l.a[2][3],r.a[3][3]),
          l3d_mulrr(l.a[3][0],r.a[0][0])
          +l3d_mulrr(l.a[3][1],r.a[1][0])
          +l3d_mulrr(l.a[3][2],r.a[2][0])
          +l3d_mulrr(l.a[3][3],r.a[3][0]),
          l3d_mulrr(l.a[3][0],r.a[0][1])
          +l3d_mulrr(l.a[3][1],r.a[1][1])
          +l3d_mulrr(l.a[3][2],r.a[2][1])
          +l3d_mulrr(l.a[3][3],r.a[3][1]),
          l3d_mulrr(l.a[3][0],r.a[0][2])
          +l3d_mulrr(l.a[3][1],r.a[1][2])
          +l3d_mulrr(l.a[3][2],r.a[2][2])
          +l3d_mulrr(l.a[3][3],r.a[3][2]),
          l3d_mulrr(l.a[3][0],r.a[0][3])
          +l3d_mulrr(l.a[3][1],r.a[1][3])
          +l3d_mulrr(l.a[3][2],r.a[2][3])
          +l3d_mulrr(l.a[3][3],r.a[3][3]));
  return res;
}

//  /*****************************************************************************
//   * l3d_matrix matrix-vector composition - returns vector
//   *****************************************************************************/
inline l3d_vector &operator |(const l3d_matrix &l, const  l3d_vector &r) {
  l3d_vector &res = l3d_get_vector_temp();

  res.set(
    l3d_mulrr(  l.a[0][0],r.a[0]) +
    l3d_mulrr(     l.a[0][1],r.a[1]) +
    l3d_mulrr(     l.a[0][2],r.a[2]) ,
    //- r.a[3] is 0 for our vectors: +l3d_mulrr(     l.a[0][3],r.a[3]),
    l3d_mulrr(  l.a[1][0],r.a[0]) +
    l3d_mulrr(     l.a[1][1],r.a[1]) +
    l3d_mulrr(     l.a[1][2],r.a[2]) ,
    //- r.a[3] is 0 for our vectors:     l3d_mulrr(     l.a[1][3],r.a[3]),
    l3d_mulrr(  l.a[2][0],r.a[0]) +
    l3d_mulrr(     l.a[2][1],r.a[1]) +
    l3d_mulrr(     l.a[2][2],r.a[2]) ,
    //- r.a[3] is 0 for our vectors:     l3d_mulrr(     l.a[2][3],r.a[3]),

    //- l.a[3][0] is 0 for our matrices:    l3d_mulrr(  l.a[3][0],r.a[0]) +
    //- l.a[3][1] is 0 for our matrices:    l3d_mulrr(     l.a[3][1],r.a[1]) +
    //- l.a[3][2] is 0 for our matrices:    l3d_mulrr(     l.a[3][2],r.a[2]) +
    l3d_mulrr(     l.a[3][3],r.a[3]) ) ;

  return res;
}


//  /*****************************************************************************
//   * l3d_matrix scalar multiplication
//   *****************************************************************************/
inline l3d_matrix& operator * (const l3d_matrix &l,
                               const l3d_real aScalar)
{
  l3d_matrix &res = l3d_get_matrix_temp();
  res.set (
    l3d_mulrr(     l.a[0][0] , aScalar),
    l3d_mulrr(     l.a[0][1] , aScalar),
    l3d_mulrr(     l.a[0][2] , aScalar),
    l3d_mulrr(     l.a[0][3] , aScalar),

    l3d_mulrr(      l.a[1][0] , aScalar),
    l3d_mulrr(      l.a[1][1] , aScalar),
    l3d_mulrr(      l.a[1][2] , aScalar),
    l3d_mulrr(      l.a[1][3] , aScalar),

    l3d_mulrr(      l.a[2][0] , aScalar),
    l3d_mulrr(      l.a[2][1] , aScalar),
    l3d_mulrr(      l.a[2][2] , aScalar),
    l3d_mulrr(      l.a[2][3] , aScalar),

    l3d_mulrr(      l.a[3][0] , aScalar),
    l3d_mulrr(      l.a[3][1] , aScalar),
    l3d_mulrr(      l.a[3][2] , aScalar),
    l3d_mulrr(      l.a[3][3] , aScalar)) ;

  return res;
}

//  /*****************************************************************************
//   * non-member functions
//   *
//   * for speed sin and cos (for rotation matrices) are stored in lookup
//   * tables sin_lu and cos_lu; see sincos.h and sincos.cc
//   *****************************************************************************/


//  /*
//   * For compilers without a 64K DGROUP limit, we can declare these
//   * matrix functions inline in the header file.
//   */

//  /*****************************************************************************
//   * creates l3d_matrix to translate a point by offsets x y and z
//   *****************************************************************************/
inline  l3d_matrix &l3d_mat_translate(l3d_real x, l3d_real y, l3d_real z) {
  l3d_matrix &res = l3d_get_matrix_temp();

  res.set(int_to_l3d_real(1),int_to_l3d_real(0),int_to_l3d_real(0),x,
          int_to_l3d_real(0),int_to_l3d_real(1),int_to_l3d_real(0),y,
          int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(1),z,
          int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(1) );
  return res;
}

//  /*****************************************************************************
//   * creates l3d_matrix to scale a point by given x, y, and z scale factors
//   *****************************************************************************/
inline  l3d_matrix &l3d_mat_scale(l3d_real x, l3d_real y, l3d_real z) {
  l3d_matrix &res = l3d_get_matrix_temp();

  res.set(int_to_l3d_real(x),int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(0),
          int_to_l3d_real(0),int_to_l3d_real(y),int_to_l3d_real(0),int_to_l3d_real(0),
          int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(z),int_to_l3d_real(0),
          int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(1) );
  return res;
}

//  /*****************************************************************************
//   * creates l3d_matrix to rotate a point around X axis
//   *****************************************************************************/
inline  l3d_matrix &l3d_mat_rotx(int theta) {
  l3d_matrix &res = l3d_get_matrix_temp();

  res.set ( int_to_l3d_real(1),int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(0),
            int_to_l3d_real(0),cos_lu[theta],-sin_lu[theta],int_to_l3d_real(0),
            int_to_l3d_real(0),sin_lu[theta],cos_lu[theta],int_to_l3d_real(0),
            int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(1) );
  return res;
}

//  /*****************************************************************************
//   * creates l3d_matrix to rotate a point around Y axis
//   *****************************************************************************/
inline l3d_matrix &l3d_mat_roty(int theta) {
  l3d_matrix &res = l3d_get_matrix_temp();

  res.set(cos_lu[theta],int_to_l3d_real(0),sin_lu[theta],int_to_l3d_real(0),
          int_to_l3d_real(0),int_to_l3d_real(1),int_to_l3d_real(0),int_to_l3d_real(0),
          -sin_lu[theta],int_to_l3d_real(0),cos_lu[theta],int_to_l3d_real(0),
          int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(1) );
  return res;
}

//  /*****************************************************************************
//   * creates l3d_matrix to rotate a point around Z axis
//   *****************************************************************************/
inline  l3d_matrix &l3d_mat_rotz(int theta) {
  l3d_matrix &res = l3d_get_matrix_temp();

  res.set(cos_lu[theta],-sin_lu[theta],int_to_l3d_real(0),int_to_l3d_real(0),
          sin_lu[theta],cos_lu[theta],int_to_l3d_real(0),int_to_l3d_real(0),
          int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(1),int_to_l3d_real(0),
          int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(1));
  return res;
}

//  /*****************************************************************************
//   * creates l3d_matrix to rotate a point around arbitrary vector U
//   *****************************************************************************/
inline  l3d_matrix &l3d_mat_rotu(const l3d_vector &u, int theta) {
  l3d_matrix &res = l3d_get_matrix_temp();

  res.set
  (l3d_mulrr(u.X_,u.X_) + l3d_mulrr(cos_lu[theta], int_to_l3d_real(1)-l3d_mulrr(u.X_,u.X_)),
   l3d_mulrr(l3d_mulrr(u.X_,u.Y_) , int_to_l3d_real(1)-cos_lu[theta])-l3d_mulrr(u.Z_,sin_lu[theta]),
   l3d_mulrr(l3d_mulrr(u.Z_,u.X_) , int_to_l3d_real(1)-cos_lu[theta])+l3d_mulrr(u.Y_,sin_lu[theta]),
   int_to_l3d_real(0),

   l3d_mulrr(l3d_mulrr(u.X_,u.Y_) , int_to_l3d_real(1)-cos_lu[theta])+l3d_mulrr(u.Z_,sin_lu[theta]),
   l3d_mulrr(u.Y_,u.Y_) + l3d_mulrr(cos_lu[theta], int_to_l3d_real(1)-l3d_mulrr(u.Y_,u.Y_)),
   l3d_mulrr(l3d_mulrr(u.Y_,u.Z_), int_to_l3d_real(1)-cos_lu[theta]) - l3d_mulrr(u.X_,sin_lu[theta]),
   int_to_l3d_real(0),

   l3d_mulrr(l3d_mulrr(u.Z_,u.X_), int_to_l3d_real(1)-cos_lu[theta]) - l3d_mulrr(u.Y_,sin_lu[theta]),
   l3d_mulrr(l3d_mulrr(u.Y_,u.Z_), int_to_l3d_real(1)-cos_lu[theta]) + l3d_mulrr(u.X_,sin_lu[theta]),
   l3d_mulrr(u.Z_,u.Z_) + l3d_mulrr(cos_lu[theta], int_to_l3d_real(1)-l3d_mulrr(u.Z_,u.Z_)),
   int_to_l3d_real(0),

   int_to_l3d_real(0),
   int_to_l3d_real(0),
   int_to_l3d_real(0),
   int_to_l3d_real(1) );

  return res;
}

//  /*****************************************************************************
//   * l3d_matrix matrix-point composition - returns new point
//   *****************************************************************************/
inline l3d_point &operator |(const l3d_matrix &l, const  l3d_point &r) {
  l3d_point &res = l3d_get_point_temp();

  res.set(
    l3d_mulrr(  l.a[0][0],r.a[0]) +
    l3d_mulrr(     l.a[0][1],r.a[1]) +
    l3d_mulrr(     l.a[0][2],r.a[2]) +
    //- r.a[3] is normally 1 for points:    l3d_mulrr(     l.a[0][3],r.a[3]),
    l.a[0][3]        ,
    l3d_mulrr(  l.a[1][0],r.a[0]) +
    l3d_mulrr(     l.a[1][1],r.a[1]) +
    l3d_mulrr(     l.a[1][2],r.a[2]) +
    //- r.a[3] is normally 1 for points:    l3d_mulrr(     l.a[1][3],r.a[3]),
    l.a[1][3],
    l3d_mulrr(  l.a[2][0],r.a[0]) +
    l3d_mulrr(     l.a[2][1],r.a[1]) +
    l3d_mulrr(     l.a[2][2],r.a[2]) +
    //- r.a[3] is normally 1 for points:    l3d_mulrr(     l.a[2][3],r.a[3]),
    l.a[2][3],
    //- l.a[3][0] is 0 for our matrices:    l3d_mulrr(  l.a[3][0],r.a[0]) +
    //- l.a[3][1] is 0 for our matrices:    l3d_mulrr(     l.a[3][1],r.a[1]) +
    //- l.a[3][2] is 0 for our matrices:    l3d_mulrr(     l.a[3][2],r.a[2]) +
    l3d_mulrr(     l.a[3][3],r.a[3]) ) ;

  return res;
}



#endif
