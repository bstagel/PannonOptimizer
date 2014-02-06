#ifndef BASISHEADBAS_H
#define BASISHEADBAS_H

#include <globals.h>
#include <simplex/basisheadio.h>
#include <utils/hashtable.h>
#include <fstream>
#include <string>
#include <cstring>

#include <iostream>

class BasisHeadBAS: public BasisHeadIO {
public:
    BasisHeadBAS() {}

    ~BasisHeadBAS() {}

    void startWrting(const char * fileName, const Model & model);

    void addInfo(const char * info);

    void addBasicVariable(const Variable * variable,
                          unsigned int basisPosition,
                          unsigned int variableIndex,
                          Numerical::Double value);
    void addNonbasicVariable(const Variable *variable,
                             unsigned int variableIndex,
                             Simplex::VARIABLE_STATE state,
                             Numerical::Double value);
    void finishWriting();

    void startReading(const char * fileName,
                      const Model & model);

    void addVariable(const Variable & variable);

    void finishReading();

private:
    std::ofstream m_outputFile;

    std::ifstream m_inputFile;

    struct BasicVariable {
        const Variable * m_variable;
        unsigned int m_basisPosition;
    };

    struct NonBasicVariable {
        const Variable * m_variable;
        Simplex::VARIABLE_STATE m_state;
    };

    struct VariableIndex {
        const Variable * m_variable;
        unsigned int m_index;
        bool operator != (const VariableIndex & vi) const {
            return strcmp(m_variable->getName(), vi.m_variable->getName()) != 0;
        }
        friend std::ostream & operator << (std::ostream & os, const VariableIndex & id) {
            os << id.m_variable->getName() << " ";
            return os;
        }
    };

    class VariableIndexHash {
    public:
        static unsigned int getHash(const VariableIndex & vi) {
            unsigned int bits = 0;
            const unsigned int length = strlen(vi.m_variable->getName());
            unsigned int index;
            const unsigned int * ptr32 = reinterpret_cast<const unsigned int*>(vi.m_variable->getName());
            const unsigned int step = sizeof(unsigned int);
            //std::cout << "name: \"" << vi.m_name << std::endl;
            for (index = 0; index < length / step; index++) {
                //std::cout << std::hex << *ptr32 << " ";
                bits ^= *ptr32;
                ptr32++;
            }
            //std::cout << std::endl << std::hex << bits << std::endl;
            const unsigned char * ptr8 = reinterpret_cast<const unsigned char *>(vi.m_variable->getName() + step * index);
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

    void getNamedVariable(const char * name, VariableIndex *index);

    std::vector<BasicVariable> m_basicVariables;

    std::vector<NonBasicVariable> m_nonBasicVariables;

    std::string m_info;

    HashTable<VariableIndex, VariableIndexHash> m_hashTable;

    void insertVariable(Simplex::VARIABLE_STATE state,
                        const VariableIndex * variableIndex,
                        const Numerical::Double * attached = 0);
};

#endif // BASISHEADBAS_H
