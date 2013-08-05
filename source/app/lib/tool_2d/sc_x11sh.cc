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

#include "sc_x11sh.h"
#include "../tool_os/memman.h"

l3d_screen_x11_shm::~l3d_screen_x11_shm(void) {
  XShmDetach(dpy, &shminfo);
  XDestroyImage(ximg);
  shmdt(shminfo.shmaddr);
}

XImage *l3d_screen_x11_shm::create_ximage(void) {
  XImage *ximg;

  ximg = XShmCreateImage(dpy, vis, depth,
                         ZPixmap,   // format
                         0,  // pixels to ignore at beg. of scanline
                         &shminfo,
                         xsize, ysize);

  shminfo.shmid = shmget(IPC_PRIVATE, ximg->bytes_per_line*ximg->height,
                         IPC_CREAT | 0777 );
  if(shminfo.shmid < 0) {
    printf("couldn't allocate shared memory\n");
    XDestroyImage(ximg);
    ximg = NULL;
  } else {

    // attach, and check for errrors
    shminfo.shmaddr = ximg->data = (char *)shmat(shminfo.shmid, 0, 0);
    if(shminfo.shmaddr == (char *) -1) {
      printf("couldn't allocate shared memory\n");
      XDestroyImage(ximg);
      ximg = NULL;
    } else {
      // set as read/write, and attach to the display
      shminfo.readOnly = False;
      XShmAttach(dpy, &shminfo);
    }
  }

  return ximg;
}

void l3d_screen_x11_shm::blit_screen(void) {
  XShmPutImage(dpy, w, gc, ximg, 0, 0, 0, 0, xsize, ysize,
               False);
}

void l3d_screen_x11_shm::create_buffer(void) {
  buffer = (unsigned char *)(ximg->data);
  sinfo->p_screenbuf = buffer;
}
