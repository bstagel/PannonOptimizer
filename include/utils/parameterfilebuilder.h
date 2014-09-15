/**
 *
 */
#ifndef PARAMETERFILEBUILDER_H
#define PARAMETERFILEBUILDER_H

#include <globals.h>
#include <string>
#include <vector>
#include <iostream>
#include <utils/parameter.h>

/**
 *
 */
class ParameterFileBuilder {
public:
    /**
     *
     * @param param
     */
    void addNewParameter(const Parameter & param);

    /**
     *
     * @param os
     * @param tabs
     */
    void writeToStream(std::ostream & os, unsigned int tabs = 0) const;

    /**
     *
     * @param nodeName
     * @param comment
     */
    void addComment(const std::string & nodeName,
                    const std::string & comment);

private:
    /**
     *
     */
    std::string m_name;

    /**
     *
     */
    std::vector<Parameter> m_values;

    /**
     *
     */
    std::vector<ParameterFileBuilder> m_nodes;

    /**
     *
     */
    std::string m_comment;

    /**
     *
     * @param os
     * @param comment
     * @param tabs
     */
    void writeComment(std::ostream & os,
                      const std::string & comment,
                      unsigned int tabs) const;

    /**
     *
     * @param os
     * @param tabs
     */
    void writeTabs(std::ostream & os, unsigned int tabs) const;
};

#endif // PARAMETERFILEBUILDER_H
