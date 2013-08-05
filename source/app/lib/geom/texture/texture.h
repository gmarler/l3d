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

#ifndef __TEXTURE_H
#define __TEXTURE_H
#include "../../tool_os/memman.h"

#include <stdlib.h>
#include <stdio.h>
#include "../vertex/coord.h"

class l3d_texture_data {
  public:
    int id;
    l3d_texture_data(void) { id=0; data = NULL; }
    virtual ~l3d_texture_data(void) { if(data) delete [] data; }
    int width, height; // must be powers of two!
    unsigned char *data;

};

class l3d_texture_space {
  public:
    l3d_coordinate O, U, V;
};

class l3d_texture : public l3d_texture_space
{
  public:
    bool owns_tex_data;
    l3d_texture_data *tex_data;
    l3d_texture(void) { owns_tex_data = false; }
    ~l3d_texture(void) { if (owns_tex_data) delete tex_data; }
};

class l3d_texture_computer {
    // this class computes texture coordinates based on a variety of
    // input parameters. the idea is that a class needing these services
    // inherits/instantiates this class (inheritance is probably easier)

  private:
    l3d_matrix _world_to_tex_matrix;
  public:
    //    static const l3d_real EPSILON_VECTOR = float_to_l3d_real(0.0001);

    l3d_point O; l3d_vector U, V;
    l3d_real *fovx,*fovy; // externally controlled horiz and vert fov terms,
    // thus pointers instead of variables (if they change outside, we
    // want our computations also to change without needing to recopy
    // the values)
    int *screen_xsize, *screen_ysize;

    l3d_real u, v;

    const l3d_matrix& world_to_tex_matrix(const l3d_texture_space &tex_space) {
      l3d_vector
      U_vector = tex_space.U.original - tex_space.O.original,
                 V_vector = tex_space.V.original - tex_space.O.original,
                            U_cross_V_vector = cross(U_vector, V_vector);

      l3d_vector w = U_cross_V_vector;
      l3d_vector v_x_u = cross(V_vector,U_vector),
                         u_x_w = cross(U_vector, w),
                                 w_x_v = cross(w, V_vector),
                                         o_vec(tex_space.O.original.X_,
                                               tex_space.O.original.Y_,
                                               tex_space.O.original.Z_,
                                               int_to_l3d_real(0));

      _world_to_tex_matrix.set(
        w_x_v.X_, w_x_v.Y_, w_x_v.Z_, dot(o_vec,w_x_v)*int_to_l3d_real(-1),
        u_x_w.X_, u_x_w.Y_, u_x_w.Z_, dot(o_vec,u_x_w)*int_to_l3d_real(-1),
        v_x_u.X_, v_x_u.Y_, v_x_u.Z_, dot(o_vec,v_x_u)*int_to_l3d_real(-1),
        int_to_l3d_real(0),
        int_to_l3d_real(0),
        int_to_l3d_real(0),
        dot(U_vector,w_x_v));


      return _world_to_tex_matrix;
    }

};

#endif




