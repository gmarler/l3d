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
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "../lib/geom/object/object3d.h"
#include "../lib/tool_2d/screen.h"
#include "../lib/tool_os/dispatch.h"
#include "../lib/raster/rasteriz.h"
#include "../lib/tool_2d/scrinfo.h"
#include "../lib/geom/world/world.h"
#include "../lib/system/fact0_2.h"
#include "../lib/geom/texture/texture.h"
#include "../lib/geom/texture/tl_ppm.h"
#include "../lib/pipeline/pi_wor.h"

#include "shapes.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>

class my_pipeline : public l3d_pipeline_world {
  public:
    my_pipeline(l3d_world *w) :
    l3d_pipeline_world(w) {}
    void key_event(int ch); // from dispatcher
};

class my_world :
      public l3d_world,
      public l3d_texture_computer
{
  public:
    my_world(void);
    void update_all(void);

    virtual ~my_world(void) {
      delete texture_loader;
      delete scache;
    }
    // no destructor necessary for objects created in constructor;
    // base class destructor takes care of these.
    l3d_texture_loader *texture_loader;
    l3d_surface_cache *scache;

    int compute_light_level(l3d_real u, l3d_real v);
    void place_lamp(void);
    void reset_surface_cache(void) {
      scache->reset();
    }
};


void my_pipeline::key_event(int c) {
  l3d_pipeline_world::key_event(c);

  switch(c) {
    case 'p': {
        my_world *w;
        w = dynamic_cast<my_world *>(world);
        if(w) {
          w->place_lamp();
          w->reset_surface_cache();
        }
      }
  }
}

main() {
  l3d_dispatcher *d;
  my_pipeline *p;
  my_world *w;

  factory_manager_v_0_2.choose_factories();
  d = factory_manager_v_0_2.dispatcher_factory->create();

  // plug our custom behavior pipeline into the dispatcher
  w = new my_world();
  p  = new my_pipeline(w);
  d->pipeline = p;// polymorphic assignment
  d->event_source = w->screen;

  // start dispatcher with the pipeline's screen
  d->start();

  // clean up
  delete p;
  delete d;
  delete w;
}

void my_world::update_all(void) {
  l3d_world::update_all();
}

my_world::my_world(void)
    : l3d_world(400,300)
{


  rasterizer_3d_imp->fovx = &(camera->fovx);
  rasterizer_3d_imp->fovy = &(camera->fovy);
  rasterizer_3d_imp->screen_xsize = &(screen->xsize);
  rasterizer_3d_imp->screen_ysize = &(screen->ysize);

  l3d_screen_info *si = screen->sinfo;
  texture_loader = new l3d_texture_loader_ppm(screen->sinfo);

  camera->VRP.set(0,0,-50,0);
  camera->near_z = float_to_l3d_real(5.5);
  camera->far_z = int_to_l3d_real(500);

  scache = new l3d_surface_cache(screen->sinfo);

  int i,j,k,onum;

  i=10; j=0; k=20;
  k=0;

  for(i=1; i<200; i+=50) {
    objects[onum = objects.next_index()] = new pyramid(texture_loader,scache);

    if (objects[onum]==NULL) exit;
    objects[onum]->modeling_xforms[0].set
    ( int_to_l3d_real(1), int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(i),
      int_to_l3d_real(0), int_to_l3d_real(1), int_to_l3d_real(0), int_to_l3d_real(1),
      int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(1), int_to_l3d_real(1),
      int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(0), int_to_l3d_real(1) );
    objects[onum]->modeling_xform =
      objects[onum]->modeling_xforms[0] ;
  }

  screen->refresh_palette();
}

int my_world::compute_light_level
(l3d_real u, l3d_real v)
{
  l3d_real x,y,z;

  // from tex space u,v coords, convert to world space x,y,z

  x = O.X_ + l3d_mulri(U.X_, u) + l3d_mulri(V.X_, v);
  y = O.Y_ + l3d_mulri(U.Y_, u) + l3d_mulri(V.Y_, v);
  z = O.Z_ + l3d_mulri(U.Z_, u) + l3d_mulri(V.Z_, v);

  // x,y,z are now the world-space coordinates of the lumel under
  // consideration

  l3d_point aLight(0,0,0,1); // light at current camera position = origin
  // (since we only rasterize sublightmaps when we have placed the light
  // by interactive positioning via key 'P')

  // we define 2 vectors: L, the vector from the facet (more precisely from
  // the facet's center) to the light source, and N, the facet's normal
  // vector. Note that vector FROM facet center TO light source is
  // computed as light_source - facet_center and not vice versa.
  // also note that the surface normal vector is defined as starting at the
  // (transformed) facet center and not at the origin. (If we defined N
  // just as aFacet.xfrm_sfcnormal.X_, Y_, Z_), this would implicitly
  // assume that N starts at the origin, which is not the case.)

  l3d_vector L(aLight.X_ - x ,
               aLight.Y_ - y ,
               aLight.Z_ - z ,
               int_to_l3d_real(0)),

  N(cross(U,V)); // correct if portalizer returns correct UVs

  l3d_real intensity;

  // calculate f_att attenuation factor to reduce sharp fall-off of
  // light intensity based on distance; this makes light more gradually
  // dim as objects are further from the light source. (Foley/van Dam/
  // Feiner/Hughes, Equation 16.8)

  l3d_real f_att=l3d_divrr(int_to_l3d_real(1) ,
                           (float_to_l3d_real(0.1)+float_to_l3d_real(0.02*sqrt(l3d_real_to_float(dot(L,L))))));
  if (f_att>int_to_l3d_real(1)) f_att=int_to_l3d_real(1);

  // calculate intensity value (0-MAX_LIGHT_LEVELS/2) for facet
  intensity = l3d_mulrr ( l3d_mulrr(int_to_l3d_real(MAX_LIGHT_LEVELS/2) , f_att) ,
                          dot(normalized(N),normalized(L)) );
  if (intensity>int_to_l3d_real(MAX_LIGHT_LEVELS/2)) { intensity = int_to_l3d_real(MAX_LIGHT_LEVELS/2); }

  if (intensity<int_to_l3d_real(0))  { intensity = int_to_l3d_real(0);  }

  // apply intensity value (brightness) to existing facet color
  // we store both the intensity and colored intensity to save computation
  // time later
  return l3d_real_to_int(intensity);
}


void my_world::place_lamp(void)
{
  int iObj, iFacet;

  l3d_vector facet_to_cam_vec;
  l3d_vector N;

  for(iObj=0; iObj<objects.num_items; iObj++) {
    l3d_object *object = objects[iObj];

    l3d_polygon_3d_clippable *pc;

    // initialize object
    object->init_nonculled_list();
    object->vertices->num_varying_items = 0;
    object->transform_stage = 0;
    for(register int ivtx=0; ivtx<object->vertices->num_fixed_items; ivtx++) {
      (*(object->vertices))[ivtx].reset();
    }

    l3d_polygon_3d_node *n;
    n = object->nonculled_polygon_nodes;
    while(n) {
      n->polygon->init_clip_ivertices();
      n->polygon->init_transformed();
      n=n->next;
    }

    // position all vertices of object into world space

    if (object->num_xforms) {
      object->transform(object->modeling_xform);
    }

    // cull/clip polys in world space


    // copy object for mirror usage
    //    l3d_object copied_sector_in_wc(*sector);
    //    l3d_object copied_camera(camera);
    //    l3d_matrix reversal_xform (-1.0,0.0,0.0,0.0,
    //				0.0,-1.0,0.0,0.0,
    //				0.0,0.0,-1.0,0.0,
    //				0.0,0.0,0.0,1.0);

    // transform surviving polys to camera space

    object->camera_transform(camera->viewing_xform);

    //// light all lumels

    n = object->nonculled_polygon_nodes;
    while(n) {
      l3d_polygon_3d_textured_lightmapped *pt;

      pt = dynamic_cast<l3d_polygon_3d_textured_lightmapped *>
           (n->polygon);
      if(pt) {
        int lumel_y,lumel_x;

        // from texture calculator:
        O = pt->surface_orientation.O.transformed;
        U = pt->surface_orientation.U.transformed - O;
        V = pt->surface_orientation.V.transformed - O;
        printf("obj %d \n", iObj);
        O.print();
        U.print();
        V.print();

        for(lumel_y=0; lumel_y < pt->lightmap->tex_data->height; lumel_y++) {
          for(lumel_x=0; lumel_x<pt->lightmap->tex_data->width;lumel_x++) {
            l3d_real u,v;
            u = l3d_divri(int_to_l3d_real(lumel_x),
                          pt->lightmap->tex_data->width);
            v = l3d_divri(int_to_l3d_real(lumel_y),
                          pt->lightmap->tex_data->height);
            int new_light = // note int, not char, for possible overflow
              (int)(pt->lightmap->tex_data->data
                    [ lumel_y * pt->lightmap->tex_data->width + lumel_x ]) +
              compute_light_level(u, v);

            if ( new_light > MAX_LIGHT_LEVELS ) new_light = MAX_LIGHT_LEVELS;

            pt->lightmap->tex_data->data
            [ lumel_y * pt->lightmap->tex_data->width + lumel_x ] = new_light;


          }
        }
      }

      n = n->next;
    }
  }
}

