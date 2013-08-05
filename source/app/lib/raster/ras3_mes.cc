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

#include "ras3_mes.h"
#include <math.h>
#include <string.h>
#include "../tool_os/memman.h"

static const GLubyte period[] =
  {0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const GLubyte space[] =
  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

static const GLubyte digits[][13] = {
  {0x0,0x0, 0x18,0x3C,0x66,0xC3,0xC3,0xC3,0xC3,0xC3,0x66,0x3C,0x18},
  {0x0,0x0, 0x7F,0x7F,0xC,0xC,0xC,0xC,0xC,0xC,0x6C,0x3C,0xC},
  {0x0,0x0, 0xFF,0xFF,0xC0,0x60,0x30,0x18,0xC,0x6,0xC6,0x66,0x3C},
  {0x0,0x0, 0x7E,0xC3,0x3,0x6,0xC,0x38,0xC,0x6,0x3,0xC3,0x7E},
  {0x0,0x0, 0x6,0x6,0x6,0x6,0xFF,0xFF,0xC6,0xC6,0x66,0x36,0x1E},
  {0x0,0x0, 0x7E,0xFF,0xC3,0x3,0x3,0xFF,0xFE,0xC0,0xC0,0xC0,0xFE},
  {0x0,0x0, 0x7E,0xFF,0xC3,0xC3,0xC3,0xFF,0xFE,0xC0,0xC0,0xC0,0x7E},
  {0x0,0x0, 0x60,0x60,0x60,0x60,0x30,0x18,0xC,0x6,0x3,0x3,0xFF},
  {0x0,0x0, 0x7E,0xFF,0xC3,0xC3,0xC3,0x7E,0xC3,0xC3,0xC3,0xC3,0x7E},
  {0x0,0x0, 0x7E,0xFF,0xC3,0x3,0x3,0x7F,0xC3,0xC3,0xC3,0xC3,0x7E}
};


static const GLubyte letters[][13] = {
  {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18},
  {0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
  {0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
  {0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce, 0xfc},
  {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
  {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xff},
  {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
  {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
  {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e},
  {0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06},
  {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3},
  {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
  {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7, 0xc3},
  {0x00, 0x00, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3},
  {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e},
  {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
  {0x00, 0x00, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c},
  {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
  {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7, 0x7e},
  {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff},
  {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
  {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
  {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
  {0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
  {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
  {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff}
};


///////////////////////////////////////////////////////////////////////
l3d_rasterizer_3d_imp * l3d_rasterizer_3d_mesa_imp_factory::create
(int xs, int ys, l3d_screen_info *si)
{
  return new l3d_rasterizer_3d_mesa_imp(xs,ys,si);
}

l3d_rasterizer_3d_mesa_imp::
l3d_rasterizer_3d_mesa_imp(int xs, int ys, l3d_screen_info *si):
    l3d_rasterizer_3d_imp(xs,ys,si),
    l3d_rasterizer_2d_imp(xs,ys,si),
    l3d_rasterizer_2d_mesa_imp(xs,ys,si)
{
  make_raster_font();
  set_text_color(si->ext_max_red,
                 si->ext_max_green,
                 si->ext_max_blue);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //sbegin center
  gluOrtho2D(-xsize/2,xsize/2,-ysize/2,ysize/2);
  //send center
  //    gluOrtho2D(0,xsize,ysize,0);
  //    glMatrixMode(GL_MODELVIEW);
  //    glLoadIdentity();

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  //glViewport(-xsize/2,ysize/2,xsize,ysize);
  glViewport(0,0,xsize,ysize);

  //- prevent back-facing polygons - those which have a counterclockwise
  //- vertex orientation - from being drawn, to mimic the behavior of the
  //- software rasterizer.
  glFrontFace(GL_CW);
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
}


void l3d_rasterizer_3d_mesa_imp::clear_buffer(void)
{
  l3d_rasterizer_2d_mesa_imp::clear_buffer();
}


void l3d_rasterizer_3d_mesa_imp::draw_point(int x, int y, unsigned long col) {
  glBegin(GL_POINTS);
  sinfo->set_color(col);
  glVertex2i(x-(xsize>>1),(ysize>>1)-y);
  glEnd();
}

void l3d_rasterizer_3d_mesa_imp::draw_line(int x0, int y0, int x1, int y1,
    unsigned long col)
{
  glBegin(GL_LINES);
  sinfo->set_color(col);
  glVertex2i(x0-(xsize>>1),(ysize>>1)-y0);
  glVertex2i(x1-(xsize>>1),(ysize>>1)-y1);
  glEnd();
}

void l3d_rasterizer_3d_mesa_imp::draw_polygon_flatshaded
(const l3d_polygon_2d_flatshaded *p_poly)
{
  int i;

  glBegin(GL_POLYGON);
  sinfo->set_color(p_poly->final_color);
  for(i=0; i<p_poly->clip_ivertices->num_items; i++) {
    glVertex2i(iceil((**(p_poly->vlist))
                     [(*p_poly->clip_ivertices)[i].ivertex].transformed.X_)

               - (xsize>>1),

               (ysize>>1) -
               iceil((**(p_poly->vlist))
                     [(*p_poly->clip_ivertices)[i].ivertex].transformed.Y_));
  }
  glEnd();
}

void l3d_rasterizer_3d_mesa_imp::texture_setup
(const l3d_polygon_3d_textured *p_poly)
{
  //     glMatrixMode(GL_PROJECTION);
  //     glLoadIdentity();
  //     gluOrtho2D(-xsize/2,xsize/2,-ysize/2,ysize/2);
  //     //    gluOrtho2D(0,xsize,ysize,0);
  // //    glMatrixMode(GL_MODELVIEW);
  // //    glLoadIdentity();

  // glMatrixMode(GL_MODELVIEW);
  // glLoadIdentity();
  // //glViewport(-xsize/2,ysize/2,xsize,ysize);
  // glViewport(0,0,xsize,ysize);

  //  O = p_poly->texture->O.transformed;
  //  U = p_poly->texture->U.transformed - O;
  //  V = p_poly->texture->V.transformed - O;


  if (!glIsTexture(p_poly->texture->tex_data->id)) {
    printf("generating texture\n");
    GLuint glid = (GLuint) (p_poly->texture->tex_data->id);
    glGenTextures(1, & glid );
    p_poly->texture->tex_data->id = glid; // changes type

    glBindTexture(GL_TEXTURE_2D, p_poly->texture->tex_data->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //    glEnable(GL_TEXTURE_GEN_S);
    //    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 p_poly->texture->tex_data->width,
                 p_poly->texture->tex_data->height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 p_poly->texture->tex_data->data);
  } else {
    glEnable(GL_TEXTURE_2D); //- texturing may have been turned off by another
    // - drawing operation (e.g. drawing text)

    glBindTexture(GL_TEXTURE_2D,p_poly->texture->tex_data->id);
  }


  float texx[4] = {0.0, 0.0, 1.0, 1.0 };
  float texy[4] = {0.0, 1.0, 1.0, 0.0 };

  //      gluBuild2DMipmaps(GL_TEXTURE_2D, 3,
  //      p_poly->texture->tex_data->width,
  //      p_poly->texture->tex_data->height,
  //      GL_RGB, GL_UNSIGNED_BYTE,
  //      p_poly->texture->tex_data->data);


}


void l3d_rasterizer_3d_mesa_imp::draw_polygon_textured
(const l3d_polygon_3d_textured *p_poly)

{
  texture_setup(p_poly);
  glBegin(GL_POLYGON);
  ogl_texturing_between_glbegin_end(p_poly);
  glEnd();
}



void l3d_rasterizer_3d_mesa_imp::ogl_texturing_between_glbegin_end
(const l3d_polygon_3d_textured *p_poly)
{
  int i;

  GLfloat plane[4];
  l3d_vector plane_normal;

  plane_normal = normalized(p_poly->sfcnormal.transformed
                            - p_poly->center.transformed);

  plane[0] = plane_normal.a[0];
  plane[1] = plane_normal.a[1];
  plane[2] = plane_normal.a[2];
  plane[3] = -plane[0] * p_poly->center.transformed.X_
             - plane[1] * p_poly->center.transformed.Y_
             - plane[2] * p_poly->center.transformed.Z_;

  for(i=0; i<p_poly->clip_ivertices->num_items; i++) {

  #define SCREEN_X ((**(p_poly->vlist)) [(*(p_poly->clip_ivertices))[i].ivertex].transformed.X_)
  #define SCREEN_Y ((**(p_poly->vlist)) [(*(p_poly->clip_ivertices))[i].ivertex].transformed.Y_)

    assert(fovx && screen_xsize && fovy && screen_ysize);

    l3d_real hf=l3d_mulri(*fovx,*screen_xsize),
                vf=l3d_mulri(*fovy,*screen_ysize);
    float orig_z_denom =
      (plane[0]*(SCREEN_X - xsize/2)/ hf -
       plane[1]*(SCREEN_Y - ysize/2)/ vf +  // note: this vf is positive not negative (unlike the vf in the texture_computer compute_us)
       plane[2]);
    if(orig_z_denom > -0.0001) {

      //- this would imply that the orig_z value is < 0.0001 ie non-positive
      //- so we cancel the entire polygon drawing operation
      return;
    }
  }


  for(i=0; i<p_poly->clip_ivertices->num_items; i++) {

#define SCREEN_X ((**(p_poly->vlist)) [(*(p_poly->clip_ivertices))[i].ivertex].transformed.X_)
#define SCREEN_Y ((**(p_poly->vlist)) [(*(p_poly->clip_ivertices))[i].ivertex].transformed.Y_)

    glTexCoord2f
    (
      ((l3d_polygon_ivertex_textured *)&(*p_poly->clip_ivertices)[i])
      ->tex_coord.X_,
      ((l3d_polygon_ivertex_textured *)&(*p_poly->clip_ivertices)[i])
      ->tex_coord.Y_
    );

    assert(fovx && screen_xsize && fovy && screen_ysize);
    l3d_real hf=l3d_mulri(*fovx,*screen_xsize),
                vf=l3d_mulri(*fovy,*screen_ysize);
    float orig_z = -plane[3] /
                   (plane[0]*(SCREEN_X - xsize/2)/ hf -
                    plane[1]*(SCREEN_Y - ysize/2)/ vf +  // note: this vf is positive not negative (unlike the vf in the texture_computer compute_us)
                    plane[2]);

    GLfloat tmp[4];

    tmp[0] =
      (((**(p_poly->vlist))
        [(*(p_poly->clip_ivertices))[i].ivertex].transformed.X_ - xsize/2)

       * orig_z );

    tmp[1] =
      ((-(**(p_poly->vlist))
        [(*(p_poly->clip_ivertices))[i].ivertex].transformed.Y_ + ysize/2)

       * orig_z );
    tmp[2] = 1;
    tmp[3] = orig_z;

    // 2d clipping generates new 2d points which never existed in 3d.
    // therefore these points do not exist as transformed_intermediates....
    //printf("%f,%f,%f,%f\n",tmp[0],tmp[1],tmp[2],tmp[3]);
    glVertex4fv(tmp);
  }
}

void l3d_rasterizer_3d_mesa_imp::make_raster_font(void)
{
  GLuint i, j;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  fontOffset = glGenLists (128);
  for (i = 0,j = 'A'; i < 26; i++,j++) {
    glNewList(fontOffset + j, GL_COMPILE);
    glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0,
             letters[i]);
    glEndList();
  }

  for (i = 0,j = 'a'; i < 26; i++,j++) {
    glNewList(fontOffset + j, GL_COMPILE);
    glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0,
             letters[i]);
    glEndList();
  }

  for (i = 0,j = '0'; i < 10; i++,j++) {
    glNewList(fontOffset + j, GL_COMPILE);
    glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0,
             digits[i]);
    glEndList();
  }

  glNewList(fontOffset + ' ', GL_COMPILE);
  glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0,
           space);
  glEndList();

  glNewList(fontOffset + '.', GL_COMPILE);
  glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0,
           period);
  glEndList();


}

void l3d_rasterizer_3d_mesa_imp::draw_text(int x, int y, const char *string) {
  sinfo->set_color(text_color);
  glDisable(GL_TEXTURE_2D); //- text should not be displayed using a texture
  glRasterPos2i(x-(xsize>>1),(ysize>>1)-y - 13); //- 13 is height of 1 char

  glPushAttrib (GL_LIST_BIT);
  glListBase(fontOffset);
  glCallLists((GLsizei) strlen(string), GL_UNSIGNED_BYTE, (GLubyte *) string);
  glPopAttrib ();

}

void l3d_rasterizer_3d_mesa_imp::set_text_color(int red, int green, int blue) {
  text_color = sinfo->ext_to_native(red, green, blue);
}

void l3d_rasterizer_3d_mesa_imp::draw_polygon_textured_lightmapped
(const l3d_polygon_3d_textured_lightmapped *p_poly)
{
  GLuint glid = (GLuint) (p_poly->texture->tex_data->id);

  l3d_list<l3d_point> old_texcoords(10);

  //- try to find texture data in surface cache
  if (!p_poly->get_scache()->find_texdata(p_poly->texture->tex_data)) {
    // printf("generating texture\n");

    if (glIsTexture(p_poly->texture->tex_data->id)) {
      // the texture might have been bound but later freed out of the
      // surface cache. if this is the case the old texture could still
      // be lying around consuming texture resources. so, we free the
      // old texture if it is still around, since we are about to rebind
      // it anyway.

      // printf("deleting texture %d\n", p_poly->texture->tex_data->id);
      glDeleteTextures(1, &glid);
    }

    glGenTextures(1, & glid );
    p_poly->texture->tex_data->id = glid;// changes type

    glBindTexture(GL_TEXTURE_2D, p_poly->texture->tex_data->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    //    glEnable(GL_TEXTURE_GEN_S);
    //    glEnable(GL_TEXTURE_GEN_T);
    glEnable(GL_TEXTURE_2D);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 p_poly->texture->tex_data->width,
                 p_poly->texture->tex_data->height,
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 p_poly->texture->tex_data->data);
  } else {
    glBindTexture(GL_TEXTURE_2D,p_poly->texture->tex_data->id);
  }

  // having read in a lightmapped polygon in w_porltex.cc, we have
  // already called compute_surface_orientation_and_size, which has
  // also already re-assigned the tex coordinates based on the larger
  // pre-tiled and pre-lit surface size. these coords are WRONG for
  // mesa rasterization, since we do not use a suface, but instead use
  // two passes, each time with dift tex coords. once with the orig tex
  // coords, and once with the tex coords for the lightmap. one solution
  // would have been to introduce TWO texture coordinates into the ivertex
  // class, and to modify all 2d clipping routines to go through an external
  // virtual clipper, which would clip geometry and possibly tex coords
  // and possibly lumel coords if mesa rasterization. this would require
  // two concrete clipper classes and a new factory and a new factory
  // manager.

  // the other solution is to forget about storing the lumel tex coords
  // with the ivertices, in fact, to forget about the ivertices entirely.
  // instead, we use the original OUV vectors and the screen space coords
  // and do an explicit screen-to-tex space computation for each
  // of the post-2d-clipping screen space vertices. then we know the
  // proper tex coords for each of the clip_ivertices, can assign
  // the u,v,and z coords to the ivertex.tex_coord, then can call the
  // usual draw_polygon_textured routine. we perform this explicit
  // texture-coord computation twice: once for the tex coords with the
  // texture space OUV, and a second time for the lumel coords with the
  // lightmap space OUV.

  // so, here: compute texturemap uv's for each clip_ivertex
  // we assume if we are using mesa, then we are on a machine which
  // can do fast floating point math; thus the computations below are
  // all done explicitly in floats instead of l3d_reals

  //- based on texture orientation: compute uvz's for each vertex
  //sbegin newuv_loop
  for(int i=0; i<p_poly->clip_ivertices->num_items; i++) {
    //send newuv_loop
    assert(fovx && screen_xsize && fovy && screen_ysize);
    //sbegin newuv_px_from_sx
    float px =
      ( (**p_poly->vlist) [(*p_poly->clip_ivertices)[i].ivertex].transformed.X_
        - *screen_xsize/2 )
      / (*screen_xsize * *fovx);

    float py =
      (*screen_ysize/2
       -
       (**p_poly->vlist) [(*p_poly->clip_ivertices)[i].ivertex].transformed.Y_ )
      /
      (*screen_ysize * *fovy);
    //send newuv_px_from_sx

    //sbegin newuv_doit
    float ox = p_poly->texture->O.transformed.X_;
    float oy = p_poly->texture->O.transformed.Y_;
    float oz = p_poly->texture->O.transformed.Z_;
    float ux = p_poly->texture->U.transformed.X_ - ox;
    float uy = p_poly->texture->U.transformed.Y_ - oy;
    float uz = p_poly->texture->U.transformed.Z_ - oz;
    float vx = p_poly->texture->V.transformed.X_ - ox;
    float vy = p_poly->texture->V.transformed.Y_ - oy;
    float vz = p_poly->texture->V.transformed.Z_ - oz;

    float u = (px*(oy*vz - oz*vy) + py*(oz*vx - ox*vz) - oy*vx + ox*vy)
              / (px*(uz*vy - uy*vz) + py*(ux*vz - uz*vx) + uy*vx - ux*vy);
    float v = (px*(oy*uz - oz*uy) + py*(oz*ux - ox*uz) - oy*ux + ox*uy)
              / ((uy*vz - uz*vy)*px + py*(uz*vx - ux*vz) + ux*vy - uy*vx);
    float z = (ox*(uz*vy - uy*vz) + oy*(ux*vz - uz*vx) + oz*(uy*vx - ux*vy))
              / (px*(uz*vy - uy*vz) + py*(ux*vz - uz*vx) + uy*vx - ux*vy);
    //send newuv_doit

    //sbegin newuv_assign
    old_texcoords[old_texcoords.next_index()] =
      ((l3d_polygon_ivertex_textured *)&(*p_poly->clip_ivertices)[i])
      ->tex_coord;

    ((l3d_polygon_ivertex_textured *)&(*p_poly->clip_ivertices)[i])
    ->tex_coord.set(float_to_l3d_real(u),
                    float_to_l3d_real(v),
                    float_to_l3d_real(z),
                    float_to_l3d_real(1.0));
    //send newuv_assign
  }

  draw_polygon_textured(p_poly);

  //sbegin texblend
  //- set up mag/min filters to make light maps smoother
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //- set up blend function to combine light map and text map
  glEnable(GL_BLEND);
  glBlendFunc(GL_ZERO, GL_SRC_ALPHA);

  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  //send texblend

  int i;

  //- try to find light map data in cache
  glid = (GLuint) (p_poly->lightmap->tex_data->id);
  if (!p_poly->get_scache()->find_texdata(p_poly->lightmap->tex_data)) {
    if (glIsTexture(p_poly->lightmap->tex_data->id)) {
      // the texture might have been bound but later freed out of the
      // surface cache. if this is the case the old texture could still
      // be lying around consuming texture resources. so, we free the
      // old texture if it is still around, since we are about to rebind
      // it anyway.

      // printf("deleting texture %d\n", p_poly->lightmap->tex_data->id);
      glDeleteTextures(1, &glid);
    }
    printf("generating texture\n");
    glGenTextures(1, & glid );
    p_poly->lightmap->tex_data->id = glid;
    printf("ID was %d\n", p_poly->lightmap->tex_data->id);

    glBindTexture(GL_TEXTURE_2D, p_poly->lightmap->tex_data->id);

    // GL_CLAMP (not GL_REPEAT) for lightmap data, otherwise bleeding
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_TEXTURE_2D);

    glPixelStorei(GL_UNPACK_ALIGNMENT,1);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 p_poly->lightmap->tex_data->width,
                 p_poly->lightmap->tex_data->height,
                 0,
                 GL_ALPHA,
                 GL_UNSIGNED_BYTE,
                 p_poly->lightmap->tex_data->data);
  } else {
    glBindTexture(GL_TEXTURE_2D,p_poly->lightmap->tex_data->id);
  }

  //- restore lumel-space uv's for each clip_ivertex

  for(int i=0; i<p_poly->clip_ivertices->num_items; i++) {
    ((l3d_polygon_ivertex_textured *)&(*p_poly->clip_ivertices)[i])
    ->tex_coord = old_texcoords[i];
  }



  //- draw the same polygon again, but this time with the light map
  glBegin(GL_POLYGON);
  ogl_texturing_between_glbegin_end(p_poly);
  glEnd();

  glDisable(GL_BLEND);

}
