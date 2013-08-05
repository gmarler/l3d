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

#ifndef __CAMCOL_H
#define __CAMCOL_H
#include "../tool_os/memman.h"

#include "camera.h"
#include "../dynamics/colsph.h"

class l3d_camera_collidable :
      virtual public l3d_camera,
      virtual public l3d_collidable_sphere
{
  public:
    //  int old_position_already_saved;
    // allows multiple calls to update (and thus mult. changes to position)
    // before collision detection; only the first, original position is saved
    // and compared with the new, possibly-multiply-updated new position.
    //^ FIXME: replace this with a reset() routine, since that's all it really
    //  is. also make the camera accessible like any other object.

    l3d_camera_collidable(void) :
    l3d_camera()
    { collision_sphere_radius = int_to_l3d_real(5);
      //     old_position_already_saved = 0;
    };
    virtual ~l3d_camera_collidable(void) {};
    virtual int update(void) {
      //     if(!old_position_already_saved) {
      //       old_position_already_saved = 1; // is reset by coll. det routine
      l3d_collidable::old_position = VRP;
      //     }
      int status=l3d_moveable::update();
      l3d_collidable::new_position = VRP;
      return status;
    }
};
#endif











