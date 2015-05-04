#ifndef PANOPTHANDLER_H
#define PANOPTHANDLER_H

#include <lp/model.h>
#include <lp/mpsproblem.h>
#include <lp/lpproblem.h>
#include <simplex/simplexparameterhandler.h>
#include <linalg/linalgparameterhandler.h>

#include <gui/core/constants.h>

#include <QProcess>
#include <QThread>
#include <QUrl>

class PanOptHandler : public QThread {
    Q_OBJECT
public:

    struct ParseResult {
        QString str;
        int line;
    };

    PanOptHandler(QString solverPath);
    ~PanOptHandler();
    void run();
    ParseResult loadProblem(QString filename);
    void kill();
    QProcess* process() { return m_process; }

    Model* getModel() { return m_model; }
signals:
    void solverFinished();
    void solverError();
private:
    Model* m_model;
    QProcess* m_process;
    QString m_problemPath;
    QString m_solverPath;
    bool m_problemLoaded;
    bool m_running;
};

#endif // PANOPTHANDLER_H
