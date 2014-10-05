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
 * With this we can write and read the basis information to / from a file.
 *
 * @class BasisHeadBAS
 */
class BasisHeadBAS: public BasisHeadIO {
public:

    /**
     * Default constructor of the class.
     *
     * @constructor
     */
    BasisHeadBAS() {}

    /**
     * Default destructor of the class.
     *
     * @destructor
     */
    ~BasisHeadBAS() {}

    /**
     * This function reopens the output file, and clears the members.
     *
     * @param fileName The name of the output file.
     * @param model The model of the LP problem.
     */
    void startWrting(const char * fileName, const Model & model);

    /**
     * Initializes the m_info member with the given parameter.
     *
     * @param The info to be added.
     */
    void addInfo(const char * info);

    /**
     * Adds a basic variable to the m_basicVariables vector.
     *
     * @param variable Pointer to the variable to be added.
     * @param basisPosition The basis position on which the variable will be added.
     * @param variableIndex The index of the variable to be added.
     * @param value The current value of the variable to be added.
     */
    void addBasicVariable(const Variable * variable,
                          unsigned int basisPosition,
                          unsigned int variableIndex,
                          Numerical::Double value);

    /**
     * Adds a nonbasic variable to the m_nonBasicVariables vector.
     *
     * @param variable Pointer to the variable to be added.
     * @param variableIndex The index of the variable to be added.
     * @param state Describes whether the nonbasic variable is on its lower or upper bound.
     * @param value The current value of the variable to be added.
     */
    void addNonbasicVariable(const Variable *variable,
                             unsigned int variableIndex,
                             Simplex::VARIABLE_STATE state,
                             Numerical::Double value);

    /**
     * Writes the nonbasic variables to the output file, and closes it.
     */
    void finishWriting();

    /**
     * This function opens the input file, and initializes the variables' hash table.
     *
     * @param fileName The name of the input file.
     * @param model The model of the LP problem.
     */
    void startReading(const char * fileName,
                      const Model & model);

    /**
     * Adds a specific variable to the hash table.
     *
     * @param variable The variable to be added.
     */
    void addVariable(const Variable & variable);

    /**
     * Reads the variables from the input file, and adds them to the right position. (Lower, upper bound, etc.)
     */
    void finishReading();

private:
    /**
     * The output file stream.
     */
    std::ofstream m_outputFile;

    /**
     * The input file stream.
     */
    std::ifstream m_inputFile;

    /**
     * This struct describes a basic variable.
     *
     * @struct BasicVariable
     */
    struct BasicVariable {
        const Variable * m_variable;
        unsigned int m_basisPosition;
    };

    /**
     * This struct describes a nonbasic variable.
     *
     * @struct NonBasicVariable
     */
    struct NonBasicVariable {
        const Variable * m_variable;
        Simplex::VARIABLE_STATE m_state;
    };

    /**
     * Struct containing a variable with its index.
     *
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
     *
     * @class VariableIndexHash
     */
    class VariableIndexHash {
    public:
        /**
         * This function performs an SHA-1 cryptographic algorithm.
         *
         * @param vi The variable with its index to be hashed.
         * @return The produced hash value.
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
     * Sets a new Variable to a given variable index struct.
     *
     * @param name Name of the new variable.
     * @param index Container of the variable.
     */
    void getNamedVariable(const char * name, VariableIndex *index);

    /**
     * Vector containing the basic variables.
     */
    std::vector<BasicVariable> m_basicVariables;

    /**
     * Vector containing the nonbasic variables.
     */
    std::vector<NonBasicVariable> m_nonBasicVariables;

    /**
     * Additional information to be stored.
     */
    std::string m_info;

    /**
     * The hash table used to store the variables.
     */
    HashTable<VariableIndex, VariableIndexHash> m_hashTable;

    /**
     * This function inserts the variable to its right place considering the state.
     *
     * @param state The state of the variable.
     * @param variableIndex The variable with its index.
     * @param attached The value of the variable if it is basic.
     */
    void insertVariable(Simplex::VARIABLE_STATE state,
                        const VariableIndex * variableIndex,
                        const Numerical::Double * attached = 0);
};

#endif // BASISHEADBAS_H
