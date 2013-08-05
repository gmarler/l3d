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

#ifndef __NL_BLENDER_CONTROLLER_H
#define __NL_BLENDER_CONTROLLER_H

#include <vector>

class blender_config;

class blender_controller {
  public:
    blender_config *config;

    virtual void find_blender_window(void) = 0;
    virtual void press_key(int code) = 0;
    virtual void release_key(int code) = 0;
    virtual void read_properties(void) = 0;
    virtual void determine_selected_face_nums(void) = 0;
    int current_id;
    char current_prop_filename[4096];
    vector<int> selected_faces;
    vector<string> property_lines;
};

#endif
