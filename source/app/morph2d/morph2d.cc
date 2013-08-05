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
#include "geom/vertex/verint.h"

#include "../lib/tool_2d/screen.h"
#include "../lib/tool_os/dispatch.h"
#include "../lib/raster/rasteriz.h"
#include "../lib/tool_2d/scrinfo.h"
#include "../lib/system/factorys.h"

class my_pipeline : public l3d_pipeline {
  protected:
    static const int num_frames = 4;
    l3d_rasterizer_2d_imp *ri;
    l3d_rasterizer_2d *r;
    l3d_vertex_interpolator interp;
    l3d_two_part_list<l3d_coordinate> *vlist, *vlist_frames[num_frames];
    l3d_polygon_2d_flatshaded *facet;

    int frame_no;
    bool currently_interpolating;

  public:
    l3d_screen *s;
    my_pipeline(void);
    virtual ~my_pipeline(void);
    void update_event(void);
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
  s->sinfo->ext_to_native(0,0,0); //- allocate black background color
  unsigned long color = s->sinfo->ext_to_native(0, 128, 255);
  s->refresh_palette();

  s->open_screen();

  vlist_frames[0] = new l3d_two_part_list<l3d_coordinate> ( 4 );
  (*vlist_frames[0])[0].original.X_ = float_to_l3d_real(100.0);
  (*vlist_frames[0])[0].original.Y_ = float_to_l3d_real(100.0);

  (*vlist_frames[0])[1].original.X_ = float_to_l3d_real(200.0);
  (*vlist_frames[0])[1].original.Y_ = float_to_l3d_real(150.0);

  (*vlist_frames[0])[2].original.X_ = float_to_l3d_real(150.0);
  (*vlist_frames[0])[2].original.Y_ = float_to_l3d_real(200.0);

  (*vlist_frames[0])[3].original.X_ = float_to_l3d_real( 50.0);
  (*vlist_frames[0])[3].original.Y_ = float_to_l3d_real(120.0);

  vlist_frames[1] = new l3d_two_part_list<l3d_coordinate> ( 4 );
  (*vlist_frames[1])[0].original.X_ = float_to_l3d_real(100.0);
  (*vlist_frames[1])[0].original.Y_ = float_to_l3d_real(100.0);

  (*vlist_frames[1])[1].original.X_ = float_to_l3d_real(200.0);
  (*vlist_frames[1])[1].original.Y_ = float_to_l3d_real(100.0);

  (*vlist_frames[1])[2].original.X_ = float_to_l3d_real(150.0);
  (*vlist_frames[1])[2].original.Y_ = float_to_l3d_real(200.0);

  (*vlist_frames[1])[3].original.X_ = float_to_l3d_real(140.0);
  (*vlist_frames[1])[3].original.Y_ = float_to_l3d_real(200.0);

  vlist_frames[2] = new l3d_two_part_list<l3d_coordinate> ( 4 );
  (*vlist_frames[2])[0].original.X_ = float_to_l3d_real(-100.0);
  (*vlist_frames[2])[0].original.Y_ = float_to_l3d_real(100.0);

  (*vlist_frames[2])[1].original.X_ = float_to_l3d_real(200.0);
  (*vlist_frames[2])[1].original.Y_ = float_to_l3d_real(100.0);

  (*vlist_frames[2])[2].original.X_ = float_to_l3d_real(950.0);
  (*vlist_frames[2])[2].original.Y_ = float_to_l3d_real(200.0);

  (*vlist_frames[2])[3].original.X_ = float_to_l3d_real(940.0);
  (*vlist_frames[2])[3].original.Y_ = float_to_l3d_real(200.0);

  vlist_frames[3] = new l3d_two_part_list<l3d_coordinate> ( 4 );
  (*vlist_frames[3])[0].original.X_ = float_to_l3d_real(100.0);
  (*vlist_frames[3])[0].original.Y_ = float_to_l3d_real(150.0);

  (*vlist_frames[3])[1].original.X_ = float_to_l3d_real(100.0);
  (*vlist_frames[3])[1].original.Y_ = float_to_l3d_real(100.0);

  (*vlist_frames[3])[2].original.X_ = float_to_l3d_real(250.0);
  (*vlist_frames[3])[2].original.Y_ = float_to_l3d_real(200.0);

  (*vlist_frames[3])[3].original.X_ = float_to_l3d_real(100.0);
  (*vlist_frames[3])[3].original.Y_ = float_to_l3d_real(200.0);

  frame_no = 0;
  currently_interpolating = false;
  vlist = vlist_frames[frame_no];

  // define polygon in relation to vertex list

  facet = new l3d_polygon_2d_flatshaded ( 4 );
  facet->vlist = & vlist;
  facet->final_color = color;
  (*(facet->ivertices))[facet->ivertices->next_index()].ivertex = 0;
  (*(facet->ivertices))[facet->ivertices->next_index()].ivertex = 1;
  (*(facet->ivertices))[facet->ivertices->next_index()].ivertex = 2;
  (*(facet->ivertices))[facet->ivertices->next_index()].ivertex = 3;

}

my_pipeline::~my_pipeline(void) {
  delete vlist_frames[0];
  delete vlist_frames[1];
  delete vlist_frames[2];
  delete vlist_frames[3];
  delete s;
  delete ri;
  delete r;
  delete facet;
}

//sbegin update_event
void my_pipeline::update_event(void) {
  if(currently_interpolating) {
    vlist = interp.list;
    if(! interp.step()) {
      currently_interpolating = false;
    }
  } else {
    frame_no++;
    if(frame_no >= num_frames) { frame_no = 0; }
    int next_frame = frame_no + 1;
    if(next_frame >= num_frames) { next_frame = 0; }

    vlist = vlist_frames[frame_no];
    interp.start( *vlist_frames[frame_no], *vlist_frames[next_frame],
                  50, 2);

    currently_interpolating = true;
  }
}
//send update_event

//sbegin draw_event
void my_pipeline::draw_event(void) {
  vlist->num_varying_items = 0; // free all clipped vertices
  for(int i=0; i<vlist->num_fixed_items; i++) { (*vlist)[i].reset(); }
  facet->init_clip_ivertices();  // performs a null-clip operation, since only
  // clipped polygons are drawn

  facet->clip_to_polygon_2d(s->view_win);
  r->clear_buffer();
  r->draw_polygon_flatshaded(facet);
  s->blit_screen();
}
//send draw_event

void my_pipeline::key_event(int ch) {
  switch(ch) {
    case 'q': done = 1;
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
