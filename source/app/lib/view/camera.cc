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
#include "camera.h"
#include "../tool_os/memman.h"

l3d_camera::l3d_camera(void) : l3d_moveable()
{
//    /* Near Z clipping plane, against which polygons will be analytically
//     * clipped */
  near_z = float_to_l3d_real(1.7);

  //  /* Far Z clipping plane, beyond which polygons will not be displayed */
  far_z = int_to_l3d_real(500);

  //  /* horizontal and vertical field of view parameters */
  fovx = float_to_l3d_real(0.866);  // 0.5 * cot(0.5*60 deg)
  fovy = l3d_mulrr(fovx, float_to_l3d_real(1.333)); //- 1.3=4/3=width/height
}
