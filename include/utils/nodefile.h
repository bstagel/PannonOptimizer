/**
 * @file nodefile.h This file contains the API of the NodeFile class.
 * @author
 */

#ifndef NODEFILE_H
#define NODEFILE_H

#include <globals.h>
#include <utils/tokenizer.h>
#include <utils/exceptions.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>

/**
 * @brief The NodeFile class
 */
class NodeFile {
public:
    class Node {
    public:

        const std::string & getName() const;

        void getNodes(const std::string & name, std::vector<Node>::const_iterator * beginIterator,
                      std::vector<Node>::const_iterator * endIterator) const;

        const std::string & getValue(const std::string & name) const;

        void getArray(const std::string & name, std::map<unsigned int, std::string>::const_iterator * beginIterator,
                      std::map<unsigned int, std::string>::const_iterator * endIterator) const;

        void build( const std::string & nodeName,
                    std::vector<Tokenizer::Token>::const_iterator beginToken,
                    std::vector<Tokenizer::Token>::const_iterator endToken,
                    const std::vector<std::string> & rows);

        static bool checkInteger(const std::string & str);

        static bool checkWord(const std::string & str);

    private:
        std::string m_name;

        std::map<std::string, std::vector<Node> > m_nodes; // Nodes with the same name

        std::map<std::string, std::string> m_values; // values are unique

        std::map<std::string, std::map<unsigned int, std::string> > m_arrays; // arrays are unique
    };

    NodeFile();

    void loadFromFile(const char * fileName);

    void loadFromStream(std::istream & steram);

    const Node & getDocumentNode() const;

private:
    Node m_document;
};

#endif // NODEFILE_H
