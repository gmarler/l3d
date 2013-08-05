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

#include "ch2_factorymanager.h"

// references to concrete factories

#include "ch2_appfactory_x.h"
#include "ch2_appfactory_xevent.h"
#include "ch2_appfactory_ximage.h"

ch2_appfactory *ch2_factorymanager::appfactory = 0;

void ch2_factorymanager::choose_factories(void) {
  int i;

  printf("Which Appfactory should create your application?\n");
  printf("1. Appfactory creating the simple X app\n");
  printf("2. Appfactory creating the simple X app with event display\n");
  printf("3. Appfactory creating the random dots app\n");

  scanf("%d", &i);

  switch(i) {
    case 1: {
        appfactory = new ch2_appfactory_x;
      };
      break;

    case 2: {
        appfactory = new ch2_appfactory_xevent;
      };
      break;

    case 3: {
        appfactory = new ch2_appfactory_ximage;
      };
      break;
  }
}
