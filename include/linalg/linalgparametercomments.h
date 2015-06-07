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

#ifndef LINALGPARAMETERCOMMENTS_H
#define LINALGPARAMETERCOMMENTS_H

const char * E_ABSOLUTE_COMMENT = "# Absolute tolerance";
const char * E_RELATIVE_COMMENT = "# Relative tolerance for additive operations ";

const char * SPARSITY_RATIO_COMMENT = "# Nonzero ratio used as a bound between sparse and dense vector forms";
const char * ELBOWROOM_COMMENT = "# The number of extra spaces reserved in vectors for new elements";

/********************************************
 * BLOCK COMMENTS
 * *****************************************/

const char * LINALG_PARAMETERFILE_FIRST_COMMENT =
        R"(# Linear Algebraic parameter file for the Pannon Optimizer
# If this file is present, the values of the given parameters can be changed.)";


#endif // LINALGPARAMETERCOMMENTS_H
