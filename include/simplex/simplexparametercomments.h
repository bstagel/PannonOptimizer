#ifndef SIMPLEXPARAMETERCOMMENTS_H
#define SIMPLEXPARAMETERCOMMENTS_H

#define TOLERANCES_PIVOT_COMMENT "# Pivot tolerance for SSX iterations"
#define TOLERANCES_E_FEASIBILITY_COMMENT "# Feasibility tolerance"
#define TOLERANCES_E_OPTIMALITY_COMMENT "# Optimality tolerance"

#define STARTING_PRESOLVE_ENABLE_COMMENT ""
#define STARTING_SCALING_ENABLE_COMMENT ""
#define STARTING_SCALING_TYPE_COMMENT "# BENICHOU: Benichou scaling with powers of 2 "

#define STARTING_BASIS_STARTING_NONBASIC_STATES_COMMENT R"(# NONBASIC_TO_LOWER: Non-basic variables at lower bound \
# NONBASIC_TO_UPPER: Non-basic variables at upper bound \
# NONBASIC_MIXED: Mixed non-basic variable bounds)"

#define STARTING_BASIS_STARTING_BASIS_STRATEGY_COMMENT R"(# Starting basis strategies \
# LOGICAL: Logical basis \
# CRASH: CRASH basis)"

#define FACTORIZATION_TYPE_COMMENT R"(# Type of basis factorization \
# PFI: Product form of the inverse \
# LU: LU factorization)"

#define FACTORIZATION_REINVERSION_FREQUENCY_COMMENT "# Frequency of reinversions in number of iterations"
#define FACTORIZATION_PIVOT_THRESHOLD_COMMENT "# Constant value used for threshold pivoting"

#define FACTORIZATION_PFI_NONTRIANGULAR_METHOD_COMMENT R"(# The method used to process the non-triangular kernel \
# SIMPLE_KERNEL: Simple search for potential pivot positions  \
# BLOCK_PIVOT: Create a block triangular form and pivot down the diagonal \
# BLOCK_ORDER = Create a block triangular form and order the blocks by column counts before pivoting)"

#define FACTORIZATION_PFI_NONTRIANGULAR_PIVOT_RULE_COMMENT R"(# Pivot rules for the non-triangular part \
# SIMPLE_PIVOT = Simple pivot tolerance is used \
# THRESHOLD_PIVOT = Threshold pivoting )"

#define PRICING_TYPE_COMMENT R"(# Pricing type \
# DANTZIG: Dantzig rule \
# DEVEX: Devex rule \
# STEEPEST_EDGE: Steepest edge rule)"

#define PRICING_SIMPRI_COMMENT R"(	# Parameters of the SIMPRI partial pricing framework \
# The range of vectors are divided into clusters during the pricing, which are \
# scanned as a round robin in order to find improving candidates. \
# With partial pricing it can be specified that only a certain number \
# of clusters are visited to find improving candidates. If no candidates \
# found, more clusters are involved until necessary. \
# It is also possible to stop the cluster visit if enough candidates are found. \
# Parameter valus 0 for improving candidates means this stop condition is turned off. \
# All the three parameters can be specified both for phase I and phase II.)"

#define PRICING_SIMPRI_PHASEI_CLUSTERS_COMMENT ""
#define PRICING_SIMPRI_PHASEI_VISIT_CLUSTERS_COMMENT ""
#define PRICING_SIMPRI_PHASEI_IMPROVING_CANDIDADES_COMMENT ""
#define PRICING_SIMPRI_PHASEII_CLUSTERS_COMMENT ""
#define PRICING_SIMPRI_PHASEII_VISIT_CLUSTERS_COMMENT ""
#define PRICING_SIMPRI_PHASEII_IMPROVING_CANDIDADES_COMMENT ""

#define RATIOTEST_NONLINEAR_PRIMAL_PHASEI_FUNCTION_COMMENT R"(	# Use the piecewise linear concave function in primal phase I. \
# TRADITIONAL: Traditional one step method \
# PIECEWISE: Piecewise linear function \
# PIECEWISE_THRESHOLD: Piecewise linear function with numerical threshold)"

#define RATIOTEST_NONLINEAR_DUAL_PHASEI_FUNCTION_COMMENT R"(	# Use the piecewise linear concave function in dual phase I. \
# TRADITIONAL: Traditional one step method \
# PIECEWISE: Piecewise linear function \
# PIECEWISE_THRESHOLD: Piecewise linear function with numerical threshold )"

#define RATIOTEST_NONLINEAR_DUAL_PHASEII_FUNCTION_COMMENT R"(	# Use the piecewise linear concave function in dual phase II. \
# TRADITIONAL: Traditional one step method \
# PIECEWIESE: Piecewise linear function \
# PIECEWISE_THRESHOLD: Piecewise linear function with numerical threshold )"

#define RATIOTEST_ENABLE_FAKE_FEASIBILITY_COMMENT "# Enable fake feasibility handling in Dual Phase I and II"
#define RATIOTEST_EXPAND_MULTIPLIER_COMMENT "# EXPAND multiplier initializing value"
#define RATIOTEST_EXPAND_DIVIDER_COMMENT "# EXPAND divider initializing value"
#define RATIOTEST_EXPAND_TYPE_COMMENT R"(		# EXPAND procedure \
# INACTIVE: Inactive \
# HARRIS: Harris ratiotest \
# EXPANDING = Expanding tolerance)"

#define PERTURBATION_PERTURB_COST_VECTOR_COMMENT R"(	# Cost vector perturbation \
# INACTIVE: Inactive \
# STRUCTURAL: Active for structural variables \
# STRUCTURAL_AND_LOGICAL: Active for structural and logical variables)"

#define PERTURBATION_E_COST_VECTOR_COMMENT "# Measure of perturbation"
#define PERTURBATION_PERTURB_RHS_COMMENT "# RHS perturbation"
#define PERTURBATION_E_RHS_COMMENT "# Measure of perturbation"
#define PERTURBATION_SHIFT_BOUNDS_COMMENT "# Bound shifting in primal"
#define PERTURBATION_E_BOUNDS_COMMENT "# Measure of shifting"

#define GLOBAL_STARTING_ALGORITHM_COMMENT R"(	# Starting algorithm of the simplex method \
# PRIMAL: primal simplex \
# DUAL: dual simplex)"    \

#define GLOBAL_SWITCH_ALGORITHM_COMMENT R"(	# Algorithm switching switching during the iterations \
# INACTIVE: inactive \
# SWITCH_BEFORE_INV: switch algorithm before each inversion \
# SWITCH_WHEN_ENTER_PH2: switch algorithm immediately when entering phase-2 \
# SWITCH_BEFORE_INV_PH2: switch algorithm before each inversion in phase-2 \
# SWITCH_WHEN_NO_IMPR: switch algorithm if the actual (phase-1 of phase-2) objective value is the same between two inversions)"

#define GLOBAL_DEBUG_LEVEL_COMMENT R"(	# Level of iteration report \
# 0 = brief problem report    & solution only                   & only solution time \
# 1 = brief problem report    & one line per inversion          & only solution time \
# 2 = detailed problem report & one line per iteration          & brief time report\
# 3 = detailed problem report & one detailed line per iteration & detailed time report)"

#define GLOBAL_ITERATION_LIMIT_COMMENT "# Maximal number of iterations"
#define GLOBAL_TIME_LIMIT_COMMENT "# Time limit for a problem (sec)"
#define GLOBAL_SAVE_BASIS_COMMENT R"(	# Previously specified states of the solution algorithm can be saved. \
# After a filename is specified, there are four options. \
# The file format can be BAS (standard format) or PBF (PanOpt Basis Format) . \
# The last basis can be marked to be saved as `basis_filename`_last.`format` . \
# A basis of a given iteration can be marked to be saved or basis can be saved periodically \
# each output will follow the following syntax: `basis_filename`_`iteration_number`.`format` . \
# Both parameters are integers, 0 means saving is turned off.)"

#define GLOBAL_SAVE_BASIS_BASIS_COMMENT ""
#define GLOBAL_SAVE_BASIS_FILENAME_COMMENT ""
#define GLOBAL_SAVE_BASIS_FORMAT_COMMENT ""
#define GLOBAL_SAVE_BASIS_LAST_BASIS_COMMENT ""
#define GLOBAL_SAVE_BASIS_ITERATION_COMMENT ""
#define GLOBAL_SAVE_BASIS_PERIODICALLY_COMMENT ""

#define GLOBAL_LOAD_BASIS_COMMENT R"(# The solver can start from a given basis. \
# The file format can be BAS (standard format) or PBF (PanOpt Basis Format). )"

#define GLOBAL_LOAD_BASIS_BASIS_COMMENT ""
#define GLOBAL_LOAD_BASIS_FILENAME_COMMENT ""
#define GLOBAL_LOAD_BASIS_FORMAT_COMMENT ""

#define GLOBAL_EXPORT_COMMENT R"(	# The solver can export specific data in CSV format for research purposes. \
# The export type specifies the set of output to be written.\
# The export should be clean, it is appended with one line for each problem solved\
# The available export types are the following: \
# PARAMETER_STUDY = Parameter study (reporting numerical problems, tolerances and the solution) \
# RATIOTEST_STUDY = Ratiotest study (reporting measures of the ratio tests) )"

#define GLOBAL_EXPORT_ENABLE_COMMENT ""
#define GLOBAL_EXPORT_FILENAME_COMMENT ""
#define GLOBAL_EXPORT_TYPE_COMMENT ""

#define ENABLE_PARALLELIZATION_COMMENT ""

/********************************************
 * BLOCK COMMENTS
 * *****************************************/

const char * SIMPLEX_PARAMETERFILE_FIRST_COMMENT =
    R"(# Simplex parameter file for the Pannon Optimizer
# If this file is present, the values of the given parameters can be changed.)";

const char * SIMPLEX_PARAMETERS_PRICING_SIMPRI_COMMENT =
    R"(# Parameters of the SIMPRI partial pricing framework
# The range of vectors are divided into clusters during the pricing, which are
# scanned as a round robin in order to find improving candidates.
# With partial pricing it can be specified that only a certain number
# of clusters are visited to find improving candidates. If no candidates
# found, more clusters are involved until necessary.
# It is also possible to stop the cluster visit if enough candidates are found.
# Parameter valus 0 for improving candidates means this stop condition is turned off.
# All the three parameters can be specified both for phase I and phase II.)";

const char * SIMPLEX_PARAMETERS_RATIOTEST_EXPAND_COMMENT =
    "# Enable fake feasibility handling in Dual Phase I and II";

const char * SIMPLEX_PARAMETERS_GLOBAL_SAVEBASIS_COMMENT =
    R"(# Previously specified states of the solution algorithm can be saved.
# After a filename is specified, there are four options.
# The file format can be BAS (standard format) or PBF (PanOpt Basis Format) .
# The last basis can be marked to be saved as `basis_filename`_last.`format` .
# A basis of a given iteration can be marked to be saved or basis can be saved periodically
# each output will follow the following syntax: `basis_filename`_`iteration_number`.`format` .
# Both parameters are integers, 0 means saving is turned off.)";

const char * SIMPLEX_PARAMETERS_GLOBAL_LOADBASIS_COMMENT =
    R"(# The solver can start from a given basis.
# The file format can be BAS (standard format) or PBF (PanOpt Basis Format).)";

const char * SIMPLEX_PARAMETERS_GLOBAL_EXPORT_COMMENT =
    R"(# The solver can export specific data in CSV format for research purposes.
# The export type specifies the set of output to be written.
# The export should be clean, it is appended with one line for each problem solved
# The available export types are the following:
# PARAMETER_STUDY = Parameter study (reporting numerical problems, tolerances and the solution)
# RATIOTEST_STUDY = Ratiotest study (reporting measures of the ratio tests))";

const char * SIMPLEX_PARAMETERS_TOLERANCES_COMMENT = "# Tolerances #";
const char * SIMPLEX_PARAMETERS_STARTING_COMMENT = "# Starting procedures";
const char * SIMPLEX_PARAMETERS_STARTING_PRESOLVE_COMMENT = "# Presolve";
const char * SIMPLEX_PARAMETERS_STARTING_SCALING_COMMENT = "# Scaling parameters";
const char * SIMPLEX_PARAMETERS_STARTING_BASIS_COMMENT = "# Starting basis procedures";
const char * SIMPLEX_PARAMETERS_FACTORIZATION_COMMENT = "# Basis factorization";
const char * SIMPLEX_PARAMETERS_FACTORIZATION_PFI_COMMENT = "# PFI specific parameters";
const char * SIMPLEX_PARAMETERS_PRICING_COMMENT = "# Pricing";
const char * SIMPLEX_PARAMETERS_RATIOTEST_COMMENT = "# Ratiotest";

#endif // SIMPLEXPARAMETERCOMMENTS_H
