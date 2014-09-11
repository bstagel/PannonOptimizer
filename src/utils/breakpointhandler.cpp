/**
 * @file breakpointhandler.cpp
 */
#include <utils/breakpointhandler.h>
#include <simplex/simplexparameterhandler.h>

#include <utils/thirdparty/prettyprint.h>

BreakpointHandler::BreakpointHandler():
    m_sortingMethod(BreakpointHandler::SELECTION),
    m_unsorted(m_breakpoints.size())
{

}

//void BreakpointHandler::insertBreakpoint(int variableIndex,
//                                         Numerical::Double value,
//                                         Numerical::Double expandedValue)
//{
//    m_breakpoints.emplace_back(variableIndex,value,expandedValue,-Numerical::Infinity);
//}

const BreakpointHandler::BreakPoint* BreakpointHandler::getBreakpoint(int index)
{
    int size = m_breakpoints.size();

#ifndef NDEBUG
    if (index >= size){
        LPERROR("Invalid index in getBreakpoint() "<<index);
        exit(-1);
    }
#endif

//    checkHeap();
    if (m_sortingMethod == BreakpointHandler::HEAP){
        //breakpoint already sorted
        if (index < size-m_unsorted){
            return &m_breakpoints[size-1-index];
        //sort next breakpoints
        } else{
            for(int i = size-m_unsorted; i<=index; i++){
                m_unsorted--;
                swapBreakpoints(0,m_unsorted);
                heapify(0);
            }
        }
    }
//    checkHeap();
    return &m_breakpoints[size-1-index];
}

unsigned BreakpointHandler::getNumberOfBreakpoints() const
{
    return m_breakpoints.size();
}

void BreakpointHandler::printBreakpoints() const
{
    LPINFO(m_breakpoints);
}

void BreakpointHandler::initSorting()
{
    //currently heapsort is always selected
    m_unsorted = m_breakpoints.size();
    if ( m_breakpoints.size() < 1){
        m_sortingMethod = SELECTION;
        selectionSort();
    } else{
        m_sortingMethod = HEAP;
        buildHeap();
    }
}

void BreakpointHandler::init(unsigned maxNumberOfBreakpoints)
{
    m_breakpoints.clear();
    m_breakpoints.reserve(maxNumberOfBreakpoints);
}

const std::vector<const BreakpointHandler::BreakPoint*> &BreakpointHandler::getExpandSecondPass()
{
    //reserve the maximal possible number: total number of breakpoints
    m_secondPassRatios.clear();
    m_secondPassRatios.reserve(m_breakpoints.size());
    const BreakPoint * breakpoint = NULL;

    Numerical::Double theta_1 = Numerical::Infinity;

    for(int i = m_unsorted; i >= 0; i--) {
        if(m_breakpoints[i].expandedValue < theta_1){
            theta_1 = m_breakpoints[i].expandedValue;
        }
    }

    if(theta_1 < 0){
        throw FallbackException(std::string("Expanded value negative"));
    }

    for(unsigned i = m_breakpoints.size() - (m_unsorted+1); i < m_breakpoints.size(); i++){
        breakpoint = getBreakpoint(i);
        //actual values smaller than the theta_1
        if(breakpoint->value <= theta_1){
            m_secondPassRatios.push_back(breakpoint);
        //breakpoints come sorted, breaking out at first bigger value
        }else{
            break;
        }
    }
    return m_secondPassRatios;
}

void BreakpointHandler::selectionSort()
{
    int maxId = 0;

    //sort actual values
    for(unsigned i=0; i < m_breakpoints.size()-1; i++){
        maxId = i;
        for(unsigned j=i+1; j < m_breakpoints.size(); j++){
            if (m_breakpoints[j].value > m_breakpoints[maxId].value){
                maxId = j;
            }
        }

        swapBreakpoints(maxId,i);
    }

    m_unsorted = 0;
}

void BreakpointHandler::heapify(int actual)
{
    int left = 2 * actual + 1;
    int right = left+1;
    int smallest = actual;

    //choosing smallest element (actual value)
    if (left < m_unsorted && (m_breakpoints[left].value < m_breakpoints[smallest].value) ) {
        smallest = left;
    }
    if (right < m_unsorted && (m_breakpoints[right].value < m_breakpoints[smallest].value) ) {
        smallest = right;
    }

    //put smallest to actual, recursive call
    if (smallest != actual){
        swapBreakpoints(actual,smallest);
        heapify(smallest);
    }
}

void BreakpointHandler::buildHeap()
{
    for(int i = m_breakpoints.size()/2; i > 0; i--){
        heapify(i-1);
    }
}

void BreakpointHandler::checkHeap(){
    Numerical::Double ref = 0;

    //check actual Values
    ref = m_breakpoints[0].value;
    for(int i=0; i<m_unsorted; i++){
        if(m_breakpoints[i].value < ref){
            LPINFO("BAD HEAP: ");
            LPINFO("ref: "<<ref);
            LPINFO("val: "<<m_breakpoints[i].value);
            LPINFO("unsorted: "<<m_unsorted);
            printBreakpoints();
            exit(-1);
        }
    }
    for(unsigned int i=m_unsorted; i<m_breakpoints.size(); i++){
        if(ref < m_breakpoints[i].value){
            LPINFO("BAD SORT:");
            LPINFO("ref: "<<ref);
            LPINFO("val: "<<m_breakpoints[i].value);
            LPINFO("unsorted: "<<m_unsorted);
            printBreakpoints();
            exit(-1);
        }
    }
}
