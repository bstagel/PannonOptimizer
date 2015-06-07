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
