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

#include "../../../dynamics/plugins/plugenv.h"
#include "../../../dynamics/colsph.h"

class l3d_texture_data;

typedef enum _movement_states
{NONE,AROUND_VFW,AROUND_VRI,AROUND_VUP,ALONG_VFW,ALONG_VRI,ALONG_VUP}
movement_states;

static const movement_states states_array[]=
  { NONE,AROUND_VFW,AROUND_VRI,AROUND_VUP,
    ALONG_VFW,ALONG_VRI,ALONG_VUP};

class l3d_plugin_videoscape_mesh_seeker
{
  public:
    l3d_plugin_environment *plugin_env;
    l3d_texture_data *tex_data;

    l3d_plugin_videoscape_mesh_seeker(l3d_plugin_environment *env);
    virtual ~l3d_plugin_videoscape_mesh_seeker(void);
    char mesh_fname[1024], texcoord_fname[1024], texture_fname[1024];

    int tick_count, max_ticks;
    movement_states movement_state;
    int turning_velocity;
    l3d_real moving_velocity;
};
