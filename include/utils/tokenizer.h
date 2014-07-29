/**
 * @file tokenizer.h This file contains the API of the Tokenizer class.
 * @author
 */

#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <globals.h>
#include <utils/exceptions.h>
#include <iostream>
#include <vector>
#include <string>
#include <debug.h>

/**
 * This class is able to divide an input file into tokens.
 * With tokens the organizing and debugging of input data is easier.
 * This class functions as a DFA with states describing the actual sequence type in
 * the input file.
 *
 * @class Tokenizer
 */
class Tokenizer {
public:

    /**
     * This struct describes a token of the tokenized file.
     */
    struct Token {
        std::string m_value;
        unsigned int m_row;
        unsigned int m_column;
    };

    /**
     * Tokenizes a specified input stream.
     *
     * @param is The input stream to be tokenized.
     * @param tokens Pointer to the output token array.
     * @param rows Pointer to the output row array.
     */
    static void tokenize(std::istream & is, std::vector< Token > * tokens,
                         std::vector<std::string> * rows);
private:

    /**
     * This type describes the state of the Tokenizer object.
     * The state decides how to handle the next input characters.
     */
    enum TOKENIZER_STATE {
        TOKENIZER_START,
        TOKENIZER_COMMENT,
        TOKENIZER_WORD,
        TOKENIZER_NUMBER,
        TOKENIZER_NUMBER_WIDTH_DOT,
       // TOKENIZER_SPECIAL,
        TOKENIZER_STRING,
        TOKENIZER_ESCAPE_SEQ,
        TOKENIZER_WHITESPACE,
        TOKENIZER_ELLIPSIS_OR_NUMBER,
        TOKENIZER_ELLIPSIS_2,
      //  TOKENIZER_ELLIPSIS_3,
        TOKENIZER_END,
        TOKENIZER_ERROR,
        TOKENIZER_STATE_COUNT
    };

    static void initStateTable(std::vector< std::vector<TOKENIZER_STATE> > * tablePtr,
                               std::vector<std::vector<std::string> > * transitionTablePtr);

    static std::string stateToString(TOKENIZER_STATE state);
};

#endif // TOKENIZER_H
