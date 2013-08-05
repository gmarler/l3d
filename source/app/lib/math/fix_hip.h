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

/* #defines for defining lower-precision fixed point precision.
 *
 * mostly used with dot products and such where large numbers are
 * multiplied with one another for a temporarily large intermediate
 * result which would otherwise overflow.
 *
 * USAGE: fx_prec.h must already have been included. for the temporary
 * lower-precision calculation this file is included right before it is
 * needed. then the calculation is done, whereby all values which are
 * already in fixed point in the default precision must be converted
 * with macro FX_CVT() into the lower-precision format. the results
 * are then converted back to default precision with macro FX_CVT_BACK().
 * finally fx_prec.h should be included again to set the precision values
 * back to defaults.
 *
 * NOTE: redefining the precision **only** has an effect on those macros
 * defined in math_fix.h. redefining the precision will not affect
 * other routines which are called and which use fixed point math,
 * since the other routines will or have been compiled with the
 * default fixed point precision. (we're using macros, not run-time
 * dynamic alteration!)
 *
 */

/* FX_PRECIS: # of bits to left-shift, i.e. # of bits devoted to fract. part
 * FX_PRECIS_QUOT: bits of precision that the quotient has after fixfixdiv
 * FX_SIGN_MASK: bitmask for testing the sign bit
 * FX_ALLBITS_MASK: mask containing all bits, used in 2's complement
 * FX_PRECIS_MASK: mask for accessing just the precision part of a fixed number
 * FX_PRECISION_MAX: 1 more than the max value of the precision bits; used
 *    when converting fix2float and also float2fix
 */

#ifdef FIXED_POINT_MATH

#undef FX_PRECIS
#undef FX_PRECIS_QUOT
#undef FX_PRECIS_MASK
#undef FX_PRECIS_MAX
#undef FX_CVT
#undef FX_CVT_BACK

#define FX_PRECIS 28
#define FX_PRECIS_QUOT 14

/* binary: 0....0 0000 0000 1111 1111; accesses the precision part */
#define FX_PRECIS_MASK 0x0FFFFFFFL
#define FX_PRECIS_MAX 268435456.0;

/* with FX_CVT we have to worry about sign extension, thus the ternary op */
#define FX_CVT(a) ((a)>0 ? (a)>>FX_PRECIS_DEFAULT-FX_PRECIS : -(-(a)>>FX_PRECIS_DEFAULT-FX_PRECIS))
#define FX_CVT_BACK(a) ((a)<<FX_PRECIS_DEFAULT-FX_PRECIS)

#define EPSILON_CEIL ((l3d_fixed)FX_PRECIS_MAX - 1)
#define iceil_fix(x) ( ((x) < 0) ? fix2int(x) : fix2int(x+EPSILON_CEIL) )

#else

/* fixed point math is not being used, so don't do any conversion */

#define FX_CVT(a) (a)
#define FX_CVT_BACK(a) (a)

#endif
