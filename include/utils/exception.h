/* 
 * File:   exception.h
 * Author: smidla
 *
 * Created on 2013. february 28., 13:32
 */

#ifndef EXCEPTION_H
#define	EXCEPTION_H

#include <iostream>

class Exception
{
    std::string m_message;
public:
    Exception(const std::string & message): m_message(message) {
        
    }
    std::string getMessage() const {
        return m_message;
    }
};

#endif	/* EXCEPTION_H */

