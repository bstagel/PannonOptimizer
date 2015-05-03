#include "gui/core/mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowTitle(e_appName+" "+QString::number(e_appVersion)+"."+QString::number(e_appSubVersion));
    this->resize(800, 600);
    this->setContextMenuPolicy(Qt::PreventContextMenu);

    QString solverPath = e_solverPath;
    if(!QUrl(solverPath).isValid()) {
        fileDialog.setNameFilter("LP problem files (*.mps *.lp)");
        fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
        fileDialog.setFileMode(QFileDialog::ExistingFile);
        while(fileDialog.selectedUrls().count() <= 0) {
            fileDialog.exec();
        }
        solverPath = fileDialog.selectedFiles().at(0);
    }
    panOpt = new PanOptHandler(solverPath);

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


    initProblemTab();
    initMatrixTab();

    sourceLayout = new QGridLayout();

    /* paramLayout = new QGridLayout(mainTabs->widget(0));
    myButton = new QRadioButton(mainTabs->widget(0));
    myButton->setText("MY BUTTON");
    paramLayout->addWidget(myButton, 0, 0, Qt::AlignTop);*/

    matrixLayout = new QGridLayout();

    solutionLayout = new QGridLayout();

    statusBar = new QStatusBar(this);
    statusBar->setSizeGripEnabled(false);
    this->setStatusBar(statusBar);
    statusBar->showMessage("Ready");

    //Connecting operation signals to slots
    connect(actions["newProblem"], SIGNAL(triggered()), this, SLOT(newProblem()));
    connect(actions["loadProblem"], SIGNAL(triggered()), this, SLOT(loadProblem()));
    connect(actions["saveProblem"], SIGNAL(triggered()), this, SLOT(saveProblem()));
    connect(actions["exitProgram"], SIGNAL(triggered()), this, SLOT(exitProgram()));
    connect(actions["startSolve"], SIGNAL(triggered()), this, SLOT(startSolve()));
    connect(actions["cancelSolve"], SIGNAL(triggered()), this, SLOT(cancelSolve()));
    connect(actions["exportSolution"], SIGNAL(triggered()), this, SLOT(exportSolution()));

    connect(panOpt, SIGNAL(solverFinished()), this, SLOT(obtainSolution()));

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
    fileDialog.setNameFilter("LP problem files (*.mps *.lp)");
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);
    fileDialog.setFileMode(QFileDialog::ExistingFile);
    fileDialog.exec();
    parseProblem();
    drawMatrix();
}

void MainWindow::saveProblem() {

}

void MainWindow::exitProgram() {
    this->close();
}

void MainWindow::startSolve() {
    statusBar->showMessage(QString("Solving problem ")+QString::fromStdString(panOpt->getModel()->getName())+QString("..."));
    panOpt->start();

    actions["startSolve"]->setEnabled(false);
    actions["cancelSolve"]->setEnabled(true);
    actions["exportSolution"]->setEnabled(false);
    tools["startSolve"]->setEnabled(false);
    tools["cancelSolve"]->setEnabled(true);
    tools["exportSolution"]->setEnabled(false);
}

void MainWindow::cancelSolve() {
    panOpt->kill();
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
    if(fileDialog.selectedUrls().count() > 0) {
        if(panOpt->loadProblem(fileDialog.selectedFiles().at(0))) {
            actions["startSolve"]->setEnabled(true);
            actions["cancelSolve"]->setEnabled(false);
            actions["exportSolution"]->setEnabled(false);
            tools["startSolve"]->setEnabled(true);
            tools["cancelSolve"]->setEnabled(false);
            tools["exportSolution"]->setEnabled(false);

            statusBar->showMessage(QString("Problem ")+QString::fromStdString(panOpt->getModel()->getName())+QString(" loaded successfully."));
        }
    }
}

void MainWindow::obtainSolution() {
    QString output = panOpt->process()->readAllStandardError();
    if(output.contains("OPTIMAL SOLUTION found!")) {
        int pos = output.indexOf("Total iterations:");
        output = output.right(output.length() - pos - 18);
        pos = output.indexOf("[REPORT ]");
        QString iterNum = output.left(pos - 2);
        pos = output.indexOf("Objective value:");
        output = output.right(output.length() - pos - 17);
        QString solution = output;
        statusBar->showMessage(QString("Solution found for problem ")+QString::fromStdString(panOpt->getModel()->getName())+QString(" in ")+iterNum+QString(" iterations (")+solution+QString(")"));
    } else {
        statusBar->showMessage(QString("No solution could be found for problem ")+QString::fromStdString(panOpt->getModel()->getName()));
    }
    actions["startSolve"]->setEnabled(true);
    actions["cancelSolve"]->setEnabled(false);
    actions["exportSolution"]->setEnabled(true);
    tools["startSolve"]->setEnabled(true);
    tools["cancelSolve"]->setEnabled(false);
    tools["exportSolution"]->setEnabled(true);
}

void MainWindow::onMPSlistSelected(QString item)
{
    QTextEdit* textArea= mainTabs->findChild<QTextEdit*>("textArea");
    if(textArea!=0)
        textArea->setText(item);


}

void MainWindow::initProblemTab()
{
    QWidget* problemTab=mainTabs->widget(0);
    QHBoxLayout * problemTabLayout = new QHBoxLayout(problemTab);

    QSplitter* splitter = new QSplitter;
    splitter->setOrientation(Qt::Vertical);
    problemTabLayout->addWidget(splitter);



    QWidget* felsoPanel=new QWidget;
    QHBoxLayout * felsoPanelLayout = new QHBoxLayout(felsoPanel);

    QWidget* jobbPanel=new QWidget;
    QVBoxLayout * jobbPanelLayout = new QVBoxLayout(jobbPanel);



    QTextEdit* textArea=new QTextEdit();
    textArea->setObjectName("textArea");
    textArea->setText("Irj ide vmit");
    felsoPanelLayout->addWidget(textArea);
    felsoPanelLayout->addWidget(jobbPanel,0);


    jobbPanel->setFixedWidth(200);
    QList<int> list;

    splitter->addWidget(felsoPanel);

    QGroupBox* log=new QGroupBox;
    log->setTitle("Log");
    QLabel* newLabel = new QLabel("- Problem 'sample' has been parsed successfully!", log);
    newLabel->setMargin(20);

    splitter->addWidget(felsoPanel);
    splitter->addWidget(log);



    list= splitter->sizes();
    list.replace(0,splitter->height()/0.2);
    list.replace(1,splitter->height()/0.8);
    splitter->setSizes(list);

    //Mindig felül legyen az ablakocska
    Qt::WindowFlags flags = windowFlags();
    this->setWindowFlags(flags | Qt::WindowStaysOnTopHint);



    //Jobb oldali felületek
    QComboBox * mps = new QComboBox;
    jobbPanelLayout->addWidget(mps,0,Qt::AlignTop);
    mps->addItem("MPS");
    mps->addItem("LP");
    connect(mps,SIGNAL(currentIndexChanged(QString)),this,SLOT(onMPSlistSelected(QString)));
    QTableView * stat= new QTableView;
    jobbPanelLayout->addWidget(stat, 0, Qt::AlignTop);

    QStandardItemModel* model= new QStandardItemModel();

    QStringList ql;

    QList<QStandardItem*> row;
    /*row<< new QStandardItem("Name");
    row<< new QStandardItem("Rows");
    row<< new QStandardItem("Columns");
    row<< new QStandardItem("Non zeros");
    row<< new QStandardItem("Density");
    model->appendColumn(row);*/

    ql.append("Name");
    ql.append("Rows");
    ql.append("Columns");
    ql.append("Non zeros");
    ql.append("Density");
    model->setVerticalHeaderLabels(ql);

    row= QList<QStandardItem*>();
    row<< new QStandardItem("sample");
    row<< new QStandardItem("3");
    row<< new QStandardItem("4");
    row<< new QStandardItem("9");
    row<< new QStandardItem("75%");
    for (int i = 0; i < 5; ++i) {
        row[i]->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    model->appendColumn(row);


    stat->setModel(model);
    stat->horizontalHeader()->hide();



}

void MainWindow::initMatrixTab()
{
    QWidget* matrixTab=mainTabs->widget(2);
    QHBoxLayout * matrixTabLayout = new QHBoxLayout(matrixTab);
    matrixView = new QGraphicsView;
    matrixView->resize(600,400);
    matrixTabLayout->addWidget(matrixView);
    scene = new QGraphicsScene;
    scene->addRect(QRectF(0,0,matrixView->width(),matrixView->height()));

    matrixView->setScene(scene);
}

void MainWindow::drawMatrix()
{
    const Matrix& matrix = panOpt->getModel()->getMatrix();
    double ratio = matrix.columnCount() / matrix.rowCount();
    int height = 400;
    int width = 600;
    if(ratio >= 1.5) {
        height = width / ratio;
    } else width = ratio * height;
    int blockSize = 1;
    if(matrix.rowCount() < height) blockSize = height / matrix.rowCount();
    scene = new QGraphicsScene;
    for(int i = 0; i < matrix.rowCount(); i++) {
        for(SparseVector::NonzeroIterator it = matrix.row(i).beginNonzero();it < matrix.row(i).endNonzero(); ++it) {
            scene->addRect(QRectF(it.getIndex() * blockSize, i * blockSize, blockSize, blockSize), QPen(), QBrush(QColor(0,0,0)));
        }
    }
    scene->addRect(QRectF(0,0,matrixView->width(),matrixView->height()));
    matrixView->setScene(scene);
}

