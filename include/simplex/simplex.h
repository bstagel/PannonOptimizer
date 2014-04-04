/**
 * @file simplex.h
 */


#ifndef SIMPLEX_H
#define	SIMPLEX_H

#include <globals.h>

#include <lp/method.h>
#include <simplex/simplexmodel.h>
#include <utils/numerical.h>
#include <utils/indexlist.h>
#include <utils/iterationreport.h>
#include <utils/iterationreportprovider.h>
#include <utils/timer.h>
//#include <simplex/basisheadio.h>

class StartingBasisFinder;
class Basis;
class BasisHeadIO;

class Simplex : public Method, public IterationReportProvider
{
    friend class BasisHeadIO;
    friend class Checker;
public:

    enum FEASIBILITY
    {
        FEASIBLE = 0,
        MINUS,
        PLUS,
        FEASIBILITY_ENUM_LENGTH
    };

    enum VARIABLE_STATE
    {
        BASIC = 0,
        NONBASIC_AT_LB,
        NONBASIC_AT_UB,
        NONBASIC_FIXED,
        NONBASIC_FREE,
        VARIABLE_STATE_ENUM_LENGTH
        // Do not modify the order of the enum!
    };


    Simplex();
    virtual ~Simplex();

    inline const Numerical::Double & getObjectiveValue() const {return m_objectiveValue;}
    inline const Numerical::Double & getPhaseIObjectiveValue() const {return m_phaseIObjectiveValue;}
    inline const std::vector<int> & getBasisHead() const {return m_basisHead;}

    const std::vector<Numerical::Double> getPrimalSolution() const;
    const std::vector<Numerical::Double> getDualSolution() const;


    void setModel(const Model & model);

    void solve();
    void findStartingBasis();

    void saveBasisToFile(const char * fileName, BasisHeadIO * basisWriter, bool releaseWriter);
    void loadBasisFromFile(const char * fileName, BasisHeadIO * basisReader, bool releaseReader);

    // Interface of the iteration report provider
    std::vector<IterationReportField> getIterationReportFields(enum ITERATION_REPORT_FIELD_TYPE & type) const;

    Entry getIterationEntry(const std::string & name,
                            enum ITERATION_REPORT_FIELD_TYPE & type) const;


protected:
    unsigned int m_iterationIndex;

    //Tolerance handling
    Numerical::Double m_masterTolerance;
    Numerical::Double m_workingTolerance;
    Numerical::Double m_toleranceStep;

    SimplexModel * m_simplexModel;

    std::vector<int> m_basisHead;
    //Template parameter holds the value of the variable
    IndexList<const Numerical::Double*> m_variableStates;
    IndexList<> m_basicVariableFeasibilities;
    IndexList<> m_reducedCostFeasibilities;
    Vector m_basicVariableValues;
    Vector m_reducedCosts;
    Numerical::Double m_objectiveValue;
    Numerical::Double m_phaseIObjectiveValue;

    bool m_feasible;
    bool m_baseChanged;
    bool m_freshBasis;
    int m_debugLevel;

    Timer m_solveTimer;
    Timer m_inversionTimer;
    Timer m_computeBasicSolutionTimer;
    Timer m_computeReducedCostsTimer;
    Timer m_computeFeasibilityTimer;
    Timer m_checkFeasibilityTimer;
    Timer m_priceTimer;
    Timer m_selectPivotTimer;
    Timer m_updateTimer;

    bool m_saveBasis;
    std::string m_saveFilename;
    std::string m_saveFormat;
    bool m_saveLastBasis;
    unsigned int m_saveIteration;
    unsigned int m_savePeriodically;
    bool m_loadBasis;
    std::string m_loadFilename;
    std::string m_loadFormat;
    bool m_enableExport;
    std::string m_exportFilename;
    int m_exportType;

    Numerical::Double m_referenceObjective;
    int m_phase1Iteration;
    Numerical::Double m_phase1Time;
    int m_fallbacks;
    int m_triggeredReinversion;
    int m_badIterations;
    int m_degenerateIterations;

    //FPU fix
    unsigned int m_old_cw;

    //Modules
    StartingBasisFinder* m_startingBasisFinder;
    Basis* m_basis;

    IterationReport m_iterationReport;

    void constraintAdded();
    void variableAdded();

    virtual void initModules();
    virtual void releaseModules();

    void saveBasis();
    void loadBasis();

    void reinvert();
    void computeBasicSolution();
    void computeReducedCosts();
    virtual void computeFeasibility() = 0;
    virtual void checkFeasibility() = 0;
    virtual void price() = 0;
    virtual void selectPivot() = 0;
    virtual void update() = 0;

    virtual void setReferenceObjective() = 0;
    virtual void checkReferenceObjective() = 0;

    virtual void initWorkingTolerance() = 0;
    virtual void computeWorkingTolerance() = 0;

    virtual void releaseLocks() = 0;

    void transform(int incomingIndex,
                   int outgoingIndex,
                   const std::vector<unsigned int>& boundflips,
                   Numerical::Double* primalTheta);

    void registerIntoIterationReport(const IterationReportProvider & provider);


};

#endif /* SIMPLEX_H */
