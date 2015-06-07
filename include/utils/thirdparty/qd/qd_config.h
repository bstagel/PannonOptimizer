//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

/* include/qd/qd_config.h.  Generated from qd_config.h.in by configure.  */
#ifndef _QD_QD_CONFIG_H
#define _QD_QD_CONFIG_H  1

#ifndef QD_API
#define QD_API /**/
#endif

/* Set to 1 if using VisualAge C++ compiler for __fmadd builtin. */
#ifndef QD_VACPP_BUILTINS_H
/* #undef QD_VACPP_BUILTINS_H */
#endif

/* If fused multiply-add is available, define to correct macro for
   using it.  It is invoked as QD_FMA(a, b, c) to compute fl(a * b + c). 
   If correctly rounded multiply-add is not available (or if unsure), 
   keep it undefined.*/
#ifndef QD_FMA
/* #undef QD_FMA */
#endif

/* If fused multiply-subtract is available, define to correct macro for
   using it.  It is invoked as QD_FMS(a, b, c) to compute fl(a * b - c). 
   If correctly rounded multiply-add is not available (or if unsure), 
   keep it undefined.*/
#ifndef QD_FMS
/* #undef QD_FMS */
#endif

/* Set the following to 1 to define commonly used function
   to be inlined.  This should be set to 1 unless the compiler 
   does not support the "inline" keyword, or if building for 
   debugging purposes. */
#ifndef QD_INLINE
#define QD_INLINE 1
#endif

/* Set the following to 1 to use ANSI C++ standard header files
   such as cmath, iostream, etc.  If set to zero, it will try to 
   include math.h, iostream.h, etc, instead. */
#ifndef QD_HAVE_STD
#define QD_HAVE_STD 1
#endif

/* Set the following to 1 to make the addition and subtraction
   to satisfy the IEEE-style error bound 

      fl(a + b) = (1 + d) * (a + b)

   where |d| <= eps.  If set to 0, the addition and subtraction
   will satisfy the weaker Cray-style error bound

      fl(a + b) = (1 + d1) * a + (1 + d2) * b

   where |d1| <= eps and |d2| eps.           */
#ifndef QD_IEEE_ADD
/* #undef QD_IEEE_ADD */
#endif

/* Set the following to 1 to use slightly inaccurate but faster
   version of multiplication. */
#ifndef QD_SLOPPY_MUL
#define QD_SLOPPY_MUL 1
#endif

/* Set the following to 1 to use slightly inaccurate but faster
   version of division. */
#ifndef QD_SLOPPY_DIV
#define QD_SLOPPY_DIV 1
#endif

/* Define this macro to be the isfinite(x) function. */
#ifndef QD_ISFINITE
#define QD_ISFINITE(x) std::isfinite(x)
#endif

/* Define this macro to be the isinf(x) function. */
#ifndef QD_ISINF
#define QD_ISINF(x) std::isinf(x)
#endif

/* Define this macro to be the isnan(x) function. */
#ifndef QD_ISNAN
#define QD_ISNAN(x) std::isnan(x)
#endif


#endif /* _QD_QD_CONFIG_H */
