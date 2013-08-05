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

#include <stdio.h>
#include <stdlib.h>

#include "tool_2d/screen.h"
#include "geom/polygon/polygon.h"
#include "raster/rasteriz.h"
#include "datastr/list.h"

#include "../lib/tool_2d/screen.h"
#include "../lib/tool_os/dispatch.h"
#include "../lib/raster/rasteriz.h"
#include "../lib/tool_2d/scrinfo.h"
#include "../lib/system/factorys.h"

class my_pipeline : public l3d_pipeline {
  protected:
    l3d_rasterizer_2d_imp *ri;
    l3d_rasterizer_2d *r;
    l3d_two_part_list<l3d_coordinate> *vlist;
    l3d_polygon_2d_flatshaded *facet;

  public:
    l3d_screen *s;
    my_pipeline(void);
    virtual ~my_pipeline(void);
    void key_event(int ch);
    void draw_event(void);
};

my_pipeline::my_pipeline(void) {
  s = factory_manager_v_0_1.screen_factory->create(320,200);
  ri = factory_manager_v_0_1.ras_2d_imp_factory->create(320,200,s->sinfo);
  r = new l3d_rasterizer_2d(ri);

  s->sinfo->ext_max_red = 255;
  s->sinfo->ext_max_green = 255;
  s->sinfo->ext_max_blue = 255;
  s->sinfo->ext_to_native(0,0,0); //- allocate black bg color
  unsigned long color = s->sinfo->ext_to_native(0, 128, 255);
  s->refresh_palette();

  s->open_screen();

  //sbegin vtx_list
  vlist = new l3d_two_part_list<l3d_coordinate> ( 4 );
  (*vlist)[0].original.X_ = float_to_l3d_real(100.0);
  (*vlist)[0].original.Y_ = float_to_l3d_real(100.0);

  (*vlist)[1].original.X_ = float_to_l3d_real(200.0);
  (*vlist)[1].original.Y_ = float_to_l3d_real(150.0);

  (*vlist)[2].original.X_ = float_to_l3d_real(150.0);
  (*vlist)[2].original.Y_ = float_to_l3d_real(200.0);

  (*vlist)[3].original.X_ = float_to_l3d_real( 50.0);
  (*vlist)[3].original.Y_ = float_to_l3d_real(120.0);
  //send vtx_list

  //- define polygon in relation to vertex list
  //sbegin define_poly
  facet = new l3d_polygon_2d_flatshaded ( 4 );
  facet->vlist = & vlist;
  facet->final_color = color;
  (*(facet->ivertices))[facet->ivertices->next_index()].ivertex = 0;
  (*(facet->ivertices))[facet->ivertices->next_index()].ivertex = 1;
  (*(facet->ivertices))[facet->ivertices->next_index()].ivertex = 2;
  (*(facet->ivertices))[facet->ivertices->next_index()].ivertex = 3;
  //send define_poly
}

my_pipeline::~my_pipeline(void) {
  delete vlist;
  delete s;
  delete ri;
  delete r;
  delete facet;
}

//sbegin draw_event
void my_pipeline::draw_event(void) {
  vlist->num_varying_items = 0; // free all clipped vertices
  facet->init_clip_ivertices();  // performs a null-clip operation, since only
  // clipped polygons are drawn
  for(int i=0; i<vlist->num_fixed_items; i++) { (*vlist)[i].reset(); }

  r->clear_buffer();
  r->draw_polygon_flatshaded(facet);
  s->blit_screen();
}
//send draw_event

void my_pipeline::key_event(int ch) {
  switch(ch) {
    case 'q': exit(0);
  }
}

main() {
  factory_manager_v_0_1.choose_factories();

  l3d_dispatcher *d;
  my_pipeline *p;

  d = factory_manager_v_0_1.dispatcher_factory->create();

  // plug our custom behavior pipeline into the dispatcher
  p  = new my_pipeline();
  d->pipeline = p;// polymorphic assignment
  d->event_source = p->s;

  // start dispatcher with the pipeline's screen
  d->start();

  // clean up
  delete d;
  delete p;

}
