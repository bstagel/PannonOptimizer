#include <cmath>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <sys/stat.h>

#include <tokenizer.h>
#include <prettyprint.h>

#define __UNUSED(x) (void)x;

#if !(__cplusplus > 199711L)
  #error ERROR: PLEASE COMPILE WITH C++0x11!
#endif

#if defined(_WIN32) || defined(__WIN32) || defined(__WIN32__)
    #ifndef WIN32
        #define WIN32
    #endif
#endif

#ifdef WIN32
    #define PATH_SEPARATOR '\\'
#else
    #define PATH_SEPARATOR '/'
#endif

const std::string outputName("result.txt");

class Value{
public:
    enum VALUE_TYPE{
        OPTION,
        SET,
        RANGE,
        VALUE_TYPE_ENUM_LENGTH
    };

protected:
    Value(VALUE_TYPE type):type(type){}

    VALUE_TYPE type;
};

class Option : public Value{
public:
    Option():Value(OPTION){}

private:

    friend std::ostream& operator<<(std::ostream & os, const Option& option){
        __UNUSED(option);
        return os;
    }
};

class Set : public Value{
public:
    Set():Value(SET){}

    void addValue(std::string value) {m_valueSet.insert(value);}
    const std::set<std::string>& getValues() const {return m_valueSet;}
    int getValueCount() const {return m_valueSet.size();}

    bool operator<=(const Set& other) const {
        for(std::set<std::string>::const_iterator it = m_valueSet.begin();
            it != m_valueSet.end();++it){
            if(other.m_valueSet.find(*it) == other.m_valueSet.end()){
                return false;
            }
        }
        return true;
    }

private:
    std::set<std::string> m_valueSet;

    friend std::ostream& operator<<(std::ostream & os, const Set& set){
        os << "{";
        for(std::set<std::string>::const_iterator it = set.m_valueSet.begin();
                it != set.m_valueSet.end(); ++it){
            if(it != set.m_valueSet.begin()){
                os << " ";
            }
            os << *it;
        }
        os << "}";
        return os;
    }
};

class Range : public Value{
public:
    Range():
        Value(RANGE),
        m_logarithmic(false),
        m_integer(false),
        m_sampleCount(1),
        m_from(0),
        m_to(0)
    {}

    //TODO ensure safety
    int getIntegerSample(int sampleIndex) const {
        return getSample(sampleIndex);
    }

    double getSample(int sampleIndex) const {
        if(sampleIndex == 0){
            return m_from;
        } else if (sampleIndex > m_sampleCount-1){
            throw SampleException();
        }
        double returnValue;
        if(m_logarithmic) {
            double step = exp(log((double)m_to / m_from) / (m_sampleCount-1));
            returnValue = m_from * pow(step,sampleIndex);
        } else  {
            double step = (double)(m_to - m_from) / (m_sampleCount-1);
            returnValue = m_from + sampleIndex*step;
        }
        return returnValue;
    }

    void setLogarithmic(bool logarithmic) {m_logarithmic = logarithmic;}
    bool getInteger() const { return m_integer;}
    void setInteger(bool integer) {m_integer = integer;}
    int getSampleCount() const { return m_sampleCount;}
    void setSampleCount(int sampleCount) {m_sampleCount = sampleCount;}
    void setFrom(double from) {m_from = from;}
    void setTo(double to) {m_to = to;}

    void checkRange(){
        if(m_from > m_to){
            double temp = m_from;
            m_from = m_to;
            m_to = temp;
        }
    }

    bool operator<=(const Range& other) const{
        if(m_from >= other.m_from && m_to <= other.m_to){
            return true;
        }
        return false;
    }

private:
    bool m_logarithmic;
    bool m_integer;
    int m_sampleCount;
    double m_from;
    double m_to;

    friend std::ostream& operator<<(std::ostream & os, const Range& range){
        os << "[" << range.m_from << ", " << range.m_to << "] " << range.m_sampleCount;
        if(range.m_logarithmic){
            os << " logarithmic";
        } else {
            os << " linear";
        }
        if(range.m_integer){
            os << " integer";
        }
        return os;
    }
};

struct ApplicationConfiguration{
    std::string applicationName;
    std::string parameterSyntax;
    std::string problemSyntax;
    std::string outputSyntax;
    std::map<std::string, Option> options;
    std::map<std::string, Set> sets;
    std::map<std::string, Range> ranges;

    friend std::ostream& operator<<(std::ostream & os, const ApplicationConfiguration& application){
        os << "Application descriptor: " << std::endl;
        os << "Application name: " << application.applicationName << std::endl;
        os << "Parameter syntax: " << application.parameterSyntax << std::endl;
        os << "Problem syntax: " << application.problemSyntax << std::endl;
        os << "Output syntax: " << application.outputSyntax << std::endl;
        os << "Options: " << std::endl;
        for(std::map<std::string, Option>::const_iterator it = application.options.begin();
                it != application.options.end(); ++it){
            os << "\t" << it->first << ": " << it->second << std::endl;
        }
        os << "Sets: " << std::endl;
        for(std::map<std::string, Set>::const_iterator it = application.sets.begin();
                it != application.sets.end(); ++it){
            os << "\t" << it->first << ": " << it->second << std::endl;
        }
        os << "Ranges: " << std::endl;
        for(std::map<std::string, Range>::const_iterator it = application.ranges.begin();
                it != application.ranges.end(); ++it){
            os << "\t" << it->first << ": " << it->second << std::endl;
        }
        os << std::endl;
        return os;
    }
};

struct SweepConfiguration{
    std::string sweepName;
    std::string applicationName;
    std::string remotePath;
    std::string remoteDir;
    std::map<std::string, Option> options;
    std::map<std::string, Set> sets;
    std::map<std::string, Range> ranges;
    std::vector<std::string> problems;

    friend std::ostream& operator<<(std::ostream & os, const SweepConfiguration& sweep){
        os << "Sweep descriptor: " << std::endl;
        os << "Application name: " << sweep.sweepName << std::endl;
        os << "Sweep name: " << sweep.applicationName << std::endl;
        os << "Remote path: " << sweep.remotePath << std::endl;
        os << "Remote dir: " << sweep.remoteDir << std::endl;
        os << "Options: " << std::endl;
        for(std::map<std::string, Option>::const_iterator it = sweep.options.begin();
                it != sweep.options.end(); ++it){
            os << "\t" << it->first << ": " << it->second << std::endl;
        }
        os << "Sets: " << std::endl;
        for(std::map<std::string, Set>::const_iterator it = sweep.sets.begin();
                it != sweep.sets.end(); ++it){
            os << "\t" << it->first << ": " << it->second << std::endl;
        }
        os << "Ranges: " << std::endl;
        for(std::map<std::string, Range>::const_iterator it = sweep.ranges.begin();
                it != sweep.ranges.end(); ++it){
            os << "\t" << it->first << ": " << it->second << std::endl;
        }
        os << "Problems: " << std::endl;
        for(std::vector<std::string>::const_iterator it = sweep.problems.begin();
                it != sweep.problems.end(); ++it){
            os << "\t" << *it << std::endl;
        }
        os << std::endl;
        return os;
    }
};

template <typename T>
std::string to_string_scientific(const T a_value, const int n = 6)
{
    std::ostringstream out;
    out << std::scientific << std::setprecision(n) << a_value;
    return out.str();
}

void printHelp() {
    std::cout << "Usage: GBACGenerator [APPLICATION] [SWEEP] \n" <<
                 "Generate paramter sweep files of the application specified by \n"<<
                 "application descriptor file [APPLICATION] using sweep descriptor \n"<<
                 "file [SWEEPFILE]\n"<<
                 "\n"<<
                 "Application specific parameters can be set in the .GAD application descriptor. \n"<<
                 "Sweep specific parameters can be set in the .GSD sweep descriptor. \n"<<
                 "\n"<<
                 "   -h, --help      \t Displays this help.\n"<<
                 "\n";
}

bool isFile(std::string path){
    struct stat s;
    if( stat(path.c_str(),&s) == 0 )
    {
        if( s.st_mode & S_IFREG )
        {
            return true;
        }
    }
    return false;
}

bool parseApplicationDescriptor(ApplicationConfiguration* application, const std::string & path){
    Tokenizer tokenizer;
    std::vector<Tokenizer::Token> tokens;
    std::vector<std::string> rows;
    std::ifstream ifs(path);
    tokenizer.tokenize(ifs, &tokens, &rows);

    ifs.close();
    unsigned int i = 0;
    while (i < tokens.size()) {
        if(tokens[i].m_value.compare("APPLICATION_NAME") == 0){
            if(i+2 < tokens.size() && tokens[i+1].m_value.compare("=") == 0){
                i+=2;
                application->applicationName = "";
                unsigned int currentRow = tokens[i].m_row;
                bool space = false;
                while(i < tokens.size() && currentRow == tokens[i].m_row ){
                    if(space){
                        application->applicationName.append(" ");
                    }
                    application->applicationName.append(tokens[i].m_value);
                    space = true;
                    i++;
                }
            } else {
                throw SyntaxErrorException("Application name error",
                                           rows[tokens[i].m_row],
                                           tokens[i].m_row,
                                           tokens[i].m_column);
            }
        } else if(tokens[i].m_value.compare("PARAMETER_SYNTAX") == 0){
            if(i+2 < tokens.size() && tokens[i+1].m_value.compare("=") == 0 &&
                    rows[tokens[i].m_row].find("$PARAMETER_NAME") != std::string::npos &&
                    rows[tokens[i].m_row].find("$PARAMETER_VALUE") != std::string::npos){
                i+=2;
                application->parameterSyntax = "";
                unsigned int currentRow = tokens[i].m_row;
                bool space = false;
                while(i < tokens.size() && currentRow == tokens[i].m_row ){
                    if(space){
                        application->parameterSyntax.append(" ");
                    }
                    application->parameterSyntax.append(tokens[i].m_value);
                    if(tokens[i].m_value.compare("-") == 0){
                        space = false;
                    } else {
                        space = true;
                    }
                    i++;
                }
            } else {
                throw SyntaxErrorException("Parameter syntax error",
                                           rows[tokens[i].m_row],
                                           tokens[i].m_row,
                                           tokens[i].m_column);
            }
        } else if(tokens[i].m_value.compare("PROBLEM_SYNTAX") == 0){
            if(i+2 < tokens.size() && tokens[i+1].m_value.compare("=") == 0 &&
                    rows[tokens[i].m_row].find("$PROBLEM_NAME") != std::string::npos){
                i+=2;
                application->problemSyntax = "";
                unsigned int currentRow = tokens[i].m_row;
                bool space = false;
                while(i < tokens.size() && currentRow == tokens[i].m_row ){
                    if(space){
                        application->problemSyntax.append(" ");
                    }
                    application->problemSyntax.append(tokens[i].m_value);
                    if(tokens[i].m_value.compare("-") == 0){
                        space = false;
                    } else {
                        space = true;
                    }
                    i++;
                }
            } else {
                throw SyntaxErrorException("Parameter syntax error",
                                           rows[tokens[i].m_row],
                                           tokens[i].m_row,
                                           tokens[i].m_column);
            }
        }  else if(tokens[i].m_value.compare("OUTPUT_SYNTAX") == 0){
            if(i+2 < tokens.size() && tokens[i+1].m_value.compare("=") == 0 &&
                    rows[tokens[i].m_row].find("$OUTPUT_NAME") != std::string::npos){
                i+=2;
                application->outputSyntax = "";
                unsigned int currentRow = tokens[i].m_row;
                bool space = false;
                while(i < tokens.size() && currentRow == tokens[i].m_row ){
                    if(space){
                        application->outputSyntax.append(" ");
                    }

                    if(tokens[i].m_value.compare("\">>\"") == 0){
                        application->outputSyntax.append(">>");
                    } else {
                        application->outputSyntax.append(tokens[i].m_value);
                    }
                    if(tokens[i].m_value.compare("-") == 0){
                        space = false;
                    } else {
                        space = true;
                    }
                    i++;
                }
            } else {
                throw SyntaxErrorException("Parameter syntax error",
                                           rows[tokens[i].m_row],
                                           tokens[i].m_row,
                                           tokens[i].m_column);
            }
        } else if(tokens[i].m_value.compare("OPTIONS") == 0){
            i++;
            while (i < tokens.size()) {
                //Check the beginning of the next line
                if(tokens[i].m_value.compare("OPTIONS") == 0 ||
                    tokens[i].m_value.compare("SETS") == 0 ||
                    tokens[i].m_value.compare("RANGES") == 0){
                    break;
                }
                //Parse an option
                application->options.insert(std::pair<std::string, Option>
                                                (tokens[i].m_value, Option()));
                i++;
            }
        } else if(tokens[i].m_value.compare("SETS") == 0){
            i++;
            while (i < tokens.size()) {
                //Check the beginning of the next line
                if(tokens[i].m_value.compare("OPTIONS") == 0 ||
                    tokens[i].m_value.compare("SETS") == 0 ||
                    tokens[i].m_value.compare("RANGES") == 0){
                    break;
                }
                //Parse a set
                if(i+3 < tokens.size() &&
                        tokens[i+1].m_value.compare("=") == 0 &&
                        tokens[i+2].m_value.compare("{") == 0){
                    std::string& name = tokens[i].m_value;
                    Set set;
                    i+=3;
                    while(tokens[i].m_value.compare("}") != 0){
                        set.addValue(tokens[i].m_value);
                        i++;
                        if(tokens[i].m_value.compare(",") == 0){
                            i++;
                        }
                    }
                    i++;
                    application->sets.insert(std::pair<std::string, Set>(name, set));
                } else {
                    throw SyntaxErrorException("Invalid set, syntax error! ",
                                               rows[tokens[i].m_row],
                                               tokens[i].m_row,
                                               tokens[i].m_column);
                }
            }
        } else if(tokens[i].m_value.compare("RANGES") == 0){
            i++;
            while (i < tokens.size()) {
                //Check the beginning of the next line
                if(tokens[i].m_value.compare("OPTIONS") == 0 ||
                    tokens[i].m_value.compare("SETS") == 0 ||
                    tokens[i].m_value.compare("RANGES") == 0){
                    break;
                }
                //Parse a range
                if(i+6 < tokens.size() &&
                        tokens[i+1].m_value.compare("=") == 0 &&
                        tokens[i+2].m_value.compare("[") == 0 &&
                        tokens[i+4].m_value.compare(",") == 0 &&
                        tokens[i+6].m_value.compare("]") == 0){
                    std::string& name = tokens[i].m_value;
                    Range range;
                    range.setFrom(std::stod(tokens[i+3].m_value));
                    range.setTo(std::stod(tokens[i+5].m_value));
                    i+=7;
                    if(i < tokens.size() &&
                            tokens[i-7].m_row == tokens[i].m_row){
                        if(tokens[i].m_value.compare("integer") == 0 ){
                            range.setInteger(true);
                            i++;
                        } else {
                            throw SyntaxErrorException("Invalid range parameter, syntax error! ",
                                                      rows[tokens[i].m_row],
                                                       tokens[i].m_row,
                                                       tokens[i].m_column);
                        }
                    }
                    range.checkRange();
                    application->ranges.insert(std::pair<std::string, Range>(name, range));
                } else {
                    throw SyntaxErrorException("Invalid range, syntax error! ",
                                               rows[tokens[i].m_row],
                                               tokens[i].m_row,
                                               tokens[i].m_column);
                }
            }
        } else {
            throw SyntaxErrorException("Invalid token! ",
                                       rows[tokens[i].m_row],
                                       tokens[i].m_row,
                                       tokens[i].m_column);
        }
    }
    std::cout << *application;
    return true;
}

bool parseSweepDescriptor(SweepConfiguration* sweep, const std::string & path){
    Tokenizer tokenizer;
    std::vector<Tokenizer::Token> tokens;
    std::vector<std::string> rows;
    std::ifstream ifs(path);

    tokenizer.tokenize(ifs, &tokens, &rows);

    ifs.close();
    unsigned int i = 0;
    while (i < tokens.size()) {
        if(tokens[i].m_value.compare("APPLICATION_NAME") == 0){
            if(i+2 < tokens.size() && tokens[i+1].m_value.compare("=") == 0){
                i+=2;
                sweep->applicationName = "";
                unsigned int currentRow = tokens[i].m_row;
                bool space = false;
                while(i < tokens.size() && currentRow == tokens[i].m_row ){
                    if(space){
                        sweep->applicationName.append(" ");
                    }
                    sweep->applicationName.append(tokens[i].m_value);
                    space = true;
                    i++;
                }
            } else {
                throw SyntaxErrorException("Application name error",
                                           rows[tokens[i].m_row],
                                           tokens[i].m_row,
                                           tokens[i].m_column);
            }
        } else if(tokens[i].m_value.compare("SWEEP_NAME") == 0){
            if(i+2 < tokens.size() && tokens[i+1].m_value.compare("=") == 0){
                i+=2;
                sweep->sweepName = "";
                unsigned int currentRow = tokens[i].m_row;
                bool space = false;
                while(i < tokens.size() && currentRow == tokens[i].m_row ){
                    if(space){
                        sweep->sweepName.append(" ");
                    }
                    sweep->sweepName.append(tokens[i].m_value);
                    space = true;
                    i++;
                }
            } else {
                throw SyntaxErrorException("Parameter syntax error",
                                           rows[tokens[i].m_row],
                                           tokens[i].m_row,
                                           tokens[i].m_column);
            }
        } else if(tokens[i].m_value.compare("INPUT_PATH") == 0){
            if(i+2 < tokens.size() && tokens[i+1].m_value.compare("=") == 0){
                i+=2;
                sweep->remotePath = "";
                unsigned int currentRow = tokens[i].m_row;
                bool space = false;
                while(i < tokens.size() && currentRow == tokens[i].m_row ){
                    if(space){
                        sweep->remotePath.append(" ");
                    }
                    sweep->remotePath.append(tokens[i].m_value);
                    space = true;
                    i++;
                }
                if(sweep->remotePath.at(0) == '"' &&
                        sweep->remotePath.at(sweep->remotePath.size()-1) == '"'){
                    sweep->remotePath = sweep->remotePath.substr(1, sweep->remotePath.size()-2);
                }
            } else {
                throw SyntaxErrorException("Parameter syntax error",
                                           rows[tokens[i].m_row],
                                           tokens[i].m_row,
                                           tokens[i].m_column);
            }
        } else if(tokens[i].m_value.compare("INPUT_DIR") == 0){
            if(i+2 < tokens.size() && tokens[i+1].m_value.compare("=") == 0){
                i+=2;
                sweep->remoteDir = "";
                unsigned int currentRow = tokens[i].m_row;
                bool space = false;
                while(i < tokens.size() && currentRow == tokens[i].m_row ){
                    if(space){
                        sweep->remoteDir.append(" ");
                    }
                    sweep->remoteDir.append(tokens[i].m_value);
                    space = true;
                    i++;
                }
                if(sweep->remoteDir.at(0) == '"' &&
                        sweep->remoteDir.at(sweep->remoteDir.size()-1) == '"'){
                    sweep->remoteDir = sweep->remoteDir.substr(1, sweep->remoteDir.size()-2);
                }
            } else {
                throw SyntaxErrorException("Parameter syntax error",
                                           rows[tokens[i].m_row],
                                           tokens[i].m_row,
                                           tokens[i].m_column);
            }
        } else if(tokens[i].m_value.compare("OPTIONS") == 0){
            i++;
            while (i < tokens.size()) {
                //Check the beginning of the next line
                if(tokens[i].m_value.compare("OPTIONS") == 0 ||
                    tokens[i].m_value.compare("SETS") == 0 ||
                    tokens[i].m_value.compare("RANGES") == 0 ||
                    tokens[i].m_value.compare("PROBLEM_NAMES") == 0){
                    break;
                }
                //Parse an option
                sweep->options.insert(std::pair<std::string, Option>
                                                (tokens[i].m_value, Option()));
                i++;
            }
        } else if(tokens[i].m_value.compare("SETS") == 0){
            i++;
            while (i < tokens.size()) {
                //Check the beginning of the next line
                if(tokens[i].m_value.compare("OPTIONS") == 0 ||
                    tokens[i].m_value.compare("SETS") == 0 ||
                    tokens[i].m_value.compare("RANGES") == 0 ||
                    tokens[i].m_value.compare("PROBLEM_NAMES") == 0){
                    break;
                }
                //Parse a set
                if(i+3 < tokens.size() &&
                        tokens[i+1].m_value.compare("=") == 0 &&
                        tokens[i+2].m_value.compare("{") == 0){
                    std::string& name = tokens[i].m_value;
                    Set set;
                    i+=3;
                    while(tokens[i].m_value.compare("}") != 0){
                        set.addValue(tokens[i].m_value);
                        i++;
                        if(tokens[i].m_value.compare(",") == 0){
                            i++;
                        }
                    }
                    i++;
                    sweep->sets.insert(std::pair<std::string, Set>(name, set));
                } else {
                    throw SyntaxErrorException("Invalid set, syntax error! ",
                                               rows[tokens[i].m_row],
                                               tokens[i].m_row,
                                               tokens[i].m_column);
                }
            }
        } else if(tokens[i].m_value.compare("RANGES") == 0){
            i++;
            while (i < tokens.size()) {
                //Check the beginning of the next line
                if(tokens[i].m_value.compare("OPTIONS") == 0 ||
                    tokens[i].m_value.compare("SETS") == 0 ||
                    tokens[i].m_value.compare("RANGES") == 0 ||
                    tokens[i].m_value.compare("PROBLEM_NAMES") == 0){
                    break;
                }
                //Parse a range
                if(i+6 < tokens.size() &&
                        tokens[i+1].m_value.compare("=") == 0 &&
                        tokens[i+2].m_value.compare("[") == 0 &&
                        tokens[i+4].m_value.compare(",") == 0 &&
                        tokens[i+6].m_value.compare("]") == 0){
                    std::string& name = tokens[i].m_value;
                    Range range;
                    range.setFrom(std::stod(tokens[i+3].m_value));
                    range.setTo(std::stod(tokens[i+5].m_value));
                    range.setSampleCount(std::stod(tokens[i+7].m_value));
                    if(tokens[i+8].m_value.compare("linear") == 0){
                        range.setLogarithmic(false);
                    } else if (tokens[i+8].m_value.compare("exponential") == 0){
                        range.setLogarithmic(true);
                    } else {
                        throw SyntaxErrorException("Invalid range parameter, syntax error! ",
                                                   rows[tokens[i].m_row],
                                                   tokens[i].m_row,
                                                   tokens[i].m_column);
                    }
                    range.checkRange();
                    i+=9;
                    sweep->ranges.insert(std::pair<std::string, Range>(name, range));
                } else {
                    throw SyntaxErrorException("Invalid range, syntax error! ",
                                               rows[tokens[i].m_row],
                                               tokens[i].m_row,
                                               tokens[i].m_column);
                }
            }
        } else if(tokens[i].m_value.compare("PROBLEM_NAMES") == 0){
            i++;
            while (i < tokens.size()) {
                //Check the beginning of the next line
                if(tokens[i].m_value.compare("OPTIONS") == 0 ||
                    tokens[i].m_value.compare("SETS") == 0 ||
                    tokens[i].m_value.compare("RANGES") == 0 ||
                    tokens[i].m_value.compare("PROBLEM_NAMES") == 0){
                    break;
                }
                //Parse a problem
                sweep->problems.push_back(tokens[i].m_value);
                i++;
            }

        } else {
            throw SyntaxErrorException("Invalid token! ",
                                       rows[tokens[i].m_row],
                                       tokens[i].m_row,
                                       tokens[i].m_column);
        }
    }
    std::cout << *sweep;
    return true;
}

void checkConsistency(const ApplicationConfiguration& application, SweepConfiguration& sweep){
    if(sweep.applicationName.compare(application.applicationName) != 0){
        std::cout << "Application name is not consistent";
        throw ConsistencyException();
    }

    for(std::map<std::string, Option>::const_iterator it = sweep.options.begin();
            it != sweep.options.end(); ++it){
        if(application.options.find(it->first) == application.options.end()){
            std::cout << "Invalid option specified in the sweep descriptor!";
            throw ConsistencyException();
        }
    }

    for(std::map<std::string, Set>::const_iterator it = sweep.sets.begin(); it != sweep.sets.end(); ++it){
        std::map<std::string, Set>::const_iterator setElement = application.sets.find(it->first);
        if(setElement == application.sets.end()){
            std::cout << "Invalid set specified in the sweep descriptor: " << it->first << "\n";
            throw ConsistencyException();
        } else {
            if(!(it->second <= setElement->second)){
                std::cout << "Invalid set values specified in the sweep descriptor: " << it->first << " \n";
                throw ConsistencyException();
            }
        }
    }

    for(std::map<std::string, Range>::iterator it = sweep.ranges.begin(); it != sweep.ranges.end(); ++it){
        std::map<std::string, Range>::const_iterator rangeElement = application.ranges.find(it->first);
        if(rangeElement == application.ranges.end()){
            std::cout << "Invalid range specified in the sweep descriptor: " << it->first << "\n";
            throw ConsistencyException();
        } else {
            if(it->second <= rangeElement->second){
                it->second.setInteger(rangeElement->second.getInteger());
            } else {
                std::cout << "Invalid range values specified in the sweep descriptor: " << it->first << " \n";
                throw ConsistencyException();
            }
        }

    }
}

void computeParameter(std::vector<std::map<std::string, std::string> >* values,
                      std::map<std::string, std::string>* actualMap,
                      unsigned int actualIndex,
                      const SweepConfiguration & sweep){
    unsigned int parameterCount = sweep.sets.size() + sweep.ranges.size();
    if(actualIndex<sweep.sets.size()){
        //Current parameter is a set
        unsigned int setIndex = actualIndex;
        auto actualSet = sweep.sets.cbegin();
        for(unsigned int i=0; i<setIndex; i++, actualSet++);
        const std::set<std::string>& valueSet = actualSet->second.getValues();
        for(auto it=valueSet.cbegin(); it != valueSet.cend(); ++it){
            (*actualMap)[actualSet->first] = *it;
            if(actualIndex == parameterCount-1){
                values->push_back(*actualMap);
            } else {
                computeParameter(values, actualMap, actualIndex+1, sweep);
            }
        }
    } else if(actualIndex >= sweep.sets.size() && actualIndex < parameterCount){
        //Current parameter is a range
        unsigned int rangeIndex = actualIndex - sweep.sets.size();
        auto actualRange = sweep.ranges.cbegin();
        for(unsigned int i=0; i<rangeIndex; i++, actualRange++);
        for(int i=0; i<actualRange->second.getSampleCount(); i++){
            if(actualRange->second.getInteger()){
                (*actualMap)[actualRange->first] = std::to_string(actualRange->second.getIntegerSample(i));
            } else {
                (*actualMap)[actualRange->first] = to_string_scientific(actualRange->second.getSample(i));
            }
            if(actualIndex == parameterCount-1){
                values->push_back(*actualMap);
            } else {
                computeParameter(values, actualMap, actualIndex+1, sweep);
            }
        }
    }
}

unsigned int generateArglists(const ApplicationConfiguration& application, const SweepConfiguration& sweep){
    std::vector<std::map<std::string, std::string> > values;
    std::map<std::string, std::string> actualMap;
    computeParameter(&values, &actualMap, 0, sweep);
    unsigned int arglistCount;
    std::string mkdirCommand ("mkdir arglists");
    system(mkdirCommand.c_str());
    mkdirCommand += PATH_SEPARATOR;
    mkdirCommand += sweep.sweepName;
    system(mkdirCommand.c_str());
    for(arglistCount=0; arglistCount<values.size(); arglistCount++){
        //Print command
        std::string line;
        line.append("./" + sweep.applicationName + " ");
        //Print parameters
        std::string nameid("$PARAMETER_NAME");
        std::string valueid("$PARAMETER_VALUE");
        for(auto it = values[arglistCount].cbegin(); it != values[arglistCount].cend(); ++it){
            std::string parameter(application.parameterSyntax);
            unsigned int namepos = parameter.find(nameid);
            if(namepos == std::string::npos){
                throw ConsistencyException();
                return 0;
            }
            parameter.replace(namepos, nameid.length(), it->first);
            unsigned int valuepos = parameter.find(valueid);
            if(valuepos == std::string::npos){
                throw ConsistencyException();
                return 0;
            }
            parameter.replace(valuepos, valueid.length(), it->second);
            line.append(parameter + " ");
        }
        //Print output syntax
        std::string output(application.outputSyntax);
        std::string outputid("$OUTPUT_NAME");
        unsigned int outputpos = output.find(outputid);
        if(outputpos == std::string::npos){
            throw ConsistencyException();
            return 0;
        }
        output.replace(outputpos, outputid.length(), outputName);

        std::ofstream arglist;
        arglist.open("arglists/"+sweep.sweepName+"/arglist_"+std::to_string(arglistCount)+".txt");
        std::cout << "arglists/"+sweep.sweepName+"/arglist_"+std::to_string(arglistCount)+".txt" << " : \n";
        for(auto it = sweep.problems.cbegin(); it != sweep.problems.cend(); ++it){
            std::string outline;
            //Print problem syntax
            std::string problem(application.problemSyntax);
            std::string problemid("$PROBLEM_NAME");
            unsigned int problempos = problem.find(problemid);
            if(problempos == std::string::npos){
                throw ConsistencyException();
                return 0;
            }
            problem.replace(problempos, problemid.length(), *it);
            outline = line + problem  + " " + output + "\n";
            arglist << outline;
            std::cout << outline;
        }
        arglist.close();
        std::cout<<"\n\n";
    }
    return arglistCount;
}

enum ScriptType
{
    METAJOB
};

void generateScript(unsigned int arglistCount, const SweepConfiguration& sweep){
    std::ofstream gbac_script;
    gbac_script.open("_3gb-metajob-" + sweep.remoteDir, std::ios::out | std::ios::binary);
    gbac_script << "%Required 1" << "\n";
    gbac_script << "%SuccessAt 100%" << "\n";
    gbac_script << "\n";

    for(unsigned int i=0; i<arglistCount; i++){
        gbac_script << "Arguments='" << outputName << "'" << "\n";
        gbac_script << "Input=arglist.txt="<< sweep.remotePath << sweep.remoteDir << "/arglist_" << i << ".txt" << "\n";
        gbac_script << "Queue \n";
        gbac_script << "\n";
    }

    gbac_script.close();
}

int main(int argc, char** argv)
{
    setbuf(stdout, 0);
    ApplicationConfiguration application;
    SweepConfiguration sweep;

    if(argc == 3){
        std::string applicationDescriptorPath = argv[1];
        std::string sweepDescriptorPath = argv[2];
        if(isFile(applicationDescriptorPath) && isFile(sweepDescriptorPath)){
            try{
                parseApplicationDescriptor(&application, applicationDescriptorPath);
                parseSweepDescriptor(&sweep, sweepDescriptorPath);
                checkConsistency(application, sweep);
                unsigned int arglistCount = generateArglists(application, sweep);
                generateScript(arglistCount, sweep);
            } catch ( const SyntaxErrorException & exception ) {
                exception.show();
                return EXIT_FAILURE;
            } catch ( const ConsistencyException & exception ) {
                std::cout << "Inconsistent files! \n";
                return EXIT_FAILURE;
            } catch ( const SampleException & exception ) {
                std::cout << "Invalid sample index! \n";
                return EXIT_FAILURE;
            }
        }
    } else {
        printHelp();
    }

    return EXIT_SUCCESS;
}
