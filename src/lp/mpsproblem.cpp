/**
 * @file mpsproblem.cpp
 */

#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <map>
#include <cmath>

#include <lp/mpsproblem.h>
#include <debug.h>
#include <utils/numerical.h>
#include <utils/timer.h>


template <int type>
unsigned int MpsModelBuilder::MpsError<type>::sm_count = 0;
template <int type>
unsigned int MpsModelBuilder::MpsError<type>::sm_limit = 0;

unsigned int MpsModelBuilder::MpsErrorData::sm_warningCount = 0;
unsigned int MpsModelBuilder::MpsErrorData::sm_errorCount = 0;
unsigned int MpsModelBuilder::MpsErrorData::sm_fatalErrorCount = 0;

const char * MpsModelBuilder::sm_expectedSection[] = {
    "ROWS", "COLUMNS", "RHS", "RANGES, BOUNDS or ENDATA",
    "BOUNDS or ENDATA", "ENDATA"
};

//#define CHECK_PARSE_DOUBLE
//TODO: utolso bitben valo elteres javitasa

std::string MpsModelBuilder::nameToString(Name name) {
    char str[9];
    memcpy(str, &name, sizeof(name));
    str[8] = 0;
    return std::string(str);
}

inline const char * MpsModelBuilder::parseDouble(const register char * ptr, Numerical::Double * val)
{

    enum NUMBER_PART
    {
        INTEGER, FRACTION, EXPONENT
    } state = INTEGER;

    //val = 0;
    const char * originalStr = ptr;
#ifdef CHECK_PARSE_DOUBLE
    Numerical::Double refValue = atof(ptr);
#endif
    unsigned int index = 0;
    register char ch = *ptr;
    bool negExponent = false;
    bool negMantissa = ch == '-';
    if (negMantissa || ch == '+') {
        ptr++;
        ch = *ptr;
        index++;
    }
    register long long int mul = 1;
    //register Numerical::Double mulchg = 10.0;
    register int exponent1 = 0;
    register int exponent2 = 0;
    register long long int mantissa = 0;
    //register unsigned long long int fraction = 0;
    bool invalidNumber = false;

    while (ch > ' ' && index < (FIELD_3_END - FIELD_3_START + 1) && !invalidNumber) {
        switch (state) {
        case INTEGER:
            if (ch >= '0' && ch <= '9') {
                mantissa *= 10;
                mantissa += ch - '0';
            } else if (ch == '.') {
                state = FRACTION;
            } else if (ch == 'E' || ch == 'e' || ch == 'D' || ch == 'd') {
                char ch2 = ptr[1];
                if (ch2 == '+' || ch2 == '-') {
                    negExponent = ch2 == '-';
                    ptr++;
                    ch = *ptr;
                    index++;
                }
                state = EXPONENT;
            } else {
                invalidNumber = true;
            }
            break;
        case FRACTION:
            if (ch >= '0' && ch <= '9') {
                exponent1++;
                mantissa *= 10;
                mantissa += (ch - '0');
                //exponent1++;
                //                    mul /= 10.0;
            } else if (ch == 'E' || ch == 'e' || ch == 'D' || ch == 'd') {
                char ch2 = ptr[1];
                if (ch2 == '+' || ch2 == '-') {
                    negExponent = ch2 == '-';
                    ptr++;
                    ch = *ptr;
                    index++;
                }
                state = EXPONENT;
            } else {
                invalidNumber = true;
            }
            break;
        case EXPONENT:
            if (ch >= '0' && ch <= '9') {
                exponent2 *= 10;
                exponent2 += ch - '0';
            } else {
                invalidNumber = true;
            }
            break;
        }
        ptr++;
        ch = *ptr;
        index++;
    }
    if (negExponent) {
        exponent2 = -exponent2;
    }
    if (negMantissa) {
        mantissa = -mantissa;
    }
    register int finalExponent = exponent2 - exponent1;

    //if (m_line == 257643) {
    //    std::cout << "eleje" << std::endl;
    //    LPINFO("mantissa: " << mantissa);
    //    LPINFO("exponent1: -" << exponent1);
    //    LPINFO("exponent2: " << exponent2);
    //    LPINFO("final exponent: " << finalExponent);
    //    LPINFO("negExponent: " << negExponent);
    //    LPINFO("negMantissa: " << negMantissa);
    //    LPINFO("mul: " << mul);

    if (finalExponent > 0) {
        register Numerical::Double result = mantissa;
        mul = 1;
        while (finalExponent >= 16) {
            finalExponent -= 16;
            result *= 10000000000000000LL;
        }
        while (finalExponent >= 8) {
            mul *= 100000000LL;
            finalExponent -= 8;
            if (mul >= 10e20) {
                result *= mul;
                mul = 1;
            }
        }
        while (finalExponent >= 4) {
            mul *= 10000LL;
            finalExponent -= 4;
            if (mul >= 10e20) {
                result *= mul;
                mul = 1;
            }
        }
        while (finalExponent >= 2) {
            mul *= 100LL;
            finalExponent -= 2;
            if (mul >= 10e20) {
                result *= mul;
                mul = 1;
            }
        }
        while (finalExponent > 0) {
            mul *= 10LL;
            finalExponent--;

            if (mul >= 10e20) {
                result *= mul;
                mul = 1;
            }
        }
        result *= mul;
        *val = result;
    } else if (finalExponent < 0) {
        register Numerical::Double result = mantissa;
        mul = 1;
        while (finalExponent <= -16) {
            finalExponent += 16;
            result /= 10000000000000000LL;
        }
        while (finalExponent <= -8) {
            mul *= 100000000LL;
            finalExponent += 8;
            if (mul >= 10e20) {
                result /= mul;
                mul = 1;
            }
        }
        while (finalExponent <= -4) {
            mul *= 10000LL;
            finalExponent += 4;
            if (mul >= 10e20) {
                result /= mul;
                mul = 1;
            }
        }
        while (finalExponent <= -2) {
            mul *= 100LL;
            finalExponent += 2;
            if (mul >= 10e20) {
                result /= mul;
                mul = 1;
            }
        }
        while (finalExponent < 0) {
            mul *= 10LL;
            finalExponent++;

            if (mul >= 10e20) {
                result /= mul;
                mul = 1;
            }
        }
        result /= mul;
        *val = result;
    } else {
        *val = mantissa;
    }

    if (ch > ' ') {
        //std::stringstream str;
        //str << originalStr;
        // szo atugrasa
        while (*ptr > ' ' || *ptr == 0) {
            if (*ptr == 0) {
                m_rowLength += ptr - m_startPtr;
                m_startPtr = ptr = m_file.nextBlock(ptr);
            } else {
                //    str << *ptr;
            }
            ptr++;
            index++;
        }
        if (index >= (FIELD_3_END - FIELD_3_START + 1)) {
            //tooLongWordError(str.str().c_str());
        }
    }
    if (invalidNumber == true) {
        invalidNumberFormat(originalStr, *val);
    }
#ifdef CHECK_PARSE_DOUBLE
    //if ((*val) != refValue) {
    if (!Numerical::rigidEqualsFloat64(*val, refValue)) {
        LPINFO(this->m_line);
        std::cerr.precision(14);
        DEVINFO(D::MPSREADER, "error: *val = " << *val << ", refValue = " << refValue);
        DEVINFO(D::MPSREADER, "difference: " << refValue - *val);
        LPERROR("error: *val = " << *val << ", refValue = " << refValue);
        LPERROR("difference: " << refValue - *val);
        Numerical::Float64 val1, val2;
        val1.m_value = *val;
        val2.m_value = refValue;

        DEVINFO(D::MPSREADER, "sign1: " << val1.m_bits.m_sign);
        DEVINFO(D::MPSREADER, "exponent1: " << val1.m_bits.m_exponent);
        DEVINFO(D::MPSREADER, "mantissa1: " << val1.m_bits.m_mantissa);
        DEVINFO(D::MPSREADER, "sign2: " << val2.m_bits.m_sign);
        DEVINFO(D::MPSREADER, "exponent2: " << val2.m_bits.m_exponent);
        DEVINFO(D::MPSREADER, "mantissa2: " << val2.m_bits.m_mantissa);
        DEVINFO(D::MPSREADER, "mantissa diference: " << (val2.m_bits.m_mantissa ^ val1.m_bits.m_mantissa));
        DEVINFO(D::MPSREADER, "line: " << m_line);

        LPERROR("sign1: " << val1.m_bits.m_sign);
        LPERROR("exponent1: " << val1.m_bits.m_exponent);
        LPERROR("mantissa1: " << val1.m_bits.m_mantissa);
        LPERROR("sign2: " << val2.m_bits.m_sign);
        LPERROR("exponent2: " << val2.m_bits.m_exponent);
        LPERROR("mantissa2: " << val2.m_bits.m_mantissa);
        LPERROR("mantissa diference: " << (val2.m_bits.m_mantissa ^ val1.m_bits.m_mantissa));
        LPERROR("line: " << m_line);

        std::cin.get();
    } else {
    }
#endif
    return ptr;
}

#define TEST_NUMBER "120.01242e+3"

struct mps_row
{
    char _type;
    char * _name;
};

struct mps_column
{
    char * _name;
    std::map<unsigned int, Numerical::Double> _coeffs;
};

//inline bool key_comp(register const char * k1, register const char * k2) {
//    return *((unsigned long long int*) k1) < *((unsigned long long int*) k2);
//    //return strcmp(k1, k2) == 1;
//}

inline bool operator<(const mps_row & r1, const mps_row & r2)
{
    return key_comp(r1._name, r2._name);
}

inline bool operator<(const mps_column & c1, const mps_column & c2)
{
    return key_comp(c1._name, c2._name);
}

typedef std::map<mps_column, unsigned int> mps_column_map;

void InitHashKeyProducer(mps_hash_key_producer & hkp)
{
    hkp.m_counter = 0;
    hkp.m_head = hkp.m_actual = 0;
}

char * GetNewKey(mps_hash_key_producer & hkp)
{
    if (hkp.m_head == 0) {
        hkp.m_head = hkp.m_actual = new mps_hash_key_list;
        //memset(hkp._head->_keys, 0, MPS_KEY_SIZE * KEY_COUNT);
        hkp.m_head->m_index = MPS_KEY_SIZE;
        hkp.m_head->m_next = 0;
        return hkp.m_head->m_keys;
    } else {
        if (hkp.m_actual->m_index >= MPS_KEY_SIZE * KEY_COUNT) {
            //cout<<"uj lista lefoglalasa"<<endl;
            //cin.get();
            hkp.m_actual->m_next = new mps_hash_key_list;
            hkp.m_actual = hkp.m_actual->m_next;
            //memset(hkp._actual->_keys, 0, MPS_KEY_SIZE * KEY_COUNT);
            hkp.m_actual->m_index = 0;
            hkp.m_actual->m_next = 0;
        }
        char * res = hkp.m_actual->m_keys + hkp.m_actual->m_index;
        hkp.m_actual->m_index += MPS_KEY_SIZE;
        return res;
    }
    return 0;
}

void ReleaseHashKeyProducer(mps_hash_key_producer & hkp)
{
    bool del = true;
    if (hkp.m_counter) {
        (*hkp.m_counter)--;
        if (*hkp.m_counter > 0) {
            del = false;
        }
    }
    if (del) {
        mps_hash_key_list * temp = hkp.m_head;
        mps_hash_key_list * next;
        while (temp) {
            next = temp->m_next;
            delete temp;
            temp = next;
        }
        delete hkp.m_counter;
    }
    hkp.m_counter = 0;
    hkp.m_actual = 0;
    hkp.m_head = 0;
}

MpsModelBuilder::MpsModelBuilder()
{
    m_line = 0;
    InitHashKeyProducer(m_hkp);
    m_rows.init(m_rowHashFunc, MPS_HASH_TABLE_SIZE_1, MPS_HASH_TABLE_SIZE_2);
    m_columns.init(m_colHashFunc, MPS_HASH_TABLE_SIZE_1, MPS_HASH_TABLE_SIZE_2);
    m_rhs.init(m_colHashFunc, MPS_HASH_TABLE_SIZE_1, MPS_HASH_TABLE_SIZE_2);
    m_bounds.init(m_boundHashFunc, MPS_HASH_TABLE_SIZE_1, MPS_HASH_TABLE_SIZE_2);
    m_ranges.init(m_colHashFunc, MPS_HASH_TABLE_SIZE_1, MPS_HASH_TABLE_SIZE_2);
    m_objectiveConstant = 0;

    MpsError<INVALID_ROW_TYPE>::setLimit(10);
    MpsError<INVALID_ROW_NAME>::setLimit(10);
    MpsError<INVALID_COLUMN_NAME>::setLimit(10);
    MpsError<INVALID_START_FIELD>::setLimit(10);
    MpsError<INVALID_END_FIELD>::setLimit(10);
    MpsError<UNNECESSARY_CONTENT>::setLimit(10);
    MpsError<TOO_LONG_WORD>::setLimit(10);
    MpsError<TOO_LONG_RECORD>::setLimit(10);
    MpsError<MISSING_COMMENT>::setLimit(10);
    MpsError<ROW_EXISTS>::setLimit(10);
    MpsError<COLUMN_EXISTS>::setLimit(10);
    MpsError<INCOMPLETE_RECORD>::setLimit(10);
    MpsError<INVALID_BOUND_TYPE>::setLimit(10);
    MpsError<INVALID_COMMENT>::setLimit(10);
    MpsError<INVALID_NUMBER_FORMAT>::setLimit(10);
    MpsError<INVALID_SECTION_NAME>::setLimit(10);
    MpsError<UNKNOWN_SECTION_NAME>::setLimit(10);
    MpsError<NAME_MISSING>::setLimit(10);
    MpsError<ROWS_MISSING>::setLimit(10);
    MpsError<COLUMNS_MISSING>::setLimit(10);
    MpsError<RHS_MISSING>::setLimit(10);
    MpsError<RANGES_MISSING>::setLimit(10);
    MpsError<BOUNDS_MISSING>::setLimit(10);
    MpsError<ENDATA_MISSING>::setLimit(10);
    MpsError<BOUND_CHANGED>::setLimit(10);
    MpsError<VALUE_OVERRIDED>::setLimit(10);
}

MpsModelBuilder::MpsModelBuilder(const MpsModelBuilder & orig) :
    ModelBuilder(orig)
{
}

MpsModelBuilder::~MpsModelBuilder()
{
    ReleaseHashKeyProducer(m_hkp);
}

const char * MpsModelBuilder::checkEmptyField(const register char * ptr, register int length)
{
    while (*ptr == ' ' && length > 1) {
        ptr++;
        length--;
    }
    if (length > 1) {
        // addMessage...
    }
    return ptr;
}

inline const char * MpsModelBuilder::skipWhiteSpace(const register char * ptr)
{
    while (*ptr <= ' ') {
        ptr++;
        //cout << *ptr << endl;
    }

    return ptr;
}

inline const char * MpsModelBuilder::goToEndLineOrWord(const register char * ptr)
{
    int offset = 0;
    //const unsigned int * intPtr = (const unsigned int*) ptr;

    /*if (*intPtr == 0x20202020) {
        ptr += 4;
        offset = 4;
        intPtr++;
        if (*intPtr == 0x20202020) {
            ptr += 4;
            offset += 4;
        }
    }*/
    while (*ptr <= ' ' && *ptr != '\n' && *ptr != '\0') {
        ptr++;
        if (*ptr == 0) {
            m_rowLength += ptr - m_startPtr;
            m_startPtr = ptr = m_file.nextBlock(ptr);
        } else if (*ptr <= ' ') {
            offset++;
        }
    }
    if (*ptr > ' ') {
        offset = 0;
    }

    m_rowLength -= offset;
    return ptr;
}

const char * MpsModelBuilder::goToEndLine(const register char * ptr)
{
    int offset = 0;
    while (*ptr != '\n' && *ptr != '\0') {
        ptr++;
        if (*ptr == 0) {
            m_rowLength += ptr - m_startPtr;
            m_startPtr = ptr = m_file.nextBlock(ptr);
        } else if (*ptr <= ' ') {
            offset++;
        }
    }
    if (*ptr > ' ') {
        offset = 0;
    }
    m_rowLength -= offset;
    return ptr;
}

inline const char * MpsModelBuilder::copyWord(const register char * ptr, register char * dest,
                                              register int length, MPS_ERROR_TYPE & errorCode)
{
    errorCode = NO_ERROR;
    register int len = 0;
    *dest = 0;
    char * originalDest = dest;

    while (len < length && *ptr > ' ') {
        *dest = *ptr;
        dest++;
        ptr++;
        len++;
    }
    // TODO: szokozokkel kiegesziteni
    while (len <= length) {
        *dest = 0;
        if (len < length) {
            dest++;
        }
        len++;
    }
    len--;

    if (len >= length && (*ptr != ' ' && *ptr != '\r' && *ptr != '\n')) {
        errorCode = TOO_LONG_WORD;
        std::stringstream str;
        str << originalDest;
        // szo atugrasa
        while (*ptr > ' ' || *ptr == 0) {
            if (*ptr == 0) {
                m_rowLength += ptr - m_startPtr;
                m_startPtr = ptr = m_file.nextBlock(ptr);
            } else {
                str << *ptr;
            }
            ptr++;
        }
        tooLongWordError(str.str().c_str());
    }
    return ptr;
}

// ha a mezo vege elott olvas, akkor a szokozoket is beleveszi, ha utana van
// akkor az elso szokoz utan vag

void showPattern(unsigned long long int num) {
    union _seged {
        unsigned long long int bits;
        char ch[8];
    } os;
    os.bits = num;
    int i;
    std::cout << std::hex;
    for (i = 7; i >= 0; i--) {
        unsigned int a = os.ch[i];
        std::cout << ((a & 0xF0) >> 4) << ( a & 0x0F ) << " ";
    }
    std::cout << std::dec;
    std::cout << std::endl;
}

const char * MpsModelBuilder::copyId(const char * ptr,
                                     Name & dest,
                                     int index,
                                     int fieldEnd,
                                     MPS_ERROR_TYPE & errorCode,
                                     unsigned int & hash1,
                                     unsigned int & hash2)
{
    errorCode = NO_ERROR;

    static Timer timer;
    static int _counter = 0;
    //timer.start();
    unsigned long long int originalPattern = *reinterpret_cast<
            const unsigned long long int*>(ptr);
    /*union _seged {
        unsigned long long int bits;
        char ch[8];
    } os, ms1, ms2;

    os.bits = originalPattern;*/

    unsigned long long int maskedPattern = originalPattern & 0x6060606060606060LLU;

    /*maskedPattern = 0x80402060C0A0E000LLU;

    ms1.bits = maskedPattern;

    showPattern(maskedPattern);
    showPattern(maskedPattern >> 1);
    showPattern(maskedPattern >> 2);
    showPattern(maskedPattern | (maskedPattern >> 1));
    LPINFO("-----------------");*/

    //maskedPattern = (maskedPattern << 1) | (maskedPattern << 2);

    //    maskedPattern = (maskedPattern | (maskedPattern << 1LLU) | (maskedPattern << 2LLU) |
    //                    (maskedPattern >> 1LLU) | (maskedPattern >> 2LLU)) & 0xe0e0e0e0e0e0e0e0;
    maskedPattern = (maskedPattern | (maskedPattern << 1LLU) | (maskedPattern >> 1LLU)) & 0x6060606060606060;

    //maskedPattern = maskedPattern >> 2;
    //ms2.bits = maskedPattern;
    if (maskedPattern == 0x6060606060606060) {
        //index += 8;
        ptr += 8;
        dest = originalPattern;
        hash1 = originalPattern % MPS_HASH_TABLE_SIZE_1;
        hash2 = originalPattern % MPS_HASH_TABLE_SIZE_2;
        /*  timer.stop();
        _counter++;
        if (_counter % 10000 == 0) {
            LPINFO(timer.getTotalElapsed());
        }*/


        return ptr;
    } else {
        /*std::cout << std::hex;

        int i;
        for (i = 7; i >= 0; i--) {
            unsigned int a = os.ch[i];
            std::cout << ((a & 0xF0) >> 4) << ( a & 0x0F ) << " ";
        }
        std::cout << std::endl;
        for (i = 7; i >= 0; i--) {
            unsigned int a = ms1.ch[i];
            std::cout << ((a & 0xF0) >> 4) << ( a & 0x0F ) << " ";
        }
        std::cout << std::endl;
        for (i = 7; i >= 0; i--) {
            unsigned int a = ms2.ch[i];
            std::cout << ((a & 0xF0) >> 4) << ( a & 0x0F ) << " ";
        }
        std::cout << std::endl;
        std::cout << std::dec;
        LPINFO( maskedPattern );
        std::cin.get();*/
    }
    timer.stop();
    _counter++;
    if (_counter % 10000 == 0) {
        LPINFO(timer.getTotalElapsed());
    }

    dest = 0;
    char * originalDest = reinterpret_cast<char*>(&dest);
    char * destPtr = originalDest;
    //    register unsigned int hashCode = 0;
    int len = 0;
    //    int end = index + MPS_NAME_LENGTH + 1;
    //std::cout << "|" << std::endl;
    // kigyujtom egy 8 bajtosba a mintakat, majd
    // raeresztunk egy maszkot, kitakarjuk minden bajtbol
    // az also 5 bitet, ha van kozte 0, akkor ott vege van
    // a mintanak

    //static int _counter0 = 0;
    //static int _counter1 = 0;
    timer.start();
    while (len < MPS_NAME_LENGTH && *ptr >= ' ') {
        if (index > fieldEnd && *ptr <= ' ') {
            break;
        }
        /*hashCode += *ptr;
        hashCode += (hashCode << 10);
        hashCode ^= (hashCode >> 6);*/

        *destPtr = *ptr;
        //  std::cout << *ptr;
        destPtr++;
        ptr++;
        len++;
        index++;
    }
    /*std::cout << "|" << std::endl;
    std::cin.get();*/
    /*unsigned long long * shadowDest = reinterpret_cast<unsigned long long*>(dest);
    const unsigned long long * shadowSrc = reinterpret_cast<const unsigned long long*>(ptr);
    *shadowDest = *shadowSrc;
    len += 8;
    index += 8;
    ptr += 8;
    *hash = *shadowDest % MPS_HASH_TABLE_SIZE;*/

    // TODO: szokozokkel kiegesziteni
    while (len < MPS_NAME_LENGTH) {
        *destPtr = ' ';
        /*hashCode += ' ';
        hashCode += (hashCode << 10);
        hashCode ^= (hashCode >> 6);*/
        if (len < MPS_NAME_LENGTH) {
            destPtr++;
        }
        len++;
    }
    //*dest = 0;

    len--;
    /*hashCode += (hashCode << 3);
    hashCode ^= (hashCode >> 11);
    hashCode += (hashCode << 15);
    *hash = hashCode % MPS_HASH_TABLE_SIZE;*/
    unsigned long long * shadowDest = reinterpret_cast<unsigned long long*>(originalDest);
    hash1 = *shadowDest % MPS_HASH_TABLE_SIZE_1;
    hash2 = *shadowDest % MPS_HASH_TABLE_SIZE_2;

    //    LPINFO("\"" << originalDest << "\", " << *hash);
    //    std::cin.get();


    if (*ptr != ' ' && *ptr != '\r' && *ptr != '\n') {
        errorCode = TOO_LONG_WORD;
        std::stringstream str;
        str << originalDest;
        // szo atugrasa
        while (*ptr > ' ' || *ptr == 0) {
            if (*ptr == 0) {
                m_rowLength += ptr - m_startPtr;
                m_startPtr = ptr = m_file.nextBlock(ptr);
            } else {
                str << *ptr;
            }
            ptr++;
        }
        tooLongWordError(str.str().c_str());
    }
    return ptr;
}

const char * MpsModelBuilder::copyName(const register char * ptr, register char * dest,
                                       register int length, MPS_ERROR_TYPE & errorCode)
{
    register int len = 0;
    char * originalDest = dest;
    *dest = 0;
    int end = -1;
    while (len < length && *ptr != '\r' && *ptr != '\n') {
        *dest = *ptr;
        if (*ptr == ' ') {
            if (end == -1) {
                end = len;
            }
        } else {
            end = -1;
        }
        dest++;
        ptr++;
        len++;
    }
    *dest = 0;
    if (end != -1) {
        originalDest[end] = 0;
    }
    //    LPINFO("name: \"" << originalDest << "\"");
    //    LPINFO("*ptr: '" << *ptr << "'");
    //    std::cin.get();
    if (*ptr > ' ') {
        errorCode = TOO_LONG_WORD;
        std::stringstream str;
        str << originalDest;
        // szo atugrasa
        while ((*ptr != '\r' && *ptr != '\n') || *ptr == 0) {
            if (*ptr == 0) {
                m_rowLength += ptr - m_startPtr;
                m_startPtr = ptr = m_file.nextBlock(ptr);
            } else {
                str << *ptr;
            }
            ptr++;
        }
        tooLongWordError(str.str().c_str(), false);
        //throw TooLongWordException();
    }
    return ptr;
}

const char * MpsModelBuilder::getSectionName(SECTION_TYPE type)
{
    switch (type) {
    case SEC_NAME:
        return "NAME";
    case SEC_ROWS:
        return "ROWS";
    case SEC_COLUMNS:
        return "COLUMNS";
    case SEC_RHS:
        return "RHS";
    case SEC_RANGES:
        return "RANGES";
    case SEC_BOUNDS:
        return "BOUNDS";
    case SEC_ENDATA:
        return "ENDATA";
    }
    return "Invalid section";
}

const char * MpsModelBuilder::getActualSectionName()
{
    return getSectionName(m_section);
}

void MpsModelBuilder::invalidStartFieldError(int index, int field_start)
{
    std::stringstream str;
    str << " word starts at " << index <<
           ", but field starts at " << field_start;
    LPERROR(str.str());
    m_errors.push_back(MpsError<INVALID_START_FIELD > (m_line, m_section, str.str(),
                                                       MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::invalidEndFieldError(int index, int field_end)
{
    std::stringstream str;
    str << " word ends at " << index <<
           ", but field ends at " << field_end;
    LPERROR(str.str());
    m_errors.push_back(MpsError<INVALID_END_FIELD > (m_line, m_section, str.str(),
                                                     MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::invalidRowTypeError(const char * type)
{
    m_modifyLogicErrors++;
    LPERROR("Invalid row type");
    m_errors.push_back(MpsError<INVALID_ROW_TYPE > (m_line, m_section,
                                                    "invalid row type: " + std::string(type), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::invalidRowName(const char * row)
{
    m_modifyLogicErrors++;
    LPERROR("Invalid row name");
    m_errors.push_back(MpsError<INVALID_ROW_NAME > (m_line, m_section,
                                                    "invalid row: \"" + std::string(row) + "\"", MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::invalidColumnName(const char * column)
{
    m_modifyLogicErrors++;
    LPERROR("Invalid column name");
    m_errors.push_back(MpsError<INVALID_COLUMN_NAME > (m_line, m_section,
                                                       "invalid column: \"" + std::string(column) + "\"", MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::valueOverridedError(const char * column, const char * row, Numerical::Double original,
                                          Numerical::Double newValue)
{
    LPERROR(__FUNCTION__);
    m_modifyLogicErrors++;
    std::stringstream str;
    int i;
    int index = strlen(column) - 1;
    while (column[index] == ' ' && index >= 0) {
        index--;
    }
    str << "value overrided in column \"";
    for (i = 0; i <= index; i++) {
        str << column[i];
    }
    str << "\", at row \"";
    index = strlen(row) - 1;
    while (row[index] == ' ' && index >= 0) {
        index--;
    }
    for (i = 0; i <= index; i++) {
        str << row[i];
    }
    str << "\", from " <<
           original << " to " << newValue;
    m_errors.push_back(MpsError<VALUE_OVERRIDED > (m_line, m_section,
                                                   str.str(), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::invalidBoundType(const char * bound)
{
    LPERROR(__FUNCTION__);
    m_modifyLogicErrors++;
    m_errors.push_back(MpsError<INVALID_BOUND_TYPE > (m_line, m_section,
                                                      "invalid bound type: " + std::string(bound), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::invalidComment(unsigned int position)
{
    std::stringstream str;
    LPERROR(__FUNCTION__);
    str << "invalid comment, comment starts at position " << position;
    m_errors.push_back(MpsError<INVALID_COMMENT > (m_line, m_section,
                                                   str.str(), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::invalidNumberFormat(const char * number, Numerical::Double parsed)
{
    m_modifyNumericErrors++;
    LPERROR(__FUNCTION__);
    std::stringstream str;
    str << "invalid number format: \"";
    while (*number > ' ') {
        str << *number;
        number++;
    }
    str << "\", parsed value: " << parsed;
    m_errors.push_back(MpsError<INVALID_NUMBER_FORMAT > (m_line, m_section,
                                                         str.str(), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::invalidSectionName(const char * name, const char * expected)
{
    m_modifyLogicErrors++;
    LPERROR(__FUNCTION__);
    std::stringstream str;
    str << "invalid section name: " << name << ", expected " << expected;
    m_errors.push_back(MpsError<INVALID_SECTION_NAME > (m_line, m_section,
                                                        str.str(), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::unknownSectionName(const char * name, int start, int end)
{
    m_modifyLogicErrors++;
    LPERROR(__FUNCTION__ << " " << name);
    std::stringstream str;
    str << "unknown section name: " << name << ", records from " << start
        << " to " << end << " were skipped";
    m_errors.push_back(MpsError<INVALID_SECTION_NAME > (m_line, m_section,
                                                        str.str(), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::unnecessaryContentError()
{
    LPERROR(__FUNCTION__);
    m_errors.push_back(MpsError<UNNECESSARY_CONTENT > (m_line, m_section,
                                                       "unnecessary content", MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::tooLongWordError(const char * word, bool logicModify)
{
    LPERROR(__FUNCTION__);
    if (logicModify) {
        m_modifyLogicErrors++;
    }
    m_errors.push_back(MpsError<TOO_LONG_WORD > (m_line, m_section,
                                                 "too long word: " + std::string(word), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::missingCommentError(const char * word)
{
    LPERROR(__FUNCTION__);
    m_errors.push_back(MpsError<MISSING_COMMENT > (m_line, m_section,
                                                   "missing comment sign (*) after " + std::string(word), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::rowExistsError(const char * row)
{
    LPERROR(__FUNCTION__);
    m_modifyLogicErrors++;
    m_errors.push_back(MpsError<ROW_EXISTS > (m_line, m_section,
                                              "row exists: " + std::string(row), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::columnExistsError(const char * column)
{
    LPERROR(__FUNCTION__);
    m_errors.push_back(MpsError<COLUMN_EXISTS > (m_line, m_section,
                                                 "column already exists: " + std::string(column), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::incompleteRecordError()
{
    LPERROR(__FUNCTION__);
    m_modifyLogicErrors++;
    m_errors.push_back(MpsError<INCOMPLETE_RECORD > (m_line, m_section,
                                                     "incomplete record", MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::tooLongRecordError(unsigned int length)
{
    LPERROR(__FUNCTION__);
    std::stringstream str;
    str << "too long record ( " << length << " characters )";
    m_errors.push_back(MpsError<TOO_LONG_RECORD > (m_line, m_section,
                                                   str.str(), MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::missingNameSection()
{
    LPERROR(__FUNCTION__);
    m_errors.push_back(MpsError<NAME_MISSING > (m_line, m_section,
                                                "missing NAME section", MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::missingRowsSection()
{
    LPERROR(__FUNCTION__);
    m_modifyLogicErrors++;
    m_errors.push_back(MpsError<ROWS_MISSING > (m_line, m_section,
                                                "missing ROWS section", MpsErrorData::FATAL_ERROR).getData());
}

void MpsModelBuilder::missingColumnsSection()
{
    LPERROR(__FUNCTION__);
    m_modifyLogicErrors++;
    m_errors.push_back(MpsError<COLUMNS_MISSING > (m_line, m_section,
                                                   "missing COLUMNS section", MpsErrorData::FATAL_ERROR).getData());
}

void MpsModelBuilder::missingRhsSection()
{
    LPERROR(__FUNCTION__);
    m_errors.push_back(MpsError<RHS_MISSING > (m_line, m_section,
                                               "missing RHS section", MpsErrorData::ERROR).getData());
}

void MpsModelBuilder::missingRangesSection()
{
    LPERROR(__FUNCTION__);
    m_errors.push_back(MpsError<RANGES_MISSING > (m_line, m_section,
                                                  "missing RANGES section", MpsErrorData::WARNING).getData());
}

void MpsModelBuilder::missingBoundsSection()
{
    LPERROR(__FUNCTION__);
    m_errors.push_back(MpsError<BOUNDS_MISSING > (m_line, m_section,
                                                  "missing BOUNDS section", MpsErrorData::WARNING).getData());
}

void MpsModelBuilder::missingEndataSection()
{
    LPERROR(__FUNCTION__);
    m_errors.push_back(MpsError<ENDATA_MISSING > (m_line, m_section,
                                                  "missing ENDATA section", MpsErrorData::ERROR).getData());
}

const char * MpsModelBuilder::nextRowType(const register char * ptr, ROW_INFO & info, bool)
{
    char word[MAX_ROW_WITH];
    MPS_ERROR_TYPE errorCode;
    ptr = goToEndLineOrWord(ptr);
    if (*ptr == '*' || *ptr == '\r' || *ptr == '\n') {
        ptr = goToEndLine(ptr);
        info = COMMENT;
        return ptr;
    }
    bool repeat;
    bool endOfFile = false;
    do {
        repeat = false;
        try {
            ptr = goToEndLineOrWord(copyWord(ptr, word, 60, errorCode));
        } catch (File::EndOfFileException & error) {
            endOfFile = true;
        }
        if (errorCode == TOO_LONG_WORD) {
            ptr++;
            ptr = goToEndLineOrWord(ptr);
        }

        /*if (nextWord) {
            ptr = goToEndLineOrWord(ptr);
        }*/
        //std::cout << "word: \"" << word << "\"" << std::endl;
        if (strcmp(word, "NAME") == 0) {
            if (m_section != SEC_NAME) {
                invalidSectionName(word, sm_expectedSection[m_section]);
            }
            info = NAME_SECTION;
            //LPINFO("reading NAME section");
        } else if (strcmp(word, "ROWS") == 0) {
            if (m_section != SEC_NAME) {
                invalidSectionName(word, sm_expectedSection[m_section]);
            }
            info = ROWS_SECTION;
            //LPINFO("reading ROWS section");
        } else if (strcmp(word, "COLUMNS") == 0) {
            if (m_section != SEC_ROWS) {
                invalidSectionName(word, sm_expectedSection[m_section]);
            }
            info = COLUMNS_SECTION;
            //LPINFO("reading COLUMNS section");
        } else if (strcmp(word, "RHS") == 0) {
            if (m_section != SEC_COLUMNS) {
                invalidSectionName(word, sm_expectedSection[m_section]);
            }
            info = RHS_SECTION;
            //LPINFO("reading RHS section");
        } else if (strcmp(word, "RANGES") == 0) {
            if (m_section != SEC_RHS) {
                invalidSectionName(word, sm_expectedSection[m_section]);
            }
            info = RANGES_SECTION;
            //LPINFO("reading RANGES section");
        } else if (strcmp(word, "BOUNDS") == 0) {
            if (m_section != SEC_RHS && m_section != SEC_RANGES) {
                invalidSectionName(word, sm_expectedSection[m_section]);
            }
            info = BOUNDS_SECTION;
            //LPINFO("reading BOUNDS section");
        } else if (strcmp(word, "ENDATA") == 0) {
            if (m_section != SEC_RHS && m_section != SEC_RANGES &&
                    m_section != SEC_BOUNDS) {
                invalidSectionName(word, sm_expectedSection[m_section]);
            }
            info = ENDATA_SECTION;
            //LPINFO("reading ENDATA section");
        } else {
            int startLine = m_line;
            info = INVALID_SECTION;
            // atugorni amig nem talalunk ervenyeset vagy vege nem lesz
            do {
                while (*ptr != '\n') {
                    ptr++;
                    if (*ptr == '\0') {
                        ptr = m_file.nextBlock(ptr);
                    }
                }
                m_line++;
                ptr++;
                m_startPtr = ptr;
                m_rowLength = 0;
            } while (*ptr < 'A' || *ptr > 'Z');
            repeat = true;
            unknownSectionName(word, startLine, m_line - 1);
        }
    } while (repeat);
    if (endOfFile == true) {
        throw File::EndOfFileException();
    }
    if (info != INVALID_SECTION && info != NAME_SECTION) {
        if (*ptr != '*' && *ptr != '\n' && *ptr != '\r' && *ptr != '\0') {
            missingCommentError(word);
        }
        ptr = goToEndLine(ptr);
    }
    return ptr;
}

const char * MpsModelBuilder::readName(const register char * ptr, ROW_INFO & info)
{
    initTooLongRecordSensor(ptr);
    char name[61];
    int index = 0;
    const char * ptr2;
    ptr2 = nextRowType(ptr, info);
    //cout<<"||"<<ptr2<<"||"<<endl;
    if (info == NAME_SECTION) {
        MPS_ERROR_TYPE errorCode;
        ptr2 = skipWhiteSpace(ptr2);
        index += ptr2 - ptr;
        if (index < FIELD_2_START) {
            invalidStartFieldError(index, FIELD_2_START);
        }
        ptr = copyName(ptr2, name, FIELD_5_END - FIELD_2_START + 1, errorCode);
        if (name[strlen(name) - 1] == '\r') {
            name[strlen(name) - 1] = '\0';
        }

        if (name[0] == '*') {
            m_name = std::string("");
        } else {
            m_name = std::string(name);
        }
        LPINFO("Problem name: \"" << m_name << "\"");
        ptr = goToEndLine(ptr);

        //cout << "||" << ptr << "||" << endl;
    } else {
        ptr = ptr2;
    }
    checkTooLongRecord(ptr);
    return ptr;
}

// egy sor rekordot dolgoz fel, ptr a 0. karakteren all, amirol feltetelezhetjuk hogy szokoz
// a beolvasas vegen a kovetkezo rekord elejere allitja a ptr-t

inline const char * MpsModelBuilder::readRowRecord(const register char * ptr, ROW_INFO & info)
{
    initTooLongRecordSensor(ptr);
    /*if (*ptr == '*') {
        info = COMMENT;
        ptr = goToEndLine(ptr);
        MPS_RETURN(ptr);
    }*/
    MPS_ERROR_TYPE errorCode;
    //if ((*ptr == ' ' && ptr[1] != ' ') || (*ptr != ' ' && ptr[1] == ' ')) {
    if (*ptr == ' ') {
        info = NO_INFO;
        Row row;
        register int index = 0;
        unsigned int hashCode1 = 0;
        unsigned int hashCode2 = 0;

        /*************************-
         * reading row type
         ************************/
        ptr = goToEndLineOrWord(ptr);
        MPS_RECORD_END_CHECK(ptr);
        index = ptr - m_startPtr + m_rowLength;
        if (index < FIELD_0_START) {
            invalidStartFieldError(index, FIELD_0_START);
        }
        row.m_type = *ptr;
        if (row.m_type != 'E' && row.m_type != 'L' && row.m_type != 'G' &&
                row.m_type != 'N') {
            char msgpart[2] = {row.m_type, 0};
            invalidRowTypeError(msgpart);
            ptr = goToEndLine(ptr);
            MPS_RETURN(ptr);
        }
        ptr++;
        if (index > FIELD_0_END + 1) {
            invalidEndFieldError(index, FIELD_0_END);
        }
        if (*ptr > ' ') {
            invalidRowTypeError(ptr - 1);
            ptr = goToEndLine(ptr);
            MPS_RETURN(ptr);
        }
        if (*ptr == '\r' || *ptr == '\n') {
            incompleteRecordError();
            ptr = goToEndLine(ptr);
            MPS_RETURN(ptr);
        }

        /*************************-
         * reading row name
         ************************/
        ptr = goToEndLineOrWord(ptr);
        index = ptr - m_startPtr + m_rowLength;
        if (index < FIELD_1_START) {
            invalidStartFieldError(index, FIELD_1_START);
        }

        //row.m_name = GetNewKey(m_hkp);
        ptr = copyId(ptr, row.m_name, index, FIELD_1_END,
                     errorCode, hashCode1, hashCode2);

        index = ptr - m_startPtr + m_rowLength;
        if (index > FIELD_1_END + 1) {
            invalidEndFieldError(index, FIELD_1_END);
        }

        if (m_rows.exist(row, hashCode1, hashCode2)) {
            rowExistsError(nameToString(row.m_name).c_str() );
        } else {
            // Row * rowPtr = m_rows.add(row, m_rows.getCount(), hashCode);
            if (row.m_type == 'N' && !m_costVectorReady) {
                m_costVectorReady = true;
                m_costVectorIndex = -1; //m_rows.getCount(); // - 1;
                m_costRow = row;
                //m_rows.add(row, -1, hashCode);
            } else {
                Row * rowPtr = m_rows.add(row, m_rows.getCount(), hashCode1, hashCode2);
                m_rowIndexTable.push_back(rowPtr);
            }
        }

        ptr = goToEndLineOrWord(ptr);
        if (*ptr == '*') {
            ptr = goToEndLine(ptr);
            MPS_RETURN(ptr);
        }
        MPS_MISSING_COMMENT_CHECK(ptr);
    } else {
        ptr = nextRowType(ptr, info);
    }
    MPS_RETURN(ptr);
}

void MpsModelBuilder::clearActualColumnData()
{
    if (m_actualColumnData) {
        List< Column::Pair >::Iterator iter = m_actualColumnData->m_coeffs.begin();
        List< Column::Pair >::Iterator iterEnd = m_actualColumnData->m_coeffs.end();
        for (; iter != iterEnd; iter++) {
            m_actualColumnNonzeros[iter.getData().m_index] = false;
            //std::cout << iter.getData().m_index << ".: " << iter.getData().m_value << std::endl;
        }
    }
}

const char * MpsModelBuilder::fastReadColumnRecord(const char * ptr,
                                                   bool * wellFormed,
                                                   ROW_INFO & info) {
    if (*ptr != ' ') {
        //LPINFO("nem jo1: "<<ptr[0]<<ptr[1]<<ptr[2]);
        //exit(1);
        //std::cin.get();
        ptr = nextRowType(ptr, info);
        *wellFormed = true;
        return ptr;
    }
    info = NO_INFO;
    const char * originalPtr = ptr;
    //static bool ascendingOrderedColumnNames = true;
    const ColumnFirstFields * firstFields;
    firstFields = reinterpret_cast<const ColumnFirstFields*>(ptr);
    if (firstFields->m_space1 != 0x20202020 ||
            firstFields->m_space2 != 0x2020 ||
            firstFields->m_space3 != 0x2020) {
        *wellFormed = false;
        LPINFO("nem jo2");
        char * p = const_cast<char*>(ptr);
        p[50] = 0;
        LPINFO(p);
        exit(1);
        std::cin.get();
        return originalPtr;
    }

    Name columnName = firstFields->m_columnName;
    unsigned int columnHashCode1 = columnName % MPS_HASH_TABLE_SIZE_1;
    unsigned int columnHashCode2 = columnName % MPS_HASH_TABLE_SIZE_2;
    Name rowName = firstFields->m_rowName;
    unsigned int rowHashCode1 = rowName % MPS_HASH_TABLE_SIZE_1;
    unsigned int rowHashCode2 = rowName % MPS_HASH_TABLE_SIZE_2;

    Numerical::Double value;
    ptr = parseDouble(ptr + sizeof(ColumnFirstFields), &value);

    Column * columnData;

    // TODO: ellenorizni!

    if (m_actualColumn != columnName) {
        // uj oszlopot talaltunk
        /*std::cerr << std::hex;
        LPINFO( (m_actualColumn >> 56) );
        LPINFO( ((m_actualColumn >> 48) & 0xFF) );
        LPINFO( ((m_actualColumn >> 40) & 0xFF) );
        LPINFO( ((m_actualColumn >> 32) & 0xFF) );
        LPINFO( ((m_actualColumn >> 24) & 0xFF) );
        LPINFO( ((m_actualColumn >> 16) & 0xFF) );
        LPINFO( ((m_actualColumn >> 8) & 0xFF) );
        LPINFO( (m_actualColumn & 0xFF) );
        Name actual = ((m_actualColumn) << 56) | // utolso 8 bitet elore tolni
                (((m_actualColumn << 40) & 0x00FF000000000000)) | // kovetkezo 8 bitet elore tolni
                (((m_actualColumn << 24) & 0x0000FF0000000000)) |
                (((m_actualColumn << 8) &  0x000000FF00000000)) |
                (((m_actualColumn >> 8) &  0x00000000FF000000)) |
                (((m_actualColumn >> 24) & 0x0000000000FF0000)) |
                (((m_actualColumn >> 40) & 0x000000000000FF00)) |
                (m_actualColumn >> 56);
        Name newName = ((columnName) << 56) | // utolso 8 bitet elore tolni
                (((columnName << 40) & 0x00FF000000000000)) | // kovetkezo 8 bitet elore tolni
                (((columnName << 24) & 0x0000FF0000000000)) |
                (((columnName << 8) &  0x000000FF00000000)) |
                (((columnName >> 8) &  0x00000000FF000000)) |
                (((columnName >> 24) & 0x0000000000FF0000)) |
                (((columnName >> 40) & 0x000000000000FF00)) |
                (columnName >> 56);
        LPINFO("*************************");
        LPINFO( (actual >> 56) );
        LPINFO( ((actual >> 48) & 0xFF) );
        LPINFO( ((actual >> 40) & 0xFF) );
        LPINFO( ((actual >> 32) & 0xFF) );
        LPINFO( ((actual >> 24) & 0xFF) );
        LPINFO( ((actual >> 16) & 0xFF) );
        LPINFO( ((actual >> 8) & 0xFF) );
        LPINFO( (actual & 0xFF) );
        std::cerr << std::dec;*/
        //ascendingOrderedColumnNames = ascendingOrderedColumnNames && (actual < newName);
        //ascendingOrderedColumnNames = ascendingOrderedColumnNames && strncmp(
        //            reinterpret_cast<char *>(&m_actualColumn), reinterpret_cast<char *>(&columnName), 8) < 0;
        //if (!ascendingOrderedColumnNames) {
        //    LPINFO( m_actualColumn << " " << columnName );
        // }
        // 1111000 110001 111001  x19   494713
        // 1111000 110010 110000  x20   494768
        clearActualColumnData();
        m_actualColumn = columnName;
        Column newColumn;
        newColumn.m_name = columnName;
        // ellenorizni kell, hogy volt-e mar ilyen
        if (m_columns.getKey(newColumn, columnHashCode1, columnHashCode2) != 0) {
            columnExistsError(nameToString(columnName).c_str());
        }

        columnData = m_columns.add(newColumn, m_columns.getCount(), columnHashCode1, columnHashCode2);
        m_columnIndexTable.push_back(columnData);

        m_actualColumnData = columnData;

    }
    // lekerdezzuk a sor adatokat
    Row * rowData = 0;
    Row actualRow;
    actualRow.m_name = rowName;
    int * rowIdPtr = m_rows.getValueAndKey(actualRow, rowData, rowHashCode1, rowHashCode2);
    //bool isCostValue = false;
    if (rowIdPtr == 0) {
        if ( m_costRow.m_name != rowName ) {
            //invalidRowName(nameToString(rowName).c_str());
            *wellFormed = false;
            return originalPtr;
        } /*else {
            isCostValue = true;
        }*/
    } else {
        m_actualColumnNonzeros[ *rowIdPtr ] = value != 0.0;
        m_actualColumnData->add( *rowIdPtr, value );
    }

    /*LPINFO("column name: " << nameToString( columnName ) );
    LPINFO("row name: " << nameToString( rowName ) );
    LPINFO("value: " << value);
    std::cin.get();*/

    *wellFormed = true;
    while (*ptr >= ' '){
        ptr++;
    }
    return ptr;
}

inline const char * MpsModelBuilder::readColumnRecord(const register char * ptr,
                                                      HashTable<Column, int, hash_function<Column> > & columns, ROW_INFO & info,
                                                      std::vector<Column*> * indexTable)
{
    //LPWARNING("readColumnRecord begin");
    initTooLongRecordSensor(ptr);
    if (*ptr == ' ') {
        MPS_ERROR_TYPE errorCode;
        info = NO_INFO;

        int index = 0;
        unsigned int columnHashCode1 = 0;
        unsigned int columnHashCode2 = 0;
        unsigned int rowHashCode1 = 0;
        unsigned int rowHashCode2 = 0;

        /*************************-
         * reading column name
         * checking field 2 ranges
         ************************/
        ptr = goToEndLineOrWord(ptr);
        MPS_RECORD_END_CHECK(ptr);
        index = ptr - m_startPtr + m_rowLength;
        if (index < FIELD_1_START) {
            invalidStartFieldError(index, FIELD_1_START);
        }
        Column col;
        //col.m_name = GetNewKey(m_hkp);
        ptr = copyId(ptr, col.m_name, index, FIELD_1_END,
                     errorCode, columnHashCode1, columnHashCode2);

        MPS_INCOMPLETE_RECORD_CHECK(ptr);
        index = ptr - m_startPtr + m_rowLength;
        if (index > FIELD_1_END + 1) {
            invalidEndFieldError(index, FIELD_1_END);
        }
        if (*ptr == '*') {
            ptr = goToEndLine(ptr);
            //LPWARNING("readColumnRecord end");
            MPS_RETURN(ptr);
        }

        /*************************-
         * checking wether the column exists or not
         * insert the new column
         ************************/
        //LPINFO("read column");
        Column * col_ptr = 0;

        //static Timer timer;
        //static int _counter = 0;
        //timer.start();
        if ( m_actualColumn == 0 ) {
            m_actualColumn = col.m_name;

            //LPINFO("elotte0");
            col_ptr = columns.add(col, columns.getCount(), columnHashCode1, columnHashCode2);
            indexTable->push_back(col_ptr);

            m_actualColumnData = col_ptr;
        } else {
            bool match = true;
            match = m_actualColumn == col.m_name;
            if (!match) {
                //timer.start();
                col_ptr = columns.getKey(col, columnHashCode1, columnHashCode2);
                clearActualColumnData();
                if (col_ptr == 0) { // megnezzuk hogy volt-e mar korabban ilyen

                    col_ptr = columns.add(col, columns.getCount(), columnHashCode1, columnHashCode2);
                    m_actualColumnData = col_ptr;
                    m_actualColumn = col.m_name;
                    indexTable->push_back(col_ptr);

                } else {
                    columnExistsError(nameToString(col.m_name).c_str());
                }
                //timer.stop();
            } else {
                // TODO: majd ezt is tarolni kell
                //std::cout << "nem valtozott" << std::endl;
                //timer.start();
                col_ptr = m_actualColumnData;
                // TODO: megnezni hogy jo-e igy
                //col_ptr = columns.getKey(col, columnHashCode1, columnHashCode2);
                //timer.stop();

            }
        }
        //timer.stop();
        /*_counter++;
        if (_counter % 10000 == 0) {
            LPINFO( timer.getTotalElapsed() );
        }*/
        //std::cin.get();

        /*************************-
         * reading row name
         * checking field 3 ranges
         ************************/
        ptr = goToEndLineOrWord(ptr);
        MPS_INCOMPLETE_RECORD_CHECK(ptr);
        index = ptr - m_startPtr + m_rowLength;
        if (index < FIELD_2_START) {
            invalidStartFieldError(index, FIELD_2_START);
        }
        Row row;
        //row.m_name = GetNewKey(m_hkp);
        ptr = copyId(ptr, row.m_name, index, FIELD_2_END,
                     errorCode, rowHashCode1, rowHashCode2);

        /*char str2[9];
        memcpy(str2, &row.m_name, 8);
        str2[8] = 0;
        LPINFO("|" << str2 << "|");
        std::cin.get();*/



        MPS_INCOMPLETE_RECORD_CHECK(ptr);
        index = ptr - m_startPtr + m_rowLength;
        if (index > FIELD_2_END + 1) {
            invalidStartFieldError(index, FIELD_2_END);
        }

        /*************************-
         * checking wether the row exists or not
         ************************/
        //timer.start();
        bool skipNumber = false;
        Row * rowPtr = 0;
        int * row_id_ptr = m_rows.getValueAndKey(row, rowPtr, rowHashCode1, rowHashCode2);
        bool newNonzero = true;
        int row_id = -1;
        bool isCostValue = false;
        if (row_id_ptr) {
            row_id = *row_id_ptr;
            bool & nonZero = m_actualColumnNonzeros[row_id];
            if (nonZero == true) {
                //LPERROR("itt balhe van gyerekek " << row_id << " " << m_line);
                //                std::cin.get();
                newNonzero = false;
            } else {
                nonZero = true;
            }
        } else {
            //if (strcmp(m_costRow.m_name, row.m_name) != 0) {
            if (m_costRow.m_name != row.m_name) {
                invalidRowName(nameToString(row.m_name).c_str());
                skipNumber = true;
            } else {
                isCostValue = true;
            }
        }
        /*timer.stop();
        _counter++;
        if (_counter % 10000 == 0) {
            LPINFO( timer.getTotalElapsed() );
        }*/
        /*************************-
         * reading the number
         ************************/
        Numerical::Double value;
        ptr = goToEndLineOrWord(ptr);
        MPS_INCOMPLETE_RECORD_CHECK(ptr);
        if (skipNumber) {
            while (*ptr > ' ') {
                if (*ptr == 0) {
                    m_rowLength += ptr - m_startPtr;
                    m_startPtr = ptr = m_file.nextBlock(ptr);
                } else {
                    ptr++;
                }
            }
            // TODO: letesztelni
        } else {
            index = ptr - m_startPtr + m_rowLength;
            if (index < FIELD_3_START) {
                invalidStartFieldError(index, FIELD_3_START);
            }
            ptr = parseDouble(ptr, &value);
            // TODO: komment kezelese
            if (value != 0.0) {
                // TODO: lehetne hash-elni az oszlopvektoron belul is
                //col_ptr->m_coeffs.insert(std::make_pair(row_id, value));
                //col_ptr->m_coeffs.insert(row_id, value);
                if (newNonzero == false) {
                    valueOverridedError( nameToString(col_ptr->m_name).c_str(),
                                         nameToString(rowPtr->m_name).c_str(),
                                         col_ptr->getValue(row_id), value);
                }

                /*if (row_id > (int) m_costVectorIndex) {
                    col_ptr->add(row_id - 1, value);
                } else if (row_id < (int) m_costVectorIndex) {*/
                if (isCostValue == false) {
                    col_ptr->add(row_id, value);
                } else if (m_section == SEC_RHS) {
                    m_objectiveConstant = value;
                }

                //}
                m_actualColumnData = col_ptr;
                if (m_section == SEC_COLUMNS) {
                    //if (row_id == (int) m_costVectorIndex) {
                    if (isCostValue == true) {
                        col_ptr->m_inCostVector = true;
                        Column::Pair pair;
                        pair.m_index = columns.getCount() - 1;
                        pair.m_value = value;
                        m_costVector.pushBack(pair);
                    }
                    if (newNonzero && isCostValue == false) {
                        rowPtr->m_nonZeros++;
                        m_nonZeros++;
                    }
                }

            } else {
                m_actualColumnNonzeros[row_id] = false;
            }
            isCostValue = false;
            index = ptr - m_startPtr + m_rowLength;
            if (index > FIELD_3_END + 1) {
                invalidEndFieldError(index, FIELD_3_END);
            }
            MPS_RECORD_END_CHECK(ptr);
            ptr = goToEndLineOrWord(ptr);
        }
        // tovabb is van, mondjam meg?
        ptr = goToEndLineOrWord(ptr);
        MPS_RECORD_END_CHECK(ptr);

        /*************************-
         * reading row name
         * checking field 3 ranges
         ************************/
        index = ptr - m_startPtr + m_rowLength;
        if (index < FIELD_4_START) {
            invalidStartFieldError(index, FIELD_4_START);
        }
        ptr = copyId(ptr, row.m_name, index, FIELD_4_END,
                     errorCode, rowHashCode1, rowHashCode2);
        MPS_INCOMPLETE_RECORD_CHECK(ptr);
        index = ptr - m_startPtr + m_rowLength;
        if (index > FIELD_4_END + 1) {
            invalidEndFieldError(index, FIELD_4_END);
        }

        /*************************-
         * checking wether the row exists or not
         ************************/
        //timer.start();
        newNonzero = true;
        row_id_ptr = m_rows.getValueAndKey(row, rowPtr, rowHashCode1, rowHashCode2);
        if (row_id_ptr) {
            row_id = *row_id_ptr;
            bool & nonZero = m_actualColumnNonzeros[row_id];
            if (nonZero == true) {
                //                LPERROR("itt balhe van gyerekek " << row_id);
                //                std::cin.get();
                newNonzero = false;
            } else {
                nonZero = true;
            }

        } else {
            //if (strcmp(row.m_name, m_costRow.m_name) != 0) {
            if (row.m_name != m_costRow.m_name ) {
                invalidRowName(nameToString(row.m_name).c_str());
                ptr = goToEndLine(ptr);
                //LPWARNING("readColumnRecord end");
                MPS_RETURN(ptr);
            } else {
                isCostValue = true;
            }
        }
        /*timer.stop();
        _counter++;
        if (_counter % 10000 == 0) {
            LPINFO( timer.getTotalElapsed() );
        }*/

        /*************************-
         * reading the number
         ************************/
        ptr = goToEndLineOrWord(ptr);
        MPS_INCOMPLETE_RECORD_CHECK(ptr);
        index = ptr - m_startPtr + m_rowLength;
        if (index < FIELD_5_START) {
            invalidStartFieldError(index, FIELD_5_START);
        }
        ptr = parseDouble(ptr, &value);
        if (index > FIELD_5_END + 1) {
            invalidEndFieldError(index, FIELD_5_END);
        }
        //TODO: komment kezelese
        index = ptr - m_startPtr + m_rowLength;
        if (value != 0.0) {
            //col_ptr->m_coeffs.insert(std::make_pair(row_id, value));
            //col_ptr->m_coeffs.insert(row_id, value);
            if (newNonzero == false) {
                valueOverridedError(nameToString(col_ptr->m_name).c_str(),
                                    nameToString(rowPtr->m_name).c_str(),
                                    col_ptr->getValue(row_id), value);
            }
            /*if (value > (int) m_costVectorIndex) {
                col_ptr->add(row_id - 1, value);
            } else if (value > (int) m_costVectorIndex) {*/
            if (isCostValue == false) {
                col_ptr->add(row_id, value);
            } else if (m_section == SEC_RHS) {
                m_objectiveConstant = value;
            }
            //}
            if (m_section == SEC_COLUMNS) {
                //if (row_id == (int) m_costVectorIndex  ) {
                if (isCostValue == true) {
                    col_ptr->m_inCostVector = true;
                    Column::Pair pair;
                    pair.m_index = columns.getCount() - 1;
                    pair.m_value = value;
                    m_costVector.pushBack(pair);
                }
                if (newNonzero && isCostValue == false) {
                    rowPtr->m_nonZeros++;
                    m_nonZeros++;
                }
            }
        } else {
            m_actualColumnNonzeros[row_id] = false;
        }
        ptr = goToEndLineOrWord(ptr);
        if (*ptr == '*') {
            ptr = goToEndLine(ptr);
            // LPWARNING("readColumnRecord end");
            MPS_RETURN(ptr);
        }
        MPS_MISSING_COMMENT_CHECK(ptr);
    } else {
        //LPWARNING("before nextRowType");
        ptr = nextRowType(ptr, info);
        //LPWARNING("after nextRowType");
    }
    //LPWARNING("readColumnRecord end");
    MPS_RETURN(ptr);
}

void MpsModelBuilder::initBoundTypeMap() {

    m_boundTypeMap.resize(128);
    m_boundTypeMap['L'].resize(128, Bound::INVALID);
    m_boundTypeMap['L']['O'] = Bound::LO;
    m_boundTypeMap['L']['I'] = Bound::LI;

    m_boundTypeMap['F'].resize(128, Bound::INVALID);
    m_boundTypeMap['F']['X'] = Bound::FX;
    m_boundTypeMap['F']['R'] = Bound::FR;

    m_boundTypeMap['U'].resize(128, Bound::INVALID);
    m_boundTypeMap['U']['P'] = Bound::UP;
    m_boundTypeMap['U']['I'] = Bound::UI;

    m_boundTypeMap['M'].resize(128, Bound::INVALID);
    m_boundTypeMap['M']['I'] = Bound::MI;

    m_boundTypeMap['P'].resize(128, Bound::INVALID);
    m_boundTypeMap['P']['L'] = Bound::PL;

    m_boundTypeMap['B'].resize(128, Bound::INVALID);
    m_boundTypeMap['B']['V'] = Bound::BV;

    m_boundTypeMap['S'].resize(128, Bound::INVALID);
    m_boundTypeMap['S']['C'] = Bound::SC;

}

inline const char * MpsModelBuilder::readBoundRecord(const register char * ptr, ROW_INFO & info)
{
    initTooLongRecordSensor(ptr);
    if (*ptr == ' ') {
        MPS_ERROR_TYPE errorCode;
        info = NO_INFO;
        register int index = 0;
        /*************************-
         * reading bound type
         ************************/
        bool needNumber;
        ptr = goToEndLineOrWord(ptr);
        MPS_RECORD_END_CHECK(ptr);
        index = ptr - m_startPtr + m_rowLength;
        if (index < FIELD_0_START) {
            invalidStartFieldError(index, FIELD_0_START);
        }
        Numerical::Double bound;
        Bound::BoundId boundId;
        //LO, LI, FX, FR, UP, UI, MI, PL, BV, SC
        char c1 = *ptr;
        char c2 = ptr[1];
        boundId.m_type = m_boundTypeMap[c1][c2];
        /*boundId.m_type = Bound::INVALID;
        if (c1 == 'L' && c2 == 'O')
            boundId.m_type = Bound::LO;
        else if (c1 == 'F' && c2 == 'X')
            boundId.m_type = Bound::FX;
        else if (c1 == 'F' && c2 == 'R')
            boundId.m_type = Bound::FR;
        else if (c1 == 'M' && c2 == 'I')
            boundId.m_type = Bound::MI;
        else if (c1 == 'P' && c2 == 'L')
            boundId.m_type = Bound::PL;
        else if (c1 == 'B' && c2 == 'V')
            boundId.m_type = Bound::BV;
        else if (c1 == 'L' && c2 == 'I')
            boundId.m_type = Bound::LI;
        else if (c1 == 'U' && c2 == 'I')
            boundId.m_type = Bound::UI;
        else if (c1 == 'U' && c2 == 'P')
            boundId.m_type = Bound::UP;
        else if (c1 == 'S' && c2 == 'C')
            boundId.m_type = Bound::SC;*/

        needNumber = boundId.m_type != Bound::FR &&
                boundId.m_type != Bound::MI &&
                boundId.m_type != Bound::PL &&
                boundId.m_type != Bound::BV &&
                boundId.m_type != Bound::MI;

        if (c1 == '*' || c2 == '*') {
            info = COMMENT;
            ptr = goToEndLine(ptr);
            MPS_RETURN(ptr);
        } else if (boundId.m_type == Bound::INVALID) {
            char bound[3] = {c1, c2, 0};
            invalidBoundType(bound);
            ptr = goToEndLine(ptr);
            MPS_RETURN(ptr);
        }
        ptr += 2;
        index += 2;
        if (index > FIELD_0_END + 1) {
            invalidEndFieldError(index, FIELD_0_END);
        }

        unsigned int boundHashCode1 = 0;
        unsigned int boundHashCode2 = 0;
        unsigned int columnHashCode1 = 0;
        unsigned int columnHashCode2 = 0;

        /*************************-
         * reading bound vector name
         ************************/
        ptr = goToEndLineOrWord(ptr);
        MPS_INCOMPLETE_RECORD_CHECK(ptr);
        index = ptr - m_startPtr + m_rowLength;

        if (index < FIELD_1_START) {
            invalidStartFieldError(index, FIELD_1_START);
        }
        Bound bound_vec;

        //bound_vec.m_name = GetNewKey(m_hkp);
        ptr = copyId(ptr, bound_vec.m_name, index, FIELD_1_END,
                     errorCode, boundHashCode1, boundHashCode2);

        MPS_INCOMPLETE_RECORD_CHECK(ptr);
        index = ptr - m_startPtr + m_rowLength;
        if (index > FIELD_1_END + 1) {
            invalidEndFieldError(index, FIELD_1_END);
        }

        /*************************-
         * checking wether the row exists or not
         ************************/
        Bound * bound_ptr;
        bound_ptr = m_bounds.getKey(bound_vec, boundHashCode1, boundHashCode2);
        if (bound_ptr == 0) {
            bound_ptr = m_bounds.add(bound_vec, m_bounds.getCount(), boundHashCode1, boundHashCode2);
        }

        /*************************-
         * reading column vector name
         ************************/
        ptr = goToEndLineOrWord(ptr);
        MPS_INCOMPLETE_RECORD_CHECK(ptr);
        index = ptr - m_startPtr + m_rowLength;
        if (index < FIELD_2_START) {
            invalidStartFieldError(index, FIELD_2_START);
        }
        Column col;

        //col.m_name = GetNewKey(m_hkp);

        ptr = copyId(ptr, col.m_name, index, FIELD_2_END,
                     errorCode, columnHashCode1, columnHashCode2);
        if (needNumber) {
            MPS_INCOMPLETE_RECORD_CHECK(ptr);
        }
        index = ptr - m_startPtr + m_rowLength;
        if (index > FIELD_2_END + 1) {
            invalidEndFieldError(index, FIELD_2_END);
        }

        static Timer timer;
        static int _counter = 0;
        //timer.start();
        int * colIdPtr = m_columns.getValue(col, columnHashCode1, columnHashCode2);
        /*timer.stop();
        _counter++;
        if (_counter % 10000 == 0) {
            LPINFO( timer.getTotalElapsed() );
        }*/

        int col_id;
        if (colIdPtr != 0) {
            col_id = *colIdPtr;
        } else {
            invalidColumnName(nameToString(col.m_name).c_str());
            ptr = goToEndLine(ptr);
            MPS_RETURN(ptr);
        }

        /*************************-
         * reading the number
         ************************/
        ptr = goToEndLineOrWord(ptr);
        if (needNumber) {
            MPS_INCOMPLETE_RECORD_CHECK(ptr);
            index = ptr - m_startPtr + m_rowLength;
            if (index < FIELD_3_START) {
                invalidStartFieldError(index, FIELD_3_START);
            }
            ptr = parseDouble(ptr, &bound);
            index = ptr - m_startPtr + m_rowLength;
            if (index > FIELD_3_END + 1) {
                invalidEndFieldError(index, FIELD_3_END);
            }
            boundId.m_column = col_id;
            bound_ptr->m_bounds.insert(std::make_pair(boundId, bound));
        }
        setBound(col_id, boundId.m_type, bound);
        ptr = goToEndLineOrWord(ptr);
        if (*ptr == '*') {
            ptr = goToEndLine(ptr);
            MPS_RETURN(ptr);
        }
        MPS_MISSING_COMMENT_CHECK(ptr);
        ptr = goToEndLineOrWord(ptr);
    } else {
        ptr = nextRowType(ptr, info);
    }
    MPS_RETURN(ptr);
}

void MpsModelBuilder::setBound(unsigned int columnIndex, Bound::BOUND_TYPE type,
                               Numerical::Double bound, bool newReady)
{
    //DEVINFO(D::MPSREADER,"MpsProblem::setBound " << columnIndex << ", " << type << " " << bound);
    //#define REPORT_BOUND_ERRORS
    Numerical::Double oldLower = m_columnLower.at(columnIndex);
    Numerical::Double oldUpper = m_columnUpper.at(columnIndex);
    Numerical::Double newLower = oldLower;
    Numerical::Double newUpper = oldUpper;
#ifdef REPORT_BOUND_ERRORS
    bool readyLower = (m_boundReady[columnIndex] & LOWER_BOUND_READY_MASK) > 0;
    bool readyUpper = (m_boundReady[columnIndex] & UPPER_BOUND_READY_MASK) > 0;
#endif
    bool isFixed = oldLower == oldUpper;
    char newReadyLower = 0;
    char newReadyUpper = 0;
#ifdef REPORT_BOUND_ERRORS
    std::stringstream str;
#endif
    //DEVINFO(D::MPSREADER,oldLower << " - " << oldUpper);
    if (isFixed) {
#ifdef REPORT_BOUND_ERRORS
        if (bound != oldLower) {
            //            LPERROR("isFixed && bound");
            m_errors.push_back(MpsError<BOUND_CHANGED > (m_line, m_section,
                                                         "bound is already fixed type", MpsErrorData::FATAL_ERROR, false).getData());
        }
#endif
        return;
    }

    switch (type) {
    case Bound::MI:
        bound = -Numerical::Infinity;
    case Bound::LI:
    case Bound::LO:
        //DEVINFO(D::MPSREADER,"changing lower bound " << readyLower);
        newLower = bound;
        newReadyLower = LOWER_BOUND_READY_MASK;
#ifdef REPORT_BOUND_ERRORS
        if (readyLower) {
            if (newLower < oldLower) {
                str << "lower bound has decreased from " << oldLower << " to " << newLower;
                m_errors.push_back(MpsError<BOUND_CHANGED > (m_line, m_section,
                                                             str.str(), MpsErrorData::WARNING).getData());
                //LPWARNING("newLower < oldLower");
            } else if (newLower > oldLower && newLower < oldUpper) {
                str << "lower bound has increased from " << oldLower << " to " << newLower;
                m_errors.push_back(MpsError<BOUND_CHANGED > (m_line, m_section,
                                                             str.str(), MpsErrorData::WARNING).getData());
                //LPWARNING("newLower > oldLower && newLower < oldUpper");
            } else if (newLower == oldUpper) {
                str << "lower bound has increased from " << oldLower << " to " << newLower <<
                       ", and bound type has become fixed type";
                m_errors.push_back(MpsError<BOUND_CHANGED > (m_line, m_section,
                                                             str.str(), MpsErrorData::WARNING).getData());
                //LPWARNING("newLower == oldUpper");
            } else if (newLower > oldUpper) {
                str << "lower bound has changed from " << oldLower << " to " << newLower <<
                       " but upper bound is less: " << oldUpper;
                m_errors.push_back(MpsError<BOUND_CHANGED > (m_line, m_section,
                                                             str.str(), MpsErrorData::FATAL_ERROR, false).getData());
                //LPERROR("newLower > oldUpper");
                return;
            }
        }
#endif
        break;
    case Bound::PL:
        bound = Numerical::Infinity;
    case Bound::UI:
    case Bound::UP:
        newUpper = bound;
        newReadyUpper = UPPER_BOUND_READY_MASK;
#ifdef REPORT_BOUND_ERRORS
        if (readyUpper) {
            if (newUpper > oldUpper) {
                str << "upper bound has increased from " << oldUpper << " to " << newUpper;
                m_errors.push_back(MpsError<BOUND_CHANGED > (m_line, m_section,
                                                             str.str(), MpsErrorData::WARNING).getData());
                //LPWARNING("newUpper > oldUpper");
            } else if (newUpper > oldLower && newUpper < oldUpper) {
                str << "upper bound has decreased from " << oldUpper << " to " << newUpper;
                m_errors.push_back(MpsError<BOUND_CHANGED > (m_line, m_section,
                                                             str.str(), MpsErrorData::WARNING).getData());
                //                    LPWARNING("newUpper > oldLower && newUpper < oldUpper < oldUpper");
            } else if (newUpper == oldLower) {
                str << "upper bound has decreased from " << oldUpper << " to " << newUpper <<
                       ", and bound type has become fixed type";
                m_errors.push_back(MpsError<BOUND_CHANGED > (m_line, m_section,
                                                             str.str(), MpsErrorData::WARNING).getData());
                //LPWARNING("newUpper == oldLower");
            } else if (newUpper < oldLower) {
                str << "upper bound has changed from " << oldUpper << " to " << newUpper <<
                       " but lower bound is greater: " << oldLower;
                m_errors.push_back(MpsError<BOUND_CHANGED > (m_line, m_section,
                                                             str.str(), MpsErrorData::FATAL_ERROR, false).getData());
                LPERROR("newUpper < oldLower");
                return;
            }
        } // ha az elejen a 0 ala ment a felso korlat, es kesobb nem lett
#endif
        // javitva akkor ezt is detektalni kell!
        break;
    case Bound::FX:
        newLower = bound;
        newUpper = bound;
        newReadyLower = LOWER_BOUND_READY_MASK;
        newReadyUpper = UPPER_BOUND_READY_MASK;
#ifdef REPORT_BOUND_ERRORS
        if (readyLower || readyUpper) {
            str << "bound type has become fixed type, bound: " << bound;
            m_errors.push_back(MpsError<BOUND_CHANGED > (m_line, m_section,
                                                         str.str(), MpsErrorData::WARNING).getData());
            //                LPWARNING("changed to fixed type");
        }
#endif
        break;
    case Bound::FR:
        newLower = -Numerical::Infinity;
        newUpper = Numerical::Infinity;
        newReadyLower = LOWER_BOUND_READY_MASK;
        newReadyUpper = UPPER_BOUND_READY_MASK;
#ifdef REPORT_BOUND_ERRORS
        if (readyLower || readyUpper) {
            m_errors.push_back(MpsError<BOUND_CHANGED > (m_line, m_section,
                                                         "bound type has become free type", MpsErrorData::WARNING).getData());
            //                LPWARNING("changed to free type");
        }
#endif
        break;
    case Bound::BV:
        setBound(columnIndex, Bound::LI, 0, false);
        setBound(columnIndex, Bound::UI, 1, true);
        return;
        break;
    case Bound::SC:
        // TODO: utananezni
        break;
    case Bound::INVALID:
        break;
    }

    if (newLower != oldLower) {
        m_columnLower[columnIndex] = newLower;
        if (oldLower == 0) {
            m_columnLowerNonZeros++;
        } else if (newLower == 0) {
            m_columnLowerNonZeros--;
        }
    }
    if (newUpper != oldUpper) {
        m_columnUpper[columnIndex] = newUpper;
        if (oldUpper == 0) {
            m_columnUpperNonZeros++;
        } else if (newUpper == 0) {
            m_columnUpperNonZeros--;
        }
    }
    if (newReady) {
        m_boundReady[columnIndex] |= newReadyLower | newReadyUpper;
    }
}

void MpsModelBuilder::printVariableStatistics()
{
    m_plusTypeCount = 0;
    m_minusTypeCount = 0;
    m_fixTypeCount = 0;
    m_boundedTypeCount = 0;
    m_freeTypeCount = 0;
    std::vector<Numerical::Double>::iterator iteratorLower = m_columnLower.begin();
    std::vector<Numerical::Double>::iterator iteratorLowerEnd = m_columnLower.end();
    std::vector<Numerical::Double>::iterator iteratorUpper = m_columnUpper.begin();
    //DEVINFO(D::MPSREADER, m_columnLower.size() );
    for (; iteratorLower < iteratorLowerEnd; iteratorLower++, iteratorUpper++) {
        Numerical::Double lower = *iteratorLower;
        Numerical::Double upper = *iteratorUpper;
        //DEVINFO(D::MPSREADER,lower << ", " << upper);
        //cin.get();
        if (lower == upper) {
            m_fixTypeCount++;
        } else if (lower == -Numerical::Infinity && upper == Numerical::Infinity) {
            m_freeTypeCount++;
        } else if (lower == -Numerical::Infinity && upper < Numerical::Infinity) {
            m_minusTypeCount++;
        } else if (lower > -Numerical::Infinity && upper == Numerical::Infinity) {
            m_plusTypeCount++;
        } else if (lower > -Numerical::Infinity && upper < Numerical::Infinity) {
            m_boundedTypeCount++;
        } else {

        }// TODO: mi van ha upper = lower = +/- infinity ?
    }
    LPINFO("Number of fixed type variables: " << m_fixTypeCount);
    LPINFO("Number of bounded type variables: " << m_boundedTypeCount);
    LPINFO("Number of minus type variables: " << m_minusTypeCount);
    LPINFO("Number of plus type variables: " << m_plusTypeCount);
    LPINFO("Number of free type variables: " << m_freeTypeCount);
}

void MpsModelBuilder::loadFromFile(const char * filename)
{
    const char * shortFileName = filename;
    const char * lastslash = NULL;
    while (*shortFileName != '\0') {
        if (*shortFileName == '\\' || *shortFileName == '/') {
            lastslash = shortFileName;
        }
        shortFileName++;
    }
    if (lastslash != NULL) {
        m_fileName = ++lastslash;
    } else {
        m_fileName = "NAME ERROR";
    }

    //    Numerical::Double val;
    //    Numerical::Double refVal = atof("-1"); //-1.4001242e+10;
    //    //parseDouble("2.002490", &val);
    //    parseDouble("0.020169E-03", &val);
    //    std::cerr.precision(10);
    //    LPINFO(val << ", " << refVal);
    //    LPINFO((val == refVal));
    //    exit(1);
    //    std::cin.get();
    //    return;

    clock_t cl_start, cl_end;
    initBoundTypeMap();
    cl_start = clock();

    m_objectiveConstant = 0;
    m_rhsIndex = 0;
    m_boundsIndex = 0;
    m_rangesIndex = 0;
    m_columnLowerNonZeros = 0;
    m_columnUpperNonZeros = 0;

    MpsErrorData::sm_warningCount = 0;
    MpsErrorData::sm_errorCount = 0;
    MpsErrorData::sm_fatalErrorCount = 0;

    m_modifyNumericErrors = 0;
    m_modifyLogicErrors = 0;

    MpsError<INVALID_ROW_TYPE>::initCount();
    MpsError<INVALID_ROW_NAME>::initCount();
    MpsError<INVALID_COLUMN_NAME>::initCount();
    MpsError<INVALID_START_FIELD>::initCount();
    MpsError<INVALID_END_FIELD>::initCount();
    MpsError<UNNECESSARY_CONTENT>::initCount();
    MpsError<TOO_LONG_WORD>::initCount();
    MpsError<TOO_LONG_RECORD>::initCount();
    MpsError<MISSING_COMMENT>::initCount();
    MpsError<ROW_EXISTS>::initCount();
    MpsError<COLUMN_EXISTS>::initCount();
    MpsError<INCOMPLETE_RECORD>::initCount();
    MpsError<INVALID_BOUND_TYPE>::initCount();
    MpsError<INVALID_COMMENT>::initCount();
    MpsError<INVALID_NUMBER_FORMAT>::initCount();
    MpsError<INVALID_SECTION_NAME>::initCount();
    MpsError<UNKNOWN_SECTION_NAME>::initCount();
    MpsError<NAME_MISSING>::initCount();
    MpsError<ROWS_MISSING>::initCount();
    MpsError<COLUMNS_MISSING>::initCount();
    MpsError<RHS_MISSING>::initCount();
    MpsError<RANGES_MISSING>::initCount();
    MpsError<BOUNDS_MISSING>::initCount();
    MpsError<ENDATA_MISSING>::initCount();
    MpsError<BOUND_CHANGED>::initCount();
    MpsError<VALUE_OVERRIDED>::initCount();

    bool nameOk = false;
    bool rowsOk = false;
    bool columnsOk = false;
    bool rhsOk = false;
    bool rangesOk = false;
    bool boundsOk = false;
    bool endataOk = false;
    m_costVectorReady = false;
    m_nonZeros = 0;
    m_actualColumnData = 0;
    //m_actualColumn.m_nameFragments[0] = 0;
    m_actualColumn = 0;
    m_actualColumnNonzeros = 0;
    bool wellFormedRecord;
    try {
        m_file.openForRead(filename, BUFF_SIZE, MPS_RECORD_SIZE);
        ROW_INFO info;
        m_section = SEC_NAME;
        m_line = 1;

        const char * ptr = m_file.getBuffer();



        while (!m_file.endOfFile(ptr) && (m_section != SEC_ENDATA)) {
            ptr = m_file.nextBlock(ptr);
            switch (m_section) {
            case SEC_NAME:
                ptr = readName(ptr, info);
                if (info != NO_INFO && info != COMMENT) {
                    if (info == NAME_SECTION && nameOk == false) {
                        nameOk = true;
                    } else if (info == ROWS_SECTION) {
                        if (nameOk == false) {
                            missingNameSection();
                        }
                        m_section = SEC_ROWS;
                        rowsOk = true;
                    } else {
                        missingNameSection();
                        missingRowsSection();
                    }
                }
                break;
            case SEC_ROWS:
                ptr = readRowRecord(ptr, info);
                if (info != NO_INFO && info != COMMENT) {
                    if (info != COLUMNS_SECTION) {
                        missingColumnsSection();
                    } else {
                        m_section = SEC_COLUMNS;
                        columnsOk = true;
                        m_actualColumnNonzeros = new bool[m_rows.getCount()];
                        bool * temp = m_actualColumnNonzeros;
                        bool * end = m_actualColumnNonzeros + m_rows.getCount();
                        while (temp < end) {
                            *temp = false;
                            temp++;
                        }
                    }
                }
                break;

            case SEC_COLUMNS:
                //ptr = fastReadColumnRecord(ptr, &wellFormedRecord, info);
                //if (wellFormedRecord == false) {
                    ptr = readColumnRecord(ptr, m_columns, info, &m_columnIndexTable);
                //}
                if (info != NO_INFO && info != COMMENT) {
                    m_columnLower.resize(m_columns.getCount(), 0);
                    m_columnUpper.resize(m_columns.getCount(), Numerical::Infinity);
                    m_boundReady.resize(m_columns.getCount(), 0);
                    m_columnLowerNonZeros = 0;
                    m_columnUpperNonZeros = m_columns.getCount();
                    clearActualColumnData();
                    m_actualColumnData = 0;
                    //m_actualColumn.m_nameFragments[0] = 0;
                    m_actualColumn = 0;
                    if (info != RHS_SECTION) {
                        missingRhsSection();
                        if (info == RANGES_SECTION) {
                            m_section = SEC_RANGES;
                            rangesOk = true;
                        } else if (info == BOUNDS_SECTION) {
                            m_section = SEC_BOUNDS;
                            boundsOk = true;
                        } else if (info == ENDATA_SECTION) {
                            m_section = SEC_ENDATA;
                            endataOk = true;
                        }
                    } else {
                        m_section = SEC_RHS;
                        rhsOk = true;
                    }
                }
                break;
            case SEC_RHS:
                //cout << "rhs olvasas elott: |" << temp << "|" << endl;
                //cin.get();
                //LPWARNING("read RHS");
                ptr = readColumnRecord(ptr, m_rhs, info, &m_rhsIndexTable);
                //LPWARNING("after read RHS");
                //LPWARNING("begin");
                if (info != NO_INFO && info != COMMENT) {
                    clearActualColumnData();
                    m_actualColumnData = 0;
                    //m_actualColumn.m_nameFragments[0] = 0;
                    m_actualColumn = 0;
                    if (info == RANGES_SECTION) {
                        m_section = SEC_RANGES;
                        //LPINFO("reading RANGES section");
                        rangesOk = true;
                    } else if (info == BOUNDS_SECTION) {
                        m_section = SEC_BOUNDS;
                        //LPINFO("reading BOUNDS section");
                        boundsOk = true;
                    } else if (info == ENDATA_SECTION) {
                        m_section = SEC_ENDATA;
                        //LPINFO("reading ENDATA section");
                        endataOk = true;
                    }
                }
                //LPWARNING("end");
                break;
            case SEC_RANGES:
                //cout << "ranges olvasas elott: |" << temp << "|" << endl;
                //cin.get();
                ptr = readColumnRecord(ptr, m_ranges, info, &m_rangeIndexTable);
                if (info != NO_INFO && info != COMMENT) {
                    clearActualColumnData();
                    m_actualColumnData = 0;
                    //m_actualColumn.m_nameFragments[0] = 0;
                    m_actualColumn = 0;
                    if (info == BOUNDS_SECTION) {
                        m_section = SEC_BOUNDS;
                        //LPINFO("reading BOUNDS section");
                        boundsOk = true;
                    } else if (info == ENDATA_SECTION) {
                        m_section = SEC_ENDATA;
                        //LPINFO("reading ENDATA section");
                        endataOk = true;
                    }
                }
                break;
            case SEC_BOUNDS:
                ptr = readBoundRecord(ptr, info);
                if (info != NO_INFO && info != COMMENT) {
                    clearActualColumnData();
                    m_actualColumnData = 0;
                    //m_actualColumn.m_nameFragments[0] = 0;
                    m_actualColumn = 0;
                    if (info == ENDATA_SECTION) {
                        m_section = SEC_ENDATA;
                        //LPINFO("reading ENDATA section");
                        endataOk = true;
                    }
                }
                break;
            case SEC_ENDATA:
                break;
            }
            ptr++;
            m_line++;
        }
        m_line--;
        m_file.close();

        collectConstraintBounds();
        collectVariableBounds();

        if (nameOk == false) {
            missingNameSection();
        }
        if (rowsOk == false) {
            missingRowsSection();
        }
        if (columnsOk == false) {
            missingColumnsSection();
        }
        if (rhsOk == false) {
            m_columnLower.resize(m_columns.getCount(), 0);
            m_columnUpper.resize(m_columns.getCount(), Numerical::Infinity);
            m_boundReady.resize(m_columns.getCount(), 0);
            m_columnLowerNonZeros = 0;
            m_columnUpperNonZeros = m_columns.getCount();

            missingRhsSection();
        }
        if (rangesOk == false) {
            missingRangesSection();
        }
        if (boundsOk == false) {
            missingBoundsSection();
        }
        if (endataOk == false) {
            missingEndataSection();
        }

    } catch (const File::EndOfFileException & error) {

    } catch (const File::FileNotFoundException & error) {
        LPERROR("File not found: " << filename);
        delete [] m_actualColumnNonzeros;
        return;
    } catch (const FatalErrorException & error) {
        LPERROR(error.getError());
    } catch (const std::string & error) {
        LPERROR(error);
    } catch (...) {
        LPERROR("Unknown error");
    }
    delete [] m_actualColumnNonzeros;
    m_actualColumnNonzeros = 0;
    cl_end = clock();
    cl_mps = (Numerical::Double) (cl_end - cl_start);
    LPINFO("errors: " << m_errors.size() );
}

void MpsModelBuilder::collectConstraintBounds()
{
    const unsigned int rhsIndex = 0;
    const unsigned int rangesIndex = 0;
    const unsigned int rowCount = getRowCount();
    m_constraints.resize(getRowCount());
    Vector ranges(rowCount);
    Vector rhs(rowCount);
    ranges.setSparsityRatio(0.0);
    rhs.setSparsityRatio(0.0);
    std::vector<bool> rangesGiven(rowCount, false);
    //std::vector<bool> rhsGiven(rowCount, false);
    getColumnVector(rhsIndex, &rhs, m_rhs, 0);

    getColumnVector(rangesIndex, &ranges, m_ranges, &rangesGiven);

    // row type       sign of r       h          u
    //----------------------------------------------
    //   G            + or -         b        b + |r|
    //   L            + or -       b - |r|      b
    //   E              +            b        b + |r|
    //   E              -          b - |r|      b
    // rowLower es rowUpper beallitasa
    // TODO: VISSZA
    std::vector<Row*>::iterator rowIterator = m_rowIndexTable.begin();
    std::vector<Row*>::iterator rowIteratorEnd = m_rowIndexTable.end();
    int rowIndex = 0;
    for (; rowIterator != rowIteratorEnd; rowIterator++, rowIndex++) {
        Constraint & constraint = m_constraints[rowIndex];
        Numerical::Double b = rhs.at(rowIndex);
        Numerical::Double r = ranges.at(rowIndex);
        //            if (m_costVectorReady && rowIndex >= (int) m_costVectorIndex) {
        //                if (rowIndex == (int) m_costVectorIndex) {
        //                    model->setObjConst(-b);
        //                    model->setObjName(rowIter->first.m_name);
        //                    continue;
        //                }
        //                rowIndex--;
        //            }

        constraint.setName(nameToString((*rowIterator)->m_name).c_str());
        //switch (rowIter->first.m_type) {
        switch ( (*rowIterator)->m_type ) {
        case 'N':
            constraint.setLowerBound(-Numerical::Infinity);
            constraint.setUpperBound(Numerical::Infinity);
            break;
        case 'G':
            constraint.setLowerBound(b);
            if (!rangesGiven[rowIndex]) {
                r = Numerical::Infinity;
            }
            constraint.setUpperBound(b + Numerical::fabs(r));
            break;
        case 'L':
            if (!rangesGiven[rowIndex]) {
                r = -Numerical::Infinity;
            }
            constraint.setLowerBound(b - Numerical::fabs(r));
            constraint.setUpperBound(b);
            break;
        case 'E':
            if (r >= 0) {
                constraint.setLowerBound(b);
                constraint.setUpperBound(b + Numerical::fabs(r));
            } else if (r < 0) {
                constraint.setLowerBound(b - Numerical::fabs(r));
                constraint.setUpperBound(b);
            }
            break;
        default:
            DEVINFO(D::MPSREADER, "Error,  rowIter->first.m_type = " << rowIter->first.m_type);
            break;
        }

    }

    /*std::vector< std::map<Row, int> >::const_iterator rowHashIter =
            m_rows.m_table.begin();
    std::vector< std::map<Row, int> >::const_iterator rowHashIterEnd =
            m_rows.m_table.end();
    std::map<Row, int>::const_iterator rowIter;
    std::map<Row, int>::const_iterator rowIterEnd;
    for (; rowHashIter < rowHashIterEnd; rowHashIter++) {
        rowIter = rowHashIter->begin();
        rowIterEnd = rowHashIter->end();
        for (; rowIter != rowIterEnd; rowIter++) {
            int rowIndex = rowIter->second;
            Constraint & constraint = m_constraints[rowIndex];
            Numerical::Double b = rhs.at(rowIndex);
            Numerical::Double r = ranges.at(rowIndex);
            //            if (m_costVectorReady && rowIndex >= (int) m_costVectorIndex) {
            //                if (rowIndex == (int) m_costVectorIndex) {
            //                    model->setObjConst(-b);
            //                    model->setObjName(rowIter->first.m_name);
            //                    continue;
            //                }
            //                rowIndex--;
            //            }
            constraint.setName(rowIter->first.m_name);
            switch (rowIter->first.m_type) {
            case 'N':
                constraint.setLowerBound(-Numerical::Infinity);
                constraint.setUpperBound(Numerical::Infinity);
                break;
            case 'G':
                constraint.setLowerBound(b);
                if (!rangesGiven[rowIndex]) {
                    r = Numerical::Infinity;
                }
                constraint.setUpperBound(b + Numerical::fabs(r));
                break;
            case 'L':
                if (!rangesGiven[rowIndex]) {
                    r = -Numerical::Infinity;
                }
                constraint.setLowerBound(b - Numerical::fabs(r));
                constraint.setUpperBound(b);
                break;
            case 'E':
                if (r >= 0) {
                    constraint.setLowerBound(b);
                    constraint.setUpperBound(b + Numerical::fabs(r));
                } else if (r < 0) {
                    constraint.setLowerBound(b - Numerical::fabs(r));
                    constraint.setUpperBound(b);
                }
                break;
            default:
                DEVINFO(D::MPSREADER, "Error,  rowIter->first.m_type = " << rowIter->first.m_type);
                break;
            }
        }
    }*/

}

void MpsModelBuilder::collectVariableBounds()
{
    m_variables.resize(getColumnCount());

    LPINFO("size: " << m_columnIndexTable.size() );
    LPINFO("size: " << m_rowIndexTable.size() );

    unsigned int index;
    std::vector<Numerical::Double>::const_iterator upperIter = m_columnUpper.begin();
    std::vector<Numerical::Double>::const_iterator lowerIter = m_columnLower.begin();
    std::vector<Numerical::Double>::const_iterator upperIterEnd = m_columnUpper.end();
    for (index = 0; upperIter < upperIterEnd; index++, upperIter++, lowerIter++) {
        m_variables[ index ].setLowerBound(*lowerIter);
        m_variables[ index ].setUpperBound(*upperIter);
        char name[9];
        memcpy(name, &m_columnIndexTable[index]->m_name, 8);
        name[8] = 0;
        m_variables[ index ].setName(name);
    }
}

/*void MpsProblem::saveToFile(const char * filename, const Model & model)
{
    //FILE * fd = fopen(filename, "wb");
    std::ofstream ofs(filename, std::ios::out | std::ios::binary);
    if (!ofs.is_open()) {
        LPERROR("Error when creating file: " << filename);
        return;
    }
    char * const buffer = new char[BUFF_SIZE];
    const char * const endBuffer = buffer + BUFF_SIZE;
    // feltetelezzuk, hogy a nev szekcio belefer a bufferbe
    char * actual = buffer;
    actual = saveName(ofs, actual, buffer, endBuffer, model);
    actual = saveRows(ofs, actual, buffer, endBuffer, model);
    actual = saveColumns(ofs, actual, buffer, endBuffer, model);
    actual = saveRhs(ofs, actual, buffer, endBuffer, model);
    actual = saveRanges(ofs, actual, buffer, endBuffer, model);
    actual = saveBounds(ofs, actual, buffer, endBuffer, model);

    strcpy(actual, "ENDATA");
    actual += 6;
    actual = saveEndLine(ofs, actual, buffer, endBuffer);
    ofs.write(buffer, actual - buffer);
    delete [] buffer;
    ofs.close();
}*/

char * MpsModelBuilder::saveEndLine(std::ofstream & ofs, char * actual, char * const buffer,
                                    const char * const endBuffer)
{
#ifndef __unix__
    *actual = '\r';
    actual++;
#endif
    *actual = '\n';
    actual++;
    *actual = '\0';
    if ((endBuffer - actual) < MPS_RECORD_SIZE) {
        //std::cout << "\"" << buffer << "\"" << std::endl;
        //std::cin.get();

        ofs.write(buffer, actual - buffer);
        actual = buffer;
    }
    return actual;
}

/*char * MpsProblem::saveName(std::ofstream & ofs, char * actual, char * const buffer,
    const char * const endBuffer, const Model & model)
{
    // NAME
    strcpy(actual, "NAME          ");
    actual += FIELD_2_START;
    strcpy(actual, model.getName().c_str());
    actual += model.getName().length();
    actual = saveEndLine(ofs, actual, buffer, endBuffer);
    return actual;
}

char * MpsProblem::saveRows(std::ofstream & ofs, char * actual, char * const buffer,
    const char * const endBuffer, const Model & model)
{
        strcpy(actual, "ROWS");
        actual += 4;
        actual = saveEndLine(ofs, actual, buffer, endBuffer);
        // eloszor a celfuggvenyt
        strcpy(actual, " N  ");
        actual += 4;
        strncpy(actual, model.m_objName, 8);
        actual += 8;
        actual = saveEndLine(ofs, actual, buffer, endBuffer);
        // johet a tobbi sor
        std::vector<Constraint>::const_iterator iter = model.m_constraints.begin();
        std::vector<Constraint>::const_iterator iterEnd = model.m_constraints.end();
        for (; iter != iterEnd; iter++) {
 *actual = ' ';
            actual++;
            switch (iter->getType()) {
                case Constraint::GREATER_OR_EQUAL:
 *actual = 'G';
                    break;
                case Constraint::LESS_OR_EQUAL:
                case Constraint::RANGE:
 *actual = 'L'; // majd a ranges-ben also korlatot adunk meg
                    break;
                case Constraint::EQUALITY:
 *actual = 'E';
                    break;
                case Constraint::NON_BINDING:
 *actual = 'N';
                    break;
            }
            actual++;
 *actual = ' ';
            actual[1] = ' ';
            actual += 2;
            strncpy(actual, iter->getName(), 8);
            actual += 8;
            actual = saveEndLine(ofs, actual, buffer, endBuffer);
        }
        return actual;
}*/

/*char * MpsProblem::saveColumns(std::ofstream & ofs, char * actual, char * const buffer,
    const char * const endBuffer, const Model & model)
{
        strcpy(actual, "COLUMNS");
        actual += 7;
        actual = saveEndLine(ofs, actual, buffer, endBuffer);
        std::vector<Variable>::const_iterator colIter = model.m_variables.begin();
        std::vector<Variable>::const_iterator colIterEnd = model.m_variables.end();
        unsigned int colIndex = 0;
        Vector cost = model.m_cost;
        cost.setSparsityRatio(0.0);
        //Numerical::Double ratio = model.m_cost.getSparsityRatio();
        //model.m_cost.setSparsityRatio(0.0);
        for (; colIter != colIterEnd; colIter++, colIndex++) {
            const char * name = colIter->getName();
            //if ((colIndex % 10 )== 0) LPINFO(colIndex << " / " << model.m_variables.size() );
            actual = saveOneColumn(ofs, actual, buffer, endBuffer, model,
                    model.m_matrix.column(colIndex), name, colIndex, 0.0, cost);
        }
        //model.m_cost.setSparsityRatio(ratio);
        return actual;
    }

    char * MpsProblem::saveRhs(std::ofstream & ofs, char * actual, char * const buffer,
            const char * const endBuffer, const Model & model) {
        strcpy(actual, "RHS");
        actual += 3;
        actual = saveEndLine(ofs, actual, buffer, endBuffer);
        Vector rhs(model.m_matrix.rowCount());
        rhs.setSparsityRatio(0.0);
        Vector cost = model.m_cost;
        cost.setSparsityRatio(0.0);

        std::vector<Constraint>::const_iterator iter = model.m_constraints.begin();
        std::vector<Constraint>::const_iterator iterEnd = model.m_constraints.end();
        unsigned int index = 0;
        for (; iter != iterEnd; iter++, index++) {
            switch (iter->getType()) {
                case Constraint::GREATER_OR_EQUAL:
                    // *actual = 'G';
                    rhs.set(index, iter->getLowerBound());
                    break;
                case Constraint::LESS_OR_EQUAL:
                    rhs.set(index, iter->getUpperBound());
                    break;
                case Constraint::RANGE:
                    rhs.set(index, iter->getUpperBound());
                    // *actual = 'L'; // majd a ranges-ben also korlatot adunk meg
                    break;
                case Constraint::EQUALITY:
                    rhs.set(index, iter->getLowerBound());
                    // *actual = 'E';
                    break;
                case Constraint::NON_BINDING:
                    // *actual = 'N';
                    break;
            }
        }
        actual = saveOneColumn(ofs, actual, buffer, endBuffer, model,
                rhs, "RHS1    ", -1, model.m_objConst, cost);

        return actual;
}*/

/*char * MpsProblem::saveRanges(std::ofstream & ofs, char * actual, char * const buffer,
    const char * const endBuffer, const Model & model)
{
        Vector ranges(model.m_matrix.rowCount());
        ranges.setSparsityRatio(0.0);
        Vector cost(2); // = model.m_cost;
        //cost.setSparsityRatio(0.0);

        std::vector<Constraint>::const_iterator iter = model.m_constraints.begin();
        std::vector<Constraint>::const_iterator iterEnd = model.m_constraints.end();
        unsigned int index = 0;
        bool needRanges = false;
        for (; iter != iterEnd; iter++, index++) {
            switch (iter->getType()) {
                case Constraint::GREATER_OR_EQUAL:
                    // *actual = 'G';
                    //rhs.set(index, iter->getLowerBound());
                    break;
                case Constraint::LESS_OR_EQUAL:
                    //rhs.set(index, iter->getUpperBound());
                    break;
                case Constraint::RANGE:
                    ranges.set(index, iter->getUpperBound() - iter->getLowerBound());
                    needRanges = true;
                    // *actual = 'L'; // majd a ranges-ben also korlatot adunk meg
                    break;
                case Constraint::EQUALITY:
                    //rhs.set(index, iter->getLowerBound());
                    // *actual = 'E';
                    break;
                case Constraint::NON_BINDING:
                    // *actual = 'N';
                    break;
            }
        }
        if (needRanges) {
            strcpy(actual, "RANGES");
            actual += 6;
            actual = saveEndLine(ofs, actual, buffer, endBuffer);
            actual = saveOneColumn(ofs, actual, buffer, endBuffer, model,
                    ranges, "RANGES1 ", -1, 0.0, cost);
        }
        return actual;
}*/

/*char * MpsProblem::saveBounds(std::ofstream & ofs, char * actual, char * const buffer,
    const char * const endBuffer, const Model & model)
{
        std::vector<Variable>::const_iterator iter = model.m_variables.begin();
        std::vector<Variable>::const_iterator iterEnd = model.m_variables.end();
        bool first = true;
        for (; iter != iterEnd; iter++) {
            if (!Numerical::isZero(iter->getLowerBound()) || iter->getUpperBound() != Numerical::Infinity) {
                if (first) {
                    first = false;
                    strcpy(actual, "BOUNDS");
                    actual = saveEndLine(ofs, actual + 6, buffer, endBuffer);
                }
            }
            switch (iter->getType()) {
                case Variable::PLUS: // also korlat veges
                    if (!Numerical::isZero(iter->getLowerBound())) {
                        //                    LPINFO(iter->getName() << ", " << iter->getLowerBound() << ", " << iter->getUpperBound());
                        //                    std::cin.get();
                        strcpy(actual, " LO BOUND1    ");
                        actual += 14;
                        strncpy(actual, iter->getName(), 8);
                        actual += 8;
 *actual = ' ';
                        actual[1] = ' ';
                        actual += 2;
                        sprintf(actual, "%12g", (double) iter->getLowerBound());
                        actual = saveEndLine(ofs, actual + 12, buffer, endBuffer);
                    }
                    break;
                case Variable::MINUS: // felso korlat veges, also korlat -vegtelen
                    // kell egy MI-t megadni a -vegtelen miatt, ekkor a felso 0 lesz
                    // kell egy  felsot megadni UP-val ha a felso nem nulla
                    strcpy(actual, " MI BOUND1    ");
                    strncpy(actual + 14, iter->getName(), 8);
                    actual = saveEndLine(ofs, actual + 12, buffer, endBuffer);
                    if (!Numerical::isZero(iter->getUpperBound())) {
                        strcpy(actual, " UP BOUND1    ");
                        actual += 14;
                        strncpy(actual, iter->getName(), 8);
                        actual += 8;
 *actual = ' ';
                        actual[1] = ' ';
                        actual += 2;
                        sprintf(actual, "%12g", (double) iter->getUpperBound());
                        actual = saveEndLine(ofs, actual + 12, buffer, endBuffer);
                    }
                    break;
                case Variable::BOUNDED: // mindket korlat veges
                    strcpy(actual, " UP BOUND1    ");
                    actual += 14;
                    strncpy(actual, iter->getName(), 8);
                    actual += 8;
 *actual = ' ';
                    actual[1] = ' ';
                    actual += 2;
                    sprintf(actual, "%12g", (double) iter->getUpperBound());
                    actual = saveEndLine(ofs, actual + 12, buffer, endBuffer);
                    if (!Numerical::isZero(iter->getLowerBound())) {
                        strcpy(actual, " LO BOUND1    ");
                        actual += 14;
                        strncpy(actual, iter->getName(), 8);
                        actual += 8;
 *actual = ' ';
                        actual[1] = ' ';
                        actual += 2;
                        sprintf(actual, "%12g", (double) iter->getLowerBound());
                        actual = saveEndLine(ofs, actual + 12, buffer, endBuffer);
                    }
                    break;
                case Variable::FIXED:
                case Variable::FREE:
                    if (iter->getType() == Variable::FIXED) {
                        strcpy(actual, " FX BOUND1    "); // hopp ezt ki kell egesziteni
                    } else {
                        strcpy(actual, " FR BOUND1    ");
                    }
                    actual += 14;
                    strncpy(actual, iter->getName(), 8);
                    actual += 8;
                    if (iter->getType() == Variable::FIXED) {
 *actual = ' ';
                        actual[1] = ' ';
                        actual += 2;
                        sprintf(actual, "%12g", (double) iter->getLowerBound());
                        actual += 12;
                    }
                    actual = saveEndLine(ofs, actual, buffer, endBuffer);
                    break;

            }
        }
        return actual;
}*/

/*char * MpsProblem::saveOneColumn(std::ofstream & ofs, char * actual, char * const buffer,
    const char * const endBuffer, const Model & model,
    const Vector & column, const char * columnName, int colIndex,
    Numerical::Double costValue, const Vector & costVector)
{
        //const Vector & cost = model.m_cost;
        Vector::ConstNonzeroIterator rowIter = column.beginNonzero();
        Vector::ConstNonzeroIterator rowIterEnd = column.endNonzero();
        const std::vector<Constraint> & constraints = model.m_constraints;
        bool isCostValue = false;
        if (colIndex > -1) {
            if (!Numerical::isZero(costVector.at(colIndex))) {
                isCostValue = true;
            }
        } else {
            if (costValue != 0.0) {
                isCostValue = true;
            }
        }
        //    if (strcmp(columnName, "PG1NG...") == 0) {
        //        LPINFO("itt van az oszlop: " << column);
        //        //            LPINFO("PG1NG... beolvasva  " << row.m_name << " " << value);
        //        std::cin.get();
        //    }

        while (rowIter != rowIterEnd || isCostValue) {
            strcpy(actual, "    "); // rekord eleje
            actual += 4;
            strncpy(actual, columnName, 8); // oszlop nevet bemasoljuk
            actual += 8;
 *actual = ' ';
            actual[1] = ' ';
            actual += 2;
            if (isCostValue) {
                strncpy(actual, model.m_objName, 8);
            } else {
                strncpy(actual, constraints[ rowIter.getIndex() ].getName(), 8);
            }
            actual += 8;
 *actual = ' ';
            actual[1] = ' ';
            actual += 2;
            if (isCostValue) {
                if (colIndex > -1) {
                    sprintf(actual, "%12g", (double) costVector.at((unsigned int) colIndex));
                } else {
                    sprintf(actual, "%12g", (double) costValue);
                }
                isCostValue = false;
            } else {
                sprintf(actual, "%12g", (double) *rowIter);
                rowIter++;
            }
            int len = strlen(actual);
            if (len > 12) {
                // balhe
            }
            actual += 12;
            if (rowIter != rowIterEnd) {
 *actual = ' ';
                actual[1] = ' ';
                actual[2] = ' ';
                actual += 3;
                strncpy(actual, constraints[ rowIter.getIndex() ].getName(), 8);
                actual += 8;
 *actual = ' ';
                actual[1] = ' ';
                actual += 2;
                sprintf(actual, "%12g", (double) *rowIter);
                int len = strlen(actual);
                if (len > 12) {
                    // balhe
                }
                actual += 12;
                rowIter++;
            }
            actual = saveEndLine(ofs, actual, buffer, endBuffer);
        }
        return actual;
}*/

/*void MpsProblem::buildModel(Model * model)
{
    model->setFileName(m_fileName);
    //clock_t cl_start, cl_end;

    model->clear();
    int colCount = m_columns.getCount();
    int rowCount = m_rows.getCount();
    model->m_modelName = m_name;
    model->init(rowCount - (m_costVectorReady ? 1 : 0), colCount);

    makeMatrix(model);
    makeRowBounds(model, rowCount, m_rhsIndex, m_rangesIndex);
    makeColumnBounds(model, m_boundsIndex);

    copyNames(model);

    //model->check();

    // sorting int columns and rows
    //std::cin.get();
    //exit(1);
    //LPINFO("convert ready");
    //cin.get();
    //model->m_cost.setSparsityRatio(0.0);
    //cl_end = clock();
    //LPINFO("Conversion time: " << ((Numerical::Double) (cl_end - cl_start) / (Numerical::Double) CLOCKS_PER_SEC) << " sec");
}*/

/*void MpsProblem::copyNames(Model * model)
{
    MpsNameStorer * nameStorer = new MpsNameStorer;
    model->m_names = nameStorer;
    nameStorer->set(m_hkp);
    std::vector< std::map<Column, int> >::iterator col_hash_iter = m_columns.m_table.begin();
    std::vector< std::map<Column, int> >::iterator col_hash_end = m_columns.m_table.end();
    for (; col_hash_iter < col_hash_end; col_hash_iter++) {
        std::map<Column, int>::iterator col_iter = col_hash_iter->begin();
        std::map<Column, int>::iterator col_end = col_hash_iter->end();
        for (; col_iter != col_end; col_iter++) {
            //col_iter->first.m_coeffs.print();
            //std::cin.get();
            const int columnIndex = col_iter->second;
            model->m_variables[columnIndex].setName(col_iter->first.getKey());
        }
    }
}*/

/*void MpsProblem::makeMatrix(Model * model)
{
    //    LPINFO("cost vector: " << m_costVectorIndex);
    //    clock_t cl_start, cl_end;
    //    cl_start = clock();
    const int colCount = m_columns.getCount();
    const int rowCount = m_rows.getCount();

    Matrix & matrix = model->getMatrix();
    Vector & costVector = model->getCost();
    std::vector< std::map<Row, int> >::iterator row_hash_iter = m_rows.m_table.begin();
    std::vector< std::map<Row, int> >::iterator row_hash_end = m_rows.m_table.end();
    for (; row_hash_iter < row_hash_end; row_hash_iter++) {
        std::map<Row, int>::iterator row_iter = row_hash_iter->begin();
        std::map<Row, int>::iterator row_iter_end = row_hash_iter->end();
        for (; row_iter != row_iter_end; row_iter++) {
            unsigned int rowIndex = row_iter->second;
            if (rowIndex != m_costVectorIndex) {
                if (rowIndex > m_costVectorIndex)
                    rowIndex--;
                Vector & vector = *matrix.m_rowWise[ rowIndex ];
                vector.prepareForData(row_iter->first.m_nonZeros, colCount);
            } else {
                costVector.prepareForData(row_iter->first.m_nonZeros, colCount);
            }
        }
    }

    std::vector< std::map<Column, int> >::iterator col_hash_iter = m_columns.m_table.begin();
    std::vector< std::map<Column, int> >::iterator col_hash_end = m_columns.m_table.end();
    for (; col_hash_iter < col_hash_end; col_hash_iter++) {
        std::map<Column, int>::iterator col_iter = col_hash_iter->begin();
        std::map<Column, int>::iterator col_end = col_hash_iter->end();
        for (; col_iter != col_end; col_iter++) {
            //col_iter->first.m_coeffs.print();
            //std::cin.get();
            const int columnIndex = col_iter->second;

            //            AvlTree<unsigned int, Numerical::Double>::Iterator row_iter = col_iter->first.m_coeffs.begin();
            //            AvlTree<unsigned int, Numerical::Double>::Iterator row_end = col_iter->first.m_coeffs.end();
            List< Column::Pair >::Iterator row_iter = col_iter->first.m_coeffs.begin();
            List< Column::Pair >::Iterator row_end = col_iter->first.m_coeffs.end();

            row_iter = col_iter->first.m_coeffs.begin();
            //            std::map<unsigned int, Numerical::Double>::const_iterator row_iter = col_iter->first.m_coeffs.begin();
            //            std::map<unsigned int, Numerical::Double>::const_iterator row_end = col_iter->first.m_coeffs.end();
            Vector & columnVector = *matrix.m_columnWise[ columnIndex ];
            columnVector.prepareForData(col_iter->first.m_coeffs.size()
                - (col_iter->first.m_inCostVector ? 1 : 0), rowCount -
                (m_costVectorReady ? 1 : 1));

            for (; row_iter != row_end; row_iter++) {
                unsigned int rowIndex = row_iter.getData().m_index;
                const Numerical::Double value = row_iter.getData().m_value;
                if (value == 1.0 / 0.0) {
                    LPINFO(col_iter->first.m_name << ": " << row_iter.getData().m_index);
                }
                if (rowIndex != m_costVectorIndex) {
                    if (rowIndex > m_costVectorIndex)
                        rowIndex--;
                    Vector & rowVector = *matrix.m_rowWise[ rowIndex ];
                    rowVector.newNonZero(value, columnIndex);
                    columnVector.newNonZero(value, rowIndex);
                    //cout<<row_iter->second<<"   [ "<<row_iter->first<<"; "<<col_iter->second<<"] "<<endl;
                    //matrix.setNewNonzero(rowIndex, col_iter->second, row_iter->second);
                } else {
                    costVector.newNonZero(value, columnIndex);
                }
            }
        }
    }
}*/

/*void MpsProblem::makeRowBounds(Model * model, int rowCount, int rhsIndex, int rangesIndex)
{
    //    clock_t cl_start, cl_end;
    //    cl_start = clock();

    Vector ranges(rowCount);
    Vector rhs(rowCount);
    ranges.setSparsityRatio(0.0);
    rhs.setSparsityRatio(0.0);
    std::vector<bool> rangesGiven(rowCount, false);
    //std::vector<bool> rhsGiven(rowCount, false);
    getColumnVector(rhsIndex, &rhs, m_rhs, 0);

    getColumnVector(rangesIndex, &ranges, m_ranges, &rangesGiven);

    model->getConstraints().resize(rowCount - (m_costVectorReady ? 1 : 0));
    //    for (int ii = m_rhs.m_table.size() / 10; ii < m_rhs.m_table.size() / 5; ii++) {
    //        std::cout<<ii<< " ";
    //        m_rhs.m_table[ii].begin()->first.m_coeffs.print();
    //        cin.get();
    //    }

    //std::cout << rhs << std::endl << std::endl;
    //std::cout << ranges << std::endl << std::endl;
    //cin.get();

    // row type       sign of r       h          u
    //----------------------------------------------
    //   G            + or -         b        b + |r|
    //   L            + or -       b - |r|      b
    //   E              +            b        b + |r|
    //   E              -          b - |r|      b
    // rowLower es rowUpper beallitasa
    //Vector & rowLower = model->getRowLower();
    //Vector & rowUpper = model->getRowUpper();
    std::vector< std::map<Row, int> >::const_iterator rowHashIter =
        m_rows.m_table.begin();
    std::vector< std::map<Row, int> >::const_iterator rowHashIterEnd =
        m_rows.m_table.end();
    std::map<Row, int>::const_iterator rowIter;
    std::map<Row, int>::const_iterator rowIterEnd;
    for (; rowHashIter < rowHashIterEnd; rowHashIter++) {
        rowIter = rowHashIter->begin();
        rowIterEnd = rowHashIter->end();
        for (; rowIter != rowIterEnd; rowIter++) {
            int rowIndex = rowIter->second;
            Numerical::Double b = rhs.at(rowIndex);
            Numerical::Double r = ranges.at(rowIndex);
            if (m_costVectorReady && rowIndex >= (int) m_costVectorIndex) {
                if (rowIndex == (int) m_costVectorIndex) {
                    model->setObjConst(-b);
                    model->setObjName(rowIter->first.m_name);
                    continue;
                }
                rowIndex--;
            }
            model->getConstraints().at(rowIndex).setName(rowIter->first.m_name);
            switch (rowIter->first.m_type) {
                case 'N':
                    //rowLower.set(rowIndex, -Numerical::Infinity);
                    //rowUpper.set(rowIndex, Numerical::Infinity);
                    model->getConstraints().at(rowIndex).setLowerBound(-Numerical::Infinity);
                    model->getConstraints().at(rowIndex).setUpperBound(Numerical::Infinity);
                    break;
                case 'G':
                    //                    LPINFO(model->getConstraints().at(rowIndex).getName() << " " << b << ", " << r);
                    //                    std::cin.get();
                    //rowLower.set(rowIndex, b);
                    model->getConstraints().at(rowIndex).setLowerBound(b);
                    if (!rangesGiven[rowIndex]) {
                        r = Numerical::Infinity;
                    }
                    //rowUpper.set(rowIndex, b + Numerical::fabs(r));
                    model->getConstraints().at(rowIndex).setUpperBound(b + Numerical::fabs(r));
                    break;
                case 'L':
                    //                    if (strcmp(model->getConstraints().at(rowIndex).getName(), "DCBOSORD") == 0) {
                    //                        LPINFO("megvagy: " << r << " " << b);
                    //                        std::cin.get();
                    //                    }
                    if (!rangesGiven[rowIndex]) {
                        r = -Numerical::Infinity;
                    }
                    //                    if (strcmp(model->getConstraints().at(rowIndex).getName(), "DCBOSORD") == 0) {
                    //                        LPINFO("megvagy: " << rowIndex << "   " << r << " " << b);
                    //                        std::cin.get();
                    //                    }
                    //rowLower.set(rowIndex, b - Numerical::fabs(r));
                    //rowUpper.set(rowIndex, b);
                    //LPINFO("b: " << b << ", r: " << r);
                    //LPINFO("fabs(r) = " << Numerical::fabs(r));
                    //LPINFO("b - Numerical::fabs(r) = " << (b - Numerical::fabs(r)));
                    //std::cin.get();
                    model->getConstraints().at(rowIndex).setLowerBound(b - Numerical::fabs(r));
                    model->getConstraints().at(rowIndex).setUpperBound(b);
                    break;
                case 'E':
                    if (r >= 0) {
                        //rowLower.set(rowIndex, b);
                        //rowUpper.set(rowIndex, b + Numerical::fabs(r));

                        model->getConstraints().at(rowIndex).setLowerBound(b);
                        model->getConstraints().at(rowIndex).setUpperBound(b + Numerical::fabs(r));
                    } else if (r < 0) {
                        //rowLower.set(rowIndex, b - Numerical::fabs(r));
                        //rowUpper.set(rowIndex, b);
                        model->getConstraints().at(rowIndex).setLowerBound(b - Numerical::fabs(r));
                        model->getConstraints().at(rowIndex).setUpperBound(b);
                    }
                    break;
                default:
                    DEVINFO(D::MPSREADER, "Error,  rowIter->first.m_type = " << rowIter->first.m_type);
                    break;
            }
        }
    }
    //    cl_end = clock();
    //    LPINFO("Elapsed time: " << ((Numerical::Double) (cl_end - cl_start) / (Numerical::Double) CLOCKS_PER_SEC) << " sec");

}*/

/*void MpsProblem::makeColumnBounds(Model * model, int boundsIndex)
{
    //    clock_t cl_start, cl_end;
    //    cl_start = clock();

    //Vector & columnLower = model->getColumnLower();
    //Vector & columnUpper = model->getColumnUpper();
    //columnLower.prepareForData(m_columnLowerNonZeros, m_columns.getCount());
    //columnUpper.prepareForData(m_columnUpperNonZeros, m_columns.getCount());
    std::vector<Variable> & variables = model->getVariables();
    variables.resize(m_columns.getCount());

    unsigned int index;
    std::vector<Numerical::Double>::const_iterator upperIter = m_columnUpper.begin();
    std::vector<Numerical::Double>::const_iterator lowerIter = m_columnLower.begin();
    std::vector<Numerical::Double>::const_iterator upperIterEnd = m_columnUpper.end();
    for (index = 0; upperIter < upperIterEnd; index++, upperIter++, lowerIter++) {
        //columnUpper.setNewNonzero(index, *upperIter);
        //columnLower.setNewNonzero(index, *lowerIter);
        //if (*lowerIter != 0.0) {
        //columnLower.newNonZero(*lowerIter, index);
        variables[ index ].setLowerBound(*lowerIter);
        //}
        //if (*upperIter != 0.0) {
        //columnUpper.newNonZero(*upperIter, index);

        variables[ index ].setUpperBound(*upperIter);
        //}
    }
    //    cl_end = clock();
    //    LPINFO("Elapsed time: " << ((Numerical::Double) (cl_end - cl_start) / (Numerical::Double) CLOCKS_PER_SEC) << " sec");

    return;
}*/

void MpsModelBuilder::getColumnVector(unsigned int columnIndex, Vector * vector,
                                      const HashTable<Column, int, hash_function<Column> > & columnHashTable,
                                      std::vector<bool> * given)
{
    __UNUSED(columnHashTable);
    Column * column = m_columnIndexTable[columnIndex];
    List< Column::Pair >::Iterator row_iter = column->m_coeffs.begin();
    List< Column::Pair >::Iterator row_end = column->m_coeffs.end();

    for (; row_iter != row_end; row_iter++) {
        if (given) {
            if (row_iter.getData().m_index >= m_costVectorIndex) {
                (*given)[row_iter.getData().m_index - 1] = true;
            } else {
                (*given)[row_iter.getData().m_index] = true;
            }
        }
        vector->set(row_iter.getData().m_index, row_iter.getData().m_value);
    }

    /*std::vector< std::map<Column, int> >::const_iterator columnHashIter =
        columnHashTable.m_table.begin();
    std::vector< std::map<Column, int> >::const_iterator columnHashEnd =
        columnHashTable.m_table.end();

    std::map<Column, int>::const_iterator col_iter;
    std::map<Column, int>::const_iterator col_end;
    for (; columnHashIter < columnHashEnd; columnHashIter++) {
        col_iter = columnHashIter->begin();
        col_end = columnHashIter->end();
        for (; col_iter != col_end; col_iter++) {
            if (col_iter->second == (int) columnIndex) {
                List< Column::Pair >::Iterator row_iter = col_iter->first.m_coeffs.begin();
                List< Column::Pair >::Iterator row_end = col_iter->first.m_coeffs.end();

                for (; row_iter != row_end; row_iter++) {
                    if (given) {
                        if (row_iter.getData().m_index >= m_costVectorIndex) {
                            (*given)[row_iter.getData().m_index - 1] = true;
                        } else {
                            (*given)[row_iter.getData().m_index] = true;
                        }
                    }
                    vector->set(row_iter.getData().m_index, row_iter.getData().m_value);
                }
            }
        }
    }*/
}

void MpsModelBuilder::setRhsIndex(int rhsIndex)
{

    m_rhsIndex = rhsIndex;
}

void MpsModelBuilder::setBoundsIndex(int boundsIndex)
{

    m_boundsIndex = boundsIndex;
}

void MpsModelBuilder::setRangesIndex(int rangesIndex)
{

    m_rangesIndex = rangesIndex;
}

int MpsModelBuilder::getRhsIndex() const
{

    return m_rhsIndex;
}

int MpsModelBuilder::getBoundsIndex() const
{

    return m_boundsIndex;
}

int MpsModelBuilder::getRangesIndex() const
{

    return m_rangesIndex;
}

int MpsModelBuilder::getRhsCount() const
{

    return m_rows.getCount();
}

int MpsModelBuilder::getBoundsCount() const
{

    return m_bounds.getCount();
}

int MpsModelBuilder::getRangesCount() const
{

    return m_ranges.getCount();
}

void MpsModelBuilder::report(int debugLevel)
{
    if (debugLevel >= 1.) {
        Numerical::Double sparsity;
        sparsity = m_nonZeros / (Numerical::Double) (m_columns.getCount() * m_rows.getCount());
        LPINFO("records: " << m_line << " , columns: " << m_columns.getCount() <<
               " , rows: " << m_rows.getCount() << ", non-zeros: " << m_nonZeros << " ( " << sparsity * 100 << " % )");
    }
    if (debugLevel >= 2.) {
        printVariableStatistics();
    }
    if (MpsErrorData::sm_warningCount > 0) {
        LPINFO(MpsErrorData::sm_warningCount << " warning" <<
               (MpsErrorData::sm_warningCount > 1 ? "s" : ""));
    }
    if (MpsErrorData::sm_errorCount > 0) {
        LPINFO(MpsErrorData::sm_errorCount << " error" <<
               (MpsErrorData::sm_errorCount > 1 ? "s" : ""));
    }
    if (MpsErrorData::sm_fatalErrorCount > 0) {
        LPINFO(MpsErrorData::sm_fatalErrorCount << " fatal error" <<
               (MpsErrorData::sm_fatalErrorCount > 1 ? "s" : ""));
    }
    if (m_modifyNumericErrors > 0) {
        LPERROR(m_modifyNumericErrors << " numerical modifier error" <<
                (m_modifyNumericErrors > 1 ? "s" : ""));
    }
    if (m_modifyLogicErrors > 0) {
        LPERROR(m_modifyLogicErrors << " logical modifier error" <<
                (m_modifyLogicErrors > 1 ? "s" : ""));
    }

    //LPINFO("MPS reading time: " << ((Numerical::Double) (cl_end - cl_start) / (Numerical::Double) CLOCKS_PER_SEC) << " sec");
    //if (MpsErrorData::sm_fatalErrorCount == 0) {
    //    LPINFO("MPS file has been successfully processed\n");
    //} else {
    //    LPINFO("MPS file has not been processed\n");
    //}
}

clock_t MpsModelBuilder::cl_mps = 0;

clock_t MpsModelBuilder::lastReadingTime()
{
    return cl_mps;
}

/*void MpsProblem::saveBasis(const char * fileName, const Model & model,
    unsigned int iterations)
{
        std::ofstream ofs(fileName, std::ios::out | std::ios::binary);

        if (ofs.is_open()) {
            char * const buffer = new char[BUFF_SIZE];
            const char * const endBuffer = buffer + BUFF_SIZE;
            // feltetelezzuk, hogy a nev szekcio belefer a bufferbe
            char * actual = buffer;

            strcpy(actual, "NAME          ");
            actual += FIELD_2_START;
            actual += sprintf(actual, "%s Iterations %u", model.getName().c_str(), iterations);
            actual = saveEndLine(ofs, actual, buffer, endBuffer);

            const std::vector<Variable> & variables = model.getVariables();
            const std::vector<Constraint> & constraints = model.getConstraints();
            std::vector<int>::const_iterator iter = model.getBasisHead().begin();
            std::vector<int>::const_iterator iterEnd = model.getBasisHead().end();
            const unsigned int structVariables = model.getMatrix().columnCount();
            unsigned int rowIndex = structVariables;
            while (iter < iterEnd) {
                while (iter < iterEnd && ((variables[ *iter ].getState() != Variable::BASIC) ||
 *iter >= (int) structVariables)) {
                    iter++;
                }
                if (iter < iterEnd) {
                    while (variables[rowIndex].getState() == Variable::BASIC) {
                        rowIndex++;
                    }
                    char state = 'L';
                    if (variables[rowIndex].getState() == Variable::NONBASIC_AT_UB) {
                        state = 'U';
                    }
                    actual += sprintf(actual, " X%c %8.8s  %8.8s", state,
                        variables[*iter].getName(),
                        constraints[rowIndex - structVariables].getName());
                    actual = saveEndLine(ofs, actual, buffer, endBuffer);
                    iter++;
                    rowIndex++;
                }
            }

            std::vector<Variable>::const_iterator varIter = variables.begin();
            std::vector<Variable>::const_iterator varIterEnd = variables.end();
            unsigned int index = 0;
            for (; index < structVariables; varIter++, index++) {
                if (varIter->getState() == Variable::BASIC) {
                    continue;
                }
                char state = 'L';
                if (varIter->getState() == Variable::NONBASIC_AT_UB) {
                    state = 'U';
                }
                actual += sprintf(actual, " %cL %8.8s", state, varIter->getName());
                actual = saveEndLine(ofs, actual, buffer, endBuffer);
            }
            strcpy(actual, "ENDATA");
            actual += 6;
            actual = saveEndLine(ofs, actual, buffer, endBuffer);
            ofs.write(buffer, actual - buffer);
            delete [] buffer;

            ofs.close();
        } else {
            LPERROR("Error at opening file " << fileName);
            return;
        }
}*/

unsigned int MpsModelBuilder::getColumnCount() const
{
    return m_columns.getCount();
}

unsigned int MpsModelBuilder::getRowCount() const
{
    return m_rows.getCount(); // - (m_costVectorReady ? 1 : 0);
}

const Variable & MpsModelBuilder::getVariable(unsigned int index) const
{
    return m_variables[index];
}

const Constraint & MpsModelBuilder::getConstraint(unsigned int index) const
{
    return m_constraints[index];
}

void MpsModelBuilder::buildRow(unsigned int, Vector *, std::vector<unsigned int> *) const
{
}

void MpsModelBuilder::buildColumn(unsigned int index, Vector * columnVector,
                                  std::vector<unsigned int> * nonzeros) const
{
    columnVector->prepareForData(m_columnIndexTable[index]->m_coeffs.size(), getRowCount());
    List<Column::Pair>::Iterator iter = m_columnIndexTable[index]->m_coeffs.begin();
    List<Column::Pair>::Iterator iterEnd = m_columnIndexTable[index]->m_coeffs.end();
    //    cout << "++++++++++++++++++++++++++++++++++++++" << endl;
    for (; iter != iterEnd; iter++) {
        columnVector->newNonZero(iter.getData().m_value, iter.getData().m_index);
        nonzeros->at(iter.getData().m_index)++;
    }
}

void MpsModelBuilder::buildCostVector(Vector * costVector) const
{
    costVector->prepareForData(m_costVector.size(), getColumnCount());
    List<Column::Pair>::Iterator iter = m_costVector.begin();
    List<Column::Pair>::Iterator iterEnd = m_costVector.end();
    for (; iter != iterEnd; iter++) {
        costVector->newNonZero(iter.getData().m_value, iter.getData().m_index);
    }
}

Numerical::Double MpsModelBuilder::getObjectiveConstant() const
{
    return m_objectiveConstant;
}

std::string MpsModelBuilder::getName() const
{
    return m_name;
}

bool MpsModelBuilder::hasRowwiseRepresentation() const
{
    return false;
}

bool MpsModelBuilder::hasColumnwiseRepresentation() const
{
    return true;
}

OBJECTIVE_TYPE MpsModelBuilder::getObjectiveType() const {
    return MINIMIZE;
}
