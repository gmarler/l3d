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

#include "p_cport.h"
#include "../../tool_os/memman.h"

l3d_polygon_2d* l3d_polygon_3d_portal_collidable::clone(void) {
  return new l3d_polygon_3d_portal_collidable(*this);
}

l3d_polygon_3d_portal_collidable::l3d_polygon_3d_portal_collidable
(const l3d_polygon_3d_portal_collidable &r)
    : l3d_polygon_3d_portal(r),
    l3d_polygon_3d_collidable(r)
{

  /*
  iv_items_factory = new l3d_polygon_ivertex_items_factory;
  init(-1); // param is ignored anyway

  iv_items_factory = r.iv_items_factory;
  vlist = r.vlist;
  *ivertices = *r.ivertices;
  *clip_ivertices = *r.clip_ivertices;
  *temp_clip_ivertices = *r.temp_clip_ivertices;

  // from l3d_polygon_3d
  sfcnormal = r.sfcnormal;
  center = r.center;
  zvisible = r.zvisible;

  // from l3d_polygon_3d_portal

  connected_sector = r.connected_sector;
  // note! the connected sector is the same object in memory as the original
  // sector; it is not a copied version of it. this is because the sector
  // is NOT OWNED by the portal, but is merely referenced by it. a copy of
  // the referenced sector could only occur in a class owning both the
  // portals and the referenced sectors.

  strcpy(connected_sector_name, r.connected_sector_name);

  // from l3d_polygon_3d_collidable
  plane = r.plane;

  */
}
