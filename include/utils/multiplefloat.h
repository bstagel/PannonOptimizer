#ifndef MULTIPLEFLOAT_H
#define MULTIPLEFLOAT_H

#include <double.h>

#if DOUBLE_TYPE == DOUBLE_GMP

#include <gmpxx.h>
#include <iostream>
#include <limits>
#include <cmath>

enum GMP_TYPE_ENUM {
    GMP_FLOAT,
    GMP_RATIONAL
};

#define DEFAULT_DEBUG_MODE true
extern bool ENABLE_MULTIPLE_PRECISION;
extern GMP_TYPE_ENUM GMP_TYPE;

class MultipleFloat {
private:
    void check() const;
public:
    MultipleFloat();

    //template<class NUMBER_TYPE>
    //MultipleFloat(const NUMBER_TYPE & value): M_NUMBER(value) {}

    MultipleFloat(float value);

    MultipleFloat(double value);

    MultipleFloat(long double value);

    MultipleFloat(short value);

    MultipleFloat(unsigned short value);

    MultipleFloat(int value);

    MultipleFloat(unsigned int value);

    MultipleFloat(long value);

    MultipleFloat(unsigned long value);

    MultipleFloat(long long value);

    MultipleFloat(unsigned long long value);

    /*operator double() const {
        if (ENABLE_MULTIPLE_PRECISION) {
            if (m_infinitySymbol != 0.0) {
                return m_infinitySymbol;
            }
            return m_number.get_d();
        } else {
            return m_value;
        }
    }

    operator float() const {
        if (ENABLE_MULTIPLE_PRECISION) {
            if (m_infinitySymbol != 0.0) {
                return m_infinitySymbol;
            }
            return m_number.get_d();
        } else {
            return m_value;
        }
    }*/

    operator long() const;

    operator unsigned long() const;

    operator int() const;

    operator unsigned int() const;

    operator char() const;

    operator unsigned char() const;

    bool operator>(const MultipleFloat & number) const;

    bool operator>=(const MultipleFloat & number) const;

    bool operator<(const MultipleFloat & number) const;

    bool operator<=(const MultipleFloat & number) const;

    bool operator==(const MultipleFloat & number) const;

    bool operator!=(const MultipleFloat & number) const;

    friend bool operator < (float value, const MultipleFloat & number);

    friend bool operator < (double value, const MultipleFloat & number);

    friend bool operator < (long double value, const MultipleFloat & number);

    friend bool operator < (char value, const MultipleFloat & number);

    friend bool operator < (unsigned char value, const MultipleFloat & number);

    friend bool operator < (short value, const MultipleFloat & number);

    friend bool operator < (unsigned short value, const MultipleFloat & number);

    friend bool operator < (int value, const MultipleFloat & number);

    friend bool operator < (unsigned int value, const MultipleFloat & number);

    friend bool operator < (long value, const MultipleFloat & number);

    friend bool operator < (unsigned long value, const MultipleFloat & number);

    friend bool operator < (long long value, const MultipleFloat & number);

    friend bool operator < (unsigned long long value, const MultipleFloat & number);

    friend bool operator < (const MultipleFloat & number, float value);

    friend bool operator < (const MultipleFloat & number, double value);

    friend bool operator < (const MultipleFloat & number, long double value);

    friend bool operator < (const MultipleFloat & number, char value);

    friend bool operator < (const MultipleFloat & number, unsigned char value);

    friend bool operator < (const MultipleFloat & number, short value);

    friend bool operator < (const MultipleFloat & number, unsigned short value);

    friend bool operator < (const MultipleFloat & number, int value);

    friend bool operator < (const MultipleFloat & number, unsigned int value);

    friend bool operator < (const MultipleFloat & number, long value);

    friend bool operator < (const MultipleFloat & number, unsigned long value);

    friend bool operator < (const MultipleFloat & number, long long value);

    friend bool operator < (const MultipleFloat & number, unsigned long long value);

    friend bool operator > (float value, const MultipleFloat & number);

    friend bool operator > (double value, const MultipleFloat & number);

    friend bool operator > (long double value, const MultipleFloat & number);

    friend bool operator > (char value, const MultipleFloat & number);

    friend bool operator > (unsigned char value, const MultipleFloat & number);

    friend bool operator > (short value, const MultipleFloat & number);

    friend bool operator > (unsigned short value, const MultipleFloat & number);

    friend bool operator > (int value, const MultipleFloat & number);

    friend bool operator > (unsigned int value, const MultipleFloat & number);

    friend bool operator > (long value, const MultipleFloat & number);

    friend bool operator > (unsigned long value, const MultipleFloat & number);

    friend bool operator > (long long value, const MultipleFloat & number);

    friend bool operator > (unsigned long long value, const MultipleFloat & number);

    friend bool operator > (const MultipleFloat & number, float value);

    friend bool operator > (const MultipleFloat & number, double value);

    friend bool operator > (const MultipleFloat & number, long double value);

    friend bool operator > (const MultipleFloat & number, char value);

    friend bool operator > (const MultipleFloat & number, unsigned char value);

    friend bool operator > (const MultipleFloat & number, short value);

    friend bool operator > (const MultipleFloat & number, unsigned short value);

    friend bool operator > (const MultipleFloat & number, int value);

    friend bool operator > (const MultipleFloat & number, unsigned int value);

    friend bool operator > (const MultipleFloat & number, long value);

    friend bool operator > (const MultipleFloat & number, unsigned long value);

    friend bool operator > (const MultipleFloat & number, long long value);

    friend bool operator > (const MultipleFloat & number, unsigned long long value);

    friend bool operator >= (const float & value, const MultipleFloat & number);

    friend bool operator >= (const double & value, const MultipleFloat & number);

    friend bool operator >= (const long double & value, const MultipleFloat & number);

    friend bool operator >= (const char & value, const MultipleFloat & number);

    friend bool operator >= (const unsigned char & value, const MultipleFloat & number);

    friend bool operator >= (const short & value, const MultipleFloat & number);

    friend bool operator >= (const unsigned short & value, const MultipleFloat & number);

    friend bool operator >= (const int & value, const MultipleFloat & number);

    friend bool operator >= (const unsigned int & value, const MultipleFloat & number);

    friend bool operator >= (const long & value, const MultipleFloat & number);

    friend bool operator >= (const unsigned long & value, const MultipleFloat & number);

    friend bool operator >= (const long long & value, const MultipleFloat & number);

    friend bool operator >= (const unsigned long long & value, const MultipleFloat & number);

    friend bool operator >= (const MultipleFloat & number, const float & value);

    friend bool operator >= (const MultipleFloat & number, const double & value);

    friend bool operator >= (const MultipleFloat & number, const long double & value);

    friend bool operator >= (const MultipleFloat & number, const char & value);

    friend bool operator >= (const MultipleFloat & number, const unsigned char & value);

    friend bool operator >= (const MultipleFloat & number, const short & value);

    friend bool operator >= (const MultipleFloat & number, const unsigned short & value);

    friend bool operator >= (const MultipleFloat & number, const int & value);

    friend bool operator >= (const MultipleFloat & number, const unsigned int & value);

    friend bool operator >= (const MultipleFloat & number, const long & value);

    friend bool operator >= (const MultipleFloat & number, const unsigned long & value);

    friend bool operator >= (const MultipleFloat & number, const long long & value);

    friend bool operator >= (const MultipleFloat & number, const unsigned long long & value);

    friend bool operator <= (const MultipleFloat & number, const float & value);

    friend bool operator <= (const MultipleFloat & number, const double & value);

    friend bool operator <= (const MultipleFloat & number, const long double & value);

    friend bool operator <= (const MultipleFloat & number, const char & value);

    friend bool operator <= (const MultipleFloat & number, const unsigned char & value);

    friend bool operator <= (const MultipleFloat & number, const short & value);

    friend bool operator <= (const MultipleFloat & number, const unsigned short & value);

    friend bool operator <= (const MultipleFloat & number, const int & value);

    friend bool operator <= (const MultipleFloat & number, const unsigned int & value);

    friend bool operator <= (const MultipleFloat & number, const long & value);

    friend bool operator <= (const MultipleFloat & number, const unsigned long & value);

    friend bool operator <= (const MultipleFloat & number, const long long & value);

    friend bool operator <= (const MultipleFloat & number, const unsigned long long & value);

    friend bool operator <= (const float & value, const MultipleFloat & number);

    friend bool operator <= (const double & value, const MultipleFloat & number);

    friend bool operator <= (const long double & value, const MultipleFloat & number);

    friend bool operator <= (const char & value, const MultipleFloat & number);

    friend bool operator <= (const unsigned char & value, const MultipleFloat & number);

    friend bool operator <= (const short & value, const MultipleFloat & number);

    friend bool operator <= (const unsigned short & value, const MultipleFloat & number);

    friend bool operator <= (const int & value, const MultipleFloat & number);

    friend bool operator <= (const unsigned int & value, const MultipleFloat & number);

    friend bool operator <= (const long & value, const MultipleFloat & number);

    friend bool operator <= (const unsigned long & value, const MultipleFloat & number);

    friend bool operator <= (const long long & value, const MultipleFloat & number);

    friend bool operator <= (const unsigned long long & value, const MultipleFloat & number);

    friend bool operator != (float value, const MultipleFloat & number);

    friend bool operator != (double value, const MultipleFloat & number);

    friend bool operator != (long double value, const MultipleFloat & number);

    friend bool operator != (char value, const MultipleFloat & number);

    friend bool operator != (unsigned char value, const MultipleFloat & number);

    friend bool operator != (short value, const MultipleFloat & number);

    friend bool operator != (unsigned short value, const MultipleFloat & number);

    friend bool operator != (int value, const MultipleFloat & number);

    friend bool operator != (unsigned int value, const MultipleFloat & number);

    friend bool operator != (long value, const MultipleFloat & number);

    friend bool operator != (unsigned long value, const MultipleFloat & number);

    friend bool operator != (long long value, const MultipleFloat & number);

    friend bool operator != (unsigned long long value, const MultipleFloat & number);

    friend bool operator != (const MultipleFloat & number, float value);

    friend bool operator != (const MultipleFloat & number, double value);

    friend bool operator != (const MultipleFloat & number, long double value);

    friend bool operator != (const MultipleFloat & number, char value);

    friend bool operator != (const MultipleFloat & number, unsigned char value);

    friend bool operator != (const MultipleFloat & number, short value);

    friend bool operator != (const MultipleFloat & number, unsigned short value);

    friend bool operator != (const MultipleFloat & number, int value);

    friend bool operator != (const MultipleFloat & number, unsigned int value);

    friend bool operator != (const MultipleFloat & number, long value);

    friend bool operator != (const MultipleFloat & number, unsigned long value);

    friend bool operator != (const MultipleFloat & number, long long value);

    friend bool operator != (const MultipleFloat & number, unsigned long long value);

    friend bool operator == (float value, const MultipleFloat & number);

    friend bool operator == (double value, const MultipleFloat & number);

    friend bool operator == (long double value, const MultipleFloat & number);

    friend bool operator == (char value, const MultipleFloat & number);

    friend bool operator == (unsigned char value, const MultipleFloat & number);

    friend bool operator == (short value, const MultipleFloat & number);

    friend bool operator == (unsigned short value, const MultipleFloat & number);

    friend bool operator == (int value, const MultipleFloat & number);

    friend bool operator == (unsigned int value, const MultipleFloat & number);

    friend bool operator == (long value, const MultipleFloat & number);

    friend bool operator == (unsigned long value, const MultipleFloat & number);

    friend bool operator == (long long value, const MultipleFloat & number);

    friend bool operator == (unsigned long long value, const MultipleFloat & number);

    friend bool operator == (const MultipleFloat & number, float value);

    friend bool operator == (const MultipleFloat & number, double value);

    friend bool operator == (const MultipleFloat & number, long double value);

    friend bool operator == (const MultipleFloat & number, char value);

    friend bool operator == (const MultipleFloat & number, unsigned char value);

    friend bool operator == (const MultipleFloat & number, short value);

    friend bool operator == (const MultipleFloat & number, unsigned short value);

    friend bool operator == (const MultipleFloat & number, int value);

    friend bool operator == (const MultipleFloat & number, unsigned int value);

    friend bool operator == (const MultipleFloat & number, long value);

    friend bool operator == (const MultipleFloat & number, unsigned long value);

    friend bool operator == (const MultipleFloat & number, long long value);

    friend bool operator == (const MultipleFloat & number, unsigned long long value);

    MultipleFloat &operator=(const MultipleFloat & number);

    MultipleFloat &operator+=(const MultipleFloat & number);

    MultipleFloat & operator-=(const MultipleFloat & number);

    MultipleFloat & operator*=(const MultipleFloat & number);

    MultipleFloat & operator/=(const MultipleFloat & number);

    MultipleFloat operator + (const MultipleFloat & number) const;

    MultipleFloat operator - (const MultipleFloat & number) const;

    MultipleFloat operator * (const MultipleFloat & number) const;

    MultipleFloat operator / (const MultipleFloat & number) const;

    MultipleFloat operator - () const;

    template<class NUMBER_TYPE>
    friend MultipleFloat operator*(const NUMBER_TYPE & value, const MultipleFloat & number) {
        if (ENABLE_MULTIPLE_PRECISION) {
            MultipleFloat result;

            bool normal1 = !std::isinf(value) && !std::isnan(value);
            bool normal2 = number.m_infinitySymbol == 0.0;

            if (normal1) {
                if (normal2) {
                    // each normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = (decltype(m_numberF))value * number.m_numberF;
                    } else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = (decltype(m_numberR))value * number.m_numberR;
                    }
                    result.m_infinitySymbol = 0.0;
                } else {
                    // normal * not normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                    }
                    result.m_infinitySymbol = value * number.m_infinitySymbol;
                }
            } else {
                if (normal2) {
                    // not normal * normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                        result.m_infinitySymbol = value * number.m_numberF.get_d();
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                        result.m_infinitySymbol = value * number.m_numberR.get_d();
                    }
                } else {
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                        result.m_infinitySymbol = value * number.m_infinitySymbol;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                        result.m_infinitySymbol = value * number.m_infinitySymbol;
                    }
                }
            }

            result.m_debug = number.m_debug;
            result.check();
            return result;
        } else {
            MultipleFloat result(value * number.m_value);
            return result;
        }

    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator*(const MultipleFloat & number, const NUMBER_TYPE & value) {
        if (ENABLE_MULTIPLE_PRECISION) {
            MultipleFloat result;
            bool normal1 = !std::isinf(value) && !std::isnan(value);
            bool normal2 = number.m_infinitySymbol == 0.0;

            if (normal1) {
                if (normal2) {
                    // each normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = number.m_numberF * (decltype(m_numberF))value;
                        result.m_infinitySymbol = 0.0;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = number.m_numberR * (decltype(m_numberR))value;
                        result.m_infinitySymbol = 0.0;
                    }
                } else {
                    // normal * not normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                        result.m_infinitySymbol = number.m_infinitySymbol * value;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                        result.m_infinitySymbol = number.m_infinitySymbol * value;
                    }
                }
            } else {
                if (normal2) {
                    // not normal * normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                        result.m_infinitySymbol = number.m_numberF.get_d() * value;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                        result.m_infinitySymbol = number.m_numberR.get_d() * value;
                    }
                } else {
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                        result.m_infinitySymbol = number.m_infinitySymbol * value;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                        result.m_infinitySymbol = number.m_infinitySymbol * value;
                    }
                }
            }
            result.m_debug = number.m_debug;
            result.check();
            return result;
        } else {
            MultipleFloat result(number.m_value * value);
            return result;
        }

    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator/(const NUMBER_TYPE & value, const MultipleFloat & number) {
        if (ENABLE_MULTIPLE_PRECISION) {
            MultipleFloat result;

            // minden normal
            bool normal1 = !std::isinf(value) && !std::isnan(value);
            bool normal2 = number.m_infinitySymbol == 0.0;
            if (normal1) {
                if (normal2) {
                    if (number != 0.0) {
                        if (GMP_TYPE == GMP_FLOAT) {
                            result.m_numberF = (decltype(m_numberF))value / number.m_numberF;
                        }else if (GMP_TYPE == GMP_RATIONAL) {
                            result.m_numberR = (decltype(m_numberR))value / number.m_numberR;
                        }
                    } else {
                        if (GMP_TYPE == GMP_FLOAT) {
                            result.m_infinitySymbol = value / number.m_numberF.get_d();
                        }else if (GMP_TYPE == GMP_RATIONAL) {
                            result.m_infinitySymbol = value / number.m_numberR.get_d();
                        }
                    }
                } else {
                    result.m_infinitySymbol = value / number.m_infinitySymbol;
                }
            } else {
                if (normal2) {
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_infinitySymbol = value / number.m_numberF.get_d();
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_infinitySymbol = value / number.m_numberR.get_d();
                    }
                } else {
                    result.m_infinitySymbol = value / number.m_infinitySymbol;
                }
            }

            result.m_debug = number.m_debug;
            result.check();
            return result;
        } else {
            MultipleFloat result(value / number.m_value);
            return result;
        }

    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator/(const MultipleFloat & number, const NUMBER_TYPE & value) {
        if (ENABLE_MULTIPLE_PRECISION) {
            MultipleFloat result;
            bool normal1 = !std::isinf(value) && !std::isnan(value);
            bool normal2 = number.m_infinitySymbol == 0.0;
            if (normal1) {
                if (normal2) {
                    if (value != 0.0) {
                        if (GMP_TYPE == GMP_FLOAT) {
                            result.m_numberF = number.m_numberF / (decltype(m_numberF))value;
                        }else if (GMP_TYPE == GMP_RATIONAL) {
                            result.m_numberR = number.m_numberR / (decltype(m_numberR))value;
                        }
                    } else {
                        if (GMP_TYPE == GMP_FLOAT) {
                            result.m_infinitySymbol = number.m_numberF.get_d() / value;
                        }else if (GMP_TYPE == GMP_RATIONAL) {
                            result.m_infinitySymbol = number.m_numberR.get_d() / value;
                        }
                    }
                } else {
                    result.m_infinitySymbol = number.m_infinitySymbol / value;
                }
            } else {
                if (normal2) {
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_infinitySymbol = number.m_numberF.get_d() / value;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_infinitySymbol = number.m_numberR.get_d() / value;
                    }

                } else {
                    result.m_infinitySymbol = number.m_infinitySymbol / value;
                }
            }
            result.m_debug = number.m_debug;
            result.check();
            return result;
        } else {
            MultipleFloat result(number.m_value / value);
            return result;
        }

    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator+(const NUMBER_TYPE & value, const MultipleFloat & number) {
        if (ENABLE_MULTIPLE_PRECISION) {
            MultipleFloat result;
            bool normal1 = !std::isinf(value) && !std::isnan(value);
            bool normal2 = number.m_infinitySymbol == 0.0;

            if (normal1) {
                if (normal2) {
                    // each normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = number.m_numberF + (decltype(m_numberF))value;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = number.m_numberR + (decltype(m_numberR))value;
                    }
                    result.m_infinitySymbol = 0.0;
                } else {
                    // normal * not normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                    }
                    result.m_infinitySymbol = number.m_infinitySymbol + value;
                }
            } else {
                if (normal2) {
                    // not normal * normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                        result.m_infinitySymbol = number.m_numberF.get_d() + value;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                        result.m_infinitySymbol = number.m_numberR.get_d() + value;
                    }
                } else {
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                    }
                    result.m_infinitySymbol = number.m_infinitySymbol + value;
                }
            }
            result.m_debug = number.m_debug;
            result.check();
            return result;
        } else {
            MultipleFloat result(value + number.m_value);
            return result;
        }

    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator+(const MultipleFloat & number, const NUMBER_TYPE & value) {
        if (ENABLE_MULTIPLE_PRECISION) {
            MultipleFloat result;
            bool normal1 = !std::isinf(value) && !std::isnan(value);
            bool normal2 = number.m_infinitySymbol == 0.0;

            if (normal1) {
                if (normal2) {
                    // each normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = number.m_numberF + (decltype(m_numberF))value;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = number.m_numberR + (decltype(m_numberR))value;
                    }
                    result.m_infinitySymbol = 0.0;
                } else {
                    // normal * not normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                    }
                    result.m_infinitySymbol = number.m_infinitySymbol + value;
                }
            } else {
                if (normal2) {
                    // not normal * normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                        result.m_infinitySymbol = number.m_numberF.get_d() + value;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                        result.m_infinitySymbol = number.m_numberR.get_d() + value;
                    }
                } else {
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                    }
                    result.m_infinitySymbol = number.m_infinitySymbol + value;
                }
            }
            result.m_debug = number.m_debug;
            result.check();
            return result;
        } else {
            MultipleFloat result(number.m_value + value);
            return result;
        }
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator-(const NUMBER_TYPE & value, const MultipleFloat & number) {
        if (ENABLE_MULTIPLE_PRECISION) {
            MultipleFloat result;
            bool normal1 = !std::isinf(value) && !std::isnan(value);
            bool normal2 = number.m_infinitySymbol == 0.0;

            if (normal1) {
                if (normal2) {
                    // each normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = (decltype(m_numberF))value - number.m_numberF;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = (decltype(m_numberR))value - number.m_numberR;
                    }
                    result.m_infinitySymbol = 0.0;
                } else {
                    // normal * not normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                    }
                    result.m_infinitySymbol = value - number.m_infinitySymbol;
                }
            } else {
                if (normal2) {
                    // not normal * normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                        result.m_infinitySymbol = value - number.m_numberF.get_d();
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                        result.m_infinitySymbol = value - number.m_numberR.get_d();
                    }
                } else {
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                    }
                    result.m_infinitySymbol = value - number.m_infinitySymbol;
                }
            }
            result.m_debug = number.m_debug;
            result.check();
            return result;
        } else {
            MultipleFloat result(value - number.m_value);
            return result;
        }
    }

    template<class NUMBER_TYPE>
    friend MultipleFloat operator-(const MultipleFloat & number, const NUMBER_TYPE & value) {
        if (ENABLE_MULTIPLE_PRECISION) {
            MultipleFloat result;
            bool normal1 = !std::isinf(value) && !std::isnan(value);
            bool normal2 = number.m_infinitySymbol == 0.0;

            if (normal1) {
                if (normal2) {
                    // each normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = number.m_numberF - (decltype(m_numberF))value;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = number.m_numberR - (decltype(m_numberR))value;
                    }
                    result.m_infinitySymbol = 0.0;
                } else {
                    // normal * not normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                    }
                    result.m_infinitySymbol = number.m_infinitySymbol - value;
                }
            } else {
                if (normal2) {
                    // not normal * normal
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                        result.m_infinitySymbol = number.m_numberF.get_d() - value;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                        result.m_infinitySymbol = number.m_numberR.get_d() - value;
                    }
                } else {
                    if (GMP_TYPE == GMP_FLOAT) {
                        result.m_numberF = 0.0;
                    }else if (GMP_TYPE == GMP_RATIONAL) {
                        result.m_numberR = 0.0;
                    }
                    result.m_infinitySymbol = number.m_infinitySymbol - value;
                }
            }
            result.m_debug = number.m_debug;
            result.check();
            return result;
        } else {
            MultipleFloat result(number.m_value - value);
            return result;
        }
    }

    static MultipleFloat abs(const MultipleFloat & number);

    static MultipleFloat sqrt(const MultipleFloat & number);

    static MultipleFloat log10(const MultipleFloat & number);

    static MultipleFloat round(const MultipleFloat & number);

    friend std::ostream & operator<<(std::ostream & os, const MultipleFloat & number);

    std::string getString() const;

    static MultipleFloat pow(const MultipleFloat & x, const MultipleFloat & y);

    double getDouble() const;

    void setDebugMode(bool flag);

private:

    //#if GMP_TYPE == GMP_FLOAT
    mpf_class m_numberF;
    //#endif
    //#if GMP_TYPE == GMP_RATIONAL
    mpq_class m_numberR;
    //#endif

    double m_value;
    double m_infinitySymbol;
    bool m_debug;

};

namespace std {
std::string to_string(const MultipleFloat & mf);
}

/*namespace std {
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
}*/

#endif //#if DOUBLE_TYPE == DOUBLE_GMP

#endif // MULTIPLEFLOAT_H
