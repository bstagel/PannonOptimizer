#include <utils/autoparameterizer.h>
#include <simplex/simplexparameterhandler.h>

void AutoParameterizer::selfParameterize()
{
    //Choose primal or dual simplex
    if (m_model.getMatrix().rowCount() > m_model.getMatrix().columnCount()) {
        SimplexParameterHandler::getInstance().setParameterValue("Global.starting_algorithm", "DUAL");
    } else {
        SimplexParameterHandler::getInstance().setParameterValue("Global.starting_algorithm", "PRIMAL");
    }

    //Starting basis
    unsigned degenerate = 0;
    const DenseVector& rhs = m_model.getRhs();
    for(unsigned i = 0; i < m_model.getRowCount(); ++i) {
        if (m_model.getVariable(m_model.getColumnCount() + i).getUpperBound() == rhs.at(i) ||
            m_model.getVariable(m_model.getColumnCount() + i).getLowerBound() == rhs.at(i)) {
            ++degenerate;
        }
    }
    if (degenerate > m_model.getRowCount() * 0.5) {
        LPINFO("Degenerate rhs positions: "<<degenerate);
        SimplexParameterHandler::getInstance().setParameterValue("Starting.Basis.starting_basis_strategy", "CRASH");
    }

    //Scaling

    //inverse
    //Degeneracy
    //Adjust tolerances
}

void AutoParameterizer::printStatistics()
{
    unsigned rows = m_model.getRowCount();
    unsigned columns = m_model.getColumnCount();

    //type of variables
    unsigned varTypes = Variable::VARIABLE_TYPE_ENUM_LENGTH;
    unsigned variables[varTypes];
    for (unsigned i = 0; i < varTypes; ++i){
        variables[i] = 0;
    }
    for (unsigned i = 0; i < rows + columns; ++i){
        Variable::VARIABLE_TYPE type = m_model.getVariable(i).getType();
        ++variables[type];
    }
    LPINFO("Variables:");
    for (unsigned i = 0; i < varTypes; ++i) {
        LPINFO("type "<<i<<": "<<variables[i]);
    }

    //primal infeasibility (considers lower logical starting basis)
    Numerical::Double e_feas = SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_feasibility");
    Numerical::Double primalInfeasibility = 0.0;
    unsigned primalInfeasibleVariables = 0;
    unsigned primalDegenerateVariables = 0;
    const DenseVector& rhs = m_model.getRhs();
    for(unsigned i = 0; i < rows; ++i) {
        Numerical::Double lb = m_model.getVariable(columns + i).getLowerBound();
        Numerical::Double ub = m_model.getVariable(columns + i).getUpperBound();
        Numerical::Double xb = rhs.at(i);
        if (Numerical::equal(xb, lb, e_feas) || Numerical::equal(xb, ub, e_feas)) {
            ++primalDegenerateVariables;
        } else if (xb + e_feas < lb){
            primalInfeasibility += xb - lb;
        } else if (xb - e_feas > ub) {
            ++primalInfeasibleVariables;
            primalInfeasibility -= xb - ub;
        }
    }
    LPINFO("Primal infeasibility: " << primalInfeasibility);
    LPINFO("Infeasible variables: " << primalInfeasibleVariables);
    LPINFO("Degenerate variables: " << primalDegenerateVariables);

    //dual infeasibility (considers lower logical starting basis)
    Numerical::Double e_opt = SimplexParameterHandler::getInstance().getDoubleParameterValue("Tolerances.e_optimality");
    Numerical::Double dualInfeasibility = 0.0;
    unsigned dualInfeasibleVariables = 0;
    unsigned dualDegenerateVariables = 0;
    Variable::VARIABLE_TYPE typeOfIthVariable;
    DenseVector cost = m_model.getCostVector();
    for(unsigned variableIndex = 0; variableIndex < cost.length(); variableIndex++){
        Numerical::Double c_j = cost.at(variableIndex);
        typeOfIthVariable = m_model.getVariable(variableIndex).getType();
        //M type infeasibility
        if (Numerical::fabs(c_j) < e_opt) {
            ++dualDegenerateVariables;
        } else if (c_j < -e_opt && (typeOfIthVariable == Variable::PLUS || typeOfIthVariable == Variable::FREE)) {
            ++dualInfeasibleVariables;
            dualInfeasibility += c_j;
        //P type infeasibility
        } else if (c_j > e_opt && (typeOfIthVariable == Variable::MINUS || typeOfIthVariable == Variable::FREE)) {
            ++dualInfeasibleVariables;
            dualInfeasibility -= c_j;
        }
    }
    LPINFO("Dual infeasibility: " << dualInfeasibility);
    LPINFO("Infeasible variables: " << dualInfeasibleVariables);
    LPINFO("Degenerate variables: " << dualDegenerateVariables);
    exit(0);
}

AutoParameterizer::AutoParameterizer(const SimplexModel &model): m_model(model)
{}

AutoParameterizer::~AutoParameterizer()
{}
