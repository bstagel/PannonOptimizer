/**
 * @file simplexparameterhandler.h
 */

#ifndef SIMPLEXPARAMETERHANDLER_H
#define SIMPLEXPARAMETERHANDLER_H

#include <globals.h>
#include <utils/parameterhandler.h>

class SimplexParameterHandler : public ParameterHandler
{
    friend class SimplexParameterHandlerTestSuite;
    friend class InitPanOpt;
public:
    static ParameterHandler & getInstance();
    /**
    * @brief writeParameterFile
    * @throws -1 if the outputstream is not open
    */
    void writeParameterFile();
    void initParameters();

    /**
     * Loads the parameters from an external file.
     *
     * @param in The input file stream of the file.
     */
    void loadValuesFromFile(std::ifstream& in);

    /**
     * Enables or disables the parallel specific options.
     *
     * @param value If it is true, the function enables the parallel options.
     */
    void enableParallelOptions(bool value);

    /**
     * Returns the value of the given parameter as a string.
     *
     * @param name The name of the parameter.
     * @return The value of the parameter.
     */
    virtual const std::string & getStringParameterValue(const std::string& name) const;

    /**
     * Returns the value of the given parameter as an integer.
     *
     * @param name The name of the parameter.
     * @return The value of the parameter.
     */
    virtual const int & getIntegerParameterValue(const std::string& name) const;

    /**
     * Returns the value of the given parameter as a double.
     *
     * @param name The name of the parameter.
     * @return The value of the parameter.
     */
    virtual const double & getDoubleParameterValue(const std::string& name) const;

    /**
     * Returns the value of the given parameter as a boolean.
     *
     * @param name The name of the parameter.
     * @return The value of the parameter.
     */
    virtual const bool & getBoolParameterValue(const std::string& name) const;

    /**
     * Sets the given parameter's value to a double.
     *
     * @param name The name of the parameter.
     * @param value The new value of the parameter.
     */
    virtual void setParameterValue(const std::string name, const double value);

    /**
     * Sets the given parameter's value to an integer.
     *
     * @param name The name of the parameter.
     * @param value The new value of the parameter.
     */
    virtual void setParameterValue(const std::string name, const int value);

    /**
     * Sets the given parameter's value to a string.
     *
     * @param name The name of the parameter.
     * @param value The new value of the parameter.
     */
    virtual void setParameterValue(const std::string name, const std::string value);

    /**
     * Sets the given parameter's value to a string.
     *
     * @param name The name of the parameter.
     * @param value The new value of the parameter.
     */
    virtual void setParameterValue(const std::string name, const char * value);

    /**
     * Sets the given parameter's value to a boolean.
     *
     * @param name The name of the parameter.
     * @param value The new value of the parameter.
     */
    virtual void setParameterValue(const std::string name, const bool value);

private:

    bool m_enableParallelization;

    bool m_getParallelOptions;

    std::vector< std::map<std::string, Parameter> > m_threadParameters;

    static const char * sm_defaultFilename;

    static SimplexParameterHandler * sm_instance;

    static void _globalInit();

    static void _globalRelease();

    SimplexParameterHandler();
    SimplexParameterHandler(const SimplexParameterHandler &){}

    void processParallelNode(const NodeFile::Node & node,
                             const std::string & name,
                             std::map<std::string, Parameter> * threadParameters);

    const Parameter & getParameter(const std::string & name) const;

    Parameter & getParameter(const std::string & name);
};

#endif // SIMPLEXPARAMETERHANDLER_H
