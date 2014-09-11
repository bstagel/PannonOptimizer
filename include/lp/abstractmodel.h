/**
 * @file abstractmodel.h
 */


#ifndef ABSTRACTMODEL_H
#define	ABSTRACTMODEL_H

#include <globals.h>
#include <string>
#include <vector>
#include <mutex>
#include <debug.h>

class Method;

class AbstractModel
{
    friend class Method;

public:
    AbstractModel() {
      //  sm_registeredMethods = new std::vector<Method*>;
    }

    virtual ~AbstractModel() {}

    virtual std::string getHash() {
        return "";
    }

protected:
    static std::mutex sm_mutex;

    // TODO: ezt a ketto muveletet lehet mutex-el vedette kellene tenni
    void registerMethod(Method * method) const {
        sm_mutex.lock();
        sm_registeredMethods.push_back(method);
        sm_mutex.unlock();
    }

    void unregisterMethod(Method * method) const {
        sm_mutex.lock();
        LPINFO("vector: " << (&sm_registeredMethods));
        //sm_mutex.unlock();
        //return;
        std::vector<Method*>::iterator it = sm_registeredMethods.begin();
        std::vector<Method*>::iterator end = sm_registeredMethods.end();
        for(;it < end; it++){
            if(*it == method){
                sm_registeredMethods.erase(it);
                break;
            }
        }
        sm_mutex.unlock();
    }

private:
    mutable std::vector<Method*> sm_registeredMethods;

};

#endif	/* ABSTRACTMODEL_H */

