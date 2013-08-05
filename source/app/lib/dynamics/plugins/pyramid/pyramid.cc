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

#include "../../../system/sys_dep.h"
#include "../../../geom/object/object3d.h"
#include "../../../geom/texture/texload.h"
#include "../../../geom/polygon/p3_ltex.h"
#include "../plugenv.h"
#include "../../../tool_os/memman.h"

class l3d_plugin_pyramid {
  public:
    l3d_plugin_environment *env;

    int xtheta;

    l3d_plugin_pyramid::l3d_plugin_pyramid(l3d_plugin_environment *e) {
      env = e;
    }
    l3d_plugin_pyramid::~l3d_plugin_pyramid(void) {
      delete env; // the env is created at the time the plugin is initialized
      // and used nowhere else but by the plugin, so the plugin is responsible
      // for deleting its environment information
    }
};

extern "C" {

  void constructor(l3d_object *target, void *data) {
    l3d_plugin_pyramid *pyramid;

    target->plugin_data = pyramid =
                            new l3d_plugin_pyramid((l3d_plugin_environment *)data);

    int i;
    int vi;
    vi = 0;
    (*target->vertices)[vi].original.set(float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(1.));
    vi++;
    (*target->vertices)[vi].original.set(float_to_l3d_real(10.0),float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(1.));
    vi++;
    (*target->vertices)[vi].original.set(float_to_l3d_real(0.),float_to_l3d_real(10.),float_to_l3d_real(0.),float_to_l3d_real(1.));
    vi++;
    (*target->vertices)[vi].original.set(float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(10.),float_to_l3d_real(1.));

    l3d_polygon_3d_flatshaded *p;

    i = target->polygons.next_index();
    target->polygons[i] = p = new l3d_polygon_3d_flatshaded(3);
    p->vlist = &target->vertices;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=0;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=1;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=3;
    p->compute_center();target->polygons[0]->compute_sfcnormal();
    p->final_color = pyramid->env->sinfo->ext_to_native
                     (rand()%pyramid->env->sinfo->ext_max_red,
                      rand()%pyramid->env->sinfo->ext_max_green,
                      rand()%pyramid->env->sinfo->ext_max_blue);

    i = target->polygons.next_index();
    target->polygons[i] = p = new l3d_polygon_3d_flatshaded(3);
    p->vlist = &target->vertices;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=2;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=3;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=1;
    p->compute_center();p->compute_sfcnormal();
    p->final_color = pyramid->env->sinfo->ext_to_native
                     (rand()%pyramid->env->sinfo->ext_max_red,
                      rand()%pyramid->env->sinfo->ext_max_green,
                      rand()%pyramid->env->sinfo->ext_max_blue);

    i = target->polygons.next_index();
    target->polygons[i] = p = new l3d_polygon_3d_flatshaded(3);
    p->vlist = &target->vertices;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=0;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=2;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=1;
    p->compute_center();p->compute_sfcnormal();
    p->final_color = pyramid->env->sinfo->ext_to_native
                     (rand()%pyramid->env->sinfo->ext_max_red,
                      rand()%pyramid->env->sinfo->ext_max_green,
                      rand()%pyramid->env->sinfo->ext_max_blue);

    i = target->polygons.next_index();
    target->polygons[i] = p = new l3d_polygon_3d_flatshaded(3);
    p->vlist = &target->vertices;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=3;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=2;
    (*(p->ivertices))[p->ivertices->next_index()].ivertex=0;
    p->compute_center();p->compute_sfcnormal();
    p->final_color = pyramid->env->sinfo->ext_to_native
                     (rand()%pyramid->env->sinfo->ext_max_red,
                      rand()%pyramid->env->sinfo->ext_max_green,
                      rand()%pyramid->env->sinfo->ext_max_blue);

    target->num_xforms = 3;

    pyramid->xtheta=0;
    target->modeling_xforms[0] = l3d_mat_rotz(pyramid->xtheta);
    target->modeling_xforms[1].set
    ( float_to_l3d_real(1.), float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(70.),
      float_to_l3d_real(0.), float_to_l3d_real(1.), float_to_l3d_real(0.), float_to_l3d_real(0.),
      float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(1.), float_to_l3d_real(0.),
      float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(1.) );
    target->modeling_xforms[2].set
    ( float_to_l3d_real(1.), float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(0.),
      float_to_l3d_real(0.), float_to_l3d_real(1.), float_to_l3d_real(0.), float_to_l3d_real(0.),
      float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(1.), float_to_l3d_real(50.),
      float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(1.) );


    target->modeling_xform=
      target->modeling_xforms[2] | (target->modeling_xforms[1] | target->modeling_xforms[0]);

  }

  void update(l3d_object *target) {
    l3d_plugin_pyramid *pyramid;
    pyramid = (l3d_plugin_pyramid *)target->plugin_data;
    pyramid->xtheta += 1; if (pyramid->xtheta>359) pyramid->xtheta-=360;
    target->modeling_xforms[0]=l3d_mat_rotz(pyramid->xtheta);
    target->modeling_xform=
      target->modeling_xforms[2] | target->modeling_xforms[1] | target->modeling_xforms[0];

  }

  void destructor(l3d_object *target) {
    delete (l3d_plugin_pyramid *) target->plugin_data;
  }

  void copy_data(l3d_object *target, l3d_object *copy_target) {
    l3d_plugin_pyramid *pyramid;
    pyramid = (l3d_plugin_pyramid *) target->plugin_data;

    l3d_plugin_environment *new_env;
    l3d_plugin_pyramid *new_pyramid;

    new_env = pyramid->env->clone();
    new_env->data = NULL;
    new_pyramid = new l3d_plugin_pyramid(new_env);
    new_pyramid->xtheta = pyramid->xtheta;

    copy_target->plugin_data = (void *) new_pyramid;
  }
}
