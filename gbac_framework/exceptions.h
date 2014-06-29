/**
 * @file exceptions.h
 */


#ifndef EXCEPTION_H
#define	EXCEPTION_H

#include <iostream>
#include <typeinfo>
#include <functional>
#include <exception>
#include <typeinfo>
#include <iostream>
#include <stdexcept>
#include <system_error>

class SyntaxErrorException{
public:
    SyntaxErrorException(const std::string & message,
                         const std::string & row,
                         unsigned int rowIndex,
                         unsigned int columnIndex):
        m_message(message),
        m_row(row),
        m_rowIndex(rowIndex),
        m_columnIndex(columnIndex) {

    }

    const std::string getRow() const {
        return m_row;
    }

    unsigned int getRowIndex() const {
        return m_rowIndex;
    }

    unsigned int getColumnIndex() const {
        return m_columnIndex;
    }

    void show() const {
        std::cout << m_rowIndex << ".: " << m_message << std::endl;
        std::cout << m_row << std::endl;
        std::cout << std::string(m_columnIndex, ' ') << "^" << std::endl;
    }
    inline std::string getMessage() const {return m_message;}

protected:
    std::string m_message;
    std::string m_row;

    unsigned int m_rowIndex;

    unsigned int m_columnIndex;
};

class ConsistencyException{

};

class SampleException{

};

#endif	/* EXCEPTION_H */

