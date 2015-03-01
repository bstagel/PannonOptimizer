#ifndef MPSPROBLEM_H
#define MPSPROBLEM_H

#include <globals.h>
#include <string>
#include <list>
#include <vector>
#include <sstream>
#include <utils/file.h>
#include <utils/hashtable.h>
#include <utils/list.h>
#include <linalg/densevector.h>
#include <lp/modelbuilder.h>
#include <lp/variable.h>
#include <lp/constraint.h>

#define CREATE_STRING( result, expr ) { \
    std::ostringstream stream; \
    stream << expr; \
    result = stream.str(); \
}

#define CHECK_NAME_FULL(nameBits, wrong) { \
    unsigned long long int maskedPattern = nameBits & 0x6060606060606060LLU; \
    maskedPattern = (maskedPattern | (maskedPattern << 1LLU) | (maskedPattern >> 1LLU)) & 0x6060606060606060; \
    wrong = wrong || maskedPattern != 0x6060606060606060LLU; \
}

#define MPS_FIELD_3_LENGTH 12

class MpsModelBuilder: public ModelBuilder {
public:
    MpsModelBuilder();

    virtual ~MpsModelBuilder();

    void loadFromFile(const std::string & fileName);

    void clear();

    // ********************************************************
    // Inherited from MatrixBuilder
    // ********************************************************

    /**
     *
     * @return
     */
    unsigned int getColumnCount() const;

    /**
     *
     * @return
     */
    unsigned int getRowCount() const;

    /**
     *
     * @param index
     * @param rowVector
     */
    void buildRow(unsigned int index, SparseVector * rowVector,
        std::vector<unsigned int> * nonzeros) const;

    /**
     *
     * @param index
     * @param columnVector
     */
    void buildColumn(unsigned int index, SparseVector * columnVector,
        std::vector<unsigned int> * nonzeros) const;

    /**
     *
     * @return
     */
    bool hasRowwiseRepresentation() const;

    /**
     *
     * @return
     */
    bool hasColumnwiseRepresentation() const;

    // ********************************************************
    // Inherited from ModelBuilder
    // ********************************************************

    /**
     *
     * @param index
     * @return
     */
    const Variable & getVariable(unsigned int index) const;

    /**
     *
     * @param index
     * @return
     */
    const Constraint & getConstraint(unsigned int index) const ;

    /**
     *
     * @param costVector
     */
    void buildCostVector(DenseVector * costVector) const;

    /**
     *
     * @return
     */
    Numerical::Double getObjectiveConstant() const;

    /**
     *
     * @return
     */
    std::string getName() const;

    /**
     */
    OBJECTIVE_TYPE getObjectiveType() const;


private:

    enum MPS_ERROR_TYPE
    {
        MPS_NO_ERROR, // for Windows
        INVALID_ROW_TYPE,
        INVALID_ROW_NAME,
        INVALID_COLUMN_NAME,
        INVALID_START_FIELD,
        INVALID_END_FIELD,
        UNNECESSARY_CONTENT,
        TOO_LONG_WORD,
        MISSING_COMMENT,
        ROW_EXISTS,
        COLUMN_EXISTS,
        INCOMPLETE_RECORD,
        TOO_LONG_RECORD,
        INVALID_BOUND_TYPE,
        NAME_MISSING,
        ROWS_MISSING,
        COLUMNS_MISSING,
        ENDATA_MISSING,
        BOUND_REPEATED,
        INVALID_COMMENT,
        INVALID_NUMBER_FORMAT,
        UNKNOWN_SECTION_NAME,
        VALUE_REPEATED,
        SECTION_ALREADY_PROCESSED
    };

    enum MPS_ERROR_LEVEL
    {
        MPS_WARNING,
        MPS_ERROR,
        MPS_FATAL_ERROR,
        MPS_ERROR_LEVEL_COUNT
    };

    unsigned int m_levelCounters[MPS_ERROR_LEVEL_COUNT];

    template <int TYPE, int LEVEL>
    class Error {
        struct Content {
            unsigned int m_line;
            std::string m_message;
        };
        unsigned int m_counter;
        unsigned int m_printLimit;
        std::list<Content> m_errors;
        unsigned int * m_levelCounters;
    public:
        Error() {
            m_counter = 0;
            m_counter = 0;
            m_levelCounters = 0;
        }

        void init(unsigned int limit,
                  unsigned int * levelCounters) {
            m_printLimit = limit;
            m_counter = 0;
            m_levelCounters = levelCounters;
        }

        void clear() {
            m_counter = 0;
            m_errors.clear();
        }

        void increase() {
            m_counter++;
        }

        void getCount() {
            return m_counter;
        }

        void add(const std::string & message, unsigned int line) {
            Content newError;
            newError.m_line = line;
            newError.m_message = message;
            m_errors.push_back(newError);
            if (m_counter < m_printLimit || m_printLimit == 0) {
                switch (LEVEL) { // TODO: kijelzest megcsinalni
                case MPS_WARNING:
                    std::cout << "WARNING: ";
                    break;
                case MPS_ERROR:
                    std::cout << "ERROR: ";
                    break;
                case MPS_FATAL_ERROR:
                    std::cout << "FATAL ERROR: ";
                    break;
                }
                std::cout << message << " (" << line << ")" << std::endl;
            }
            if (m_levelCounters != 0) {
                m_levelCounters[LEVEL]++;
            }
            m_counter++;
        }
    };

    unsigned int m_errorPrintLimit;

    Error<INVALID_ROW_TYPE, MPS_ERROR> m_invalidRowType;
    Error<INVALID_ROW_NAME, MPS_ERROR> m_invalidRowName;
    Error<INVALID_COLUMN_NAME, MPS_ERROR> m_invalidColumnName;
    Error<INVALID_START_FIELD, MPS_ERROR> m_invalidStartField;
    Error<INVALID_END_FIELD, MPS_ERROR> m_invalidEndField;
    Error<UNNECESSARY_CONTENT, MPS_WARNING> m_unnecessaryContent;
    Error<TOO_LONG_WORD, MPS_ERROR> m_tooLongWord;
    Error<MISSING_COMMENT, MPS_WARNING> m_missingComment;
    Error<ROW_EXISTS, MPS_ERROR> m_rowExists;
    Error<COLUMN_EXISTS, MPS_ERROR> m_columnExists;
    Error<INCOMPLETE_RECORD, MPS_ERROR> m_incompleteRecord;
    Error<TOO_LONG_RECORD, MPS_WARNING> m_tooLongRecord;
    Error<INVALID_BOUND_TYPE, MPS_ERROR> m_invalidBoundType;
    Error<NAME_MISSING, MPS_WARNING> m_nameMissing;
    Error<ROWS_MISSING, MPS_FATAL_ERROR> m_rowsMissing;
    Error<COLUMNS_MISSING, MPS_FATAL_ERROR> m_columnsMissing;
    Error<ENDATA_MISSING, MPS_WARNING> m_endataMissing;
    Error<BOUND_REPEATED, MPS_ERROR> m_boundRepeated;
    Error<INVALID_COMMENT, MPS_ERROR> m_invalidComment;
    Error<INVALID_NUMBER_FORMAT, MPS_ERROR> m_invalidNumberFormat;
    Error<UNKNOWN_SECTION_NAME, MPS_ERROR> m_unknownSectionName;
    Error<VALUE_REPEATED, MPS_ERROR> m_valueRepeated;
    Error<SECTION_ALREADY_PROCESSED, MPS_ERROR> m_sectionAlreadyProcessed;

    enum MPS_SECTION {
        MPS_NAME,
        MPS_ROWS,
        MPS_COLUMNS,
        MPS_RHS,
        MPS_RANGES,
        MPS_BOUNDS,
        MPS_ENDATA,
        MPS_UNKNOWN
    } m_section;

    template <class T>
    class MpsErrors {

    };

    union MpsName {
        char m_chars[8];
        unsigned long long int m_bits;
    };

    union MpsRowType {
        char m_chars[4];
        unsigned int m_bits;
    };

    union EmptySection4 {
        unsigned int m_bits;
        char m_chars[4];
    };

    union EmptySection2 {
        unsigned short m_bits;
        char m_chars[2];
    };

#pragma pack(push)
#pragma pack(1)

    struct MpsRowRecord {
        MpsRowType m_type;
        MpsName m_name;
        char m_endOfLine[2];
    };

    struct Row {
        MpsName m_name;
        Numerical::Double m_rhs;
        Numerical::Double m_range;
        unsigned int m_index;
        unsigned int m_line;
        char m_rowType;
    };

    struct MpsColumnRecordFirst {
        EmptySection4 m_spaces1;
        MpsName m_columnName;
        EmptySection2 m_spaces2;
        MpsName m_rowName;
        EmptySection2 m_spaces3;
    };

    struct MpsBoundRecord {
        EmptySection4 m_spaces1;
        MpsName m_columnName;
        EmptySection2 m_spaces2;
        MpsName m_rowName;
    };

    struct MpsColumnRecordSecond {
        MpsName m_rowName;
        EmptySection2 m_spaces;
        //char m_number[12];
        //EmptySection4 m_end;
    };

    struct Column {
        MpsName m_name;
        double * m_firstValue;
        unsigned int * m_firstIndex;
        std::list<double*>::const_iterator m_beginValueIter;
        std::list<unsigned int*>::const_iterator m_beginIndexIter;
        unsigned int m_nonzeros;
        unsigned int m_index;
        double m_lowerBound;
        double m_upperBound;
        unsigned int m_line;
        bool m_isInteger;
    };

#pragma pack(pop)

    static const int sm_valueBufferSize = 131072;

    std::list<double *> m_values;
    std::list<unsigned int *> m_indices;
    double * m_actualValue;
    const double * m_actualValueEnd;
    unsigned int * m_actualIndex;
    const unsigned int * m_actualIndexEnd;

    // cache optimization!
    struct MpsRows {
        std::list<std::vector<Row>* > m_rows;
        unsigned int m_listSize;
        unsigned int m_lastSize;
        unsigned int m_lastCounter;
        unsigned int m_totalCounter;
    } m_rows;

    struct Index {
        MpsName m_name;

    };

    struct RowIndex: public Index {
        //MpsName m_name;
        Row * m_row;
        bool operator != (const RowIndex & index) const {
            return m_name.m_bits != index.m_name.m_bits;
        }
    };

    struct MpsColumns {
        std::list<std::vector<Column>* > m_columns;
        unsigned int m_listSize;
        unsigned int m_lastSize;
        unsigned int m_lastCounter;
        unsigned int m_totalCounter;
    } m_columns;

    struct ColumnIndex: public Index {
        //MpsName m_name;
        Column * m_column;
        bool operator != (const ColumnIndex & index) const {
            return m_name.m_bits != index.m_name.m_bits;
        }
    };

    File m_inputFile;

    unsigned int m_bufferSize;

    const char * m_buffer;

    const char * m_bufferEnd;

    std::string m_name;

    unsigned int m_currentLine;

    class MpsNameHash {
    public:
        static unsigned long long int getHash(const Index & index) {
            return index.m_name.m_bits;
        }
    };

    HashTable<RowIndex, MpsNameHash> m_rowsTable;

    HashTable<ColumnIndex, MpsNameHash> m_columnsTable;

    std::vector<const Row*> m_rowsMap;

    std::vector<const Column*> m_columnsMap;

    MpsName m_costVectorName;

    std::vector<std::pair<double, unsigned int> > m_costVector;

    double m_costConstant;

    void showBuffer(int count, bool newLine);

    MPS_SECTION readSectionType();

    void skipEndLine();

    bool skipComments();

    void skipSection();

    void readName();

    void readRows();

    void finishRows();

    void saveColumn(Column * actualColumnPtr,
                    std::vector<Column> ** lastVectorPtr,
                    bool * columnPattern);

    bool parseRecordStart(double * value,
                          MpsName * id1,
                          MpsName * id2,
                          EmptySection4 * startSection,
                          bool *newLine);

    bool parseBoundRecord(double * value,
                          MpsName * id1,
                          MpsName * id2,
                          EmptySection4 * startSection);

    bool parseRecordEnd(double * value,
                        MpsName * id);

    void readColumns();

    void finishColumns();

    void readRhsOrRanges(Numerical::Double Row::*range );

    void readBounds();

    double parseDouble(const char * ptr);

    std::string getName(MpsName name) const;

    void invalidRowType(char type, unsigned int line);

    void invalidRowName(const std::string & name, unsigned int line);

    void rowExists(const RowIndex & row);

};

#endif // MPSPROBLEM_H
