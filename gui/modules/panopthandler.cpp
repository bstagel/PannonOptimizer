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

bool PanOptHandler::loadProblem(QString filename, LPFormat format) {
    if(!m_running) {
        m_model = new Model();
        MpsModelBuilder* builder = new MpsModelBuilder();
        builder->loadFromFile(filename.toStdString());
        m_model->build(*builder);
        delete builder;

        m_problemPath = filename;
        m_problemLoaded = true;
    } else return false;
    return true;
}
