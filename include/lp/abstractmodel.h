/**
 * @file abstractmodel.h
 */


#ifndef ABSTRACTMODEL_H
#define	ABSTRACTMODEL_H

#include <globals.h>

#include <vector>

class Method;

class AbstractModel
{
friend class Method;

public:
    virtual ~AbstractModel() {}

protected:
    // TODO: ezt a ketto muveletet lehet mutex-el vedette kellene tenni
    void registerMethod(Method * method){
        m_registeredMethods.push_back(method);
    }

    void unregisterMethod(Method * method){
        std::vector<Method*>::iterator it = m_registeredMethods.begin();
        std::vector<Method*>::iterator end = m_registeredMethods.end();
        for(;it < end; it++){
            if(*it == method){
                m_registeredMethods.erase(it);
                break;
            }
        }

    }

private:
    std::vector<Method*> m_registeredMethods;

};

#endif	/* ABSTRACTMODEL_H */

