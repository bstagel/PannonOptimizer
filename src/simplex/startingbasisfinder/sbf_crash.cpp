/**
 * @file sbf_crash.cpp
 **/

#include <simplex/startingbasisfinder/sbf_crash.h>
#include <simplex/simplex.h>
#include <utils/indexlist.h>

#include <utils/timer.h>

#include <fstream>

SbfCrash::SbfCrash(const SimplexModel& model,
                   std::vector<int>* basisHead,
                   IndexList<const Numerical::Double*>* variableStates,
                   StartingBasisFinder::STARTING_NONBASIC_STATES nonbasicStates):
    SbfSuper(model, basisHead, variableStates, nonbasicStates)
{

}

SbfCrash::~SbfCrash()
{

}

void SbfCrash::run()
{
    Timer timer;

    LPINFO("CRASH eleje");
    timer.start();

    m_basisHead->clear();

    const unsigned int rowCount = m_model.getRowCount();
    const unsigned int columnCount = m_model.getColumnCount();

    //std::vector<unsigned int> rowCounter(rowCount);
    std::vector<unsigned int> columnCounter(columnCount);

    // TODO: majd jo lenne meghatarozni a legsurubb sor suruseget, es
    // azzal memoriat sporolhatunk
    IndexList<> rowCounter(rowCount, columnCount + 1);

    unsigned int rowIndex;
    unsigned int columnIndex;

    const Matrix & matrix = m_model.getMatrix();

    //LPINFO("eddig jo");

    unsigned int min = matrix.columnCount();
    for (rowIndex = 0; rowIndex < rowCount; rowIndex++) {
        unsigned int nonzeros = matrix.row(rowIndex).nonZeros();
        rowCounter.insert( nonzeros, rowIndex );
        if (nonzeros < min && nonzeros > 0) {
            min = nonzeros;
        }
        //rowCounter[rowIndex] = matrix.row(rowIndex).nonZeros();
    }

    for (columnIndex = 0; columnIndex < columnCount; columnIndex++) {
        columnCounter[columnIndex] = matrix.column(columnIndex).nonZeros();
        //  LPINFO(columnIndex << " = " << columnCounter[columnIndex]);
    }
    /*LPINFO("------------------------------------");*/

    unsigned int counter;
    bool ready = false;

    std::vector<unsigned int> pivotRows;

    std::vector<char> nonbasicVarialbes(rowCount + columnCount, 1);

    for (counter = 0; counter < rowCount && ready == false; counter++) {
        unsigned int minIndex = 0;
        IndexList<>::Iterator rowCounterIter, rowCounterIterEnd;
        if (min == 0) {
            min = 1;
        }
        minIndex = min;
        rowCounter.getIterators(&rowCounterIter, &rowCounterIterEnd, minIndex, 1);
        while ( minIndex < rowCounter.getPartitionCount() && rowCounterIter == rowCounterIterEnd ) {
            minIndex++;
            if (minIndex < rowCounter.getPartitionCount()) {
                rowCounter.getIterators(&rowCounterIter, &rowCounterIterEnd, minIndex, 1);
            }
        }
        min = minIndex;
        if (minIndex >= columnCount) {
            ready = true;
        } else {

            minIndex = rowCounterIter.getData();
            // LPINFO( "first partition: " << rowCounterIter.getData() << "   " << rowCounterIter.getPartitionIndex() );
            //LPINFO("minimal index: " << minIndex << "  " << rowCounter[minIndex]);

            // choose the column
            rowIndex = minIndex;
            Vector::NonzeroIterator iter = matrix.row(rowIndex).beginNonzero();
            Vector::NonzeroIterator iterEnd = matrix.row(rowIndex).endNonzero();

            while (iter != iterEnd && columnCounter[iter.getIndex()] == 0) {
                iter++;
            }
            if (iter == iterEnd) {
                LPERROR("Hiba " << __LINE__ << "  " << counter);
                // TODO: wrong matrix?
            }
            minIndex = iter.getIndex();
            iter++;
            for (; iter != iterEnd; iter++) {
                if (columnCounter[ iter.getIndex() ] < columnCounter[minIndex] &&
                        columnCounter[ iter.getIndex() ] > 0) {
                    minIndex = iter.getIndex();
                }
            }
            //  LPINFO("minimal column index: " << minIndex << "  " << columnCounter[minIndex]);
            m_basisHead->push_back(minIndex);
            nonbasicVarialbes[minIndex] = 0;

            //columnCounter[minIndex] = 0;

            //rowCounter[rowIndex] = 0;
            /*iter = matrix.column(minIndex).beginNonzero();
            iterEnd = matrix.column(minIndex).endNonzero();
            for (; iter != iterEnd; iter++) {
                //rowCounter[ iter.getIndex() ]--;
                // LPINFO("\t" << iter.getIndex() << "   " << rowCounter.where( iter.getIndex() ));
                unsigned int currentCount = rowCounter.where( iter.getIndex() );
                if (currentCount > 0 && iter.getIndex() != rowIndex) {
                    currentCount--;
                    rowCounter.move( iter.getIndex(), currentCount );
                    if (currentCount < min) {
                        min = currentCount;
                    }
                }
            }*/

            rowCounter.move(rowIndex, 0);
            // LPINFO("where? " << rowCounter.where(rowIndex) );

            iter = matrix.row(rowIndex).beginNonzero();
            iterEnd = matrix.row(rowIndex).endNonzero();
            for (; iter != iterEnd; iter++) {
                columnCounter[ iter.getIndex() ] = 0;
                Vector::NonzeroIterator nonzIter = matrix.column( iter.getIndex() ).beginNonzero();
                Vector::NonzeroIterator nonzIterEnd = matrix.column( iter.getIndex() ).endNonzero();
                for (; nonzIter != nonzIterEnd; nonzIter++) {
                    unsigned int currentCount = rowCounter.where( nonzIter.getIndex() );
                    if (currentCount > 0 && nonzIter.getIndex() != rowIndex) {
                        currentCount--;
                        rowCounter.move( nonzIter.getIndex(), currentCount );
                        if (currentCount < min) {
                            min = currentCount;
                        }
                    }
                }
            }
            columnCounter[minIndex] = 0;
            pivotRows.push_back(rowIndex);
            // std::cin.get();
        }

        //break;
    }

    // insert logical columns
    unsigned int index;
    LPINFO("structural variables: " << m_basisHead->size());
    for (index = m_basisHead->size(); index < rowCount; index++) {
        m_basisHead->push_back( index + columnCount );
        nonbasicVarialbes[index + columnCount] = 0;
    }

    // set variable types
    // basic variables
    for (index = 0; index < rowCount; index++) {
        m_variableStates->insert(Simplex::BASIC, m_basisHead->at(index), &ZERO);
    }

    // nonbasic variables
    for (index = 0; index < rowCount + columnCount; index++) {
        if (nonbasicVarialbes[index] == 1) {
            adjustVariableByType(nonbasicVarialbes[index], Simplex::NONBASIC_AT_LB);
        }
    }

    timer.stop();
    LPINFO("time: " << timer.getTotalElapsed() );

    LPINFO( m_basisHead->size() << "  /  " << rowCount );

    // check the triangularity
    Matrix basis;
    std::set<unsigned int> indices;
    for (index = 0; index < rowCount; index++) {
        //        LPINFO("index: " << m_basisHead->at(index));
        indices.insert(m_basisHead->at(index));

        if (m_basisHead->at(index) < (int)columnCount) {
            basis.appendColumn( matrix.column( m_basisHead->at(index) ) );
        } else {
            Vector unit(rowCount);
            unit.set(m_basisHead->at(index) - columnCount, 1.0);
            basis.appendColumn(unit);
        }

    }
    LPINFO("indexek: " << indices.size() << " / " << rowCount);
    //basis.show();

    Matrix triangle;
    bool error = false;
    ofstream ofs("pattern.txt");
    for (index = 0; index < pivotRows.size(); index++) {
        triangle.appendRow( basis.row( pivotRows[index] ) );
        unsigned int j;
        for (j = index + 1; j < rowCount; j++) {
            if (triangle.get(index, j) != 0.0) {
                error = true;
            }
        }
        for (j = 0; j < rowCount; j++) {
            if (triangle.get(index, j) == 0.0) {
                ofs << " ";
            } else {
                ofs << "X";
            }
        }
        ofs << endl;
    }
    ofs.close();

    ofs.open("basispattern.txt");
    for (int i = 0; i < (int)basis.columnCount(); i++) {
        for (int j = 0; j < (int)basis.rowCount(); j++) {
            if (basis.get(i, j) == 0.0) {
                ofs << " ";
            } else {
                ofs << "X";
            }
        }
        ofs << endl;
    }
    ofs.close();

    if (error) {
        LPERROR("The basis is not triangular!");
    } else {
        LPINFO("Triangular basis");
    }




    //triangle.show();

    //  exit(0);
}
