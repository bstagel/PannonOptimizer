#include "gui/core/mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowTitle(e_appName+" "+QString::number(e_appVersion)+"."+QString::number(e_appSubVersion));
    this->resize(800, 600);
    this->setContextMenuPolicy(Qt::PreventContextMenu);

    this->menuBar = new QMenuBar(this);
    menuBar->clear();

    //File menu
    QMenu * menu = new QMenu(menuBar);
    menu->setTitle("File");
    actions.insert("newProblem", menu->addAction("New"));
    actions.insert("loadProblem", menu->addAction("Load"));
    actions.insert("saveProblem", menu->addAction("Save"));
    actions.insert("exitProgram", menu->addAction("Exit"));
    menuBar->addMenu(menu);

    //Actions menu
    menu = new QMenu(menuBar);
    menu->setTitle("Actions");
    actions.insert("startSolve", menu->addAction("Solve problem"));
    actions.insert("cancelSolve", menu->addAction("Cancel solution"));
    actions.insert("exportSolution", menu->addAction("Export solution"));
    menuBar->addMenu(menu);

    this->setMenuBar(menuBar);

    //Set toolbar icons
    mainToolBar = new QToolBar(this);
    mainToolBar->setMovable(false);

    tools.insert("newProblem", new QToolButton(mainToolBar));
    tools.insert("loadProblem", new QToolButton(mainToolBar));
    tools.insert("saveProblem", new QToolButton(mainToolBar));
    tools.insert("startSolve", new QToolButton(mainToolBar));
    tools.insert("cancelSolve", new QToolButton(mainToolBar));
    tools.insert("exportSolution", new QToolButton(mainToolBar));
    tools["newProblem"]->setIcon(QIcon(":/icons/new.png"));
    tools["loadProblem"]->setIcon(QIcon(":/icons/open.png"));
    tools["saveProblem"]->setIcon(QIcon(":/icons/save.png"));
    tools["startSolve"]->setIcon(QIcon(":/icons/play.png"));
    tools["cancelSolve"]->setIcon(QIcon(":/icons/stop.png"));
    tools["exportSolution"]->setIcon(QIcon(":/icons/export.png"));
    mainToolBar->addWidget(tools["newProblem"]);
    mainToolBar->addWidget(tools["loadProblem"]);
    mainToolBar->addWidget(tools["saveProblem"]);
    mainToolBar->addWidget(tools["startSolve"]);
    mainToolBar->addWidget(tools["cancelSolve"]);
    mainToolBar->addWidget(tools["exportSolution"]);
    this->addToolBar(mainToolBar);

    connect(tools["newProblem"], SIGNAL(released()), actions["newProblem"], SLOT(trigger()));
    connect(tools["loadProblem"], SIGNAL(released()), actions["loadProblem"], SLOT(trigger()));
    connect(tools["saveProblem"], SIGNAL(released()), actions["saveProblem"], SLOT(trigger()));
    connect(tools["startSolve"], SIGNAL(released()), actions["startSolve"], SLOT(trigger()));
    connect(tools["cancelSolve"], SIGNAL(released()), actions["cancelSolve"], SLOT(trigger()));
    connect(tools["exportSolution"], SIGNAL(released()), actions["exportSolution"], SLOT(trigger()));

    centralWidget = new QWidget(this);
    centralWidget->setAutoFillBackground(true);
    this->setCentralWidget(centralWidget);
    tabLayout = new QVBoxLayout(centralWidget);
    mainTabs = new QTabWidget(centralWidget);
    tabLayout->addWidget(mainTabs);
    mainTabs->setAutoFillBackground(true);
    mainTabs->addTab(new QWidget(mainTabs), "Problem");
    mainTabs->addTab(new QWidget(mainTabs), "Parameters");
    mainTabs->addTab(new QWidget(mainTabs), "Matrix");
    mainTabs->addTab(new QWidget(mainTabs), "Solution");

    sourceLayout = new QGridLayout();

    paramLayout = new QGridLayout();

    matrixLayout = new QGridLayout();

    solutionLayout = new QGridLayout();

    statusBar = new QStatusBar(this);
    statusBar->setSizeGripEnabled(false);
    this->setStatusBar(statusBar);
    statusBar->showMessage("Ready");

    fileDialog.setNameFilter("LP problem files (*.mps *.lp)");

    //Connecting operation signals to slots
    connect(actions["newProblem"], SIGNAL(triggered()), this, SLOT(newProblem()));
    connect(actions["loadProblem"], SIGNAL(triggered()), this, SLOT(loadProblem()));
    connect(actions["saveProblem"], SIGNAL(triggered()), this, SLOT(saveProblem()));
    connect(actions["exitProgram"], SIGNAL(triggered()), this, SLOT(exitProgram()));
    connect(actions["startSolve"], SIGNAL(triggered()), this, SLOT(startSolve()));
    connect(actions["cancelSolve"], SIGNAL(triggered()), this, SLOT(cancelSolve()));
    connect(actions["exportSolution"], SIGNAL(triggered()), this, SLOT(exportSolution()));

    connect(&panOpt, SIGNAL(finished()), this, SLOT(obtainSolution()));

    newProblem();
}

MainWindow::~MainWindow() { }

void MainWindow::newProblem() {
    actions["startSolve"]->setEnabled(false);
    actions["cancelSolve"]->setEnabled(false);
    actions["exportSolution"]->setEnabled(false);
    tools["startSolve"]->setEnabled(false);
    tools["cancelSolve"]->setEnabled(false);
    tools["exportSolution"]->setEnabled(false);
}

void MainWindow::loadProblem() {
    parseProblem();
}

void MainWindow::saveProblem() {

}

void MainWindow::exitProgram() {
    this->close();
}

void MainWindow::startSolve() {
    statusBar->showMessage(QString("Solving problem ")+QString::fromStdString(panOpt.getModel()->getName())+QString("..."));
    panOpt.run();

    actions["startSolve"]->setEnabled(false);
    actions["cancelSolve"]->setEnabled(true);
    actions["exportSolution"]->setEnabled(false);
    tools["startSolve"]->setEnabled(false);
    tools["cancelSolve"]->setEnabled(true);
    tools["exportSolution"]->setEnabled(false);
}

void MainWindow::cancelSolve() {
    actions["startSolve"]->setEnabled(true);
    actions["cancelSolve"]->setEnabled(false);
    actions["exportSolution"]->setEnabled(true);
    tools["startSolve"]->setEnabled(true);
    tools["cancelSolve"]->setEnabled(false);
    tools["exportSolution"]->setEnabled(true);
}

void MainWindow::exportSolution() {

}

void MainWindow::parseProblem() {
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.exec();
    if(fileDialog.selectedUrls().count() > 0) {
        if(panOpt.loadProblem(fileDialog.selectedFiles().at(0))) {
            actions["startSolve"]->setEnabled(true);
            actions["cancelSolve"]->setEnabled(false);
            actions["exportSolution"]->setEnabled(false);
            tools["startSolve"]->setEnabled(true);
            tools["cancelSolve"]->setEnabled(false);
            tools["exportSolution"]->setEnabled(false);

            statusBar->showMessage(QString("Problem ")+QString::fromStdString(panOpt.getModel()->getName())+QString(" loaded successfully."));
        }
    }
}

void MainWindow::obtainSolution() {
    statusBar->showMessage(QString("Solution found for problem ")+QString::fromStdString(panOpt.getModel()->getName()));
}
