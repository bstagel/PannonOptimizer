#include <utils/breakpointhandler.h>
#include <simplex/simplexparameterhandler.h>

#include <utils/thirdparty/prettyprint.h>

BreakpointHandler::BreakpointHandler():
    m_sortingMethod(BreakpointHandler::SELECTION),m_unsorted(m_breakpoints.size())
{

}

void BreakpointHandler::insertBreakpoint(int variableIndex, Numerical::Double value)
{
    m_breakpoints.emplace_back(variableIndex,value,-Numerical::Infinity);
}

const BreakpointHandler::BreakPoint* BreakpointHandler::getBreakpoint(int index)
{
    int size = m_breakpoints.size();

    if (index >= size){
        LPERROR("Kicimzett a getBreakpoint");
        exit(-1);
    }

//    checkHeap();
    if (m_sortingMethod == BreakpointHandler::HEAP){
        if (index < size-m_unsorted){
            return &m_breakpoints[size-1-index];
        //sort next breakpoint
        } else if (size-m_unsorted == index){
            m_unsorted--;
            swapBreakpoints(0,m_unsorted);
            heapify(0);
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

void BreakpointHandler::selectMethod()
{
    //TODO: one stepnél harmadik ág
    m_unsorted = m_breakpoints.size();

    if (m_breakpoints.size() < 1){
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

void BreakpointHandler::selectionSort()
{
    int maxId = 0;
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

    //choosing smallest element
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
    double ref = m_breakpoints.at(0).value;
    for(int i=0; i<m_unsorted; i++){
        if(m_breakpoints.at(i).value < ref){
            LPINFO("BAD HEAP: ");
            LPINFO("ref: "<<ref);
            LPINFO("val: "<<m_breakpoints.at(i).value);
            LPINFO("unsorted: "<<m_unsorted);
            printBreakpoints();
            exit(-1);
        }
    }
    for(unsigned int i=m_unsorted; i<m_breakpoints.size(); i++){
        if(ref < m_breakpoints.at(i).value){
            LPINFO("BAD SORT:");
            LPINFO("ref: "<<ref);
            LPINFO("val: "<<m_breakpoints.at(i).value);
            LPINFO("unsorted: "<<m_unsorted);
            printBreakpoints();
            exit(-1);
        }
    }
}
