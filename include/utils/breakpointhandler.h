#ifndef BREAKPOINTHANDLER_H
#define BREAKPOINTHANDLER_H

#include <utils/numerical.h>
#include <vector>
using namespace std;

class BreakpointHandler
{
public:
    struct BreakPoint{
        int variableIndex;
        Numerical::Double value;
        mutable Numerical::Double functionValue;
        friend ostream & operator<<(ostream & os, const BreakPoint & breakpoint)
        {
            os << "(" << breakpoint.variableIndex << "; " << breakpoint.value <<
                  "; " << breakpoint.functionValue << ")";
            return os;
        }
        const BreakPoint& operator=(const BreakPoint& other){
            variableIndex = other.variableIndex;
            value = other.value;
            functionValue = other.functionValue;
            return *this;
        }
    };
    BreakpointHandler();
    void insertBreakpoint(int variableIndex, Numerical::Double value);
    const BreakPoint * getBreakpoint(int index);
    unsigned getNumberOfBreakpoints()const;
    void printBreakpoints() const;

    enum SORTING_METHOD
    {
        SELECTION = 0,
        HEAP,
        SORTING_METHOD_ENUM_LENGTH
    };

    void selectMethod();
    void init(unsigned maxNumberOfBreakpoints);

private:
    vector<BreakPoint> m_breakpoints;
    SORTING_METHOD m_sortingMethod;
    void selectionSort();
    int m_unsorted;
    void heapify(int actual);
    void buildHeap();
    void swapBreakpoints(int first, int second);
};

#endif // BREAKPOINTHANDLER_H
