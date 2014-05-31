#ifndef SIMPLEXCONTROLLER_H
#define SIMPLEXCONTROLLER_H

#include <globals.h>
#include <utils/iterationreportprovider.h>
#include <simplex/primalsimplex.h>
#include <simplex/dualsimplex.h>

class SimplexController: public IterationReportProvider {
public:
    SimplexController();
    virtual ~SimplexController();

    void solve(const Model& model, Simplex::ALGORITHM startingAlgorithm);

    // Interface of the iteration report provider
    std::vector<IterationReportField> getIterationReportFields(enum ITERATION_REPORT_FIELD_TYPE & type) const;

    Entry getIterationEntry(const std::string & name,
                            enum ITERATION_REPORT_FIELD_TYPE & type) const;

    void writeSolutionReport();

    const Numerical::Double & getObjectiveValue() const;
    const Numerical::Double & getPhaseIObjectiveValue() const;
    const std::vector<int> & getBasisHead() const;
    const std::vector<Numerical::Double> getPrimalSolution() const;
    const std::vector<Numerical::Double> getDualSolution() const;

private:
    Simplex::ALGORITHM m_currentAlgorithm;

    PrimalSimplex * m_primalSimplex;
    DualSimplex * m_dualSimplex;
    Simplex * m_currentSimplex;

    int m_iterationIndex;
    bool m_freshBasis;

    Timer m_solveTimer;
    //Parameter values
    const int & m_debugLevel;
    const bool & m_saveBasis;
    const std::string & m_saveFilename;
    const bool & m_saveLastBasis;
    const bool & m_loadBasis;
    const std::string & m_loadFilename;
    const std::string & m_loadFormat;
    const int & m_exportType;
    const bool & m_enableExport;
    const std::string & m_exportFilename;

    int m_triggeredReinversion;

    IterationReport m_iterationReport;

    void switchAlgorithm(const Model &model);
};

#endif // SIMPLEXCONTROLLER_H
