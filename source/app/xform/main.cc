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

#include "../lib/geom/polygon/p_flat.h"
#include "../lib/tool_2d/screen.h"
#include "../lib/tool_os/dispatch.h"
#include "../lib/raster/rasteriz.h"
#include "../lib/tool_2d/si_idx.h"
#include "../lib/tool_2d/si_rgb.h"
#include "../lib/system/factorys.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

class my_pipeline : public l3d_pipeline {
  public:
    l3d_screen *screen;
    l3d_rasterizer_2d *rasterizer;
    l3d_rasterizer_2d_imp *ras_imp;

    l3d_two_part_list<l3d_coordinate> *vlist;

    int c;  // color

    my_pipeline(void);
    virtual ~my_pipeline(void);
    void key_event(int ch); // from dispatcher
    void update_event(void); // from dispatcher
    void draw_event(void);   // from dispatcher

    void init(void);
    virtual void pixel(char **p,int a,int b,int c, int d){};
};

my_pipeline::my_pipeline(void) {
  screen = factory_manager_v_0_1.screen_factory->create(640,480);
  ras_imp = factory_manager_v_0_1.
            ras_2d_imp_factory->create(640,480,screen->sinfo);
  rasterizer = new l3d_rasterizer_2d(ras_imp);

  vlist = new l3d_two_part_list<l3d_coordinate> ( 3 );

}

my_pipeline::~my_pipeline(void) {
  delete screen;
  delete ras_imp;
  delete rasterizer;
  delete vlist;
}

void my_pipeline::key_event(int c) {
  switch(c) {
    case 'q': exit(0);
  }
}

void my_pipeline::update_event() {
  int i;
  int x,y;

  for(i=0; i<7 ; i++) {

    //sbegin vtx_list
    int px=rand()%1200-640, py=rand()%800-400;

    (*vlist)[0].transformed.X_ = int_to_l3d_real(100+px);
    (*vlist)[0].transformed.Y_ = int_to_l3d_real(100+py);
    (*vlist)[1].transformed.X_ = int_to_l3d_real(300+px);
    (*vlist)[1].transformed.Y_ = int_to_l3d_real(150+py);
    (*vlist)[2].transformed.X_ = int_to_l3d_real(70+px);
    (*vlist)[2].transformed.Y_ = int_to_l3d_real(120+py);
    //send vtx_list

    //sbegin polygon
    l3d_polygon_2d_flatshaded p(3);

    p.vlist = &vlist;
    (*(p.ivertices))[p.ivertices->next_index()].ivertex = 0;
    (*(p.ivertices))[p.ivertices->next_index()].ivertex = 1;
    (*(p.ivertices))[p.ivertices->next_index()].ivertex = 2;
    //send polygon

    //sbegin random_color
    int r,g,b;

    unsigned long col;
    l3d_screen_info_indexed *si_idx;
    l3d_screen_info_rgb *si_rgb;
    if (si_idx = dynamic_cast<l3d_screen_info_indexed *>( screen->sinfo )) {
      col = rand() % si_idx->get_palette_size();
    } else if (si_rgb = dynamic_cast<l3d_screen_info_rgb *>(screen->sinfo)) {

      r = rand() % ((si_rgb->red_mask) >> (si_rgb->red_shift));
      g = rand() % ((si_rgb->green_mask) >> (si_rgb->green_shift));
      b = rand() % ((si_rgb->blue_mask) >> (si_rgb->blue_shift));

      col =  r<<si_rgb->red_shift |
             g<<si_rgb->green_shift |
             b<<si_rgb->blue_shift;
    }
    p.final_color = col;
    //send random_color


    // reset the polygon to have no references to clip-vertices
    //sbegin do_new_clip
    vlist->num_varying_items = 0;// reset the SHARED list to have no definitions of clip-vertices
    p.init_clip_ivertices();

    if (p.clip_to_polygon_2d(screen->view_win)) {
      rasterizer->draw_polygon_flatshaded(&p);
    }
    //send do_new_clip

    //sbegin line
    x = rand() % 540;
    y = rand() % 380;
    rasterizer->draw_line(0,0,x,y,col);
    //send line

  }
}

void my_pipeline::draw_event(void) {
  screen->blit_screen();
}

main() {
  l3d_dispatcher *d;
  my_pipeline *p;

  factory_manager_v_0_1.choose_factories();
  d = factory_manager_v_0_1.dispatcher_factory->create();

  // plug our custom behavior pipeline into the dispatcher
  p  = new my_pipeline();
  d->pipeline = p;// polymorphic assignment
  d->event_source = p->screen;

  // start dispatcher with the pipeline's screen
  d->start();

  // clean up
  delete p;
  delete d;
}



























