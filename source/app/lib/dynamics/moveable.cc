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
#include <math.h>
#include "moveable.h"
#include "../tool_os/memman.h"

l3d_moveable::l3d_moveable(void) {

  // initialize location (View Reference Point)

  VRP.set(int_to_l3d_real(50),int_to_l3d_real(5),int_to_l3d_real(50),int_to_l3d_real(0));

  // initialize camera orientation (View UP vector, View ForWard vector,
  //   View RIght vector

  // VUP, VFW, and VRI should all be normalized (magnitude = 1)

  VUP.set(float_to_l3d_real(0.0),float_to_l3d_real(1.0),float_to_l3d_real(0.0),float_to_l3d_real(0.0));
  VFW.set(float_to_l3d_real(0.0), float_to_l3d_real(0.0), float_to_l3d_real(1.0),float_to_l3d_real(0.0));
  VRI = cross(VUP,VFW);

  VUP_tip=VRP+VUP;
  VFW_tip=VRP+VFW;
  VRI_tip=VRP+VRI;

  // rotational and translational velocity initially 0

  VFW_velocity = int_to_l3d_real(0);
  VUP_velocity = int_to_l3d_real(0);
  VRI_velocity = int_to_l3d_real(0);
  VFW_thrust = int_to_l3d_real(0.2);
  VUP_thrust = int_to_l3d_real(0.2);
  VRI_thrust = int_to_l3d_real(0.2);

  rot_VUP_velocity = int_to_l3d_real(0);
  rot_VRI_velocity = int_to_l3d_real(0);
  rot_VFW_velocity = int_to_l3d_real(0);

  //     /* with all orientation parameters set, we can now calculate the
  //        viewing transformation matrix which transforms the arbitrary
  //        camera orientation specified by the above parameters into
  //        the "standard" camera orientation (camera at origin, looking
  //        straight along z axis, perfectly right-side-up */

  calculate_viewing_xform();
}


//  /*****************************************************************************
//   * Move the camera's location in the direction of the View ForWard vector
//   *****************************************************************************/
void l3d_moveable::translateVFW() {
  VRP.X_ = VRP.X_ + l3d_mulrr(VFW.X_, VFW_velocity);
  VUP_tip.X_ = VUP_tip.X_ + l3d_mulrr(VFW.X_, VFW_velocity);
  VFW_tip.X_ = VFW_tip.X_ + l3d_mulrr(VFW.X_, VFW_velocity);
  VRI_tip.X_ = VRI_tip.X_ + l3d_mulrr(VFW.X_, VFW_velocity);

  VRP.Y_ = VRP.Y_ + l3d_mulrr(VFW.Y_, VFW_velocity);
  VUP_tip.Y_ = VUP_tip.Y_ + l3d_mulrr(VFW.Y_, VFW_velocity);
  VFW_tip.Y_ = VFW_tip.Y_ + l3d_mulrr(VFW.Y_, VFW_velocity);
  VRI_tip.Y_ = VRI_tip.Y_ + l3d_mulrr(VFW.Y_, VFW_velocity);

  VRP.Z_ = VRP.Z_ + l3d_mulrr(VFW.Z_, VFW_velocity);
  VUP_tip.Z_ = VUP_tip.Z_ + l3d_mulrr(VFW.Z_, VFW_velocity);
  VFW_tip.Z_ = VFW_tip.Z_ + l3d_mulrr(VFW.Z_, VFW_velocity);
  VRI_tip.Z_ = VRI_tip.Z_ + l3d_mulrr(VFW.Z_, VFW_velocity);


  //     /* viewing xform matrix must be recalculated. we do this later,
  //        however, since multiple camera translations/rotations can
  //        occur and we do not want to calculate the viewing xform
  //        matrix at every intermediate stage; only at the end. */
}

//  /*****************************************************************************
//   * Move the camera's location in the direction of the View RIght vector
//   *****************************************************************************/
void l3d_moveable::translateVRI() {
  VRP.X_ = VRP.X_ + l3d_mulrr(VRI.X_, VRI_velocity);
  VUP_tip.X_ = VUP_tip.X_ + l3d_mulrr(VRI.X_, VRI_velocity);
  VFW_tip.X_ = VFW_tip.X_ + l3d_mulrr(VRI.X_, VRI_velocity);
  VRI_tip.X_ = VRI_tip.X_ + l3d_mulrr(VRI.X_, VRI_velocity);

  VRP.Y_ = VRP.Y_ + l3d_mulrr(VRI.Y_, VRI_velocity);
  VUP_tip.Y_ = VUP_tip.Y_ + l3d_mulrr(VRI.Y_, VRI_velocity);
  VFW_tip.Y_ = VFW_tip.Y_ + l3d_mulrr(VRI.Y_, VRI_velocity);
  VRI_tip.Y_ = VRI_tip.Y_ + l3d_mulrr(VRI.Y_, VRI_velocity);

  VRP.Z_ = VRP.Z_ + l3d_mulrr(VRI.Z_, VRI_velocity);
  VUP_tip.Z_ = VUP_tip.Z_ + l3d_mulrr(VRI.Z_, VRI_velocity);
  VFW_tip.Z_ = VFW_tip.Z_ + l3d_mulrr(VRI.Z_, VRI_velocity);
  VRI_tip.Z_ = VRI_tip.Z_ + l3d_mulrr(VRI.Z_, VRI_velocity);

  //     /* viewing xform matrix must be recalculated. we do this later,
  //        however, since multiple camera translations/rotations can
  //        occur and we do not want to calculate the viewing xform
  //        matrix at every intermediate stage; only at the end. */
}

//  /*****************************************************************************
//   * Move the camera's location in the direction of the View UP vector
//   *****************************************************************************/
void l3d_moveable::translateVUP() {
  VRP.X_ = VRP.X_ + l3d_mulrr(VUP.X_, VUP_velocity);
  VUP_tip.X_ = VUP_tip.X_ + l3d_mulrr(VUP.X_, VUP_velocity);
  VFW_tip.X_ = VFW_tip.X_ + l3d_mulrr(VUP.X_, VUP_velocity);
  VRI_tip.X_ = VRI_tip.X_ + l3d_mulrr(VUP.X_, VUP_velocity);

  VRP.Y_ = VRP.Y_ + l3d_mulrr(VUP.Y_, VUP_velocity);
  VUP_tip.Y_ = VUP_tip.Y_ + l3d_mulrr(VUP.Y_, VUP_velocity);
  VFW_tip.Y_ = VFW_tip.Y_ + l3d_mulrr(VUP.Y_, VUP_velocity);
  VRI_tip.Y_ = VRI_tip.Y_ + l3d_mulrr(VUP.Y_, VUP_velocity);

  VRP.Z_ = VRP.Z_ + l3d_mulrr(VUP.Z_, VUP_velocity);
  VUP_tip.Z_ = VUP_tip.Z_ + l3d_mulrr(VUP.Z_, VUP_velocity);
  VFW_tip.Z_ = VFW_tip.Z_ + l3d_mulrr(VUP.Z_, VUP_velocity);
  VRI_tip.Z_ = VRI_tip.Z_ + l3d_mulrr(VUP.Z_, VUP_velocity);

  //     /* viewing xform matrix must be recalculated. we do this later,
  //        however, since multiple camera translations/rotations can
  //        occur and we do not want to calculate the viewing xform
  //        matrix at every intermediate stage; only at the end. */
}

//  /*****************************************************************************
//   * Rotate the camera around the camera's View UP vector
//   *****************************************************************************/
void l3d_moveable::rotateVUP() {
  int angle=l3d_real_to_int(rot_VUP_velocity);
  l3d_matrix a;

  if(angle<0) angle+=360;  //- convert negative angle into 360-angle

  //     /* get rotation matrix for arbitrary vector (function rotu()) and
  //        apply it to VFW */
  VFW = l3d_mat_rotu(VUP, angle) |  VFW;
  VRI = cross(VUP,VFW);   //- calculate VRI based on VUP and new VFW

  //     /* viewing xform matrix must be recalculated. we do this later,
  //        however, since multiple camera translations/rotations can
  //        occur and we do not want to calculate the viewing xform
  //        matrix at every intermediate stage; only at the end. */
}

//  /*****************************************************************************
//   * Rotate the camera around the camera's View RIght vector
//   *****************************************************************************/
void l3d_moveable::rotateVRI() {
  int angle=l3d_real_to_int(rot_VRI_velocity);

  if(angle<0) angle+=360;  //- convert negative angle into 360-angle

  //     /* get rotation matrix for arbitrary vector (function rotu()) and
  //        apply it to VFW */
  VFW = l3d_mat_rotu(VRI,angle) | VFW;
  VUP = cross(VFW,VRI);  //- calculate VUP based on VRI and new VFW

  //     /* viewing xform matrix must be recalculated. we do this later,
  //        however, since multiple camera translations/rotations can
  //        occur and we do not want to calculate the viewing xform
  //        matrix at every intermediate stage; only at the end. */
}

//  /*****************************************************************************
//   * Rotate the camera around the camera's View ForWard vector
//   *****************************************************************************/
void l3d_moveable::rotateVFW() {
  int angle=l3d_real_to_int(rot_VFW_velocity);
  if(angle<0) angle+=360;
  //     /* get rotation matrix for arbitrary vector (function rotu()) and
  //        apply it to VUP */
  VUP = l3d_mat_rotu(VFW,angle) | VUP;
  VRI = cross(VUP,VFW);  //- calculate VRI based on VFW and new VUP

  //     /* viewing xform matrix must be recalculated. we do this later,
  //        however, since multiple camera translations/rotations can
  //        occur and we do not want to calculate the viewing xform
  //        matrix at every intermediate stage; only at the end. */
}

//  /*****************************************************************************
//   * update the camera's position based on current translational and rotational
//   * velocities
//   *****************************************************************************/
int l3d_moveable::update(void) {

  //     /* decay all velocities (translational and rotational */

  if(VFW_velocity != int_to_l3d_real(0)) {
    VFW_velocity = l3d_mulrr(VFW_velocity,float_to_l3d_real(0.95));
  }
  if(VRI_velocity != int_to_l3d_real(0)) {
    VRI_velocity = l3d_mulrr(VRI_velocity,float_to_l3d_real(0.95));
  }
  if(VUP_velocity != int_to_l3d_real(0)) {
    VUP_velocity = l3d_mulrr(VUP_velocity,float_to_l3d_real(0.95));
  }
  if(rot_VUP_velocity != int_to_l3d_real(0)) {
    rot_VUP_velocity = l3d_mulrr(rot_VUP_velocity,float_to_l3d_real(0.85));
  }
  if(rot_VRI_velocity != int_to_l3d_real(0)) {
    rot_VRI_velocity = l3d_mulrr(rot_VRI_velocity,float_to_l3d_real(0.85));
  }
  if(rot_VFW_velocity != int_to_l3d_real(0)) {
    rot_VFW_velocity = l3d_mulrr(rot_VFW_velocity,float_to_l3d_real(0.85));
  }

  //     /* rotate and translate for all velocities which are not 0 */

  if(rot_VUP_velocity) rotateVUP();
  if(rot_VRI_velocity) rotateVRI();
  if(rot_VFW_velocity) rotateVFW();

  if(VFW_velocity) translateVFW();
  if(VUP_velocity) translateVUP();
  if(VRI_velocity) translateVRI();

  //- It is important to normalize our camera vectors now, since repeated
  //- application of relative rotational transformations will eventually,
  //- due to numerical round off error, cause the length of these vectors
  //- to no longer be one. This subsequently causes problems in the calculation
  //- of the camera rotation matrix, which assumes that the vectors VFW,
  //- VRI, and VUP are all unit vectors (i.e. length 1). It would also be
  //- possible to do the normalization during computation of the camera
  //- rotation matrix, but this is not the approach chosen here.

  VFW = normalized(VFW);
  VRI = normalized(VRI);
  VUP = normalized(VUP);

  calculate_viewing_xform();  // recalculate viewing xform matrix

  return 0;  // no particularly special return value
}

//  /*****************************************************************************
//   * For a totally arbitrary camera position and orientation, calculate
//   * the transformation matrix which converts this into the standard
//   * camera orientation (camera at origin, looking straight along z axis,
//   * perfectly right-side-up)
//   *****************************************************************************/
void l3d_moveable::calculate_viewing_xform(void) {

  l3d_matrix R, TnVRP;
  l3d_matrix inverse_R, inverse_TnVRP;


  //- Translate VRP to origin


  TnVRP.set
  (int_to_l3d_real(1),int_to_l3d_real(0),int_to_l3d_real(0),-VRP.X_,
   int_to_l3d_real(0),int_to_l3d_real(1),int_to_l3d_real(0),-VRP.Y_,
   int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(1),-VRP.Z_,
   int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(0),
   int_to_l3d_real(1) );

  //-  Rotate VRI (View RIght vector)   into positive X-axis,
  //-         VUP (View UP vector)      into positive Y-axis,
  //-         VFW (View ForWard vector) into positive Z-axis.
  //      * Notes: First row of R is the unit vector rotated into the +ve X axis
  //      *        Second row of R is the unit vector rotated into the +ve Y axis
  //      *        Third row of R is the unit vector rotated into the +ve Z axis

  R.set
  ( VRI.X_,  VRI.Y_,  VRI.Z_,  int_to_l3d_real(0),
    VUP.X_,  VUP.Y_,  VUP.Z_,  int_to_l3d_real(0),
    VFW.X_,  VFW.Y_,  VFW.Z_,  int_to_l3d_real(0),
    int_to_l3d_real(0),
    int_to_l3d_real(0),
    int_to_l3d_real(0),
    int_to_l3d_real(1) );

  //     /* The composite matrix first applies the matrix to translate to
  //      * the origin, then applies the matrix to rotate the camera orientation
  //      * into the perfectly right-side-up and looking-straight-ahead
  //      * orientation. Remember transformation matrices are applied right
  //      * to left.
  //      */

  viewing_xform = R | TnVRP;


  //- calculate the inverse matrix by applying the inverse transformations
  //- in the reverse order.

  //- First, undo the rotation: the inverse is the transpose of the original
  //- rotation matrix.

  inverse_R.set
  ( VRI.X_,  VUP.X_,  VFW.X_,  int_to_l3d_real(0),
    VRI.Y_,  VUP.Y_,  VFW.Y_,  int_to_l3d_real(0),
    VRI.Z_,  VUP.Z_,  VFW.Z_,  int_to_l3d_real(0),
    int_to_l3d_real(0),
    int_to_l3d_real(0),
    int_to_l3d_real(0),
    int_to_l3d_real(1) );

  //- next, undo the translation: simply translate by the opposite amount

  inverse_TnVRP.set
  (int_to_l3d_real(1),int_to_l3d_real(0),int_to_l3d_real(0),VRP.X_,
   int_to_l3d_real(0),int_to_l3d_real(1),int_to_l3d_real(0),VRP.Y_,
   int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(1),VRP.Z_,
   int_to_l3d_real(0),int_to_l3d_real(0),int_to_l3d_real(0),
   int_to_l3d_real(1) );

  //- concatenate the inverse matrices: first undo the rotation, then undo the
  //- translation
  inverse_viewing_xform = inverse_TnVRP | inverse_R;
}

void l3d_moveable::transform(const l3d_matrix &m) {
  // transform orientation

  //    VRP.transformed = m | VRP.transformed;
  //    VUP_tip.transformed = m | VUP_tip.transformed;
  //    VFW_tip.transformed = m | VFW_tip.transformed;
  //    VRI_tip.transformed = m | VRI_tip.transformed;
  //    VUP = VUP_tip - VRP;
  //    VFW = VFW_tip - VRP;
  //    VRI = VRI_tip - VRP;
}

//  void l3d_moveable::reset(void) {
//    VRP.transformed = VRP.original;
//    VUP_tip.transformed = VUP_tip.original;
//    VFW_tip.transformed = VUP_tip.original;
//    VRI_tip.transformed = VRI_tip.original;
//    VUP = VUP_tip - VRP;
//    VFW = VFW_tip - VRP;
//    VRI = VRI_tip - VRP;

//  }
