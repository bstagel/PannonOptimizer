#ifndef MULTIPLEFLOAT_H
#define MULTIPLEFLOAT_H

#include <gmpxx.h>
#include <iostream>

class MultipleFloat {
public:
    MultipleFloat(): m_number(0.0) {

    }

    //template<class NUMBER_TYPE>
    //MultipleFloat(const NUMBER_TYPE & value): m_number(value) {}

    MultipleFloat(float value): m_number(value) { }

    MultipleFloat(double value): m_number(value) { }

    MultipleFloat(long double value): m_number((double)value) { }

    MultipleFloat(int value): m_number(value) { }

    MultipleFloat(unsigned int value): m_number(value) { }

    MultipleFloat(long value): m_number(value) { }

    MultipleFloat(unsigned long value): m_number(value) { }

    operator double() const {
        return m_number.get_d();
    }

    operator float() const {
        return m_number.get_d();
    }

    operator long() const {
        return m_number.get_si();
    }

    operator unsigned long() const {
        return m_number.get_ui();
    }

    operator int() const {
        return m_number.get_si();
    }

    operator unsigned int() const {
        return m_number.get_ui();
    }

    operator char() const {
        return m_number.get_si();
    }

    operator unsigned char() const {
        return m_number.get_ui();
    }

    bool operator>(const MultipleFloat & number) const {
        return m_number > number.m_number;
    }

    bool operator>=(const MultipleFloat & number) const {
        return m_number >= number.m_number;
    }

    bool operator<(const MultipleFloat & number) const {
        return m_number < number.m_number;
    }

    bool operator<=(const MultipleFloat & number) const {
        return m_number <= number.m_number;
    }

    bool operator==(const MultipleFloat & number) const {
        return m_number == number.m_number;
    }

    bool operator!=(const MultipleFloat & number) const {
        return m_number != number.m_number;
    }

    friend bool operator < (double value, const MultipleFloat & number) {
        return value < number.m_number;
    }

    friend bool operator < (char value, const MultipleFloat & number) {
        return value < number.m_number;
    }

    friend bool operator < (int value, const MultipleFloat & number) {
        return value < number.m_number;
    }

    friend bool operator < (unsigned int value, const MultipleFloat & number) {
        return value < number.m_number;
    }

    friend bool operator < (long value, const MultipleFloat & number) {
        return value < number.m_number;
    }

    friend bool operator < (unsigned long value, const MultipleFloat & number) {
        return value < number.m_number;
    }

    friend bool operator < (const MultipleFloat & number, double value) {
        return number.m_number < value;
    }

    friend bool operator < (const MultipleFloat & number, char value) {
        return number.m_number < value;
    }

    friend bool operator < (const MultipleFloat & number, int value) {
        return number.m_number < value;
    }

    friend bool operator < (const MultipleFloat & number, unsigned int value) {
        return number.m_number < value;
    }

    friend bool operator < (const MultipleFloat & number, long value) {
        return number.m_number < value;
    }

    friend bool operator < (const MultipleFloat & number, unsigned long value) {
        return number.m_number < value;
    }

    friend bool operator > (double value, const MultipleFloat & number) {
        return value > number.m_number;
    }

    friend bool operator > (char value, const MultipleFloat & number) {
        return value > number.m_number;
    }

    friend bool operator > (int value, const MultipleFloat & number) {
        return value > number.m_number;
    }

    friend bool operator > (unsigned int value, const MultipleFloat & number) {
        return value > number.m_number;
    }

    friend bool operator > (long value, const MultipleFloat & number) {
        return value > number.m_number;
    }

    friend bool operator > (unsigned long value, const MultipleFloat & number) {
        return value > number.m_number;
    }

    friend bool operator > (const MultipleFloat & number, double value) {
        return number.m_number > value;
    }

    friend bool operator > (const MultipleFloat & number, char value) {
        return number.m_number > value;
    }

    friend bool operator > (const MultipleFloat & number, int value) {
        return number.m_number > value;
    }

    friend bool operator > (const MultipleFloat & number, unsigned int value) {
        return number.m_number > value;
    }

    friend bool operator > (const MultipleFloat & number, long value) {
        return number.m_number > value;
    }

    friend bool operator > (const MultipleFloat & number, unsigned long value) {
        return number.m_number > value;
    }

    template<class NUMBER_TYPE>
    friend bool operator >= (const NUMBER_TYPE & value, const MultipleFloat & number) {
        return value >= number.m_number;
    }

    template<class NUMBER_TYPE>
    friend bool operator >= (const MultipleFloat & number, const NUMBER_TYPE & value) {
        return number.m_number >= value;
    }

    template<class NUMBER_TYPE>
    friend bool operator <= (const MultipleFloat & number, const NUMBER_TYPE & value) {
        return number.m_number <= value;
    }

    template<class NUMBER_TYPE>
    friend bool operator <= (const NUMBER_TYPE & value, const MultipleFloat & number) {
        return value <= number.m_number;
    }

    friend bool operator != (double value, const MultipleFloat & number) {
        return value != number.m_number;
    }

    friend bool operator != (char value, const MultipleFloat & number) {
        return value != number.m_number;
    }

    friend bool operator != (int value, const MultipleFloat & number) {
        return value != number.m_number;
    }

    friend bool operator != (unsigned int value, const MultipleFloat & number) {
        return value != number.m_number;
    }

    friend bool operator != (long value, const MultipleFloat & number) {
        return value != number.m_number;
    }

    friend bool operator != (unsigned long value, const MultipleFloat & number) {
        return value != number.m_number;
    }

    friend bool operator != (const MultipleFloat & number, double value) {
        return number.m_number != value;
    }

    friend bool operator != (const MultipleFloat & number, char value) {
        return number.m_number != value;
    }

    friend bool operator != (const MultipleFloat & number, int value) {
        return number.m_number != value;
    }

    friend bool operator != (const MultipleFloat & number, unsigned int value) {
        return number.m_number != value;
    }

    friend bool operator != (const MultipleFloat & number, long value) {
        return number.m_number != value;
    }

    friend bool operator != (const MultipleFloat & number, unsigned long value) {
        return number.m_number != value;
    }

    friend bool operator == (double value, const MultipleFloat & number) {
        return value == number.m_number;
    }

    friend bool operator == (char value, const MultipleFloat & number) {
        return value == number.m_number;
    }

    friend bool operator == (int value, const MultipleFloat & number) {
        return value == number.m_number;
    }

    friend bool operator == (unsigned int value, const MultipleFloat & number) {
        return value == number.m_number;
    }

    friend bool operator == (long value, const MultipleFloat & number) {
        return value == number.m_number;
    }

    friend bool operator == (unsigned long value, const MultipleFloat & number) {
        return value == number.m_number;
    }


    friend bool operator == (const MultipleFloat & number, double value) {
        return number.m_number == value;
    }

    friend bool operator == (const MultipleFloat & number, char value) {
        return number.m_number == value;
    }

    friend bool operator == (const MultipleFloat & number, int value) {
        return number.m_number == value;
    }

    friend bool operator == (const MultipleFloat & number, unsigned int value) {
        return number.m_number == value;
    }

    friend bool operator == (const MultipleFloat & number, long value) {
        return number.m_number == value;
    }

    friend bool operator == (const MultipleFloat & number, unsigned long value) {
        return number.m_number == value;
    }


    MultipleFloat & operator+=(const MultipleFloat & number) {
        m_number += number.m_number;
        return *this;
    }

    MultipleFloat & operator-=(const MultipleFloat & number) {
        m_number -= number.m_number;
        return *this;
    }

    MultipleFloat & operator*=(const MultipleFloat & number) {
        m_number *= number.m_number;
        return *this;
    }

    MultipleFloat & operator/=(const MultipleFloat & number) {
        m_number /= number.m_number;
        return *this;
    }

    MultipleFloat operator + (const MultipleFloat & number) const {
        MultipleFloat result;
        result.m_number = m_number + number.m_number;
        return result;
    }

    MultipleFloat operator - (const MultipleFloat & number) const {
        MultipleFloat result;
        result.m_number = m_number - number.m_number;
        return result;
    }

    MultipleFloat operator * (const MultipleFloat & number) const {
        MultipleFloat result;
        result.m_number = m_number * number.m_number;
        return result;
    }

    MultipleFloat operator / (const MultipleFloat & number) const {
        MultipleFloat result;
        result.m_number = m_number / number.m_number;
        return result;
    }

    MultipleFloat operator - () const {
        MultipleFloat result;
        result.m_number = -m_number;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator*(const NUMBER_TYPE & value, const MultipleFloat & number) {
        MultipleFloat result;
        result.m_number = value * number.m_number;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator*(const MultipleFloat & number, const NUMBER_TYPE & value) {
        MultipleFloat result;
        result.m_number = number.m_number * value;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator/(const NUMBER_TYPE & value, const MultipleFloat & number) {
        MultipleFloat result;
        result.m_number = value / number.m_number;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator/(const MultipleFloat & number, const NUMBER_TYPE & value) {
        MultipleFloat result;
        result.m_number = number.m_number / value;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator+(const NUMBER_TYPE & value, const MultipleFloat & number) {
        MultipleFloat result;
        result.m_number = value + number.m_number;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator+(const MultipleFloat & number, const NUMBER_TYPE & value) {
        MultipleFloat result;
        result.m_number = number.m_number + value;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator-(const NUMBER_TYPE & value, const MultipleFloat & number) {
        MultipleFloat result;
        result.m_number = value - number.m_number;
        return result;
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator-(const MultipleFloat & number, const NUMBER_TYPE & value) {
        MultipleFloat result;
        result.m_number = number.m_number - value;
        return result;
    }

    static MultipleFloat abs(const MultipleFloat & number) {
        MultipleFloat result;
        result.m_number = ::abs(number.m_number);
        return result;
    }

    static MultipleFloat sqrt(const MultipleFloat & number) {
        MultipleFloat result;
        result.m_number = ::sqrt(number.m_number);
        return result;
    }

    static MultipleFloat log10(const MultipleFloat & number) {
        MultipleFloat result;
        result.m_number = ::log10(number.m_number.get_d());
        return result;
    }

    static MultipleFloat round(const MultipleFloat & number) {
        MultipleFloat result;
        int sign = sgn(number.m_number);
        result.m_number = ::abs(number.m_number);
        if (result.m_number - trunc(result.m_number) < 0.5) {
            result.m_number = trunc(result.m_number);
        } else {
            result.m_number = trunc(result.m_number) + 1.0;
        }
        result.m_number = result.m_number.get_si() * (sign == 1 ? -1 : 1);
        return result;
    }

    friend std::ostream & operator<<(std::ostream & os, const MultipleFloat & number) {
        os << number.m_number;
        return os;
    }

    std::string getString() const {
        mp_exp_t exp;
        return m_number.get_str(exp);
    }

    static MultipleFloat pow(const MultipleFloat & x, const MultipleFloat & y) {
        MultipleFloat result;
        result = pow((double)x, (double)y);
        return result;
    }

private:

    mpf_class m_number;
};

namespace std {
template<>
  struct is_floating_point<MultipleFloat>
  //: public integral_constant<bool, (__is_floating_point_helper<typename
    //                remove_cv<_Tp>::type>::value)>
  {
      static constexpr bool value = true;
      typedef integral_constant<bool, value> type;
      constexpr operator bool() { return value; }
  };

  inline string
  to_string(MultipleFloat __val)
  {
      return __val.getString();
  }
}

#endif // MULTIPLEFLOAT_H
