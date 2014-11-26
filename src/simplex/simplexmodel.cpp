/**
 * @file simplexmodel.cpp
 */

#include <simplex/simplexmodel.h>
#include <random>
#include <cmath>
#include <simplex/simplexparameterhandler.h>

SimplexModel::SimplexModel(const Model & model):
    m_model(model),
    m_perturbedCostVector(false),
    m_perturbedRhs(false),
    m_perturbedBounds(false)
{
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

    DenseVector::NonzeroIterator it = m_model.getCostVector().beginNonzero();
    DenseVector::NonzeroIterator itEnd = m_model.getCostVector().endNonzero();

    m_costVector.resize(variablesSize);
    for(;it != itEnd; ++it){
        m_costVector.set(it.getIndex(),*it);
    }

    //Set the variables
    m_variables.resize(variablesSize);

    m_rhs.reInit(rowCount);


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
    DenseVector::Iterator it = getCostVector().begin();
    DenseVector::Iterator end = getCostVector().end();
    for(unsigned int i=0; it<end; i++, ++it){
        out << (i==0?" ":"+ ") << *it << "*x" << i << " \n";
    }
    out << "\n s.t.: \n";
    for (unsigned int i = 0; i<getMatrix().rowCount();i++){
        const SparseVector & row=getMatrix().row(i);
        SparseVector::NonzeroIterator it = row.beginNonzero();
        SparseVector::NonzeroIterator end = row.endNonzero();
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
    //parameter references
    const double & epsilon = SimplexParameterHandler::getInstance().getDoubleParameterValue("Perturbation.e_cost_vector");
    const std::string & perturbMethod = SimplexParameterHandler::getInstance().getStringParameterValue("Perturbation.perturb_cost_vector");
    const std::string & perturbTarget = SimplexParameterHandler::getInstance().getStringParameterValue("Perturbation.perturb_target");
    const bool & perturbLogical = SimplexParameterHandler::getInstance().getBoolParameterValue("Perturbation.perturb_logical");
    const Numerical::Double & psi = SimplexParameterHandler::getInstance().getDoubleParameterValue("Perturbation.psi");
    const Numerical::Double & tolerance = SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_optimality");
    const Numerical::Double & xi = SimplexParameterHandler::getInstance().getDoubleParameterValue("Perturbation.xi_multiplier") *
                                    tolerance;
    const std::string & weighting = SimplexParameterHandler::getInstance().getStringParameterValue("Perturbation.weighting");

    std::default_random_engine engine;
    DenseVector epsilonValues(getColumnCount()+getRowCount());
    LPINFO("Cost vector perturbation with e = " << epsilon <<
           " method: " << perturbMethod << " target: " << perturbTarget << " logical: " << perturbLogical);

    unsigned numberOfPerturbations = 0;
    if(perturbLogical){
        numberOfPerturbations = getColumnCount() + getRowCount();
    }else{
        numberOfPerturbations = getColumnCount();
    }

    //in random directions
    if(perturbMethod == "RANDOM"){
        std::uniform_real_distribution<double> distribution(-epsilon,epsilon);
        for(unsigned i=0;i < numberOfPerturbations;i++){
            if(perturbTarget == "ALL"){
                epsilonValues.set(i,distribution(engine));
            }else if(perturbTarget == "NONZEROS" && m_costVector.at(i) != 0){
                epsilonValues.set(i,distribution(engine));
            }else if(perturbTarget == "ZEROS" && m_costVector.at(i) == 0){
                epsilonValues.set(i,distribution(engine));
            }
        }

    //in feasible direction
    }else if(perturbMethod == "FEASIBLE"){
        std::uniform_real_distribution<double> negDistribution(-epsilon,-0.5*epsilon);
        std::uniform_real_distribution<double> posDistribution(0.5*epsilon,epsilon);
        std::uniform_int_distribution<int> signDistribution(0,1);
        for(unsigned i=0;i < numberOfPerturbations;i++){
            Variable::VARIABLE_TYPE variableType = getVariable(i).getType();
            switch(variableType){
            //PLUS in positive direction (thus d_j >= 0 is feasible)
            case Variable::PLUS:{
                if(perturbTarget == "ALL"){
                    epsilonValues.set(i,posDistribution(engine) + Numerical::fabs(m_costVector.at(i)) * psi);
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
                    epsilonValues.set(i,negDistribution(engine) - Numerical::fabs(m_costVector.at(i)) * psi);
                }else if(perturbTarget == "ZEROS" && m_costVector.at(i) == 0){
                    epsilonValues.set(i,negDistribution(engine));
                }else if(perturbTarget == "NONZEROS" && m_costVector.at(i) != 0){
                    epsilonValues.set(i,negDistribution(engine));
                }
                break;
            }
            case Variable::BOUNDED:{
//                if(perturbTarget == "ALL"){
//                    epsilonValues.set(i,(signDistribution(engine)-0.5)*2*posDistribution(engine));
//                }else if(perturbTarget == "ZEROS" && m_costVector.at(i) == 0){
//                    epsilonValues.set(i,(signDistribution(engine)-0.5)*2*posDistribution(engine));
//                }else if(perturbTarget == "NONZEROS" && m_costVector.at(i) != 0){
//                    epsilonValues.set(i,(signDistribution(engine)-0.5)*2*posDistribution(engine));
//                }
                if( m_costVector.at(i) < 0 && (perturbTarget == "ALL" || perturbTarget == "NONZEROS") ){
                    epsilonValues.set(i,negDistribution(engine) - Numerical::fabs(m_costVector.at(i)) * psi);
                }else if(m_costVector.at(i) > 0 && (perturbTarget == "ALL" || perturbTarget == "NONZEROS") ){
                    epsilonValues.set(i,posDistribution(engine) + Numerical::fabs(m_costVector.at(i)) * psi);
               }else if(m_costVector.at(i) == 0 && (perturbTarget == "ALL" || perturbTarget == "ZEROS")){
                    epsilonValues.set(i,(signDistribution(engine)-0.5)*2*(posDistribution(engine) + Numerical::fabs(m_costVector.at(i)) * psi));
                }
                break;
            }
            default:{
                break;
            }
            }
        }

    //with sign of c_j
    }else if(perturbMethod == "SIGN"){
        std::uniform_real_distribution<double> negDistribution(-epsilon,-0.5*epsilon);
        std::uniform_real_distribution<double> posDistribution(0.5*epsilon,epsilon);
        std::uniform_int_distribution<int> signDistribution(0,1);
        for(unsigned i=0;i < numberOfPerturbations;i++){
            if( m_costVector.at(i) < 0 && (perturbTarget == "ALL" || perturbTarget == "NONZEROS") ){
                epsilonValues.set(i,negDistribution(engine));
            }else if(m_costVector.at(i) > 0 && (perturbTarget == "ALL" || perturbTarget == "NONZEROS") ){
                epsilonValues.set(i,posDistribution(engine));
            }else if(m_costVector.at(i) == 0 && (perturbTarget == "ALL" || perturbTarget == "ZEROS")){
                epsilonValues.set(i,(signDistribution(engine)-0.5)*2*posDistribution(engine));
            }
        }
    //Koberstein
    }else if(perturbMethod == "KOBERSTEIN"){
        //considering size of c_j and fix part
        for(unsigned i=0; i < numberOfPerturbations; i++){
            epsilonValues.set(i, (xi + Numerical::fabs(m_costVector.at(i)) * psi) );
        }

        //considering types of variables
        std::uniform_real_distribution<double> distribution(0.5,1);
        for(unsigned i=0; i < numberOfPerturbations; i++){
            //those with finite upper bound
            if(getVariable(i).getUpperBound() != Numerical::Infinity){
                epsilonValues.set(i, - distribution(engine) * epsilonValues.at(i) );
            }else{
                epsilonValues.set(i, distribution(engine) * epsilonValues.at(i) );
            }
        }
    }

    //considering number of nonzeros (alpha column)
    if(weighting == "WEIGHT" || weighting == "SET_TO_INTERVAL"){
        std::vector<Numerical::Double> weightVector{0, 0.01, 0.1, 1, 2, 5, 10, 20, 30, 40, 100};

//        int maxNonzero = 0;
//        for(unsigned i=0; i < getColumnCount(); ++i){
//            unsigned nonzeros = getMatrix().column(i).nonZeros();
//            if(nonzeros > maxNonzero){
//                maxNonzero = nonzeros;
//            }
//        }
//        std::vector<Numerical::Double> weightVector;
//        weightVector.resize(maxNonzero);
//        for(unsigned i=0; i < weightVector.size(); ++i){
////            weightVector[i] = log2(i+2);
//            weightVector[i] = i+1;
//        }
//        for(unsigned i=0; i < getColumnCount(); ++i){
//            unsigned nonzeros = getMatrix().column(i).nonZeros();
//            epsilonValues.set(i,weightVector[nonzeros-1] * epsilonValues.at(i));
//        }

        for(unsigned i=0; i < getColumnCount(); ++i){
            unsigned nonzeros = getMatrix().column(i).nonZeros();
            if(nonzeros > 9){
                epsilonValues.set(i,weightVector[9] * epsilonValues.at(i));
            }else{
                epsilonValues.set(i,weightVector[nonzeros] * epsilonValues.at(i));
            }
        }
        //logical alpha vectors have one nonzero element
        if(perturbLogical){
           for(unsigned i=getColumnCount(); i < getColumnCount()+getRowCount(); ++i) {
               epsilonValues.set(i,weightVector[1] * epsilonValues.at(i));
           }
        }
    }

    //checking the interval
    if(weighting == "SET_TO_INTERVAL"){
        Numerical::Double minValue = 1E-2 * tolerance < psi ? 1E-2 * tolerance : psi;
        Numerical::Double avg = 0;
        //summarize nonzero c_j values
        auto it = m_costVector.beginNonzero();
        auto endit = m_costVector.endNonzero();
        int n = m_costVector.length();
        for(; it != endit; ++it){
            avg+=(Numerical::fabs(*it)-avg)/n;
        }
        Numerical::Double maxValue = 1E+3 * tolerance > psi * 10 * avg ? 1E+3 * tolerance : psi * 10 * avg;
        LPINFO("Setting to interval: (" << std::scientific << minValue << "," << maxValue << ")");
        for(unsigned i=0; i < numberOfPerturbations; i++){
            Numerical::Double absEpsilon = Numerical::fabs(epsilonValues.at(i));
            double multiplier = 1;
            if(absEpsilon == 0){
                epsilonValues.set(i,minValue);
                absEpsilon = Numerical::fabs(epsilonValues.at(i));
            }
            while(absEpsilon > maxValue){
                multiplier *= 0.1;
                absEpsilon *= 0.1;
            }
            while(absEpsilon < minValue){
                multiplier *= 10;
                absEpsilon *= 10;
            }
            if(multiplier != 1){
                epsilonValues.set(i,epsilonValues.at(i) * multiplier);
            }
        }
    }

    //perturbation: c + epsilonValues
    //TODO: ADD_FAST kapcsolo hasznalata?
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
    DenseVector epsilonValues;

    epsilonValues.resize(m_rhs.length());
    for(unsigned i=0;i < m_rhs.length();i++){
        epsilonValues.insert(i,distribution(engine));
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
    if (m_perturbedCostVector){
        LPINFO("Resetting cost vector!");
        m_costVector = m_originalCostVector;
        m_perturbedCostVector = false;
    }
    if (m_perturbedRhs){
        m_rhs = m_originalRhs;
        m_perturbedRhs = false;
    }
    if (m_perturbedBounds){
        Numerical::Double lb = 0;
        Numerical::Double ub = 0;
        for(unsigned i=0;i < getColumnCount();i++){
            lb = getVariable(i).getLowerBound();
            ub = getVariable(i).getUpperBound();
            m_variables[i].setLowerBound(lb);
            m_variables[i].setUpperBound(ub);
        }
        m_perturbedBounds = false;
    }
}
