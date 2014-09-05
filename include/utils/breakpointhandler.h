/**
 * @file breakpointhandler.h This file contains the API of the BreakpointHandler class.
 */
#ifndef BREAKPOINTHANDLER_H
#define BREAKPOINTHANDLER_H

#include <utils/numerical.h>
#include <vector>
#include <ostream>

/**
 * The aim of this class is to handle the ratios defined in the ratiotests by storing, sorting them.
 *
 * @class BreakpointHandler
 */
class BreakpointHandler
{
public:
    /**
     * This structure describes a Breakpoint, one point of the piecewise linear concave function in the ratiotest.
     *
     * @struct BreakPoint
     */
    struct BreakPoint{
        /**
         * This is an initializing constructor.
         * @param variableIndex shows the variable, which defined this breakpoint
         * @param value shows the actual value of the ratio
         * @param expandedValue shows the expanded value of the ratio
         * @param functionValue defines the corresponding objective function value to the ratio
         * @constructor
         */
        BreakPoint(int variableIndex, Numerical::Double value, Numerical::Double expandedValue, Numerical::Double functionValue):
            variableIndex(variableIndex),
            value(value),
            expandedValue(expandedValue),
            functionValue(functionValue){}

        /**
         * This is an initializing constructor.
         * If the expandedValue parameter is not given it will be initialized with the value parameter.
         * @param variableIndex shows the variable, which defined this breakpoint
         * @param value shows the actual value of the ratio
         * @param functionValue defines the corresponding objective function value to the ratio
         * @constructor
         */
        BreakPoint(int variableIndex, Numerical::Double value, Numerical::Double functionValue):
            variableIndex(variableIndex),
            value(value),
            expandedValue(value),
            functionValue(functionValue){}

        /**
         * Default desctructor of the Breakpoint structure.
         * @destructor
         */
        ~BreakPoint(){}

        /**
         * Shows the variable, which defined this breakpoint.
         */
        int variableIndex;

        /**
         * Shows the actual value of the ratio.
         */
        Numerical::Double value;

        /**
         * Shows the expanded value of the ratio.
         */
        Numerical::Double expandedValue;

        /**
         * Defines the corresponding objective function value to the ratio.
         */
        mutable Numerical::Double functionValue;

        /**
         * Operator to write the breakpoints onto an ostream.
         * @param os is an ofstream reference on which the data will be written
         * @param breakpoint is a reference to the breakpoint which is to be shown
         * @return The reference of the ofstream parameter os.
         */
        friend std::ostream & operator<<(std::ostream & os, const BreakPoint & breakpoint){
            os << "(" << breakpoint.variableIndex << "; " << breakpoint.value <<
                  "; " << breakpoint.expandedValue <<
                  "; " << breakpoint.functionValue << ")";
            return os;
        }

        /**
         * Assignment operator to be able to simply copy the attributes of a Breakpoint object.
         * @param other is a reference to the Breakpoint object to be copied.
         * @return constant reference to the object on which the function was called.
         */
        const BreakPoint& operator=(const BreakPoint& other){
            variableIndex = other.variableIndex;
            value = other.value;
            expandedValue = other.expandedValue;
            functionValue = other.functionValue;
            return *this;
        }
    };

    /**
     * Default constructor of the class.
     *
     * @constructor
     */
    BreakpointHandler();

    /**
     * With this function we can add a breakpoint to the Breakpointhandler, which stores them in a vector.
     * @param variableIndex shows the variable that defined the breakpoint
     * @param value is the actual value of the ratio
     * @param expandedValue is the expanded value of the ratio
     */
    ALWAYS_INLINE void insertBreakpoint(int variableIndex, Numerical::Double value, Numerical::Double expandedValue = Numerical::Infinity){
        m_breakpoints.emplace_back(variableIndex,value,expandedValue,-Numerical::Infinity);
    }

    /**
     * This is the function which provides the breakpoints to the ratiotests.
     * If it's called, it performs the sorting of the ratios corresponding to the current sorting method.
     * The currently implemented methods can be found at BreakpointHandler::SORTING_METHOD.
     * @param index is the indexth stored breakpoint
     * @return A const pointer to the selected breakpoint.
     */
    const BreakPoint * getBreakpoint(int index);

    /**
     * A getter that returns the number of the stored breakpoints.
     */
    unsigned getNumberOfBreakpoints()const;

    /**
     * Displays the breakpoints in human-readable format.
     */
    void printBreakpoints() const;

    /**
     * This indicates the sorting methods which are implemented and can be used to sort the breakpoints.
     */
    enum SORTING_METHOD{
        SELECTION = 0,
        HEAP,
        SORTING_METHOD_ENUM_LENGTH
    };

    /**
     * In this function we can implement in what case what sorting algorithm we want to use.
     */
    void initSorting();

    /**
     * A simple initializing function which performs a clear() and a reserve() operation.
     * @param maxNumberOfBreakpoints shows how many elements are to be reserved
     */
    void init(unsigned maxNumberOfBreakpoints);

    /**
     * Function returning the second pass ratios of expand procedure.
     * In the first pass ratios with expanded bounds are defined, and a theta steplength is choosen among them.
     * In the second pass of the Harris ratio test and the Expand procedure we need ratios
     * (defined with exact bounds) smaller than parameter theta.
     */
    const std::vector<const BreakPoint *> &getExpandSecondPass();

private:
    /**
     * A vector storing the Breakpoints defined in the ratiotests.
     */
    std::vector<BreakPoint> m_breakpoints;

    /**
     * This attribute shows the currently used sorting method.
     */
    SORTING_METHOD m_sortingMethod;

    /**
     * This member indicates how many breakpoints are sorted currently.
     * The vector is sorted backwards, the smallest breakpoint is the last element. The last sorted element
     * can be obtained if we substract this value from the size of the vector.
     */
    int m_unsorted;

    /**
     * Vector containing the ratios of the expand second pass used in the Harris ratio test and the Expand procedure.
     * These ratios are defined by the exact bounds and at the end of the procedure, the ratio with the biggest
     *  absolute alpha value is choosen. So it provides a stable pivot and is degeneracy avare strategy.
     */
    std::vector<const BreakPoint*> m_secondPassRatios;

    /**
     * An implemented sorting method: the selection sort algorithm.
     * If this function is called it performs the sorting of all elements.
     */
    void selectionSort();

    /**
     * The wide known heapify function, to maintain the heap property of the stored breakpoints.
     * @param actual points to the actual element of the vector
     */
    void heapify(int actual);

    /**
     * This function builds a heap from the vector storing the breakpoints.
     * The root of the heap will be the last element of the vector.
     */
    void buildHeap();

    /**
     * This function helps to check whether the heap property is maintained or not. It can be used for debug.
     */
    void checkHeap();

    /**
      * With this function we can swap two breakpoints given in parameters.
      * It performs an efficient swapping using the std::swap method, that takes advantage of the move constructor
      * of the Breakpoint struct.
      */
    inline void swapBreakpoints(int first, int second){std::swap(m_breakpoints[first],m_breakpoints[second]);}
};

#endif // BREAKPOINTHANDLER_H
