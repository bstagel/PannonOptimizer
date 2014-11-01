#include <lp/mpsproblem.h>
#include <utils/numerical.h>
#include <iostream>
#include <cstring>

using namespace std;

#define DEFAULT_MPS_ERROR_PRINT_LIMIT 10

MpsModelBuilder::MpsModelBuilder() {
    m_bufferSize = 20 * 1024;
    m_buffer = 0;
    m_errorPrintLimit = DEFAULT_MPS_ERROR_PRINT_LIMIT;
    m_costConstant = 0;
    m_costVectorName.m_bits = 0;
}

MpsModelBuilder::~MpsModelBuilder() {
    clear();
}

void MpsModelBuilder::clear(){

    std::list<std::vector<Row>* >::iterator rowListIter = m_rows.m_rows.begin();
    std::list<std::vector<Row>* >::iterator rowListIterEnd = m_rows.m_rows.end();
    for (; rowListIter != rowListIterEnd; rowListIter++) {
        delete *rowListIter;
    }

    m_rows.m_rows.clear();

    std::list<std::vector<Column>* >::iterator columnListIter = m_columns.m_columns.begin();
    std::list<std::vector<Column>* >::iterator columnListIterEnd = m_columns.m_columns.end();
    for (; columnListIter != columnListIterEnd; columnListIter++) {
        delete *columnListIter;
    }

    m_columns.m_columns.clear();

    std::list<double *>::iterator doubleIter = m_values.begin();
    std::list<double *>::iterator doubleIterEnd = m_values.end();
    for (; doubleIter != doubleIterEnd; doubleIter++) {
        release(*doubleIter);
    }

    m_values.clear();

    std::list<unsigned int *>::iterator intIter = m_indices.begin();
    std::list<unsigned int *>::iterator intIterEnd = m_indices.end();
    for (; intIter != intIterEnd; intIter++) {
        release(*intIter);
    }

    m_indices.clear();
}

void MpsModelBuilder::showBuffer(int count, bool newLine) {
    cout << "-------------------" << endl << ">";
    for (int i = 0; i < count; i++) {
        if (newLine) {
            cout << i << ".: ";
        }
        if (m_buffer[i] == '\n') {
            cout << "\\n";
        } else if (m_buffer[i] >= ' '){
            cout << m_buffer[i];
        }
        if (newLine) {
            cout << endl;
        }
        //cin.get();
    }
    cout << endl;
    cin.get();
}

MpsModelBuilder::MPS_SECTION MpsModelBuilder::readSectionType() {
    std::string type(7, ' ');
    unsigned int index = 0;
    while (*m_buffer > ' ') {
        type[index] = *m_buffer;
        index++;
        m_buffer++;
        if (m_buffer >= m_bufferEnd) {
            m_buffer = m_inputFile.nextBlock(m_buffer);
        }
    }
    if (type == "NAME   ") {
        return MPS_NAME;
    }
    if (type == "ROWS   ") {
        return MPS_ROWS;
    }
    if (type == "COLUMNS") {
        return MPS_COLUMNS;
    }
    if (type == "RHS    ") {
        return MPS_RHS;
    }
    if (type == "RANGES ") {
        return MPS_RANGES;
    }
    if (type == "BOUNDS ") {
        return MPS_BOUNDS;
    }
    if (type == "ENDATA ") {
        return MPS_ENDATA;
    }

    return MPS_UNKNOWN;
}

void MpsModelBuilder::skipEndLine() {
    while (*m_buffer < ' ') {
        m_buffer++;
        if (m_buffer >= m_bufferEnd) {
            m_buffer = m_inputFile.nextBlock(m_buffer);
        }
    }
    m_currentLine++;
}

bool MpsModelBuilder::skipComments() {
    if (*m_buffer == '*' || *m_buffer <= ' ') {

        while (*m_buffer >= ' ') {
            m_buffer++;
            if (m_buffer >= m_bufferEnd) {
                m_buffer = m_inputFile.nextBlock(m_buffer);
            }
        }
        while (*m_buffer < ' ') {
            m_buffer++;
            if (m_buffer >= m_bufferEnd) {
                m_buffer = m_inputFile.nextBlock(m_buffer);
            }
        }
        m_currentLine++;
        return true;
    }
    return false;
}

void MpsModelBuilder::readName() {
    while (*m_buffer == ' ') {
        m_buffer++;
        if (m_buffer >= m_bufferEnd) {
            m_buffer = m_inputFile.nextBlock(m_buffer);
        }
    }
    if (*m_buffer > ' ') {
        m_name = m_name + *m_buffer;
        m_buffer++;
        while (*m_buffer > ' ') {
            m_name = m_name + *m_buffer;
            m_buffer++;
            if (m_buffer >= m_bufferEnd) {
                m_buffer = m_inputFile.nextBlock(m_buffer);
            }
        }
    }
    while (*m_buffer < ' ') {
        m_buffer++;
        if (m_buffer >= m_bufferEnd) {
            m_buffer = m_inputFile.nextBlock(m_buffer);
        }
    }
    m_currentLine++;
}

std::string MpsModelBuilder::getName(MpsName name) const {
    char cname[9] = {0};
    panOptMemcpy(cname, name.m_chars, 8);
    return std::string(cname);
}

void MpsModelBuilder::readRows() {

    //const unsigned int recordCount =
    //        (m_bufferEnd - m_buffer) / (sizeof(MpsRowRecord) + 2);

    bool wrong;
    m_rows.m_listSize = 1;
    m_rows.m_lastSize = 10;

    std::vector<Row> * lastVector;
    m_rows.m_lastCounter = 0;
    m_rows.m_totalCounter = 0;
    lastVector = new std::vector<Row>;
    lastVector->resize(m_rows.m_lastSize);

    m_rows.m_rows.push_back(lastVector);


    do {
        do {
            do {

                // suppose that the records are correct!
                const MpsRowRecord * record = reinterpret_cast<const MpsRowRecord *>(m_buffer);
                // check
                wrong = (record->m_type.m_bits & 0xFFFF00FF) != 0x20200020;

                CHECK_NAME_FULL(record->m_name.m_bits, wrong);

                char rowType = record->m_type.m_chars[1];
                if (unlikely(rowType != 'N' && rowType != 'G' && rowType != 'L' && rowType != 'E'
                             && record->m_type.m_chars[0] == ' ')) {
                    //invalidRowType(rowType, m_currentLine);
                    wrong = true;
                }

                if (unlikely(rowType == 'N' && m_costVectorName.m_bits == 0 && !wrong)) {
                    m_costVectorName = record->m_name;
                    m_currentLine++;
                    m_buffer += sizeof(MpsRowType) + sizeof(MpsName);
                    m_buffer += record->m_endOfLine[0] < ' ';
                    m_buffer += record->m_endOfLine[1] < ' ';
                    //LPINFO("COST VECTOR NEVE ELMENTVE");
                } else if (likely(!wrong)) {
                    //LPINFO("SOR ELMENTVE");
                    Row newRow;
                    newRow.m_name = record->m_name;
                    newRow.m_rhs = 0.0;
                    newRow.m_range = Numerical::Infinity;
                    newRow.m_index = m_rows.m_totalCounter;
                    newRow.m_line = m_currentLine;
                    newRow.m_rowType = record->m_type.m_chars[1];
                    (*lastVector)[ m_rows.m_lastCounter ] = newRow;

                    m_rows.m_totalCounter++;
                    m_rows.m_lastCounter++;
                    m_buffer += sizeof(MpsRowType) + sizeof(MpsName);
                    m_buffer += record->m_endOfLine[0] < ' ';
                    m_buffer += record->m_endOfLine[1] < ' ';
                    m_currentLine++;
                }

                //LPINFO("------------------------------- " << getName(record->m_name));
                //cin.get();
            } while (!wrong && m_rows.m_lastCounter < m_rows.m_lastSize && (m_buffer + 80) < m_bufferEnd);
            if (m_buffer + 80 >= m_bufferEnd) {
                m_buffer = m_inputFile.nextBlock(m_buffer);
            }
            if (m_rows.m_lastCounter >= m_rows.m_lastSize) {
                m_rows.m_lastSize *= 2;
                m_rows.m_listSize++;
                m_rows.m_lastCounter = 0;
                lastVector = new std::vector<Row>;
                lastVector->resize(m_rows.m_lastSize);

                m_rows.m_rows.push_back(lastVector);

            }
        } while (wrong == false);

        if (*m_buffer == ' ') {
            //showBuffer(60, false);
            //LPINFO("ROSSSSZ");
            //cin.get();
            char type;
            MpsName name;
            name.m_bits = 0;
            while (*m_buffer == ' ') {
                m_buffer++;
            }
            type = *m_buffer;
            m_buffer++;
            while (*m_buffer == ' ') {
                m_buffer++;
            }
            unsigned int nameIndex = 0;
            while (nameIndex < 8 && *m_buffer >= ' ') {
                name.m_chars[nameIndex] = *m_buffer;
                nameIndex++;
                m_buffer++;
            }
            m_currentLine++;
            while (nameIndex < 8) {
                name.m_chars[nameIndex] = ' ';
                nameIndex++;
            }
            Row newRow;
            newRow.m_name = name;
            newRow.m_rhs = 0.0;
            newRow.m_range = Numerical::Infinity;
            newRow.m_index = m_rows.m_totalCounter;
            newRow.m_line = m_currentLine;
            newRow.m_rowType = type;

            if (unlikely(type != 'N' && type != 'G' && type != 'L' && type != 'E')) {
                invalidRowType(type, m_currentLine);
            } else {

                if (unlikely(type == 'N' && m_costVectorName.m_bits == 0)) {
                    m_costVectorName = name;
                } else {

                    (*lastVector)[ m_rows.m_lastCounter ] = newRow;

                    m_rows.m_lastCounter++;
                    m_rows.m_totalCounter++;
                    if (m_rows.m_lastCounter >= m_rows.m_lastSize) {
                        m_rows.m_lastSize *= 2;
                        m_rows.m_listSize++;
                        lastVector = new std::vector<Row>;
                        lastVector->resize(m_rows.m_lastSize);

                        m_rows.m_rows.push_back(lastVector);
                        m_rows.m_lastCounter = 0;
                    }
                }
            }

            skipEndLine();

        }
    } while (*m_buffer == ' ');

}

void MpsModelBuilder::finishRows() {
    m_rowsMap.resize( m_rows.m_totalCounter, 0 );
    unsigned int minus = 0;
    m_rowsTable.setSize( m_rows.m_totalCounter );
    std::list<std::vector<Row>* >::iterator listIter = m_rows.m_rows.begin();
    std::list<std::vector<Row>* >::iterator listIterEnd = m_rows.m_rows.end();

    RowIndex rowIndex;
    listIterEnd--;
    for (; listIter != listIterEnd; listIter++) {
        std::vector<Row>::iterator nameIter = (*listIter)->begin();
        std::vector<Row>::iterator nameIterEnd = (*listIter)->end();

        for (; nameIter != nameIterEnd; nameIter++) {
            rowIndex.m_name = nameIter->m_name;
            rowIndex.m_row = &(*nameIter);
            rowIndex.m_row->m_line -= minus;

            m_rowsMap[ rowIndex.m_row->m_index ] = rowIndex.m_row;
            if (unlikely(m_rowsTable.addUnique( rowIndex ) == false)) {
                rowExists(rowIndex);
                minus++;
            }
        }
    }
    std::vector<Row>::iterator nameIter = (*listIterEnd)->begin();
    unsigned int index = 0;
    for (; index < m_rows.m_lastCounter; index++, nameIter++) {
        rowIndex.m_name = nameIter->m_name;
        rowIndex.m_row = &(*nameIter);
        rowIndex.m_row->m_line -= minus;
        m_rowsMap[ rowIndex.m_row->m_index ] = rowIndex.m_row;
        if (unlikely(m_rowsTable.addUnique( rowIndex ) == false)) {
            rowExists(rowIndex);
            minus++;
        }
    }
}

double MpsModelBuilder::parseDouble(const char * ptr) {
    enum NUMBER_PART
    {
        INTEGER, FRACTION, EXPONENT
    } state = INTEGER;

    //val = 0;
    //const char * originalStr = ptr;
#ifdef CHECK_PARSE_DOUBLE
    Numerical::Double refValue = atof(ptr);
#endif
    unsigned int index = 0;
    while (*ptr == ' ') {
        ptr++;
    }
    char ch = *ptr;

    bool negExponent = false;
    bool negMantissa = ch == '-';
    if (negMantissa || ch == '+') {
        ptr++;
        ch = *ptr;
        index++;
    }
    long long int mul = 1;
    //Numerical::Double mulchg = 10.0;
    int exponent1 = 0;
    int exponent2 = 0;
    long long int mantissa = 0;
    //unsigned long long int fraction = 0;
    bool invalidNumber = false;

    while (ch > ' ' && index < MPS_FIELD_3_LENGTH && !invalidNumber) {
        switch (state) {
        case INTEGER:
            if (ch >= '0' && ch <= '9') {
                mantissa *= 10;
                mantissa += ch - '0';
            } else if (ch == '.') {
                state = FRACTION;
            } else if (ch == 'E' || ch == 'e' || ch == 'D' || ch == 'd') {
                char ch2 = ptr[1];
                if (ch2 == '+' || ch2 == '-') {
                    negExponent = ch2 == '-';
                    ptr++;
                    ch = *ptr;
                    index++;
                }
                state = EXPONENT;
            } else {
                invalidNumber = true;
            }
            break;
        case FRACTION:
            if (ch >= '0' && ch <= '9') {
                exponent1++;
                mantissa *= 10;
                mantissa += (ch - '0');
                //exponent1++;
                //                    mul /= 10.0;
            } else if (ch == 'E' || ch == 'e' || ch == 'D' || ch == 'd') {
                char ch2 = ptr[1];
                if (ch2 == '+' || ch2 == '-') {
                    negExponent = ch2 == '-';
                    ptr++;
                    ch = *ptr;
                    index++;
                }
                state = EXPONENT;
            } else {
                invalidNumber = true;
            }
            break;
        case EXPONENT:
            if (ch >= '0' && ch <= '9') {
                exponent2 *= 10;
                exponent2 += ch - '0';
            } else {
                invalidNumber = true;
            }
            break;
        }
        ptr++;
        ch = *ptr;
        index++;
    }
    if (negExponent) {
        exponent2 = -exponent2;
    }
    if (negMantissa) {
        mantissa = -mantissa;
    }
    int finalExponent = exponent2 - exponent1;

    //if (m_line == 257643) {
    //    std::cout << "eleje" << std::endl;
    //    LPINFO("mantissa: " << mantissa);
    //    LPINFO("exponent1: -" << exponent1);
    //    LPINFO("exponent2: " << exponent2);
    //    LPINFO("final exponent: " << finalExponent);
    //    LPINFO("negExponent: " << negExponent);
    //    LPINFO("negMantissa: " << negMantissa);
    //    LPINFO("mul: " << mul);

    if (finalExponent > 0) {
        double result = mantissa;
        mul = 1;
        while (finalExponent >= 16) {
            finalExponent -= 16;
            result *= 10000000000000000LL;
        }
        while (finalExponent >= 8) {
            mul *= 100000000LL;
            finalExponent -= 8;
            if (mul >= 10e20) {
                result *= mul;
                mul = 1;
            }
        }
        while (finalExponent >= 4) {
            mul *= 10000LL;
            finalExponent -= 4;
            if (mul >= 10e20) {
                result *= mul;
                mul = 1;
            }
        }
        while (finalExponent >= 2) {
            mul *= 100LL;
            finalExponent -= 2;
            if (mul >= 10e20) {
                result *= mul;
                mul = 1;
            }
        }
        while (finalExponent > 0) {
            mul *= 10LL;
            finalExponent--;

            if (mul >= 10e20) {
                result *= mul;
                mul = 1;
            }
        }
        result *= mul;
        return result;
    } else if (finalExponent < 0) {
        double result = mantissa;
        mul = 1;
        while (finalExponent <= -16) {
            finalExponent += 16;
            result /= 10000000000000000LL;
        }
        while (finalExponent <= -8) {
            mul *= 100000000LL;
            finalExponent += 8;
            if (mul >= 10e20) {
                result /= mul;
                mul = 1;
            }
        }
        while (finalExponent <= -4) {
            mul *= 10000LL;
            finalExponent += 4;
            if (mul >= 10e20) {
                result /= mul;
                mul = 1;
            }
        }
        while (finalExponent <= -2) {
            mul *= 100LL;
            finalExponent += 2;
            if (mul >= 10e20) {
                result /= mul;
                mul = 1;
            }
        }
        while (finalExponent < 0) {
            mul *= 10LL;
            finalExponent++;

            if (mul >= 10e20) {
                result /= mul;
                mul = 1;
            }
        }
        result /= mul;
        return result;
    } else {
        return mantissa;
    }

#ifdef CHECK_PARSE_DOUBLE
    //if ((*val) != refValue) {
    if (!Numerical::rigidEqualsFloat64(*val, refValue)) {
        LPINFO(this->m_line);
        std::cerr.precision(14);
        DEVINFO(D::MPSREADER, "error: *val = " << *val << ", refValue = " << refValue);
        DEVINFO(D::MPSREADER, "difference: " << refValue - *val);
        LPERROR("error: *val = " << *val << ", refValue = " << refValue);
        LPERROR("difference: " << refValue - *val);
        Numerical::Float64 val1, val2;
        val1.m_value = *val;
        val2.m_value = refValue;

        DEVINFO(D::MPSREADER, "sign1: " << val1.m_bits.m_sign);
        DEVINFO(D::MPSREADER, "exponent1: " << val1.m_bits.m_exponent);
        DEVINFO(D::MPSREADER, "mantissa1: " << val1.m_bits.m_mantissa);
        DEVINFO(D::MPSREADER, "sign2: " << val2.m_bits.m_sign);
        DEVINFO(D::MPSREADER, "exponent2: " << val2.m_bits.m_exponent);
        DEVINFO(D::MPSREADER, "mantissa2: " << val2.m_bits.m_mantissa);
        DEVINFO(D::MPSREADER, "mantissa diference: " << (val2.m_bits.m_mantissa ^ val1.m_bits.m_mantissa));
        DEVINFO(D::MPSREADER, "line: " << m_line);

        LPERROR("sign1: " << val1.m_bits.m_sign);
        LPERROR("exponent1: " << val1.m_bits.m_exponent);
        LPERROR("mantissa1: " << val1.m_bits.m_mantissa);
        LPERROR("sign2: " << val2.m_bits.m_sign);
        LPERROR("exponent2: " << val2.m_bits.m_exponent);
        LPERROR("mantissa2: " << val2.m_bits.m_mantissa);
        LPERROR("mantissa diference: " << (val2.m_bits.m_mantissa ^ val1.m_bits.m_mantissa));
        LPERROR("line: " << m_line);

        std::cin.get();
    } else {
    }
#endif
    return 0.0;
}

void MpsModelBuilder::saveColumn(Column * actualColumnPtr,
                                 std::vector<Column> ** lastVectorPtr,
                                 bool *columnPattern) {
    Column & actualColumn = *actualColumnPtr;
    std::vector<Column> * & lastVector = *lastVectorPtr;
    (*lastVector)[ m_columns.m_lastCounter ] = actualColumn;

    ////////////////////////////////////////////////////
    // Column * column = &(*lastVector)[ m_columns.m_lastCounter ];
    // LPINFO( *(column->m_beginIndexIter) );
    // std::list<unsigned int*>::const_iterator indexIter0 = column->m_beginIndexIter;
    // LPINFO( *indexIter0 );
    ////////////////////////////////////////////////////

    m_columns.m_totalCounter++;
    m_columns.m_lastCounter++;


    if (m_columns.m_lastCounter >= m_columns.m_lastSize) {
        m_columns.m_lastSize *= 2;
        m_columns.m_listSize++;
        m_columns.m_lastCounter = 0;
        lastVector = new std::vector<Column>;
        lastVector->resize(m_columns.m_lastSize);

        m_columns.m_columns.push_back(lastVector);

    }
    unsigned int * indexPtr = actualColumn.m_firstIndex;
    std::list<unsigned int*>::const_iterator indexIter = actualColumn.m_beginIndexIter;
    unsigned int * endPtr = *indexIter + sm_valueBufferSize;
    unsigned int counter;
    for (counter = 0; counter < actualColumn.m_nonzeros; counter++) {
        columnPattern[ *indexPtr ] = false;
        indexPtr++;
        if (indexPtr >= endPtr) {
            indexIter++;
            indexPtr = *indexIter;
            endPtr = *indexIter + sm_valueBufferSize;
        }
    }

    actualColumn.m_firstValue = m_actualValue;
    actualColumn.m_firstIndex = m_actualIndex;
    actualColumn.m_beginValueIter = m_values.end();
    actualColumn.m_beginIndexIter = m_indices.end();
    actualColumn.m_beginValueIter--;
    actualColumn.m_beginIndexIter--;
    actualColumn.m_nonzeros = 0;

    actualColumn.m_index++;

}

bool MpsModelBuilder::parseRecordStart(double * value,
                                       MpsName * id1,
                                       MpsName * id2,
                                       EmptySection4 * startSection,
                                       bool * newLine) {
    unsigned int index;
    char number[MPS_FIELD_3_LENGTH + 1] = {0};
    bool wrong = false;

    if (unlikely(*m_buffer != ' ')) {
        return false;
    }

    const MpsColumnRecordFirst * record = reinterpret_cast<const MpsColumnRecordFirst *>(m_buffer);
    m_buffer += sizeof(MpsColumnRecordFirst);

    for (index = 0; index < MPS_FIELD_3_LENGTH && m_buffer[index] >= ' '; index++) {
        number[index] = m_buffer[index];
    }
    m_buffer += index;
    for (; index < MPS_FIELD_3_LENGTH; index++) {
        number[index] = ' ';
    }

    *startSection = record->m_spaces1;

    wrong = wrong || record->m_spaces2.m_bits != 0x2020;

    CHECK_NAME_FULL(record->m_columnName.m_bits, wrong);
    *id1 = record->m_columnName;

    unsigned int spaces = 0;
    while (spaces < 3 && m_buffer[spaces] == ' ') {
        spaces++;
    }
    wrong = wrong || (spaces < 3 && spaces > 0);


    /*if (wrong) {
        cout << "WRONG " << spaces << endl;
    }*/

    m_buffer += spaces;
    CHECK_NAME_FULL(record->m_rowName.m_bits, wrong);

    if (unlikely(wrong == true)) {
        return false;
    }

    *id2 = record->m_rowName;
    *value = parseDouble(number);

    spaces = 0;
    while (spaces < 2 && *m_buffer == ' ') { // TODO: tul hosszu ellen vedekezes
        m_buffer++;
        spaces++;
    }

    bool endLine = false;
    while (*m_buffer < ' ') { // TODO: tul hosszu ellen vedekezes
        m_buffer++;
        endLine = true;
    }
    *newLine = !(*m_buffer > ' ' && endLine == false && ((spaces | 0x2) == 0x2));

    return true;
}

bool MpsModelBuilder::parseBoundRecord(double * value,
                                       MpsName * id1,
                                       MpsName * id2,
                                       EmptySection4 * startSection) {
    unsigned int index;
    char number[MPS_FIELD_3_LENGTH + 1] = {0};
    bool wrong = false;

    if (unlikely(*m_buffer != ' ')) {
        return false;
    }

    const MpsColumnRecordFirst * record = reinterpret_cast<const MpsColumnRecordFirst *>(m_buffer);
    m_buffer += sizeof(MpsBoundRecord);

    *startSection = record->m_spaces1;

    wrong = wrong || record->m_spaces2.m_bits != 0x2020;

    CHECK_NAME_FULL(record->m_columnName.m_bits, wrong);
    *id1 = record->m_columnName;

    //cout << wrong << endl;

    CHECK_NAME_FULL(record->m_rowName.m_bits, wrong);
    *id2 = record->m_rowName;

    //cout << wrong << endl;

    //cout << hex << record->m_spaces1.m_bits  << " " << (record->m_spaces1.m_bits == 0x20465220) << dec << endl;

    if ( record->m_spaces1.m_bits == 0x20524620 ||  // FR
         record->m_spaces1.m_bits == 0x204D4D20 ||  // MI
         record->m_spaces1.m_bits == 0x204C5020 ||  // PL
         record->m_spaces1.m_bits == 0x20564220 ||  // BV
         record->m_spaces1.m_bits == 0x20435320) { // SC
        unsigned int spaces = 0;
        while (*m_buffer == ' ') { // TODO: tul hosszu ellen vedekezes
            m_buffer++;
            spaces++;
        }
        // TODO: ha van itt valami, akkor azt ignoralni kell
        while (*m_buffer < ' ') { // TODO: tul hosszu ellen vedekezes
            m_buffer++;
        }
        return true;
    }
    unsigned int spaces = 0;
    while (spaces < 2 && m_buffer[spaces] == ' ') {
        spaces++;
    }
    wrong = wrong || (spaces != 2);

    m_buffer += 2;

    //showBuffer(40, false);


    for (index = 0; index < MPS_FIELD_3_LENGTH && m_buffer[index] >= ' '; index++) {
        number[index] = m_buffer[index];
    }
    m_buffer += index;
    for (; index < MPS_FIELD_3_LENGTH; index++) {
        number[index] = ' ';
    }

    *value = parseDouble(number);

    //showBuffer(40, false);

    spaces = 0;
    while (m_buffer[spaces] == ' ') {
        spaces++;
    }
    //wrong = wrong || (spaces < 3 && spaces > 0);

    m_buffer += spaces;

    if (unlikely(wrong == true)) {
        return false;
    }

    //bool endLine = false;
    while (*m_buffer < ' ') { // TODO: tul hosszu ellen vedekezes
        m_buffer++;
        //    endLine = true;
    }
    //showBuffer(40, false);

    return true;
}

bool MpsModelBuilder::parseRecordEnd(double * value,
                                     MpsName * id) {
    unsigned int index;
    char number[MPS_FIELD_3_LENGTH + 1] = {0};

    const MpsColumnRecordSecond * record = reinterpret_cast<const MpsColumnRecordSecond *>(m_buffer);
    m_buffer += sizeof(MpsColumnRecordSecond);

    bool wrong = record->m_spaces.m_bits != 0x2020;

    CHECK_NAME_FULL(record->m_rowName.m_bits, wrong);

    if (unlikely(wrong == true)) {
        return false;
    }

    for (index = 0; index < MPS_FIELD_3_LENGTH && m_buffer[index] >= ' '; index++) {
        number[index] = m_buffer[index];
    }
    m_buffer += index;
    for (; index < MPS_FIELD_3_LENGTH; index++) {
        number[index] = ' ';
    }

    while (*m_buffer == ' ') {
        m_buffer++;
    }

    while (*m_buffer < ' ') {
        m_buffer++;
    }


    *id = record->m_rowName;

    *value = parseDouble(number);
    return true;
}

void MpsModelBuilder::readColumns() {

    //const unsigned int recordCount =
    //        (m_bufferEnd - m_buffer) / (sizeof(MpsColumnRecordFirst) + sizeof(MpsColumnRecordSecond));

    //unsigned int columnCount = 1;
    bool wrong;
    Column actualColumn;
    const RowIndex * rowIndexPtr1;
    const RowIndex * rowIndexPtr2;
    RowIndex rowIndex;
    double value1 = 0.0;
    double value2 = 0.0;
    actualColumn.m_name.m_bits = 0;
    actualColumn.m_lowerBound = 0.0;
    actualColumn.m_isInteger = false;
    actualColumn.m_upperBound = 1.0 / 0.0; // TODO: majd kicserelni vegtelenre

    bool * columnPattern = new bool[ m_rows.m_totalCounter ];
    unsigned int index;
    for (index = 0; index < m_rows.m_totalCounter; index++) {
        columnPattern[index] = false;
    }

    ////////////////////////////////////////////////////////////////////////////////

    m_columns.m_listSize = 1;
    m_columns.m_lastSize = 10;

    std::vector<Column> * lastVector;
    m_columns.m_lastCounter = 0;
    m_columns.m_totalCounter = 0;
    actualColumn.m_index = 0;
    lastVector = new std::vector<Column>;
    lastVector->resize(m_columns.m_lastSize);

    m_columns.m_columns.push_back(lastVector);

    double * values = alloc<double, 64>( sm_valueBufferSize );
    unsigned int * indices = alloc<unsigned int, 64>( sm_valueBufferSize );

    m_actualValue = values;
    m_actualValueEnd = values + sm_valueBufferSize;
    m_actualIndex = indices;
    m_actualIndexEnd = indices + sm_valueBufferSize;

    m_values.push_back(values);
    m_indices.push_back(indices);

    actualColumn.m_firstValue = m_actualValue;
    actualColumn.m_firstIndex = m_actualIndex;
    actualColumn.m_beginValueIter = m_values.begin();
    actualColumn.m_beginIndexIter = m_indices.begin();
    actualColumn.m_nonzeros = 0;

    MpsName columnName;
    bool endLine;
    EmptySection4 startSection;

    do {
        do {
            do {

                // showBuffer(64, false);

                // suppose that the records are correct!
                // read column data here

                const char * originalBuffer = m_buffer;

                wrong = false;
                rowIndexPtr1 = 0;
                rowIndexPtr2 = 0;
                if (likely(parseRecordStart(&value1, &columnName, &rowIndex.m_name, &startSection, &endLine) == true)) {

                    if (unlikely(startSection.m_bits != 0x20202020)) {
                        wrong = true;
                    }

                    if (unlikely(actualColumn.m_name.m_bits == 0)) {
                        actualColumn.m_line = m_currentLine;
                    }

                    if (unlikely(actualColumn.m_name.m_bits != 0 && columnName.m_bits != actualColumn.m_name.m_bits && !wrong)) {
                        saveColumn(&actualColumn, &lastVector, columnPattern);
                        actualColumn.m_line = m_currentLine;
                    }
                    actualColumn.m_name = columnName;

                    bool cost1 = false;
                    bool cost2 = false;
                    rowIndexPtr1 = m_rowsTable.get( rowIndex );

                    if (unlikely(rowIndexPtr1 == 0)) {
                        if (likely(m_costVectorName.m_bits == rowIndex.m_name.m_bits)) {
                            m_costVector.push_back( std::make_pair( value1, actualColumn.m_index ) );
                            cost1 = true;
                        } else {

                        }
                        // TODO: error
                    }

                    if (endLine == false) {
                        if (likely(parseRecordEnd(&value2, &rowIndex.m_name))) {
                            rowIndexPtr2 = m_rowsTable.get( rowIndex );
                            if (unlikely(rowIndexPtr2 == 0)) {
                                if (likely(m_costVectorName.m_bits == rowIndex.m_name.m_bits)) {
                                    m_costVector.push_back( std::make_pair( value2, actualColumn.m_index ) );
                                    cost2 = true;

                                } else {

                                }
                            }

                        } else {
                            wrong = true;
                        }
                    }

                    if (likely(!wrong)) {
                        if (likely(!cost1)) {
                            actualColumn.m_nonzeros++;
                            *m_actualValue = value1;
                            m_actualValue++;
                            *m_actualIndex = rowIndexPtr1->m_row->m_index;
                            m_actualIndex++;
                            if (unlikely(m_actualValue >= m_actualValueEnd)) {
                                values = alloc<double, 64>( sm_valueBufferSize );
                                indices = alloc<unsigned int, 64>( sm_valueBufferSize );
//                                m_doublePointers.push_back(values);
//                                m_intPointers.push_back(indices);

                                m_values.push_back(values);
                                m_indices.push_back(indices);
                                m_actualValue = values;
                                m_actualValueEnd = values + sm_valueBufferSize;
                                m_actualIndex = indices;
                                m_actualIndexEnd = indices + sm_valueBufferSize;
                            }
                            columnPattern[ rowIndexPtr1->m_row->m_index ] = true;
                        }
                        if (rowIndexPtr2 != 0 && !cost2) {
                            actualColumn.m_nonzeros++;
                            *m_actualValue = value2;
                            m_actualValue++;
                            *m_actualIndex = rowIndexPtr2->m_row->m_index;
                            m_actualIndex++;
                            if (unlikely(m_actualValue >= m_actualValueEnd)) {
                                values = alloc<double, 64>( sm_valueBufferSize );
                                indices = alloc<unsigned int, 64>( sm_valueBufferSize );
                                m_values.push_back(values);
                                m_indices.push_back(indices);
                                m_actualValue = values;
                                m_actualValueEnd = values + sm_valueBufferSize;
                                m_actualIndex = indices;
                                m_actualIndexEnd = indices + sm_valueBufferSize;
                            }

                            columnPattern[ rowIndexPtr2->m_row->m_index ] = true;
                        }
                        m_currentLine++;
                    } else {
                        if (unlikely(wrong)) {
                            wrong = true;
                            m_buffer = originalBuffer;
                        }
                    }

                } else {
                    wrong = true;
                    m_buffer = originalBuffer;
                }

            } while (!wrong && (m_buffer + 80) < m_bufferEnd);
            if (m_buffer + 80 >= m_bufferEnd) {
                m_buffer = m_inputFile.nextBlock(m_buffer);
            }

        } while (wrong == false);

        if (*m_buffer == ' ') {

            skipEndLine();
        }
    } while (*m_buffer == ' ');

    saveColumn(&actualColumn, &lastVector, columnPattern);
    delete [] columnPattern;
}

void MpsModelBuilder::finishColumns() {

    m_columnsMap.resize( m_columns.m_totalCounter, 0 );

    m_columnsTable.setSize( m_columns.m_totalCounter );
    std::list<std::vector<Column>* >::iterator listIter = m_columns.m_columns.begin();
    std::list<std::vector<Column>* >::iterator listIterEnd = m_columns.m_columns.end();

    ColumnIndex columnIndex;
    listIterEnd--;
    for (; listIter != listIterEnd; listIter++) {
        std::vector<Column>::iterator nameIter = (*listIter)->begin();
        std::vector<Column>::iterator nameIterEnd = (*listIter)->end();

        for (; nameIter != nameIterEnd; nameIter++) {
            columnIndex.m_name = nameIter->m_name;
            columnIndex.m_column = &(*nameIter);

            m_columnsMap[ columnIndex.m_column->m_index ] = columnIndex.m_column;
            if (m_columnsTable.addUnique( columnIndex ) == false) {
                // TODO: ismetlodo sor, feljegyezni, kezelni
            }
        }
    }
    std::vector<Column>::iterator nameIter = (*listIterEnd)->begin();
    unsigned int index = 0;
    for (; index < m_columns.m_lastCounter; index++, nameIter++) {
        columnIndex.m_name = nameIter->m_name;
        columnIndex.m_column = &(*nameIter);
        m_columnsMap[ columnIndex.m_column->m_index ] = columnIndex.m_column;
        if (m_columnsTable.addUnique( columnIndex ) == false) {
            // TODO: ismetlodo sor, feljegyezni, kezelni
        }
    }

}

void MpsModelBuilder::readRhsOrRanges(Numerical::Double Row::*range) {
    MpsName rhsName;
    MpsName currentRhsName;
    const RowIndex * rowIndexPtr1;
    const RowIndex * rowIndexPtr2;
    RowIndex rowIndex;
    double value1 = 0.0;
    double value2 = 0.0;
    rhsName.m_bits = 0;

    // to prevent multiple values
    bool * columnPattern = new bool[ m_rows.m_totalCounter ];
    unsigned int index;
    for (index = 0; index < m_rows.m_totalCounter; index++) {
        columnPattern[index] = false;
    }

    bool endLine;
    EmptySection4 startSection;
    bool wrong;

    do {
        do {
            do {

                // showBuffer(10, false);

                // suppose that the records are correct!
                // read column data here

                const char * originalBuffer = m_buffer;

                wrong = false;
                rowIndexPtr1 = 0;
                rowIndexPtr2 = 0;
                if (likely(parseRecordStart(&value1, &currentRhsName, &rowIndex.m_name, &startSection, &endLine) == true)) {

                    if (unlikely(rhsName.m_bits == 0)) {
                        rhsName = currentRhsName;
                    }

                    bool cost1 = false;
                    bool cost2 = false;
                    rowIndexPtr1 = m_rowsTable.get( rowIndex );
                    if (unlikely(rowIndexPtr1 == 0)) {
                        if (likely(m_costVectorName.m_bits == rowIndex.m_name.m_bits)) {
                            m_costConstant = value1;
                            cost1 = true;
                            // TODO: majd ellenorizni, hogy ranges-nel ne legyen,
                            // es hogy nem ismetlodik-e
                        } else {
                            // TODO: error
                        }
                    }

                    if (endLine == false) {
                        if (likely(parseRecordEnd(&value2, &rowIndex.m_name))) {
                            rowIndexPtr2 = m_rowsTable.get( rowIndex );
                            if (unlikely(rowIndexPtr2 == 0)) {
                                if (likely(m_costVectorName.m_bits == rowIndex.m_name.m_bits)) {
                                    m_costConstant = value2;
                                    cost2 = true;
                                    // TODO: majd ellenorizni, hogy ranges-nel ne legyen,
                                    // es hogy nem ismetlodik-e
                                } else {
                                    // TODO: error
                                }
                            }

                        } else {
                            wrong = true;
                        }
                    }

                    if (unlikely(startSection.m_bits != 0x20202020)) {
                        wrong = true;
                    }

                    if (likely(!wrong)) {
                        if (likely(!cost1)) {
                            rowIndexPtr1->m_row->*range = value1;
                            unsigned int index = rowIndexPtr1->m_row->m_index;
                            if (unlikely(columnPattern[ index ] == true)) {
                                // TODO: hibat jelezni
                            }
                            columnPattern[ index ] = true;
                        }
                        if (rowIndexPtr2 != 0 && !cost2) {
                            rowIndexPtr2->m_row->*range = value2;
                            index = rowIndexPtr2->m_row->m_index;
                            if (unlikely(columnPattern[ index ] == true)) {
                                // TODO: hibat jelezni
                            }
                            columnPattern[ index ] = true;
                        }
                        m_currentLine++;
                    } else {
                        if (unlikely(wrong)) {
                            wrong = true;
                            m_buffer = originalBuffer;
                        }
                    }

                } else {
                    wrong = true;
                    m_buffer = originalBuffer;
                }

            } while (!wrong && (m_buffer + 80) < m_bufferEnd);
            if (m_buffer + 80 >= m_bufferEnd) {
                m_buffer = m_inputFile.nextBlock(m_buffer);
            }

        } while (wrong == false);

        if (*m_buffer == ' ') {

            skipEndLine();
        }
    } while (*m_buffer == ' ');

    delete [] columnPattern;
}

void MpsModelBuilder::readBounds() {
    bool wrong;
    MpsName boundName;
    MpsName currentBoundName;
    const ColumnIndex * columnIndexPtr;
    ColumnIndex columnIndex;
    double value = 0.0;
    boundName.m_bits = 0;

    // to prevent multiple rhs values
    char * boundsPattern = new char[ m_columns.m_totalCounter ];
    unsigned int index;
    for (index = 0; index < m_columns.m_totalCounter; index++) {
        boundsPattern[index] = 0;
    }

    ////////////////////////////////////////////////////////////////////////////////

    // bool endLine;
    EmptySection4 startSection;

    do {
        do {
            do {
                //showBuffer(64, false);

                // suppose that the records are correct!
                // read column data here

                const char * originalBuffer = m_buffer;

                wrong = false;
                columnIndexPtr = 0;

                if (likely(parseBoundRecord(&value, &currentBoundName, &columnIndex.m_name, &startSection) == true)) {

                    if (unlikely(boundName.m_bits == 0)) {
                        boundName = currentBoundName;
                    }

                    bool shortName = false;
                    CHECK_NAME_FULL(columnIndex.m_name.m_bits, shortName);
                    if (unlikely(shortName == true)) {
                        unsigned int index = 0;
                        while (index < 8 && columnIndex.m_name.m_chars[index] >= ' ') {
                            index++;
                        }
                        for (; index < 8; index++) {
                            columnIndex.m_name.m_chars[index] = ' ';
                        }
                        m_buffer = originalBuffer + 1 + 2 + 1 + 8 + 2;
                        while (*m_buffer >= ' ') {
                            m_buffer++;
                        }
                        while (*m_buffer < ' ') {
                            m_buffer++;
                        }
                    }
                    columnIndexPtr = m_columnsTable.get( columnIndex );
                    if (unlikely(columnIndexPtr == 0)) {
                        LPERROR("Column not exists: " << getName(columnIndex.m_name));
                        // TODO: error
                    }

                    if (unlikely((startSection.m_bits & 0xFF0000FF) != 0x20000020)) {
                        wrong = true;
                    }
                    if (likely(!wrong)) {

                        Column * actualColumn = columnIndexPtr->m_column;
                        //rowIndexPtr1->m_row->m_rhs = value1;
                        unsigned int index = actualColumn->m_index;

                        switch ( startSection.m_bits ) {
                        case 0x204F4C20: // LO
                            actualColumn->m_lowerBound = value;
                            boundsPattern[index] += 1;
                            break;
                        case 0x20505520: // UP
                            actualColumn->m_upperBound = value;
                            boundsPattern[index] += 16;
                            break;
                        case 0x20584620: // FX
                            actualColumn->m_lowerBound = value;
                            actualColumn->m_upperBound = value;
                            boundsPattern[index] += 1;
                            boundsPattern[index] += 16;
                            break;
                        case 0x20524620: // FR
                            actualColumn->m_lowerBound = -1.0 / 0.0; // TODO: atirni
                            actualColumn->m_upperBound = 1.0 / 0.0;
                            boundsPattern[index] += 1;
                            boundsPattern[index] += 16;
                            break;
                        case 0x20494D20: // MI
                            actualColumn->m_lowerBound = -1.0 / 0.0; // TODO: atirni
                            boundsPattern[index] += 1;
                            break;
                        case 0x204C5020: // PL
                            actualColumn->m_upperBound = 1.0 / 0.0; // TODO: atirni
                            boundsPattern[index] += 16;
                            break;
                        case 0x20564220: // BV
                            actualColumn->m_lowerBound = 0.0;
                            actualColumn->m_upperBound = 1.0;
                            actualColumn->m_isInteger = true;
                            boundsPattern[index] += 1;
                            boundsPattern[index] += 16;
                            break;
                        case 0x20494C20 : // LI
                            actualColumn->m_lowerBound = value;
                            actualColumn->m_isInteger = true;
                            boundsPattern[index] += 1;
                            break;
                        case 0x20495520 : // UI
                            actualColumn->m_upperBound = value;
                            actualColumn->m_isInteger = true;
                            boundsPattern[index] += 16;
                            break;
                        case 0x20435320 : // SC
                            // TODO: utana nezni hogy mi ez
                            break;
                        default:
                            break;
                        }

                        if (unlikely(boundsPattern[ index ] >= 32)) {
                            // TODO: hibat jelezni, felso korlatot tobbszor adtuk meg
                            boundsPattern[ index ] -= 16;
                        }
                        if (unlikely((boundsPattern[ index ] & 0x3) >= 2)) {
                            // TODO: hibat jelezni, also korlatot tobbszor adtuk meg
                            boundsPattern[ index ] -= 1;
                        }
                        m_currentLine++;

                    } else {
                        wrong = true;
                        m_buffer = originalBuffer;
                    }

                } else {
                    wrong = true;
                    m_buffer = originalBuffer;
                }

            } while (!wrong && (m_buffer + 80) < m_bufferEnd);
            if (m_buffer + 80 >= m_bufferEnd) {
                m_buffer = m_inputFile.nextBlock(m_buffer);
            }

        } while (wrong == false);

        if (*m_buffer == ' ') {
            skipEndLine();
        }
    } while (*m_buffer == ' ');
    delete [] boundsPattern;
}

void MpsModelBuilder::skipSection() {

    if (m_buffer >= m_bufferEnd) {
        m_buffer = m_inputFile.nextBlock(m_buffer);
    }

    do {

        while (*m_buffer >= ' ') {
            m_buffer++;
            if (m_buffer >= m_bufferEnd) {
                m_buffer = m_inputFile.nextBlock(m_buffer);
            }
        }
        skipEndLine();

    } while (*m_buffer <= ' ');
    // TODO: kezelni ha hirtelen veget er a szekcio, akkor hiba
}

void MpsModelBuilder::invalidRowType(char type, unsigned int line) {
    std::string message;
    CREATE_STRING(message, "Invalid row type: '" << type << "'");
    m_invalidRowType.add(message, line);
}

void MpsModelBuilder::invalidRowName(const std::string & name, unsigned int line) {
    std::string message;
    CREATE_STRING(message, "Invalid row name: " << name);
    m_invalidRowName.add(message, line);
}

void MpsModelBuilder::rowExists(const RowIndex & row) {
    const RowIndex * rowPtr = m_rowsTable.get(row);
    // store the error message
    std::string message;
    CREATE_STRING(message, "The row " <<
                  getName(row.m_name) <<
                  " has defined in line " << rowPtr->m_row->m_line);
    m_rowExists.add(message, row.m_row->m_line);

}

void MpsModelBuilder::loadFromFile(const std::string & fileName) {

    clear();

    m_invalidRowType.init(m_errorPrintLimit, m_levelCounters);
    m_invalidRowName.init(m_errorPrintLimit, m_levelCounters);
    m_rowExists.init(m_errorPrintLimit, m_levelCounters);

    m_invalidColumnName.init(m_errorPrintLimit, m_levelCounters);
    m_invalidStartField.init(m_errorPrintLimit, m_levelCounters);
    m_invalidEndField.init(m_errorPrintLimit, m_levelCounters);
    m_unnecessaryContent.init(m_errorPrintLimit, m_levelCounters);
    m_tooLongWord.init(m_errorPrintLimit, m_levelCounters);
    m_missingComment.init(m_errorPrintLimit, m_levelCounters);
    m_columnExists.init(m_errorPrintLimit, m_levelCounters);
    m_incompleteRecord.init(m_errorPrintLimit, m_levelCounters);
    m_tooLongRecord.init(m_errorPrintLimit, m_levelCounters);
    m_invalidBoundType.init(m_errorPrintLimit, m_levelCounters);
    m_nameMissing.init(m_errorPrintLimit, m_levelCounters);
    m_rowsMissing.init(m_errorPrintLimit, m_levelCounters);
    m_columnsMissing.init(m_errorPrintLimit, m_levelCounters);
    m_endataMissing.init(m_errorPrintLimit, m_levelCounters);
    m_boundRepeated.init(m_errorPrintLimit, m_levelCounters);
    m_invalidComment.init(m_errorPrintLimit, m_levelCounters);
    m_invalidNumberFormat.init(m_errorPrintLimit, m_levelCounters);
    m_unknownSectionName.init(m_errorPrintLimit, m_levelCounters);
    m_valueRepeated.init(m_errorPrintLimit, m_levelCounters);
    m_sectionAlreadyProcessed.init(m_errorPrintLimit, m_levelCounters);

    m_currentLine = 0;
    m_inputFile.openForRead(fileName.c_str(), m_bufferSize, 100);

    m_buffer = m_inputFile.getBuffer();
    m_bufferEnd = m_buffer + m_bufferSize;

    // skip comments and empty records
    while (skipComments()) {}

    while (readSectionType() != MPS_NAME) { // TODO: vagy a vegere ertunk

    }

    m_section = MPS_NAME;

    readName();

    while (skipComments()) {}

    while (readSectionType() != MPS_ROWS) { // TODO: vagy a vegere ertunk
        skipSection();
    }

    skipEndLine();

    m_section = MPS_ROWS;

    readRows();

    finishRows();

    while (skipComments()) {}

    while (readSectionType() != MPS_COLUMNS) { // TODO: vagy a vegere ertunk
        skipSection();
    }

    skipEndLine();

    readColumns();

    finishColumns();

    while (skipComments()) {}

    MPS_SECTION section;

    do {
        section = readSectionType();

        while (section != MPS_RHS &&
               section != MPS_RANGES &&
               section != MPS_BOUNDS &&
               section != MPS_ENDATA &&
               section != MPS_UNKNOWN) { // TODO: vagy a vegere ertunk
            skipSection();
            section = readSectionType();
        }
        if (section != MPS_ENDATA) {
            skipEndLine();
        }
        if (section == MPS_RHS) {
            readRhsOrRanges(&Row::m_rhs);
        }

        if (section == MPS_RANGES) {
            readRhsOrRanges(&Row::m_range);
        }

        if (section == MPS_BOUNDS) {
            readBounds();
        }

    } while (section != MPS_ENDATA);

}

// ********************************************************
// Inherited from MatrixBuilder
// ********************************************************

unsigned int MpsModelBuilder::getColumnCount() const {
    return m_columns.m_totalCounter;
}

unsigned int MpsModelBuilder::getRowCount() const {
    return m_rows.m_totalCounter;
}

void MpsModelBuilder::buildRow(unsigned int index, Vector * rowVector,
                               std::vector<unsigned int> * nonzeros) const {
    __UNUSED(index);
    __UNUSED(rowVector);
    __UNUSED(nonzeros);
}

void MpsModelBuilder::buildColumn(unsigned int index, Vector * columnVector,
                                  std::vector<unsigned int> * nonzeros) const {

    const Column * column = m_columnsMap[index];
    columnVector->prepareForData( column->m_nonzeros, m_rows.m_totalCounter );

    if ( column->m_nonzeros == 0 ) {
        return;
    }

    unsigned int * indexPtr = column->m_firstIndex;
    double * valuePtr = column->m_firstValue;
    std::list<unsigned int*>::const_iterator indexIter = column->m_beginIndexIter;

    std::list<double*>::const_iterator valueIter = column->m_beginValueIter;
    unsigned int * endPtr = *indexIter + sm_valueBufferSize;
    unsigned int counter;
    for (counter = 0; counter < column->m_nonzeros; counter++) {
        //columnPattern[ *indexPtr ] = false;
        columnVector->newNonZero( *valuePtr, *indexPtr );

        (*nonzeros)[*indexPtr]++;
        indexPtr++;
        valuePtr++;
        if (indexPtr >= endPtr) {
            indexIter++;
            valueIter++;
            indexPtr = *indexIter;
            valuePtr = *valueIter;
            endPtr = *indexIter + sm_valueBufferSize;
        }
    }


}

bool MpsModelBuilder::hasRowwiseRepresentation() const {
    return false;
}

bool MpsModelBuilder::hasColumnwiseRepresentation() const {
    return true;
}

// ********************************************************
// Inherited from ModelBuilder
// ********************************************************

const Variable & MpsModelBuilder::getVariable(unsigned int index) const {
    const Column * column = m_columnsMap[index];
    static Variable result;
    result = Variable::createVariable(getName(column->m_name).c_str(),
                                      column->m_lowerBound,
                                      column->m_upperBound);
    return result;
}

const Constraint & MpsModelBuilder::getConstraint(unsigned int index) const {
    const Row * row = m_rowsMap[index];
    Numerical::Double lowerBound = 0.0;
    Numerical::Double upperBound = 0.0;
    Numerical::Double range = row->m_range;
    Numerical::Double rhs = row->m_rhs;

    switch ( row->m_rowType ) {
    case 'N':
        lowerBound = -Numerical::Infinity;
        upperBound = Numerical::Infinity;
        break;
    case 'G':
        lowerBound = rhs;
        upperBound = rhs + Numerical::fabs(range);
        break;
    case 'L':
        if (range == Numerical::Infinity) {
            range = -Numerical::Infinity;
        }
        lowerBound = rhs - Numerical::fabs(range);
        upperBound = rhs;
        break;
    case 'E':
        if (range == Numerical::Infinity) {
            range = 0.0;
        }
        if (range >= 0.0) {
            lowerBound = rhs;
            upperBound = rhs + Numerical::Double(range);
        } else {
            upperBound = rhs;
            lowerBound = rhs - Numerical::Double(range);
        }
        break;
    default:
        break;
    }

    static Constraint result;
    try {
        result = Constraint::createConstraint( getName(row->m_name).c_str(),
                                               lowerBound, upperBound);
    } catch (const Constraint::InvalidLowerBoundException & ex) {
        LPERROR(row->m_rowType);
        LPERROR(ex.getMessage());
        exit(0);
    }

    return result;
}

void MpsModelBuilder::buildCostVector(Vector * costVector) const {
    costVector->reInit( m_columns.m_totalCounter );
    costVector->setSparsityRatio(0.0);
    std::vector<std::pair<double, unsigned int> >::const_iterator iter = m_costVector.begin();
    std::vector<std::pair<double, unsigned int> >::const_iterator iterEnd = m_costVector.end();

    for (; iter != iterEnd; ++iter) {
        costVector->setNewNonzero( iter->second, iter->first );
    }
}

Numerical::Double MpsModelBuilder::getObjectiveConstant() const {
    return m_costConstant;
}

std::string MpsModelBuilder::getName() const {
    return m_name;
}

OBJECTIVE_TYPE MpsModelBuilder::getObjectiveType() const {
    return MINIMIZE;
}
