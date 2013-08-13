/* 
 * File:   POMpsProblem.h
 * Author: smidla
 *
 * Created on 2010. oktober 19., 23:13
 */


#ifndef MPSPROBLEM_H
#define	MPSPROBLEM_H

using namespace std;

#include <vector>
#include <map>
#include <list>
#include <iostream>
#include <sstream>
#include <fstream>
//#include <lp/model.h>
#include <lp/variable.h>
#include <lp/constraint.h>
#include <utils/file.h>
#include <debug.h>
#include <linalg/vector.h>
//#include <utils/avltree.h>
#include <utils/list.h>
#include <utils/namestorer.h>
#include <lp/modelbuilder.h>

#define MPS_RECORD_SIZE 80
#define BUFF_SIZE 102400
//#define BUFF_SIZE 51200
//#define BUFF_SIZE 500
//gf#define BUFF_SIZE 120
#define MPS_KEY_SIZE 10
#define KEY_COUNT 500
#define MPS_HASH_TABLE_SIZE 78953 //24989

#define FIELD_0_START  1
#define FIELD_0_END    2
#define FIELD_1_START  4
#define FIELD_1_END   11
#define FIELD_2_START 14
#define FIELD_2_END   21
#define FIELD_3_START 24
#define FIELD_3_END   35
#define FIELD_4_START 39
#define FIELD_4_END   46
#define FIELD_5_START 49
#define FIELD_5_END   60

#define MAX_ROW_WITH 61
#define MPS_NAME_LENGTH 8

#define LOWER_BOUND_READY_MASK 0x1
#define UPPER_BOUND_READY_MASK 0x2

#define INVALID_ROW_TYPE_LIMIT 10
#define INVALID_ROW_NAME_LIMIT 10
#define INVALID_FIELD_LIMIT    10
#define UNNECESSARY_CONTENT_LIMIT 10
#define TOO_LONG_WORD_LIMIT 10

#define MPS_RETURN(ptr) m_carriageReturn = (ptr)[-1] == '\r'; checkTooLongRecord(ptr); return (ptr);
#define MPS_RECORD_END_CHECK(ptr)   if (*ptr == '*') invalidComment(index); \
                                    switch (*(ptr)) { \
                                        case '\r': \
                                            (ptr)++; \
                                        case '\n': \
                                            MPS_RETURN(ptr); \
                                        case '*': \
                                            info = COMMENT; \
                                            ptr = goToEndLine(ptr); \
                                            MPS_RETURN(ptr); \
                                        }
#define MPS_INCOMPLETE_RECORD_CHECK(ptr) if (*ptr == '*') invalidComment(index); \
                                        switch (*(ptr)) { \
                                        case '\r': \
                                            (ptr)++; \
                                        case '\n': \
                                            incompleteRecordError(); \
                                            MPS_RETURN(ptr); \
                                        case '*': \
                                            info = COMMENT; \
                                            ptr = goToEndLine(ptr); \
                                            incompleteRecordError(); \
                                            MPS_RETURN(ptr); \
                                        }
#define MPS_MISSING_COMMENT_CHECK(ptr) if (*(ptr) != '\r' && *(ptr) != '\n' && *(ptr) != '*') { \
                                           unnecessaryContentError(); \
                                           ptr = goToEndLine(ptr); \
                                       }

/**
 */
struct mps_hash_key_list
{
    int m_index;
    char m_keys[MPS_KEY_SIZE * KEY_COUNT];
    mps_hash_key_list * m_next;
};

/**
 */
struct mps_hash_key_producer
{
    mps_hash_key_list * m_head;
    mps_hash_key_list * m_actual;
    int * m_counter;
};

void ReleaseHashKeyProducer(mps_hash_key_producer & hkp);

class MpsNameStorer : public NameStorer
{

    void increase()
    {
        (*(m_hkp.m_counter))++;
    }

    void decrease()
    {
        ReleaseHashKeyProducer(m_hkp);
    }

    void copy(NameStorer * original)
    {
        set((dynamic_cast<MpsNameStorer*> (original))->m_hkp);
    }

    mps_hash_key_producer m_hkp;
public:

    MpsNameStorer()
    {
        m_hkp.m_counter = 0;
        m_hkp.m_actual = 0;
        m_hkp.m_head = 0;
    }

    MpsNameStorer(MpsNameStorer & original): NameStorer(original) {
        copy(&original);
    }
    
    void set(mps_hash_key_producer & hkp)
    {
        if (hkp.m_counter == 0) {
            hkp.m_counter = new int;
            *(hkp.m_counter) = 1;
        }
        m_hkp = hkp;
        (*m_hkp.m_counter)++;
    }

    ~MpsNameStorer()
    {
        decrease();
    }

    NameStorer * getCopy()
    {
        MpsNameStorer * result = new MpsNameStorer(*this);
        return result;
    }
};

/**
 *
 * @param k1
 * @param k2
 * @return
 */
inline bool key_comp(register const char * k1, register const char * k2)
{
    return *((unsigned long long int*) k1) < *((unsigned long long int*) k2);
}

/**
 *
 */
template<class KEY, class VALUE, class HASH_FUNC>
class HashTable
{
    friend class MpsProblem;
public:

    /**
     *
     */
    HashTable()
    {
        m_count = 0;
    }

    /**
     *
     */
    ~HashTable()
    {
    }

    /**
     *
     * @param hash_func
     * @param size
     */
    inline void init(HASH_FUNC hash_func, int size)
    {
        m_hash_func = hash_func;
        m_table.resize(size);
    }

    /**
     *
     * @param key
     * @param value
     * @param hash
     */
    inline KEY * add(KEY & key, VALUE value, const int hash /*= -1*/)
    {
        // if (hash == -1)
        //     hash = m_hash_func(key, m_table.size());
        m_count++;
        std::map<KEY, VALUE> & refMap = m_table.at(hash);
        return const_cast<KEY*> (&(refMap.insert(std::make_pair(key, value)).first->first));
        //return const_cast<KEY*> (&(m_table[hash].insert(std::make_pair(key, value)).first->first));

    }

    /**
     *
     * @param key
     * @param hash
     * @return
     */
    inline bool exist(KEY & key, const int hash /*= -1*/)
    {
        // if (hash == -1)
        //     hash = m_hash_func(key, m_table.size());
        return m_table[hash].find(key) != m_table[hash].end();
    }

    /**
     *
     * @param key
     * @param hash
     * @return
     */
    inline VALUE * getValue(KEY & key, const int hash /*= -1*/)
    {
        //if (hash == -1)
        //    hash = m_hash_func(key, m_table.size());
        //if (!exist(key, hash))
        //    return 0;
        std::map<KEY, VALUE> & refMap = m_table[hash];
        typename std::map<KEY, VALUE >::iterator iter = refMap.find(key);
        if (iter != refMap.end()) {
            return &iter->second;
        }
        return 0;
    }

    /**
     *
     * @param key
     * @param hash
     * @return
     */
    inline KEY * getKey(KEY & key, const int hash /*= -1*/)
    {
        //if (hash == -1)
        //    hash = m_hash_func(key, m_table.size());
        /*if (!exist(key, hash))
            return 0;*/
        std::map<KEY, VALUE> & refMap = m_table[hash];
        typename std::map<KEY, VALUE >::const_iterator iter = refMap.find(key);
        if (iter != refMap.end()) {
            return const_cast<KEY*> (&iter->first);
            //return const_cast<KEY*> (&m_table[hash].find(key)->first);
        }
        return 0;
    }

    inline VALUE * getValueAndKey(KEY & key, KEY * & resKey, const int hash /*= -1*/)
    {
        //if (hash == -1)
        //    hash = m_hash_func(key, m_table.size());
        //if (!exist(key, hash))
        //    return 0;
        std::map<KEY, VALUE> & refMap = m_table[hash];
        typename std::map<KEY, VALUE >::iterator iter = refMap.find(key);
        if (iter != refMap.end()) {
            resKey = const_cast<KEY*> (&iter->first);
            return &iter->second;
        }
        return 0;
    }

    /**
     *
     * @return
     */
    inline unsigned int getCount() const
    {
        return m_count;
    }
private:
    /**
     */
    std::vector< std::map<KEY, VALUE> > m_table;
    /**
     */
    HASH_FUNC m_hash_func;
    /**
     */
    unsigned int m_count;
};

/**
 *
 * @param key
 * @param max
 * @return
 */
inline unsigned int mps_hash(register const char * key, int max)
{
    register unsigned int num = 0;
    while (*key) {
        num += *key;
        num += (num << 10);
        num ^= (num >> 6);
        key++;
    }
    num += (num << 3);
    num ^= (num >> 11);
    num += (num << 15);

    return num % max;
}

/**
 *
 * @param v
 * @param max
 * @return
 */
template<class T>
class hash_function
{
public:

    /**
     *
     * @param v
     * @param max
     * @return
     */
    int operator()(T & v, size_t max)
    {
        return mps_hash(v.getKey(), max);
    }
};

/**
 */
class MpsProblem : public ModelBuilder
{
    // FIXME: CONFLICTING DEFINE AND ENUMERATED CONSTANTS
#ifdef _WINERROR_H
#undef ERROR
#undef NO_ERROR
#endif

    static const char * sm_expectedSection[];

    /**
     */
    enum SECTION_TYPE
    {
        SEC_NAME,
        SEC_ROWS,
        SEC_COLUMNS, 
        SEC_RHS, 
        SEC_RANGES, 
        SEC_BOUNDS,
        SEC_ENDATA
    };

    /**
     */
    enum ROW_INFO
    {
        NAME_SECTION, 
        ROWS_SECTION, 
        COLUMNS_SECTION, 
        RHS_SECTION,
        RANGES_SECTION, 
        BOUNDS_SECTION, 
        ENDATA_SECTION, 
        INVALID_SECTION,
        COMMENT, NO_INFO
    };

    /**
     */

    enum MPS_ERROR_TYPE
    {
        NO_ERROR,
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
        RHS_MISSING, 
        RANGES_MISSING, 
        BOUNDS_MISSING,
        ENDATA_MISSING, 
        BOUND_CHANGED, 
        INVALID_COMMENT, 
        INVALID_NUMBER_FORMAT,
        UNKNOWN_SECTION_NAME, 
        INVALID_SECTION_NAME, 
        VALUE_OVERRIDED,

        SECTION_ALREADY_PROCESSED
    };

    /**
     */
    class Exception
    {
    };

    /**
     */
    class FatalErrorException : public Exception
    {
        std::string m_error;
    public:

        /**
         *
         * @param error
         */
        FatalErrorException(std::string error) : m_error(error)
        {
        }

        /**
         *
         * @return
         */
        std::string getError() const
        {
            return m_error;
        }
    };

    /**
     */
    union Name
    {
        char m_name[MPS_NAME_LENGTH + 1];
        unsigned int m_nameFragments[ MPS_NAME_LENGTH / sizeof (unsigned int) ];
    };

    /**
     */
    class Row
    {
        friend class MpsProblem;
        /**
         */
        char m_type;
        /**
         */
        char * m_name;


        /**
         */
        unsigned int m_nonZeros;
    public:

        Row()
        {
            m_nonZeros = 0;
            m_name = 0;
        }

        /**
         *
         * @return
         */
        inline const char * getKey() const
        {
            return m_name;
        }

        /**
         *
         * @param r
         * @return
         */
        inline bool operator<(const Row & r) const
        {
            return key_comp(m_name, r.m_name);
        }

    } * _r;

    /**
     */
    class Column
    {
        friend class MpsProblem;

        struct Pair
        {
            unsigned int m_index;
            Numerical::Double m_value;
        };

        /**
         */
        char m_type;
        /**
         */
        char * m_name;
        /**
         */
        List<Pair> m_coeffs;
        //std::list<Pair> m_coeffs;
        //AvlTree<unsigned int, Numerical::Double> m_coeffs;
        //std::map<unsigned int, Numerical::Double> m_coeffs;
        bool m_inCostVector;
    public:

        Column()
        {
            m_inCostVector = false;
        }

        inline void add(unsigned int index, Numerical::Double value)
        {
            Pair pair = {index, value};
            m_coeffs.pushBack(pair);
            //            m_coeffs.push_back(Pair{index, value});
        }

        inline Numerical::Double getValue(unsigned int rowIndex) const
        {
            List<Pair>::Iterator iter = m_coeffs.begin();
            List<Pair>::Iterator iterEnd = m_coeffs.end();
            for (; iter != iterEnd; iter++) {
                if (iter.getData().m_index == rowIndex) {
                    return iter.getData().m_value;
                }
            }
            return 0.0;
        }

        /**
         *
         * @return
         */
        inline const char * getKey() const
        {
            return m_name;
        }

        /**
         *
         * @param c
         * @return
         */
        inline bool operator<(const Column & c) const
        {
            return key_comp(m_name, c.m_name);
        }
    } * _c;

    /**
     */
    class Bound
    {
        friend class MpsProblem;

        /**
         */
        enum BOUND_TYPE
        {
            INVALID, LO, UP, FX, FR, MI, PL, BV, LI, UI, SC
        };

        /**
         */

        /*struct Bound_ {
            Numerical::Double m_value;
        } * _b;*/

        class BoundId
        {
        public:

            BoundId()
            {
            }
            unsigned int m_column;
            BOUND_TYPE m_type;

            inline bool operator<(const BoundId & id) const
            {
                register unsigned int id1 = m_column * 100 + m_type;
                register unsigned int id2 = id.m_column * 100 + id.m_type;
                return id1 < id2;
            }
        };

        /**
         */
        char * m_name;
        /**
         */
        std::map<BoundId, Numerical::Double> m_bounds;
    public:

        /**
         *
         * @return
         */
        inline const char * getKey() const
        {
            return m_name;
        }

        /**
         *
         * @param b
         * @return
         */
        inline bool operator<(const Bound & b) const
        {
            return key_comp(m_name, b.m_name);
        }

    };

    union ColumnName
    {
        char m_name[8];
        unsigned int m_nameFragment[ 8 / sizeof (unsigned int) ];
    };

    /**
     */
    struct MpsErrorData
    {

        /**
         */
        enum ERROR_LEVEL
        {
            WARNING, ERROR, FATAL_ERROR
        };
        /**
         */
        unsigned int m_row;
        /**
         */
        SECTION_TYPE m_section;
        /**
         */
        std::string m_text;
        /**
         */
        ERROR_LEVEL m_level;
        /**
         */
        static unsigned int sm_warningCount;
        /**
         */
        static unsigned int sm_errorCount;
        /**
         */
        static unsigned int sm_fatalErrorCount;
    };

    /**
     *
     */
    template <int type>
    class MpsError
    {
    public:

        /**
         *
         * @param row
         * @param section
         * @param text
         * @param level
         */
        inline MpsError(unsigned int row, SECTION_TYPE section, std::string text,
                MpsErrorData::ERROR_LEVEL level, bool terminate = false)
        {
            m_data.m_row = row;
            m_data.m_section = section;
            m_data.m_text = text;
            m_data.m_level = level;
            switch (level) {
                case MpsErrorData::WARNING:
                    MpsErrorData::sm_warningCount++;
                    break;
                case MpsErrorData::ERROR:
                    MpsErrorData::sm_errorCount++;
                    break;
                case MpsErrorData::FATAL_ERROR:
                    MpsErrorData::sm_fatalErrorCount++;
                    break;
            }
            std::stringstream str;
            str << "row: [" << row << "], section: " << MpsProblem::getSectionName(section) <<
                    ", error code: " << type << " : " << text;
            if (sm_count < sm_limit) {
                if (level != MpsErrorData::WARNING) {
                    LPERROR(str.str());
                } else {
                    LPWARNING(str.str());
                }
            } else if (sm_count == sm_limit) {
                if (level != MpsErrorData::WARNING) {
                    LPERROR("The number of type " << type << " errors is unless " << sm_limit);
                } else {
                    LPWARNING("The number of type " << type << " errors is unless " << sm_limit);
                }
            }
            sm_count++;

            if (level == MpsErrorData::FATAL_ERROR && terminate) {
                throw FatalErrorException(std::string("fatal error: ") + str.str());
            }
        }

        /**
         *
         * @return
         */
        inline MpsErrorData getData() const
        {
            return m_data;
        }

        /**
         *
         * @return
         */
        static unsigned int getLimit()
        {
            return sm_limit;
        }

        /**
         *
         * @param limit
         */
        static void setLimit(unsigned int limit)
        {
            sm_limit = limit;
        }

        /**
         *
         */
        static void initCount()
        {
            sm_count = 0;
        }

    private:
        /**
         */
        MpsErrorData m_data;
        /**
         */
        static unsigned int sm_count;
        /**
         */
        static unsigned int sm_limit;
    };

    /**
     */
    hash_function<Row> m_rowHashFunc;
    /**
     */
    hash_function<Column> m_colHashFunc;
    /**
     */
    hash_function<Bound> m_boundHashFunc;
    /**
     */
    HashTable<Row, int, hash_function<Row> > m_rows;
    /**
     */
    HashTable<Column, int, hash_function<Column> > m_columns;
    /**
     */
    HashTable<Column, int, hash_function<Column> > m_rhs;
    /**
     */
    HashTable<Column, int, hash_function<Column> > m_ranges;
    /**
     */
    HashTable<Bound, int, hash_function<Bound> > m_bounds;
    /**
     */
    std::vector<Row*> m_rowIndexTable;
    /**
     */
    std::vector<Column*> m_columnIndexTable;
    /**
     */
    std::vector<Column*> m_rhsIndexTable;
    /**
     */
    std::vector<Column*> m_rangeIndexTable;
    /**
     */
    std::vector<Column*> m_boundInexTable;
    /**
     */
    Row m_costRow;
public:
    /**
     *
     */
    MpsProblem();
    /**
     *
     * @param orig
     */
    MpsProblem(const MpsProblem& orig);
    /**
     *
     */
    virtual ~MpsProblem();
    /**
     *
     * @param filename
     */
    void loadFromFile(const char * filename);

    /**
     *
     * @param filename
     * @param model
     */
//    static void saveToFile(const char * filename, const Model & model);
    
    /**
     * 
     * @param fileName
     * @param model
     */
//    static void saveBasis(const char * fileName, const Model & model, unsigned int iterations);
    /**
     *
     * @param rhsIndex
     */
    void setRhsIndex(int rhsIndex);
    /**
     *
     * @param boundsIndex
     */
    void setBoundsIndex(int boundsIndex);
    /**
     *
     * @param rangesIndex
     */
    void setRangesIndex(int rangesIndex);
    /**
     *
     * @return
     */
    int getRhsIndex() const;
    /**
     *
     * @return
     */
    int getBoundsIndex() const;
    /**
     *
     * @return
     */
    int getRangesIndex() const;
    /**
     *
     * @return
     */
    int getRhsCount() const;
    /**
     *
     * @return
     */
    int getBoundsCount() const;
    /**
     *
     * @return
     */
    int getRangesCount() const;

    /**
     *
     * @param problem
     * @param rhsIndex
     * @param boundsIndex
     * @param rangesIndex
     */
//    void buildModel(Model * model);

    /**
     * Name of the  problem defined in the corresponding MPS file
     * @return the name of the problem
     * @author Mark Korondi <korondi@dcs.uni-pannon.hu>
     */
    inline std::string name() const
    {
        return m_name;
    }

    /**
     * Iteration report support.
     * @author Peter Tar <tar@dcs.uni-pannon.hu>
     */
    void report(int debugLevel);

    /**
     * Total working time report support.
     * @author Peter Tar <tar@dcs.uni-pannon.hu>
     */
    static clock_t lastReadingTime();

    unsigned int getColumnCount() const;

    /**
     * 
     * @return 
     */
    unsigned int getRowCount() const;

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
    const Constraint & getConstraint(unsigned int index) const;

    /**
     * 
     * @param index
     * @param rowVector
     */
    void buildRow(unsigned int index, Vector * rowVector) const;

    /**
     * 
     * @param index
     * @param columnVector
     */
    void buildColumn(unsigned int index, Vector * columnVector) const;

    /**
     * 
     * @param costVector
     */
    void buildCostVector(Vector * costVector) const;

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
    
private:

    /**
     * Mps reading time
     */
    static clock_t cl_mps;
    /**
     */
    std::list<MpsErrorData> m_errors;
    /**
     */
    unsigned int m_modifyNumericErrors;
    /**
     */
    unsigned int m_modifyLogicErrors;
    /**
     */
    mps_hash_key_producer m_hkp;
    /**
     */
    File m_file;
    /**
     */
    std::string m_name;
    /**
     */
    SECTION_TYPE m_section;
    /**
     */
    unsigned int m_line;
    /**
     */
    Name m_actualColumn;
    /**
     */
    Column * m_actualColumnData;
    /**
     */
    bool * m_actualColumnNonzeros;
    /**
     */
    const char * m_startPtr;
    /**
     */
    int m_rowLength;
    /**
     */
    bool m_carriageReturn;
    /**
     */
    unsigned int m_nonZeros;
    /**
     */
    std::vector<Numerical::Double> m_columnLower;
    /**
     */
    std::vector<Numerical::Double> m_columnUpper;
    /**
     */
    unsigned int m_columnLowerNonZeros;
    /**
     */
    unsigned int m_columnUpperNonZeros;

    /**
     */
    std::vector<char> m_boundReady;
    /**
     */
    unsigned int m_plusTypeCount;
    /**
     */
    unsigned int m_minusTypeCount;
    /**
     */
    unsigned int m_fixTypeCount;
    /**
     */
    unsigned int m_boundedTypeCount;
    /**
     */
    unsigned int m_freeTypeCount;
    /**
     *
     */
    unsigned int m_costVectorIndex;
    /**
     */
    bool m_costVectorReady;
    
    /**
     */
    List<Column::Pair> m_costVector;
    /**
     */
    int m_rhsIndex;
    /**
     */
    int m_boundsIndex;
    /**
     */
    int m_rangesIndex;
    /**
     */
    std::string m_fileName;

    static char * saveEndLine(std::ofstream & ofs, char * actual, char * const buffer,
            const char * const bufferEnd);
//    static char * saveName(std::ofstream & ofs, char * actual, char * const buffer,
//            const char * const endBuffer, const Model & model);
//    static char * saveRows(std::ofstream & ofs, char * actual, char * const buffer,
//            const char * const endBuffer, const Model & model);
//    static char * saveColumns(std::ofstream & ofs, char * actual, char * const buffer,
//            const char * const endBuffer, const Model & model);
//    static char * saveRhs(std::ofstream & ofs, char * actual, char * const buffer,
//            const char * const endBuffer, const Model & model);
//    static char * saveRanges(std::ofstream & ofs, char * actual, char * const buffer,
//            const char * const endBuffer, const Model & model);
//    static char * saveBounds(std::ofstream & ofs, char * actual, char * const buffer,
//            const char * const endBuffer, const Model & model);
//    static char * saveOneColumn(std::ofstream & ofs, char * actual, char * const buffer,
//            const char * const endBuffer, const Model & model,
//            const Vector & column, const char * columnName, int colIndex,
//            Numerical::Double costValue, const Vector & costVector);

    void printVariableStatistics();
    /**
     *
     * @param model
     */
//    void makeMatrix(Model * model);
    /**
     *
     * @param model
     * @param rowCount
     * @param rhsIndex
     * @param rangesIndex
     */
//    void makeRowBounds(Model * model, int rowCount, int rhsIndex, int rangesIndex);
    /**
     *
     * @param model
     * @param boundsIndex
     */
//    void makeColumnBounds(Model * model, int boundsIndex);
    /**
     * 
     * @param model
     */
//    void copyNames(Model * model);
    /**
     *
     * @param columnIndex
     * @param vector
     * @param columnHashTable
     * @param given
     */
    void getColumnVector(unsigned int columnIndex, Vector * vector,
            const HashTable<Column, int, hash_function<Column> > & columnHashTable,
            std::vector<bool> * given);

    /**
     *
     * @param columnIndex
     * @param type
     * @param bound
     * @param newReady
     */
    void setBound(unsigned int columnIndex, Bound::BOUND_TYPE type, Numerical::Double bound,
            bool newReady = true);
    /**
     *
     * @param msg1
     * @param msg2
     * @param line
     */
    const char * parseDouble(const register char * ptr, Numerical::Double * val);
    //Numerical::Double parseDouble(const register char * str, const register char * & ptr) const;
    /**
     *
     * @param ptr
     * @param length
     * @return
     */
    const char * checkEmptyField(const register char * ptr, register int length);
    /**
     *
     * @param ptr
     * @return
     */
    const char * skipWhiteSpace(const register char * ptr);

    /**
     * 
     * @param ptr
     * @param dest
     * @param length
     * @param errorCode
     * @return
     */
    const char * copyWord(const register char * ptr, register char * dest,
            register int length, MPS_ERROR_TYPE & errorCode);

    /**
     * 
     * @param ptr
     * @param dest
     * @param length
     * @param errorCode
     * @param hash
     * @return
     */
    const char * copyId(const register char * ptr, register char * dest,
            int index, int fieldEnd, MPS_ERROR_TYPE & errorCode, register unsigned int * hash);
    /**
     *
     * @param ptr
     * @param dest
     * @param end
     * @param length
     * @param errorCode
     * @return
     */
    const char * copyName(const register char * ptr, register char * dest,
            register int length, MPS_ERROR_TYPE & errorCode);
    /**
     *
     * @param ptr
     * @return
     */
    const char * goToEndLineOrWord(const register char * ptr);
    /**
     *
     * @param ptr
     * @return
     */
    const char * goToEndLine(const register char * ptr);
    /**
     *
     * @param ptr
     * @param info
     * @return
     */
    const char * readRowRecord(const register char * ptr, ROW_INFO & info);
    /**
     *
     * @param ptr
     * @param columns
     * @param info
     * @return
     */
    const char * readColumnRecord(const register char * ptr, HashTable<Column, int,
            hash_function<Column> > & columns, ROW_INFO & info, 
            vector<Column*> * indexTable);
    /**
     *
     * @param ptr
     * @param info
     * @return
     */
    const char * readBoundRecord(const register char * ptr, ROW_INFO & info);
    /**
     *
     * @param ptr
     * @param info
     * @param nextWord
     * @return
     */
    const char * nextRowType(const register char * ptr, ROW_INFO & info, bool nextRow = true);
    /**
     *
     * @param ptr
     * @param info
     * @return
     */
    const char * readName(const register char * ptr, ROW_INFO & info);

    /**
     * 
     */
    void clearActualColumnData();

    /**
     *
     * @param ptr
     */
    inline void initTooLongRecordSensor(const char * ptr)
    {
        m_startPtr = ptr;
        m_rowLength = 0;
        m_carriageReturn = false;
    }

    /**
     *
     * @param ptr
     */
    inline void checkTooLongRecord(const char * ptr)
    {
        unsigned int length = m_rowLength + (ptr - m_startPtr) - (m_carriageReturn ? 1 : 0);
        if (length > MAX_ROW_WITH) {
            tooLongRecordError(length);
        }
    }

    /**
     *
     * @return
     */
    const char * getActualSectionName();
    /**
     *
     * @param type
     * @return
     */
    static const char * getSectionName(SECTION_TYPE type);

    /**
     *
     * @param index
     * @param field_start
     */
    void invalidStartFieldError(int index, int field_start);

    /**
     *
     * @param index
     * @param field_end
     */
    void invalidEndFieldError(int index, int field_end);
    /**
     *
     * @param type
     */
    void invalidRowTypeError(const char * type);
    /**
     *
     * @param row
     */
    void invalidRowName(const char * row);

    /**
     *
     * @param column
     * @param row
     * @param original
     * @param newValue
     */
    void valueOverridedError(const char * column, const char * row, Numerical::Double original,
            Numerical::Double newValue);

    /**
     *
     * @param column
     */
    void invalidColumnName(const char * column);
    /**
     *
     * @param bound
     */
    void invalidBoundType(const char * bound);
    /**
     *
     * @param position
     */
    void invalidComment(unsigned int position);
    /**
     * 
     * @param number
     * @param index
     */
    void invalidNumberFormat(const char * number, Numerical::Double parsed);
    /**
     * 
     * @param name
     * @param expected
     */
    void invalidSectionName(const char * name, const char * expected);
    /**
     * 
     * @param name
     * @param start
     * @param end
     */
    void unknownSectionName(const char * name, int start, int end);
    /**
     *
     */
    void unnecessaryContentError();
    /**
     *
     * @param word
     */
    void tooLongWordError(const char * word, bool logicModify = true);
    /**
     *
     * @param length
     */
    void tooLongRecordError(unsigned int length);
    /**
     *
     * @param word
     */
    void missingCommentError(const char * word);
    /**
     *
     * @param row
     */
    void rowExistsError(const char * row);
    /**
     *
     * @param column
     */
    void columnExistsError(const char * column);
    /**
     *
     */
    void incompleteRecordError();
    /**
     *
     */
    void missingNameSection();
    /**
     *
     */
    void missingRowsSection();
    /**
     *
     */
    void missingColumnsSection();
    /**
     *
     */
    void missingRhsSection();
    /**
     *
     */
    void missingRangesSection();
    /**
     *
     */
    void missingBoundsSection();
    /**
     *
     */
    void missingEndataSection();
};



#endif	/* MPSPROBLEM_H */
