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

#include <utils/parameterfilebuilder.h>

void ParameterFileBuilder::addNewParameter(const Parameter &param) {
    std::string name = param.getName();
    std::size_t dotIndex = name.find_first_of(".");
    if (dotIndex == std::string::npos) {
        m_values.push_back(param);
        return;
    }
    // block
    std::string blockName = name.substr(0, dotIndex);

    Parameter newParameter = param;
    std::string subName = name.substr(dotIndex + 1);

    newParameter.setName(subName);
    auto nodeIter = m_nodes.begin();
    auto nodeIterEnd = m_nodes.end();

    while (nodeIter != nodeIterEnd && nodeIter->m_name != blockName) {
        nodeIter++;
    }
    if (nodeIter == nodeIterEnd) {
        ParameterFileBuilder newNode;
        newNode.m_name = blockName;
        newNode.addNewParameter(newParameter);
        m_nodes.push_back(newNode);
    } else {

        nodeIter->addNewParameter(newParameter);
    }
    //}
}

void ParameterFileBuilder::writeToStream(std::ostream &os, unsigned int tabs) const {
    if (m_name.length() == 0 && m_comment.length() > 0) {
        writeComment(os, m_comment, tabs);
    }
    auto valueIter = m_values.begin();
    auto valueIterEnd = m_values.end();
    for (; valueIter != valueIterEnd; valueIter++) {
        // write comments
        std::string comment = valueIter->getComment();
        if (comment.length() > 0) {
            os << std::endl;
            writeComment(os, comment, tabs);
        }

        writeTabs(os, tabs);
        os << valueIter->getName() << " = ";
        Entry entry = valueIter->getEntry();
        Entry::ENTRY_TYPE type = valueIter->getEntryType();
        switch (type) {
        case Entry::BOOL:
            if (entry.m_bool == false) {
                os << "false";
            } else {
                os << "true";
            }
            break;
        case Entry::DOUBLE:
            os << entry.m_double;
            break;
        case Entry::INTEGER:
            os << entry.m_integer;
            break;
        case Entry::STRING:
            if (entry.m_string->find_first_of(".") == std::string::npos) {
                os << *entry.m_string;
            } else {
                os << '\"'<< *entry.m_string << '\"';
            }
            break;
        }
        os << std::endl;
    }
    auto nodeIter = m_nodes.begin();
    auto nodeIterEnd = m_nodes.end();
    for (; nodeIter != nodeIterEnd; nodeIter++) {

        os << std::endl;
        if (nodeIter->m_comment.length() > 0) {
            writeComment(os, nodeIter->m_comment, tabs);
        } else {
            os << std::endl;
        }
        writeTabs(os, tabs);
        os << nodeIter->m_name << " { " << std::endl;

        nodeIter->writeToStream(os, tabs+3);
        writeTabs(os, tabs);
        os << "}" << std::endl;

    }


}

void ParameterFileBuilder::addComment(const std::string &nodeName, const std::string &comment) {
    if (nodeName.length() == 0) {
        m_comment = comment;
    } else {
        std::size_t dotIndex = nodeName.find_first_of(".");
        if (dotIndex == std::string::npos) {
            for (auto & nodeIter: m_nodes) {
                if (nodeIter.m_name == nodeName) {
                    nodeIter.addComment("", comment);
                }
            }
        } else {
            std::string blockName = nodeName.substr(0, dotIndex);
            std::string subName = nodeName.substr(dotIndex + 1);
            for (auto & nodeIter: m_nodes) {
                if (nodeIter.m_name == blockName) {
                    nodeIter.addComment(subName, comment);
                }
            }
        }
    }
}

void ParameterFileBuilder::writeComment(std::ostream &os, const std::string &comment, unsigned int tabs) const {
    writeTabs(os, tabs);
    unsigned int index;
    for (index = 0; index < comment.length(); index++) {
        if (comment[index] == '\\' and index < comment.length() - 1
                && comment[index + 1] == '\n') {
            os << ' ';
        } else if (comment[index] == '\n') {
            os << '\n';
            writeTabs(os, tabs);
        } else if (comment[index] >= ' ') {
            os << comment[index];
        }
    }
    os << std::endl << std::endl;
}

void ParameterFileBuilder::writeTabs(std::ostream &os, unsigned int tabs) const {
    unsigned int index;
    for (index = 0; index < tabs; index++) {
        os << ' ';
    }
}
