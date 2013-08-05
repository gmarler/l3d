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

/* #defines for defining fixed point precision. can be overridden locally
 * by setting the following #defines to the desired precision values.
 * see fix_lowp.h for a concrete example.
 *
 * the precision can be defined more than once in the same file. if a particular
 * calculation needs a different precision, it should define its own precision,
 * make the calculations, then reinclude fix_prec.h to restore the default
 * values (expected by all other fixed point routines). See fix_lowp.h for
 * an example.
 */

/* FX_PRECIS: # of bits to left-shift, i.e. # of bits devoted to fract. part
 * FX_PRECIS_QUOT: bits of precision that the quotient has after fixfixdiv
 * FX_SIGN_MASK: bitmask for testing the sign bit
 * FX_ALLBITS_MASK: mask containing all bits, used in 2's complement
 * FX_PRECIS_MASK: mask for accessing just the precision part of a fixed number
 * FX_PRECISION_MAX: 1 more than the max value of the precision bits; used
 *    when converting fix2float and also float2fix
 */

#ifndef FX_PRECIS_DEFAULT
#define FX_PRECIS_DEFAULT 14
#endif

#undef FX_PRECIS
#undef FX_PRECIS_QUOT
#undef FX_PRECIS_MASK
#undef FX_PRECIS_MAX

#define FX_PRECIS 14
#define FX_PRECIS_QUOT 8

#define FX_SIGN_MASK    0x80000000L
#define FX_ALLBITS_MASK 0xFFFFFFFFL

/* binary: 0....0 0011 1111 1111 1111; accesses the precision part */
#define FX_PRECIS_MASK 0x00003FFFL
#define FX_PRECIS_MAX 16384.0

#define EPSILON_CEIL ((l3d_fixed)FX_PRECIS_MAX - 1)
#define iceil_fix(x) ( ((x) < 0) ? fix2int(x) : fix2int(x+EPSILON_CEIL) )

#undef l3d_fix_new_prec
#undef l3d_fix_norm_prec

