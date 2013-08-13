/* 
 * File:   namestorer.h
 * Author: smidla
 *
 * Created on 2011. marcius 21., 22:22
 */

#ifndef NAMESTORER_H
#define	NAMESTORER_H

class NameStorer
{
protected:
    virtual void increase() = 0;
    virtual void decrease() = 0;

    virtual void copy(NameStorer * original)
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

