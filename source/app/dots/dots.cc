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

#define MAXDOTS 150

#define PI 3.14159265

enum progtypes { RUNWAY, STARS, SNOW };

progtypes progtype;

//- STEP 1: CHOOSE THE FACTORIES

void choose_factories(void) {
  //sbegin choose_fac
  factory_manager_v_0_1.choose_factories();
  //send choose_fac
}

//- STEP 2: DECLARE A PIPELINE SUBCLASS

l3d_real fovx, fovy;
int xsize, ysize;
//- in a real program these projection parameters, which are set by the main
//- program but used by the dot's projection routines, would not be global
//- but rather in a "projection parameters" class, used by both the dot
//- class and the main program class

//sbegin mydot
//sbegin project0
class mydot {
    //send project0
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
    //sbegin project1
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
    //send project1
};
//send mydot

//sbegin dbstore0
class my_pipeline : public l3d_pipeline {
    //send dbstore0
  protected:
    l3d_rasterizer_2d_imp *ri;
    l3d_rasterizer_2d *r;

    unsigned long color;

    //sbegin dbstore1
    mydot *dots[MAXDOTS];
    int numdots;
    //send dbstore1

    long i, j;
    long wc_win_width, wc_win_height, wc_win_dist, fov;
    long wc_win_left, wc_win_top;

    virtual int out_of_wc_bounds(l3d_point const &p)=0;

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
  s = factory_manager_v_0_1.screen_factory->create(320,240);
  ri = factory_manager_v_0_1.ras_2d_imp_factory->create(320,240,s->sinfo);
  r = new l3d_rasterizer_2d(ri);

  s->sinfo->ext_to_native(0,0,0); //- allocate black background color
  s->sinfo->ext_max_red =
    s->sinfo->ext_max_green =
      s->sinfo->ext_max_blue =  255;
  color = s->sinfo->ext_to_native(255, 255, 255); // red, green, blue
  s->refresh_palette();

  xsize = s->xsize;
  ysize = s->ysize;
  fovx = float_to_l3d_real(1.0/tan(50./180. * PI));
  fovy = l3d_mulrr(fovx,float_to_l3d_real(320.0/240.0));
    //-  320/240 is the physical screen size ratio, which is 4/3 or 1.3333
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

//sbegin mainloop
void my_pipeline::update_event() {
  int i;

  /* move and display all dots */
  for(i=0;i<numdots;i++) {
    //- dynamics / simulation
    dots[i]->position.reset();
    dots[i]->move_dot();
    if(out_of_wc_bounds(dots[i]->position.transformed)) {
      dots[i]->place_dot();
    }

    //- perspective projection
    dots[i]->project_dot();
  }
}

void my_pipeline::draw_event(void) {
  int i;
  r->clear_buffer();

  for(i=0;i<numdots;i++) {
    //sbegin cull
    //- culling
    if(dots[i]->position.transformed.X_>=0 &&
        dots[i]->position.transformed.X_ <= int_to_l3d_real(s->xsize) &&
        dots[i]->position.transformed.Y_ >= 0 &&
        dots[i]->position.transformed.Y_ <= int_to_l3d_real(s->ysize))
    {
      //send cull
      //- rasterization
      //sbegin raster
      r->draw_point(ifloor(dots[i]->position.transformed.X_),
                    ifloor(dots[i]->position.transformed.Y_),
                    color);
      //send raster
    }
  }

  //sbegin blit
  s->blit_screen();
  //send blit
}
//send mainloop

//-
//- runway pipeline subclass
//-

//sbegin movedot0
class dot_runway_light : public mydot {
  public:
    void place_dot(void); //- virtual
    void move_dot(void); //- virtual
};

void dot_runway_light::place_dot(void) {
  position.reset();

  displacement.X_ = int_to_l3d_real(xsize);
  velocity.set(int_to_l3d_real(-5),
               int_to_l3d_real(0),
               int_to_l3d_real(0),
               int_to_l3d_real(0));
}

void dot_runway_light::move_dot(void) {
  displacement = displacement + velocity;
  position.reset();
  position.transformed = position.transformed + displacement;
}

//send movedot0
class my_pipeline_runway : public my_pipeline {
  public:
    my_pipeline_runway(void) {
      int dot_i = 0;
      for(dot_i=0; dot_i<MAXDOTS; dot_i++) {
        dots[dot_i] = new dot_runway_light;
      }

      dot_i = 0;
      for(i=-200; i<200; i+=40) {
        for(j=100; j<500; j+=40) {
          dots[dot_i]->position.reset();
          dots[dot_i]->displacement.set(int_to_l3d_real(i),
                                        int_to_l3d_real(-70),
                                        int_to_l3d_real(j),
                                        int_to_l3d_real(0));
          dots[dot_i]->velocity.set(int_to_l3d_real(-5),
                                    int_to_l3d_real(0),
                                    int_to_l3d_real(0),
                                    int_to_l3d_real(0));
          dot_i++;
        }
      }
      numdots = dot_i;
    }

    int out_of_wc_bounds(l3d_point const &p);
};

//sbegin bounds0
int my_pipeline_runway::out_of_wc_bounds(l3d_point const &p) {
  return(p.X_<int_to_l3d_real(-200));
}

//send bounds0
//-
//- snow pipeline subclass
//-

//sbegin movedot1
class dot_snowflake : public mydot {
  public:
    void place_dot(void); //- virtual
    void move_dot(void); //- virtual
};

void dot_snowflake::place_dot(void) {
  displacement.set(int_to_l3d_real( - (rand() % xsize)),
                   int_to_l3d_real(100 + (rand()%100)),
                   int_to_l3d_real(64 + (rand()%ysize)),
                   int_to_l3d_real(0));
  position.reset();
  velocity.set(int_to_l3d_real(rand()%2),
               int_to_l3d_real(-1),
               int_to_l3d_real(0),
               int_to_l3d_real(0));
}

void dot_snowflake::move_dot(void) {
  displacement = displacement + velocity;
  position.reset();
  position.transformed = position.transformed + displacement;
  velocity.set(int_to_l3d_real(rand()%2),
               int_to_l3d_real(-1),
               int_to_l3d_real(0),
               int_to_l3d_real(0));
}

//send movedot1
class my_pipeline_snow : public my_pipeline {
  public:
    my_pipeline_snow(void) {
      int i;

      for(i=0; i<MAXDOTS; i++) {
        dots[i] = new dot_snowflake();
        dots[i]->place_dot();
      }
      numdots = MAXDOTS;
    }
    int out_of_wc_bounds(l3d_point const &p);
};

//sbegin bounds1
int my_pipeline_snow::out_of_wc_bounds(l3d_point const &p) {
  return(p.Y_<int_to_l3d_real(-70));
}

//send bounds1
//-
//- star field pipeline subclass
//-

//sbegin movedot2
class dot_star : public mydot {
  public:
    void place_dot(void); //- virtual
    void move_dot(void); //- virtual
};

void dot_star::place_dot(void) {
  displacement.set(int_to_l3d_real(1000 - (rand() % 2000)),
                   int_to_l3d_real(1000 - (rand() % 2000)),
                   int_to_l3d_real(200 + (rand()%1000)),
                   int_to_l3d_real(0));
  position.reset();
  velocity.set(int_to_l3d_real(0),
               int_to_l3d_real(0),
               int_to_l3d_real(-10),
               int_to_l3d_real(0));
}

void dot_star::move_dot(void) {
  displacement = displacement + velocity;
  position.reset();
  position.transformed = position.transformed + displacement;
}

//send movedot2
class my_pipeline_stars : public my_pipeline {
  public:
    my_pipeline_stars(void) {

      int i;

      for(i=0; i<MAXDOTS; i++) {
        dots[i] = new dot_star();
        dots[i]->place_dot();
      }
      numdots = MAXDOTS;
    }
    int out_of_wc_bounds(l3d_point const &p);
};

//sbegin bounds2
int my_pipeline_stars::out_of_wc_bounds(l3d_point const &p) {
  return(p.Z_<int_to_l3d_real(20));
}

//send bounds2
int main(int argc, char **argv) {
  if(argc==2) {
    sscanf(argv[1], "%d", &progtype);
  }
  else {
    progtype = RUNWAY;
  }

  choose_factories();

  l3d_dispatcher *d;
  my_pipeline *p;

  //- STEP 3: CREATE A DISPATCHER

  d = factory_manager_v_0_1.dispatcher_factory->create();

  //- STEP 4: CREATE A PIPELINE

  //- plug our custom behavior pipeline into the dispatcher
  switch(progtype) {
    case RUNWAY: p  = new my_pipeline_runway(); break;
    case STARS: p  = new my_pipeline_stars(); break;
    case SNOW: p  = new my_pipeline_snow(); break;
  }

  //- STEP 5: START DISPATCHER

  d->pipeline = p; //- polymorphic assignment
  d->event_source = p->s;
  d->start();

  // clean up
  delete d;
  delete p;
}


