/**
 * @file model.cpp
 */

#include <lp/model.h>
#include <utils/sha1.h>

Model::Model()
{
    m_costConstant = 0.0;
    m_objectiveType = MINIMIZE;
    m_scaled = false;
    m_presolved = false;
}

void Model::build(const ModelBuilder & builder)
{
    unsigned int index;
    m_name = builder.getName();
    m_objectiveType = builder.getObjectiveType();
    m_costConstant = builder.getObjectiveConstant();
    builder.buildCostVector(&m_costVector);

    m_constraints.resize(builder.getRowCount());
    m_variables.resize(builder.getColumnCount());

    m_matrix.build(builder);

    for (index = 0; index < m_variables.size(); index++) {
        m_variables[index] = builder.getVariable(index);
        m_variables[index].setVector(m_matrix.column(index));
    }

    for (index = 0; index < m_constraints.size(); index++) {
        m_constraints[index] = builder.getConstraint(index);
        m_constraints[index].setVector(m_matrix.row(index));
    }

    m_matrix.sortVectors();

    /*m_matrix.reInit(builder.getRowCount(), builder.getColumnCount(), false);

    unsigned int rowIndex;
    for (rowIndex = 0; rowIndex < builder.getRowCount(); rowIndex++) {
        Vector * rowVector = new Vector;
        m_matrix.m_rowWise[rowIndex] = rowVector;
        builder.buildRow(rowIndex, rowVector);
        m_constraints[rowIndex].setVector(*rowVector);
    }

    unsigned int columnIndex;
    for (columnIndex = 0; columnIndex < builder.getColumnCount(); columnIndex++) {
        Vector * columnVector = new Vector;
        m_matrix.m_columnWise[columnIndex] = columnVector;
        builder.buildColumn(columnIndex, columnVector);
        m_constraints[columnIndex].setVector(*columnVector);
    }*/
}

Model::~Model()
{
    if(m_presolved) {
        for(unsigned int i = 0; i < m_substituteVectors->size(); i++) {
            delete (*m_substituteVectors)[i];
        }
    }
}

void Model::print(std::ostream& out) const
{
    out << (m_objectiveType == MINIMIZE ? "min" : "max");
    DenseVector::Iterator it = m_costVector.begin();
    DenseVector::Iterator end = m_costVector.end();
    // For maximization problems the cost vector is multiplied by -1
    if(m_objectiveType == MINIMIZE){
        for (unsigned int i = 0; it < end; i++, ++it) {
            out << (i == 0 ? " " : "+ ") << *it << "*x" << i;
        }
        if(m_costConstant != 0){
            out << "+ " << m_costConstant;
        }
        out << "\n";
    } else {
        for (unsigned int i = 0; it < end; i++, ++it) {
            out << (i == 0 ? " " : "+ ") << -(*it) << "*x" << i;
        }
        if(m_costConstant != 0){
            out << "+ " << -m_costConstant;
        }
        out << "\n";
    }
    out << "\n s.t.: \n";
    for (unsigned int i = 0; i < m_matrix.rowCount(); i++) {
        out << m_constraints[i].getLowerBound() << " <= ";
        const SparseVector & row = m_matrix.row(i);
        SparseVector::NonzeroIterator it = row.beginNonzero();
        SparseVector::NonzeroIterator end = row.endNonzero();
        for (unsigned int j = 0; it < end; j++, ++it) {
            out << (j == 0 ? " " : "+ ") << *it << "*x" << j << " ";
        }
        out << "<= " << m_constraints[i].getUpperBound() << "\n";
    }
    out << "\n";
    for (unsigned int i = 0; i < m_variables.size(); i++) {
        const Variable & variable = m_variables[i];
        out << variable.getLowerBound() << " x" << i << " " << variable.getUpperBound();
    }
}

//void Model::print(std::ostream& out) const
//{
//    out << (m_objectiveType == MINIMIZE ? "min" : "max");
//    Vector::Iterator it = m_costVector.begin();
//    Vector::Iterator end = m_costVector.end();
//    for (unsigned int i = 0; it < end; i++, ++it) {
//        out << (i == 0 ? " " : "+ ") << *it << "*x" << i << " \n";
//    }
//    out << "\n s.t.: \n";
//    for (unsigned int i = 0; i < m_matrix.rowCount(); i++) {
//        out << m_constraints[i].getLowerBound() << " <= ";
//        const Vector & row = m_matrix.row(i);
//        Vector::Iterator it = row.begin();
//        Vector::Iterator end = row.end();
//        for (unsigned int j = 0; it < end; j++, ++it) {
//            out << (j == 0 ? " " : "+ ") << *it << "*x" << j << " ";
//        }
//        out << "<= " << m_constraints[i].getUpperBound() << "\n";
//    }
//    out << "\n";
//    for (unsigned int i = 0; i < m_variables.size(); i++) {
//        const Variable & variable = m_variables[i];
//        out << variable.getLowerBound() << " x" << i << " " << variable.getUpperBound();
//    }
//}

void Model::clear()
{
    m_matrix.clear();
    m_variables.clear();
    m_constraints.clear();

}

void Model::addVariable(const Variable & variable, const SparseVector & column)
{
    m_variables.push_back(variable);
    m_matrix.appendColumn(column);
}

void Model::addConstraint(const Constraint & constraint, const SparseVector &row)
{
    m_constraints.push_back(constraint);
    m_matrix.appendRow(row);
}

void Model::scale() {
    Numerical::Double lowerBound;
    Numerical::Double upperBound;

    Scaler scaler;
    scaler.scale( &m_matrix, m_name.c_str() );

    m_scaled = true;
    m_rowMultipliers = scaler.getRowMultipliers();
    m_columnMultipliers = scaler.getColumnMultipliers();
    auto & rowMultipliers = scaler.getRowMultipliers();

    auto & columnMultipliers = scaler.getColumnMultipliers();

    auto mulIter = rowMultipliers.begin();
    std::vector<Constraint>::iterator constraintIter = m_constraints.begin();
    std::vector<Constraint>::iterator constraintIterEnd = m_constraints.end();
    for (; constraintIter < constraintIterEnd; constraintIter++, mulIter++) {
        lowerBound = constraintIter->getLowerBound() * *mulIter;
        upperBound = constraintIter->getUpperBound() * *mulIter;

        try {
            constraintIter->setBounds(lowerBound, upperBound);
        } catch ( Constraint::InvalidLowerBoundException & ex ) {
            LPERROR(ex.getMessage());
            exit(1);
        }
    }

    mulIter = columnMultipliers.begin();
    std::vector<Variable>::iterator variableIter = m_variables.begin();
    std::vector<Variable>::iterator variableIterEnd = m_variables.end();
    unsigned int index = 0;
    for (; variableIter < variableIterEnd; variableIter++, mulIter++, index++) {
        lowerBound = variableIter->getLowerBound() / *mulIter;
        upperBound = variableIter->getUpperBound() / *mulIter;
        variableIter->setBounds(lowerBound, upperBound);
        m_costVector.set( index, m_costVector.at(index) * *mulIter );
    }

    // TODO: ezt majd okosabban kellene, jobb ha a scaler mondja meg
    m_history.push_back("Scaler: Benichou");
}

std::string Model::getHash() const {
    if (m_hash.size() > 0) {
        return m_hash;
    }

    SHA1Generator generator;
    generator.start();

    generator.addString( m_name );
    unsigned int intTemp;
    intTemp = m_matrix.rowCount();
    generator.addBuffer(&intTemp, sizeof(intTemp));
    intTemp = m_matrix.columnCount();
    generator.addBuffer(&intTemp, sizeof(intTemp));
    if ( m_objectiveType == MINIMIZE ) {
        intTemp = 0;
    } else {
        intTemp = 1;
    }
    generator.addBuffer(&intTemp, sizeof(intTemp));

    double doubleTemp;
    doubleTemp = Numerical::DoubleToIEEEDouble(this->m_costConstant);
    generator.addBuffer(&doubleTemp, sizeof(doubleTemp));

    // cost vector
    generator.addString( m_objectiveRowName );
    intTemp = m_costVector.nonZeros();
    generator.addBuffer(&intTemp, sizeof(intTemp));
    DenseVector::NonzeroIterator nonzIter = m_costVector.beginNonzero();
    DenseVector::NonzeroIterator nonzIterEnd = m_costVector.endNonzero();
    for (; nonzIter != nonzIterEnd; ++nonzIter) {
        intTemp = nonzIter.getIndex();
        generator.addBuffer(&intTemp, sizeof(intTemp));
        doubleTemp = Numerical::DoubleToIEEEDouble(*nonzIter);
        generator.addBuffer(&doubleTemp, sizeof(doubleTemp));
    }

    // variables
    std::vector<Variable>::const_iterator varIter = m_variables.begin();
    std::vector<Variable>::const_iterator varIterEnd = m_variables.end();
    for (; varIter != varIterEnd; ++varIter) {
        generator.addString( varIter->getName() );
        intTemp = varIter->getType();
        generator.addBuffer(&intTemp, sizeof(intTemp));
        doubleTemp = Numerical::DoubleToIEEEDouble(varIter->getLowerBound());
        generator.addBuffer(&doubleTemp, sizeof(doubleTemp));
        doubleTemp = Numerical::DoubleToIEEEDouble(varIter->getUpperBound());
        generator.addBuffer(&doubleTemp, sizeof(doubleTemp));
    }
    // constraints
    std::vector<Constraint>::const_iterator constrIter = m_constraints.begin();
    std::vector<Constraint>::const_iterator constrIterEnd = m_constraints.end();
    for (; constrIter != constrIterEnd; constrIter++) {
        generator.addString( constrIter->getName() );
        intTemp = constrIter->getType();
        generator.addBuffer(&intTemp, sizeof(intTemp));
        doubleTemp = Numerical::DoubleToIEEEDouble(constrIter->getLowerBound());
        generator.addBuffer(&doubleTemp, sizeof(doubleTemp));
        doubleTemp = Numerical::DoubleToIEEEDouble(constrIter->getUpperBound());
        generator.addBuffer(&doubleTemp, sizeof(doubleTemp));
    }
    // matrix
    unsigned int rowIndex;
    for (rowIndex = 0; rowIndex < m_matrix.rowCount(); rowIndex++) {
        const SparseVector & row = m_matrix.row(rowIndex);
        intTemp = row.nonZeros();
        generator.addBuffer(&intTemp, sizeof(intTemp));
        SparseVector::NonzeroIterator nonzIterSparse = row.beginNonzero();
        SparseVector::NonzeroIterator nonzIterEndSparse = row.endNonzero();
        for (; nonzIterSparse != nonzIterEndSparse; ++nonzIterSparse) {
            intTemp = nonzIterSparse.getIndex();
            generator.addBuffer(&intTemp, sizeof(intTemp));
            doubleTemp = Numerical::DoubleToIEEEDouble(*nonzIterSparse);
            generator.addBuffer(&doubleTemp, sizeof(doubleTemp));
        }
    }

    m_hash = SHA1Generator::convertHashToString( generator.end() );
    return m_hash;
}

/**************************************************************
 * PRESOLVER API IMPLEMENTATION
 *************************************************************/

void Model::removeVariable(unsigned int index) {
    m_matrix.removeColumn(index);
    m_costVector.remove(index);
    m_variables.erase( m_variables.begin() + index );
}

void Model::removeConstraint(unsigned int index) {
    m_matrix.removeRow(index);
    m_constraints.erase( m_constraints.begin() + index );
}

void Model::addToConstraint(unsigned int dest, unsigned int source, Numerical::Double lambda) {
    SparseVector row = m_matrix.row(dest);
    row.addVector(lambda, m_matrix.row(source));
    unsigned int index;
    for (index = 0; index < m_matrix.columnCount(); index++) {
        m_matrix.set(dest, index, row.at(index));
    }
}

void Model::addToCostVector(unsigned int source, Numerical::Double lambda) {
    m_costVector.addVector(lambda, m_matrix.row(source));
}

void Model::addToCostCoefficient(unsigned int index, Numerical::Double value) {
    m_costVector.set( index, Numerical::stableAdd(m_costVector.at(index), value) );
}

void Model::setCostConstant(Numerical::Double value) {
    m_costConstant = value;
}

void Model::setSubstitueVectors(std::vector<DenseVector *> * substituteVectors) {
    m_substituteVectors = substituteVectors;
    m_presolved = true;
}

void Model::changeRowName(int rowIndex, std::string& name){
    m_constraints[rowIndex].setName(name.c_str());
}

void Model::changeColumnName(int columnIndex, std::string& name){
    m_variables[columnIndex].setName(name.c_str());
}

void Model::changeConstraintLower(const double* rowLower){
    for(unsigned int i=0; i<m_constraints.size(); i++){
        m_constraints[i].setLowerBound(rowLower[i]);
    }
}

void Model::changeConstraintUpper(const double* rowUpper){
    for(unsigned int i=0; i<m_constraints.size(); i++){
        m_constraints[i].setUpperBound(rowUpper[i]);
    }
}

void Model::changeVariableLower(const double* colLower){
    for(unsigned int i=0; i<m_variables.size(); i++){
        m_variables[i].setLowerBound(colLower[i]);
    }
}

void Model::changeVariableUpper(const double* colUpper){
    for(unsigned int i=0; i<m_variables.size(); i++){
        m_variables[i].setUpperBound(colUpper[i]);
    }
}

void Model::changeObjectiveCoefficients(const double* objCoefficients){
    m_costVector.createVectorFromDenseArray(objCoefficients,m_variables.size());
}
