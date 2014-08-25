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

void BreakpointHandler::insertBreakpoint(int variableIndex,
                                         Numerical::Double value,
                                         Numerical::Double expandedValue)
{
    m_breakpoints.emplace_back(variableIndex,value,expandedValue,-Numerical::Infinity);
}

const BreakpointHandler::BreakPoint* BreakpointHandler::getBreakpoint(int index)
{
    int size = m_breakpoints.size();

#ifndef NDEBUG
    if (index >= size){
        LPERROR("Invalid index in getBreakpoint()");
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

void BreakpointHandler::selectMethod(bool expand)
{
    //currently heapsort is always selected
    m_expand = expand;
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

const std::vector<const BreakpointHandler::BreakPoint*> &BreakpointHandler::getExpandSecondPass(Numerical::Double theta)
{
    //Reinit the heap with the exact values
    selectMethod(false);
    //reserve the maximal possible number: total number of breakpoints
    m_secondPassRatios.clear();
    m_secondPassRatios.reserve(m_breakpoints.size());
    const BreakPoint * breakpoint = NULL;

    for(unsigned i=0; i < m_breakpoints.size(); i++){
        breakpoint = getBreakpoint(i);
        //actual values smaller than the theta parameter
        if(breakpoint->value <= theta){
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

    //sort expanded values
    if(m_expand){
        for(unsigned i=0; i < m_breakpoints.size()-1; i++){
            maxId = i;
            for(unsigned j=i+1; j < m_breakpoints.size(); j++){
                if (m_breakpoints[j].expandedValue > m_breakpoints[maxId].expandedValue){
                    maxId = j;
                }
            }

            swapBreakpoints(maxId,i);
        }

    //sort actual values
    }else{
        for(unsigned i=0; i < m_breakpoints.size()-1; i++){
            maxId = i;
            for(unsigned j=i+1; j < m_breakpoints.size(); j++){
                if (m_breakpoints[j].value > m_breakpoints[maxId].value){
                    maxId = j;
                }
            }

            swapBreakpoints(maxId,i);
        }
    }
    m_unsorted = 0;
}

void BreakpointHandler::heapify(int actual)
{
    int left = 2 * actual + 1;
    int right = left+1;
    int smallest = actual;

    //choosing smallest element (expanded value)
    if(m_expand){
        if (left < m_unsorted && (m_breakpoints[left].expandedValue < m_breakpoints[smallest].expandedValue) ) {
            smallest = left;
        }
        if (right < m_unsorted && (m_breakpoints[right].expandedValue < m_breakpoints[smallest].expandedValue) ) {
            smallest = right;
        }

    //choosing smallest element (actual value)
    }else{
        if (left < m_unsorted && (m_breakpoints[left].value < m_breakpoints[smallest].value) ) {
            smallest = left;
        }
        if (right < m_unsorted && (m_breakpoints[right].value < m_breakpoints[smallest].value) ) {
            smallest = right;
        }
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

    //check expanded Values
    if(m_expand){
        ref = m_breakpoints[0].expandedValue;
        for(int i=0; i<m_unsorted; i++){
            if(m_breakpoints[i].expandedValue < ref){
                LPINFO("BAD HEAP: ");
                LPINFO("ref: "<<ref);
                LPINFO("expandedValue: "<<m_breakpoints[i].expandedValue);
                LPINFO("unsorted: "<<m_unsorted);
                printBreakpoints();
                exit(-1);
            }
        }
        for(unsigned int i=m_unsorted; i<m_breakpoints.size(); i++){
            if(ref < m_breakpoints[i].expandedValue){
                LPINFO("BAD SORT:");
                LPINFO("ref: "<<ref);
                LPINFO("expandedValue: "<<m_breakpoints[i].expandedValue);
                LPINFO("unsorted: "<<m_unsorted);
                printBreakpoints();
                exit(-1);
            }
        }

    //check actual Values
    }else{
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
}
