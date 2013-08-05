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

#ifndef __OBJECT3D_H
#define __OBJECT3D_H
#include "../../tool_os/memman.h"

#include "../../system/sys_dep.h"
#include "../../view/camera.h"
#include "../../math/matrix.h"
#include "../texture/texture.h"
#include "../polygon/poly3.h"
#include "../../datastr/list.h"
#include "../point/point.h"
#include "../../dynamics/plugins/plugload.h"

//
// constants representing internal constraints:
//    MAX_XFORMS:    total number of modeling transforms a l3d_object may have.

#define MAX_XFORMS 7

class l3d_object
{

  public:

    // constructor, destructor
    l3d_object(int num_fixed_vertices);
    char name[80];
    virtual ~l3d_object(void);
    l3d_object& operator= (const l3d_object &r);

    l3d_object *parent; // if this object is contained in another

    // dynamics
    virtual int update(void); // overridden in descendants

    // modeling and transformation
    l3d_matrix modeling_xform;                 // composite modelint xform
    l3d_matrix modeling_xforms[MAX_XFORMS];    // all modeling xform matrices
    int num_xforms;

    l3d_two_part_list<l3d_coordinate> *vertices;

    l3d_list<l3d_polygon_3d *> polygons;
    l3d_list<l3d_polygon_3d_node> polygon_node_pool;
    l3d_polygon_3d_node *nonculled_polygon_nodes; // points to 1st object under
    // consideration in polygon_node_pool

    void init_nonculled_list(void);

    int transform_stage;

    virtual void reset(void);
    virtual void transform(const l3d_matrix &m);
    void after_camera_transform(void)
    {
      // do NOT make the mistake of just processing the vertices
      // in the vertex list! because the nonculled_polygon_nodes
      // might ALSO contain pointers to polygons having DIFFERENT
      // vlists (i.e. contained objects in a sector). these vlists
      // also must be processed! therefore, instead of looping over
      // the object's own vlist, we must loop over all POLYGONS,
      // processing each POLYGON!

      //         for(register int i=0; i<vertices->num_fixed_items +
      //               vertices->num_varying_items; i++)
      //         {
      //           if((*vertices)[i].transform_stage >= transform_stage) {
      //             (*vertices)[i].transformed_intermediates[1] =
      //               (*vertices)[i].transformed;
      //           }
      //         }

      l3d_polygon_3d_node *n = nonculled_polygon_nodes;
      while(n) {
        for(register int i=0; i<n->polygon->clip_ivertices->num_items; i++) {
          //             if(!(
          //             (**n->polygon->vlist)[ (*n->polygon->clip_ivertices)[i].ivertex ]
          //               .transformed_intermediates[1] ==
          //             (**n->polygon->vlist)[ (*n->polygon->clip_ivertices)[i].ivertex ]
          //               .transformed;
          //             ))
          {
            (**n->polygon->vlist)[ (*n->polygon->clip_ivertices)[i].ivertex ]
            .transformed_intermediates[1] =
              (**n->polygon->vlist)[ (*n->polygon->clip_ivertices)[i].ivertex ]
              .transformed;
          }
        }
        // for each used vertex in each polygon, save the current cam
        // space coords as transformed_intermediate number 1.

        n = n->next;
      }

      // we must use 2 loops because the polygon's after_camera_transform
      // might already assume that ALL vertices in its vlist have already
      // been processed by the object after_camera_transform,
      // a situation which can only be guaranteed if ALL polygons
      // (and all the possibly different vlists they belong to) have
      // already been fully processed with the above loop.

      n = nonculled_polygon_nodes;
      while(n) {
        n->polygon->after_camera_transform();
        // allow each polygon to, if necessary, save any information
        // necessary as soon as camera coordinates are available (such
        // information might be the cam-space Z for u/z v/z interpolation,
        // which needs to be saved in the polygon's ivertex)

        n = n->next;
      }

    }

    void camera_transform(l3d_matrix const &m) {
      // special routine to execute camera transform AND do some
      // standard post-processing initialization requiring camera
      // space coordinates

      transform(m);
      after_camera_transform();
    }

    void apply_perspective_projection
    (const l3d_camera &aCamera, int xsize, int ysize);

    //   l3d_object(const l3d_object &o); // copy constructor

    // plug-in behavior
    char plugin_name[1024];
    l3d_plugin_loader *plugin_loader;
    //sbegin plugin_fptr
    void (*plugin_constructor)(l3d_object *target, void *data);
    void (*plugin_update)(l3d_object *target);
    void (*plugin_destructor)(l3d_object *target);
    void (*plugin_copy_data)(const l3d_object *target,l3d_object *copy_target);
    void *plugin_data;
    //send plugin_fptr
};

// Apply perspective projection formula to all points within a l3d_object

inline void l3d_object::apply_perspective_projection(const l3d_camera &aCamera,
    int xsize, int ysize)
{
  transform_stage++;

  l3d_polygon_3d_node *n = nonculled_polygon_nodes;
  while(n) {
    for(register int i=0; i<n->polygon->clip_ivertices->num_items; i++) {

      // for all post-clipping vertices in polygon: transform if needed
      if ((**(n->polygon->vlist))[(*n->polygon->clip_ivertices)[i].ivertex].
          transform_stage != transform_stage )
      {
        l3d_point *v;
        v = & ( (**(n->polygon->vlist))
                [(*n->polygon->clip_ivertices)[i].ivertex ].transformed );

#define COORD ( (**(n->polygon->vlist)) [(*n->polygon->clip_ivertices)[i].ivertex ] )

        (**(n->polygon->vlist))[(*n->polygon->clip_ivertices)[i].ivertex ].
        transform_stage = transform_stage;

        v->X_  = l3d_mulri(l3d_divrr(l3d_mulrr(v->X_,
                                               /* TIMES */ aCamera.fovx),
                                     /* DIVBY */ v->Z_),
                           /* TIMES */ xsize) + int_to_l3d_real(xsize>>1);
        v->Y_ = int_to_l3d_real(ysize>>1) -
                l3d_mulrr(l3d_divrr(l3d_mulrr(v->Y_,
                                              aCamera.fovy),
                                    v->Z_)  ,
                          ysize) ;

        //  	COORD.transformed_intermediates[0].X_ =
        //  	  l3d_mulri(l3d_mulrr(v->X_,aCamera.fovx),xsize) +
        //  	  l3d_mulri(v->Z_, xsize>>1);
        //  	COORD.transformed_intermediates[0].Y_ = l3d_mulri(v->Z_, ysize>>1) -
        //  	  l3d_mulri(l3d_mulrr(v->Y_, aCamera.fovy), ysize);
        //  	COORD.transformed_intermediates[0].Z_ = v->Z_;
        //  	COORD.transformed_intermediates[0].W_ = v->Z_;

        //	COORD.transformed_intermediates[1] = *v;

        //  	v->X_ = l3d_divrr(COORD.transformed_intermediates[0].X_ ,
        //  			  COORD.transformed_intermediates[0].W_);
        //  	v->Y_ = l3d_divrr(COORD.transformed_intermediates[0].Y_,
        //  			  COORD.transformed_intermediates[0].W_);

      }
    }
    n = n->next;
  }
}

#endif
