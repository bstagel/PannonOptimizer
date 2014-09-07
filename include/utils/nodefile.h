/**
 * @file nodefile.h This file contains the API of the NodeFile class.
 */

#ifndef NODEFILE_H
#define NODEFILE_H

#include <globals.h>
#include <utils/tokenizer.h>
#include <utils/exceptions.h>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

/**
 * This class is able to process tokenized PanOpt basis head files.
 *
 * @class NodeFile
 */
class NodeFile {
public:

    /**
     * This class represents a single node.
     *
     * @class Node
     */
    class Node {
    public:

        /**
         * Returns the name of the Node.
         *
         * @return The name of the node.
         */
        const std::string & getName() const;

        /**
         * Returns iterators to visit the nodes with the given name.
         *
         * @param name The name of the nodes to be queried.
         * @param beginIterator The iterator pointing to the first element of the list of nodes.
         * @param endIterator The iterator pointing to the last element of the list of nodes.
         */
        void getNodes(const std::string & name, std::vector<Node>::const_iterator * beginIterator,
                      std::vector<Node>::const_iterator * endIterator) const;

        /**
         * Returns the value stored at the given name.
         *
         * @throws PanOptException if the value is not found.
         * @param name The name the value to be found.
         * @return The value.
         */
        const std::string & getValue(const std::string & name) const;

        /**
         * Returns the array stored at the given name.
         *
         * @throws PanOptException if the array is not found.
         * @param name The name the array to be found.
         * @return The array.
         */
        void getArray(const std::string & name, std::map<unsigned int, std::string>::const_iterator * beginIterator,
                      std::map<unsigned int, std::string>::const_iterator * endIterator) const;

        /**
         * Returns the set contains the node names.
         *
         * @return The set.
         */
        std::set<std::string> getNodeNames() const;

        /**
         * Returns the set contains the value names.
         *
         * @return The set.
         */
        std::set<std::string> getValueNames() const;

        /**
         * Builds the node from the tokenized file.
         *
         * @see Tokenizer
         * @throws SyntaxErrorException if there is a syntax error in the file.
         * @param nodeName The name of the node to build.
         * @param beginToken The iterator pointing to the first token of the file.
         * @param endToken The iterator pointing to the last token of the file.
         * @param rows The rows of the file.
         */
        void build( const std::string & nodeName,
                    std::vector<Tokenizer::Token>::const_iterator beginToken,
                    std::vector<Tokenizer::Token>::const_iterator endToken,
                    const std::vector<std::string> & rows);

        /**
         * Checks whether the input string is an integer.
         *
         * @param str The string to be checked.
         * @return True if the string in an integer.
         */
        static bool checkInteger(const std::string & str);

        /**
         * Checks whether the input string is a word.
         *
         * @param str The string to be checked.
         * @return True if the string in an word.
         */
        static bool checkWord(const std::string & str);

    private:

        /**
         * The name of the node.
         */
        std::string m_name;

        /**
         * The vector of nodes with the same name.
         */
        std::map<std::string, std::vector<Node> > m_nodes; // Nodes with the same name

        /**
         * The values corresponding to this node.
         */
        std::map<std::string, std::string> m_values; // values are unique

        /**
         * The arrays corresponding to this node.
         */
        std::map<std::string, std::map<unsigned int, std::string> > m_arrays; // arrays are unique
    };

    /**
     * Default constructor of NodeFile.
     *
     * @constructor
     */
    NodeFile();

    /**
     * Loads the node to the actual NodeFile object from a file.
     *
     * @param fileName The name of the file.
     */
    void loadFromFile(const char * fileName);

    /**
     * Loads the node to the actual NodeFile object from an input stream.
     *
     * @param stream
     */
    void loadFromStream(std::istream & stream);

    /**
     * Returns the node loaded to the actual NodeFile object.
     *
     * @return The node loaded to the actual NodeFile object.
     */
    const Node & getDocumentNode() const;

private:

    /**
     * The node loaded to the actual NodeFile object.
     */
    Node m_document;
};

#endif // NODEFILE_H
