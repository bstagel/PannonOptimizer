/**
 * @file startingbasisfinder.cpp
 */

#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <simplex/startingbasisfinder.h>
#include <simplex/startingbasisfinder/sbf_super.h>
#include <simplex/startingbasisfinder/sbf_logical.h>
#include <simplex/startingbasisfinder/sbf_structural.h>
//#include <simplex/startingbasisfinder/sbf_crash_adg.h>
//#include <simplex/startingbasisfinder/sbf_crash_ltsf.h>
#include <simplex/startingbasisfinder/sbf_crash.h>

#include <debug.h>

#include <simplex/simplexmodel.h>
#include <utils/numerical.h>
#include <utils/indexlist.h>

using namespace std;

typedef pair<int,int> intpair;

StartingBasisFinder::StartingBasisFinder(const SimplexModel& model,
                                         std::vector<int>* basisHead,
                                         IndexList<const Numerical::Double*>* variableStates) :
    m_model(model),
    m_basisHead(basisHead),
    m_variableStates(variableStates),
    m_algorithm(NULL)
{

}

StartingBasisFinder::~StartingBasisFinder()
{
    if(m_algorithm != NULL){
        delete m_algorithm;
        m_algorithm = 0;
    }
}

void StartingBasisFinder::print (int printLevel)
{
    if (printLevel & PRINT_STATISTIC ) {
        LPINFO("Starting basis found. Head contains " /*<<
                       m_basisStructVarCount << " structural variables"
                       << " ( / " <<  m_basisHead->size() << " ==> "
                       << (Numerical::Double)m_basisStructVarCount/m_basisHead->size()*100 << "% )"*/ );
    }

    if (printLevel & PRINT_DETAILED) {
        stringstream out;

        for (unsigned int i=0; i < m_basisHead->size(); i++) {
            out << (*m_basisHead)[i] << " ";
        }
        string basisHeadString = out.str();
        LPINFO("Starting basis found. Head contains the following variables: \n" << basisHeadString );
    }

    if (printLevel & PRINT_STRUCTURALS) {
        stringstream out;
        unsigned int basisSize = m_model.getRowCount();

        for (unsigned int i=0; i < m_basisHead->size(); i++) {
            if ((unsigned int) (*m_basisHead)[i] < basisSize) {
                out  << (*m_basisHead)[i] << " ";
            }
        }

        string basisHeadString = out.str();
        LPINFO("Starting basis found. Head contains the following structural variables: \n" << basisHeadString );
    }
}

void StartingBasisFinder::findStartingBasis(STARTING_BASIS_STRATEGY strategy, STARTING_NONBASIC_STATES nonbasicStates)
{
    if(m_algorithm != NULL){
        delete m_algorithm;
        m_algorithm = 0;
    }
    DEVINFO(D::STARTINGBASISFINDER, "Find starting basis: " << strategy);

    switch(strategy) {
    case STRUCTURAL:
        m_algorithm = new SbfStructural(m_model, m_basisHead, m_variableStates, nonbasicStates);
        m_algorithm->run();
        break;

    case LOGICAL:
        m_algorithm = new SbfLogical(m_model, m_basisHead, m_variableStates, nonbasicStates);
        m_algorithm->run();
        break;

    case SYMBOLIC_CRASH:
        m_algorithm = new SbfCrash(m_model, m_basisHead, m_variableStates, nonbasicStates);
        m_algorithm->run();
        break;

        //        case SYMBOLIC_CRASH:
        //            /* First step: logical basis*/
        //            m_algorithm = new SbfLogical(orig_problem, m_basisHead, m_variables, m_freeBasisPositions, m_freeStructVars, m_preferredStructVars);
        //            ((SbfLogical*)m_algorithm)->setLogicalStrategy(SbfLogical::MIXED_LOGICAL);
        //            m_algorithm->run();
        //            reconfigureFreeVariables();

        //            /* Second step: Symbolic crash */
        //            m_algorithm = new SbfCrashSymbolic(orig_problem, m_basisHead, m_variables, m_freeBasisPositions, m_freeStructVars, m_preferredStructVars);
        //            m_algorithm->run();
        //            m_basisStructVarCount = m_algorithm->getBasisStructVarCount();
        //            assert(m_algorithm->test(SbfSuper::TEST_UNIQUE+SbfSuper::TEST_TRINAGULARITY) == true);
        //            break;

        //        case LTSF_CRASH:
        //            /* First step: logical basis*/
        //            DEVINFO(D::STARTINGBASISFINDER, "Run logical...");
        //            m_algorithm = new SbfLogical(orig_problem, m_basisHead, m_variables, m_freeBasisPositions, m_freeStructVars, m_preferredStructVars);
        //            ((SbfLogical*)m_algorithm)->setLogicalStrategy(SbfLogical::MIXED_LOGICAL);
        //            m_algorithm->run();
        //            DEVINFO(D::STARTINGBASISFINDER, "Reconfigure variables...");
        //            reconfigureFreeVariables();

        //            /* Second step: LTSF crash */
        //            DEVINFO(D::STARTINGBASISFINDER, "Run LTSF...");
        //            m_algorithm = new SbfCrashLtsf(orig_problem, m_basisHead, m_variables, m_freeBasisPositions, m_freeStructVars, m_preferredStructVars);
        //            m_algorithm->run();
        //            m_basisStructVarCount = m_algorithm->getBasisStructVarCount();
        //            assert(m_algorithm->test(SbfSuper::TEST_UNIQUE+SbfSuper::TEST_TRINAGULARITY) == true);
        //            break;

        //        case ADG_CRASH:
        //            /* First step: logical basis*/
        //            DEVINFO(D::STARTINGBASISFINDER, "Run logical...");
        //            m_algorithm = new SbfLogical(orig_problem, m_basisHead, m_variables, m_freeBasisPositions, m_freeStructVars, m_preferredStructVars);
        //            ((SbfLogical*)m_algorithm)->setLogicalStrategy(SbfLogical::MIXED_LOGICAL);
        //            m_algorithm->run();
        //            DEVINFO(D::STARTINGBASISFINDER, "Reconfigure variables...");
        //            reconfigureFreeVariables();

        //            /* Second step: ADG crash */
        //            DEVINFO(D::STARTINGBASISFINDER, "Run ADG...");
        //            m_algorithm = new SbfCrashAdg(orig_problem, m_basisHead, m_variables, m_freeBasisPositions, m_freeStructVars, m_preferredStructVars);
        //            ((SbfCrashAdg*)m_algorithm)->setAdgStrategy(SbfCrashAdg::ROW_COUNT);
        //            m_algorithm->run();
        //            m_basisStructVarCount = m_algorithm->getBasisStructVarCount();
        //            assert(m_algorithm->test(SbfSuper::TEST_UNIQUE+SbfSuper::TEST_TRINAGULARITY) == true);
        //            break;

        //        case ADG_LTSF_CRASH:
        //            /* First step: logical basis*/
        //            DEVINFO(D::STARTINGBASISFINDER, "Run logical...");
        //            m_algorithm = new SbfLogical(orig_problem, m_basisHead, m_variables, m_freeBasisPositions, m_freeStructVars, m_preferredStructVars);
        //            ((SbfLogical*)m_algorithm)->setLogicalStrategy(SbfLogical::MIXED_LOGICAL);
        //            m_algorithm->run();
        //            reconfigureFreeVariables();

        //            /* Second step: ADG crash */
        //            DEVINFO(D::STARTINGBASISFINDER, "Run ADG...");
        //            m_algorithm = new SbfCrashAdg(orig_problem, m_basisHead, m_variables, m_freeBasisPositions, m_freeStructVars, m_preferredStructVars);
        //            ((SbfCrashAdg*)m_algorithm)->setAdgStrategy(SbfCrashAdg::ROW_COUNT);
        //            m_algorithm->run();
        //            m_basisStructVarCount = m_algorithm->getBasisStructVarCount();
        //            assert(m_algorithm->test(SbfSuper::TEST_UNIQUE+SbfSuper::TEST_TRINAGULARITY) == true);
        //            reconfigureFreeVariables();

        //            /* Third step: LTSF crash */
        //            DEVINFO(D::STARTINGBASISFINDER, "Run LTSF...");
        //            m_algorithm = new SbfCrashLtsf(orig_problem, m_basisHead, m_variables, m_freeBasisPositions, m_freeStructVars, m_preferredStructVars);
        //            m_algorithm->run();
        //            m_basisStructVarCount = m_algorithm->getBasisStructVarCount();
        //            assert(m_algorithm->test(SbfSuper::TEST_UNIQUE+SbfSuper::TEST_TRINAGULARITY) == true);

        //            break;

    default:
        throw ParameterException("Invalid basis finder algorithm selected");
        break;
    }
}

