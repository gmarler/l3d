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
#include <sys/time.h>

#include "../lib/tool_2d/screen.h"
#include "../lib/tool_os/dispatch.h"
#include "../lib/raster/rasteriz.h"
#include "../lib/tool_2d/scrinfo.h"
#include "../lib/system/fact0_3.h"

#include "../lib/geom/point/point.h"
#include "../lib/math/vector.h"
#include "../lib/math/matrix.h"

#define MAXDOTS 1500

#define PI 3.14159265

void choose_factories(void) {
  factory_manager_v_0_3.choose_factories();
}

l3d_real fovx, fovy;
int xsize, ysize;

class mydot {
  public:
    l3d_coordinate position;
    l3d_vector displacement;
    l3d_vector velocity;
    mydot(void) { position.original.set(int_to_l3d_real(0),
                                          int_to_l3d_real(0),
                                          int_to_l3d_real(0),
                                          int_to_l3d_real(1)); }
    virtual void place_dot(void) = 0;
    virtual void move_dot(void) = 0;
    virtual void project_dot(void) {
      position.transformed.X_ =
        l3d_mulri(l3d_mulrr(l3d_divrr( position.transformed.X_,
                                       position.transformed.Z_),
                            fovx),
                  xsize)
        +
        int_to_l3d_real(xsize >> 1);

      position.transformed.Y_ =
        l3d_mulri(l3d_mulrr(l3d_divrr(-position.transformed.Y_,
                                      position.transformed.Z_),
                            fovy),
                  ysize)
        +
        int_to_l3d_real(ysize >> 1);

    }
    virtual int is_alive(void) = 0;
};

class my_pipeline : public l3d_pipeline {
  protected:
    l3d_rasterizer_2d_imp *ri;
    l3d_rasterizer_2d *r;

    unsigned long color;

    mydot *dots[MAXDOTS];
    int numdots;

    long i, j;
    long wc_win_width, wc_win_height, wc_win_dist, fov;
    long wc_win_left, wc_win_top;


  public:
    l3d_screen *s;
    my_pipeline(void);
    virtual ~my_pipeline(void);

    void key_event(int ch);
    void update_event(void);
    void draw_event(void);
};

my_pipeline::my_pipeline(void) {
  s = factory_manager_v_0_3.screen_factory->create(400,300);
  ri = factory_manager_v_0_3.ras_2d_imp_factory->create(400,300,s->sinfo);
  r = new l3d_rasterizer_2d(ri);

  s->sinfo->ext_to_native(0,0,0);
  s->sinfo->ext_max_red =
    s->sinfo->ext_max_green =
      s->sinfo->ext_max_blue =  255;
  color = s->sinfo->ext_to_native(255, 255, 255); // red, green, blue
  s->refresh_palette();

  xsize = s->xsize;
  ysize = s->ysize;
  fovx = float_to_l3d_real(1.0/tan(50./180. * PI));
  fovy = float_to_l3d_real(1.0/tan(30./180. * PI));

}

my_pipeline::~my_pipeline(void) {

  int dot_i = 0;
  for(dot_i=0; dot_i<numdots; dot_i++) {
    delete dots[dot_i];
  }

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

  for(i=0;i<numdots;i++) {
    dots[i]->position.reset();
    dots[i]->move_dot();
    if(! (dots[i] -> is_alive())) {
      dots[i]->place_dot();
    }
    dots[i]->project_dot();
  }
}

void my_pipeline::draw_event(void) {
  int i;
  r->clear_buffer();

  for(i=0;i<numdots;i++) {
    if(dots[i]->position.transformed.X_>=0 &&
        dots[i]->position.transformed.X_ <= int_to_l3d_real(s->xsize) &&
        dots[i]->position.transformed.Y_ >= 0 &&
        dots[i]->position.transformed.Y_ <= int_to_l3d_real(s->ysize))
    {
      r->draw_point(ifloor(dots[i]->position.transformed.X_),
                    ifloor(dots[i]->position.transformed.Y_),
                    color);
    }
  }

  s->blit_screen();
}

class dot_particle : public mydot {
  protected:
    l3d_vector accel;
    float lifetime;
    long last_sec, last_usec;
  public:
    void place_dot(void); //- virtual
    void move_dot(void); //- virtual
    int is_alive(void);
};

void dot_particle::place_dot(void) {
  displacement.set(0,
                   0,
                   50,
                   0);
  position.reset();
  accel.set (float_to_l3d_real(0.0),
             float_to_l3d_real(-9.8),
             float_to_l3d_real(0.0),
             float_to_l3d_real(0.0));

  velocity.set(float_to_l3d_real( l3d_divri( int_to_l3d_real(rand()%500),10) - int_to_l3d_real(25) ),
               float_to_l3d_real( l3d_divri( int_to_l3d_real(rand()%500),10) ),
               float_to_l3d_real( l3d_divri( int_to_l3d_real(-rand()%500),10)),
               float_to_l3d_real(0.0));
  // lifetime = (rand() % 500) / 100.0 + 1.0;
  lifetime = 15.0;

  struct timeval tv;
  struct timezone tz;
  tz.tz_minuteswest = -60;
  gettimeofday(&tv,&tz);
  last_sec = tv.tv_sec;
  last_usec = tv.tv_usec;

}

void dot_particle::move_dot(void) {

  struct timeval tv;
  struct timezone tz;
  tz.tz_minuteswest = -60;
  gettimeofday(&tv,&tz);

  l3d_real delta_t = float_to_l3d_real
                     ( (tv.tv_sec + 0.000001 * tv.tv_usec -
                        (last_sec + 0.000001 * last_usec)) );

  displacement = displacement + velocity*delta_t;

  l3d_vector total_accel;
  total_accel = accel;
  velocity = velocity + total_accel*delta_t;

  if(displacement.a[0] > int_to_l3d_real(100)) {
    velocity.a[0] = l3d_divri(-velocity.a[0],4);
    displacement.a[0] = int_to_l3d_real(100);
  }
  if(displacement.a[0] < int_to_l3d_real(-100)) {
    velocity.a[0] = l3d_divri(-velocity.a[0],4);
    displacement.a[0] = int_to_l3d_real(-100);
  }
  if(displacement.a[2] > int_to_l3d_real(120)) {
    velocity.a[2] = l3d_divri(-velocity.a[2],4);
    displacement.a[2] = int_to_l3d_real(120);
  }
  if(displacement.a[2] < int_to_l3d_real(60)) {
    velocity.a[2] = l3d_divri(-velocity.a[2],4);
    displacement.a[2] = int_to_l3d_real(60);
  }
  if(displacement.a[1] < int_to_l3d_real(-10)) {
    displacement.a[1] = int_to_l3d_real(-10);
    velocity.a[1] = l3d_divri(-velocity.a[1],4);

  }
  position.reset();
  position.transformed = position.transformed + displacement;

  lifetime -= delta_t;

  last_sec = tv.tv_sec;
  last_usec = tv.tv_usec;

}

int dot_particle::is_alive(void) {
  return (lifetime > 0.0);
}

class my_pipeline_particle : public my_pipeline {
  public:
    my_pipeline_particle(void) {
      int i;

      for(i=0; i<MAXDOTS; i++) {
        dots[i] = new dot_particle();
        dots[i]->place_dot();
      }
      numdots = MAXDOTS;
    }
};

int main(int argc, char **argv) {

  choose_factories();

  l3d_dispatcher *d;
  my_pipeline *p;

  d = factory_manager_v_0_3.dispatcher_factory->create();
  p  = new my_pipeline_particle();

  d->pipeline = p;
  d->event_source = p->s;
  d->start();

  delete d;
  delete p;
}


