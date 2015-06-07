//=================================================================================================
/*!
//  This file is part of the Pannon Optimizer library. 
//  This library is free software; you can redistribute it and/or modify it under the 
//  terms of the GNU Lesser General Public License as published by the Free Software 
//  Foundation; either version 3.0, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
//  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License; see the file 
//  COPYING. If not, see http://www.gnu.org/licenses/.
*/
//=================================================================================================

#include <utils/tokenizer.h>
#include <debug.h>

/*
 * Words: starts with letter, continues with letters, numbers, or _
 * Numbers: contains numbers (0...9), letter 'e', 'E', '+', '-', or '.'
 *      starts with numbers, '+', '-', or '.'
 * Special tokens: [, ], {, }, =, ... and ,
 * Strings: bordered by "
 * White space: space and every character code below space
 */

void Tokenizer::initStateTable(std::vector< std::vector<TOKENIZER_STATE> > * tablePtr,
                               std::vector<std::vector<std::string> > * transitionTablePtr) {
    unsigned int state;
    unsigned int index;
    tablePtr->clear();
    tablePtr->resize(TOKENIZER_STATE_COUNT);

    transitionTablePtr->clear();
    transitionTablePtr->resize(TOKENIZER_STATE_COUNT);
    for (state = 0; state < TOKENIZER_STATE_COUNT; state++) {
        (*transitionTablePtr)[state].resize(TOKENIZER_STATE_COUNT);
    }

    for (state = 0; state < TOKENIZER_STATE_COUNT; state++) {
        (*tablePtr)[state].resize(256, TOKENIZER_ERROR);

        switch (state) {
        case TOKENIZER_START:
            for (index = '\0'; index <= ' '; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WHITESPACE;
            }
            (*tablePtr)[state]['\"'] = TOKENIZER_STRING;
            (*transitionTablePtr)[state][TOKENIZER_STRING] = "c";
            (*tablePtr)[state]['#'] = TOKENIZER_COMMENT;
            (*tablePtr)[state]['+'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['-'] = TOKENIZER_NUMBER;
            for (index = '0'; index <= '9'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_NUMBER;
            }
            (*transitionTablePtr)[state][TOKENIZER_NUMBER] = "c";
            (*tablePtr)[state]['.'] = TOKENIZER_ELLIPSIS_OR_NUMBER;
            (*transitionTablePtr)[state][TOKENIZER_ELLIPSIS_OR_NUMBER] = "c";
            (*tablePtr)[state]['['] = TOKENIZER_START;
            (*tablePtr)[state][']'] = TOKENIZER_START;
            (*tablePtr)[state]['{'] = TOKENIZER_START;
            (*tablePtr)[state]['}'] = TOKENIZER_START;
            (*tablePtr)[state]['='] = TOKENIZER_START;
            (*tablePtr)[state][','] = TOKENIZER_START;
            (*transitionTablePtr)[state][TOKENIZER_START] = "cad";

            (*tablePtr)[state]['$'] = TOKENIZER_WORD;
            for (index = 'a'; index <= 'z'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            for (index = 'A'; index <= 'Z'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            (*tablePtr)[state]['_'] = TOKENIZER_WORD;
            (*transitionTablePtr)[state][TOKENIZER_WORD] = "c";

            (*tablePtr)[state][0] = TOKENIZER_END;
            break;
        case TOKENIZER_COMMENT:
            for (index = '\0'; index <= 255; index++) {
                (*tablePtr)[state][index] = TOKENIZER_COMMENT;
            }
            (*tablePtr)[state][0] = TOKENIZER_END;
            break;
        case TOKENIZER_ELLIPSIS_OR_NUMBER:
            // . is a valid token, means 0.0
            (*tablePtr)[state]['#'] = TOKENIZER_COMMENT;
            (*transitionTablePtr)[state][TOKENIZER_COMMENT] = "ad";
            (*tablePtr)[state]['\"'] = TOKENIZER_STRING;
            (*transitionTablePtr)[state][TOKENIZER_STRING] = "adc";
            (*tablePtr)[state]['+'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['-'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['e'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['E'] = TOKENIZER_NUMBER;

            for (index = '0'; index <= '9'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_NUMBER;
            }
            (*transitionTablePtr)[state][TOKENIZER_NUMBER] = "c";

            for (index = '\0'; index <= ' '; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WHITESPACE;
                // . means 0.0
            }
            (*transitionTablePtr)[state][TOKENIZER_WHITESPACE] = "ad";
            (*tablePtr)[state]['$'] = TOKENIZER_WORD;
            for (index = 'a'; index <= 'z'; index++) {
                if (index == 'e') {
                    continue;
                }
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            for (index = 'A'; index <= 'Z'; index++) {
                if (index == 'E') {
                    continue;
                }
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            (*tablePtr)[state]['_'] = TOKENIZER_WORD;
            (*transitionTablePtr)[state][TOKENIZER_WORD] = "adc";

            (*tablePtr)[state]['.'] = TOKENIZER_ELLIPSIS_2;
            (*transitionTablePtr)[state][TOKENIZER_ELLIPSIS_2] = "c";
            (*tablePtr)[state]['['] = TOKENIZER_START;
            (*tablePtr)[state][']'] = TOKENIZER_START;
            (*tablePtr)[state]['{'] = TOKENIZER_START;
            (*tablePtr)[state]['}'] = TOKENIZER_START;
            (*tablePtr)[state]['='] = TOKENIZER_START;
            (*tablePtr)[state][','] = TOKENIZER_START;
            (*transitionTablePtr)[state][TOKENIZER_COMMENT] = "adcad";
            (*tablePtr)[state][0] = TOKENIZER_END;
            break;
        case TOKENIZER_ELLIPSIS_2:
            (*tablePtr)[state]['.'] = TOKENIZER_START;
            (*transitionTablePtr)[state][TOKENIZER_START] = "cad";
            (*tablePtr)[state][0] = TOKENIZER_ERROR;
            break;
        case TOKENIZER_NUMBER:

            (*tablePtr)[state]['#'] = TOKENIZER_COMMENT;
            (*transitionTablePtr)[state][TOKENIZER_COMMENT] = "ad";
            (*tablePtr)[state]['\"'] = TOKENIZER_STRING;
            (*transitionTablePtr)[state][TOKENIZER_STRING] = "adc";
            (*tablePtr)[state]['+'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['-'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['e'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['E'] = TOKENIZER_NUMBER;

            for (index = '0'; index <= '9'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_NUMBER;
            }
            (*transitionTablePtr)[state][TOKENIZER_NUMBER] = "c";

            for (index = '\0'; index <= ' '; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WHITESPACE;
                // . means 0.0
            }
            (*transitionTablePtr)[state][TOKENIZER_WHITESPACE] = "ad";
            (*tablePtr)[state]['$'] = TOKENIZER_WORD;
            for (index = 'a'; index <= 'z'; index++) {
                if (index == 'e') {
                    continue;
                }
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            for (index = 'A'; index <= 'Z'; index++) {
                if (index == 'E') {
                    continue;
                }
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            (*tablePtr)[state]['_'] = TOKENIZER_WORD;
            (*transitionTablePtr)[state][TOKENIZER_WORD] = "adc";

            (*tablePtr)[state]['.'] = TOKENIZER_NUMBER_WIDTH_DOT;
            (*transitionTablePtr)[state][TOKENIZER_NUMBER_WIDTH_DOT] = "c";

            (*tablePtr)[state]['['] = TOKENIZER_START;
            (*tablePtr)[state][']'] = TOKENIZER_START;
            (*tablePtr)[state]['{'] = TOKENIZER_START;
            (*tablePtr)[state]['}'] = TOKENIZER_START;
            (*tablePtr)[state]['='] = TOKENIZER_START;
            (*tablePtr)[state][','] = TOKENIZER_START;
            (*transitionTablePtr)[state][TOKENIZER_START] = "adcad";
            (*tablePtr)[state][0] = TOKENIZER_END;
            break;
        case TOKENIZER_NUMBER_WIDTH_DOT:
            (*tablePtr)[state]['.'] = TOKENIZER_ELLIPSIS_2;
            (*transitionTablePtr)[state][TOKENIZER_ELLIPSIS_2] = "s1c";
            (*tablePtr)[state]['#'] = TOKENIZER_COMMENT;
            (*transitionTablePtr)[state][TOKENIZER_COMMENT] = "ad";
            (*tablePtr)[state]['\"'] = TOKENIZER_STRING;
            (*transitionTablePtr)[state][TOKENIZER_STRING] = "ad";
            (*tablePtr)[state]['+'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['-'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['e'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['E'] = TOKENIZER_NUMBER;

            for (index = '0'; index <= '9'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_NUMBER;
            }
            (*transitionTablePtr)[state][TOKENIZER_NUMBER] = "c";

            for (index = '\0'; index <= ' '; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WHITESPACE;
                // . means 0.0
            }
            (*transitionTablePtr)[state][TOKENIZER_WHITESPACE] = "ad";

            for (index = 'a'; index <= 'z'; index++) {
                if (index == 'e') {
                    continue;
                }
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            (*tablePtr)[state]['$'] = TOKENIZER_WORD;
            for (index = 'A'; index <= 'Z'; index++) {
                if (index == 'E') {
                    continue;
                }
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            (*tablePtr)[state]['_'] = TOKENIZER_WORD;
            (*transitionTablePtr)[state][TOKENIZER_WORD] = "adc";

            (*tablePtr)[state]['['] = TOKENIZER_START;
            (*tablePtr)[state][']'] = TOKENIZER_START;
            (*tablePtr)[state]['{'] = TOKENIZER_START;
            (*tablePtr)[state]['}'] = TOKENIZER_START;
            (*tablePtr)[state]['='] = TOKENIZER_START;
            (*tablePtr)[state][','] = TOKENIZER_START;
            (*transitionTablePtr)[state][TOKENIZER_START] = "adcad";
            (*tablePtr)[state][0] = TOKENIZER_END;
            break;
        case TOKENIZER_WORD:
            for (index = '\0'; index <= ' '; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WHITESPACE;
            }
            (*transitionTablePtr)[state][TOKENIZER_WHITESPACE] = "ad";
            (*transitionTablePtr)[state][TOKENIZER_COMMENT] = "ad";
            (*tablePtr)[state]['$'] = TOKENIZER_WORD;
            for (index = 'a'; index <= 'z'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            for (index = 'A'; index <= 'Z'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            for (index = '0'; index <= '9'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            (*tablePtr)[state]['_'] = TOKENIZER_WORD;
            (*transitionTablePtr)[state][TOKENIZER_WORD] = "c";

            (*tablePtr)[state]['#'] = TOKENIZER_COMMENT;
            (*transitionTablePtr)[state][TOKENIZER_COMMENT] = "ad";
            (*tablePtr)[state]['\"'] = TOKENIZER_STRING;
            (*transitionTablePtr)[state][TOKENIZER_STRING] = "adc";
            (*tablePtr)[state]['+'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['-'] = TOKENIZER_NUMBER;
            (*transitionTablePtr)[state][TOKENIZER_NUMBER] = "adc";
            (*tablePtr)[state]['.'] = TOKENIZER_ELLIPSIS_OR_NUMBER;
            (*transitionTablePtr)[state][TOKENIZER_ELLIPSIS_OR_NUMBER] = "adc";
            (*tablePtr)[state]['['] = TOKENIZER_START;
            (*tablePtr)[state][']'] = TOKENIZER_START;
            (*tablePtr)[state]['{'] = TOKENIZER_START;
            (*tablePtr)[state]['}'] = TOKENIZER_START;
            (*tablePtr)[state]['='] = TOKENIZER_START;
            (*tablePtr)[state][','] = TOKENIZER_START;
            (*transitionTablePtr)[state][TOKENIZER_START] = "adcad";
            (*tablePtr)[state][0] = TOKENIZER_END;
            break;
        case TOKENIZER_STRING:
            for (index = ' '; index < 128; index++) {
                (*tablePtr)[state][index] = TOKENIZER_STRING;
            }
            (*transitionTablePtr)[state][TOKENIZER_STRING] = "c";
            (*tablePtr)[state]['\"'] = TOKENIZER_START;
            (*transitionTablePtr)[state][TOKENIZER_START] = "cad";
            (*tablePtr)[state]['\\'] = TOKENIZER_ESCAPE_SEQ;
            (*tablePtr)[state][0] = TOKENIZER_ERROR;
            break;
        case TOKENIZER_ESCAPE_SEQ:
            (*tablePtr)[state]['\"'] = TOKENIZER_STRING;
            (*tablePtr)[state]['\\'] = TOKENIZER_STRING;
            (*transitionTablePtr)[state][TOKENIZER_STRING] = "c";
            (*tablePtr)[state][0] = TOKENIZER_ERROR;
            break;
        case TOKENIZER_WHITESPACE:
            (*tablePtr)[state]['\"'] = TOKENIZER_STRING;
            (*transitionTablePtr)[state][TOKENIZER_STRING] = "c";
            (*tablePtr)[state]['#'] = TOKENIZER_COMMENT;
            (*tablePtr)[state]['+'] = TOKENIZER_NUMBER;
            (*tablePtr)[state]['-'] = TOKENIZER_NUMBER;
            (*transitionTablePtr)[state][TOKENIZER_NUMBER] = "c";
            for (index = '\0'; index <= ' '; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WHITESPACE;
            }

            for (index = '0'; index <= '9'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_NUMBER;
            }

            (*tablePtr)[state]['.'] = TOKENIZER_ELLIPSIS_OR_NUMBER;
            (*transitionTablePtr)[state][TOKENIZER_ELLIPSIS_OR_NUMBER] = "c";
            (*tablePtr)[state]['['] = TOKENIZER_START;
            (*tablePtr)[state][']'] = TOKENIZER_START;
            (*tablePtr)[state]['{'] = TOKENIZER_START;
            (*tablePtr)[state]['}'] = TOKENIZER_START;
            (*tablePtr)[state]['='] = TOKENIZER_START;
            (*tablePtr)[state][','] = TOKENIZER_START;
            (*transitionTablePtr)[state][TOKENIZER_START] = "cad";
            (*tablePtr)[state]['$'] = TOKENIZER_WORD;
            for (index = 'a'; index <= 'z'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            for (index = 'A'; index <= 'Z'; index++) {
                (*tablePtr)[state][index] = TOKENIZER_WORD;
            }
            (*tablePtr)[state]['_'] = TOKENIZER_WORD;
            (*transitionTablePtr)[state][TOKENIZER_WORD] = "c";
            (*tablePtr)[state][0] = TOKENIZER_END;
            break;
        case TOKENIZER_END:
            break;
        }
    }
}

std::string Tokenizer::stateToString(TOKENIZER_STATE state) {
    switch (state) {
    case TOKENIZER_START:
        return "TOKENIZER_START";
    case TOKENIZER_COMMENT:
        return "TOKENIZER_COMMENT";
    case TOKENIZER_WORD:
        return "TOKENIZER_WORD";
    case TOKENIZER_NUMBER:
        return "TOKENIZER_NUMBER";
    case TOKENIZER_NUMBER_WIDTH_DOT:
        return "TOKENIZER_NUMBER_WIDTH_DOT";
    case TOKENIZER_STRING:
        return "TOKENIZER_STRING";
    case TOKENIZER_ESCAPE_SEQ:
        return "TOKENIZER_ESCAPE_SEQ";
    case TOKENIZER_WHITESPACE:
        return "TOKENIZER_WHITESPACE";
    case TOKENIZER_ELLIPSIS_OR_NUMBER:
        return "TOKENIZER_ELLIPSIS_OR_NUMBER";
    case TOKENIZER_ELLIPSIS_2:
        return "TOKENIZER_ELLIPSIS_2";
    case TOKENIZER_ERROR:
        return "TOKENIZER_ERROR";
    case TOKENIZER_STATE_COUNT:
        break;
    case TOKENIZER_END:
        return "TOKENIZER_END";
    }
    return "";
}

void Tokenizer::tokenize(std::istream & is, std::vector<Token> * tokens,
                         std::vector<std::string> * rows) {
    tokens->clear();
    std::string rawRow;
    std::string row;
    char character;
    std::vector<std::vector<TOKENIZER_STATE> > stateTable;
    /*
     * Transition: c: concat a new character
     *             d: delete the current token
     *             a: add the token to the list
     *             sXXX: split and store the first part, XXX is the length of the
     *                  second part
     */
    std::vector<std::vector<std::string> > transitionTable;
    initStateTable(&stateTable, &transitionTable);
    unsigned int rowIndex = 0;
    do {
        Token token;
        TOKENIZER_STATE state = TOKENIZER_START;
        std::getline(is, rawRow);
        unsigned int characterIndex = 0;
        row = "";
        for (characterIndex = 0; characterIndex < rawRow.length(); characterIndex++) {
            if (rawRow[characterIndex] >= ' ') {
                row = row + rawRow[characterIndex];
            } else if (rawRow[characterIndex] == '\t') {
                row = row + "    ";
            }
        }
        if (rows != 0) {
            rows->push_back(row);
        }

        characterIndex = 0;
        while (characterIndex <= row.length() && state != TOKENIZER_COMMENT) {
            if (characterIndex < row.length()) {
                character = row[characterIndex];
            } else {
                character = 0;
            }

            TOKENIZER_STATE newState = stateTable[state][character];
            if (newState == TOKENIZER_ERROR) {
                throw SyntaxErrorException("Syntax error: invalid character in the token",
                                           row, rowIndex, characterIndex);
            }
            std::string program = transitionTable[state][newState];
            unsigned int instructionIndex = 0;
            while (instructionIndex < program.length()) {
                unsigned int splitLength;
                int firstLength;
                char code = program[instructionIndex];
                Token firstPart;
                switch (code) {
                case 'c':
                    token.m_value = token.m_value + character;
                    if (token.m_value.length() == 1) {
                        token.m_column = characterIndex;
                        token.m_row = rowIndex;
                    }
                    break;
                case 'd':
                    token.m_value = "";
                    break;
                case 'a':
                    tokens->push_back(token);
                    break;
                case 's':
                    splitLength = 0;
                    instructionIndex++;
                    while (instructionIndex < program.length() &&
                           program[instructionIndex] >= '0' &&
                           program[instructionIndex] <= '9') {
                        splitLength *= 10;
                        splitLength += program[instructionIndex] - '0';
                        instructionIndex++;
                    }

                    instructionIndex--;
                    firstLength = token.m_value.length() - (int)splitLength;
                    if (firstLength < 0) {
                        firstLength = 0;
                    }
                    firstPart = token;
                    firstPart.m_value = token.m_value.substr(0, firstLength);
                    tokens->push_back(firstPart);
                    token.m_value = token.m_value.substr( firstLength );
                    token.m_column = characterIndex;
                }
                instructionIndex++;
            }
            state = newState;
            characterIndex++;
        }
        // TODO: csak akkor kell beszurni, ha olyan allapotban vagyunk
        if (token.m_value.length() > 0) {
            tokens->push_back(token);
        }
        rowIndex++;
    } while (!is.eof());

}
