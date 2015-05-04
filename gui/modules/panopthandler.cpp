#include "gui/modules/panopthandler.h"
#include <QDebug>

PanOptHandler::PanOptHandler(QString solverPath) : m_model(NULL), m_process(NULL), m_problemPath(""), m_solverPath(solverPath), m_problemLoaded(false), m_running(false) { }

PanOptHandler::~PanOptHandler() {
    delete m_model;
    m_process->kill();
    delete m_process;
}

void PanOptHandler::run() {
    if(!m_running && m_model != NULL) {
        m_running = true;
        m_process = new QProcess();
        qDebug()<<"Process: "<<m_solverPath<<" -f "<<m_problemPath;
        QStringList args;
        args.append("-dv");
        args.append("-f");
        args.append(m_problemPath);
        m_process->setArguments(args);
        m_process->setProgram(QUrl(m_solverPath).path());
        m_process->start();
        if(m_process->waitForFinished(1800000)) {
            if(m_running == false) return;
            m_running = false;
            emit solverFinished();
        } else {
            if(m_running == false) return;
            m_running = false;
            emit solverError();
        }
    } else return;
}

void PanOptHandler::kill() {
    if(m_running) {
        m_process->deleteLater();
        m_process = NULL;
        m_running = false;
    }
}

PanOptHandler::ParseResult PanOptHandler::loadProblem(QString filename) {
    ParseResult result;
    result.str = "";
    result.line = -1;
    if(!m_running) {
        m_model = new Model();
        if(filename.length() > 3 && filename.right(3).toUpper() == ".LP") {
            LpModelBuilder* builder = new LpModelBuilder();
            builder->loadFromFile(filename.toStdString());
            if(builder->getError().m_errorStr != "") {
                result.line = builder->getError().m_errorLine;
                result.str = QString::fromStdString(builder->getError().m_errorStr);
                return result;
            }
            m_model->build(*builder);
            delete builder;
        } else {
            MpsModelBuilder* builder = new MpsModelBuilder();
            builder->loadFromFile(filename.toStdString());
            m_model->build(*builder);
            delete builder;
        }

        m_problemPath = filename;
        m_problemLoaded = true;
    } else {
        result.str = "The solver is still running!";
        result.line = 0;
    }
    return result;
}
