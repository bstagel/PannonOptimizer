#include "gui/modules/panopthandler.h"
#include <QDebug>

PanOptHandler::PanOptHandler() : m_problemLoaded(false), m_running(false), m_model(NULL) { }

PanOptHandler::~PanOptHandler() { }

void PanOptHandler::run() {
    if(!m_running && m_model != NULL) {
        m_controller = new SimplexController();
        m_controller->solve(*m_model);
    } else return;
}

bool PanOptHandler::loadProblem(QString filename, LPFormat format) {
    if(!m_running) {
        m_model = new Model();
        MpsModelBuilder* builder = new MpsModelBuilder();
        builder->loadFromFile(filename.toStdString());
        m_model->build(*builder);
        delete builder;

        m_problemLoaded = true;
    } else return false;
    return true;
}
