/**
 * @file simplexparameterhandler.cpp
 */

#include <simplex/simplexparameterhandler.h>
#include <simplex/simplexparametercomments.h>

#include <fstream>
#include <iostream>
#include <thread>

#include <defaultparameters.h>
#include <initpanopt.h>

#include <utils/parameterfilebuilder.h>

thread_local const char * SimplexParameterHandler::sm_defaultFilename = "simplex.PAR";

SimplexParameterHandler * SimplexParameterHandler::sm_instance = 0;

SimplexParameterHandler::SimplexParameterHandler()
{
    m_filename = sm_defaultFilename;
    m_enableParallelization = false;
    m_getParallelOptions = false;
}

ParameterHandler& SimplexParameterHandler::getInstance()
{
    try{
        return *sm_instance;
    } catch (const ParameterException & exception) {
        LPERROR( "Simplex ParameterException: " <<exception.getMessage() );
        exit(-1);
    }
}

void SimplexParameterHandler::_globalInit() {
    sm_instance = new SimplexParameterHandler;
    sm_instance->initParameters();
    sm_instance->readParameterFile(sm_defaultFilename);
}

void SimplexParameterHandler::_globalRelease()
{
    delete sm_instance;
    sm_instance = nullptr;
}

void SimplexParameterHandler::writeParameterFile(){
    try {
        std::ofstream out;
        out.open(m_filename, std::ios::out | std::ios::binary);

        if (!out.is_open()) throw -1;

        ParameterFileBuilder root;

        for (auto & iter: m_valueNames) {
            root.addNewParameter(getParameter(iter));
        }
        root.addComment("", SIMPLEX_PARAMETERFILE_FIRST_COMMENT);
        root.addComment("Tolerances", SIMPLEX_PARAMETERS_TOLERANCES_COMMENT);
        root.addComment("Starting", SIMPLEX_PARAMETERS_STARTING_COMMENT);
        root.addComment("Starting.Presolve", SIMPLEX_PARAMETERS_STARTING_PRESOLVE_COMMENT);
        root.addComment("Starting.Scaling", SIMPLEX_PARAMETERS_STARTING_SCALING_COMMENT);
        root.addComment("Starting.Basis", SIMPLEX_PARAMETERS_STARTING_BASIS_COMMENT);
        root.addComment("Factorization", SIMPLEX_PARAMETERS_FACTORIZATION_COMMENT);
        root.addComment("Factorization.PFI", SIMPLEX_PARAMETERS_FACTORIZATION_PFI_COMMENT);
        root.addComment("Pricing", SIMPLEX_PARAMETERS_PRICING_COMMENT);
        root.addComment("Pricing.Simpri", SIMPLEX_PARAMETERS_PRICING_SIMPRI_COMMENT);
        root.addComment("Ratiotest", SIMPLEX_PARAMETERS_RATIOTEST_COMMENT);
        root.addComment("Ratiotest.Expand", SIMPLEX_PARAMETERS_RATIOTEST_EXPAND_COMMENT);
        root.addComment("Global.SaveBasis", SIMPLEX_PARAMETERS_GLOBAL_SAVEBASIS_COMMENT);
        root.addComment("Global.LoadBasis", SIMPLEX_PARAMETERS_GLOBAL_LOADBASIS_COMMENT);
        root.addComment("Globa.Export", SIMPLEX_PARAMETERS_GLOBAL_EXPORT_COMMENT);

        // TODO:
        // - atrakni az egeszet a parameterhandler-be
        root.writeToStream(out);
        out.close();
    }
    catch(int) {
        std::cerr << "Parameter file can not be written: " << m_filename;
    }
}

void SimplexParameterHandler::initParameters()
{
    //Tolerances
    createParameter("Tolerances.e_pivot",
                    Entry::DOUBLE,
                    TOLERANCES_PIVOT_COMMENT);
    setParameterValue("Tolerances.e_pivot",
                      DefaultParameters::Tolerances::E_PIVOT);

    createParameter("Tolerances.e_feasibility",
                    Entry::DOUBLE,
                    TOLERANCES_E_FEASIBILITY_COMMENT);
    setParameterValue("Tolerances.e_feasibility",
                      DefaultParameters::Tolerances::E_FEASIBILITY);

    createParameter("Tolerances.e_optimality",
                    Entry::DOUBLE,
                    TOLERANCES_E_OPTIMALITY_COMMENT);
    setParameterValue("Tolerances.e_optimality",
                      DefaultParameters::Tolerances::E_OPTIMALITY);

    //Starting procedures
    createParameter("Starting.Presolve.enable",
                    Entry::BOOL, STARTING_PRESOLVE_ENABLE_COMMENT);
    setParameterValue("Starting.Presolve.enable",
                      DefaultParameters::Starting::Presolve::ENABLE);

    createParameter("Starting.Scaling.enable",
                    Entry::BOOL,
                    STARTING_SCALING_ENABLE_COMMENT);
    setParameterValue("Starting.Scaling.enable",
                      DefaultParameters::Starting::Scaling::ENABLE);

    createParameter("Starting.Scaling.type",
                    Entry::STRING,
                    STARTING_SCALING_TYPE_COMMENT);
    setParameterValue("Starting.Scaling.type",
                      DefaultParameters::Starting::Scaling::TYPE);

    createParameter("Starting.Basis.starting_nonbasic_states",
                    Entry::STRING,
                    STARTING_BASIS_STARTING_NONBASIC_STATES_COMMENT);
    setParameterValue("Starting.Basis.starting_nonbasic_states", DefaultParameters::Starting::Basis::STARTING_NONBASIC_STATES);

    createParameter("Starting.Basis.starting_basis_strategy",
                    Entry::STRING,
                    STARTING_BASIS_STARTING_BASIS_STRATEGY_COMMENT);
    setParameterValue("Starting.Basis.starting_basis_strategy",
                      DefaultParameters::Starting::Basis::STARTING_BASIS_STRATEGY);

    //Basis factorization
    createParameter("Factorization.type",
                    Entry::STRING,
                    FACTORIZATION_TYPE_COMMENT);
    setParameterValue("Factorization.type",
                      DefaultParameters::Factorization::TYPE);

    createParameter("Factorization.reinversion_frequency",
                    Entry::INTEGER,
                    FACTORIZATION_REINVERSION_FREQUENCY_COMMENT);
    setParameterValue("Factorization.reinversion_frequency",
                      DefaultParameters::Factorization::REINVERSION_FREQUENCY);

    createParameter("Factorization.pivot_threshold",
                    Entry::DOUBLE,
                    FACTORIZATION_PIVOT_THRESHOLD_COMMENT);
    setParameterValue("Factorization.pivot_threshold",
                      DefaultParameters::Factorization::PIVOT_THRESHOLD);

    createParameter("Factorization.PFI.nontriangular_method",
                    Entry::STRING,
                    FACTORIZATION_PFI_NONTRIANGULAR_METHOD_COMMENT);
    setParameterValue("Factorization.PFI.nontriangular_method",
                      DefaultParameters::Factorization::PFI::NONTRIANGULAR_METHOD);

    createParameter("Factorization.PFI.nontriangular_pivot_rule",
                    Entry::STRING,
                    FACTORIZATION_PFI_NONTRIANGULAR_PIVOT_RULE_COMMENT);
    setParameterValue("Factorization.PFI.nontriangular_pivot_rule",
                      DefaultParameters::Factorization::PFI::NONTRIANGULAR_PIVOT_RULE);

    //Pricing
    createParameter("Pricing.type",
                    Entry::STRING,
                    PRICING_TYPE_COMMENT);
    setParameterValue("Pricing.type",
                      DefaultParameters::Pricing::TYPE);

    createParameter("Pricing.Simpri.phaseI_clusters",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEI_CLUSTERS_COMMENT);
    setParameterValue("Pricing.Simpri.phaseI_clusters",
                      DefaultParameters::Pricing::Simpri::PHASEI_CLUSTERS);

    createParameter("Pricing.Simpri.phaseI_visit_clusters",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEI_VISIT_CLUSTERS_COMMENT);
    setParameterValue("Pricing.Simpri.phaseI_visit_clusters",
                      DefaultParameters::Pricing::Simpri::PHASEI_VISIT_CLUSTERS);

    createParameter("Pricing.Simpri.phaseI_improving_candidates",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEI_IMPROVING_CANDIDADES_COMMENT);
    setParameterValue("Pricing.Simpri.phaseI_improving_candidates",
                      DefaultParameters::Pricing::Simpri::PHASEI_IMPROVING_CANDIDATES);

    createParameter("Pricing.Simpri.phaseII_clusters",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEII_CLUSTERS_COMMENT);
    setParameterValue("Pricing.Simpri.phaseII_clusters",
                      DefaultParameters::Pricing::Simpri::PHASEII_CLUSTERS);

    createParameter("Pricing.Simpri.phaseII_visit_clusters",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEII_VISIT_CLUSTERS_COMMENT);
    setParameterValue("Pricing.Simpri.phaseII_visit_clusters",
                      DefaultParameters::Pricing::Simpri::PHASEII_VISIT_CLUSTERS);

    createParameter("Pricing.Simpri.phaseII_improving_candidates",
                    Entry::INTEGER,
                    PRICING_SIMPRI_PHASEII_IMPROVING_CANDIDADES_COMMENT);
    setParameterValue("Pricing.Simpri.phaseII_improving_candidates",
                      DefaultParameters::Pricing::Simpri::PHASEII_IMPROVING_CANDIDATES);

    //Ratiotest
    createParameter("Ratiotest.nonlinear_primal_phaseI_function",
                    Entry::STRING,
                    RATIOTEST_NONLINEAR_PRIMAL_PHASEI_FUNCTION_COMMENT);
    setParameterValue("Ratiotest.nonlinear_primal_phaseI_function",
                      DefaultParameters::Ratiotest::NONLINEAR_PRIMAL_PHASEI_FUNCTION);

    createParameter("Ratiotest.nonlinear_dual_phaseI_function",
                    Entry::STRING,
                    RATIOTEST_NONLINEAR_DUAL_PHASEI_FUNCTION_COMMENT);
    setParameterValue("Ratiotest.nonlinear_dual_phaseI_function",
                      DefaultParameters::Ratiotest::NONLINEAR_DUAL_PHASEI_FUNCTION);

    createParameter("Ratiotest.nonlinear_dual_phaseII_function",
                    Entry::STRING,
                    RATIOTEST_NONLINEAR_DUAL_PHASEII_FUNCTION_COMMENT);
    setParameterValue("Ratiotest.nonlinear_dual_phaseII_function",
                      DefaultParameters::Ratiotest::NONLINEAR_DUAL_PHASEII_FUNCTION);

    createParameter("Ratiotest.enable_fake_feasibility",
                    Entry::BOOL,
                    RATIOTEST_ENABLE_FAKE_FEASIBILITY_COMMENT);
    setParameterValue("Ratiotest.enable_fake_feasibility",
                      DefaultParameters::Ratiotest::ENABLE_FAKE_FEASIBILITY);

    createParameter("Ratiotest.Expand.multiplier",
                    Entry::DOUBLE,
                    RATIOTEST_EXPAND_MULTIPLIER_COMMENT);
    setParameterValue("Ratiotest.Expand.multiplier",
                      DefaultParameters::Ratiotest::Expand::MULTIPLIER);

    createParameter("Ratiotest.Expand.divider",
                    Entry::INTEGER,
                    RATIOTEST_EXPAND_DIVIDER_COMMENT);
    setParameterValue("Ratiotest.Expand.divider",
                      DefaultParameters::Ratiotest::Expand::DIVIDER);

    createParameter("Ratiotest.Expand.type",
                    Entry::STRING,
                    RATIOTEST_EXPAND_TYPE_COMMENT);
    setParameterValue("Ratiotest.Expand.type",
                      DefaultParameters::Ratiotest::Expand::TYPE);

    //Perturbation
    createParameter("Perturbation.perturb_cost_vector",
                    Entry::STRING,
                    PERTURBATION_PERTURB_COST_VECTOR_COMMENT);
    setParameterValue("Perturbation.perturb_cost_vector",
                      DefaultParameters::Perturbation::PERTURB_COST_VECTOR);

    createParameter("Perturbation.perturb_target",
                    Entry::STRING,
                    PERTURBATION_PERTURB_TARGET_COMMENT);
    setParameterValue("Perturbation.perturb_target",
                      DefaultParameters::Perturbation::PERTURB_TARGET);

    createParameter("Perturbation.perturb_logical",
                    Entry::BOOL,
                    PERTURBATION_PERTURB_LOGICAL_COMMENT);
    setParameterValue("Perturbation.perturb_logical",
                      DefaultParameters::Perturbation::PERTURB_LOGICAL);

    createParameter("Perturbation.e_cost_vector",
                    Entry::DOUBLE,
                    PERTURBATION_E_COST_VECTOR_COMMENT);
    setParameterValue("Perturbation.e_cost_vector",
                      DefaultParameters::Perturbation::E_COST_VECTOR);

    createParameter("Perturbation.xi_multiplier",
                    Entry::DOUBLE,
                    PERTURBATION_XI_MULTIPLIER_COMMENT);
    setParameterValue("Perturbation.xi_multiplier",
                      DefaultParameters::Perturbation::XI_MULTIPLIER);

    createParameter("Perturbation.weighting",
                    Entry::STRING,
                    PERTURBATION_WEIGHTING_COMMENT);
    setParameterValue("Perturbation.weighting",
                      DefaultParameters::Perturbation::WEIGHTING);

    createParameter("Perturbation.psi",
                    Entry::DOUBLE,
                    PERTURBATION_PSI_COMMENT);
    setParameterValue("Perturbation.psi",
                      DefaultParameters::Perturbation::PSI);

    createParameter("Perturbation.perturb_rhs",
                    Entry::BOOL,
                    PERTURBATION_PERTURB_RHS_COMMENT);
    setParameterValue("Perturbation.perturb_rhs",
                      DefaultParameters::Perturbation::PERTURB_RHS);

    createParameter("Perturbation.e_rhs",
                    Entry::DOUBLE,
                    PERTURBATION_E_RHS_COMMENT);
    setParameterValue("Perturbation.e_rhs",
                      DefaultParameters::Perturbation::E_RHS);

    createParameter("Perturbation.shift_bounds",
                    Entry::BOOL,
                    PERTURBATION_SHIFT_BOUNDS_COMMENT);
    setParameterValue("Perturbation.shift_bounds",
                      DefaultParameters::Perturbation::SHIFT_BOUNDS);

    createParameter("Perturbation.e_bounds",
                    Entry::DOUBLE,
                    PERTURBATION_E_BOUNDS_COMMENT);
    setParameterValue("Perturbation.e_bounds",
                      DefaultParameters::Perturbation::E_BOUNDS);

    //Global
    createParameter("Global.starting_algorithm",
                    Entry::STRING,
                    GLOBAL_STARTING_ALGORITHM_COMMENT);
    setParameterValue("Global.starting_algorithm",
                      DefaultParameters::Global::STARTING_ALGORITHM);

    createParameter("Global.switch_algorithm",
                    Entry::STRING,
                    GLOBAL_SWITCH_ALGORITHM_COMMENT);
    setParameterValue("Global.switch_algorithm",
                      DefaultParameters::Global::SWITCH_ALGORITHM);

    createParameter("Global.debug_level",
                    Entry::INTEGER,
                    GLOBAL_DEBUG_LEVEL_COMMENT);
    setParameterValue("Global.debug_level",
                      DefaultParameters::Global::DEBUG_LEVEL);

    createParameter("Global.iteration_limit",
                    Entry::INTEGER,
                    GLOBAL_ITERATION_LIMIT_COMMENT);
    setParameterValue("Global.iteration_limit",
                      DefaultParameters::Global::ITERATION_LIMIT);

    createParameter("Global.time_limit",
                    Entry::DOUBLE,
                    GLOBAL_TIME_LIMIT_COMMENT);
    setParameterValue("Global.time_limit",
                      DefaultParameters::Global::TIME_LIMIT);

    createParameter("Global.SaveBasis.basis",
                    Entry::BOOL,
                    GLOBAL_SAVE_BASIS_BASIS_COMMENT);
    setParameterValue("Global.SaveBasis.basis",
                      DefaultParameters::Global::SaveBasis::BASIS);

    createParameter("Global.SaveBasis.filename",
                    Entry::STRING,
                    GLOBAL_SAVE_BASIS_FILENAME_COMMENT);
    setParameterValue("Global.SaveBasis.filename",
                      std::string(DefaultParameters::Global::SaveBasis::FILENAME));

    createParameter("Global.SaveBasis.format",
                    Entry::STRING,
                    GLOBAL_SAVE_BASIS_FORMAT_COMMENT);
    setParameterValue("Global.SaveBasis.format",
                      std::string(DefaultParameters::Global::SaveBasis::FORMAT));

    createParameter("Global.SaveBasis.last_basis",
                    Entry::BOOL,
                    GLOBAL_SAVE_BASIS_LAST_BASIS_COMMENT);
    setParameterValue("Global.SaveBasis.last_basis",
                      DefaultParameters::Global::SaveBasis::LAST_BASIS);

    createParameter("Global.SaveBasis.iteration",
                    Entry::INTEGER,
                    GLOBAL_SAVE_BASIS_ITERATION_COMMENT);
    setParameterValue("Global.SaveBasis.iteration",
                      DefaultParameters::Global::SaveBasis::ITERATION);

    createParameter("Global.SaveBasis.periodically",
                    Entry::INTEGER,
                    GLOBAL_SAVE_BASIS_PERIODICALLY_COMMENT);
    setParameterValue("Global.SaveBasis.periodically",
                      DefaultParameters::Global::SaveBasis::PERIODICALLY);

    createParameter("Global.LoadBasis.basis",
                    Entry::BOOL,
                    GLOBAL_LOAD_BASIS_BASIS_COMMENT);
    setParameterValue("Global.LoadBasis.basis",
                      DefaultParameters::Global::LoadBasis::BASIS);

    createParameter("Global.LoadBasis.filename",
                    Entry::STRING,
                    GLOBAL_LOAD_BASIS_FILENAME_COMMENT);
    setParameterValue("Global.LoadBasis.filename",
                      std::string(DefaultParameters::Global::LoadBasis::FILENAME));

    createParameter("Global.LoadBasis.format",
                    Entry::STRING,
                    GLOBAL_LOAD_BASIS_FORMAT_COMMENT);
    setParameterValue("Global.LoadBasis.format",
                      std::string(DefaultParameters::Global::LoadBasis::FORMAT));

    createParameter("Global.Export.enable",
                    Entry::BOOL,
                    GLOBAL_EXPORT_ENABLE_COMMENT);
    setParameterValue("Global.Export.enable",
                      DefaultParameters::Global::Export::ENABLE);

    createParameter("Global.Export.filename",
                    Entry::STRING,
                    GLOBAL_EXPORT_FILENAME_COMMENT);
    setParameterValue("Global.Export.filename",
                      std::string(DefaultParameters::Global::Export::FILENAME));

    createParameter("Global.Export.type",
                    Entry::STRING,
                    GLOBAL_EXPORT_TYPE_COMMENT);
    setParameterValue("Global.Export.type",
                      DefaultParameters::Global::Export::TYPE);

    createParameter("enableParallelization",
                    Entry::BOOL,
                    ENABLE_PARALLELIZATION_COMMENT);
    setParameterValue("enableParallelization",
                      DefaultParameters::ENABLE_PARALLELIZATION);


}

void SimplexParameterHandler::processParallelNode(const NodeFile::Node &node,
                                                  const std::string &name,
                                                  std::map<std::string, Parameter> *threadParameters)
{
    std::set<std::string> valueNames = node.getValueNames();
    auto valueIter = valueNames.begin();
    auto valueIterEnd = valueNames.end();
    for (; valueIter != valueIterEnd; valueIter++) {
        std::string parameterName = name + *valueIter;
        std::string value = node.getValue(*valueIter);
        //LPINFO("parameter: " << parameterName << " = " << value);
        Parameter parameter = m_values.at( parameterName );
        Entry::ENTRY_TYPE type = parameter.getEntryType();
        switch (type) {
        case Entry::BOOL:
            //LPINFO("\tBOOL");
            if (value == "true") {
                parameter.setBoolValue(true);
            } else if (value == "false") {
                parameter.setBoolValue(false);
            } else {
                throw ParameterException(std::string("Parameter error in parameter file: ").append(parameterName));
            }
            break;
        case Entry::DOUBLE:
            parameter.setDoubleValue( atof(value.c_str()) );
            //LPINFO("\tDOUBLE");
            break;
        case Entry::INTEGER:
            parameter.setIntegerValue( atoi(value.c_str()) );
            //LPINFO("\tINTEGER");
            break;
        case Entry::STRING:
            if (value[0] == '\"' && value[ value.length() - 1 ] == '\"') {
                value = value.substr(1, value.length() - 2);
            }
            parameter.setStringValue(value);
            //LPINFO("\tSTRING");
            break;
        }
        threadParameters->insert(std::make_pair(parameterName, parameter));

    }

    std::set<std::string> names = node.getNodeNames();
    auto nameIter = names.begin();
    auto nameIterEnd = names.end();
    for (; nameIter != nameIterEnd; nameIter++) {
        //LPINFO(*nameIter);
        std::vector<NodeFile::Node>::const_iterator nodeIter;
        std::vector<NodeFile::Node>::const_iterator nodeIterEnd;
        node.getNodes(*nameIter, &nodeIter, &nodeIterEnd);
        for (; nodeIter != nodeIterEnd; nodeIter++) {
            processParallelNode(*nodeIter, *nameIter + ".", threadParameters);
        }

    }
}

const Parameter &SimplexParameterHandler::getParameter(const std::string &name) const
{
    if (m_enableParallelization) {
        unsigned int threadId = ThreadSupervisor::getThreadId();
        auto iter = m_threadParameters[threadId].find(name);
        if (iter == m_threadParameters[threadId].end()) {
            return m_values.find(name)->second;
        }
        return iter->second;

    } else {
        auto iter = m_values.find(name);
        return iter->second;
    }
}

Parameter &SimplexParameterHandler::getParameter(const std::string & name)
{
    if (m_enableParallelization) {
        unsigned int threadId = ThreadSupervisor::getThreadId();
        auto iter = m_threadParameters[threadId].find(name);
        if (iter == m_threadParameters[threadId].end()) {
            return m_values.find(name)->second;
        }
        return iter->second;

    } else {
        auto iter = m_values.find(name);
        return iter->second;
    }}

void SimplexParameterHandler::loadValuesFromFile(std::ifstream &in)
{
    ParameterHandler::loadValuesFromFile(in);

    // process parallel options
    NodeFile::Node root = m_nodeFile.getDocumentNode();
    m_enableParallelization = false;
    try {
        m_enableParallelization = root.getValue("enableParallelization") == "true";
        std::vector<NodeFile::Node>::const_iterator nodeIter;
        std::vector<NodeFile::Node>::const_iterator nodeIterEnd;
        root.getNodes("Parallel", &nodeIter, &nodeIterEnd);

        std::vector<NodeFile::Node>::const_iterator threadIter;
        std::vector<NodeFile::Node>::const_iterator threadIterEnd;

        nodeIter->getNodes("Thread", &threadIter, &threadIterEnd);
        for (; threadIter != threadIterEnd; threadIter++) {
            //LPWARNING(threadIter->getName());
            std::map<std::string, Parameter> threadParameters;
            processParallelNode(*threadIter, "", &threadParameters);
            m_threadParameters.push_back(threadParameters);
        }
    } catch (...) {}

}

void SimplexParameterHandler::enableParallelOptions(bool value)
{
    m_enableParallelization = value;
}

const std::string &SimplexParameterHandler::getStringParameterValue(const std::string &name) const {
    const Parameter & parameter = getParameter(name);
    return *(parameter.getEntry().m_string);
}

const int &SimplexParameterHandler::getIntegerParameterValue(const std::string &name) const {
    const Parameter & parameter = getParameter(name);
    return parameter.getEntry().m_integer;
}

const double &SimplexParameterHandler::getDoubleParameterValue(const std::string &name) const {
    const Parameter & parameter = getParameter(name);
    return parameter.getEntry().m_double;
}

const bool &SimplexParameterHandler::getBoolParameterValue(const std::string &name) const {
    const Parameter & parameter = getParameter(name);
    return parameter.getEntry().m_bool;
}

void SimplexParameterHandler::setParameterValue(const std::string name,
                                                const double value){
    m_values.at(name).setDoubleValue(value);
}

void SimplexParameterHandler::setParameterValue(const std::string name,
                                                const int value){
    m_values.at(name).setIntegerValue(value);
}

void SimplexParameterHandler::setParameterValue(const std::string name,
                                                const std::string value){
    m_values.at(name).setStringValue(value);
}

void SimplexParameterHandler::setParameterValue(const std::string name,
                                                const char *value){
    m_values.at(name).setStringValue(std::string(value));
}

void SimplexParameterHandler::setParameterValue(const std::string name,
                                                const bool value){
    m_values.at(name).setBoolValue(value);
}
