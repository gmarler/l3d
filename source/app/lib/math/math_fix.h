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

#ifndef __MATH_FIX_H
#define __MATH_FIX_H
#include "../tool_os/memman.h"

#include "../system/sys_dep.h"
#include <assert.h>

//  /*
//   *****************************************************************************
//   *
//   * MATH_FIX.H
//   *
//   * Routines for performing fixed point math. For speed, all these
//   * routines are declared as macros and are not part of any C++ class.
//   *
//   * We use macros and NOT inline functions because we wish to be able
//   * to REDEFINE the fixed-point precision in the same file. This avoids
//   * some nasty overflow/underflow errors for particular calculations.
//   * The calculations are done with higher/lower precision, then the
//   * results are converted back into the original precision. This is possible
//   * with macros, but not possible with inline functions.
//   *
//   * Of course making parameterized inline functions would also be possible,
//   * where the precision parameters are all passed in,  but we want to keep
//   * this as simple (and thus fast) as possible.
//   * 
//   *****************************************************************************
//   */

//- We perform fixed point math by using 32-bit integers, which 
//- we assume are signed. Since we assume they are signed,
//- fixed point multiplication does not do sign extension, for speed
//- reasons.

typedef long l3d_fixed;  //- this must be 32 bits, and MUST BE SIGNED


//- Now we define how many bits we use to store the fractional part, and
//- the remainder we use to store the integer part.
//-
//- FX_PRECIS is the # of precision bits, which we set to 14
//- FX_PRECIS_QUOT is the # of precision bits for the divisor AND quotient
//-
//- 14 bits means we can represent up to 2^-16384 precision = 4 dec places
//- implies 32 - 14 - 1(sign bit) = 17 bits for whole part = max 131072
//- implies 32 - 14 -1(sign) whole bits after int multiplication = max 131072
//- implies 32 - (14+14) -1(sign) whole bits after fix*fix mult. = max 8
//-    but we can express fix*fix mult as a series of fix*int mult's; slower
//- implies FX_PRECIS_QUOT bits of precision for divisor AND quotient
//-
//- summarizing:
//- addition/subtraction of fixed points: +/- 131072.0000
//- mult of fixed*int:                  : +/- 131072.0000
//- mult of fixed*fixed:                : +/- 8.00000000
//- slower mult of fixed*fixed          : +/- 131072.0000
//- div of fixed/fixed                  : +/- 131072.00
//- div of fixed/int                    : +/- 131072.0000

#ifndef FX_PRECISION_ALREADY_DEFINED
#define FX_PRECISION_ALREADY_DEFINED
#include "fix_prec.h"
#endif

extern int fm_whole;
extern long fm_localdata;
extern float fm_sign;

//- conversion routines between fixed and floating/integer and vice-versa

//  #ifdef SLOW
//  inline Tfixed float2fix(float aFloat) {
//    fm_whole = (int)aFloat;  /* whole part of floating point number */

//    /* if negative */
//    if(aFloat < 0.0) {

//      /*
//       * convert floating and whole part to positive numbers, left shift
//       * as positive, then take two's complement of result to turn it
//       * into negative
//       */
//      aFloat = -aFloat;
//      fm_whole = -fm_whole;

//      return
//        ( ( ((long)fm_whole << FX_PRECIS)           /* whole part left shifted */
//            |
//            (int) ((aFloat-fm_whole)*FX_PRECIS_MAX) /* frac. part left shifted */
//          ) ^ FX_ALLBITS_MASK) + 1;
//      /* two's complement = -ve */
//    }

//    /* if positive */
//    else {
//      return ((long)fm_whole << FX_PRECIS) |        /* whole part left shifted */
//             (int) ((aFloat-fm_whole)*FX_PRECIS_MAX); /* frac. part     */
//    }
//  }
//  #else

#define float2fix(aFloat) ((long)((aFloat)*FX_PRECIS_MAX))

//  #endif // SLOW

#define int2fix(aInt) ((long)(aInt) << FX_PRECIS)

//  #ifdef SLOW
//  inline float fix2float(Tfixed aFixed) {
//    fm_sign = 1.0;
//    fm_localdata = aFixed;

//    if(fm_localdata & FX_SIGN_MASK) {  //- if negative
//      //- change the negative fixed point number into a positive one and
//      //- multiply by -1 at the end
//      fm_sign = -1.0;
//      //- applying the 2's complement to a 2's comp. -ve number yields a +ve
//      fm_localdata = (fm_localdata ^ FX_ALLBITS_MASK) + 1;
//    }

//    //- right shift whole part, calculate fractional part, times -1 if needed
//    return  (float)
//            ( (fm_localdata>>FX_PRECIS) +                           /* whole part */
//              (float)(fm_localdata&FX_PRECIS_MASK)/FX_PRECIS_MAX )  /* frac. part */
//            *fm_sign;                     /* in case we flipped the sign up above */
//  }
//  #else

#define fix2float(aFixed) ((double)((aFixed) / FX_PRECIS_MAX))

// #endif // SLOW

#define fix2int(aFixed) ( (aFixed) >> FX_PRECIS)

//- right shift for whole, discard frac. part

//- routines for fixed*fixed and fixed*int multiplication
//-
//- general notes for multiplication:
//- 1. ensure operands are in correct format (sign-extending if necessary)
//- 2. ensure result is in correct format (sign-extending if necessary)
//- 
//- #2 automatically taken care of IF fixed-type is signed
//- thus we just worry about #1, for FixIntMul


//-
//- just treat a as a regular signed long and b as a regular signed int
//- proof:
//-    f1_fixed = Int(f1*SHIFT)
//-    f1_fixed * x = Int(f1*SHIFT) * x;
//-    f1_fixed * x = Int(f1*x*SHIFT);
//-
//- the result is properly shifted.
//-
//- sign extension is unnecessary if we assume a is signed; this means
//- we do not need sign-extending code such as
//-    if(b<0) {  // sign-extend b, if b -ve
//-       return a * ( ((long)(-b)^FX_ALLBITS_MASK) + 1 ) ;
//-    }
//-

#define fixintmul(a,b) ( { \
assert((fix2float(a)*(b)-fix2float((a)*(b)) > -0.2) && \
       (fix2float(a)*(b)-fix2float((a)*(b)) <  0.2) || \
       (printf("fixintmul %f %d = %f, should be %f\n", \
               fix2float(a),b,fix2float((a)*(b)),fix2float(a)*(b))\
        &&0)); \
(a)*(b); } )

//- a=fixed, b=int

#define FIX_ASM
#ifdef FIX_ASM

#define fixfixmul(a,b) \
   ( { long __result = (a), __arg = (b); \
       asm("imul %2;" \
           "shrd %3,%2,%0;" \
          : "=a" (__result) \
          : "0"  (__result), "d"  (__arg) , "I" (FX_PRECIS) ); \
       assert( \
             ((fix2float(__result) - fix2float(a)*fix2float(b) > -0.2) \
               && \
              (fix2float(__result) - fix2float(a)*fix2float(b) <  0.2)) \
              || \
              (printf("fixfixmul %f %f = %f, should be %f\n", \
		      fix2float(a),fix2float(b),fix2float(__result), \
		      fix2float(a)*fix2float(b)) \
              && 0) \
             ); \
       __result; \
      } \
   )
//  #ifdef OLD
//  asm("mov %2,%edx\nimul %2\nadd $0x8000,%0\nadc 0,%2\nshrd %3,%2,%0\n" 
//      : "0"  (__result), "Ad"  (__arg) , "I" (FX_PRECIS) ); 

//  #endif

#else

//-
//- if we were to use a formula of the form
//-     return ((a*b) >> FX_PRECIS) ;
//- then we would have only 
//-    32 - (14+14) -1(sign) whole bits after fix*fix mult. = max 8
//- as the maximum after a fixed*fixed multiplication.
//-
//- proof:
//-    f1_fixed = Int(f1*SHIFT)
//-    f2_fixed = Int(f2*SHIFT)
//-    f1_fixed- f2_fixed = Int(f1*SHIFT)- Int(f2*SHIFT)
//-                        = Int(f1*f2*SHIFT*SHIFT)
//-                          so we shift TWICE = 2*SHIFT bits taken up by 
//-                          fractional part; = TOTAL_BITS - 2*SHIFT -1(sign)
//-                          for whole part   = 32 - 2*14 - 1 = 3; 2^3=8 !
//-
//- Instead we recast the fixed*fixed multplication as follows:
//-
//- fixed1- fixed2 = (f1_whole + f1_frac)- (f2_whole + f2_frac)
//-                 =    f1_whole*f2_whole
//-                    + f1_frac*f2_frac
//-                    + f1_whole*f2_frac
//-                    + f1_frac*f2_whole
//- note that if fixed1 is negative, BOTH f1_whole and f1_frac are negative
//- particularly important is f1_frac, since if we try to access the
//- fractional part with &FX_SIGN_MASK we lose the sign if it's negative.
//- thus if a # is negative we access its fractional part by first
//- making it positive, &'ing with FX_SIGN_MASK, and -ating the result.
//-


(
  /* if */ ( ((b) & FX_SIGN_MASK) ^ ((a) & FX_SIGN_MASK) ) ? /* dift signs */ \
  /* if */ ( ((b) & FX_SIGN_MASK) ^ ((a) & FX_SIGN_MASK) ) ? /* dift signs */ \
  /* if */  ((b) & FX_SIGN_MASK) ?  /* a +ve, b -ve */ \
  - ( \
      ( (((a)>>FX_PRECIS) * ((-(b))>>FX_PRECIS)) << FX_PRECIS ) + \
      ( ((a) & FX_PRECIS_MASK) * ((-(b)) & FX_PRECIS_MASK) >> FX_PRECIS ) + \
      ( ((a)>>FX_PRECIS) * ((-(b)) & FX_PRECIS_MASK) ) + \
      ( ((-(b))>>FX_PRECIS) * ((a) & FX_PRECIS_MASK) ) \
    ) \
  /* else */ : /* a -ve, b +ve */ \
  - ( \
      ((((-(a))>>FX_PRECIS) * ((b)>>FX_PRECIS)) << FX_PRECIS ) + \
      (((-(a)) & FX_PRECIS_MASK) * ((b) & FX_PRECIS_MASK) >> FX_PRECIS )+\
      (((-(a))>>FX_PRECIS) * ((b) & FX_PRECIS_MASK) ) + \
      (((b)>>FX_PRECIS) * ((-(a)) & FX_PRECIS_MASK) ) \
    ) \
  /* else */ : /* same signs */ \
      /* if */ ((b) & FX_SIGN_MASK) ?  /* a -ve, b -ve */ \
      ( \
        ((((-(a))>>FX_PRECIS) * ((-(b))>>FX_PRECIS)) << FX_PRECIS ) + \
        (((-(a)) & FX_PRECIS_MASK) * ((-(b)) & FX_PRECIS_MASK) >> FX_PRECIS )+\
        (((-(a))>>FX_PRECIS) * ((-(b)) & FX_PRECIS_MASK) ) + \
        (((-(b))>>FX_PRECIS) * ((-(a)) & FX_PRECIS_MASK) ) \
      ) \
      /* else */ : /* a +ve, b+ve */ \
      ( \
        ((((a)>>FX_PRECIS) * ((b)>>FX_PRECIS)) << FX_PRECIS ) + \
        (((a) & FX_PRECIS_MASK) * ((b) & FX_PRECIS_MASK) >> FX_PRECIS ) + \
        (((a)>>FX_PRECIS) * ((b) & FX_PRECIS_MASK) ) + \
        (((b)>>FX_PRECIS) * ((a) & FX_PRECIS_MASK) ) \
      )
    )

#endif


//- routines for fixed/int and fixed/fixed division

//- NOTES: fixed division -- each bit you take OUT of the divisor is
//- a bit of precision you RECEIVE in the quotient.  If you take NO
//- bits out of the divisor (i.e. divisor is not right-shifted and
//- has full precision), quotient has NO fractional precision!
//- proof:
//-    f1_fixed = Int(f1 SHIFT)
//-    f2_fixed = Int(f2 SHIFT)
//-    f1_fixed/f2_fixed = Int(f1*SHIFT) div Int(f2*SHIFT)
//-                      = Int((f1*SHIFT)/(f2*SHIFT))
//-                      = Int(f1/f2)
//-                      = no fractional precision at all
//-                        a final shift of SHIFT is necessary to cvt 2 fixed

//- taking bits out of the divisor means doing a reverse-shift on the
//- divisor and yields:
//-    f1_fixed/f2_fixed = Int(f1*SHIFT) div Int(f2*SHIFT/RSHIFT)
//-                      = Int( (f1*SHIFT)/(f2*SHIFT/RSHIFT) )
//-                      = Int( f1*RSHIFT / f2 )
//-                      = gained RSHIFT bits of precision in result
//-                        final shift of SHIFT-RSHIFT is needed to cvt 2 fixed

//- This is solved in HW by making dividend twice as large, but here we
//- must simply make our divisor less accurate. We make our divisor
//- less accurate by FX_PRECIS_QUOT (=RSHIFT) bits. We normally
//- shift by FX_PRECIS (=SHIFT) bits.


//- general notes for division:
//- 1. ensure operands are both positive (negating -- based on the
//-    type of the operand -- as necessary)
//- 2. ensure result has correct sign (negating, based on the representation
//-    of Tfixed, as necessary)



#ifdef FIX_ASM

// input: eax = dividend
//        ebx = divisor
//
// eax is left shifted into edx to make dividend bigger
// NOPE! must START in edx, RIGHT shift (32-FX_PRECIS) bits INTO AX,
// cuz otherwise left shifting don't sign-extend!
// oddly shrd does not shift the src operand, we do this separately
// with a sar

#define fixfixdiv(a,b) \
   ( { long __result = (a), __arg = (b); \
       asm("xor %1,%1;"  \
           "shrd %4,%3,%1;" \
           "sar  %4,%3;" \
           "idiv %2" \
          : "=a" (__result) \
          : "0"  (0), "b"  (__arg) , "d"(a) , "I" (32-FX_PRECIS) ); \
       assert( \
              (fix2float(__result) - fix2float(a)/fix2float(b) > -0.2) \
              && \
              (fix2float(__result) - fix2float(a)/fix2float(b) <  0.2) \
              || \
              (printf("fixfixdiv %f %f = %f, should be %f\n", \
		      fix2float(a),fix2float(b),fix2float(__result), \
		      fix2float(a)/fix2float(b)) \
              && 0) \
             ); \
       __result; \
      } \
   )

#else

(\
 /* NEGATIVE RESULT */ \
 \
 /* if */ ( ((b) & FX_SIGN_MASK) ^ ((a) & FX_SIGN_MASK)) ? /* dift signs */ \
 /* if */ ((b)&FX_SIGN_MASK) ?  /* a+ve, b-ve */ \
 /* do a positive/positive division and negate (2's comp.) result */ \
 ~ (((a)/((~(b)+1)>>FX_PRECIS_QUOT)) << (FX_PRECIS-FX_PRECIS_QUOT)) \
 + 1 \
 /* else */ : /* a-ve, b+ve */ \
 /* do a positive/positive division and negate (2's comp.) result */ \
 ~ (((~(a)+1)/((b)>>FX_PRECIS_QUOT)) << (FX_PRECIS - FX_PRECIS_QUOT)) \
 + 1\
 \
 /* POSITIVE RESULT */ \
 \
 /* else */ : \
 /* if */ ((b)&FX_SIGN_MASK) ?  /* a-ve, b-ve */ \
 \
 /* do a positive/positive division instead of a -ve/-ve division */ \
 ((~(a)+1) / ((-(b))>>FX_PRECIS_QUOT)) << (FX_PRECIS - FX_PRECIS_QUOT) \
 /* else */ : /* a+ve, b+ve */ \
 \
 /* easiest case: a positive/positive division */ \
 ((a) / ((b)>>FX_PRECIS_QUOT)) << (FX_PRECIS - FX_PRECIS_QUOT)
)

#endif



//- just treat a as a regular signed long and b as a regular signed int
//- proof:
//-    f1_fixed = Int(f1*SHIFT)
//-    f1_fixed / x = Int(f1*SHIFT) / x;
//-    f1_fixed / x = Int(f1/x*SHIFT);

//- the result is properly shifted.


#define fixintdiv(a,b) ( { \
assert((fix2float(a)/(b)-fix2float((a)/(b)) > -0.2) && \
       (fix2float(a)/(b)-fix2float((a)/(b)) <  0.2) || \
       (printf("fixintdiv %f %d = %f, should be %f\n", \
               fix2float(a),b,fix2float((a)/(b)),fix2float(a)/(b))\
        &&0)); \
(a)/(b); } )
/* a fix, b int */

#endif
