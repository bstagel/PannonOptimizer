#ifndef DOUBLEHISTORY_H
#define DOUBLEHISTORY_H

#include <globals.h>
#include <cmath>
#include <iostream>
#include <string>
#include <debug.h>
#include <utils/refobject.h>

class DoubleHistory {
public:
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

    struct Double {
        double m_value;
        RefObject<Double> m_left;
        RefObject<Double> m_right;
        std::string m_environment;
        OPERATION_TYPE m_type;
        bool m_initialized;
    };

    DoubleHistory() {
        m_number = RefObject<Double>(new Double);
        m_number->m_initialized = false;
        m_number->m_left = 0;
        m_number->m_right = 0;
        m_number->m_type = NONE;
    }

    DoubleHistory(double value) {
        init(value);
    }

    DoubleHistory(int value) {
        init(value);
    }

    DoubleHistory(unsigned int value) {
        init(value);
    }

    DoubleHistory(long int value) {
        init(value);
    }

    DoubleHistory(long long int value) {
        init(value);
    }

    DoubleHistory(unsigned long int value) {
        init(value);
    }

    DoubleHistory(unsigned long long int value) {
        init(value);
    }

    DoubleHistory(const DoubleHistory & orig) {
        m_number = RefObject<Double>(new Double);
        m_number->m_initialized = orig.m_number->m_initialized;
        m_number->m_left = orig.m_number->m_left;
        m_number->m_right = orig.m_number->m_right;
        m_number->m_type = orig.m_number->m_type;
        m_number->m_value = orig.m_number->m_value;
    }

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

    operator int() const {
        return (int)m_number->m_value;
    }

    operator unsigned int() const {
        return (unsigned int)m_number->m_value;
    }

    operator double() const {
        return m_number->m_value;
    }

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

    DoubleHistory operator + (double right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    DoubleHistory operator + (int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    DoubleHistory operator + (unsigned int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    DoubleHistory operator + (long int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    DoubleHistory operator + (unsigned long int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    DoubleHistory operator + (long long int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    DoubleHistory operator + (unsigned long long int right) const {
        DoubleHistory rightSide = right;
        return *this + rightSide;
    }

    friend DoubleHistory operator + (double left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    friend DoubleHistory operator + (int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    friend DoubleHistory operator + (long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    friend DoubleHistory operator + (long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    friend DoubleHistory operator + (unsigned int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    friend DoubleHistory operator + (unsigned long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

    friend DoubleHistory operator + (unsigned long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide + right;
    }

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

    DoubleHistory operator - (double right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    DoubleHistory operator - (int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    DoubleHistory operator - (unsigned int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    DoubleHistory operator - (long int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    DoubleHistory operator - (unsigned long int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    DoubleHistory operator - (long long int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

    DoubleHistory operator - (unsigned long long int right) const {
        DoubleHistory rightSide = right;
        return *this - rightSide;
    }

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

    friend DoubleHistory operator - (double left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    friend DoubleHistory operator - (int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    friend DoubleHistory operator - (long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    friend DoubleHistory operator - (long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    friend DoubleHistory operator - (unsigned int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    friend DoubleHistory operator - (unsigned long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

    friend DoubleHistory operator - (unsigned long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide - right;
    }

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

    DoubleHistory operator * (double right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    DoubleHistory operator * (int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    DoubleHistory operator * (unsigned int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    DoubleHistory operator * (long int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    DoubleHistory operator * (unsigned long int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    DoubleHistory operator * (long long int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

    DoubleHistory operator * (unsigned long long int right) const {
        DoubleHistory rightSide = right;
        return *this * rightSide;
    }

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

    friend DoubleHistory operator * (double left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    friend DoubleHistory operator * (int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    friend DoubleHistory operator * (long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    friend DoubleHistory operator * (long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    friend DoubleHistory operator * (unsigned int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    friend DoubleHistory operator * (unsigned long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

    friend DoubleHistory operator * (unsigned long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide * right;
    }

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

    DoubleHistory operator / (double right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    DoubleHistory operator / (int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    DoubleHistory operator / (unsigned int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    DoubleHistory operator / (long int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    DoubleHistory operator / (unsigned long int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    DoubleHistory operator / (long long int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    DoubleHistory operator / (unsigned long long int right) const {
        DoubleHistory rightSide = right;
        return *this / rightSide;
    }

    friend DoubleHistory operator / (double left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    friend DoubleHistory operator / (int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    friend DoubleHistory operator / (long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    friend DoubleHistory operator / (long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    friend DoubleHistory operator / (unsigned int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    friend DoubleHistory operator / (unsigned long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

    friend DoubleHistory operator / (unsigned long long int left, const DoubleHistory & right) {
        DoubleHistory leftSide = left;
        return leftSide / right;
    }

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

    bool operator < (const DoubleHistory & num) const {
        return m_number->m_value < num.m_number->m_value;
    }

    bool operator <= (const DoubleHistory & num) const {
        return m_number->m_value <= num.m_number->m_value;
    }

    bool operator > (const DoubleHistory & num) const {
        return m_number->m_value > num.m_number->m_value;
    }

    bool operator >= (const DoubleHistory & num) const {
        return m_number->m_value >= num.m_number->m_value;
    }

    bool operator == (const DoubleHistory & num) const {
        return m_number->m_value == num.m_number->m_value;
    }

    bool operator != (const DoubleHistory & num) const {
        return m_number->m_value != num.m_number->m_value;
    }

    bool operator < (double num) const {
        return m_number->m_value < num;
    }

    bool operator <= (double num) const {
        return m_number->m_value <= num;
    }

    bool operator > (double num) const {
        return m_number->m_value > num;
    }

    bool operator >= (double num) const {
        return m_number->m_value >= num;
    }

    bool operator == (double num) const {
        return m_number->m_value == num;
    }

    bool operator != (double num) const {
        return m_number->m_value != num;
    }

    bool operator < (int num) const {
        return m_number->m_value < num;
    }

    bool operator <= (int num) const {
        return m_number->m_value <= num;
    }

    bool operator > (int num) const {
        return m_number->m_value > num;
    }

    bool operator >= (int num) const {
        return m_number->m_value >= num;
    }

    bool operator == (int num) const {
        return m_number->m_value == num;
    }

    bool operator != (int num) const {
        return m_number->m_value != num;
    }

    bool operator < (unsigned int num) const {
        return m_number->m_value < num;
    }

    bool operator <= (unsigned int num) const {
        return m_number->m_value <= num;
    }

    bool operator > (unsigned int num) const {
        return m_number->m_value > num;
    }

    bool operator >= (unsigned int num) const {
        return m_number->m_value >= num;
    }

    bool operator == (unsigned int num) const {
        return m_number->m_value == num;
    }

    bool operator != (unsigned int num) const {
        return m_number->m_value != num;
    }

    bool operator < (long int num) const {
        return m_number->m_value < num;
    }

    bool operator <= (long int num) const {
        return m_number->m_value <= num;
    }

    bool operator > (long int num) const {
        return m_number->m_value > num;
    }

    bool operator >= (long int num) const {
        return m_number->m_value >= num;
    }

    bool operator == (long int num) const {
        return m_number->m_value == num;
    }

    bool operator != (long int num) const {
        return m_number->m_value != num;
    }

    bool operator < (unsigned long int num) const {
        return m_number->m_value < num;
    }

    bool operator <= (unsigned long int num) const {
        return m_number->m_value <= num;
    }

    bool operator > (unsigned long int num) const {
        return m_number->m_value > num;
    }

    bool operator >= (unsigned long int num) const {
        return m_number->m_value >= num;
    }

    bool operator == (unsigned long int num) const {
        return m_number->m_value == num;
    }

    bool operator != (unsigned long int num) const {
        return m_number->m_value != num;
    }

    bool operator < (long long int num) const {
        return m_number->m_value < num;
    }

    bool operator <= (long long int num) const {
        return m_number->m_value <= num;
    }

    bool operator > (long long int num) const {
        return m_number->m_value > num;
    }

    bool operator >= (long long int num) const {
        return m_number->m_value >= num;
    }

    bool operator == (long long int num) const {
        return m_number->m_value == num;
    }

    bool operator != (long long int num) const {
        return m_number->m_value != num;
    }

    bool operator < (unsigned long long int num) const {
        return m_number->m_value < num;
    }

    bool operator <= (unsigned long long int num) const {
        return m_number->m_value <= num;
    }

    bool operator > (unsigned long long int num) const {
        return m_number->m_value > num;
    }

    bool operator >= (unsigned long long int num) const {
        return m_number->m_value >= num;
    }

    bool operator == (unsigned long long int num) const {
        return m_number->m_value == num;
    }

    bool operator != (unsigned long long int num) const {
        return m_number->m_value != num;
    }

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

    friend std::ostream & operator << (std::ostream & os, const DoubleHistory & num) {
        os << num.m_number->m_value;
        return os;
    }

    static void setEnvironment(const std::string & env) {
        sm_environment = env;
    }

    void writeToXML(std::ostream & os) {
        __UNUSED(os);
    }

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

    static DoubleHistory fabs(const DoubleHistory & num) {
        return _fabs(num);
    }

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

    static DoubleHistory sqrt(const DoubleHistory & num) {
        return _sqrt(num);
    }

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

    static DoubleHistory round(const DoubleHistory & num) {
        return _round(num);
    }

private:

    RefObject<Double> m_number;

    static std::string sm_environment;

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
