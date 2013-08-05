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

#include "shapes.h"
#include "../lib/tool_os/memman.h"
#include <stdlib.h>
#include <string.h>

pyramid::pyramid(l3d_texture_loader *l) :
    l3d_object(4)
{
  (*vertices)[0].original.set(float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(1.));
  (*vertices)[1].original.set(float_to_l3d_real(10.0),float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(1.));
  (*vertices)[2].original.set(float_to_l3d_real(0.),float_to_l3d_real(10.),float_to_l3d_real(0.),float_to_l3d_real(1.));
  (*vertices)[3].original.set(float_to_l3d_real(0.),float_to_l3d_real(0.),float_to_l3d_real(10.),float_to_l3d_real(1.));

  l3d_polygon_3d_textured *p;
  int pi;

  //sbegin texcre
  pi = polygons.next_index();
  polygons[pi] = p = new l3d_polygon_3d_textured(3);
  //send texcre

  polygons[pi]->vlist = &vertices;
  (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=0; (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=1; (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=3;

  //sbegin loadtex
  l->load("test.ppm");
  p->texture = new l3d_texture;
  p->texture->tex_data = new l3d_texture_data;
  p->texture->tex_data->width = l->width;
  p->texture->tex_data->height = l->height;
  p->texture->tex_data->data = l->data;
  p->texture->owns_tex_data = true;
  //send loadtex
  //sbegin texorient
  p->texture->O = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[0].ivertex];
  p->texture->U = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[1].ivertex];
  p->texture->V = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[2].ivertex];
  //send texorient
  //sbegin comp3d
  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();
  p->assign_tex_coords_from_tex_space(*p->texture);
  //send comp3d

  pi = polygons.next_index();
  polygons[pi] = p = new l3d_polygon_3d_textured(3);
  polygons[pi]->vlist = &vertices;

  (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=2; (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=3; (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=1;

  p->texture = new l3d_texture;
  p->texture->tex_data = new l3d_texture_data;
  p->texture->tex_data->width = l->width;
  p->texture->tex_data->height = l->height;
  p->texture->tex_data->data = l->data;
  p->texture->owns_tex_data = false;

  p->texture->O = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[0].ivertex];
  p->texture->U = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[1].ivertex];
  p->texture->V = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[2].ivertex];

  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();
  p->assign_tex_coords_from_tex_space(*p->texture);

  pi = polygons.next_index();
  polygons[pi] = p = new l3d_polygon_3d_textured(3);
  polygons[pi]->vlist = &vertices;
  (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=0; (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=2; (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=1;

  p->texture = new l3d_texture;
  p->texture->tex_data = new l3d_texture_data;
  p->texture->tex_data->width = l->width;
  p->texture->tex_data->height = l->height;
  p->texture->tex_data->data = l->data;
  p->texture->owns_tex_data = false;

  p->texture->O = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[0].ivertex];
  p->texture->U = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[1].ivertex];
  p->texture->V = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[2].ivertex];

  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();
  p->assign_tex_coords_from_tex_space(*p->texture);

  pi = polygons.next_index();
  polygons[pi] = p = new l3d_polygon_3d_textured(3);
  polygons[pi]->vlist = &vertices;
  (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=3; (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=2; (*(polygons[pi]->ivertices))[polygons[pi]->ivertices->next_index()].ivertex=0;

  p->texture = new l3d_texture;
  p->texture->tex_data = new l3d_texture_data;
  p->texture->tex_data->width = l->width;
  p->texture->tex_data->height = l->height;
  p->texture->tex_data->data = l->data;
  p->texture->owns_tex_data = false;

  p->texture->O = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[0].ivertex];
  p->texture->U = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[1].ivertex];
  p->texture->V = (**(polygons[pi]->vlist))[(*(polygons[pi]->ivertices))[2].ivertex];

  polygons[pi]->compute_center();polygons[pi]->compute_sfcnormal();
  p->assign_tex_coords_from_tex_space(*p->texture);

  num_xforms = 2;
  xtheta=0;
  modeling_xforms[0] = l3d_mat_rotx(xtheta);
  modeling_xforms[1].set
  ( float_to_l3d_real(1.), float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(0.),
    float_to_l3d_real(0.), float_to_l3d_real(1.), float_to_l3d_real(0.), float_to_l3d_real(0.),
    float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(1.), float_to_l3d_real(0.),
    float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(0.), float_to_l3d_real(1.) );

  modeling_xform=
    modeling_xforms[1] | modeling_xforms[0];
}

pyramid::~pyramid(void) {
  // polygons deleted by parent class l3d_object
  //    for(register int i=0; i<polygons.num_items; i++) {
  //  //      delete polygons[i];
  //    }
}

int pyramid::update(void) {
  xtheta += dx_theta; if (xtheta>359) xtheta-=360;
  modeling_xforms[0]=l3d_mat_rotx(xtheta);
  modeling_xform=
    modeling_xforms[1] | modeling_xforms[0];
}






