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
 * @file method.h
 */


#ifndef METHOD_H
#define	METHOD_H

#include <globals.h>

#include <lp/abstractmodel.h>

class Method
{
friend class AbstractModel;

protected:
    void registerMethodWithModel(Method* method, const AbstractModel & model){model.registerMethod(method);}
    void unregisterMethodWithModel(Method* method, const AbstractModel & model){
        model.unregisterMethod(method);
    }
    virtual void constraintAdded() {}
    virtual void variableAdded() {}

private:

};

#endif	/* METHOD_H */

