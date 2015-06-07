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

/*
 * include/fpu.h
 *
 * This work was supported by the Director, Office of Science, Division
 * of Mathematical, Information, and Computational Sciences of the
 * U.S. Department of Energy under contract number DE-AC03-76SF00098.
 *
 * Copyright (c) 2001
 *
 * Contains functions to set and restore the round-to-double flag in the
 * control word of a x86 FPU.  The algorithms in the double-double and
 * quad-double package does not function with the extended mode found in
 * these FPU.
 */
#ifndef _QD_FPU_H
#define _QD_FPU_H

#include <qd/qd_config.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Set the round-to-double flag, and save the old control word in old_cw.
 * If old_cw is NULL, the old control word is not saved.
 */
QD_API void fpu_fix_start(unsigned int *old_cw);

/*
 * Restore the control word.
 */
QD_API void fpu_fix_end(unsigned int *old_cw);

#ifdef __cplusplus
}
#endif

#endif  /* _QD_FPU_H */
