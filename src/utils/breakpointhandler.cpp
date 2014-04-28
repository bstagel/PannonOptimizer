#include <utils/breakpointhandler.h>
#include <simplex/simplexparameterhandler.h>

#include <utils/thirdparty/prettyprint.h>

BreakpointHandler::BreakpointHandler():
    m_sortingMethod(BreakpointHandler::SELECTION),m_unsorted(m_breakpoints.size())
{

}

void BreakpointHandler::insertBreakpoint(int variableIndex, Numerical::Double value)
{
    BreakPoint breakpoint;
    breakpoint.value = value;
    breakpoint.variableIndex = variableIndex;
    breakpoint.functionValue = -Numerical::Infinity;
    m_breakpoints.push_back(breakpoint);
    //TODO plusz muvelet, igy nem hatekony
}

const BreakpointHandler::BreakPoint* BreakpointHandler::getBreakpoint(int index)
{
    int size = m_breakpoints.size();
//    LPERROR("gbp: "<<index);
    if (index >= size){
        LPERROR("Kicimzett a getBreakpoint");
//        LPERROR("index: "<<index);
//        LPERROR(m_breakpoints);
        exit(-1);
    }

    if (m_sortingMethod == BreakpointHandler::HEAP){
        if (index < size-m_unsorted){
            return &m_breakpoints[size-1-index];
        //sort next breakpoint
        } else if (size-m_unsorted == index){
            m_unsorted--;
            swapBreakpoints(0,m_unsorted);
            heapify(0);
        } else{
            LPERROR("--BreakpointHandler ERROR--");
        }
    }
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

    if (m_breakpoints.size() < 1 ){
        m_sortingMethod = SELECTION;
        selectionSort();
    } else{
        m_sortingMethod = HEAP;
        buildHeap();
//        LPINFO("Heap built:");
//        for(unsigned i=0;i<m_breakpoints.size();i++){
//            LPINFO(m_breakpoints[i].value);
//        }
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
        LPINFO("maxid: "<<maxId);
        LPINFO("i: "<<i);
        LPINFO(" m_breakpoints.size(): "<< m_breakpoints.size() );

        swapBreakpoints(maxId,i);
    }
    m_unsorted = 0;
}

void BreakpointHandler::heapify(int actual)
{
    int left = 2 * actual + 1;
    int right = left+1;
    int smallest = actual;

    //chosing smallest element
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

void BreakpointHandler::swapBreakpoints(int first, int second)
{
    BreakPoint temp;
    temp = m_breakpoints[first];
    m_breakpoints[first] = m_breakpoints[second];
    m_breakpoints[second] = temp;
}
