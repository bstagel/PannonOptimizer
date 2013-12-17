/**
 * @file constraint.h
 */


#ifndef CONSTRAINT_H
#define	CONSTRAINT_H

#include <globals.h>

#include <utils/numerical.h>
#include <linalg/vector.h>
#include <utils/exceptions.h>

/**
 * Represents a general constraint of an LP problem. The constraint has a name, an L lower bound
 * and an U upper bound (\f$L \in \{-\infty\} \cup \mathbb{R} \f$ and \f$U \in \{+\infty\} \cup \mathbb{R} \f$).
 * The general formula of a constraint is the following:
 * \f$ L \leq a_1 x_1 + a_2 x_2 + \ldots + a_n + x_n \leq U \f$
 */
class Constraint
{
    friend class ConstraintTestSuite;
    friend class Model;
public:

    /**
     * This enum type describes the type of the constraint.
     */
    enum CONSTRAINT_TYPE
    {
        GREATER_OR_EQUAL = 0, /*!< The constraint has a finite lower bound */
        LESS_OR_EQUAL, /*!< The constraint has a finite upper bound */
        RANGE, /*!< The constraint has finite bounds */
        EQUALITY, /*!< The finite bounds are the same */
        NON_BINDING, /*!< The constraint has no finite bounds */
        CONSTRAINT_TYPE_ENUM_LENGTH
    };

    /**
     * A general exception class that represents constraint exception. Contains
     * a copy of the wrong constraint.
     */
    class ConstraintException : public PanOptException
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
     * Creates a GREATER_OR_EQUAL type constraint, with 0 lower and infinity upper
     * bound, and the constraint has no name.
     */
    inline Constraint();

    /**
     * Creates a constraint, with the given bounds.
     *
     * If the upperBound is -infinity, the function throws a
     * Constraint::InvalidUpperBoundException.
     * If the lowerBound is infinity, the function throws a
     * Constraint::InvalidLowerBoundException.
     * If the upperBound is less than the lowerBound, the function throws a
     * Constraint::InvalidBoundsException.
     *
     * @param name The name of the constraint
     * @param lowerBound The lower bound of the constraint
     * @param upperBound The upper bound of the constraint
     * @return The requested constraint
     */
    static Constraint createConstraint(const char * name,
                                       Numerical::Double lowerBound,
                                       Numerical::Double upperBound) throw (InvalidLowerBoundException,
                                                                            InvalidUpperBoundException,
                                                                            InvalidBoundsException);
    
    /**
     * Creates a GREATER_OR_EQUAL type constraint, with the given lower bound.
     * The upper boudn will be + infinity. If the lower bound is - infinity, then
     * the constraint's type will be NON_BINDING.
     *
     * If the lowerBound is + infinity, the functon throws a
     * Constraint::InvalidLowerBoundException.
     *
     * @param name The name of the constraint
     * @param lowerBound The lower bound of the constraint
     * @return The requiested constraint
     */
    static Constraint createGreaterTypeConstraint(const char * name,
                                                  Numerical::Double lowerBound) throw (InvalidLowerBoundException);

    /**
     * Creates a LESS_OR_EQUAL type constraint with the given upper bound.
     * The lower bound will be - infinity. If the upperBound is + infinity,
     * then the constraint' type will be NON_BINDING.
     *
     * If the upperBound is - infinity, the function throws a
     * Constraint::InvalidUpperBoundException.
     *
     * @param name The name of the constraint
     * @param upperBound The upper bound of the constraint
     * @return The requested constraint
     */
    static Constraint createLessTypeConstraint(const char * name,
                                               Numerical::Double upperBound) throw (InvalidUpperBoundException);

    /**
     * Creates a RANGE type constraint, with the given bounds.
     * If the bounds are the same, the result will be an EQUALITY,
     * and if the bounds are not finite, the result can be GREATER_OR_EQUAL,
     * LESS_OR_EQUAL or NON_BINDING also.
     *
     * If the upperBound is -infinity, the function throws a
     * Constraint::InvalidUpperBoundException.
     * If the lowerBound is infinity, the function throws a
     * Constraint::InvalidLowerBoundException.
     * If the upperBound is less than the lowerBound, the function throws a
     * Constraint::InvalidBoundsException.
     *
     * @param name The name of the constraint
     * @param lowerBound The lower bound of the constraint
     * @param upperBound The upper bound of the constraint
     * @return The requested constraint
     */
    static Constraint createRangeTypeConstraint(const char * name,
                                                Numerical::Double lowerBound,
                                                Numerical::Double upperBound) throw (InvalidLowerBoundException,
                                                                                     InvalidUpperBoundException,
                                                                                     InvalidBoundsException);

    /**
     * Creates an EQUALITY type constraint, with the given value.
     *
     * @param name The name of the constraint
     * @param value The value of the bounds
     * @return The requiested constraint
     */
    static Constraint createEqualityTypeConstraint(const char * name,
                                                   Numerical::Double value);

    /**
     * Creates a NON_BINDING type constraint.
     *
     * @param name The name of the constraint
     * @return The requiested constraint
     */
    static Constraint createNonBindingTypeConstraint(const char * name);

    /**
     * Returns with the lower bound of the constraint.
     *
     * @return The constraint's lower bound
     */
    inline Numerical::Double getLowerBound() const;

    /**
     * Sets the lower bound of the constraint.
     * If the lower bound is invalid, the function throws a
     * Constraint::InvalidLowerBoundException, and if the lower bound is
     * greater than the upper bound, the function throws a
     * Constraint::InvalidBoundsException.
     *
     * @param lowerBound The lower bound of the constarint.
     */
    inline void setLowerBound(Numerical::Double lowerBound)
    throw (InvalidLowerBoundException,
           InvalidBoundsException);

    /**
     * Returns with the type of the constraint.
     *
     * @return The type of the constraint.
     */
    inline CONSTRAINT_TYPE getType() const;

    /**
     * Returns with the upper bound of the constraint.
     *
     * @return upperBound The constraint's upper bound.
     */
    inline Numerical::Double getUpperBound() const;

    /**
     * Sets the upper bound of the constraint.
     * If the upper bound is invalid, the function throws a
     * Constraint::InvalidUpperBoundException, and if the upper bound is
     * lower than the lower bound, the function throws a
     * Constraint::InvalidBoundsException.
     *
     * @param upperBound The upper bound of the constarint.
     */
    inline void setUpperBound(Numerical::Double upperBound)
    throw (InvalidUpperBoundException,
           InvalidBoundsException);

    /**
     * Sets the lower and upper bound of the constraint. If
     * the bounds are invalid, the function throws a
     * Constraint::InvalidLowerBoundException, Constraint::InvalidUpperBoundException or
     * Constraint::InvalidBoundsException.
     * @param lowerBound
     * @param upperBound
     */
    inline void setBounds(Numerical::Double lowerBound,
                          Numerical::Double upperBound)
    throw (InvalidLowerBoundException,
           InvalidUpperBoundException,
           InvalidBoundsException);

    /**
     * Sets the constraint's name.
     *
     * @param name The constraint's new name.
     */
    inline void setName(const char * name);

    /**
     * Returns with the name of the constraint.
     *
     * @return The constraint's name.
     */
    inline const char * getName() const;

    /**
     * Returns with the pointer of the corresponding vector.
     *
     * @return The pointer of the corresponding vector.
     */
    inline const Vector * getVector() const;

    /**
     * Returns with true when each data members are equal,
     * otherwise false.
     *
     * @return The equality of the two constraints.
     */
    inline bool operator==(const Constraint & constraint) const;

    /**
     * Writes the properties of the constraint to the ostream
     * object.
     *
     * @return The reference of the ostream object.
     */
    friend std::ostream & operator<<(std::ostream & os, const Constraint & constraint);

private:
    /**
     * Represents the lower bound of the constraint. It can be - infinity also.
     * When a function sets this variable to + infinity, the function throws
     * and InvalidLowerBoundException.
     */
    Numerical::Double m_lowerBound;

    /**
     * Represents the upper bound of the constraint. It can be + infinity also.
     * When a function sets this variable to - infinity, the function throws
     * and InvalidUpperBoundException.
     */
    Numerical::Double m_upperBound;

    /**
     * Represents the type of the constraint. It is modified by the adjustType()
     * function.
     */
    CONSTRAINT_TYPE m_type;

    /**
     * Represents the name of the constraint. The default name is <NO NAME>.
     */
    std::string m_name;

    /**
     * Represents the coefficient vector of the constraint. The class Model fills
     * this value.
     */
    const Vector * m_vector;

    /**
     * General constructor of the class. It creates a constraint with the given
     * properties, and throws exception when the bounds are invalid. This is the
     * helper function of the public constraint creator functions.
     *
     * @param lowerBound The lower bound of the constraint.
     * @param upperBound The upper bound of the constraint.
     * @param name The name of the constraint.
     */
    inline Constraint(Numerical::Double lowerBound,
                      Numerical::Double upperBound,
                      const char * name) throw (InvalidLowerBoundException,
                                                InvalidUpperBoundException,
                                                InvalidBoundsException);

    /**
     * This function adjustes the m_type variable considering the m_lowerBound
     * and m_upperBound. It supposes that the bounds are correct.
     */
    inline void adjustType();

    /**
     * Sets the pointer of the corresponding coefficient vector. This is a private
     * function, because only the friend classes can call it: The class Model
     * uses this function when builds the model.
     *
     * @param vector The vector of the coefficient values.
     */
    inline void setVector(const Vector & vector);

    /**
     * Checks the validity of the bounds. If the lower bound is + infinity, it
     * thorws an InvalidLowerBoundException, if the upper bound is - infinity, it
     * throws an InvalidUpperBoundException, and if the lower bound is greater than
     * the upper bound, it throws an InvalidBoundsException.
     */
    void check() const throw (InvalidLowerBoundException,
                              InvalidUpperBoundException,
                              InvalidBoundsException);
};

inline Constraint::Constraint()
{
    m_lowerBound = 0;
    m_upperBound = Numerical::Infinity;
    m_type = GREATER_OR_EQUAL;
    m_vector = 0;
}

inline Constraint::Constraint(Numerical::Double lowerBound,
                              Numerical::Double upperBound,
                              const char * name) throw (InvalidLowerBoundException,
                                                        InvalidUpperBoundException,
                                                        InvalidBoundsException)
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
throw (InvalidLowerBoundException,
       InvalidBoundsException)
{
    m_lowerBound = lowerBound;
    check();
    adjustType();
}

inline Constraint::CONSTRAINT_TYPE Constraint::getType() const
{
    return m_type;
}

inline Numerical::Double Constraint::getUpperBound() const
{
    return m_upperBound;
}

inline void Constraint::setUpperBound(Numerical::Double upperBound)
throw (InvalidUpperBoundException,
       InvalidBoundsException)
{
    m_upperBound = upperBound;
    check();
    adjustType();
}

// TODO: tesztelot irni neki
inline void Constraint::setBounds(Numerical::Double lowerBound,
                      Numerical::Double upperBound)
throw (InvalidLowerBoundException,
       InvalidUpperBoundException,
       InvalidBoundsException) {
    m_lowerBound = lowerBound;
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
    } else if (m_lowerBound == -Numerical::Infinity) {
        if (m_upperBound == Numerical::Infinity) {
            m_type = NON_BINDING;
        } else if (m_upperBound < Numerical::Infinity) {
            m_type = LESS_OR_EQUAL;
        }
    } else if (m_lowerBound > -Numerical::Infinity) {
        if (m_upperBound == Numerical::Infinity) {
            m_type = GREATER_OR_EQUAL;
        } else if (m_upperBound < Numerical::Infinity) {
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

