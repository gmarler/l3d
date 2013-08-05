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
#include <string.h>
#include "object3d.h"
#include "../../view/camera.h"
#include "../../tool_os/memman.h"


// l3d_object constructor and destructor - empty (could be extended
// later to include some default object initializations)

l3d_object::l3d_object(int num_fixed_vertices) {
  // allocate space for vertices: we allocate MORE than just num_vertices
  // because we want a free "pool" of vertices which will be dynamically
  // used to store newly created polygon vertices during clipping. note that
  // this means clipping of ALL polygons stored in this object. since clipping
  // can completely change from frame to frame, we don't want to dynamically
  // allocate space for new vertices during clipping but instead rely on
  // a fixed pool of vertices. we assume 50% more vertices can be created
  // during clipping (num_vertices/2); if this number is exceeded the
  // pool is "expanded" (actually a new larger pool is created, the contents
  // of the old pool are copied and deleted, and the new pool becomes the
  // active pool). actually the number of new vertices can be upper bounded
  // (loosely) by: 2 x num_polygons new vertices PER CLIP OPERATION (assuming
  // that every polygon gets split during a clip, yielding 2 new vertices
  // for convex polygons). Thus theoretically if the number of clip operations
  // is unbounded, so is the number of new vertices which can be created.
  // it is thus essential to allow for a growing list.
  //
  // it is also important to store the pool for free vertices in the same
  // array as the original vertices so that newly generated clip vertices
  // may be treated exactly the same as original vertices - this is
  // particularly convenient during rasterization. otherwise, when
  // doing anything with a polygon, we would need to either look in
  // the original_vertex_list OR the newly_generated_clip_vertex_list
  // to find the vertices referenced by the polygon. it is much simpler
  // to keep both in one list.

  vertices = new l3d_two_part_list<l3d_coordinate> ( num_fixed_vertices );
  // note that the # fixed vertices can never grow, since this is
  // a two-part-list (so that clipping can refer by index to both
  // original and clipped vertices easily)

  nonculled_polygon_nodes = NULL;

  transform_stage = 0; // how many times a transformation has been applied
  // to (surviving polygons of) this object. the objects polygons are culled,
  // and vertices of surviving polygons are transformed. the object is further
  // culled, vertices of surviving polygons are transformed. the idea
  // of transform_stage is to notice of a particular vertex has already
  // had the "latest transformation" applied. each vertex also stores a
  // transform_stage. if a vertex belongs to a surviving polygon, and
  // the vertex transform_stage < object transform_stage, then this vertex
  // must be transformed. later if another polygon shares this same vertex
  // this vertex will not be transformed a second time since the vertex
  // transform_stage = object transform_stage. for the next object transform,
  // we increment object transform_stage, making all vertex transform_stages
  // again "old". the other method would be to reset all vtx transform_stages
  // to zero before each new transformation, which would be more time consuming.
  // in any event we need to reset all vtx transform_stages to zero after
  // each frame (i.e. before the next round of transformations due to a
  // change in camera position or other possible culling parameters)
  num_xforms = 0;

  parent = NULL;

  plugin_constructor = NULL;
  plugin_update = NULL;
  plugin_destructor = NULL;
}

l3d_object & l3d_object::operator= (const l3d_object &r) {
  strcpy(name, r.name);
  parent = r.parent;

  num_xforms = r.num_xforms;
  modeling_xform = r.modeling_xform;
  for(int i=0; i<r.num_xforms; i++) {
    modeling_xforms[i] = r.modeling_xforms[i];
  }

  *vertices = *r.vertices; //- copy list and contents, not just pointer to list

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

  //- nonculled_polygon_nodes points to a polygon node in the polygon
  //- node pool. since we are making a copy, we have to point to the
  //- *corresponding* entry in the *new* node pool, which means we have to
  //- find the INDEX of the entry in the original list.

  nonculled_polygon_nodes = NULL;
  for(int i=0; i<r.polygon_node_pool.num_items; i++) {
    if(r.nonculled_polygon_nodes == &r.polygon_node_pool[i]) {
      nonculled_polygon_nodes = &polygon_node_pool[i];
    }
  }


  // ORDER DEPENDENCY: IMPORTANT
  // 1. FIRST, the polygons must be copied. this is done above.
  // 2. ONLY AFTERWARDS should the plug-in "copy data" be called
  // 3. in the case of the vidmesh plugin, the copy data routine
  //    makes a copy of the texture data, then rewrites all polygons
  //    to point to the new texture data. otherwise, by default,
  //    the copied polygons point to the same texture as originally.
  //    which is ok, unless the original object is deleted!!!

  // the polygon itself cannot be responsible for allocating
  // a new texture object; the texture is stored in the higher-level
  // object. thus the polygon clone routine cannot and should
  // not attempt to clone the texture; this is the responsibility
  // of the object (vidmesh plugin, e.g) containing the texture.

  transform_stage = r.transform_stage;
  strcpy(plugin_name, r.plugin_name);
  if(r.plugin_loader) {
    plugin_loader = r.plugin_loader->clone();
  } else {
    plugin_loader = NULL;
  }
  plugin_constructor = r.plugin_constructor;
  plugin_update = r.plugin_update;
  plugin_destructor = r.plugin_destructor;
  plugin_copy_data = r.plugin_copy_data;
  if(r.plugin_copy_data) {
    (*r.plugin_copy_data)(&r, this);
  } else {
    plugin_data = NULL;
  }

  return *this;
}

// l3d_object::l3d_object (l3d_object const &o) {
//   vertices = new l3d_two_part_list<l3d_coordinate> ( o.num_vertices );
//   vertices = o.vertices; // copies list and contents

//   polygons = new l3d_polygon_3d* [o.num_polygons];
//   this->num_polygons = o.num_polygons;
//   for(int i=0; i<num_polygons; i++) { polygons[i] = o.polygons[i]; }

//   polygon_node_pool = new l3d_polygon_3d_node [num_polygons];

//   nonculled_polygon_nodes = NULL;

//   transform_stage = 0; // how many times a transformation has been applied
//   num_xforms = 0;

// }

void l3d_object::init_nonculled_list(void) {
  l3d_polygon_3d_node *prev=NULL;

  // we MUST *FIRST* assign all nodes, THEN link nodes together. this
  // is because node assignment might cause the list to grow and the
  // objects to be copied, which would make any previously assigned
  // linked-list pointers invalid!!!!!

  polygon_node_pool.num_items = 0;
  for (int i=0; i<polygons.num_items; i++) {
    if(polygons[i]) {
      int next_pool_idx = polygon_node_pool.next_index();
      polygon_node_pool[next_pool_idx].polygon = polygons[i];
    } else {
      // polygon_node_pool[next_pool_idx].polygon = NULL;
    }
  }

  // link nodes together
  for(int i=0;i<polygon_node_pool.num_items; i++) {
    polygon_node_pool[i].prev = prev;
    polygon_node_pool[i].next = NULL;
    if (prev) {
      prev->next = &polygon_node_pool[i];
    }
    prev = &polygon_node_pool[i];
  }

  if(polygon_node_pool.num_items > 0) {
    nonculled_polygon_nodes = &polygon_node_pool[0];
  } else {
    nonculled_polygon_nodes = NULL;
  }
}

l3d_object::~l3d_object() {
  // delete the polygons in the list

  for(int i=0; i<polygons.num_items;i++) {
    //        printf("deleting poly %p in %p\n",polygons[i],this);
    //        printf("  vlist is %p, %p\n",polygons[i]->vlist, *polygons[i]->vlist);
    //        printf("  has %d verts\n",(**polygons[i]->vlist).num_fixed_items);
    delete polygons[i];
  }


  // vertices might be null if a descendant class uses vertex morphing with
  // vertex lists which do NOT belong to the object itself. in this case,
  // the derived class destructor first sets vertices to NULL, to ensure that
  // these external vertices don't get incorrectly deleted here.

  if(vertices) delete vertices;

  if(plugin_destructor) {
    (*plugin_destructor)(this);
  }

  if(plugin_loader) {
    plugin_loader->close();
  }

  if(plugin_loader) delete plugin_loader;
}

int l3d_object::update(void) {
  if(plugin_update) {
    (*plugin_update)(this);
  }

  return 1;
}

void l3d_object::transform(const l3d_matrix &m) {

  transform_stage++;

  // transform geometry

  l3d_polygon_3d_node *n = nonculled_polygon_nodes;

  while(n) {
    // for each nonculled polygon: transform its vertices (if not already
    // transformed), and also allow recomputation of sfc normal, etc...

    n->polygon->transform(m, transform_stage);
    n = n->next;
  }
}

void l3d_object::reset(void) {
  // initialize object
  init_nonculled_list();
  transform_stage = 0;

  // initialize vertex list (reset all clipped vertices)
  //DELME    objects[iObj]->num_clip_vertices = 0;
  vertices->num_varying_items = 0;


  // initialize all vertices (set transformed back to original)

  for(register int ivtx=0;
      ivtx<vertices->num_fixed_items;
      ivtx++)
  {
    (*vertices)[ivtx].reset();
  }

  // reset all polygons

  l3d_polygon_3d_node *n;
  n = nonculled_polygon_nodes;
  while(n) {
    n->polygon->init_clip_ivertices();
    n->polygon->init_transformed();
    n=n->next;
  }
}

