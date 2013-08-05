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

#include "../lib/geom/point/point.h"
#include "../lib/math/vector.h"
#include "../lib/math/matrix.h"
#include "../lib/view/camera.h"

#define MAXSPIKES 150
#define PI 3.14159265

unsigned long red_color;
unsigned long green_color;
unsigned long blue_color;

//- STEP 1: CHOOSE THE FACTORIES

void choose_factories(void) {
  //sbegin choose_fac
  factory_manager_v_0_1.choose_factories();
  //send choose_fac
}

//- STEP 2: DECLARE A PIPELINE SUBCLASS

l3d_real fovx, fovy;
int xsize, ysize;

class spike {
  public:

    l3d_two_part_list<l3d_coordinate> *vlist;
    l3d_polygon_2d_flatshaded *faces[3];
    l3d_vector displacement;

    l3d_vector velocity;

    spike(void) {
      //- declare common shared vertex list for all polygons
      vlist = new l3d_two_part_list<l3d_coordinate> (4);
      int i=0;
      (*vlist)[i++].original.set(int_to_l3d_real(0),
                                 int_to_l3d_real(0),
                                 int_to_l3d_real(100),
                                 int_to_l3d_real(1));
      (*vlist)[i++].original.set(int_to_l3d_real(10),
                                 int_to_l3d_real(10),
                                 int_to_l3d_real(100),
                                 int_to_l3d_real(1));
      (*vlist)[i++].original.set(int_to_l3d_real(10),
                                 int_to_l3d_real(0),
                                 int_to_l3d_real(100),
                                 int_to_l3d_real(1));
      (*vlist)[i++].original.set(int_to_l3d_real(0),
                                 int_to_l3d_real(0),
                                 int_to_l3d_real(0),
                                 int_to_l3d_real(1));

      //- define polygons in terms of indices into shared vertex list
      faces[0] = new l3d_polygon_2d_flatshaded(3);
      faces[0]->vlist = & vlist; //- shared vertex list
      (*(faces[0]->ivertices))[faces[0]->ivertices->next_index()].ivertex = 3;
      (*(faces[0]->ivertices))[faces[0]->ivertices->next_index()].ivertex = 0;
      (*(faces[0]->ivertices))[faces[0]->ivertices->next_index()].ivertex = 1;

      faces[1] = new l3d_polygon_2d_flatshaded(3);
      faces[1]->vlist = & vlist; //- shared vertex list
      (*(faces[1]->ivertices))[faces[1]->ivertices->next_index()].ivertex = 3;
      (*(faces[1]->ivertices))[faces[1]->ivertices->next_index()].ivertex = 1;
      (*(faces[1]->ivertices))[faces[1]->ivertices->next_index()].ivertex = 2;

      faces[2] = new l3d_polygon_2d_flatshaded(3);
      faces[2]->vlist = & vlist; //- shared vertex list
      (*(faces[2]->ivertices))[faces[2]->ivertices->next_index()].ivertex = 3;
      (*(faces[2]->ivertices))[faces[2]->ivertices->next_index()].ivertex = 2;
      (*(faces[2]->ivertices))[faces[2]->ivertices->next_index()].ivertex = 0;
    }

    ~spike(void) {
      delete faces[0];
      delete faces[1];
      delete faces[2];
      delete vlist;
    }

    void place_spike(void) {
      displacement.set(int_to_l3d_real(1000 - (rand() % 2000)),
                       int_to_l3d_real(1000 - (rand() % 2000)),
                       int_to_l3d_real(200),
                       int_to_l3d_real(1));
      velocity.set(int_to_l3d_real(0),
                   int_to_l3d_real(0),
                   int_to_l3d_real(-10-rand()%40),
                   int_to_l3d_real(0));

      l3d_matrix transform;

      transform =
        l3d_mat_translate(displacement.a[0],
                          displacement.a[1],
                          displacement.a[2]);


      int q;
      for(q=0; q<vlist->num_fixed_items; q++) {
        (*vlist)[q].reset();
        (*vlist)[q].transformed =	transform | (*vlist)[q].transformed;
      }
    }

    void move_spike(void) {
      displacement = displacement + velocity;
    }

    void reset(void) {
      int q;
      for(q=0; q<vlist->num_fixed_items; q++) {
        (*vlist)[q].reset();
      }
    }

    void apply_xform(const l3d_matrix & m) {
      int q;
      //sbegin apply_xform
      for(q=0; q<vlist->num_fixed_items; q++) {
        (*vlist)[q].transformed =	m | (*vlist)[q].transformed;
      }
      //send apply_xform
    }

    void project_spike() {
      int v;

      for(v=0; v<vlist->num_fixed_items; v++) {
        (*vlist)[v].transformed.X_ =
          l3d_mulri(l3d_mulrr(l3d_divrr( (*vlist)[v].transformed.X_,
                                         (*vlist)[v].transformed.Z_),
                              fovx),
                    xsize)
          +
          int_to_l3d_real(xsize >> 1);

        (*vlist)[v].transformed.Y_ =
          l3d_mulri(l3d_mulrr(l3d_divrr(-(*vlist)[v].transformed.Y_,
                                        (*vlist)[v].transformed.Z_),
                              fovy),
                    ysize)
          +
          int_to_l3d_real(ysize >> 1);
      }
    }

};

class my_pipeline : public l3d_pipeline {
  protected:
    l3d_rasterizer_2d_imp *ri;
    l3d_rasterizer_2d *r;


    spike spikes[MAXSPIKES];
    long i, j;
    long wc_win_width, wc_win_height, wc_win_dist, fov;
    long wc_win_left, wc_win_top;
    int numspikes;

    virtual int out_of_wc_bounds(l3d_point const &p);

    l3d_camera *camera;

  public:
    l3d_screen *s;
    int animation;
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
  s->sinfo->ext_to_native(0,0,0); //- allocate black background color
  red_color = s->sinfo->ext_to_native(255, 0, 0); // red, green, blue
  green_color = s->sinfo->ext_to_native(0, 255, 0); // red, green, blue
  blue_color = s->sinfo->ext_to_native(0,0, 255); // red, green, blue
  s->refresh_palette();

  fovx = float_to_l3d_real(0.5*1.0/tan(50./180. * PI));
  fovy = float_to_l3d_real(0.5*1.0/tan(30./180. * PI));
  xsize = s->xsize;
  ysize = s->ysize;

  numspikes = MAXSPIKES;
  for(i=0; i<MAXSPIKES; i++) {
    spikes[i].place_spike();

    spikes[i].faces[0]->final_color = red_color;
    spikes[i].faces[1]->final_color = green_color;
    spikes[i].faces[2]->final_color = blue_color;
  }

  camera = new l3d_camera;
  camera->fovx = 0.41955;
  camera->near_z = 0.01;
  camera->far_z = 100;

  animation = 1;
}

my_pipeline::~my_pipeline(void) {
  delete s;
  delete ri;
  delete r;
  delete camera;
}

void my_pipeline::key_event(int ch) {
  switch(ch) {
    case 'q': {
        exit(0);
      }
    case 'j':
      if(camera->rot_VUP_velocity>int_to_l3d_real(-95))
        camera->rot_VUP_velocity -= int_to_l3d_real(5);
      break;
    case 'l':
      if(camera->rot_VUP_velocity<int_to_l3d_real( 95))
        camera->rot_VUP_velocity += int_to_l3d_real(5);
      break;
    case 'J':
      if(camera->rot_VFW_velocity<int_to_l3d_real( 95))
        camera->rot_VFW_velocity += int_to_l3d_real(5);
      break;
    case 'L':
      if(camera->rot_VFW_velocity>int_to_l3d_real(-95))
        camera->rot_VFW_velocity -= int_to_l3d_real(5);
      break;
    case 'I':
      if(camera->rot_VRI_velocity<int_to_l3d_real( 95))
        camera->rot_VRI_velocity += int_to_l3d_real(5);
      break;
    case 'K':
      if(camera->rot_VRI_velocity>int_to_l3d_real(-95))
        camera->rot_VRI_velocity -= int_to_l3d_real(5);
      break;
    case 'k':
      if(camera->VFW_velocity    >int_to_l3d_real(-90))
        camera->VFW_velocity -= camera->VFW_thrust;
      break;
    case 'i':
      if(camera->VFW_velocity    <int_to_l3d_real( 90))
        camera->VFW_velocity += camera->VFW_thrust;
      break;
    case 's':
      if(camera->VRI_velocity    >int_to_l3d_real(-90))
        camera->VRI_velocity -= camera->VRI_thrust;
      break;
    case 'f':
      if(camera->VRI_velocity    <int_to_l3d_real( 90))
        camera->VRI_velocity += camera->VRI_thrust;
      break;
    case 'd':
      if(camera->VUP_velocity    >int_to_l3d_real(-90))
        camera->VUP_velocity -= camera->VUP_thrust;
      break;
    case 'e':
      if(camera->VUP_velocity    <int_to_l3d_real( 90))
        camera->VUP_velocity += camera->VUP_thrust;
      break;

      //- field-of-view modification
    case 'X':
      camera->fovx = camera->fovx + float_to_l3d_real(0.1);
      break;
    case 'x':
      camera->fovx = camera->fovx - float_to_l3d_real(0.1);
      break;
    case 'Y':
      camera->fovy = camera->fovy + float_to_l3d_real(0.1);
      break;
    case 'y':
      camera->fovy = camera->fovy - float_to_l3d_real(0.1);
      break;

      //- speed
  case 'v': case 'V':
      camera->VUP_thrust += int_to_l3d_real(1.0);
      if(camera->VUP_thrust  > 3.0) {
        camera->VUP_thrust  -= int_to_l3d_real(3.0);
      }
      camera->VFW_thrust = camera->VUP_thrust;
      camera->VRI_thrust = camera->VUP_thrust;
      break;

    case '1':
      animation = 1;
      break;
    case '0':
      animation = 0;
      break;
  }
}

void my_pipeline::update_event() {
  int i;


  camera->update();

  //move and display all spikes
  for(i=0;i<numspikes;i++) {
    // dynamics
    //sbegin wc

    if(animation) {
      spikes[i].move_spike();
    }
    spikes[i].reset();

    // sbegin modelxform
    spikes[i].apply_xform(
      l3d_mat_translate(spikes[i].displacement.a[0],
                        spikes[i].displacement.a[1],
                        spikes[i].displacement.a[2])
    );
    // send modelxform

    //- "transformed" now contains MODEL COORDINATES
    if(out_of_wc_bounds( (*spikes[i].vlist)[0].transformed)
        || out_of_wc_bounds( (*spikes[i].vlist)[1].transformed)
        || out_of_wc_bounds( (*spikes[i].vlist)[2].transformed)
        || out_of_wc_bounds( (*spikes[i].vlist)[3].transformed))
      //- we must check all vertices of the spike since its orientation
      //- is now arbitrary, and any point might be the foremost point
      //- which has now moved out of bounds
    {
      spikes[i].place_spike();
    }
    //send wc

    spikes[i].apply_xform(camera->viewing_xform);
    //- "transformed" now contains VIEW (or CAMERA) COORDINATES

  }
}

void my_pipeline::draw_event(void) {
  int i;
  r->clear_buffer();

  for(i=0;i<numspikes;i++) {
    int v;
    int infront = 1;
    for(v=0;v<spikes[i].vlist->num_fixed_items;v++) {

      //- at this point, "transformed" is in view coordinates
      if( (*spikes[i].vlist)[v].transformed.a[2] < camera->near_z ) {
        infront = 0;
        break;
      }
    }

    if(infront) {
      // perspective projection
      spikes[i].project_spike();

      for(int f=0; f<3; f++) {
        spikes[i].faces[f]->init_clip_ivertices();
        spikes[i].faces[f]->clip_to_polygon_2d(s->view_win);
        r->draw_polygon_flatshaded(spikes[i].faces[f]);
      }
    }
  }
  s->blit_screen();
}

int my_pipeline::out_of_wc_bounds(l3d_point const &p) {
  return(p.Z_<int_to_l3d_real(1));
}

int main(int argc, char **argv) {
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
