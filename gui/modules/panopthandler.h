#ifndef PANOPTHANDLER_H
#define PANOPTHANDLER_H

#include <lp/model.h>
#include <lp/mpsproblem.h>
#include <simplex/simplexparameterhandler.h>
#include <linalg/linalgparameterhandler.h>
#include <simplex/simplexcontroller.h>

#include <QThread>
#include <QUrl>

class PanOptHandler : public QThread {
public:
    enum LPFormat {
        MPS = 0,
        LP
    };

    PanOptHandler();
    ~PanOptHandler();
    void run();
    bool loadProblem(QString filename, LPFormat format = MPS);

    Model* getModel() { return m_model; }
signals:
    void solverFinished();
private:
    Model* m_model;
    SimplexController* m_controller;
    bool m_problemLoaded;
    bool m_running;
};

#endif // PANOPTHANDLER_H
