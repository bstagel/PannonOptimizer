/**
 * @file basisheadbas.h This file contains the BasisHeadBAS class.
 */
#ifndef BASISHEADBAS_H
#define BASISHEADBAS_H

#include <globals.h>
#include <simplex/basisheadio.h>
#include <utils/hashtable.h>
#include <fstream>
#include <string>
#include <cstring>
#include <iostream>

/**
 * This class describes a BAS (standard) format of the basis.
 * With this we can write, and read the basis into / from a file.
 *
 * @class BasisHeadBAS
 */
class BasisHeadBAS: public BasisHeadIO {
public:

    /**
     * Default constructor of the class.
     * @constructor
     */
    BasisHeadBAS() {}

    /**
     * Default destructor of the class.
     * @destructor
     */
    ~BasisHeadBAS() {}

    /**
     * This function reopens the outputfile, and clears the members.
     * @param fileName name of the outputfile
     * @param model model of the LP problem
     */
    void startWrting(const char * fileName, const Model & model);

    /**
     * Initializes the m_info member with the given parameter.
     * @param info will be added as an std::string
     */
    void addInfo(const char * info);

    /**
     * With this function we can add a new basicvariable to the m_basicVariables vector.
     * @param variable pointer of the variable to be added
     * @param basisPosition the basis position on which the variable should be added
     * @param variableIndex parameter unused
     * @param value parameter unused
     */
    void addBasicVariable(const Variable * variable,
                          unsigned int basisPosition,
                          unsigned int variableIndex,
                          Numerical::Double value);

    /**
     * With this function we can add a new nonbasic variable to the m_nonBasicVariables vector.
     * @param variable pointer of the variable to be added
     * @param variableIndex parameter unused
     * @param state describes whether the nonbasic variable is on its lower or upper bound
     * @param value parameter unused
     */
    void addNonbasicVariable(const Variable *variable,
                             unsigned int variableIndex,
                             Simplex::VARIABLE_STATE state,
                             Numerical::Double value);

    /**
     * Writes the nonbasic variables into the output file, and closes it.
     */
    void finishWriting();

    /**
     * This function opens the input file, and initializes the hash.
     * @param fileName name of the input file
     * @param model model of the LP problem
     */
    void startReading(const char * fileName,
                      const Model & model);

    /**
     * Adds the variable to the hash.
     * @param variable variable to be added
     */
    void addVariable(const Variable & variable);

    /**
     * Reades the variables from the input file, and adds them to the right position. (Lower, upper bound, etc.)
     */
    void finishReading();

private:
    /**
     * The output file.
     */
    std::ofstream m_outputFile;

    /**
     * The input file.
     */
    std::ifstream m_inputFile;

    /**
     * This struct describes a basicvariable.
     * @struct BasicVariable
     */
    struct BasicVariable {
        const Variable * m_variable;
        unsigned int m_basisPosition;
    };

    /**
     * This struct describes a nonbasic variable.
     * @struct NonBasicVariable
     */
    struct NonBasicVariable {
        const Variable * m_variable;
        Simplex::VARIABLE_STATE m_state;
    };

    /**
     * Struct containing a variable, and its index.
     * @struct VariableIndex
     */
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

    /**
     * Describes the hash used to read and write data from / to a file.
     * @class VariableIndexHash
     */
    class VariableIndexHash {
    public:
        /**
         * This function performs an SHA-1 cryptographic algorithm.
         * @param vi is a variable and its index
         * @return the produced hash value.
         */
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

    /**
     * This creates a new Variable according to the name parameter, and sets it to the second parameter.
     * @param name name of the new variable
     * @param index container of the variable
     */
    void getNamedVariable(const char * name, VariableIndex *index);

    /**
     * Vector containing basicvariables.
     */
    std::vector<BasicVariable> m_basicVariables;

    /**
     * Vector storing nonbasic variables.
     */
    std::vector<NonBasicVariable> m_nonBasicVariables;

    /**
     * Stores additional information that can be written.
     */
    std::string m_info;

    /**
     * The hash table used to store the variables.
     */
    HashTable<VariableIndex, VariableIndexHash> m_hashTable;

    /**
     * This function inserts the variable to its right place considering the state.
     * @param state state of the variable
     * @param variableIndex variable and its index
     * @param attached optional parameter for the basicvariables: their values
     */
    void insertVariable(Simplex::VARIABLE_STATE state,
                        const VariableIndex * variableIndex,
                        const Numerical::Double * attached = 0);
};

#endif // BASISHEADBAS_H
