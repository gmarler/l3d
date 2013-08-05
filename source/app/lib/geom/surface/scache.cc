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

#include "scache.h"

#include "../../tool_2d/si_rgb.h"
#include "../../tool_os/memman.h"

l3d_surface::l3d_surface(void) {
  surface = NULL;
  polygon = NULL;
}

// cant be inline in header file because of forward declaration
l3d_surface::~l3d_surface(void)
{
  // don't delete the polygon; we simply used the polygon pointer
  // as an ID to the associated polygon, but we don't own it.

  // on the other hand we DO own the surface data, so if
  // we are destroyed, so is the surface.

  if(surface) delete surface;
if(polygon) { polygon->surface = NULL; }
}


l3d_texture *l3d_surface_cache::
combine_lightmap_and_texmap(int width,
                            int height,
                            const l3d_polygon_3d_textured_lightmapped *polygon)
{

  // see if texture is already in cache
  // a linear search is so inelegant

  {
    int i;

    for(i=0; i<surfaces->num_items; i++) {
      if((*surfaces)[i].polygon == polygon) { // we already computed the lightmap
        return (*surfaces)[i].surface;
      }
    }
  }

  l3d_texture *new_tex;
  l3d_surface *new_surface;

  new_tex = new l3d_texture;

  int next_surface_index = surfaces->next_index();
  (*surfaces)[next_surface_index].polygon = polygon;
  (*surfaces)[next_surface_index].surface = new_tex;

  new_tex->tex_data = new l3d_texture_data;
  new_tex->owns_tex_data = true;
  new_tex->tex_data->width = width; // max width of new TILED AND LIT texture
  new_tex->tex_data->height = height;
  new_tex->tex_data->data = new unsigned char [sinfo->bytes_per_pixel *
                            new_tex->tex_data->width *
                            new_tex->tex_data->height];

  int i,j,u,v;

  //    printf("height is %d, width %d\n",
  //             new_tex->tex_data->height,
  //             new_tex->tex_data->width
  //    );

  //- tile texture into larger tiled area
  for(v=0; v<new_tex->tex_data->height; v++) {
    for(u=0; u<new_tex->tex_data->width; u++) {
      for(i=0; i<sinfo->bytes_per_pixel; i++) {
        *(new_tex->tex_data->data
          + (v*new_tex->tex_data->width + u) * sinfo->bytes_per_pixel
          + i)
        =
          *(polygon->texture->tex_data->data
            + ( ((v&(polygon->texture->tex_data->height-1))*polygon->texture->tex_data->width
                 + (u&(polygon->texture->tex_data->width-1))) * sinfo->bytes_per_pixel
                + i ) ) ;
      }
    }
  }

  unsigned char *c2 = new_tex->tex_data->data;
  unsigned char *l = polygon->lightmap->tex_data->data;
  l3d_real lx, ly, dlx, dly;

  // do NOT apply lightmap to ENTIRE tiled surface area, but only to
  // the USED (i.e. visible) surface area!

  //  #ifdef ONLY_LIGHT_USED_PART
  //    dly = l3d_divrr(int_to_l3d_real(polygon->lightmap->tex_data->height) ,
  //  		  int_to_l3d_real(polygon->surface_used_v1-
  //  				  polygon->surface_used_v0));
  //    dlx = l3d_divrr(int_to_l3d_real(polygon->lightmap->tex_data->width) ,
  //  		  int_to_l3d_real(polygon->surface_used_u1-
  //  				  polygon->surface_used_u0));
  //    for(j=polygon->surface_used_v0, ly=int_to_l3d_real(0);
  //        j<polygon->surface_used_v1;
  //        j++, ly+=dly)
  //    {
  //     for(i=polygon->surface_used_u0, lx=int_to_l3d_real(0);
  //         i<polygon->surface_used_u1;
  //         i++, lx+=dlx)
  //     {
  //  #else

  dly = l3d_divrr(int_to_l3d_real(polygon->lightmap->tex_data->height) ,
                  int_to_l3d_real(new_tex->tex_data->height));
  dlx = l3d_divrr(int_to_l3d_real(polygon->lightmap->tex_data->width) ,
                  int_to_l3d_real(new_tex->tex_data->width));
  for(j=0, ly=int_to_l3d_real(0); j<new_tex->tex_data->height; j++, ly+=dly) {
    for(i=0, lx=int_to_l3d_real(0); i<new_tex->tex_data->width; i++, lx+=dlx) {
      //  #endif

      int i_texel=(j*new_tex->tex_data->width + i) * sinfo->bytes_per_pixel;
      c2 = new_tex->tex_data->data + i_texel;

      int lumel_idx = l3d_real_to_int(ly)*polygon->lightmap->tex_data->width+l3d_real_to_int(lx);
      //printf("lumel idx %d, coord %d %d, lx ly %f %f\n", lumel_idx,i,j, lx, ly);

      int lumel =  l[lumel_idx];
      sinfo->light_native(c2, lumel);

    } // i of new_tex
  } // j of new_tex

  return new_tex;
}

l3d_texture_data *l3d_surface_cache::
find_texdata(l3d_texture_data *texdata)
{

  // see if texture is already in cache
  // a linear search is so inelegant

  int i;

  for(i=0; i<surfaces->num_items; i++) {
    if((*surfaces)[i].surface->tex_data == texdata) { // we already have the tex in
      // the cache (having presumably performed, externally, glBindTexture
      // upon initial entry into the cache

      return (*surfaces)[i].surface->tex_data;
    }
  }

  l3d_texture *new_tex;
  l3d_surface *new_surface;

  new_tex = new l3d_texture;

  int next_surface_index = surfaces->next_index();
  (*surfaces)[next_surface_index].polygon = NULL;
  (*surfaces)[next_surface_index].surface = new_tex;

  new_tex->tex_data = texdata;
  new_tex->owns_tex_data = false;  // IMPORTANT! we do NOT own this texdata,
  // so when this surface is deleted (when the cache is reset for instance)
  // we cannot delete this tex_data, since it belongs to the polygon.

  return NULL;
}
