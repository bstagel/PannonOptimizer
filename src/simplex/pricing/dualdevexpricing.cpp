/**
 * @file   dualdantzigpricing.cpp
 */

#include <simplex/pricing/dualdevexpricing.h>
#include <simplex/simplex.h>

DualDevexPricing::DualDevexPricing(const DenseVector &basicVariableValues,
                                   IndexList<> * basicVariableFeasibilities,
                                   const IndexList<> & reducedCostFeasibilities,
                                   const std::vector<int> & basisHead,
                                   const SimplexModel & simplexModel,
                                   const Basis & basis):
    DualPricing(basicVariableValues,
                basicVariableFeasibilities,
                reducedCostFeasibilities,
                basisHead,
                simplexModel,
                basis)
{
    unsigned int size = simplexModel.getColumnCount() + simplexModel.getRowCount();
    m_referenceFramework.resize(size);
    m_weights.resize(simplexModel.getRowCount());
    setReferenceFramework();
    m_previousPhase = 0;
}

DualDevexPricing::~DualDevexPricing()
{

}

int DualDevexPricing::performPricingPhase1() {
    static bool begin = true;
    if (begin) {
     //   setReferenceFramework();
    }
    begin = false;
    initPhase1();

    if (m_previousPhase != 1) {
        setReferenceFramework();
        m_previousPhase = 1;
    }
    if (m_basis.isFresh()) {
    }

    //TODO: A sorok szamat hivjuk mindenutt rowCountnak, az oszlopokat meg columnCount-nak, ne keverjunk
    //    const unsigned int variableCount = m_simplexModel.getMatrix().rowCount();
    Numerical::Double max = 0;
    m_outgoingIndex = -1;
    unsigned int index;
    m_phase1Simpri.start();

    while (m_phase1Simpri.getCandidateIndex(&index) ) {
        if ( m_used[index] == true ) {
            continue;
        }
        //LPINFO("SIMPRI: " << index);
        unsigned int variableIndex = m_basisHead[index];
        Variable::VARIABLE_TYPE variableType = m_simplexModel.getVariable(variableIndex).getType();
        Numerical::Double weighetReducedCost = m_phase1ReducedCosts[index] / m_weights[index];
       // LPINFO("1 - weight: " << m_weights[index]);
        if ( variableType == Variable::FIXED ||
             variableType == Variable::BOUNDED ||
             (variableType == Variable::PLUS && weighetReducedCost > 0) ||
             (variableType == Variable::MINUS && weighetReducedCost < 0)) {
            if ( Numerical::fabs(weighetReducedCost) > max ) {
                max = Numerical::fabs(weighetReducedCost);
                m_outgoingIndex = index;
                m_phase1Simpri.improvingIndexFound();
            }
        }
    }

    if (m_outgoingIndex != -1) {
        m_reducedCost = m_phase1ReducedCosts[m_outgoingIndex];
    }
    return m_outgoingIndex;
}

int DualDevexPricing::performPricingPhase2() {
    initPhase2();

    if (m_previousPhase != 2) {
        setReferenceFramework();
        m_previousPhase = 2;
    }

    IndexList<>::Iterator iter, iterEnd;
    Numerical::Double max = -1.0;
    unsigned int rowIndex;
    m_outgoingIndex = -1;
    //m_updater->m_simplexModel.getVariable(0);
    m_phase2Simpri.start();
    Numerical::Double difference;
    while ( m_phase2Simpri.getCandidateIndex(&rowIndex) ) {
        if ( m_used[rowIndex] == true ) {
            continue;
        }
        int variableIndex = m_basisHead[rowIndex];
        switch ( m_basicVariableFeasibilities->where( rowIndex ) ) {
        case Simplex::MINUS:
            difference =  (m_simplexModel.getVariable(variableIndex).getLowerBound() -
                           m_basicVariableValues.at(rowIndex)) / m_weights[rowIndex];
            //LPINFO("2 - weight: " << m_weights[rowIndex]);
            if (difference > max) {
                max = difference;
                m_reducedCost = difference * m_weights[rowIndex];
                m_outgoingIndex = rowIndex;
                m_phase2Simpri.improvingIndexFound();
            }
            break;
        case Simplex::PLUS:
            difference = (m_basicVariableValues.at(rowIndex) -
                    m_simplexModel.getVariable(variableIndex).getUpperBound()) / m_weights[rowIndex];
           // LPINFO("2 - weight: " << m_weights[rowIndex]);
            if (difference > max) {
                max = difference;
                m_reducedCost = difference * m_weights[rowIndex];
                m_outgoingIndex = rowIndex;
                m_phase2Simpri.improvingIndexFound();
            }
            break;
        }
    }

    return m_outgoingIndex;

}

void DualDevexPricing::update(int incomingIndex,
                              int outgoingIndex,
                              const DenseVector &incomingAlpha,
                              const DenseVector &pivotRow,
                              const DenseVector &pivotRowOfBasisInverse)
{
    __UNUSED(pivotRowOfBasisInverse);

    Numerical::Double alphaPNorm = 0.0;
    unsigned int index = 0;
   // std::cout << std::endl;
    for (index = 0; index < m_referenceFramework.size(); index++) {
     //   std::cout << "(" << pivotRow.at(index) << ", " << (char)(m_referenceFramework[index]+'0') << ") ";
        if (m_referenceFramework[index] == 1) {
            Numerical::Double value = pivotRow.at(index);
            alphaPNorm += value * value;
        }
    }
   // std::cin.get();

    alphaPNorm = Numerical::sqrt(alphaPNorm);

    Numerical::Double alpha_p_q = pivotRow.at(incomingIndex);
    DenseVector::NonzeroIterator nonzIter = incomingAlpha.beginNonzero();
    DenseVector::NonzeroIterator nonzIterEnd = incomingAlpha.endNonzero();
    for (; nonzIter != nonzIterEnd; ++nonzIter) {
        index = nonzIter.getIndex();
        if (unlikely((int)index == outgoingIndex)) {
            continue;
        }
        Numerical::Double actualWeight = m_weights[index];
        Numerical::Double newWeight = Numerical::fabs(*nonzIter / alpha_p_q) * alphaPNorm;
        if (newWeight > actualWeight) {
           // LPWARNING("modify " << actualWeight << " -> " << newWeight);
            m_weights[index] = newWeight;
        } else {
           // LPERROR("not modify: " << actualWeight << " -> " << newWeight << "   " << alphaPNorm);
        }
    }

    Numerical::Double pivotWeight = alphaPNorm / alpha_p_q;
    if (pivotWeight > 1.0) {
        //LPWARNING("*modify " << m_weights[outgoingIndex] << " -> " << pivotWeight);
        m_weights[outgoingIndex] = pivotWeight;

        if (pivotWeight > alphaPNorm * 3) {
            setReferenceFramework();
            LPINFO("SET REFERENCE FRAMEWORK");
        }
    }

    //LPINFO(alphaPNorm << ", " << m_weights[outgoingIndex]);
    //cin.get();
}

void DualDevexPricing::checkAndFix()
{

}

void DualDevexPricing::setReferenceFramework() {
    unsigned int index = 0;
    for (index = 0; index < m_referenceFramework.size(); index++) {
        m_referenceFramework[index] = 0;
    }

    for (index = 0; index < m_weights.size(); index++) {
        m_weights[index] = 1.0;
    }

    std::vector<int>::const_iterator iter = m_basisHead.begin();
    std::vector<int>::const_iterator iterEnd = m_basisHead.end();
    for (; iter != iterEnd; ++iter) {
        m_referenceFramework[*iter] = 1;
    }
}
