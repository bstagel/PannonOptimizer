#include <lp/lpproblem.h>

LpModelBuilder::LpModelBuilder()
{
    m_inputFile = nullptr;
    clear();
    try {
        m_inputFile = new std::ifstream;
    } catch(std::bad_alloc x) {
    }
}

LpModelBuilder::~LpModelBuilder()
{
    if(m_inputFile != nullptr) delete m_inputFile;
    m_inputFile = nullptr;
    if(m_curVar != nullptr) delete m_curVar;
    m_curVar = nullptr;
}

void LpModelBuilder::loadFromFile(const std::string &fileName)
{
    m_inputFile->open(fileName);
    if (m_inputFile->is_open())
    {
        while (std::getline(*m_inputFile, m_buffer) && !m_terminated) {
            m_currentLine++;
            switch(m_section) {
            case LP_START:
                m_word = getNextWord();
                getSection();
                break;
            case LP_OBJECTIVE:
                m_word = getNextWord();
                if(checkSection(m_word)) {
                    getSection();
                } else {
                    processObj();
                }
                break;
            case LP_ST:
                m_word = getNextWord();
                if(checkSection(m_word)) {
                    getSection();
                } else {
                    processSt();
                }
                break;
            case LP_BOUNDS:
                m_word = getNextWord();
                if(checkSection(m_word)) {
                    getSection();
                } else {
                    processBounds();
                }
                break;
            default:
                getSection();
                break;
            }
        }
        indexEntities();
        m_inputFile->close();
    } else {
        m_error.m_errorType = 3;
        m_error.m_errorLine = m_currentLine;
        m_error.m_errorStr = "Unable to open file.";
    }

    if(m_terminated) {
        std::cout<<"LP READER:"<<std::endl;
        std::cout<<"Error at "<<fileName<<" (Line "<<m_error.m_errorLine<<")"<<std::endl<<m_error.m_errorStr<<std::endl;
        return;
    }

//    std::cout<<"Cost vector: ";
//    for(unsigned i = 0; i < m_costVector.length(); i++) {
//        if(m_costVector[i])std::cout<<"("<<i<<";"<<m_costVector[i]<<") ";
//    }
//    std::cout<<std::endl;
//    std::cout<<"Rows: "<<std::endl;
//    for(std::list<LpRow>::iterator it = m_lpRows.begin(); it != m_lpRows.end(); ++it) {
//        std::cout<<"Row "<<(*it).m_rowName<<": "<<(*it).m_lowerBound<<"<= ";
//        for(unsigned i = 0; i < (*it).m_vector.length(); i++) {
//            if((*it).m_vector[i])std::cout<<"("<<i<<";"<<(*it).m_vector[i]<<") ";
//        }
//        std::cout<<" <="<<(*it).m_upperBound<<std::endl;
//    }
//    std::cout<<"Variables: "<<std::endl;
//    for(std::list<LpVar>::iterator it = m_lpVars.begin(); it != m_lpVars.end(); ++it) {
//        std::cout<<(*it).m_lowerBound<<"<= "<<(*it).m_varName;
//        std::cout<<" <="<<(*it).m_upperBound<<std::endl;
    //    }
}

LpModelBuilder::LpError LpModelBuilder::getError()
{
    return m_error;
}

void LpModelBuilder::clear()
{
    m_section = LP_START;
    m_lpVars.clear();
    m_lpRows.clear();
    m_row.m_lowerBound = -Numerical::Infinity;
    m_row.m_upperBound = Numerical::Infinity;
    m_row.m_rowName = "";
    m_row.m_vector.reInit(0);
    m_costVector.reInit(0);
    m_costConstant = 0;
    m_problemName = "";
    m_buffer = "";
    m_word = "";
    m_currentLine = 0;
    m_error.m_errorType = 0;
    if(m_inputFile != nullptr) delete m_inputFile;
    m_inputFile = nullptr;
    m_maximize = false;
    m_eqState = LP_EQ_NAME;
    m_currentCoeff = 1;
    m_terminated = false;
    m_type = -1;
    m_curVar = nullptr;
}

unsigned int LpModelBuilder::getColumnCount() const
{
    return m_lpVars.size();
}

unsigned int LpModelBuilder::getRowCount() const
{
    return m_lpRows.size();
}

void LpModelBuilder::buildRow(unsigned int index, SparseVector *rowVector, std::vector<unsigned int> *nonzeros) const
{
    LpRow* rowSource = m_rows[index];
    rowVector->reInit(getColumnCount());
    rowVector->addVector(1, rowSource->m_vector);
    SparseVector::NonzeroIterator itEnd = rowSource->m_vector.endNonzero();
    for(SparseVector::NonzeroIterator it = rowSource->m_vector.beginNonzero(); it != itEnd; ++it) {
       (*nonzeros)[it.getIndex()]++;
    }
}

void LpModelBuilder::buildColumn(unsigned int index, SparseVector *columnVector, std::vector<unsigned int> *nonzeros) const
{
    __UNUSED(index);
    __UNUSED(columnVector);
    __UNUSED(nonzeros);
}

bool LpModelBuilder::hasRowwiseRepresentation() const
{
    return true;
}

bool LpModelBuilder::hasColumnwiseRepresentation() const
{
    return false;
}

const Variable &LpModelBuilder::getVariable(unsigned int index) const
{
    LpVar* varSource = m_vars[index];
    static Variable result;
    result = Variable::createVariable(varSource->m_varName.c_str(),
                                      varSource->m_lowerBound,
                                      varSource->m_upperBound);
    return result;
}

const Constraint &LpModelBuilder::getConstraint(unsigned int index) const
{
    LpRow* rowSource = m_rows[index];
    static Constraint result;
    try {
        result = Constraint::createConstraint( rowSource->m_rowName.c_str(),
                                               rowSource->m_lowerBound,
                                               rowSource->m_upperBound);
    } catch (const Constraint::InvalidLowerBoundException & ex) {
        std::cout<<ex.getMessage()<<std::endl;
        exit(0);
    }

    return result;
}

void LpModelBuilder::buildCostVector(DenseVector *costVector) const
{
    costVector->reInit(getColumnCount());
    costVector->addVector(1, m_costVector);
}

double LpModelBuilder::getObjectiveConstant() const
{
    return m_costConstant;
}

std::string LpModelBuilder::getName() const
{
    return m_problemName;
}

OBJECTIVE_TYPE LpModelBuilder::getObjectiveType() const
{
    return MINIMIZE;
}

std::string LpModelBuilder::upper(const std::string &str)
{
    std::string result = str;
    toupper(result);
    return result;
}

void LpModelBuilder::toupper(std::string &str)
{
    for (auto & c: str) c = std::toupper(c);
}

std::string LpModelBuilder::getNextWord()
{
    unsigned cursor = 0;
    for(; cursor < m_buffer.length(); cursor++) if(m_buffer[cursor] != ' ') break;
    if(cursor != 0) m_buffer = m_buffer.substr(cursor, m_buffer.length() - cursor);
    std::string word = "";
    for(cursor = 0; cursor < m_buffer.length(); cursor++) if(m_buffer[cursor] == ' ' || m_buffer[cursor] == '\0') break;
    if(cursor != 0) {
        word = m_buffer.substr(0, cursor);
        m_buffer = m_buffer.substr(cursor, m_buffer.length() - cursor);
    }
    return word;
}

bool LpModelBuilder::checkSection(const std::string &word)
{
    if(word.length() > 0 && word[0] != '\\') {
        std::string uWord = upper(word);
        if(uWord == "MAXIMIZE" || uWord == "MAXIMUM" || uWord == "MAX" || uWord == "MINIMIZE" || uWord == "MINIMUM" ||
           uWord == "MIN" || uWord == "ST" || uWord == "S.T." || uWord == "SUBJECT" ||uWord == "BOUNDS" ||
           uWord == "BOUND" || uWord == "END") {
            return true;
        }
    }
    return false;
}

void LpModelBuilder::getSection()
{
    if(m_word == "") m_word = getNextWord();
    if(m_word.length() > 0 && m_word[0] != '\\') {
        toupper(m_word);
        if(m_word == "MAXIMIZE" || m_word == "MAXIMUM" || m_word == "MAX") {
            m_maximize = true;
            m_eqState = LP_EQ_NAME;
            m_section = LP_OBJECTIVE;
        }
        if(m_word == "MINIMIZE" || m_word == "MINIMUM" || m_word == "MIN") {
            m_maximize = false;
            m_eqState = LP_EQ_NAME;
            m_section = LP_OBJECTIVE;
        }
        if(m_word == "ST" || m_word == "S.T." || ((m_word == "SUBJ" || m_word == "SUBJECT") && upper(getNextWord()) == "TO")) {
            m_eqState = LP_EQ_NAME;
            m_section = LP_ST;
        }
        if(m_word == "BOUNDS" || m_word == "BOUND") {
            m_type = -1;
            m_curVar = nullptr;
            m_eqState = LP_EQ_SIGN;
            m_section = LP_BOUNDS;
        }
        if(m_word == "END") {
            m_section = LP_END;
        }
    }
}

void LpModelBuilder::processObj()
{
    if(m_eqState == LP_EQ_NAME) {
        unsigned pos = m_word.find(':');
        if(pos != std::string::npos) {
            m_problemName = m_word.substr(0, pos);
            if(pos + 1 < m_word.length()) {
                m_word = m_word.substr(pos + 1, m_word.length() - pos - 1);
            } else {
                m_word = getNextWord();
            }
        } else {
            m_problemName = "obj";
        }
        m_eqState = LP_EQ_SIGN;
    }

    parseRow(m_costVector);

    if(m_error.m_errorType != 0) std::cout<<m_error.m_errorStr<<" (Line: "<<m_error.m_errorLine<<")"<<std::endl;
}

void LpModelBuilder::processSt()
{
    if(m_eqState == LP_EQ_NAME) {
        unsigned pos = m_word.find(':');
        if(pos != std::string::npos) {
            m_row.m_rowName = m_word.substr(0, pos);
            if(pos + 1 < m_word.length()) {
                m_word = m_word.substr(pos + 1, m_word.length() - pos - 1);
            } else {
                m_word = getNextWord();
            }
        } else {
            m_row.m_rowName = "e_con_";
            m_row.m_rowName.append(std::to_string(m_lpRows.size() + 1));
        }
        m_eqState = LP_EQ_SIGN;
    }
    if(m_eqState != LP_EQ_TYPE) {
        parseRow(m_row.m_vector);
    }
    if(m_eqState == LP_EQ_TYPE) {
        unsigned pos = 0;
        while(pos < m_word.length() && (m_word[pos] == '<' || m_word[pos] == '>' || m_word[pos] == '=')) pos++;
        std::string type = m_word.substr(0, pos);
        if(pos + 1 < m_word.length()) {
            m_word = m_word.substr(pos + 1, m_word.length() - pos - 1);
        } else {
            m_word = getNextWord();
        }
        if(type == "=") {
            m_type = 0;
        } else if(type == "<=" || type == "=<" || type == "<") {
            m_type = 1;
        } else if(type == ">=" || type == "=>" || type == ">") {
            m_type = 2;
        } else {
            m_terminated = true;
            m_buffer = "";
            m_word = "";
            m_error.m_errorType = 3;
            m_error.m_errorLine = m_currentLine;
            m_error.m_errorStr = "Parse error.";
        }
        m_eqState = LP_EQ_RHS;
    }
    if(m_eqState == LP_EQ_RHS) {
        if(m_word != "") {
            unsigned endPos = 0;
            while(endPos < m_word.length() && m_word[endPos] != '\\') endPos++;
            if(m_type == 0 || m_type == 1) m_row.m_upperBound = std::stod(m_word.substr(0, endPos));
            if(m_type == 0 || m_type == 2) m_row.m_lowerBound = std::stod(m_word.substr(0, endPos));
            m_eqState = LP_EQ_NAME;
            m_lpRows.push_back(m_row);
            m_row.m_vector.reInit(0);
            m_row.m_lowerBound = -Numerical::Infinity;
            m_row.m_upperBound = Numerical::Infinity;
        }
    }
}

void LpModelBuilder::processBounds()
{
    while(m_word != "" && m_word[0] != '\\') {
        while(m_word.length() > 0 && m_word[0] != '\\') {
            if(m_eqState == LP_EQ_SIGN) {
                if(m_word[0] == '+') {
                    m_currentCoeff = 1;
                    m_word = m_word.substr(1, m_word.length() - 1);
                    m_eqState = LP_EQ_COEFF;
                } else if(m_word[0] == '-') {
                    m_currentCoeff = -1;
                    m_word = m_word.substr(1, m_word.length() - 1);
                    m_eqState = LP_EQ_COEFF;
                } else if((m_word[0] >= '0' && m_word[0] <= '9') || std::toupper(m_word[0]) == 'E') {
                    m_currentCoeff = 1;
                    m_eqState = LP_EQ_COEFF;
                } else if(m_word[0] == '<' || m_word[0] == '>' || m_word[0] == '=' || m_curVar != nullptr) {
                    m_terminated = true;
                    m_buffer = "";
                    m_word = "";
                    m_error.m_errorType = 3;
                    m_error.m_errorLine = m_currentLine;
                    m_error.m_errorStr = "Parse error.";
                } else {
                    m_eqState = LP_EQ_VAR;
                }
            } else if(m_eqState == LP_EQ_COEFF) {
                if((m_word[0] >= '0' && m_word[0] <= '9') || std::toupper(m_word[0]) == 'E') {
                    unsigned endPos = 0;
                    while(endPos < m_word.length() && ((m_word[endPos] >= '0' && m_word[endPos] <= '9') || std::toupper(m_word[endPos]) == 'E' || m_word[endPos] == '.' || m_word[endPos] == '-' || m_word[endPos] == '+')) endPos++;
                    m_currentCoeff *= std::stod(m_word.substr(0, endPos));
                    if(endPos != m_word.length()) {
                        m_word = m_word.substr(endPos, m_word.length() - endPos);
                    } else m_word = "";
                } else if(upper(m_word) == "INF" || upper(m_word) == "INFINITY") {
                    m_currentCoeff *= Numerical::Infinity;
                    m_word = "";
                } else {
                    m_terminated = true;
                    m_buffer = "";
                    m_word = "";
                    m_error.m_errorType = 3;
                    m_error.m_errorLine = m_currentLine;
                    m_error.m_errorStr = "Parse error.";
                    return;
                }
                m_eqState = m_curVar == nullptr ? LP_EQ_TYPE : LP_EQ_RHS;
            } else if(m_eqState == LP_EQ_TYPE) {
                unsigned pos = 0;
                while(pos < m_word.length() && (m_word[pos] == '<' || m_word[pos] == '>' || m_word[pos] == '=')) pos++;
                std::string type = m_word.substr(0, pos);
                if(pos < m_word.length()) {
                    m_word = m_word.substr(pos, m_word.length() - pos);
                } else {
                    m_word = getNextWord();
                }
                if(type == "=") {
                    m_type = 0;
                } else if(type == "<=" || type == "=<" || type == "<") {
                    m_type = 1;
                } else if(type.length() == 0 && upper(m_word) == "FREE" && m_curVar != nullptr) {
                    m_curVar->m_lowerBound = -Numerical::Infinity;
                    m_curVar->m_upperBound = Numerical::Infinity;
                    m_type = 0;
                    m_curVar = nullptr;
                    m_eqState = LP_EQ_SIGN;
                    m_word = "";
                    return;
                } else if((m_word[0] >= '0' && m_word[0] <= '9') || std::toupper(m_word[0]) == 'E') {
                    m_type = 0;
                    m_curVar = nullptr;
                    m_eqState = LP_EQ_SIGN;
                } else {
                    std::cout<<upper(m_word)<<std::endl;
                    m_terminated = true;
                    m_buffer = "";
                    m_word = "";
                    m_error.m_errorType = 3;
                    m_error.m_errorLine = m_currentLine;
                    m_error.m_errorStr = "Parse error.";
                }
                m_eqState = m_curVar == nullptr ? LP_EQ_VAR : LP_EQ_SIGN;
            } else if(m_eqState == LP_EQ_VAR) {
                if((m_word[0] >= '0' && m_word[0] <= '9') || std::toupper(m_word[0]) == 'E' || m_word[0] == '-' || m_word[0] == '+' || m_word[0] == '<' || m_word[0] == '=' || m_word[0] == '>') {
                    m_terminated = true;
                    m_buffer = "";
                    m_word = "";
                    m_error.m_errorType = 3;
                    m_error.m_errorLine = m_currentLine;
                    m_error.m_errorStr = "Parse error.";
                } else {
                    unsigned endPos = 0;
                    while(endPos < m_word.length() && m_word[endPos] != '\\' && m_word[endPos] != '<' && m_word[endPos] != '=') endPos++;
                    std::string currentVar = m_word.substr(0, endPos);
                    if(endPos != m_word.length()) {
                        m_word = m_word.substr(endPos, m_word.length() - endPos);
                    } else m_word = "";
                    m_curVar = getVarPtr(currentVar);
                    if(m_curVar == nullptr) {
                        m_terminated = true;
                        m_buffer = "";
                        m_word = "";
                        m_error.m_errorType = 3;
                        m_error.m_errorLine = m_currentLine;
                        m_error.m_errorStr = "Undefined variable.";
                        return;
                    }
                    if(m_type == 1) {
                        m_curVar->m_lowerBound = m_currentCoeff;
                    }
                    m_eqState = LP_EQ_TYPE;
                }
            }
            if(m_eqState == LP_EQ_RHS) {
                m_curVar->m_upperBound = m_currentCoeff;
                if (m_type == 0) m_curVar->m_lowerBound = m_currentCoeff;
                m_type = 0;
                m_curVar = nullptr;
                m_eqState = LP_EQ_SIGN;
            }
        }
        if(m_word == "") m_word = getNextWord();
    }
}

void LpModelBuilder::indexEntities()
{
    m_rows.resize(getRowCount());
    m_vars.resize(getColumnCount());
    m_costVector.resize(getColumnCount());
    int i = 0;
    std::list<LpVar>::iterator itVarEnd = m_lpVars.end();
    for(std::list<LpVar>::iterator it = m_lpVars.begin(); it != itVarEnd; ++it, i++) {
        m_vars[i] = &(*it);
    }
    i = 0;
    std::list<LpRow>::iterator itRowEnd = m_lpRows.end();
    for(std::list<LpRow>::iterator it = m_lpRows.begin(); it != itRowEnd; ++it, i++) {
        (*it).m_vector.resize(getColumnCount());
        m_rows[i] = &(*it);
    }
    if(m_maximize) {
        m_costVector.scale(-1);
        m_maximize = false;
    }
}

void LpModelBuilder::parseRow(SparseVector &target)
{
    target.resize(m_lpVars.size());
    if(m_word == "") m_word = getNextWord();
    while(m_word != "" && m_word[0] != '\\') {
        while(m_word.length() > 0 && m_word[0] != '\\') {
            if(m_eqState == LP_EQ_SIGN) {
                if(m_word[0] == '+') {
                    m_currentCoeff = 1;
                    m_word = m_word.substr(1, m_word.length() - 1);
                    m_eqState = LP_EQ_COEFF;
                } else if(m_word[0] == '-') {
                    m_currentCoeff = -1;
                    m_word = m_word.substr(1, m_word.length() - 1);
                    m_eqState = LP_EQ_COEFF;
                } else if((m_word[0] >= '0' && m_word[0] <= '9') || std::toupper(m_word[0]) == 'E') {
                    m_eqState = LP_EQ_COEFF;
                } else if(m_word[0] == '<' || m_word[0] == '>' || m_word[0] == '=') {
                    m_eqState = LP_EQ_TYPE;
                } else {
                    m_eqState = LP_EQ_VAR;
                }
            } else if(m_eqState == LP_EQ_COEFF) {
                if((m_word[0] >= '0' && m_word[0] <= '9') || std::toupper(m_word[0]) == 'E') {
                    unsigned endPos = 0;
                    while(endPos < m_word.length() && ((m_word[endPos] >= '0' && m_word[endPos] <= '9') || std::toupper(m_word[endPos]) == 'E' || m_word[endPos] == '.' || m_word[endPos] == '-' || m_word[endPos] == '+')) endPos++;
                    m_currentCoeff *= std::stod(m_word.substr(0, endPos));
                    if(endPos != m_word.length()) {
                        m_word = m_word.substr(endPos, m_word.length() - endPos);
                    } else m_word = "";
                    m_eqState = LP_EQ_VAR;
                } else {
                    m_eqState = LP_EQ_VAR;
                }
            } else if(m_eqState == LP_EQ_VAR) {
                if((m_word[0] >= '0' && m_word[0] <= '9') || std::toupper(m_word[0]) == 'E') {
                    m_terminated = true;
                    m_buffer = "";
                    m_word = "";
                    m_error.m_errorType = 3;
                    m_error.m_errorLine = m_currentLine;
                    m_error.m_errorStr = "Parse error.";
                } else {
                    unsigned endPos = 0;
                    while(endPos < m_word.length() && m_word[endPos] != '\\' && m_word[endPos] != '+' && m_word[endPos] != '-') endPos++;
                    std::string currentVar = m_word.substr(0, endPos);
                    if(endPos != m_word.length()) {
                        m_word = m_word.substr(endPos, m_word.length() - endPos);
                    } else m_word = "";
                    int index = getVar(currentVar);
                    if(index == -1) {
                        m_lpVars.push_back(LpVar {currentVar, 0, Numerical::Infinity});
                        target.resize(m_lpVars.size());
                        index = m_lpVars.size() - 1;
                    }
                    if(target[index] != 0) {
                        m_terminated = true;
                        m_buffer = "";
                        m_word = "";
                        m_error.m_errorType = 2;
                        m_error.m_errorLine = m_currentLine;
                        m_error.m_errorStr = "Error: duplicated variable: ";
                        m_error.m_errorStr.append(currentVar);
                    } else {
                        target.set(index, m_currentCoeff);
                    }
                    m_eqState = LP_EQ_SIGN;
                }
            } else if(m_eqState == LP_EQ_TYPE) return;
        }
        if(m_word == "") m_word = getNextWord();
    }
}

int LpModelBuilder::getVar(const std::string &name)
{
    int i = 0;
    std::list<LpVar>::iterator itEnd = m_lpVars.end();
    for(std::list<LpVar>::iterator it = m_lpVars.begin(); it != itEnd; ++it, i++) {
        if((*it).m_varName == name) return i;
    }
    return -1;
}

LpModelBuilder::LpVar *LpModelBuilder::getVarPtr(const std::string &name)
{
    int i = 0;
    std::list<LpVar>::iterator itEnd = m_lpVars.end();
    for(std::list<LpVar>::iterator it = m_lpVars.begin(); it != itEnd; ++it, i++) {
        if((*it).m_varName == name) return &(*it);
    }
    return nullptr;
}
