/**
 * @file simplex.cpp
 */

#include <simplex/simplex.h>
#include <utils/parameterhandler.h>
#include <utils/parameterhandler.h>
#include <utils/timer.h>
#include <utils/exceptions.h>

Simplex::Simplex():
    m_simplexModel(NULL),
    m_variableStates(0,0),
    m_variableFeasibilities(0,0),
    m_reducedCostFeasibilities(0,0)
{

}

Simplex::~Simplex()
{

}

//void Simplex::getVariablesByState(VARIABLE_STATE state, IndexList::Iterator *begin, IndexList::Iterator *end) const
//{

//}

//void Simplex::getVariablesByFeasibility(VARIABLE_STATE state, IndexList::Iterator *begin, IndexList::Iterator *end) const
//{

//}

//void Simplex::getReducedCostsByFeasibility(VARIABLE_STATE state, IndexList::Iterator *begin, IndexList::Iterator *end) const
//{

//}

void Simplex::setModel(const Model &model)
{
    if(m_simplexModel){
        delete m_simplexModel;
    }
    unregisterMethodWithModel(this, model);
    m_simplexModel = new SimplexModel(model);
    registerMethodWithModel(this, model);
}

void Simplex::constraintAdded()
{

}

void Simplex::variableAdded()
{

}

void Simplex::solve() {
    initModules();

    unsigned int iterationIndex;
    const unsigned int iterationLimit = 10000;
    const double timeLimit = 1000;
    Timer timer;

    try {

        for (iterationIndex = 0;
             iterationIndex < iterationLimit && timer.getTotalElapsed() < timeLimit;
             iterationIndex++) {
            timer.start();
            iterate();
            timer.stop();
        }

    } catch ( const OptimalException & exception ) {
        LPINFO("Optimal solution found!");
        // TODO: postsovle, post scaling
    } catch ( const InfeasibleException & exception ) {
        LPERROR("The problem is infeasible.");
    } catch ( const UnboundedException & exception ) {
        LPERROR("The problem is unbounded.");
    } catch ( const NumericalException & exception ) {
        LPERROR("Numerical error!");
    } catch ( const std::bad_alloc & exception ) {
        LPERROR("STL bad alloc exception: " << exception.what() );
    } catch ( const std::bad_cast & exception ) {
        LPERROR("STL bad cast exception: " << exception.what() );
    } catch ( const std::bad_exception & exception ) {
        LPERROR("STL bad exception exception: " << exception.what() );
    } catch ( const std::bad_typeid & exception ) {
        LPERROR("STL bad typeid exception: " << exception.what() );
    } catch (const std::ios_base::failure & exception ) {
        LPERROR("STL ios_base::failure exception: " << exception.what() );
    } catch (const std::logic_error & exception ) {
        LPERROR("STL logic error exception: " << exception.what() );
    } catch ( const std::range_error & exception ) {
        LPERROR("STL range error: " << exception.what() );
    } catch ( const std::overflow_error & exception ) {
        LPERROR("STL arithmetic overflow error: " << exception.what() );
    } catch ( const std::underflow_error & exception ) {
        LPERROR("STL arithmetic underflow error: " << exception.what() );
    } catch (const std::runtime_error & exception ) {
        LPERROR("STL runtime error: " << exception.what() );
    }
#if __cplusplus > 199711L
    catch ( const std::bad_weak_ptr & exception ) {
        LPERROR("STL bad weak pointer exception: " << exception.what() );
    }
    catch ( const std::bad_function_call & exception ) {
        LPERROR("STL bad function call exception: " << exception.what() );
    }
    catch ( const std::system_error & exception ) {
        //LPERROR("STL system error: \"" << exception.code().message() << "\" " << exception.what() );
        LPERROR("STL system error: " << std::endl);
        LPERROR("\tError: " << exception.what() << std::endl);
        LPERROR("\tCode: " << exception.code().value() << std::endl);
        LPERROR("\tCategory: " << exception.code().category().name() << std::endl);
        LPERROR("\tMessage: " << exception.code().message() << std::endl);
    }
#endif
    catch (const std::exception & exception) {
        LPERROR("General STL exception: " << exception.what());
    } catch (...) {
        LPERROR("Unknown exception");
    }

    releaseModules();
}
