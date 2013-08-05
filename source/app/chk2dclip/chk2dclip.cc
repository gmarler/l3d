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

#include "screen.h"
#include "object3d.h"

class Tdummy_obj : public T3dobject {
    /* virtual */ int update(void) { return 0; }
};

main() {
  Tscreen s;
  Tdummy_obj obj,clipobj;
  T3dfacet facet,clipfacet;

  s.open_screen();

  /* four points */

  clipfacet.clip_ivertices[0] = 0;
  clipfacet.clip_ivertices[1] = 1;
  clipfacet.clip_ivertices[2] = 2;
  clipfacet.clip_ivertices[3] = 3;
  clipfacet.num_clip_pts = 4;
  clipfacet.final_color = 15;
  clipfacet.owner = &clipobj;

  clipobj.num_vertices = 4;
  clipobj.num_clip_vertices = 0;
  clipobj.view_vertices[0].X_ = Float2MT(100.0); clipobj.view_vertices[0].Y_ = Float2MT(100.0);
  clipobj.view_vertices[1].X_ = Float2MT(200.0); clipobj.view_vertices[1].Y_ = Float2MT(150.0);
  clipobj.view_vertices[2].X_ = Float2MT(150.0); clipobj.view_vertices[2].Y_ = Float2MT(200.0);
  clipobj.view_vertices[3].X_ = Float2MT( 50.0); clipobj.view_vertices[3].Y_ = Float2MT(120.0);

  s.fillpoly(&clipfacet);

  /* triangle */

  facet.clip_ivertices[0] = 0;
  facet.clip_ivertices[1] = 1;
  facet.clip_ivertices[2] = 2;
  facet.num_clip_pts = 3;
  facet.final_color = 63;
  facet.owner = &obj;

  obj.num_vertices = 3;
  obj.num_clip_vertices = 0;
  obj.view_vertices[0].X_ = Float2MT(0.0); obj.view_vertices[0].Y_ = Float2MT(0.0);
  obj.view_vertices[1].X_ = Float2MT(20.0); obj.view_vertices[1].Y_ = Float2MT(0.0);
  obj.view_vertices[2].X_ = Float2MT(250.0); obj.view_vertices[2].Y_ = Float2MT(300.0);

  for(int i=0; i<100; i++) {

    s.fillpoly(&clipfacet);

    facet.owner->num_clip_vertices=0;
    facet.clip_ivertices[0] = 0;
    facet.clip_ivertices[1] = 1;
    facet.clip_ivertices[2] = 2;
    facet.num_clip_pts = 3;

    facet.final_color = 109;
    s.fillpoly(&facet);

    facet.final_color = 63;
    if(s.get_rasterizer()->clip_poly2d(&facet,&clipfacet)) s.fillpoly(&facet);
    s.blit_screen();

    for(int j=0; j<3; j++) {
      obj.view_vertices[j].X_ += Int2MT(1);
      printf("%f ",obj.view_vertices[j].X_);
    }
    printf("\n");
    s.clear_screenbuf();
  }

  /* triangle two */

  facet.clip_ivertices[0] = 0;
  facet.clip_ivertices[1] = 1;
  facet.clip_ivertices[2] = 2;
  facet.num_clip_pts = 3;
  facet.final_color = 63;
  facet.owner = &obj;

  obj.num_vertices = 3;
  obj.num_clip_vertices = 0;
  obj.view_vertices[0].X_ = Float2MT(0.0); obj.view_vertices[0].Y_ = Float2MT(0.0);
  obj.view_vertices[1].X_ = Float2MT(600.0); obj.view_vertices[1].Y_ = Float2MT(0.0);
  obj.view_vertices[2].X_ = Float2MT(0.0); obj.view_vertices[2].Y_ = Float2MT(300.0);

  for(int i=0; i<2000; i++) {

    s.fillpoly(&clipfacet);

    facet.owner->num_clip_vertices=0;
    facet.clip_ivertices[0] = 0;
    facet.clip_ivertices[1] = 1;
    facet.clip_ivertices[2] = 2;
    facet.num_clip_pts = 3;

    facet.final_color = 109;

    facet.final_color = 63;
    if(s.get_rasterizer()->clip_poly2d(&facet,&clipfacet)) {
      printf("accepted %d\n", MT2Int(obj.view_vertices[0].X_));
      s.fillpoly(&facet);
      s.blit_screen();
    }

    for(int j=0; j<3; j++) {
      obj.view_vertices[j].X_ += Int2MT(1);
      //         printf("%f ",obj.view_vertices[j].X_);
    }
    //      printf("\n");
    s.clear_screenbuf();
  }


}
