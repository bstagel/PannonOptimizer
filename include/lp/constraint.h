/* 
 * File:   constraint.h
 * Author: smidla
 *
 * Created on 2011. marcius 22., 11:09
 */

#ifndef CONSTRAINT_H
#define	CONSTRAINT_H

#include <utils/numerical.h>
#include <linalg/vector.h>
#include <utils/exception.h>

class Constraint
{
    friend class ConstraintTestSuite;
    friend class Model;
public:

    /**
     * 
     */
    enum TYPE
    {
        GREATER_OR_EQUAL, /* The constraint has a finite lower bound */
        LESS_OR_EQUAL, /* The constraint has a finite upper bound */
        RANGE, /* The constraint has finite bounds */
        EQUALITY, /* The finite bounds are the same */
        NON_BINDING /* The constraint has no bounds */
    };

    class ConstraintException : public Exception
    {
    public:
        /**
         * Constructor of the ConstraintException. It gets the original wrong
         * constraint, and creates its copy. Moreover, it gets a message about
         * the error.
         * 
         * @param constraint The wrong constraint object.
         * @param message The human readable message about the error.
         */
        ConstraintException(const Constraint & constraint,
            const std::string & message);

        /**
         * Destructor of the ConstraintException.
         */
        virtual ~ConstraintException();

        /**
         * Returns with the address of copy of the wrong constraint.
         * 
         * @return Address of the wrong constraint.
         */
        const Constraint * getConstraint() const;
    private:

        /**
         * Address of the wrong constraint.
         */
        Constraint * m_constraint;
    };

    /**
     * The class expresses an exception about a constraint. This
     * exception indicates that the upper bound of the constraint
     * is invalid. It contains the copy of the wrong constraint.
     */
    class InvalidUpperBoundException : public ConstraintException
    {
    public:
        /**
         * Constructor of the InvalidUpperBoundException. It gets the original wrong
         * constraint, and creates its copy. Moreover, it gets a message about
         * the error.
         * 
         * @param variable The wrong constraint object.
         * @param message The human readable message about the error.
         */
        InvalidUpperBoundException(const Constraint & constraint,
            const std::string & message);
    };

    /**
     * The class expresses an exception about a constraint. This
     * exception indicates that the lower bound of the constraint
     * is invalid. It contains the copy of the wrong constraint.
     */
    class InvalidLowerBoundException : public ConstraintException
    {
    public:
        /**
         * Constructor of the InvalidLowerBoundException. It gets the original wrong
         * constraint, and creates its copy. Moreover, it gets a message about
         * the error.
         * 
         * @param variable The wrong constraint object.
         * @param message The human readable message about the error.
         */
        InvalidLowerBoundException(const Constraint & constraint,
            const std::string & message);
    };

    /**
     * The class expresses an exception about a constraint. This
     * exception indicates that the bounds of the constraint are invalid. 
     * It contains the copy of the wrong constraint.
     */
    class InvalidBoundsException : public ConstraintException
    {
    public:
        /**
         * Constructor of the InvalidBoundsException. It gets the original wrong
         * constraint, and creates its copy. Moreover, it gets a message about
         * the error.
         * 
         * @param constraint The wrong constraint object.
         * @param message The human readable message about the error.
         */
        InvalidBoundsException(const Constraint & constraint,
            const std::string & message);
    };    
    
    /**
     * 
     * 
     * 
     */
    inline Constraint();

    /**
     * 
     * 
     * @param name The name of the constraint
     * @param lowerBound
     * @return 
     */
    static Constraint createGreaterTypeConstraint(const char * name,
        Numerical::Double lowerBound);

    /**
     * 
     * @param name
     * @param upperBound
     * @return 
     */
    static Constraint createLessTypeConstraint(const char * name,
        Numerical::Double upperBound);

    /**
     * 
     * @param name
     * @param lowerBound
     * @param upperBound
     * @return 
     */
    static Constraint createRangeTypeConstraint(const char * name,
        Numerical::Double lowerBound,
        Numerical::Double upperBound);

    /**
     * 
     * @param name
     * @return 
     */
    static Constraint createEqualityTypeConstraint(const char * name,
        Numerical::Double value);

    /**
     * 
     * @param name
     * @return 
     */
    static Constraint createNonBindingTypeConstraint(const char * name);

    /**
     *
     * @return
     */
    inline Numerical::Double getLowerBound() const;

    /**
     *
     * @param lowerBound
     */
    inline void setLowerBound(Numerical::Double lowerBound);

    /**
     *
     * @return
     */
    inline TYPE getType() const;

    /**
     *
     * @return
     */
    inline Numerical::Double getUpperBound() const;

    /**
     *
     * @param upperBound
     */
    inline void setUpperBound(Numerical::Double upperBound);

    /**
     *
     * @return
     */
    inline void setName(const char * name);

    /**
     *
     * @return
     */
    inline const char * getName() const;

    /**
     * 
     * @return 
     */
    inline const Vector * getVector() const;

    /**
     * 
     * @param constraint
     * @return 
     */
    inline bool operator==(const Constraint & constraint) const;

    /**
     * 
     */
    friend std::ostream & operator<<(std::ostream & os, const Constraint & constraint);

private:
    /**
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    Numerical::Double m_lowerBound;

    /**
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    Numerical::Double m_upperBound;

    /**
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    TYPE m_type;

    /**
     * @author Jozsef Smidla <smidla@dcs.uni-pannon.hu>
     */
    std::string m_name;

    /**
     */
    const Vector * m_vector;

    /**
     * 
     * @param lowerBound
     * @param upperBound
     * @param value
     * @param name
     * @param vector
     */
    inline Constraint(Numerical::Double lowerBound,
        Numerical::Double upperBound,
        const char * name);

    /**
     * 
     */
    inline void adjustType();

    /**
     * 
     * @param vector
     */
    inline void setVector(const Vector & vector);

    /**
     * 
     */
    void check() const;
};

inline Constraint::Constraint()
{
    m_lowerBound = 0;
    m_upperBound = infinity;
    m_type = GREATER_OR_EQUAL;
    m_vector = 0;
}

inline Constraint::Constraint(Numerical::Double lowerBound,
    Numerical::Double upperBound,
    const char * name)
{
    m_lowerBound = lowerBound;
    m_upperBound = upperBound;
    if (name != 0) {
        m_name = name;
    }
    m_vector = 0;
    check();
    adjustType();
}

inline Numerical::Double Constraint::getLowerBound() const
{
    return m_lowerBound;
}

inline void Constraint::setLowerBound(Numerical::Double lowerBound)
{
    m_lowerBound = lowerBound;
    check();
    adjustType();
}

inline Constraint::TYPE Constraint::getType() const
{
    return m_type;
}

inline Numerical::Double Constraint::getUpperBound() const
{
    return m_upperBound;
}

inline void Constraint::setUpperBound(Numerical::Double upperBound)
{
    m_upperBound = upperBound;
    check();
    adjustType();
}

inline void Constraint::setName(const char * name)
{
    m_name = name;
}

inline const char * Constraint::getName() const
{
    if (m_name.length() > 0) {
        return m_name.c_str();
    }
    return "<NO NAME>";
}

inline void Constraint::setVector(const Vector & vector)
{
    m_vector = &vector;
}

inline const Vector * Constraint::getVector() const
{
    return m_vector;
}

void Constraint::adjustType()
{
    if (m_lowerBound == m_upperBound) {
        m_type = EQUALITY;
    } else if (m_lowerBound == -infinity) {
        if (m_upperBound == infinity) {
            m_type = NON_BINDING;
        } else if (m_upperBound < infinity) {
            m_type = LESS_OR_EQUAL;
        }
    } else if (m_lowerBound > -infinity) {
        if (m_upperBound == infinity) {
            m_type = GREATER_OR_EQUAL;
        } else if (m_upperBound < infinity) {
            m_type = RANGE;
        }
    }
}

inline bool Constraint::operator==(const Constraint & constraint) const
{
    if (this->m_lowerBound != constraint.m_lowerBound) {
        return false;
    }
    if (this->m_name != constraint.m_name) {
        return false;
    }
    if (this->m_type != constraint.m_type) {
        return false;
    }
    if (this->m_upperBound != constraint.m_upperBound) {
        return false;
    }
    if (this->m_vector != constraint.m_vector) {
        return false;
    }
    return true;
}

#endif	/* CONSTRAINT_H */

