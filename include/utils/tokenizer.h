#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <globals.h>
#include <utils/exceptions.h>
#include <iostream>
#include <vector>
#include <string>
#include <debug.h>

class Tokenizer {
public:
    struct Token {
        std::string m_value;
        unsigned int m_row;
        unsigned int m_column;
    };

    static void tokenize(std::istream & is, std::vector< Token > * tokens,
                         std::vector<std::string> * rows);
private:
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
