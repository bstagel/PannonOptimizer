/**
 * @file doublehistory.h
 * @author Jozsef Smidla
 */

#ifndef DOUBLEHISTORY_H
#define DOUBLEHISTORY_H

#include <globals.h>
#include <cmath>
#include <iostream>
#include <string>
#include <debug.h>
#include <utils/refobject.h>

/**
 * Class implementing double precision floating point variable with operation history.
 * This allows to backtrack the operations and events changing the variable to the actual
 * value, making debugging easier.
 *
 * @class DoubleHistory
 */
class DoubleHistory {
public:

    /**
     * This type describes an operation.
     */
    enum OPERATION_TYPE {
        NONE,
        ADD,
        SUB,
        DIV,
        MUL,
        SQRT,
        ROUND,
        ABS,
        INV,
    };

    /**
     * This struct records the actual value of the variable and the previous operation to it.
     * It also gives reference of the operators of the previous operation.
     */
    struct Double {
        double m_value;
        RefObject<Double> m_left;
        RefObject<Double> m_right;
        std::string m_environment;
        OPERATION_TYPE m_type;
        bool m_initialized;
    };

    /**
     * Default constructor of the DoubleHistory class.
     *
     * @constructor
     */
    DoubleHistory() {
        m_number = RefObject<Double>(new Double);
        m_number->m_initialized = false;
        m_number->m_left = 0;
        m_number->m_right = 0;
        m_number->m_type = NONE;
    }

    /**
     * Initializes the DoubleHistory class to a given double value.
     *
     * @constructor
     * @param value The value the DoubleHistory object to be initialized.
     */
    DoubleHistory(double value) {
        init(value);
    }

    /**
     * Initializes the DoubleHistory class to a given integer value.
     *
     * @constructor
     * @param value The value the DoubleHistory object to be initialized.
     */
    DoubleHistory(int value) {
        init(value);
    }

    /**
     * Initializes the DoubleHistory class to a given unsigned integer value.
     *
     * @constructor
     * @param value The value the DoubleHistory object to be initialized.
     */
    DoubleHistory(unsigned int value) {
        init(value);
    }

    /**
     * Initializes the DoubleHistory class to a given long integer value.
     *
     * @constructor
     * @param value The value the DoubleHistory object to be initialized.
     */
    DoubleHistory(long int value) {
        init(value);
    }

    /**
     * Initializes the DoubleHistory class to a given long long integer value.
     *
     * @constructor
     * @param value The value the DoubleHistory object to be initialized.
     */
    DoubleHistory(long long int value) {
        init(value);
    }

    /**
     * Initializes the DoubleHistory class to a given unsigned long integer value.
     *
     * @constructor
     * @param value The value the DoubleHistory object to be initialized.
     */
    DoubleHistory(unsigned long int value) {
        init(value);
    }

    /**
     * Initializes the DoubleHistory class to a given unsigned long long integer value.
     *
     * @constructor
     * @param value The value the DoubleHistory object to be initialized.
     */
    DoubleHistory(unsigned long long int value) {
        init(value);
    }

    /**
     * Copy constructor of the DoubleHistory class.
     *
     * @constructor
     * @param orig The original object to be copied.
     */
    DoubleHistory(const DoubleHistory & orig) {
        m_number = RefObject<Double>(new Double);
        m_number->m_initialized = orig.m_number->m_initialized;
        m_number->m_left = orig.m_number->m_left;
        m_number->m_right = orig.m_number->m_right;
        m_number->m_type = orig.m_number->m_type;
        m_number->m_value = orig.m_number->m_value;
    }

    /**
     * Assignment operator of the DoubleHistory class.
     *
     * @param orig The original element to be copied.
     * @return Reference of the copied object.
     */
    DoubleHistory & operator=(const DoubleHistory & orig) {
        if (this == &orig) {
            return *this;
        }
        m_number = new Double;
        m_number->m_initialized = orig.m_number->m_initialized;
        m_number->m_left = orig.m_number->m_left;
        m_number->m_right = orig.m_number->m_right;
        m_number->m_type = orig.m_number->m_type;
        m_number->m_value = orig.m_number->m_value;
        return *this;
    }

    /**
     * Integer conversion operator.
     */
    operator int() const {
        return (int)m_number->m_value;
    }

    /**
     * Unsigned integer conversion operator.
     */
    operator unsigned int() const {
        return (unsigned int)m_number->m_value;
    }

    /**
     * Double conversion operator
     */
    operator double() const {
        return m_number->m_value;
    }

    /**
     * Left side addition operator with another DoubleHistory object.
     *
     * @param right The DoubleHistory to be added to this.
     * @return The sum of the two DoubleHistories.
     */
    DoubleHistory operator + (const DoubleHistory & right) const {
        DoubleHistory result;
        result.m_number->m_initialized = m_number->m_initialized && right.m_number->m_initialized;
        result.m_number->m_value = m_number->m_value + right.m_number->m_value;
        result.m_number->m_left = m_number;
        result.m_number->m_right = right.m_number;
        result.m_number->m_type = ADD;
        result.m_number->m_environment = sm_environment;

        return result;

    }

    /**
     * Left side addition operator with a double variable.
     *
     * @param right The double to be added to this.
     * @return The sum of the two operands.
     */
    DoubleHistory operator + (double right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    /**
     * Left side addition operator with an integer variable.
     *
     * @param right The integer to be added to this.
     * @return The sum of the two operands.
     */
    DoubleHistory operator + (int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    /**
     * Left side addition operator with an unsigned integer variable.
     *
     * @param right The unsigned integer to be added to this.
     * @return The sum of the two operands.
     */
    DoubleHistory operator + (unsigned int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    /**
     * Left side addition operator with a long integer variable.
     *
     * @param right The long integer to be added to this.
     * @return The sum of the two operands.
     */
    DoubleHistory operator + (long int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    /**
     * Left side addition operator with an unsigned long integer variable.
     *
     * @param right The unsigned long integer to be added to this.
     * @return The sum of the two operands.
     */
    DoubleHistory operator + (unsigned long int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    /**
     * Left side addition operator with a long long integer variable.
     *
     * @param right The long long integer to be added to this.
     * @return The sum of the two operands.
     */
    DoubleHistory operator + (long long int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    /**
     * Left side addition operator with an unsigned long long integer variable.
     *
     * @param right The unsigned long long integer to be added to this.
     * @return The sum of the two operands.
     */
    DoubleHistory operator + (unsigned long long int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    /**
     * Operator to double-DoubleHistory addition.
     *
     * @param left The double to be added.
     * @param right The DoubleHistory to be added.
     * @return The sum of the two operators.
     */
    friend DoubleHistory operator + (double left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    /**
     * Operator to integer-DoubleHistory addition.
     *
     * @param left The integer to be added.
     * @param right The DoubleHistory to be added.
     * @return The sum of the two operators.
     */
    friend DoubleHistory operator + (int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    /**
     * Operator to long integer-DoubleHistory addition.
     *
     * @param left The long integer to be added.
     * @param right The DoubleHistory to be added.
     * @return The sum of the two operators.
     */
    friend DoubleHistory operator + (long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    /**
     * Operator to long long integer-DoubleHistory addition.
     *
     * @param left The long long integer to be added.
     * @param right The DoubleHistory to be added.
     * @return The sum of the two operators.
     */
    friend DoubleHistory operator + (long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    /**
     * Operator to unsigned integer-DoubleHistory addition.
     *
     * @param left The unsigned integer to be added.
     * @param right The DoubleHistory to be added.
     * @return The sum of the two operators.
     */
    friend DoubleHistory operator + (unsigned int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    /**
     * Operator to unsigned long integer-DoubleHistory addition.
     *
     * @param left The unsigned long integer to be added.
     * @param right The DoubleHistory to be added.
     * @return The sum of the two operators.
     */
    friend DoubleHistory operator + (unsigned long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    /**
     * Operator to unsigned long long integer-DoubleHistory addition.
     *
     * @param left The unsigned long long integer to be added.
     * @param right The DoubleHistory to be added.
     * @return The sum of the two operators.
     */
    friend DoubleHistory operator + (unsigned long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    /**
     * Short addition operator for two DoubleHistory objects.
     *
     * @param right The other DoubleHistory object to be added.
     * @return Reference of the result DoubleHistory object.
     */
    DoubleHistory & operator += (const DoubleHistory & right) {

        RefObject<Double> temp = m_number;

        m_number = RefObject<Double>(new Double);
        m_number->m_initialized = temp->m_initialized && right.m_number->m_initialized;
        m_number->m_value = temp->m_value + right.m_number->m_value;
        m_number->m_left = temp;
        m_number->m_right = right.m_number;
        m_number->m_type = ADD;
        m_number->m_environment = sm_environment;

        return *this;
    }

    /**
     * Left side substraction operator with another DoubleHistory object.
     *
     * @param right The DoubleHistory to be substracted from this.
     * @return The difference of the two DoubleHistories.
     */
    DoubleHistory operator - (const DoubleHistory & right) const {
        DoubleHistory result;
        result.m_number->m_initialized = m_number->m_initialized && right.m_number->m_initialized;
        result.m_number->m_value = m_number->m_value - right.m_number->m_value;
        result.m_number->m_left = m_number;
        result.m_number->m_right = right.m_number;
        result.m_number->m_type = SUB;
        result.m_number->m_environment = sm_environment;

        return result;
    }

    /**
     * Left side substraction operator with a double variable.
     *
     * @param right The double to be substracted from this.
     * @return The difference of the two operands.
     */
    DoubleHistory operator - (double right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    /**
     * Left side substraction operator with an integer variable.
     *
     * @param right The integer to be substracted from this.
     * @return The difference of the two operands.
     */
    DoubleHistory operator - (int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    /**
     * Left side substraction operator with an unsigned integer variable.
     *
     * @param right The unsigned integer to be substracted from this.
     * @return The difference of the two operands.
     */
    DoubleHistory operator - (unsigned int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    /**
     * Left side substraction operator with a long integer variable.
     *
     * @param right The long integer to be substracted from this.
     * @return The difference of the two operands.
     */
    DoubleHistory operator - (long int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    /**
     * Left side substraction operator with an unsigned long integer variable.
     *
     * @param right The unsigned long integer to be substracted from this.
     * @return The difference of the two operands.
     */
    DoubleHistory operator - (unsigned long int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    /**
     * Left side substraction operator with a long long integer variable.
     *
     * @param right The long long integer to be substracted from this.
     * @return The difference of the two operands.
     */
    DoubleHistory operator - (long long int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    /**
     * Left side substraction operator with an unsigned long long integer variable.
     *
     * @param right The unsigned long long integer to be substracted from this.
     * @return The difference of the two operands.
     */
    DoubleHistory operator - (unsigned long long int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    /**
     * Short substraction operator for two DoubleHistory objects.
     *
     * @param right The other DoubleHistory object to be substracted.
     * @return Reference of the result DoubleHistory object.
     */
    DoubleHistory & operator -= (const DoubleHistory & right) {
        RefObject<Double> temp = m_number;
        m_number = RefObject<Double>(new Double);
        m_number->m_initialized = temp->m_initialized && right.m_number->m_initialized;
        m_number->m_value = temp->m_value - right.m_number->m_value;
        m_number->m_left = temp;
        m_number->m_right = right.m_number;
        m_number->m_type = SUB;
        m_number->m_environment = sm_environment;
        return *this;
    }

    /**
     * Operator to double-DoubleHistory substraction.
     *
     * @param left The double to be substracted from.
     * @param right The DoubleHistory to be substracted.
     * @return The difference of the two operators.
     */
    friend DoubleHistory operator - (double left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    /**
     * Operator to integer-DoubleHistory substraction.
     *
     * @param left The integer to be substracted from.
     * @param right The DoubleHistory to be substracted.
     * @return The difference of the two operators.
     */
    friend DoubleHistory operator - (int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    /**
     * Operator to long integer-DoubleHistory substraction.
     *
     * @param left The long integer to be substracted from.
     * @param right The DoubleHistory to be substracted.
     * @return The difference of the two operators.
     */
    friend DoubleHistory operator - (long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    /**
     * Operator to long long integer-DoubleHistory substraction.
     *
     * @param left The long lnog integer to be substracted from.
     * @param right The DoubleHistory to be substracted.
     * @return The difference of the two operators.
     */
    friend DoubleHistory operator - (long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    /**
     * Operator to unsigned integer-DoubleHistory substraction.
     *
     * @param left The unsigned integer to be substracted from.
     * @param right The DoubleHistory to be substracted.
     * @return The difference of the two operators.
     */
    friend DoubleHistory operator - (unsigned int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    /**
     * Operator to unsigned long integer-DoubleHistory substraction.
     *
     * @param left The unsigned long integer to be substracted from.
     * @param right The DoubleHistory to be substracted.
     * @return The difference of the two operators.
     */
    friend DoubleHistory operator - (unsigned long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    /**
     * Operator to unsigned long long integer-DoubleHistory substraction.
     *
     * @param left The unsigned long long integer to be substracted from.
     * @param right The DoubleHistory to be substracted.
     * @return The difference of the two operators.
     */
    friend DoubleHistory operator - (unsigned long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    /**
     * Left side multiplication operator with another DoubleHistory object.
     *
     * @param right The DoubleHistory to be multiplied by this.
     * @return The product of the two DoubleHistories.
     */
    DoubleHistory operator * (const DoubleHistory & right) const {
        DoubleHistory result;
        result.m_number->m_initialized = m_number->m_initialized && right.m_number->m_initialized;
        result.m_number->m_value = m_number->m_value * right.m_number->m_value;
        result.m_number->m_left = m_number;
        result.m_number->m_right = right.m_number;
        result.m_number->m_type = MUL;
        result.m_number->m_environment = sm_environment;

        return result;
    }

    /**
     * Left side multiplication operator with a double variable.
     *
     * @param right The double to be multiplied by this.
     * @return The product of the two operands.
     */
    DoubleHistory operator * (double right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    /**
     * Left side multiplication operator with an integer variable.
     *
     * @param right The integer to be multiplied by this.
     * @return The product of the two operands.
     */
    DoubleHistory operator * (int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    /**
     * Left side multiplication operator with an unsigned integer variable.
     *
     * @param right The unsigned integer to be multiplied by this.
     * @return The product of the two operands.
     */
    DoubleHistory operator * (unsigned int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    /**
     * Left side multiplication operator with a long integer variable.
     *
     * @param right The long integer to be multiplied by this.
     * @return The product of the two operands.
     */
    DoubleHistory operator * (long int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    /**
     * Left side multiplication operator with an unsigned long integer variable.
     *
     * @param right The unsigned long integer to be multiplied by this.
     * @return The product of the two operands.
     */
    DoubleHistory operator * (unsigned long int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    /**
     * Left side multiplication operator with a long long integer variable.
     *
     * @param right The long long integer to be multiplied by this.
     * @return The product of the two operands.
     */
    DoubleHistory operator * (long long int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    /**
     * Left side multiplication operator with an unsigned long long integer variable.
     *
     * @param right The unsigned long long integer to be multiplied by this.
     * @return The product of the two operands.
     */
    DoubleHistory operator * (unsigned long long int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    /**
     * Short multiplication operator for two DoubleHistory objects.
     *
     * @param right The other DoubleHistory object to be multiplied.
     * @return Reference of the result DoubleHistory object.
     */
    DoubleHistory & operator *= (const DoubleHistory & right) {
        RefObject<Double> temp = m_number;
        m_number = RefObject<Double>(new Double);
        m_number->m_initialized = temp->m_initialized && right.m_number->m_initialized;
        m_number->m_value = temp->m_value * right.m_number->m_value;
        m_number->m_left = temp;
        m_number->m_right = right.m_number;
        m_number->m_type = MUL;
        m_number->m_environment = sm_environment;
        return *this;
    }

    /**
     * Operator to double-DoubleHistory multiplication.
     *
     * @param left The double to be multiplied.
     * @param right The DoubleHistory to be multiplied by.
     * @return The product of the two operators.
     */
    friend DoubleHistory operator * (double left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    /**
     * Operator to integer-DoubleHistory multiplication.
     *
     * @param left The integer to be multiplied.
     * @param right The DoubleHistory to be multiplied by.
     * @return The product of the two operators.
     */
    friend DoubleHistory operator * (int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    /**
     * Operator to long integer-DoubleHistory multiplication.
     *
     * @param left The long integer to be multiplied.
     * @param right The DoubleHistory to be multiplied by.
     * @return The product of the two operators.
     */
    friend DoubleHistory operator * (long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    /**
     * Operator to long long integer-DoubleHistory multiplication.
     *
     * @param left The long long integer to be multiplied.
     * @param right The DoubleHistory to be multiplied by.
     * @return The product of the two operators.
     */
    friend DoubleHistory operator * (long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    friend DoubleHistory operator * (unsigned int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    /**
     * Operator to unsigned long integer-DoubleHistory multiplication.
     *
     * @param left The unsigned long integer to be multiplied.
     * @param right The DoubleHistory to be multiplied by.
     * @return The product of the two operators.
     */
    friend DoubleHistory operator * (unsigned long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    /**
     * Operator to unsigned long long integer-DoubleHistory multiplication.
     *
     * @param left The unsigned long long integer to be multiplied.
     * @param right The DoubleHistory to be multiplied by.
     * @return The product of the two operators.
     */
    friend DoubleHistory operator * (unsigned long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    /**
     * Left side division operator with another DoubleHistory object.
     *
     * @param right The DoubleHistory to be divided by this.
     * @return The quotient of the two DoubleHistories.
     */
    DoubleHistory operator / (const DoubleHistory & right) const {
        DoubleHistory result;
        result.m_number->m_initialized = m_number->m_initialized && right.m_number->m_initialized;
        result.m_number->m_value = m_number->m_value / right.m_number->m_value;
        result.m_number->m_left = m_number;
        result.m_number->m_right = right.m_number;
        result.m_number->m_type = DIV;
        result.m_number->m_environment = sm_environment;

        return result;
    }

    /**
     * Left side division operator with a double variable.
     *
     * @param right The double dividing this.
     * @return The quotient of the two operands.
     */
    DoubleHistory operator / (double right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    /**
     * Left side division operator with an integer variable.
     *
     * @param right The integer dividing this.
     * @return The quotient of the two operands.
     */
    DoubleHistory operator / (int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    /**
     * Left side division operator with an unsigned integer variable.
     *
     * @param right The unsigned integer dividing this.
     * @return The quotient of the two operands.
     */
    DoubleHistory operator / (unsigned int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    /**
     * Left side division operator with a long integer variable.
     *
     * @param right The long integer dividing this.
     * @return The quotient of the two operands.
     */
    DoubleHistory operator / (long int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    /**
     * Left side division operator with an unsigned long integer variable.
     *
     * @param right The unsigned long integer dividing this.
     * @return The quotient of the two operands.
     */
    DoubleHistory operator / (unsigned long int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    /**
     * Left side division operator with a long long integer variable.
     *
     * @param right The long long integer dividing this.
     * @return The quotient of the two operands.
     */
    DoubleHistory operator / (long long int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    /**
     * Left side division operator with an unsigned long long integer variable.
     *
     * @param right The unsigned long long integer dividing this.
     * @return The quotient of the two operands.
     */
    DoubleHistory operator / (unsigned long long int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    /**
     * Operator to double-DoubleHistory division.
     *
     * @param left The double to be divided.
     * @param right The DoubleHistory to be divided by.
     * @return The quotient of the two operators.
     */
    friend DoubleHistory operator / (double left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    /**
     * Operator to integer-DoubleHistory division.
     *
     * @param left The integer to be divided.
     * @param right The DoubleHistory to be divided by.
     * @return The quotient of the two operators.
     */
    friend DoubleHistory operator / (int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    /**
     * Operator to long integer-DoubleHistory division.
     *
     * @param left The long integer to be divided.
     * @param right The DoubleHistory to be divided by.
     * @return The quotient of the two operators.
     */
    friend DoubleHistory operator / (long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    /**
     * Operator to long long integer-DoubleHistory division.
     *
     * @param left The long long integer to be divided.
     * @param right The DoubleHistory to be divided by.
     * @return The quotient of the two operators.
     */
    friend DoubleHistory operator / (long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    /**
     * Operator to unsigned integer-DoubleHistory division.
     *
     * @param left The unsigned integer to be divided.
     * @param right The DoubleHistory to be divided by.
     * @return The quotient of the two operators.
     */
    friend DoubleHistory operator / (unsigned int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    /**
     * Operator to unsigned long integer-DoubleHistory division.
     *
     * @param left The unsigned long integer to be divided.
     * @param right The DoubleHistory to be divided by.
     * @return The quotient of the two operators.
     */
    friend DoubleHistory operator / (unsigned long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    /**
     * Operator to unsigned long long integer-DoubleHistory division.
     *
     * @param left The unsigned long long integer to be divided.
     * @param right The DoubleHistory to be divided by.
     * @return The quotient of the two operators.
     */
    friend DoubleHistory operator / (unsigned long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    /**
     * Short division operator for two DoubleHistory objects.
     *
     * @param right The other DoubleHistory object dividing this.
     * @return Reference of the result DoubleHistory object.
     */
    DoubleHistory & operator /= (const DoubleHistory & right) {
        RefObject<Double> temp = m_number;
        m_number = RefObject<Double>(new Double);
        m_number->m_initialized = temp->m_initialized && right.m_number->m_initialized;
        m_number->m_value = temp->m_value / right.m_number->m_value;
        m_number->m_left = temp;
        m_number->m_right = right.m_number;
        m_number->m_type = DIV;
        m_number->m_environment = sm_environment;
        return *this;
    }

    /**
     * Negation operator for the DoubleHistory class.
     *
     * @return The multiplicant of this object and -1.
     */
    DoubleHistory operator-() const {
        DoubleHistory result;
        result.m_number->m_initialized = m_number->m_initialized;
        result.m_number->m_value = -m_number->m_value;
        result.m_number->m_left = m_number;
        result.m_number->m_right = 0;
        result.m_number->m_type = INV;
        result.m_number->m_environment = sm_environment;
        return result;
    }

    /**
     * Less than operator to two DoubleHistory variables.
     *
     * @param num The other DoubleHistory object to be compared.
     * @return True if the value of this is less than num's.
     */
    bool operator < (const DoubleHistory & num) const {
        return m_number->m_value < num.m_number->m_value;
    }

    /**
     * Less than or equal to operator to two DoubleHistory variables.
     *
     * @param num The other DoubleHistory object to be compared.
     * @return True if the value of this is less than or equal to num's.
     */
    bool operator <= (const DoubleHistory & num) const {
        return m_number->m_value <= num.m_number->m_value;
    }

    /**
     * Greater than operator between two DoubleHistory variables.
     *
     * @param num The other DoubleHistory object to be compared.
     * @return True if the value of this is greater than num's.
     */
    bool operator > (const DoubleHistory & num) const {
        return m_number->m_value > num.m_number->m_value;
    }

    /**
     * Greater than or equal to operator to two DoubleHistory variables.
     *
     * @param num The other DoubleHistory object to be compared.
     * @return True if the value of this is greater than or equal to num's.
     */
    bool operator >= (const DoubleHistory & num) const {
        return m_number->m_value >= num.m_number->m_value;
    }

    /**
     * Equality operator to two DoubleHistory variables.
     *
     * @param num The other DoubleHistory object to be compared.
     * @return True if the value of this is equal to num's.
     */
    bool operator == (const DoubleHistory & num) const {
        return m_number->m_value == num.m_number->m_value;
    }

    /**
     * Non-equality operator to two DoubleHistory variables.
     *
     * @param num The other DoubleHistory object to be compared.
     * @return True if the value of this is not equal to num's.
     */
    bool operator != (const DoubleHistory & num) const {
        return m_number->m_value != num.m_number->m_value;
    }

    /**
     * Less than operator comparing a double variable.
     *
     * @param num The double variable to be compared.
     * @return True if the value of this is less than num's.
     */
    bool operator < (double num) const {
        return m_number->m_value < num;
    }

    /**
     * Less than or equal to operator comparing a double variable.
     *
     * @param num The double variable to be compared.
     * @return True if the value of this is less than or equal to num's.
     */
    bool operator <= (double num) const {
        return m_number->m_value <= num;
    }

    /**
     * Greater than operator comparing a double variable.
     *
     * @param num The double variable to be compared.
     * @return True if the value of this is greater than num's.
     */
    bool operator > (double num) const {
        return m_number->m_value > num;
    }

    /**
     * Greater than or equal to operator comparing a double variable.
     *
     * @param num The double variable to be compared.
     * @return True if the value of this is greater than or equal to num's.
     */
    bool operator >= (double num) const {
        return m_number->m_value >= num;
    }

    /**
     * Equality operator comparing a double variable.
     *
     * @param num The double variable to be compared.
     * @return True if the value of this is equal to num's.
     */
    bool operator == (double num) const {
        return m_number->m_value == num;
    }

    /**
     * Non-equality operator comparing a double variable.
     *
     * @param num The double variable to be compared.
     * @return True if the value of this is not equal to num's.
     */
    bool operator != (double num) const {
        return m_number->m_value != num;
    }

    /**
     * Less than operator comparing an integer variable.
     *
     * @param num The integer variable to be compared.
     * @return True if the value of this is less than num's.
     */
    bool operator < (int num) const {
        return m_number->m_value < num;
    }

    /**
     * Less than or equal to operator comparing an integer variable.
     *
     * @param num The integer variable to be compared.
     * @return True if the value of this is less than or equal to num's.
     */
    bool operator <= (int num) const {
        return m_number->m_value <= num;
    }

    /**
     * Greater than operator comparing an integer variable.
     *
     * @param num The integer variable to be compared.
     * @return True if the value of this is greater than num's.
     */
    bool operator > (int num) const {
        return m_number->m_value > num;
    }

    /**
     * Greater than or equal to operator comparing an integer variable.
     *
     * @param num The integer variable to be compared.
     * @return True if the value of this is greater than or equal to num's.
     */
    bool operator >= (int num) const {
        return m_number->m_value >= num;
    }

    /**
     * Equality operator comparing an integer variable.
     *
     * @param num The integer variable to be compared.
     * @return True if the value of this is equal to num's.
     */
    bool operator == (int num) const {
        return m_number->m_value == num;
    }

    /**
     * Non-equality operator comparing an integer variable.
     *
     * @param num The integer variable to be compared.
     * @return True if the value of this is not equal to num's.
     */
    bool operator != (int num) const {
        return m_number->m_value != num;
    }

    /**
     * Less than operator comparing an unsigned integer variable.
     *
     * @param num The unsigned integer variable to be compared.
     * @return True if the value of this is less than num's.
     */
    bool operator < (unsigned int num) const {
        return m_number->m_value < num;
    }

    /**
     * Less than or equal to operator comparing an unsigned integer variable.
     *
     * @param num The unsigned integer variable to be compared.
     * @return True if the value of this is less than or equal to num's.
     */
    bool operator <= (unsigned int num) const {
        return m_number->m_value <= num;
    }

    /**
     * Greater than operator comparing an unsigned integer variable.
     *
     * @param num The unsigned integer variable to be compared.
     * @return True if the value of this is greater than num's.
     */
    bool operator > (unsigned int num) const {
        return m_number->m_value > num;
    }

    /**
     * Greater than or equal to operator comparing an unsigned integer variable.
     *
     * @param num The unsigned integer variable to be compared.
     * @return True if the value of this is greater than or equal to num's.
     */
    bool operator >= (unsigned int num) const {
        return m_number->m_value >= num;
    }

    /**
     * Equality operator comparing an unsigned integer variable.
     *
     * @param num The unsigned integer variable to be compared.
     * @return True if the value of this is equal to num's.
     */
    bool operator == (unsigned int num) const {
        return m_number->m_value == num;
    }

    /**
     * Non-equality operator comparing an unsigned integer variable.
     *
     * @param num The unsigned integer variable to be compared.
     * @return True if the value of this is not equal to num's.
     */
    bool operator != (unsigned int num) const {
        return m_number->m_value != num;
    }

    /**
     * Less than operator comparing a long integer variable.
     *
     * @param num The long integer variable to be compared.
     * @return True if the value of this is less than num's.
     */
    bool operator < (long int num) const {
        return m_number->m_value < num;
    }

    /**
     * Less than or equal to operator comparing a long integer variable.
     *
     * @param num The long integer variable to be compared.
     * @return True if the value of this is less than or equal to num's.
     */
    bool operator <= (long int num) const {
        return m_number->m_value <= num;
    }

    /**
     * Greater than operator comparing a long integer variable.
     *
     * @param num The long integer variable to be compared.
     * @return True if the value of this is greater than num's.
     */
    bool operator > (long int num) const {
        return m_number->m_value > num;
    }

    /**
     * Greater than or equal to operator comparing a long integer variable.
     *
     * @param num The long integer variable to be compared.
     * @return True if the value of this is greater than or equal to num's.
     */
    bool operator >= (long int num) const {
        return m_number->m_value >= num;
    }

    /**
     * Equality operator comparing a long integer variable.
     *
     * @param num The long integer variable to be compared.
     * @return True if the value of this is equal to num's.
     */
    bool operator == (long int num) const {
        return m_number->m_value == num;
    }

    /**
     * Non-equality operator comparing a long integer variable.
     *
     * @param num The long integer variable to be compared.
     * @return True if the value of this is not equal to num's.
     */
    bool operator != (long int num) const {
        return m_number->m_value != num;
    }

    /**
     * Less than operator comparing an unsigned long integer variable.
     *
     * @param num The unsigned long integer variable to be compared.
     * @return True if the value of this is less than num's.
     */
    bool operator < (unsigned long int num) const {
        return m_number->m_value < num;
    }

    /**
     * Less than or equal to operator comparing an unsigned long integer variable.
     *
     * @param num The unsigned long integer variable to be compared.
     * @return True if the value of this is less than or equal to num's.
     */
    bool operator <= (unsigned long int num) const {
        return m_number->m_value <= num;
    }

    /**
     * Greater than operator comparing an unsigned long integer variable.
     *
     * @param num The unsigned long integer variable to be compared.
     * @return True if the value of this is greater than num's.
     */
    bool operator > (unsigned long int num) const {
        return m_number->m_value > num;
    }

    /**
     * Greater than or equal to operator comparing an unsigned long integer variable.
     *
     * @param num The unsigned long integer variable to be compared.
     * @return True if the value of this is greater than or equal to num's.
     */
    bool operator >= (unsigned long int num) const {
        return m_number->m_value >= num;
    }

    /**
     * Equality operator comparing an unsigned long integer variable.
     *
     * @param num The unsigned long integer variable to be compared.
     * @return True if the value of this is equal to num's.
     */
    bool operator == (unsigned long int num) const {
        return m_number->m_value == num;
    }

    /**
     * Non-equality operator comparing an unsigned long integer variable.
     *
     * @param num The unsigned long integer variable to be compared.
     * @return True if the value of this is not equal to num's.
     */
    bool operator != (unsigned long int num) const {
        return m_number->m_value != num;
    }

    /**
     * Less than operator comparing a long long integer variable.
     *
     * @param num The long long integer variable to be compared.
     * @return True if the value of this is less than num's.
     */
    bool operator < (long long int num) const {
        return m_number->m_value < num;
    }

    /**
     * Less than or equal to operator comparing a long long integer variable.
     *
     * @param num The long long integer variable to be compared.
     * @return True if the value of this is less than or equal to num's.
     */
    bool operator <= (long long int num) const {
        return m_number->m_value <= num;
    }

    /**
     * Greater than operator comparing a long long integer variable.
     *
     * @param num The long long integer variable to be compared.
     * @return True if the value of this is greater than num's.
     */
    bool operator > (long long int num) const {
        return m_number->m_value > num;
    }

    /**
     * Greater than or equal to operator comparing a long long integer variable.
     *
     * @param num The long long integer variable to be compared.
     * @return True if the value of this is greater than or equal to num's.
     */
    bool operator >= (long long int num) const {
        return m_number->m_value >= num;
    }

    /**
     * Equality operator comparing a long long integer variable.
     *
     * @param num The long long integer variable to be compared.
     * @return True if the value of this is equal to num's.
     */
    bool operator == (long long int num) const {
        return m_number->m_value == num;
    }

    /**
     * Non-equality operator comparing a long long integer variable.
     *
     * @param num The long long integer variable to be compared.
     * @return True if the value of this is not equal to num's.
     */
    bool operator != (long long int num) const {
        return m_number->m_value != num;
    }

    /**
     * Less than operator comparing an unsigned long long integer variable.
     *
     * @param num The unsigned long long integer variable to be compared.
     * @return True if the value of this is less than num's.
     */
    bool operator < (unsigned long long int num) const {
        return m_number->m_value < num;
    }

    /**
     * Less than or equal to operator comparing an unsigned long long integer variable.
     *
     * @param num The unsigned long long integer variable to be compared.
     * @return True if the value of this is less than or equal to num's.
     */
    bool operator <= (unsigned long long int num) const {
        return m_number->m_value <= num;
    }

    /**
     * Greater than operator comparing an unsigned long long integer variable.
     *
     * @param num The unsigned long long integer variable to be compared.
     * @return True if the value of this is greater than num's.
     */
    bool operator > (unsigned long long int num) const {
        return m_number->m_value > num;
    }

    /**
     * Greater than or equal to operator comparing an unsigned long long integer variable.
     *
     * @param num The unsigned long long integer variable to be compared.
     * @return True if the value of this is greater than or equal to num's.
     */
    bool operator >= (unsigned long long int num) const {
        return m_number->m_value >= num;
    }

    /**
     * Equality operator comparing an unsigned long long integer variable.
     *
     * @param num The unsigned long long integer variable to be compared.
     * @return True if the value of this is equal to num's.
     */
    bool operator == (unsigned long long int num) const {
        return m_number->m_value == num;
    }

    /**
     * Non-equality operator comparing an unsigned long long integer variable.
     *
     * @param num The unsigned long long integer variable to be compared.
     * @return True if the value of this is not equal to num's.
     */
    bool operator != (unsigned long long int num) const {
        return m_number->m_value != num;
    }

    /**
     * Prints the data of the DoubleHistory object to the output.
     *
     * @param os The output stream to be printed to.
     * @param level The depth level of history operations to be printed.
     */
    void print(std::ostream & os, unsigned int level = 0) const {
        __UNUSED(level);
        os << "VALUE = " << m_number->m_value << std::endl;
        os << "INITIALIZED = " << (m_number->m_initialized ? "true" : "false") << std::endl;
        os << "HISTORY:" << std::endl;
        os << "ADDR: " << m_number << std::endl;
        os << "ENVIRONMENT: " << m_number->m_environment << std::endl;
        if (m_number->m_type == NONE) {
            return;
        }
        os << "\t" << m_number->m_value << " = ";
        switch (m_number->m_type) {
        case ADD:
            os << m_number->m_left->m_value << " + " << m_number->m_right->m_value;
            break;
        case SUB:
            os << m_number->m_left->m_value << " - " << m_number->m_right->m_value;
            break;
        case DIV:
            os << m_number->m_left->m_value << " / " << m_number->m_right->m_value;
            break;
        case MUL:
            os << m_number->m_left->m_value << " * " << m_number->m_right->m_value;
            break;
        case ABS:
            os << "| " << m_number->m_left->m_value << " |";
            break;
        case SQRT:
            os << "sqrt( " << m_number->m_left->m_value << " )";
            break;
        case ROUND:
            os << "round( " << m_number->m_left->m_value << " )";
            break;
        case INV:
            os << " -" << m_number->m_left->m_value;
            break;
        case NONE:
            os << "New object";
            break;
        }
        os << std::endl;

    }

    /**
     * Prints the value of the DoubleHistory object to the output.
     *
     * @param os The output stream to be printed to.
     * @param num The DoubleHistory object to be printed.
     * @return Reference to the output stream with the printed data.
     */
    friend std::ostream & operator << (std::ostream & os, const DoubleHistory & num) {
        os << num.m_number->m_value;
        return os;
    }

    /**
     * Sets the environment to the given value.
     *
     * @param env String describing the new environment.
     */
    static void setEnvironment(const std::string & env) {
        sm_environment = env;
    }

    /**
     * Prints the data of the DoubleHistory object to an XML structure.
     *
     * @param os The output stream to be printed to.
     */
    void writeToXML(std::ostream & os) {
        __UNUSED(os);
    }

    /**
     * Returns the absolute value of the DoubleHistory object.
     * This function is not an inner function of the DoubleHistory class.
     *
     * @param num Reference to the DoubleHistory object.
     * @return The absolute value of the DoubleHistory object.
     */
    friend DoubleHistory _fabs(const DoubleHistory & num) {
        DoubleHistory result;
        result.m_number->m_initialized = num.m_number->m_initialized;
        result.m_number->m_value = ::fabs(num.m_number->m_value);
        result.m_number->m_left = num.m_number;
        result.m_number->m_right = 0;
        result.m_number->m_type = ABS;
        result.m_number->m_environment = sm_environment;

        return result;
    }

    /**
     * Returns the absolute value of the DoubleHistory object.
     *
     * @param num Reference to the DoubleHistory object.
     * @return The absolute value of the DoubleHistory object.
     */
    static DoubleHistory fabs(const DoubleHistory & num) {
        return _fabs(num);
    }

    /**
     * Returns the square root of the DoubleHistory object.
     * This function is not an inner function of the DoubleHistory class.
     *
     * @param num Reference to the DoubleHistory object.
     * @return The square root of the DoubleHistory object.
     */
    friend DoubleHistory _sqrt(const DoubleHistory & num) {
        DoubleHistory result;
        result.m_number->m_initialized = num.m_number->m_initialized;
        result.m_number->m_value = ::sqrt(num.m_number->m_value);
        result.m_number->m_left = num.m_number;
        result.m_number->m_right = 0;
        result.m_number->m_type = SQRT;
        result.m_number->m_environment = sm_environment;

        return result;
    }

    /**
     * Returns the square root of the DoubleHistory object.
     *
     * @param num Reference to the DoubleHistory object.
     * @return The square root of the DoubleHistory object.
     */
    static DoubleHistory sqrt(const DoubleHistory & num) {
        return _sqrt(num);
    }

    /**
     * Returns the rounded value of the DoubleHistory object.
     * This function is not an inner function of the DoubleHistory class.
     *
     * @param num Reference to the DoubleHistory object.
     * @return The rounded value of the DoubleHistory object.
     */
    friend DoubleHistory _round(const DoubleHistory & num) {
        DoubleHistory result;
        result.m_number->m_initialized = num.m_number->m_initialized;
        result.m_number->m_value = ::round(num.m_number->m_value);
        result.m_number->m_left = num.m_number;
        result.m_number->m_right = 0;
        result.m_number->m_type = ROUND;
        result.m_number->m_environment = sm_environment;

        return result;
    }

    /**
     * Returns the rounded value of the DoubleHistory object.
     *
     * @param num Reference to the DoubleHistory object.
     * @return The rounded value of the DoubleHistory object.
     */
    static DoubleHistory round(const DoubleHistory & num) {
        return _round(num);
    }

private:

    /**
     * The DoubleHistory objects' data are stored by reference.
     * This is needed due to multiple references to one number by history
     * data.
     */
    RefObject<Double> m_number;

    /**
     * This static string describes the environment of all the objects.
     */
    static std::string sm_environment;

    /**
     * Initializes the DoubleHistory object to a given value.
     * Helper function of the constructors.
     *
     * @param value The value of the DoubleHistory object to be initialized.
     */
    void init(double value) {
        m_number = RefObject<Double>(new Double);
        m_number->m_initialized = true;
        m_number->m_left = 0;
        m_number->m_right = 0;
        m_number->m_type = NONE;
        m_number->m_value = value;
    }
};




#endif // DOUBLEHISTORY_H
