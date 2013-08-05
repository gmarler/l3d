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

#include "tl_ppm.h"
#include "../../system/sys_dep.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../tool_os/memman.h"

int l3d_texture_loader_ppm::process_tok(char *tok) {
  switch(state) {
    case 0: {
        if(strcmp(tok,"P3")==0) {
          state=1;
          return 1;
        }
        else if (strcmp(tok,"P6")==0) {
          state=10;
          return 1;
        }
        else return 0;
      }

    case 1: {
        sscanf(tok,"%d",&width);
        state=2;
        return 1;
      }

    case 2: {
        sscanf(tok,"%d",&height);
        data = new unsigned char [width*height*si->bytes_per_pixel];

        cur_pdata = data;

        state = 3;
        return 1;
      }

    case 3: {
        int maxcol=0;
        sscanf(tok,"%d",&maxcol);
        si->ext_max_red = si->ext_max_green = si->ext_max_blue = maxcol;
        state = 4;
        return 1;
      }

    case 4: {
        sscanf(tok,"%d",&cur_r);
        state = 5;
        return 1;
      }

    case 5: {
        sscanf(tok,"%d",&cur_g);
        state = 6;
        return 1;
      }

    case 6: {
        int blue=0;
        sscanf(tok,"%d",&cur_b);

        //printf("got ASC %d %d %d\n",_cur_r,_cur_g,_cur_b);

        unsigned long native_col = si->ext_to_native(cur_r, cur_g, cur_b);

        register int i;
        unsigned long mask = MAX_BYTE;
        char shift = 0;

        // stuff the bytes in the unsigned long col into the texture buffer, in
        // little-endian order
        for(i=0; i<si->bytes_per_pixel; i++) {
          *cur_pdata++ = (native_col & mask) >> shift;
          mask <<= BITS_PER_BYTE;
          shift += BITS_PER_BYTE;
        }

        state = 4;
        return 1;
      }

    case 10: {
        sscanf(tok,"%d",&width);
        state = 11;
        return 1;
      }

    case 11: {
        sscanf(tok,"%d",&height);
        data = new unsigned char [width*height*si->bytes_per_pixel] ;

        cur_pdata = data;

        state = 12;
        return 1;
      }

    case 12: {
        int maxcol=0;
        sscanf(tok,"%d",&maxcol);
        si->ext_max_red = si->ext_max_green = si->ext_max_blue = maxcol;
        state = 13;
        return 2;  // signal caller that we must start reading binary data now
      }

    case 13: {
        unsigned char c;
        sscanf(tok,"%c",&c);
        cur_r = c;
        state = 14;
        return 1;
      }

    case 14: {
        unsigned char c;
        sscanf(tok,"%c",&c);
        cur_g = c;
        state = 15;
        return 1;
      }

    case 15: {
        unsigned char c;
        sscanf(tok,"%c",&c);
        cur_b = c;

        unsigned long native_col = si->ext_to_native(cur_r, cur_g, cur_b);

        register int i;
        unsigned long mask = MAX_BYTE;
        char shift = 0;

        // stuff the bytes in the unsigned long col into the texture buffer, in
        // little-endian order
        for(i=0; i<si->bytes_per_pixel; i++) {
          *cur_pdata++ = (native_col & mask) >> shift;
          mask <<= BITS_PER_BYTE;
          shift += BITS_PER_BYTE;
        }

        state = 13;
        return 1;
      }

    default: fprintf(stderr,"unknown error parsing ppm file\n");
      return -1;
  } // case
}

void l3d_texture_loader_ppm::load(const char *fname) {
  FILE *fp;
  char *tok;
  char s[256]="";
  int result;

  fp=fopen(fname, "r");
  if(!fp) {
    fprintf(stderr,"Error opening texture ppm file %s\n", fname);
    return;
  }

  state = 0;
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

  if(!feof(fp) && result==2) {  //- P6 binary ppm file
    s[1] = 0;
    s[0] = fgetc(fp);

    //- skip comment lines before begin of binary image data
    while (s[0] == '#') {
      s[0] = fgetc(fp);
      while(s[0] != '\n') {
        s[0] = fgetc(fp);
      }
    }

    //- process binary image data
    while(!feof(fp)) {
      process_tok(s);
      s[0] = fgetc(fp);
    }
  }
}

