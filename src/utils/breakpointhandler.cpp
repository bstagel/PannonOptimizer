/**
 * @file breakpointhandler.cpp
 */
#include <utils/breakpointhandler.h>
#include <simplex/simplexparameterhandler.h>

#include <utils/thirdparty/prettyprint.h>

BreakpointHandler::BreakpointHandler():
    m_sortingMethod(BreakpointHandler::SELECTION),
    m_unsorted(0),
    m_size(0)
{

}

const BreakpointHandler::BreakPoint* BreakpointHandler::getBreakpoint(unsigned index)
{
#ifndef NDEBUG
    if (index >= m_size){
        LPERROR("Invalid index in getBreakpoint() "<<index);
        exit(-1);
    }
#endif

//    checkHeap();
    if (m_sortingMethod == BreakpointHandler::HEAP){
        //breakpoint already sorted
        if (index < m_size-m_unsorted){
            return &m_breakpoints[m_size-1-index];
        //sort next breakpoints
        } else{
            for(unsigned i = m_size-m_unsorted; i<=index; i++){
                m_unsorted--;
                swapBreakpoints(0,m_unsorted);
                heapify(0);
            }
        }
    }
//    printBreakpoints();
//    LPINFO("_breakpoints[size-1-index]: "<<m_breakpoints[size-1-index].variableIndex);
//    checkHeap();
    return &m_breakpoints[m_size-1-index];
}

unsigned BreakpointHandler::getNumberOfBreakpoints() const
{
    return m_size;
}

void BreakpointHandler::printBreakpoints() const
{
    LPINFO(m_breakpoints);
}

void BreakpointHandler::finalizeBreakpoints()
{
    m_size = m_breakpoints.size();
    m_unsorted = m_size;
}

void BreakpointHandler::initSorting()
{
    //currently heapsort is always selected
//    if ( m_breakpoints.size()-1 < 1){
//        m_sortingMethod = SELECTION;
//        selectionSort();
//    } else{
        m_sortingMethod = HEAP;
        buildHeap();
//    }
}

void BreakpointHandler::init(unsigned maxNumberOfBreakpoints)
{
    m_breakpoints.clear();
//    m_breakpoints.reserve(maxNumberOfBreakpoints);
    m_breakpoints.reserve(maxNumberOfBreakpoints+1);
}

const std::vector<const BreakpointHandler::BreakPoint*> &BreakpointHandler::getExpandSecondPass()
{
    //reserve the maximal possible number: total number of breakpoints
    m_secondPassRatios.clear();
    m_secondPassRatios.reserve(m_size);
    const BreakPoint * breakpoint = NULL;

    Numerical::Double theta_1 = Numerical::Infinity;

    for(int i = m_unsorted; i >= 0; i--) {
        if(m_breakpoints[i].additionalValue < theta_1){
            theta_1 = m_breakpoints[i].additionalValue;
        }
    }

    if(theta_1 < 0){
        LPINFO("theta_1 "<<theta_1);
        throw FallbackException(std::string("Expanded value negative"));
    }

    for(unsigned i = m_size - (m_unsorted+1); i < m_size; i++){
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
    for(unsigned i=0; i < m_size-1; i++){
        maxId = i;
        for(unsigned j=i+1; j < m_size; j++){
            if (m_breakpoints[j].value > m_breakpoints[maxId].value){
                maxId = j;
            }
        }

        swapBreakpoints(maxId,i);
    }

    m_unsorted = 0;
}

void BreakpointHandler::heapify(unsigned actual)
{
    unsigned left = 2 * actual + 1;
    unsigned right = left+1;
    unsigned smallest = left;

    if (right < m_unsorted){
        if (m_breakpoints[right].value < m_breakpoints[left].value) {
            smallest = right;
        }
    } else if(left >= m_unsorted){
        return;
    }

//    put smallest to actual, recursive call
    if (m_breakpoints[smallest].value < m_breakpoints[actual].value){
        swapBreakpoints(actual,smallest);
        heapify(smallest);
    }
}

void BreakpointHandler::buildHeap()
{
    for(int i = (m_size)/2; i > 0; i--){
        heapify(i-1);
    }
}

void BreakpointHandler::checkHeap(){
    Numerical::Double ref = 0;

    //check actual Values
    ref = m_breakpoints[0].value;
    for(unsigned i=0; i<m_unsorted; i++){
        if(m_breakpoints[i].value < ref){
            LPINFO("BAD HEAP: ");
            LPINFO("ref: "<<ref);
            LPINFO("val: "<<m_breakpoints[i].value);
            LPINFO("unsorted: "<<m_unsorted);
            printBreakpoints();
            exit(-1);
        }
    }
    for(unsigned i=m_unsorted; i<m_size; i++){
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
