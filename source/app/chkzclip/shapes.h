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

#include "object3d.h"

class Tpyramid:public T3dobject {
    int xtheta;
  public:
    Tpyramid(void);
    virtual ~Tpyramid(void);
    /* virtual */ int update(void);
};


class Tcube:public T3dobject {
    int xtheta,ytheta,ztheta;
    int xgoal, ygoal, zgoal;
    int xrotating,yrotating,zrotating;
    int xthetainc, ythetainc, zthetainc;
  public:
    Tcube(void);
    virtual ~Tcube(void);
    /* virtual */ int update(void);
};

class Torbitcube: public Tcube {
    int xorbit, yorbit, zorbit;
    int xogoal, yogoal, zogoal;
    int xorbiting, yorbiting, zorbiting;
  public:
    Torbitcube(void);
    virtual ~Torbitcube(void);
    /* virtual */ int update(void);
};

class Tmountains: public T3dobject {
#define TRIANGLES_PER_STRIP 14
#define NUM_STRIPS 5
    Mtype deltay [ (TRIANGLES_PER_STRIP/2 + 1) * (NUM_STRIPS+1) ];
  public:
    Tmountains(void);
    /* virtual */ int update(void);
};

class Ttorus: public T3dobject {
  private:
    void load(char *fname);
  public:
    Ttorus(void);
    virtual ~Ttorus(void);
    /* virtual */ int update(void);
};

extern unsigned char tex_quilt[TEX_HEIGHT][TEX_WIDTH];
void init_textures(void);
