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

#include "texture.h"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int Ttexture::find_or_make_pal_idx(int r, int g, int b) {
  int i;

  for(i=0; i<_plocal_palette->size; i++) {
    if(    _plocal_palette->entries[i].r == r
           && _plocal_palette->entries[i].g == g
           && _plocal_palette->entries[i].b == b )
    {
      return i;
    }
  }

  if ( i >= _plocal_palette->size ) {  // no existing match found
    if (i >= MAX_PALSIZE) {
      fprintf(stderr,"palette overflow, colors will be lost\n");
      return MAX_PALSIZE-1;  // arbitrary return value
    }

    //printf("allocating new color %d %d %d\n",r,g,b);

    _plocal_palette->entries[_plocal_palette->size].r = r;
    _plocal_palette->entries[_plocal_palette->size].g = g;
    _plocal_palette->entries[_plocal_palette->size].b = b;
    _plocal_palette->size++;

    return _plocal_palette->size - 1;
  }
}

int Ttexture::process_tok(char *tok) {
  switch(get_read_state()) {
    case 0: {
        if(strcmp(tok,"P3")==0) {
          set_read_state(1);
          return 1;
        }
        else if (strcmp(tok,"P6")==0) {
          set_read_state(10);
          return 1;
        }
        else return 0;
      }

    case 1: {
        int width=0;
        sscanf(tok,"%d",&width);
        set_width(width);
        set_read_state(2);
        return 1;
      }

    case 2: {
        int height=0;
        sscanf(tok,"%d",&height);
        set_height(height);
        set_ptexdata((unsigned char *)
                     malloc( sizeof(unsigned char)*_width*_height ));

        _cur_tex_x = _cur_tex_y = 0;

        set_read_state(3);
        return 1;
      }

    case 3: {
        int maxcol=0;
        sscanf(tok,"%d",&maxcol);
        if(maxcol!=255) {
          fprintf(stderr,"warning: color resolution per r,g,b != 255\n");
        }
        set_read_state(4);
        return 1;
      }

    case 4: {
        int red=0;
        sscanf(tok,"%d",&red);
        _cur_r=red;
        set_read_state(5);
        return 1;
      }

    case 5: {
        int green=0;
        sscanf(tok,"%d",&green);
        _cur_g=green;
        set_read_state(6);
        return 1;
      }

    case 6: {
        int blue=0;
        sscanf(tok,"%d",&blue);
        _cur_b=blue;

        //printf("got ASC %d %d %d\n",_cur_r,_cur_g,_cur_b);
        *(_ptexdata + _cur_tex_x + _cur_tex_y*get_width()) =
          find_or_make_pal_idx(_cur_r, _cur_g, _cur_b);

        _cur_tex_x++;
        if(_cur_tex_x >= get_width()) {
          _cur_tex_x = 0;
          _cur_tex_y++;
        }

        set_read_state(4);
        return 1;
      }

    case 10: {
        int width=0;
        sscanf(tok,"%d",&width);
        set_width(width);
        set_read_state(11);
        return 1;
      }

    case 11: {
        int height=0;
        sscanf(tok,"%d",&height);
        set_height(height);
        set_ptexdata((unsigned char *)
                     malloc( sizeof(unsigned char)*_width*_height ));

        _cur_tex_x = _cur_tex_y = 0;

        set_read_state(12);
        return 1;
      }

    case 12: {
        int maxcol=0;
        sscanf(tok,"%d",&maxcol);
        if(maxcol!=255) {
          fprintf(stderr,"warning: color resolution per r,g,b != 255\n");
        }
        set_read_state(13);
        return 2;  // signal caller that we must start reading binary data now
      }

    case 13: {
        unsigned char red=0;
        sscanf(tok,"%c",&red);
        _cur_r=red;
        set_read_state(14);
        return 1;
      }

    case 14: {
        unsigned char green=0;
        sscanf(tok,"%c",&green);
        _cur_g=green;
        set_read_state(15);
        return 1;
      }

    case 15: {
        unsigned char blue=0;
        sscanf(tok,"%c",&blue);
        _cur_b=blue;

        //printf("got BIN %d %d %d\n",_cur_r,_cur_g,_cur_b);
        *(_ptexdata + _cur_tex_x + _cur_tex_y*get_width()) =
          find_or_make_pal_idx(_cur_r, _cur_g, _cur_b);

        _cur_tex_x++;
        if(_cur_tex_x >= get_width()) {
          _cur_tex_x = 0;
          _cur_tex_y++;
        }

        set_read_state(13);
        return 1;
      }

    default: fprintf(stderr,"unknown error parsing ppm file\n");
  } // case
}

void Ttexture::read_from_ppm(char *fname) {
  FILE *fp;
  char *tok;
  char s[256]="";
  int result;


  set_plocal_palette( new Tpalette );
  _plocal_palette->size = 0;

  fp=fopen(fname, "r");
  if(!fp) {
    fprintf(stderr,"Error opening texture ppm file %s\n", fname);
    return;
  }

  set_read_state(0);
  fgets(s, 256, fp);
  while (!feof(fp)) {
    tok = strtok(s," \t\n");
    while(tok) {
      if(tok[0]=='#') break;

      result = process_tok(tok);
      tok = strtok(NULL," \t\n");
    }
    if (result==2) break;
    fgets(s, 256, fp);
  }

  if(!feof(fp) && result==2) {  // P6 binary ppm file
    s[1] = 0;
    s[0] = fgetc(fp);
    while(!feof(fp)) {
      process_tok(s);
      s[0] = fgetc(fp);
    }
  }
}

Ttexture::Ttexture(void) {
}

Ttexture::Ttexture(int width, int height) {
}

Ttexture::~Ttexture(void) {
  if (_plocal_palette) free(_plocal_palette);
  if (_ptexdata) free(_ptexdata);
}

int Ttexture::find_nearest_pal_entry(Tpalette *src_pal,
                                     Tpalette *dest_pal, int src_i)
{
  int i;

  for(i=0; i<src_pal->size; i++) {
    if (    dest_pal->entries[i].r == src_pal->entries[src_i].r
            && dest_pal->entries[i].g == src_pal->entries[src_i].g
            && dest_pal->entries[i].b == src_pal->entries[src_i].b
       )
    {
      return i;
    }
  }

  printf("couldn't exactly match r %d g %d b %d\n",
         src_pal->entries[src_i].r,
         src_pal->entries[src_i].g,
         src_pal->entries[src_i].b
        );
  return 0;
}

void Ttexture::map_to_palette(Tpalette *pPal) {
  int i;
  int translation_palette[MAX_PALSIZE];

  for(i=0; i<pPal->size; i++) {
    translation_palette[i] = find_nearest_pal_entry
                             (_plocal_palette, pPal, i);
  }
}

