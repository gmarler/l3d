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

#ifndef __BSPTREE_H
#define __BSPTREE_H
#include "../../tool_os/memman.h"

class l3d_halfspace;
class l3d_object_clippable_boundable;
class l3d_object;
class l3d_bsptree {
  protected:
    void _permanently_merge_objects(l3d_object *dest, l3d_object *src);
    void _recursive_delete_tree(l3d_halfspace *root);

  public:
    l3d_halfspace *root;
    l3d_bsptree(void);
    virtual ~l3d_bsptree(void);
    void compute_for_object(l3d_halfspace *root,
                            l3d_object_clippable_boundable *rest_of_obj);

    void push_polys_to_children
    (l3d_halfspace *tree,
     l3d_object_clippable_boundable *unsplit_obj_from_parent);

    void make_videoscape_meshes(l3d_halfspace *tree);
    void print_stats(l3d_halfspace *tree);
};

#endif
