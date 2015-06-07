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

    virtual std::string getHash() const {
        return "";
    }

protected:
    static std::mutex sm_mutex;

    void registerMethod(Method * method) const {
        sm_mutex.lock();
        sm_registeredMethods.push_back(method);
        sm_mutex.unlock();
    }

    void unregisterMethod(Method * method) const {
        sm_mutex.lock();
//        LPINFO("vector: " << (&sm_registeredMethods));
        //sm_mutex.unlock();
        //return;
        std::vector<Method*>::iterator it = sm_registeredMethods.begin();
        std::vector<Method*>::iterator end = sm_registeredMethods.end();
        for(;it < end; ++it){
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

