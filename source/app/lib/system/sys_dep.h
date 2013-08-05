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


#ifndef __SYSDEP_H
#define __SYSDEP_H
#include "../tool_os/memman.h"

/*
 ****************************************************************************
 * 
 * SYS_DEP.H
 *
 * System-dependent declarations -- compiler defines, fixed or floating
 * point math, etc.
 *
 ****************************************************************************
 */

/*
 * Compiler-specific defines and includes
 *
 * Possible choices: BORLAND16 defined ==> Borland (16 bit) compiler
 *                   else              ==> GNU GCC compiler
 */
#ifdef BORLAND16_DOS

#define DOS_OS
#undef WINDOWS_OS
#undef X11_OS
#define FARPTR far
#define FARmalloc farmalloc
#define FARfree farfree
#define ABS_CAST (double)
#define DGROUP64K
#undef INT32
#define SCREEN_XSIZE 320
#define SCREEN_YSIZE 200
#include <graphics.h>
#include <alloc.h>
#include <dos.h>
#include <conio.h>
#include <mem.h>

#elif defined DJGPP_DOS

#define DOS_OS
#undef WINDOWS_OS
#undef X11_OS
#define FARPTR
#define FARmalloc malloc
#define FARfree free
#define ABS_CAST
#undef DGROUP64K
#define INT32
#define SCREEN_XSIZE 320
#define SCREEN_YSIZE 200
#include <dpmi.h>
#include <pc.h>
#include <go32.h>
#include <dos.h>
#include <conio.h>
#include <mem.h>

#elif defined BORLAND16_WIN

#undef DOS_OS
#define WINDOWS_OS
#undef X11_OS
#include <windows.h>
#include <windowsx.h>
#define FARPTR far
#define FARmalloc farmalloc
#define FARfree farfree
#define ABS_CAST (double)
#define DGROUP64K
#undef INT32
#define SCREEN_XSIZE 320
#define SCREEN_YSIZE 200
#include <alloc.h>
#include <mem.h>

#elif defined CYGNUSGCC_WIN

#define MAIN_FUNCTION_NAME int WINAPI WinMain (HINSTANCE hInst,  HINSTANCE hPrevInstance, char * lpszCmdParam, int nCmdShow)
#define SW_RAST_Y_REVERSAL(ysize,y) ((ysize) - (y))


#undef DOS_OS
#define WINDOWS_OS
#undef X11_OS
extern "C" {
#include <windows.h>
#include <windowsx.h>
}
#define FARPTR
#define FARmalloc malloc
#define FARfree free
#define ABS_CAST 
#undef DGROUP64K
#define INT32
#define BITS_PER_BYTE 8

#elif defined LINUX_GCC_X

#undef DOS_OS
#undef WINDOWS_OS
#define SW_RAST_Y_REVERSAL(ysize,y) (y)
#define X11_OS
#define FARPTR
#define FARmalloc malloc
#define FARfree free
#define ABS_CAST 
#undef DGROUP64K
#define INT32
#define SCREEN_XSIZE 320
#define SCREEN_YSIZE 200
#define BITS_PER_BYTE 8

#endif


#define l3d_abs(x) ( (x)<0?(-x):(x) )

/*
 * Fixed vs. floating point
 */

#define MAX_BYTE 255

#include "../math/math_fix.h"
#define EPSILON_CEIL ((l3d_fixed)FX_PRECIS_MAX - 1)
#define iceil_fix(x) ( ((x) < 0) ? fix2int(x) : fix2int(x+EPSILON_CEIL) )


#ifdef FIXED_POINT_MATH

typedef l3d_fixed l3d_real;
#define l3d_real_to_float(m) fix2float(m)
#define float_to_l3d_real(f) float2fix(f)
#define l3d_real_to_int(m)   fix2int(m)
#define int_to_l3d_real(i)   int2fix(i)
#define l3d_mulrr(a,b) fixfixmul(a,b)
#define l3d_mulri(a,b) fixintmul(a,b)
#define l3d_divrr(a,b) fixfixdiv(a,b)
#define l3d_divri(a,b) fixintdiv(a,b)
//#define EPSILON_FLOOR (int_to_l3d_real(1) - ((l3d_fixed)FX_PRECIS_MAX - 1))
#define iceil(x) ( ((x) < 0) ? l3d_real_to_int(x) : l3d_real_to_int(x+EPSILON_CEIL) )
// #define iceil(x) ( ((x) < 0) ? l3d_real_to_int(x-EPSILON_CEIL) : l3d_real_to_int(x) )
#define ifloor(x) ( ((x) < 0) ? l3d_real_to_int(x-EPSILON_CEIL) : l3d_real_to_int(x) )
#define l3d_sqrt(x)  ( float_to_l3d_real(sqrt(l3d_real_to_float(x))) )
#define l3d_real_to_fixed(m) (m)
#define fixed_to_l3d_real(m) (m)

#else

//sbegin l3d_real
typedef float l3d_real;
#define l3d_real_to_float(m) (m)
#define float_to_l3d_real(f) ((float)(f))
#define l3d_real_to_int(m)   ((int)(m))
#define int_to_l3d_real(i)   ((float)(i))
#define l3d_mulrr(a,b) (a)*(b)
#define l3d_mulri(a,b) (a)*(b)
#define l3d_divrr(a,b) (a)/(b)
#define l3d_divri(a,b) (a)/(b)
#define iceil(x) ( (int)ceil((double)(x)) )
#define ifloor(x) ( (int)floor((double)(x)) )
#define l3d_sqrt(x)  ( float_to_l3d_real(sqrt(l3d_real_to_float(x))) )
#define l3d_real_to_fixed(f) float2fix(f)
#define fixed_to_l3d_real(f) fix2float(f)

//send l3d_real

#endif

#endif

