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

#include "pl_linux.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../../tool_os/memman.h"

l3d_plugin_loader *l3d_plugin_loader_linux_factory::create(void) {
  return new l3d_plugin_loader_linux;
}

l3d_plugin_loader_linux::l3d_plugin_loader_linux(void) {
  filename[0] = '\0';
  handle = 0;
}

void l3d_plugin_loader_linux::load(const char *filename) {
  assert(strlen(filename)<sizeof(this->filename));
  strncpy(this->filename, filename, sizeof(this->filename));
  handle = dlopen(filename, RTLD_LAZY);
  if(!handle) {
    fputs(dlerror(),stderr);
  }
}

void *l3d_plugin_loader_linux::find_symbol(char *symbol) {
  assert(handle);
  return dlsym(handle, symbol);
}

void l3d_plugin_loader_linux::close(void) {

  if(handle) {
    //        printf("closing pl loader, fname %s\n", filename);
    dlclose(handle);
  }
  //      else {
  //          printf("not closing pl loader, fname %s\n", filename);
  //      }
}

l3d_plugin_loader * l3d_plugin_loader_linux::clone(void) {
  //  printf("cloning, cur fname %s\n", filename);

  l3d_plugin_loader_linux *loader;
  loader = new l3d_plugin_loader_linux;
  strncpy(loader->filename, filename, sizeof(loader->filename));
  loader->load(loader->filename);
  //  printf("cloned fname %s to %s\n", filename, loader->filename);
  return loader;
}


