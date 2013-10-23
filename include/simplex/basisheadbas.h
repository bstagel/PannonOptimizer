#ifndef BASISHEADBAS_H
#define BASISHEADBAS_H

#include <globals.h>
#include <simplex/basisheadio.h>
#include <utils/hashtable.h>
#include <fstream>
#include <string>

#include <iostream>

class BasisHeadBAS: public BasisHeadIO {
public:
    BasisHeadBAS() {}

    ~BasisHeadBAS() {}

    void startWrting(const char * fileName);

    void addInfo(const char * info);

    void addBasicVariable(const Variable & variable,
                          unsigned int basisPosition,
                          Numerical::Double value);
    void addNonbasicVariable(const Variable & variable,
                             Simplex::VARIABLE_STATE state,
                             Numerical::Double value);
    void finishWriting();

    void startReading(const char * fileName,
                      unsigned int variableCount);

    void addVariable(const Variable & variable);

    void finishReading(std::vector<int> * basisHead,
                       IndexList<const Numerical::Double*> * variableStates);

private:
    std::ofstream m_outputFile;

    std::ifstream m_inputFile;

    struct BasicVariable {
        Variable m_variable;
        unsigned int m_basisPosition;
    };

    struct NonBasicVariable {
        Variable m_variable;
        Simplex::VARIABLE_STATE m_state;
    };

    struct VariableIndex {
        std::string m_name;
        unsigned int m_index;
        bool operator != (const VariableIndex & vi) const {
            return m_name != vi.m_name;
        }
    };

    class VariableIndexHash {
    public:
        static unsigned int getHash(const VariableIndex & vi) {
            unsigned int bits = 0;
            const unsigned int length = vi.m_name.length();
            unsigned int index;
            const unsigned int * ptr32 = reinterpret_cast<const unsigned int*>(vi.m_name.data());
            const unsigned int step = sizeof(unsigned int);
            //std::cout << "name: \"" << vi.m_name << std::endl;
            for (index = 0; index < length / step; index++) {
                //std::cout << std::hex << *ptr32 << " ";
                bits ^= *ptr32;
                ptr32++;
            }
            //std::cout << std::endl << std::hex << bits << std::endl;
            const unsigned char * ptr8 = reinterpret_cast<const unsigned char *>(vi.m_name.data() + step * index);
            index *= step;
            unsigned long long int littleBits = 0;
            unsigned int shift = 0;
            for (; index < length; index++) {
                littleBits |= *ptr8 << shift;
                shift += 8;
                ptr8++;
            }
            //std::cout << std::hex << littleBits << " ";
            bits ^= littleBits;
            //std::cout << std::hex << bits << std::endl;
            //std::cin.get();
            return bits;
        }
    };

    std::vector<BasicVariable> m_basicVariables;

    std::vector<NonBasicVariable> m_nonBasicVariables;

    std::string m_info;

    HashTable<VariableIndex, VariableIndexHash> m_hashTable;
};

#endif // BASISHEADBAS_H
