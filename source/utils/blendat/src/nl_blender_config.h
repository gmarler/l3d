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

#ifndef _BLENDER_CONFIG_H
#define _BLENDER_CONFIG_H

#include <string>

class blender_config {
  public:
    string blender_path;
    string blender_work_filename;
    string db_path;
    string oid_filename;

    // static const unsigned long BLENDER_EDITOR_MAGIC_COLOR = 0xdeadbeef;
    // static const char BLENDER_EDITOR_MAGIC_COLOR_STR[] = "0xdeadbeef";
    static float BLENDER_MAGIC_X_SIZE;
    static char ATTRIB_FNAME[];
};

#endif
