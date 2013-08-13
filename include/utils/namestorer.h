/**
 * @file namestorer.h
 */


#ifndef NAMESTORER_H
#define	NAMESTORER_H

#include "globals.h"

class NameStorer
{
protected:
    virtual void increase() = 0;
    virtual void decrease() = 0;

    virtual void copy(NameStorer *)
    {
    }
public:

    NameStorer()
    {
    }

    NameStorer(NameStorer & original)
    {
        copy(&original);
    }

    NameStorer & operator=(NameStorer & original)
    {
        decrease();
        copy(&original);
        return *this;
    }

    virtual ~NameStorer()
    {
        //decrease();
    }
    virtual NameStorer * getCopy() = 0;
};

#endif	/* NAMESTORER_H */

