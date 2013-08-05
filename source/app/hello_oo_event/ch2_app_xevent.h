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

#ifndef __CH2_APP_XEVENT_H
#define __CH2_APP_XEVENT_H

#include "ch2_app_x.h"

class ch2_app_xevent : public ch2_app_x {
  protected:
    char *screen;
    XImage *ximg;
    int depth, bytespp, scanline_pad;

    const char *event_name(int type) const;
    void create_window(void); // virtual
    void ask_to_be_notified_of_interesting_events(void); // virtual
    void event_loop(void); // virtual

  public:
    virtual ~ch2_app_xevent(void);

};

#endif
