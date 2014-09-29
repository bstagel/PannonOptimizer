/**
 * @file simplexmodel.cpp
 */

#include <simplex/simplexmodel.h>
#include <random>
#include <simplex/simplexparameterhandler.h>

SimplexModel::SimplexModel(const Model & model):
    m_model(model),
    m_perturbedCostVector(false),
    m_perturbedRhs(false),
    m_perturbedBounds(false)
{
    m_rhs.setSparsityRatio(DENSE);
    makeComputationalForm();
}

void SimplexModel::makeComputationalForm()
{
    unsigned int i;

    //Set the matrix
    const Matrix & matrix = getMatrix();
    //Set the constraints
    const std::vector<Constraint> & constraints = getConstraints();

    //Set the cost vector
    const unsigned int rowCount = matrix.rowCount();
    const unsigned int columnCount = matrix.columnCount();
    const unsigned int variablesSize = rowCount + columnCount;

    Vector::NonzeroIterator it = m_model.getCostVector().beginNonzero();
    Vector::NonzeroIterator itEnd = m_model.getCostVector().endNonzero();

    m_costVector.resize(variablesSize);
    m_costVector.setSparsityRatio(DENSE);
    for(;it != itEnd; ++it){
        m_costVector.setNewNonzero(it.getIndex(),*it);
    }

    //Set the variables
    m_variables.resize(variablesSize);

    m_rhs.reInit(rowCount);
    m_rhs.setSparsityRatio(DENSE);


    //Set the structural variables
    for (i=0; i < columnCount; i++) {
        //TODO: Is this fast enough?
        m_variables[i] = m_model.getVariable(i);
    }

    //Set the logical variables
    for (i=0; i < rowCount; i++) {
        m_variables[columnCount + i].setName(constraints[i].getName());

        switch (constraints[i].getType()) {
        case Constraint::LESS_OR_EQUAL: {
            m_variables[columnCount + i].setLowerBound(0.);
            m_variables[columnCount + i].setUpperBound(Numerical::Infinity);
            m_rhs.set(i, constraints[i].getUpperBound());
            break;
        }

        case Constraint::GREATER_OR_EQUAL: {
            m_variables[columnCount + i].setLowerBound(-Numerical::Infinity);
            m_variables[columnCount + i].setUpperBound(0.);
            m_rhs.set(i, constraints[i].getLowerBound());
            break;
        }

        case Constraint::RANGE: {
            m_variables[columnCount + i].setLowerBound(0.);
            m_variables[columnCount + i].setUpperBound(constraints[i].getUpperBound() - constraints[i].getLowerBound());
            m_rhs.set(i, constraints[i].getUpperBound());
            break;
        }

        case Constraint::NON_BINDING: {
            m_variables[columnCount + i].setLowerBound(-Numerical::Infinity);
            m_variables[columnCount + i].setUpperBound(Numerical::Infinity);
            m_rhs.set(i, 0.);
            Numerical::Double temp = constraints[i].getUpperBound();
            Numerical::Double temp2 = constraints[i].getLowerBound();
            if ( temp != -Numerical::Infinity) {
                m_rhs.set(i, temp);
            } else if ( temp2 != Numerical::Infinity) {
                m_rhs.set(i, temp2);
            } else {
                m_rhs.set(i, 0.);
            }
            break;
        }

        case Constraint::EQUALITY: {
            m_variables[columnCount + i].setLowerBound(0.);
            m_variables[columnCount + i].setUpperBound(0.);
            m_rhs.set(i, constraints[i].getUpperBound());
            break;
        }

        default: {
            LPWARNING("Unhandled case");
            break;
        }
        }
    }
}

void SimplexModel::print(std::ostream& out) const
{
    out << (getObjectiveType()==MINIMIZE?"min":"max");
    Vector::Iterator it = getCostVector().begin();
    Vector::Iterator end = getCostVector().end();
    for(unsigned int i=0; it<end; i++, ++it){
        out << (i==0?" ":"+ ") << *it << "*x" << i << " \n";
    }
    out << "\n s.t.: \n";
    for (unsigned int i = 0; i<getMatrix().rowCount();i++){
        const Vector & row=getMatrix().row(i);
        Vector::Iterator it = row.begin();
        Vector::Iterator end = row.end();
        for(unsigned int j=0; it<end; j++, ++it){
            out << (j==0?" ":"+ ") << *it << "*x" << j << " ";
        }
        out << " + y" << i << " = " << m_rhs.at(i) <<"\n";
    }
    out << "\n";
    unsigned int columnCount = getColumnCount();
    for(unsigned int i = 0; i<getVariables().size();i++){
        const Variable & variable = getVariable(i);
        if(i<columnCount){
            out << variable.getLowerBound() << " x" << i << " " << variable.getUpperBound()<<"\n";
        } else {
            out << variable.getLowerBound() << " y" << i-columnCount << " " << variable.getUpperBound()<<"\n";
        }
    }
}

void SimplexModel::perturbCostVector()
{
    m_originalCostVector = m_costVector;
    //generate random epsilon values
    const double & epsilon = SimplexParameterHandler::getInstance().getDoubleParameterValue("Perturbation.e_cost_vector");
    const std::string & perturbMethod = SimplexParameterHandler::getInstance().getStringParameterValue("Perturbation.perturb_cost_vector");
    const std::string & perturbTarget = SimplexParameterHandler::getInstance().getStringParameterValue("Perturbation.perturb_target");
    const bool & perturbLogical = SimplexParameterHandler::getInstance().getBoolParameterValue("Perturbation.perturb_logical");
    std::default_random_engine engine;
    Vector epsilonValues;
    epsilonValues.resize(getColumnCount()+getRowCount());
    LPINFO("Cost vector perturbation with e = " << epsilon <<
           " method: " << perturbMethod << " target: " << perturbTarget << " logical: " << perturbLogical);

    //in random directions
    if(perturbMethod == "STRUCTURAL_RANDOM"){
        std::uniform_real_distribution<double> distribution(-epsilon,epsilon);
        //structural
        if(!perturbLogical){
            for(unsigned i=0;i < getColumnCount();i++){
                if(perturbTarget == "ALL"){
                    epsilonValues.set(i,distribution(engine));
                }else if(perturbTarget == "NONZEROS" && m_costVector.at(i) != 0){
                    epsilonValues.set(i,distribution(engine));
                }else if(perturbTarget == "ZEROS" && m_costVector.at(i) == 0){
                    epsilonValues.set(i,distribution(engine));
                }
            }
        //logical
        }else{
            for(unsigned i=0;i < getColumnCount()+getRowCount();i++){
                if(perturbTarget == "ALL"){
                    epsilonValues.set(i,distribution(engine));
                }else if(perturbTarget == "NONZEROS" && m_costVector.at(i) != 0){
                    epsilonValues.set(i,distribution(engine));
                }else if(perturbTarget == "ZEROS" && m_costVector.at(i) == 0){
                    epsilonValues.set(i,distribution(engine));
                }
            }
        }

    //in feasible direction
    }else if(perturbMethod == "STRUCTURAL_FEASIBLE"){
        std::uniform_real_distribution<double> negDistribution(-1.5*epsilon,-0.5*epsilon);
        std::uniform_real_distribution<double> posDistribution(0.5*epsilon,1.5*epsilon);
//        std::uniform_real_distribution<double> negDistribution(-epsilon,0);
//        std::uniform_real_distribution<double> posDistribution(0,epsilon);
        std::uniform_real_distribution<double> distribution(-epsilon,epsilon);
        //structural
        if(!perturbLogical){
            for(unsigned i=0;i < getColumnCount();i++){
                Variable::VARIABLE_TYPE variableType = getVariable(i).getType();
                switch(variableType){
                //PLUS in positive direction (thus d_j >= 0 is feasible)
                case Variable::PLUS:{
                    if(perturbTarget == "ALL"){
                        epsilonValues.set(i,posDistribution(engine));
                    }else if(perturbTarget == "ZEROS" && m_costVector.at(i) == 0){
                        epsilonValues.set(i,posDistribution(engine));
                    }else if(perturbTarget == "NONZEROS" && m_costVector.at(i) != 0){
                        epsilonValues.set(i,posDistribution(engine));
                    }
                    break;
                }
                //MINUS in negative direction (thus d_j <= 0 is feasible)
                case Variable::MINUS:{
                    if(perturbTarget == "ALL"){
                        epsilonValues.set(i,negDistribution(engine));
                    }else if(perturbTarget == "ZEROS" && m_costVector.at(i) == 0){
                        epsilonValues.set(i,negDistribution(engine));
                    }else if(perturbTarget == "NONZEROS" && m_costVector.at(i) != 0){
                        epsilonValues.set(i,negDistribution(engine));
                    }
                    break;
                }
                case Variable::BOUNDED:{
                    if(perturbTarget == "ALL"){
                        epsilonValues.set(i,distribution(engine));
                    }else if(perturbTarget == "ZEROS" && m_costVector.at(i) == 0){
                        epsilonValues.set(i,distribution(engine));
                    }else if(perturbTarget == "NONZEROS" && m_costVector.at(i) != 0){
                        epsilonValues.set(i,distribution(engine));
                    }
                    break;
                }
                default:
                    epsilonValues.set(i,0);
                    break;
                }
            }
        //logical
        }else{
            for(unsigned i=0;i < getColumnCount()+getRowCount();i++){
                Variable::VARIABLE_TYPE variableType = getVariable(i).getType();
                switch(variableType){
                //PLUS in positive direction (thus d_j >= 0 is feasible)
                case Variable::PLUS:{
                    if(perturbTarget == "ALL"){
                        epsilonValues.set(i,posDistribution(engine));
                    }else if(perturbTarget == "ZEROS" && m_costVector.at(i) == 0){
                        epsilonValues.set(i,posDistribution(engine));
                    }else if(perturbTarget == "NONZEROS" && m_costVector.at(i) != 0){
                        epsilonValues.set(i,posDistribution(engine));
                    }
                    break;
                }
                //MINUS in negative direction (thus d_j <= 0 is feasible)
                case Variable::MINUS:{
                    if(perturbTarget == "ALL"){
                        epsilonValues.set(i,negDistribution(engine));
                    }else if(perturbTarget == "ZEROS" && m_costVector.at(i) == 0){
                        epsilonValues.set(i,negDistribution(engine));
                    }else if(perturbTarget == "NONZEROS" && m_costVector.at(i) != 0){
                        epsilonValues.set(i,negDistribution(engine));
                    }
                    break;
                }
                case Variable::BOUNDED:{
                    if(perturbTarget == "ALL"){
                        epsilonValues.set(i,distribution(engine));
                    }else if(perturbTarget == "ZEROS" && m_costVector.at(i) == 0){
                        epsilonValues.set(i,distribution(engine));
                    }else if(perturbTarget == "NONZEROS" && m_costVector.at(i) != 0){
                        epsilonValues.set(i,distribution(engine));
                    }
                    break;
                }
                default:
                    epsilonValues.set(i,0);
                    break;
                }
            }
        }

    //with sign of c_j
    }else if(perturbMethod == "STRUCTURAL_SIGN"){
//        std::uniform_real_distribution<double> negDistribution(-1.5*epsilon,-0.5*epsilon);
//        std::uniform_real_distribution<double> posDistribution(0.5*epsilon,1.5*epsilon);
        std::uniform_real_distribution<double> negDistribution(-epsilon,0);
        std::uniform_real_distribution<double> posDistribution(0,epsilon);
        //structural
        if(!perturbLogical){
            for(unsigned i=0;i < getColumnCount();i++){
                if( m_costVector.at(i) < 0 && (perturbTarget == "ALL" || perturbTarget == "NONZEROS") ){
                    epsilonValues.set(i,negDistribution(engine));
                }else if(m_costVector.at(i) > 0 && (perturbTarget == "ALL" || perturbTarget == "NONZEROS") ){
                    epsilonValues.set(i,posDistribution(engine));
                }else if(m_costVector.at(i) == 0 && (perturbTarget == "ALL" || perturbTarget == "ZEROS")){
                    epsilonValues.set(i,posDistribution(engine));
                }
            }
        //logical
        }else{
            for(unsigned i=0;i < getColumnCount()+getRowCount();i++){
                if(m_costVector.at(i) < 0 && (perturbTarget == "ALL" || perturbTarget == "NONZEROS") ){
                    epsilonValues.set(i,negDistribution(engine));
                }else if(m_costVector.at(i) > 0 && (perturbTarget == "ALL" || perturbTarget == "NONZEROS") ){
                    epsilonValues.set(i,posDistribution(engine));
                }else if(m_costVector.at(i) == 0 && (perturbTarget == "ALL" || perturbTarget == "ZEROS")){
                    epsilonValues.set(i,posDistribution(engine));
                }
            }
        }
    }

    //perturbation: c+epsilonValues
    m_costVector.addVector(1,epsilonValues);
    m_perturbedCostVector = true;
}

void SimplexModel::perturbRHS()
{
    LPINFO("RHS perturbation");
    m_originalRhs = m_rhs;
    //generate random epsilon values
    const double & epsilon = SimplexParameterHandler::getInstance().getDoubleParameterValue("Perturbation.e_rhs");
    std::default_random_engine engine;
    std::uniform_real_distribution<double> distribution(-epsilon,epsilon);
    Vector epsilonValues;

    epsilonValues.resize(m_rhs.length());
    for(unsigned i=0;i < m_rhs.length();i++){
        epsilonValues.insertElement(i,distribution(engine));
    }

    //perturbation
    m_rhs.addVector(1,epsilonValues);

    m_perturbedRhs = true;
}

void SimplexModel::shiftBounds()
{
    LPINFO("Bound shifting");
    //generate random epsilon values
    const double & epsilon = SimplexParameterHandler::getInstance().getDoubleParameterValue("Perturbation.e_bounds");
    std::default_random_engine engine;
    std::uniform_real_distribution<double> distribution(-epsilon,epsilon);
    //TODO: Ezt mashogy kell lefoglalni, feltolteni
    Vector epsilonValuesLower;
    Vector epsilonValuesUpper;
    for(unsigned i=0;i < getColumnCount();i++){
        epsilonValuesLower.insertElement(i,distribution(engine));
        epsilonValuesUpper.insertElement(i,distribution(engine));
    }
    Numerical::Double lb = 0;
    Numerical::Double ub = 0;

    for(unsigned i=0;i < getColumnCount();i++){
        lb = getVariable(i).getLowerBound();
        ub = getVariable(i).getUpperBound();
        if (lb != -Numerical::Infinity){
            m_variables[i].setLowerBound(lb+epsilonValuesLower.at(i));
        }
        if (ub != Numerical::Infinity){
            m_variables[i].setUpperBound(ub+epsilonValuesUpper.at(i));
        }
    }

    m_perturbedBounds = true;
}


void SimplexModel::resetModel()
{
    if (m_perturbedCostVector != 0){
        m_costVector.clear();
        m_costVector = m_originalCostVector;
    }
    if (m_perturbedRhs != 0){
        m_rhs.clear();
        m_rhs = m_originalRhs;
    }
    if (m_perturbedBounds != 0){
        Numerical::Double lb = 0;
        Numerical::Double ub = 0;
        for(unsigned i=0;i < getColumnCount();i++){
            lb = getVariable(i).getLowerBound();
            ub = getVariable(i).getUpperBound();
            m_variables[i].setLowerBound(lb);
            m_variables[i].setUpperBound(ub);
        }
    }
}
