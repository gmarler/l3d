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

#include <stdlib.h>
#include <stdio.h>

#include "../lib/tool_2d/screen.h"
#include "../lib/tool_os/dispatch.h"
#include "../lib/raster/rasteriz.h"
#include "../lib/tool_2d/scrinfo.h"
#include "../lib/system/factorys.h"

//------------------------------------------------------------------------
//-
//- STEP 1: CHOOSE THE FACTORIES
//-
//------------------------------------------------------------------------

void choose_factories(void) {
  //sbegin choose_fac
  factory_manager_v_0_1.choose_factories();
  //send choose_fac
}

//------------------------------------------------------------------------
//-
//- STEP 2: DECLARE A PIPELINE SUBCLASS
//-
//------------------------------------------------------------------------

class my_pipeline : public l3d_pipeline {
  protected:
    l3d_rasterizer_2d_imp *ri;
    l3d_rasterizer_2d *r;

    int x, y, dx, dy;
    unsigned long color;

  public:
    l3d_screen *s;
    my_pipeline(void);
    virtual ~my_pipeline(void);

    //sbegin override
    void key_event(int ch);  //- from dispatcher
    void update_event(void); //- from dispatcher
    void draw_event(void);   //- from dispatcher
    //send override
};

my_pipeline::my_pipeline(void) {
  s = factory_manager_v_0_1.screen_factory->create(320,200);
  ri = factory_manager_v_0_1.ras_2d_imp_factory->create(320,200,s->sinfo);
  r = new l3d_rasterizer_2d(ri);

  s->sinfo->ext_max_red =
    s->sinfo->ext_max_green =
      s->sinfo->ext_max_blue =  255;

  s->sinfo->ext_to_native(0, 0, 0); //- allocate background color
  color = s->sinfo->ext_to_native(0, 255, 128); // red, green, blue
  s->refresh_palette();

  x = y = dx = dy = 0;
}

my_pipeline::~my_pipeline(void) {
  delete s;
  delete ri;
  delete r;
}

void my_pipeline::key_event(int ch) {
  switch(ch) {
    case 'h': dx=-1; dy=0; break;
    case 'l': dx=1; dy=0; break;
    case 'j': dx=0; dy=1; break;
    case 'k': dx=0; dy=-1; break;
    case ' ': dx=0;dy=0; break;
    case 'q': {
        exit(0);
      }
  }
}

void my_pipeline::update_event() {
  x += dx;
  y += dy;

  if(x < 0) x = 0;
  if(x > s->xsize-1) x = s->xsize-1;
  if(y < 0) y = 0;
  if(y > s->ysize-1) y = s->ysize-1;
}

void my_pipeline::draw_event(void) {
  r->draw_point(x,y, color);
  s->blit_screen();
}

main() {
  choose_factories();

  l3d_dispatcher *d;
  my_pipeline *p;

  //------------------------------------------------------------------------
  //-
  //- STEP 3: CREATE A DISPATCHER
  //-
  //------------------------------------------------------------------------

  d = factory_manager_v_0_1.dispatcher_factory->create();

  //------------------------------------------------------------------------
  //-
  //- STEP 4: CREATE A PIPELINE
  //-
  //------------------------------------------------------------------------

  //- plug our custom behavior pipeline into the dispatcher
  p  = new my_pipeline();

  //------------------------------------------------------------------------
  //-
  //- STEP 5: START DISPATCHER
  //-
  //------------------------------------------------------------------------

  d->pipeline = p; //- polymorphic assignment
  d->event_source = p->s;
  d->start();

  // clean up
  delete d;
  delete p;
}
