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
