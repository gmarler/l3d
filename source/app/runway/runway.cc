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

#define MAXDOTS 500
#define NEARDOTS_INTERVAL 1
#define MEDDOTS_INTERVAL 2
#define FARDOTS_INTERVAL 4

//- STEP 1: CHOOSE THE FACTORIES

void choose_factories(void) {
  //sbegin choose_fac
  factory_manager_v_0_1.choose_factories();
  //send choose_fac
}

//- STEP 2: DECLARE A PIPELINE SUBCLASS

class my_pipeline : public l3d_pipeline {
  protected:
    l3d_rasterizer_2d_imp *ri;
    l3d_rasterizer_2d *r;

    unsigned long color;

    int neardots_x[MAXDOTS], neardots_y, neardot_x_spacing, neardots_ctr,
    meddots_x[MAXDOTS], meddots_y, meddot_x_spacing, meddots_ctr,
    fardots_x[MAXDOTS], fardots_y, fardot_x_spacing, fardots_ctr,
    num_neardots, num_meddots, num_fardots;


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
  s->sinfo->ext_to_native(0, 0, 0); //- allocate black background color
  color = s->sinfo->ext_to_native(255, 255, 255); // red, green, blue
  s->refresh_palette();

  int i,x;
  // Nearest row of runway lights
  neardots_y = 180;                 // nearest row: bottom of screen
  neardot_x_spacing=20;             // nearest row: wide spacing
  neardots_ctr = NEARDOTS_INTERVAL; // nearest row: fast movement
  // squeeze as many lights as possible into a row, with the given spacing
  for(i=0, x=0; x<320 && i < MAXDOTS; i++) {
    neardots_x[i] = x;
    x+=neardot_x_spacing;
  }
  num_neardots=i;

  /* Middle row of runway lights */
  meddots_y = 140;                  /* middle row: higher on screen */
  meddot_x_spacing=10;              /* middle row: medium spacing */
  meddots_ctr = MEDDOTS_INTERVAL;   /* middle row: medium speed */
  /* squeeze as many lights as possible into a row, with the given spacing */
  for(i=0, x=0; x<320 && i < MAXDOTS; i++) {
    meddots_x[i] = x;
    x+=meddot_x_spacing;
  }
  num_meddots=i;

  /* Farthest row of runway lights */
  fardots_y = 120;                  /* farthest row: highest up on screen */
  fardot_x_spacing=7;               /* farthest row: tightly spaced */
  fardots_ctr = FARDOTS_INTERVAL;   /* farthest row: slow movement */
  /* squeeze as many lights as possible into a row, with the given spacing */
  for(i=0, x=0; x<320 && i < MAXDOTS; i++) {
    fardots_x[i] = x;
    x+=fardot_x_spacing;
  }
  num_fardots=i;
}

my_pipeline::~my_pipeline(void) {
  delete s;
  delete ri;
  delete r;
}

void my_pipeline::key_event(int ch) {
  switch(ch) {
    case 'q': {
        exit(0);
      }
  }
}

void my_pipeline::update_event() {
  int i;

  /* move all dots */
  neardots_ctr--;
  if (neardots_ctr == 0) { /* neardots move once every NEARDOTS_INTERVAL */
    neardots_ctr = NEARDOTS_INTERVAL;
    for(i=0;i<num_neardots;i++) {
      neardots_x[i] --;
      if(neardots_x[i]<0) neardots_x[i] += 320;
    }
  }
  meddots_ctr--;
  if (meddots_ctr == 0) { /* meddots move once every MEDDOTS_INTERVAL */
    meddots_ctr = MEDDOTS_INTERVAL;
    for(i=0;i<num_meddots;i++) {
      meddots_x[i] --;
      if(meddots_x[i]<0) meddots_x[i] += 320;
    }
  }
  fardots_ctr--;
  if (fardots_ctr == 0) { /* fardots move once every FARDOTS_INTERVAL */
    fardots_ctr = FARDOTS_INTERVAL;
    for(i=0;i<num_fardots;i++) {
      fardots_x[i] --;
      if(fardots_x[i]<0) fardots_x[i] += 320;
    }
  }
}

void my_pipeline::draw_event(void) {
  int i;
  r->clear_buffer();
  /* plot all dots */

  for(i=0; i<num_neardots; i++) {
    r->draw_point(neardots_x[i],neardots_y,color);
  }
  for(i=0; i<num_meddots; i++) {
    r->draw_point(meddots_x[i],meddots_y,color);
  }
  for(i=0; i<num_fardots; i++) {
    r->draw_point(fardots_x[i],fardots_y,color);
  }

  s->blit_screen();
}

main() {
  choose_factories();

  l3d_dispatcher *d;
  my_pipeline *p;

  //- STEP 3: CREATE A DISPATCHER

  d = factory_manager_v_0_1.dispatcher_factory->create();

  //- STEP 4: CREATE A PIPELINE

  //- plug our custom behavior pipeline into the dispatcher
  p  = new my_pipeline();

  //- STEP 5: START DISPATCHER

  d->pipeline = p; //- polymorphic assignment
  d->event_source = p->s;
  d->start();

  // clean up
  delete d;
  delete p;
}
