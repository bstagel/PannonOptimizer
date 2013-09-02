/**
 * @file method.h
 */


#ifndef METHOD_H
#define	METHOD_H

#include "globals.h"

#include "lp/method.h"
#include "lp/abstractmodel.h"

class Method
{
friend class AbstractModel;

protected:
    void registerMethodWithModel(Method* method, AbstractModel model){model.registerMethod(method);}
    void unregisterMethodWithModel(Method* method, AbstractModel model){model.unregisterMethod(method);}
    virtual void constraintAdded() {}
    virtual void variableAdded() {}

private:

};

#endif	/* METHOD_H */

