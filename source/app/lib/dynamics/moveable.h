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

#ifndef __MOVEABLE_H
#define __MOVEABLE_H
#include "../tool_os/memman.h"

#include "../system/sys_dep.h"
#include "../math/matrix.h"
#include "../geom/point/point.h"

class l3d_moveable {
  public:
    // constructor
    l3d_moveable(void);

    //    /* orientation vectors (heavily influenced by camera model)
    //     *    VRP: View Reference Point. Location of the camera.
    //     *    VFW: View ForWard vector. Normal vector to the view plane; points
    //     *         FROM the camera TO the direction being looked in. We always
    //     *         keep the VFW normalized (magnitude=1).
    //     *    VUP: View UP vector. The projection of the VUP on the View Plane
    //     *         is the direction "up". We always keep the VUP normalized
    //     *         (magnitude=1).
    //     *    VRI: View Right Vector. Points in the "right-hand" direction for
    //     *         the camera. May be computed as VUP x VFW (in LHS 3D coordinates).
    //     *         We always keep the VRI normalized (magnitude=1).
    //     */
    l3d_point VRP, VFW_tip, VUP_tip, VRI_tip;
    l3d_vector VFW, VUP, VRI;

    //     /* matrix which converts coordinates into "standard eye coordinates" --
    //        eye at origin, looking along z-axis, perfectly right-side up */
    l3d_matrix viewing_xform;
    l3d_matrix inverse_viewing_xform;

    //     /* function which computes the values for the "viewing_xform" matrix */
    void calculate_viewing_xform(void);

    //     /* movement */
    virtual int update(void);       //- update position of camera
    l3d_real
    rot_VUP_velocity, // rotational velocity around VUP axis
    rot_VRI_velocity, // rotational velocity around VRI axis
    rot_VFW_velocity, // rotational velocity around VFW axis
    VUP_velocity,     // translational velocity along +ve VUP axis
    VRI_velocity,     // translational velocity along +ve VRI axis
    VFW_velocity,     // translational velocity along +ve VFW axis
    VUP_thrust,
    VRI_thrust,
    VFW_thrust;
    void rotateVFW(void);   // rot VUP,VRI around VFW by amt rot_VFW_velocity
    void rotateVUP(void);  // rot VFW,VRI around VUP by amt rot_VUP_velocity
    void rotateVRI(void);   // rot VFW,VUP around VRI by amt rot_VRI_velocity
    void translateVFW(void);// translate VRP along VFW by amt VFW_velocity
    void translateVUP(void);// translate VRP along VUP by amt VUP_velocity
    void translateVRI(void);// translate VRP along VRI by amt VRI_velocity

    //  virtual void reset(void);
    virtual void transform(const l3d_matrix &m);
};
#endif

