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

#include<string.h>
#include "sector.h"
#include "../../tool_os/memman.h"

l3d_sector & l3d_sector::operator= (const l3d_sector &r) {
  strcpy(name, r.name);
  parent = r.parent;

  num_xforms = r.num_xforms;
  modeling_xform = r.modeling_xform;
  for(int i=0; i<r.num_xforms; i++) {
    modeling_xforms[i] = r.modeling_xforms[i];
  }

  *vertices = *r.vertices;

  polygons = r.polygons;
  for(int i=0; i<r.polygons.num_items; i++) {
    l3d_polygon_3d *p = dynamic_cast<l3d_polygon_3d *>(r.polygons[i]->clone());
    if(p) {
      p->vlist = &vertices;
      polygons[i] = p;
    } else {
      printf("error during cloning: somehow a non-3d polygon is in an object!\n");
    }
  }


  polygon_node_pool = r.polygon_node_pool;

  // nonculled_polygon_nodes points to a polygon node in the polygon
  // node pool. since we are making a copy, we have to point to the
  // corresponding entry in the new node pool, which means we have to
  // find the INDEX of the entry in the original list.

  nonculled_polygon_nodes = NULL;
  for(int i=0; i<r.polygon_node_pool.num_items; i++) {
    if(r.nonculled_polygon_nodes == &r.polygon_node_pool[i]) {
      nonculled_polygon_nodes = &polygon_node_pool[i];
    }
  }

  transform_stage = r.transform_stage;
  strcpy(plugin_name, r.plugin_name);
  plugin_loader = r.plugin_loader;
  plugin_constructor = r.plugin_constructor;
  plugin_update = r.plugin_update;
  plugin_destructor = r.plugin_destructor;
  plugin_data = r.plugin_data;

  // IMPORTANT NOTE!!! the pointers to other objects remain the same.
  // the pointed-to objects do not get copied. this is because the
  // objects do not belong to this sector but are merely referenced
  // by this sector.
  //
  // this means that there should be a class one level higher in the
  // inheritance hierarchy which contains a global pool of objects,
  // and a global pool of sectors which reference these objects.
  // the assignment operator for this higher-level class would copy
  // all objects, then copy all sectors, then remap all sector-object
  // references to point to the copied objects.

  // in other words, the limited context of a single sector is not enough
  // to know how to copy the externally owned objects which are referenced.

  objects = r.objects;

  return *this;
}


l3d_sector::~l3d_sector(void) {
  // DO NOT DELETE THE OBJECTS IN THE SECTOR!!! they don't BELONG to the
  // sector, they are just LOCATED IN the sector. the world is responsible
  // for deleting the plug-in objects.
  //    for(int i=0; i<objects->num_items;i++) {
  //      delete (*objects)[i];
  //    }
  delete objects;

}

void l3d_sector::render(l3d_polygon_2d *clipwin,
                        l3d_rasterizer_3d *rasterizer, l3d_camera *camera,
                        int xsize, int ysize)
{
  static int level=0;
  level++;

  // limit depth of portal recursion
  // if (level>3) {level--;return;}
  //  printf("%d: rendering sector %s\n", level, name);

  int iObj, iFacet;

  // variables for back-face culling
  l3d_vector facet_to_cam_vec;
  l3d_vector N;

  l3d_polygon_3d_clippable *pc;

  //- init polys for all objects contained in the sector

  for(int i=0; i<objects->num_items; i++) {
    (*objects)[i]->init_nonculled_list();
    (*objects)[i]->vertices->num_varying_items = 0;
    (*objects)[i]->transform_stage = 0;

    for(register int ivtx=0; ivtx<(*objects)[i]->vertices->num_fixed_items; ivtx++) {
      (*((*objects)[i]->vertices))[ivtx].reset();
    }

    l3d_polygon_3d_node *n;
    n = (*objects)[i]->nonculled_polygon_nodes;
    while(n) {
      n->polygon->init_clip_ivertices();
      n->polygon->init_transformed();
      n=n->next;
    }

    //- position all vertices of object into world space

    // (*objects)[i] transform
    if ((*objects)[i]->num_xforms) {
      (*objects)[i]->transform((*objects)[i]->modeling_xform);
    }

    //- sector transform
    if (num_xforms) {
      (*objects)[i]->transform(modeling_xform);
    }
  }

  //- now concatenate all polys in all objects into one big object-polys
  //- list.
  // IMPORTANT!!! after this operation, the nonculled_polygon_nodes
  // CANNOT be used anymore within the context of one single object,
  // because the last node of an object's nonculled poly list points to
  // the first node of the NEXT object's nonculled list, because this
  // is exactly what the concatenation does.

  l3d_polygon_3d_node *n_objects=0;

  for(int i=0; i<objects->num_items; i++) {
    // init_nonculled_list was already called in the first loop,
    // where we placed all polys into world space. otherwise, we
    // must call init_nonculled list here.
    // (*objects)[i]->init_nonculled_list();

    //- prepend this object's polys to the overall list of all polys in all
    //- objects
    if((*objects)[i]->nonculled_polygon_nodes) {
      l3d_polygon_3d_node *n_temp;
      n_temp = (*objects)[i]->nonculled_polygon_nodes;
      while(n_temp->next) {
        n_temp=n_temp->next;
      }

      // assign current poly list to end of this object's poly list
      n_temp->next = n_objects;
      if(n_objects) { n_objects->prev = n_temp; } // dont forget the rev link!
      // reassign the overall poly list to be the new, longer list
      n_objects = (*objects)[i]->nonculled_polygon_nodes;

      // the head element of the list can have no previous element
      n_objects->prev = NULL;
    }
  }

  //- initialize polys for the sector geometry
  init_nonculled_list();
  vertices->num_varying_items = 0;
  transform_stage = 0;

  for(register int ivtx=0; ivtx<vertices->num_fixed_items; ivtx++) {
    (*(vertices))[ivtx].reset();
  }

  l3d_polygon_3d_node *n;
  n = nonculled_polygon_nodes;
  while(n) {
    n->polygon->init_clip_ivertices();
    n->polygon->init_transformed();
    n=n->next;
  }


  //- position all vertices of sector into world space

  if (num_xforms) {
    transform(modeling_xform);
  }

  //- at this point, the sector geometry and the geometry of all
  //- contained objects are in world coordinates and have any transformations
  //- already applied. now we dump all polys from all objects and the sector
  //- into one big list - from now on, there is no distinction between
  //- sector polys and contained-object-polys. they are just polys from now
  //- on. note that the object polys come after the sector polys in the list.
  //    l3d_polygon_3d_node *last_sector_node;
  {
    l3d_polygon_3d_node *n;

    //- for all object polygons and their vlists: set the transform stage
    //- to be the same as for the sector.
    // this is necessary because the object maybe
    // underwent more or fewer transformations due to its own modeling
    // transformation status (yes or no), meaning that till now the
    // transform stage for the objectpolys and the sectorpolys might be
    // different. now, we must set them the same since they are one list.

    n = n_objects;
    while(n) {
      n->polygon->set_transform_stage(transform_stage);
      n = n->next;
    }

    n = nonculled_polygon_nodes;
    if(n) {
      while(n->next) { n = n->next; }
      //	last_sector_node = n;
      n->next = n_objects; //- append object polys to sector poly list
      if(n_objects) { n_objects->prev = n; }
    } else {
      //	last_sector_node=NULL;
      nonculled_polygon_nodes = n_objects;
    }
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

  camera_transform(camera->viewing_xform); // sector and object geometry
  //    for(int i=0;i<objects->num_items;i++){
  //      (*objects)[i]->transform(camera->viewing_xform); // object geometry
  //    }

  // draw sector geometry

  // back face culling: elimination of back-facing facets
  //      n = nonculled_polygon_nodes;
  //      int debug_cnt=0;
  //      while(n) {
  //        debug_cnt++;
  //        n = n->next;
  //      }
  //      printf("%d polys after camera transform", debug_cnt);

  n = nonculled_polygon_nodes;

  while(n) {

    //  #ifdef CLIP3D

    //        // FIXME: this could actually be done BEFORE the camera transform
    //        // by applying the inverse camera transform to get the camera
    //        // position in world space, doing frustum clipping there
    //        l3d_point origin(0,0,0,1), p0, p1;
    //        l3d_plane clip_plane;

    //        int survived=1;
    //    int idx0,idx1;

    //    idx0 = clipwin->clip_ivertices->num_items-1;
    //    idx1 = 0;

    //    while(survived) {
    //      p1 = (**(clipwin->vlist))[(*clipwin->clip_ivertices)[idx0].ivertex].transformed;
    //      p0 = (**(clipwin->vlist))[(*clipwin->clip_ivertices)[idx1].ivertex].transformed;

    //      /*

    //        reverse projection, not needed if clipwin is in 3d

    //      p0.X_ -= screen->xsize / 2;
    //      p0.Y_ = screen->ysize/2 - p0.Y_;
    //      p0.Z_ = l3d_mulri(camera->fovx, screen->xsize);
    //      p1.X_ -= screen->xsize / 2;
    //      p1.Y_ = screen->ysize/2 - p1.Y_;
    //      p1.Z_ = l3d_mulri(camera->fovx, screen->xsize);
    //      */

    //      clip_plane.align_with_points( origin, p0, p1);

    //      pc = dynamic_cast<l3d_polygon_3d_clippable *>(n->polygon);
    //      if(pc) if( ! (survived=pc->clip_to_plane(clip_plane)) ) break;

    //      idx0 = idx1;
    //      if(idx0==clipwin->clip_ivertices->num_items-1) break;
    //      idx1++;
    //    }

    //    /*
    //      origin.set(0,0,20,1);
    //      p0.set(0,100,20,1);
    //      p1.set(100,100,20,1);

    //      clip_plane.align_with_points( origin, p1, p0);

    //      pc = dynamic_cast<l3d_polygon_3d_clippable *>(n->polygon);
    //      if(pc) survived=pc->clip_to_plane(clip_plane);
    //      pc = NULL;


    //    */
    //  #endif

    //    {
    //  	l3d_polygon_3d_portal *ptest;
    //  	if ( (ptest=dynamic_cast<l3d_polygon_3d_portal *>(n->polygon)) ) {
    //            printf("portal %p exists before backface\n", ptest);
    //          }
    //    }


    N.set(n->polygon->sfcnormal.transformed.X_ - n->polygon->center.transformed.X_,
          n->polygon->sfcnormal.transformed.Y_ - n->polygon->center.transformed.Y_,
          n->polygon->sfcnormal.transformed.Z_ - n->polygon->center.transformed.Z_,
          int_to_l3d_real(0));

    facet_to_cam_vec.set (0 - n->polygon->center.transformed.X_,
                          0 - n->polygon->center.transformed.Y_,
                          0 - n->polygon->center.transformed.Z_,
                          int_to_l3d_real(0));

    /* dot product explicitly formulated so no fixed point overflow */
    /* (facet_to_cam_vec is very large) */
    /* (use of the inline function "dot" uses default fixed precision) */
    //#include "../lib/math/fix_lowp.h"
    //#define BACKFACE_EPS float_to_l3d_real(0.0)
    //      if(( l3d_mulrr(FX_CVT(N.X_), FX_CVT(facet_to_cam_vec.X_)) +
    //	   l3d_mulrr(FX_CVT(N.Y_), FX_CVT(facet_to_cam_vec.Y_)) +
    //	   l3d_mulrr(FX_CVT(N.Z_), FX_CVT(facet_to_cam_vec.Z_)) > BACKFACE_EPS)
    /* equivalent to: if(N.dot(facet_to_cam_vec) > 0 ) */
    if(
      //	 survived

      //	 &&

#define BACKFACE_EPS float_to_l3d_real(0.001)
      (dot(N,facet_to_cam_vec) > BACKFACE_EPS )


      //#include "../lib/math/fix_prec.h"

      &&

      n->polygon->clip_near_z(camera->near_z)


    )

    {
      // polygon is not culled
    } else {
      // take this polygon out of the list of nonculled_polygon_nodes

      if(n->prev) {
        n->prev->next = n->next;
      } else {
        nonculled_polygon_nodes = n->next;
      }

      if(n->next) {
        n->next->prev = n->prev;
      }
    }


    n = n->next;
  }

  // project surviving vertices of surviving polygons

  //    // for the sector:
  apply_perspective_projection
  (*camera, xsize, ysize);
  //    // for the contained objects:
  //    for(int iObj=0; iObj<objects->num_items;objects++) {
  //      (*objects)[iObj]->apply_perspective_projection(*camera,xsize,ysize);
  //    }


  //     /////////MIRRRRRRRRRROOOOOOOOOOOOORRRRRRRRR
  //     // transform surviving polys to camera space

  //     sector->transform(camera.viewing_xform);

  //     // back face culling: elimination of back-facing facets
  //     n = sector->nonculled_polygon_nodes;
  //     while(n) {
  //       N.set(n->polygon->sfcnormal.transformed.X_ - n->polygon->center.transformed.X_,
  // 	    n->polygon->sfcnormal.transformed.Y_ - n->polygon->center.transformed.Y_,
  // 	    n->polygon->sfcnormal.transformed.Z_ - n->polygon->center.transformed.Z_,
  // 	    int_to_l3d_real(0));

  //       facet_to_cam_vec.set (0 - n->polygon->center.transformed.X_,
  // 			    0 - n->polygon->center.transformed.Y_,
  // 			    0 - n->polygon->center.transformed.Z_,
  // 			    int_to_l3d_real(0));

  //       /* dot product explicitly formulated so no fixed point overflow */
  //       /* (facet_to_cam_vec is very large) */
  //       /* (use of the inline function "dot" uses default fixed precision) */
  //       //#include "../lib/math/fix_lowp.h"
  //       //#define BACKFACE_EPS float_to_l3d_real(0.0)
  //       //      if(( l3d_mulrr(FX_CVT(N.X_), FX_CVT(facet_to_cam_vec.X_)) +
  //       //	   l3d_mulrr(FX_CVT(N.Y_), FX_CVT(facet_to_cam_vec.Y_)) +
  //       //	   l3d_mulrr(FX_CVT(N.Z_), FX_CVT(facet_to_cam_vec.Z_)) > BACKFACE_EPS)
  //          /* equivalent to: if(N.dot(facet_to_cam_vec) > 0 ) */
  //       if((dot(N,facet_to_cam_vec) > 0 )
  //       //#include "../lib/math/fix_prec.h"

  // 	 &&

  // 	 n->polygon->clip_near_z(camera.near_z))

  // 	{
  // 	  // polygon is not culled
  // 	} else {
  // 	  // take this polygon out of the list of nonculled_polygon_nodes
  // 	  if(n->prev) {
  // 	    n->prev->next = n->next;
  // 	  } else {
  // 	    sector->nonculled_polygon_nodes = n->next;
  // 	  }

  // 	  if(n->next) {
  // 	    n->next->prev = n->prev;
  // 	  }
  // 	}


  //       n = n->next;
  //     }
  //     /////////MIRRRRRRRRRROOOOOOOOOOOOORRRRRRRRR

  int ncount=0;
  n = nonculled_polygon_nodes;
  while(n) {

    //  	l3d_polygon_3d_portal *ptest;
    //  	if ( (ptest=dynamic_cast<l3d_polygon_3d_portal *>(n->polygon)) ) {
    //            printf("portal %p exists before clipping\n", ptest);
    //          }

    //      pc = dynamic_cast<l3d_polygon_3d_clippable *>(n->polygon);
    //
    //      if (pc) //  || (!pc && printf("nonclippable!\n") && ( n->polygon->clip_to_polygon_2d(clipwin)))) {
    if(n->polygon->clip_to_polygon_2d(clipwin))
    {


      // polygon survived all culling tests and will be drawn.
      l3d_polygon_3d_portal *p;
      if ( (p=dynamic_cast<l3d_polygon_3d_portal *>(n->polygon)) ) {
        //          printf("traversign portal %p\n", p);

        //  #ifdef CLIP3D
        //  	  // restore the original 3d coordinates; at this point, the
        //  	  // portal is projected into 2d, and a reverse projection
        //  	  // into 3d would lose information and cause inaccurate
        //  	  // clipping

        //  	  int pi, idxv,idxc;
        //  	  (*clipwin->vlist)->num_varying_items = 0;
        //  	  clipwin->clip_ivertices->num_items=0;

        //  	  for(pi=0; pi<p->clip_ivertices->num_items; pi++) {
        //  	    idxc = clipwin->clip_ivertices->next_index();
        //  	    idxv = (*clipwin->vlist)->next_varying_index();
        //  	    (**clipwin->vlist)[idxv].transformed =
        //  	      (**p->vlist)[(*p->clip_ivertices)[pi].ivertex].
        //  	      transformed_intermediates[1];

        //  	    (*clipwin->clip_ivertices) [idxc].ivertex = idxv;
        //  	  }

        //  #endif

        //draws lines around portals
        //            /*
        //  	  int pi=0;
        //  	  unsigned long pcol;
        //  	  pcol = 0xFFFF;
        //  	  l3d_point p0,p1;
        //  	  p0 = (**p->vlist)[(*p->clip_ivertices)[0].ivertex].transformed;
        //  	  for(pi=1;pi<p->clip_ivertices->num_items;pi++) {
        //  	    p1 = (**p->vlist)[(*p->clip_ivertices)[pi].ivertex].transformed;
        //  	    rasterizer->draw_line(p0.X_,p0.Y_,p1.X_,p1.Y_,pcol);
        //  	    p0 = p1;
        //  	  }
        //  	  p1 = (**p->vlist)[(*p->clip_ivertices)[0].ivertex].transformed;
        //  	  rasterizer->draw_line(p0.X_,p0.Y_,p1.X_,p1.Y_,pcol);
        //            */

        p->connected_sector->render(p,rasterizer,camera,xsize,ysize);
      } else {
        ncount++;
        n->polygon->draw(rasterizer);
      }
    }
    n = n->next;
  }
  //  printf("sector %p, %d polys drawn\n",this, ncount);


  //    if(last_sector_node) {
  //      last_sector_node->next=NULL;
  //    }

  //       printf("%d: leaving sector %s\n", level, name);
  level--;
}

int l3d_sector::update(void) {
  for(register int i=0; i<objects->num_items; i++) {
    (*objects)[i]->update();
  }
  return 1;
}

