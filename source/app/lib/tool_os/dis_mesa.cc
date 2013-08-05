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

#include "dis_mesa.h"
#include "../tool_os/memman.h"
#include <stdlib.h>

l3d_dispatcher *l3d_dispatcher_mesa::last_dispatcher;

l3d_dispatcher_mesa::l3d_dispatcher_mesa(void) {
  l3d_dispatcher_mesa::last_dispatcher = this;
}

void l3d_dispatcher_mesa::glut_display_func(void) {
  l3d_dispatcher_mesa::last_dispatcher->pipeline->draw_event();
  glutPostRedisplay();
}

void l3d_dispatcher_mesa::glut_idle_func(void) {
  l3d_dispatcher_mesa::last_dispatcher->pipeline->update_event();
}

void l3d_dispatcher_mesa::glut_keyboard_func(unsigned char key, int x, int y) {
  l3d_dispatcher_mesa::last_dispatcher->pipeline->key_event(key);

  //- unfortunately GLUT doesnt provide a clean way of exiting, so
  //- we have to jump out of the program here if the pipeline says
  //- it is done
  if(l3d_dispatcher_mesa::last_dispatcher->pipeline->done) exit(0);
}

void l3d_dispatcher_mesa::start(void) {
  glutDisplayFunc(l3d_dispatcher_mesa::glut_display_func);
  glutIdleFunc(l3d_dispatcher_mesa::glut_idle_func);
  glutKeyboardFunc(l3d_dispatcher_mesa::glut_keyboard_func);
  glutMainLoop();
}
