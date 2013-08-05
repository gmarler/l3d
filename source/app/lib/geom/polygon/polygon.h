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

#ifndef __POLYGON_H
#define __POLYGON_H
#include "../../tool_os/memman.h"

#include "../../system/sys_dep.h"
#include "../../math/matrix.h"
#include "../vertex/coord.h"
#include "../../datastr/list.h"

#define DEFAULT_IVERTICES_PER_POLY 10

class l3d_rasterizer_2d;

//sbegin ivertex_decl
class l3d_polygon_ivertex {
  public:
    int ivertex;

    virtual l3d_polygon_ivertex& operator= (const l3d_polygon_ivertex &r) {
      // DELME printf("virtual ivertex assignment\n");
      ivertex = r.ivertex;
      return *this;
    }

    virtual ~l3d_polygon_ivertex(void) {
      //    printf("iv dest\n");
    }// destructor MUST be virtual here since
    // l3d_polygon destroys the l3d_polygon_vertex without necessarily
    // knowing what concrete type they are (since l3d_polygon descendants
    // can override create_vertices to create new concrete l3d_polygon_vertex
    // types)
};
//send ivertex_decl

//sbegin ivtx_factory
class l3d_polygon_ivertex_items_factory :
      public l3d_list_item_factory<l3d_polygon_ivertex>
{
  public:
    virtual l3d_polygon_ivertex_items_factory *clone(void);
};

//send ivtx_factory

class l3d_polygon_2d {
  protected:

  public:
    l3d_polygon_ivertex_items_factory *iv_items_factory;
    l3d_polygon_2d(void) { iv_items_factory = NULL; };
    // this constructor is called as a base class constructor by derived
    // classes to get an "empty" l3d_polygon which is then initialized
    // within the derived class's constructor

    l3d_polygon_2d(int num_pts);
    // this constructor is called by clients to get a "real" l3d_polygon,
    // creating the vertices and all

    l3d_polygon_2d(const l3d_polygon_2d &r);

    void init(int num_pts);
    virtual ~l3d_polygon_2d(void);

    // vlist is an EXTERNALLY ALLOCATED AND list of vertices, containing
    // both original vertices and, immediately thereafter, temporary
    // vertices generated during clipping. vlist MUST have been allocated
    // with new (not statically declared) since this list may be expanded
    // and the original memory discarded during the expansion operation.

    //sbegin vlist
    l3d_two_part_list<l3d_coordinate> **vlist;
    //send vlist

    // original def'n of poly: original indices of original vertices in vlist

    l3d_list<l3d_polygon_ivertex>
    *ivertices,
    *clip_ivertices,
    *temp_clip_ivertices;

    void init_clip_ivertices(void);// copies ivertices into clip_ivertices
    // DELME  void init_temp_clip_ivertices(void);// copies clip_ivertices

    void swap_temp_and_main_clip_lists(void);

    virtual void clip_segment_to_edge_2d
    (l3d_polygon_ivertex *ivertex,
     const l3d_point *pC0, const l3d_point *pC1,
     int crossedge_idx0,int crossedge_idx1);
    virtual int clip_to_edge_2d
    (const l3d_point *pPoint1, const l3d_point *pPoint2);
    int clip_to_polygon_2d(const l3d_polygon_2d *p_poly);
    int side_of_point(const l3d_point *p,
                      const l3d_point *p1,
                      const l3d_point *p2);

    int next_clipidx_left(int idx, int count) const;
    int next_clipidx_right(int idx, int count) const;

  virtual void draw(l3d_rasterizer_2d *r) {}
    virtual l3d_polygon_2d *clone(void);
};

inline void l3d_polygon_2d::init_clip_ivertices(void) {
  // copy original ivertex list into clip_ivertex list

  //    printf("uh... %p = %p\n", & ((*clip_ivertices)[0]), & ((*ivertices)[0]));
  //    printf("access cl: %d...\n", (*clip_ivertices)[0].ivertex);
  //    printf("access iv: %d...\n", (*ivertices)[0].ivertex);
  //    (*clip_ivertices)[0].ivertex = (*ivertices)[0].ivertex;
  //    printf("ivtx assignment worked...\n");
  //    (*ivertices)[0] = (*ivertices)[0];
  //    printf("iv=iv worked...\n");
  //    (*clip_ivertices)[0] = (*clip_ivertices)[0];
  //    printf("ci=ci worked...\n");
  *clip_ivertices = *ivertices ; // copies params and data
  //  printf("assign done\n");
}

// DELME inline void l3d_polygon_2d::init_temp_clip_ivertices(void) {
// DELME   // copy clip_ivertex list into temporary clip_ivertex list
// DELME
// DELME  *temp_clip_ivertices = *clip_ivertices; // copies params and data
// DELME
// DELME }

inline void l3d_polygon_2d::swap_temp_and_main_clip_lists(void) {
  // swap pointers between main clipped list and temp clip list.
  // this is much faster, of course, than actually recopying the contents...

  l3d_list<l3d_polygon_ivertex> *temp;
  temp = clip_ivertices;
  clip_ivertices = temp_clip_ivertices;
  temp_clip_ivertices = temp;
}

inline int l3d_polygon_2d::next_clipidx_left(int idx, int count) const {
  idx = idx - 1;
  if(idx<0) idx = count-1;
  return idx;
}

inline int l3d_polygon_2d::next_clipidx_right(int idx, int count) const {
  idx = idx + 1;
  if(idx>=count) idx = 0;
  return idx;
}

inline int l3d_polygon_2d::side_of_point(const l3d_point *p,
    const l3d_point *p1, const l3d_point *p2)
{
  l3d_real s;

  const l3d_real EPSILON=float_to_l3d_real(0.05);

  #ifdef FIXED_POINT_MATH

#undef FX_PRECIS
#undef FX_PRECIS_QUOT
#undef FX_PRECIS_MASK
#undef FX_PRECIS_MAX
#undef FX_CVT
#undef FX_CVT_BACK

  #define FX_PRECIS 8
  #define FX_PRECIS_QUOT 8

  #define FX_SIGN_MASK    0x80000000L
  #define FX_ALLBITS_MASK 0xFFFFFFFFL

//    /* binary: 0....0 0011 1111 1111 1111; accesses the precision part */
  #define FX_PRECIS_MASK 0x000000FFL
  #define FX_PRECIS_MAX 256.0

  #define FX_CONVERT >> FX_PRECIS_DEFAULT-FX_PRECIS
  #define FX_CONVERT_BACK << FX_PRECIS_DEFAULT-FX_PRECIS

  #else

  #define FX_CONVERT 
  #define FX_CONVERT_BACK

  #endif

  // #include "../../math/fix_lowp.h"  

  //   s=
  //      l3d_mulrr((p2->Y_ - p1->Y_) FX_CONVERT,(p2->X_ - p->X_) FX_CONVERT)
  //    - l3d_mulrr((p2->X_ - p1->X_) FX_CONVERT,(p2->Y_ - p->Y_) FX_CONVERT);

  // a 2D vector normal to the segment (x0,y0) to (x1,y1) is
  // a 2D vector normal to the segment (x0,y0) to (x0+dx,y0+dy) is
  // the vector from point (x0,y0) to point (x0-dy, y0+dx)
  // (flip x/y movement and point it inside, remember this is in screen
  //  space with x+ -> right and y+ -> down)
  //                 point (x0,y0) to point (x0-(y1-y0), y0+(x1-x0))
  // the vector      (x0-(y1-y0)-x0, y0+(x1-x0)-y0)
  //                 (y0-y1, x1-x0)
  //
  // a 2D vector from the line segment to the point is *p - *p1
  //
  // the dot between the normal vector and the segment-point vector is
  // the cosine of the angle. if positive, dot is on "front" side of
  // the normal (and edge), otherwise on the "back" side
  //

  //(p2.y-p1.y)*(p2.x-p.x) - (p2.x-p1.x)*(p2.y-p.y)

  //(p1.y-p2.y)*(p.x-p1.x) + (p2.x-p1.x)*(p.y-p1.y)
  //

  l3d_vector normal_vector_pointing_inside_segment
  (p1->Y_ - p2->Y_,
   p2->X_ - p1->X_,
   int_to_l3d_real(0),
   int_to_l3d_real(0));

#ifndef FIXED_POINT_MATH
  s = dot(normal_vector_pointing_inside_segment, *p - *p1);
#else

  //- explicitly formulate dot product for fixed point to avoid overflow
//    s = FX_CVT_BACK
//      (l3d_mulrr(FX_CVT(normal_vector_pointing_inside_segment.X_),
//                 FX_CVT((p->X_ - p1->X_)))
//       + l3d_mulrr(FX_CVT(normal_vector_pointing_inside_segment.Y_),
//                   FX_CVT((p->Y_ - p1->Y_))));
    s = 
       l3d_mulrr(normal_vector_pointing_inside_segment.X_ FX_CONVERT,
                 (p->X_ - p1->X_) FX_CONVERT)
       + l3d_mulrr((normal_vector_pointing_inside_segment.Y_) FX_CONVERT,
                   (p->Y_ - p1->Y_) FX_CONVERT ) ;

#endif

//    /*
//    printf("my way %f: (%f %f) (%f %f) @ %f,%f\n", l3d_real_to_float(s),
//          normal_vector_pointing_inside_segment.X_,
//          normal_vector_pointing_inside_segment.Y_,
//          (*p-*p1).X_,
//          (*p-*p1).Y_,
//          p->X_,
//          p->Y_
//    );
//    */
  // we use -epsilon as the "outside" criteria to allow points "RIGHT ON"
  // the edge to be counted as inside; otherwise they might, due to
  // numerical inaccuracy, be marked as "just outside" and cause a false
  // clip. this is in particular problematic with polygons which share
  // vertices/edges with portals, since then the polygon lies EXACTLY
  // on the portal clip edge.

  if(s<-EPSILON) return -1;
  else if(s>-EPSILON) return 1;
  else return 0;


#include "../../math/fix_prec.h"

}


#include "../../raster/rasteriz.h"

#endif
