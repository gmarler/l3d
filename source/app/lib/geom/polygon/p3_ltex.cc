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

#include "p3_ltex.h"

#include "../../raster/ras3_sw.h"
#include "../../tool_os/memman.h"

#define EPSILON_VECTOR 0.001

l3d_polygon_3d_textured_lightmapped::l3d_polygon_3d_textured_lightmapped
(int num_pts, l3d_surface_cache *scache):
    l3d_polygon_2d(),
    l3d_polygon_3d(),
    l3d_polygon_3d_clippable(),
    l3d_polygon_3d_textured(num_pts)
{
  this->scache = scache;
  lightmap = new l3d_texture;
  lightmap->tex_data = new l3d_texture_data;
  lightmap->owns_tex_data = true;
  lightmap->tex_data->width = 64;
  lightmap->tex_data->height = 64;
  lightmap->tex_data->data = new unsigned char[lightmap->tex_data->width * lightmap->tex_data->height];

  // test lightmap data
  //  char lmdata[32][32] =
  //  {
  //    {00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00},
  //    {00,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11},
  //    {00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,11,00,00,00,00,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,11,00,00,00,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,11,00,00,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,11,00,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,11,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,11,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,11,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,11,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,11,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,11,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,11,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,11,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,11,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,11,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,11,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,11,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,11,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //    {00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,00,11},
  //  };

  int i,j;
  for(j=0;j<lightmap->tex_data->height;j++) {
    for(i=0;i<lightmap->tex_data->width;i++) {
      //lightmap->tex_data->data[i + j*lightmap->tex_data->width] = lmdata[j][i];
      //      lightmap->tex_data->data[i + j*lightmap->tex_data->width] = i/2;
      lightmap->tex_data->data[i + j*lightmap->tex_data->width] = MAX_LIGHT_LEVELS/4;
    }
  }

  surface = NULL; // the tiled, lit, to-be-displayed surface (not
  // to be confused with the lightmap itself, which is ONLY light
  // and no texture

}

void l3d_polygon_3d_textured_lightmapped::compute_surface_orientation_and_size
(void)
{

  O = texture->O.original;
  U = texture->U.original - O;
  V = texture->V.original - O;

  l3d_real minu,minv,maxu,maxv;

  int i= 0;

  l3d_point tex_point;
  tex_point =
    world_to_tex_matrix(*texture)
    |
    (**vlist) [(*ivertices)[i].ivertex].original;
  u = tex_point.X_ = l3d_divrr(tex_point.X_, tex_point.W_);
  v = tex_point.Y_ = l3d_divrr(tex_point.Y_, tex_point.W_);
  // dont care about z

  maxu = minu =  tex_point.X_;
  maxv = minv =  tex_point.Y_;
  //          printf("found from %f %f %f a NEW FANGLED X-Y u,v of %f,%f\n",
  //  	       (**vlist)[(*ivertices)[i].ivertex].original.X_,
  //  	       (**vlist)[(*ivertices)[i].ivertex].original.Y_,
  //  	       (**vlist)[(*ivertices)[i].ivertex].original.Z_,
  //  	       minu, minv);

  U = texture->U.original - texture->O.original;
  V = texture->V.original - texture->O.original;

  //  printf("OK folks:\n");
  //  printf("O appears to be %f %f %f %f\n", O.X_, O.Y_, O.Z_, O.W_);
  //  printf("U appears to be %f %f %f %f\n", U.X_, U.Y_, U.Z_, U.W_);
  //  printf("V appears to be %f %f %f %f\n", V.X_, V.Y_, V.Z_, V.W_);



#define VTX(i) ((**(vlist))[ (*(clip_ivertices))[i].ivertex ].transformed)


  for(i=1; i < ivertices->num_items; i++) {

    l3d_real u,v;

    //      /*
    //  printf("point P appears to be %f %f %f %f\n",
    //  (**vlist) [i].transformed_intermediates[1].X_,
    //  (**vlist) [i].transformed_intermediates[1].Y_,
    //  (**vlist) [i].transformed_intermediates[1].Z_,
    //  (**vlist) [i].transformed_intermediates[1].W_);
    //  */


    tex_point =
      world_to_tex_matrix(*texture)
      |
      (**vlist) [(*ivertices)[i].ivertex].original;
    u = tex_point.X_ = l3d_divrr(tex_point.X_, tex_point.W_);
    v = tex_point.Y_ = l3d_divrr(tex_point.Y_, tex_point.W_);
    // dont care about z

    //          printf("found from %f %f %f a NEW FANGLED X-Y u,v of %f,%f\n",
    //  	       (**vlist) [(*ivertices)[i].ivertex].original.X_,
    //  	       (**vlist)[(*ivertices)[i].ivertex].original.Y_,
    //  	       (**vlist)[(*ivertices)[i].ivertex].original.Z_,
    //   u, v);




    if(u<minu) minu=u;
    if(u>maxu) maxu=u;
    if(v<minv) minv=v;
    if(v>maxv) maxv=v;
  }
  //printf("end poly\n");

  // we now have the minu and maxu of the polygon in texture space
  // (ASSUMING that the O,U,and V all lie within the plane of the polygon -
  // otherwise all bets are off, since then the seen texels depend on the
  // camera position)

  int iminu, iminv, imaxu, imaxv;

  // snap bounding rectangle to an integer power of 2, so that we have a pow2
  // number of texture-map repetitions(tiles) in the u and v directions;
  // this means if the original texmap dimensions were powers of 2 (they
  // better be) then the new tiled, larger map is also pow2.

  iminu = ifloor(minu);
  iminv = ifloor(minv);
  imaxu = iceil(maxu);
  imaxv = iceil(maxv);


  int u_tilecount = imaxu - iminu;
  int v_tilecount = imaxv - iminv;

  //  it is necessary to round the surface up to powers of two, because
  //  with s/w, the textured lightmapped polygons are drawn WITH THE SAME
  //  rasterizer routine as with textured. this means that the & with the
  //  texture size-1 is always done. even though since a surface is
  //  pre-tiled, and even though it will *NEVER* be wrapped around by
  //  the rasterizer, the rasterizer still applies a logical and
  //  (since the rasterizer doesn't know that the incoming textured polygon
  //  won't be wrapped), so we must round the sfc size up to a power of two
  //  to use the same, unchanged, wrappable texture mapping routine as before.

  int u_tilecount_pow2 = 1 << iceil(float_to_l3d_real(log(u_tilecount) / log(2)));
  int v_tilecount_pow2 = 1 << iceil(float_to_l3d_real(log(v_tilecount) / log(2)));
  imaxu += u_tilecount_pow2 - u_tilecount;
  imaxv += v_tilecount_pow2 - v_tilecount;


  surface_xsize = texture->tex_data->width * (imaxu - iminu);
  surface_ysize = texture->tex_data->height * (imaxv - iminv);

  //    printf("tiled surface size is %d by %d, hope these are powers of 2...\n",
  //  	 surface_xsize, surface_ysize);

  // calc. the actual used area in texture space; only this area will be lit.
  // the rest of the tex space is not lit and is invisible, wasted space.
  // XX it is wasted so that the texture size may be a power of two so that
  // XX the texture coordinate computation can be done with a bitmask instead
  // XX of a modulo operation.
  // ^^ not true, no power of 2 restriction because surfaces not tiled,
  //    and the light map is stretched in its entirety over the texture map.
  //    leads to no wasted light map space, but to inconsistent light map
  //    resolution across all polys.

  //  printf("minu %f maxu %f minv %f maxv %f\n",
  //         minu, maxu, minv, maxv);

  //    surface_used_u0 = l3d_real_to_int(l3d_mulri(minu, texture->tex_data->width))
  //      - iminu*texture->tex_data->width;
  //    surface_used_v0 = l3d_real_to_int(l3d_mulri(minv, texture->tex_data->height))
  //      - iminv*texture->tex_data->height;
  //    surface_used_u1 = l3d_real_to_int(l3d_mulri(maxu, texture->tex_data->width))
  //      - iminu*texture->tex_data->width;
  //    surface_used_v1 = l3d_real_to_int(l3d_mulri(maxv, texture->tex_data->height))
  //      - iminv*texture->tex_data->height;




  // based upon the rounded bounding box, compute the world-space
  // O, U, and V orientations
  surface_orientation.O.original.set(O.X_ + U.X_*iminu + V.X_*iminv,
                                     O.Y_ + U.Y_*iminu + V.Y_*iminv,
                                     O.Z_ + U.Z_*iminu + V.Z_*iminv,
                                     int_to_l3d_real(1));
  surface_orientation.U.original.set(O.X_ + U.X_*imaxu + V.X_*iminv,
                                     O.Y_ + U.Y_*imaxu + V.Y_*iminv,
                                     O.Z_ + U.Z_*imaxu + V.Z_*iminv,
                                     int_to_l3d_real(1));
  surface_orientation.V.original.set(O.X_ + U.X_*iminu + V.X_*imaxv,
                                     O.Y_ + U.Y_*iminu + V.Y_*imaxv,
                                     O.Z_ + U.Z_*iminu + V.Z_*imaxv,
                                     int_to_l3d_real(1));

  // and compute, most importantly, the new u,v coords

  //    /*
  //    printf("new ouv ");
  //      surface_orientation.O.original = O;
  //      surface_orientation.U.original = O + U;
  //      surface_orientation.V.original = O + V;
  //    */

  //  surface_orientation.O.original.print();
  //  surface_orientation.U.original.print();
  //  surface_orientation.V.original.print();

  assign_tex_coords_from_tex_space(surface_orientation);
}

//  void l3d_polygon_3d_textured_lightmapped::
//  compute_surface_size_from_existing_uv_coords
//  (void)
//  {
//    l3d_real minu,minv,maxu,maxv;

//    int i= 0;

//    l3d_point tex_point;

//    O = texture->O.original;
//    U = texture->U.original - O;
//    V = texture->V.original - O;

//    assert( dynamic_cast <l3d_polygon_ivertex_textured *>
//            ( & ((*ivertices)[i]) ) );
//    tex_point =
//      ((l3d_polygon_ivertex_textured *)
//       ( & ((*ivertices)[i]))) -> tex_coord;
//    u = tex_point.X_ = l3d_divrr(tex_point.X_, tex_point.W_);
//    v = tex_point.Y_ = l3d_divrr(tex_point.Y_, tex_point.W_);
//    // dont care about z

//    maxu = minu =  tex_point.X_;
//    maxv = minv =  tex_point.Y_;
//    //          printf("found from %f %f %f a NEW FANGLED X-Y u,v of %f,%f\n",
//    //  	       (**vlist)[(*ivertices)[i].ivertex].original.X_,
//    //  	       (**vlist)[(*ivertices)[i].ivertex].original.Y_,
//    //  	       (**vlist)[(*ivertices)[i].ivertex].original.Z_,
//    //  	       minu, minv);

//  #define VTX(i) ((**(vlist))[ (*(clip_ivertices))[i].ivertex ].transformed)


//    for(i=1; i < ivertices->num_items; i++) {

//      l3d_real u,v;

//      //      /*
//      //  printf("point P appears to be %f %f %f %f\n",
//      //  (**vlist) [i].transformed_intermediates[1].X_,
//      //  (**vlist) [i].transformed_intermediates[1].Y_,
//      //  (**vlist) [i].transformed_intermediates[1].Z_,
//      //  (**vlist) [i].transformed_intermediates[1].W_);
//      //  */


//      assert( dynamic_cast <l3d_polygon_ivertex_textured *>
//              ( & ((*ivertices)[i]) ) );
//      tex_point =
//        ((l3d_polygon_ivertex_textured *)
//         ( & ((*ivertices)[i]))) -> tex_coord;

//      u = tex_point.X_ = l3d_divrr(tex_point.X_, tex_point.W_);
//      v = tex_point.Y_ = l3d_divrr(tex_point.Y_, tex_point.W_);
//      // dont care about z

//      //          printf("found from %f %f %f a NEW FANGLED X-Y u,v of %f,%f\n",
//      //  	       (**vlist) [(*ivertices)[i].ivertex].original.X_,
//      //  	       (**vlist)[(*ivertices)[i].ivertex].original.Y_,
//      //  	       (**vlist)[(*ivertices)[i].ivertex].original.Z_,
//      //   u, v);


//      if(u<minu) minu=u;
//      if(u>maxu) maxu=u;
//      if(v<minv) minv=v;
//      if(v>maxv) maxv=v;
//    }
//    //printf("end poly\n");

//    // we now have the minu and maxu of the polygon in texture space
//    // (ASSUMING that the O,U,and V all lie within the plane of the polygon -
//    // otherwise all bets are off, since then the seen texels depend on the
//    // camera position)

//    int iminu, iminv, imaxu, imaxv;

//    // snap bounding rectangle to an integer power of 2, so that we have a pow2
//    // number of texture-map repetitions(tiles) in the u and v directions;
//    // this means if the original texmap dimensions were powers of 2 (they
//    // better be) then the new tiled, larger map is also pow2.

//    iminu = ifloor(minu);
//    iminv = ifloor(minv);
//    imaxu = iceil(maxu);
//    imaxv = iceil(maxv);


//    int u_tilecount = imaxu - iminu;
//    int v_tilecount = imaxv - iminv;

//    //  it is unnecessary to round the surface up to powers of two, because
//    //  the power of two restriction is only there to make wraparound
//    //  textures easy. since a surface is pre-tiled, it will *NEVER* be
//    //  wrapped around by the rasterizer, thus we don't need to round up to
//    //  power of two here.

//    //  int u_tilecount_pow2 = 1 << iceil(float_to_l3d_real(log(u_tilecount) / log(2)));
//    //  int v_tilecount_pow2 = 1 << iceil(float_to_l3d_real(log(v_tilecount) / log(2)));
//    //  imaxu += u_tilecount_pow2 - u_tilecount;
//    //  imaxv += v_tilecount_pow2 - v_tilecount;


//    surface_xsize = texture->tex_data->width * (imaxu - iminu);
//    surface_ysize = texture->tex_data->height * (imaxv - iminv);

//    //    printf("tiled surface size is %d by %d, hope these are powers of 2...\n",
//    //  	 surface_xsize, surface_ysize);

//    // calc. the actual used area in texture space; only this area will be lit.
//    // the rest of the tex space is not lit and is invisible, wasted space.
//    // XX it is wasted so that the texture size may be a power of two so that
//    // XX the texture coordinate computation can be done with a bitmask instead
//    // XX of a modulo operation.
//    // ^^ not true, no power of 2 restriction because surfaces not tiled,
//    //    and the light map is stretched in its entirety over the texture map.
//    //    leads to no wasted light map space, but to inconsistent light map
//    //    resolution across all polys.

//    //  printf("minu %f maxu %f minv %f maxv %f\n",
//    //         minu, maxu, minv, maxv);

//    //    surface_used_u0 = l3d_real_to_int(l3d_mulri(minu, texture->tex_data->width))
//    //      - iminu*texture->tex_data->width;
//    //    surface_used_v0 = l3d_real_to_int(l3d_mulri(minv, texture->tex_data->height))
//    //      - iminv*texture->tex_data->height;
//    //    surface_used_u1 = l3d_real_to_int(l3d_mulri(maxu, texture->tex_data->width))
//    //      - iminu*texture->tex_data->width;
//    //    surface_used_v1 = l3d_real_to_int(l3d_mulri(maxv, texture->tex_data->height))
//    //      - iminv*texture->tex_data->height;



//    // based upon the rounded bounding box, compute the world-space
//    // O, U, and V orientations
//    surface_orientation.O.original.set(O.X_ + U.X_*iminu + V.X_*iminv,
//  				     O.Y_ + U.Y_*iminu + V.Y_*iminv,
//  				     O.Z_ + U.Z_*iminu + V.Z_*iminv,
//  				     int_to_l3d_real(1));
//    surface_orientation.U.original.set(O.X_ + U.X_*imaxu + V.X_*iminv,
//  				     O.Y_ + U.Y_*imaxu + V.Y_*iminv,
//  				     O.Z_ + U.Z_*imaxu + V.Z_*iminv,
//  				     int_to_l3d_real(1));
//    surface_orientation.V.original.set(O.X_ + U.X_*iminu + V.X_*imaxv,
//  				     O.Y_ + U.Y_*iminu + V.Y_*imaxv,
//  				     O.Z_ + U.Z_*iminu + V.Z_*imaxv,
//  				     int_to_l3d_real(1));

//    // based upon the rounded bounding box, compute the new texture coords
//    // since these texture coords were manually assigned before (as opposed
//    // to being procedurally computed via an O,U,V texture space), we don't
//    // use a matrix to compute the new coords, but instead simply scale the
//    // new coords down to fit within the larger tiled surface.

//    for(i=0; i < ivertices->num_items; i++) {
//      l3d_real u,v;

//      assert( dynamic_cast <l3d_polygon_ivertex_textured *>
//              ( & ((*ivertices)[i]) ) );
//      tex_point =
//        ((l3d_polygon_ivertex_textured *)
//         ( & ((*ivertices)[i]))) -> tex_coord;

//      u = tex_point.X_ = l3d_divrr(tex_point.X_, tex_point.W_);
//      v = tex_point.Y_ = l3d_divrr(tex_point.Y_, tex_point.W_);
//      // dont care about z

//      u = u / float(u_tilecount);
//      v = v / float(v_tilecount);

//      ((l3d_polygon_ivertex_textured *)
//       ( & ((*ivertices)[i]))) -> tex_coord.set
//        (float_to_l3d_real(u),
//         float_to_l3d_real(v),
//         int_to_l3d_real(1),
//         int_to_l3d_real(1));

//    }
//  }


void l3d_polygon_3d_textured_lightmapped::init_transformed(void) {
  l3d_polygon_3d_textured::init_transformed();

  surface_orientation.O.transformed = surface_orientation.O.original;
  surface_orientation.U.transformed = surface_orientation.U.original;
  surface_orientation.V.transformed = surface_orientation.V.original;

}


void l3d_polygon_3d_textured_lightmapped::transform(const l3d_matrix &m, int count) {
  l3d_polygon_3d_textured::transform(m,count);

  surface_orientation.O.transformed = m | surface_orientation.O.transformed;
  surface_orientation.U.transformed = m | surface_orientation.U.transformed;
  surface_orientation.V.transformed = m | surface_orientation.V.transformed;

}

l3d_polygon_2d* l3d_polygon_3d_textured_lightmapped::clone(void) {
  return new l3d_polygon_3d_textured_lightmapped(*this);
}

l3d_polygon_3d_textured_lightmapped::l3d_polygon_3d_textured_lightmapped
(const l3d_polygon_3d_textured_lightmapped &r)
    : l3d_polygon_2d(r),
    l3d_polygon_3d(r),
    l3d_polygon_3d_textured(r),
    l3d_polygon_3d_clippable(r),
    l3d_texture_computer(r)
{
  scache = r.scache;
  surface = r.surface;
  surface_orientation = r.surface_orientation;
  surface_xsize = r.surface_xsize;
  surface_ysize = r.surface_ysize;
  //    surface_used_u0 = r.surface_used_u0;
  //    surface_used_v0 = r.surface_used_v0;
  //    surface_used_u1 = r.surface_used_u1;
  //    surface_used_v1 = r.surface_used_v1;
  lightmap = r.lightmap;
}
