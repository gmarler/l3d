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

#include "pi_wor.h"

#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include "../tool_os/memman.h"

void l3d_pipeline_world::key_event(int c) {
  switch(c) {
    case 'q': done = 1;

      //- default keys for world->camera movement -- full 360 degree movement
    case 'j': if(world->camera->rot_VUP_velocity>int_to_l3d_real(-95)) world->camera->rot_VUP_velocity -= int_to_l3d_real(5); break;
    case 'l': if(world->camera->rot_VUP_velocity<int_to_l3d_real( 95)) world->camera->rot_VUP_velocity += int_to_l3d_real(5); break;
    case 'J': if(world->camera->rot_VFW_velocity<int_to_l3d_real( 95)) world->camera->rot_VFW_velocity += int_to_l3d_real(5); break;
    case 'L': if(world->camera->rot_VFW_velocity>int_to_l3d_real(-95)) world->camera->rot_VFW_velocity -= int_to_l3d_real(5); break;
    case 'I': if(world->camera->rot_VRI_velocity<int_to_l3d_real( 95)) world->camera->rot_VRI_velocity += int_to_l3d_real(5); break;
    case 'K': if(world->camera->rot_VRI_velocity>int_to_l3d_real(-95)) world->camera->rot_VRI_velocity -= int_to_l3d_real(5); break;
    case 'k': if(world->camera->VFW_velocity    >int_to_l3d_real(-90)) world->camera->VFW_velocity -= world->camera->VFW_thrust; break;
    case 'i': if(world->camera->VFW_velocity    <int_to_l3d_real( 90)) world->camera->VFW_velocity += world->camera->VFW_thrust; break;
    case 's': if(world->camera->VRI_velocity    >int_to_l3d_real(-90)) world->camera->VRI_velocity -= world->camera->VRI_thrust; break;
    case 'f': if(world->camera->VRI_velocity    <int_to_l3d_real( 90)) world->camera->VRI_velocity += world->camera->VRI_thrust; break;
    case 'd': if(world->camera->VUP_velocity    >int_to_l3d_real(-90)) world->camera->VUP_velocity -= world->camera->VUP_thrust; break;
    case 'e': if(world->camera->VUP_velocity    <int_to_l3d_real( 90)) world->camera->VUP_velocity += world->camera->VUP_thrust; break;

      //- field-of-view modification
    case 'X': world->camera->fovx = world->camera->fovx + float_to_l3d_real(0.1); break;
    case 'x': world->camera->fovx = world->camera->fovx - float_to_l3d_real(0.1); break;
    case 'Y': world->camera->fovy = world->camera->fovy + float_to_l3d_real(0.1); break;
    case 'y': world->camera->fovy = world->camera->fovy - float_to_l3d_real(0.1); break;

      //- speed
  case 'v': case 'V':
      world->camera->VUP_thrust += int_to_l3d_real(1.0);
      if(world->camera->VUP_thrust  > 3.0) {
        world->camera->VUP_thrust  -= int_to_l3d_real(3.0);
      }
      world->camera->VFW_thrust = world->camera->VUP_thrust;
      world->camera->VRI_thrust = world->camera->VUP_thrust;
      break;

      //- display

  case 'n': case 'N':
      world->should_display_status = 1-world->should_display_status; //- toggle
      break;

  }

}

void l3d_pipeline_world::update_event() {
  world->update_all();
}

void l3d_pipeline_world::draw_event(void) {
  static int frame=0;
  static long last_sec, last_usec;
  long sec, usec;
  static float fps;
  static char str[80];

  struct timeval tv;
  struct timezone tz;
  tz.tz_minuteswest = -60;

  world->rasterizer->clear_buffer();
  world->draw_all();

  // FPS calculation
  frame++;
  if((frame & 0x3F) == 0) {
    gettimeofday(&tv,&tz);
    fps = 64 * 1.0 / ( (tv.tv_sec + tv.tv_usec/1000000.0) - (last_sec + last_usec/1000000.0) );
    last_sec = tv.tv_sec;
    last_usec = tv.tv_usec;

  }

  if(world->should_display_status) {
    sprintf(str,"FPS: %f", fps);
    world->rasterizer->draw_text(0, 0, str);
  }

  // blit
  world->screen->blit_screen();

}

