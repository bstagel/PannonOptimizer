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

#include <utils/nodefile.h>
#include <fstream>

NodeFile::NodeFile() {

}

void NodeFile::loadFromFile(const char * fileName) {
    std::ifstream inputFile(fileName);
    if (inputFile.is_open() == false) {
        throw FileNotFoundException("Error when opening node file!", fileName);
    }
    loadFromStream(inputFile);
}

void NodeFile::loadFromStream(std::istream & stream) {
    std::vector<std::string> rows;
    std::vector<Tokenizer::Token> tokens;

    Tokenizer::tokenize(stream, &tokens, &rows);
    m_document.build("", tokens.begin(), tokens.end(), rows);

}

const NodeFile::Node & NodeFile::getDocumentNode() const {
    return m_document;
}

const std::string & NodeFile::Node::getName() const {
    return m_name;
}

void NodeFile::Node::getNodes(const std::string & name, std::vector<Node>::const_iterator * beginIterator,
                              std::vector<Node>::const_iterator * endIterator) const {
    std::map<std::string, std::vector<Node> >::const_iterator nodeIter = m_nodes.find(name);
    if (nodeIter == m_nodes.end()) {
        throw PanOptException("Missing node: " + name);
    }
    *beginIterator = nodeIter->second.begin();
    *endIterator = nodeIter->second.end();
}

const std::string & NodeFile::Node::getValue(const std::string & name) const {
    std::map<std::string, std::string>::const_iterator iter = m_values.find(name);
    if (iter == m_values.end()) {
        throw PanOptException("Missing value: " + name);
    }
    return iter->second;
}

void NodeFile::Node::getArray(const std::string & name,
                              std::map<unsigned int, std::string>::const_iterator * beginIterator,
                              std::map<unsigned int, std::string>::const_iterator * endIterator) const {
    std::map<std::string, std::map<unsigned int, std::string> >::const_iterator arrayIter = m_arrays.find(name);
    if (arrayIter == m_arrays.end()) {
        throw PanOptException("Missing array: " + name);
    }
    *beginIterator = arrayIter->second.begin();
    *endIterator = arrayIter->second.end();
}

std::set<std::string> NodeFile::Node::getNodeNames() const
{
    std::set<std::string> result;
    auto iter = m_nodes.begin();
    auto iterEnd = m_nodes.end();

    for (; iter != iterEnd; ++iter) {
        result.insert( iter->first );
    }
    return result;
}

std::set<std::string> NodeFile::Node::getValueNames() const
{
    std::set<std::string> result;
    auto iter = m_values.begin();
    auto iterEnd = m_values.end();

    for (; iter != iterEnd; ++iter) {
        result.insert( iter->first );
    }
    return result;
}

bool NodeFile::Node::checkInteger(const std::string & str) {
    unsigned int index;
    if (str.length() == 0) {
        return false;
    }
    for (index = 0; index < str.length(); index++) {
        if (str[index] < '0' || str[index] > '9') {
            return false;
        }
    }
    return true;
}

bool NodeFile::Node::checkWord(const std::string & str) {
    unsigned int index;
    if (str.length() == 0) {
        return false;
    }
    for (index = 0; index < str.length(); index++) {
        if (str[index] >= '0' && str[index] <= '9') {
            if (index == 0) {
                return false;
            }
            continue;
        }
        if (str[index] >= 'a' && str[index] <= 'z') {
            continue;
        }
        if (str[index] >= 'A' && str[index] <= 'Z') {
            continue;
        }
        if (str[index] == '_') {
            continue;
        }
        return false;
    }
    return true;
}

void NodeFile::Node::build( const std::string & nodeName,
                            std::vector<Tokenizer::Token>::const_iterator beginToken,
                            std::vector<Tokenizer::Token>::const_iterator endToken,
                            const std::vector<std::string> & rows) {
    m_name = nodeName;
    /*
     * <Name> <{, [, or =
     * { new node
     * [ array
     * = value
     */
    std::string name;
    std::string nextToken;
    Tokenizer::Token prevToken;
    while (beginToken < endToken) {
        name = beginToken->m_value;
        if (checkWord(name) == false) {
            throw SyntaxErrorException("Syntax error: invalid name: " + name,
                                       rows[beginToken->m_row], beginToken->m_row,
                    beginToken->m_column);
        }
        prevToken = *beginToken;
        beginToken++;
        if (beginToken == endToken) {
            throw SyntaxErrorException("Syntax error: expected {, = or [",
                                       rows[prevToken.m_row], prevToken.m_row,
                    prevToken.m_column);
        }
        nextToken = beginToken->m_value;
        if (nextToken != "{" && nextToken != "[" && nextToken != "=") {
            throw SyntaxErrorException("Syntax error: expected {, [, or = after node name",
                                       rows[beginToken->m_row], beginToken->m_row,
                    beginToken->m_column);
        }
        if (nextToken == "{") {
            int counter = 1;
            std::vector<Tokenizer::Token>::const_iterator openToken = beginToken;
            prevToken = *beginToken;
            beginToken++;
            if (beginToken == endToken) {
                throw SyntaxErrorException("Syntax error: expected } or expression after }",
                                           rows[prevToken.m_row], prevToken.m_row,
                        prevToken.m_column);
            }

            if (beginToken->m_value != "}") {
                std::vector<Tokenizer::Token>::const_iterator start =
                        beginToken;
                std::vector<Tokenizer::Token>::const_iterator end =
                        beginToken;

                while (beginToken != endToken && counter > 0) {
                    if (beginToken->m_value == "{") {
                        counter++;
                    }
                    if (beginToken->m_value == "}") {
                        counter--;
                    }
                    end = beginToken;
                    beginToken++;
                }
                if (counter > 0) {
                    throw SyntaxErrorException("Syntax error: missing pair of }",
                                               rows[openToken->m_row], openToken->m_row,
                            openToken->m_column);
                }
                Node newNode;
                newNode.build(name, start, end, rows);
                m_nodes[name].push_back(newNode);
                beginToken = end;
            } else {
                Node newNode;
                newNode.build(name, endToken, endToken, rows);
                m_nodes[name].push_back(newNode);
            }
        } else if (nextToken == "[") {
            std::map<std::string, std::map<unsigned int, std::string> >::iterator
                    arrayIter = m_arrays.find(name);
            if (arrayIter == m_arrays.end()) {
                std::map<unsigned int, std::string> newArray;
                m_arrays.insert(std::make_pair(name, newArray));
                arrayIter = m_arrays.find(name);
            }
            prevToken = *beginToken;
            beginToken++;
            if (beginToken == endToken) {
                throw SyntaxErrorException("Syntax error: incomplete array",
                                           rows[prevToken.m_row], prevToken.m_row,
                        prevToken.m_column);
            }
            std::string number1Str = beginToken->m_value;
            if (checkInteger(number1Str) == false) {
                throw SyntaxErrorException("Syntax error: invalid positive integer: " + number1Str,
                                           rows[beginToken->m_row], beginToken->m_row,
                        beginToken->m_column);            }
            unsigned int startIndex = atoi( number1Str.c_str() );
            prevToken = *beginToken;
            beginToken++;
            if (beginToken == endToken) {
                throw SyntaxErrorException("Syntax error: incomplete array",
                                           rows[prevToken.m_row], prevToken.m_row,
                        prevToken.m_column);
            }
            if (beginToken->m_value != "]" && beginToken->m_value != "...") {
                throw SyntaxErrorException("Syntax error: expected ] or ... after [",
                                           rows[beginToken->m_row], beginToken->m_row,
                        beginToken->m_column);
            }
            if (beginToken->m_value == "]") {
                // next has to be =
                prevToken = *beginToken;
                beginToken++;
                if (beginToken == endToken) {
                    throw SyntaxErrorException("Syntax error: incomplete array",
                                               rows[prevToken.m_row], prevToken.m_row,
                            prevToken.m_column);
                }
                if (beginToken->m_value != "=") {
                    throw SyntaxErrorException("Syntax error: expected = after ]",
                                               rows[beginToken->m_row], beginToken->m_row,
                            beginToken->m_column);
                }
                prevToken = *beginToken;
                beginToken++;
                if (beginToken == endToken) {
                    throw SyntaxErrorException("Syntax error: incomplete array",
                                               rows[prevToken.m_row], prevToken.m_row,
                            prevToken.m_column);
                }
                arrayIter->second.insert(std::make_pair(startIndex, beginToken->m_value));
            } else if (beginToken->m_value == "...") {
                std::string number2Str;
                // next has to be a number
                prevToken = *beginToken;
                beginToken++;
                if (beginToken == endToken) {
                    throw SyntaxErrorException("Syntax error: incomplete array",
                                               rows[prevToken.m_row], prevToken.m_row,
                            prevToken.m_column);
                }
                number2Str = beginToken->m_value;
                if (checkInteger(number2Str) == false) {
                    throw SyntaxErrorException("Syntax error: invalid positive integer: " + number2Str,
                                               rows[beginToken->m_row], beginToken->m_row,
                            beginToken->m_column);
                }
                unsigned int index;
                unsigned int endIndex = atoi( number2Str.c_str() );
                if (endIndex < startIndex) {
                    throw SyntaxErrorException("Syntax error: second index is less than the first index: " + number2Str
                                               + " < " + number1Str,
                                               rows[beginToken->m_row], beginToken->m_row,
                            beginToken->m_column);
                }
                // next has to be ]
                prevToken = *beginToken;
                beginToken++;
                if (beginToken == endToken) {
                    throw SyntaxErrorException("Syntax error: incomplete array",
                                               rows[prevToken.m_row], prevToken.m_row,
                            prevToken.m_column);
                }
                if (beginToken->m_value != "]") {
                    throw SyntaxErrorException("Syntax error: expected ] after index",
                                               rows[beginToken->m_row], beginToken->m_row,
                            beginToken->m_column);
                }
                prevToken = *beginToken;
                beginToken++;
                if (beginToken == endToken) {
                    throw SyntaxErrorException("Syntax error: incomplete array",
                                               rows[prevToken.m_row], prevToken.m_row,
                            prevToken.m_column);
                }
                if (beginToken->m_value != "=") {
                    throw SyntaxErrorException("Syntax error: expected = after ]",
                                               rows[beginToken->m_row], beginToken->m_row,
                            beginToken->m_column);
                }
                beginToken++;
                for (index = startIndex; index <= endIndex; index++) {
                    prevToken = *beginToken;
                    if (beginToken == endToken) {
                        throw SyntaxErrorException("Syntax error: incomplete array",
                                                   rows[prevToken.m_row], prevToken.m_row,
                                prevToken.m_column);
                    }
                    if (beginToken->m_value == "{" ||
                            beginToken->m_value == "[" ||
                            beginToken->m_value == "=") {
                        throw SyntaxErrorException("Syntax error: incomplete array (an array cannot contain the symbols {, [ and =)",
                                                   rows[beginToken->m_row], beginToken->m_row,
                                beginToken->m_column);
                    }
                    arrayIter->second.insert(std::make_pair(index, beginToken->m_value));
                    if (index < endIndex) {
                        beginToken++;
                    }
                }
            }
        } else if (nextToken == "=") {
            prevToken = *beginToken;
            beginToken++;
            if (beginToken == endToken) {
                throw SyntaxErrorException("Syntax error: expected value after =",
                                           rows[prevToken.m_row], prevToken.m_row,
                        prevToken.m_column);
            }
            std::string value;
            value = beginToken->m_value;
            if (m_values.find(name) != m_values.end()) {
                throw SyntaxErrorException("Syntax error: value already exits (" + name + ")",
                                           rows[beginToken->m_row], beginToken->m_row,
                        beginToken->m_column);
            }
            m_values.insert(std::make_pair(name, value));
        }
        beginToken++;
    }
}
