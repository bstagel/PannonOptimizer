#include "gui/core/mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowTitle(e_appName+" "+QString::number(e_appVersion)+"."+QString::number(e_appSubVersion));
    this->resize(800, 600);
    this->setContextMenuPolicy(Qt::PreventContextMenu);

//    //Mindig felül legyen az ablakocska
//    Qt::WindowFlags flags = windowFlags();
//    this->setWindowFlags(flags | Qt::WindowStaysOnTopHint);

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
    selectedFileName = "";
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
    initParametersTab();
    initMatrixTab();
    initSolutionTab();

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
    textEdit->setText("");
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
    if(fileDialog.selectedUrls().count() > 0 && fileDialog.selectedFiles().at(0).contains(".")) {
        selectedFileName = fileDialog.selectedFiles().at(0);
        parseProblem();
    }
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
    PanOptHandler::ParseResult loadResult = panOpt->loadProblem(selectedFileName);
    if(loadResult.line == -1) {
        actions["startSolve"]->setEnabled(true);
        actions["cancelSolve"]->setEnabled(false);
        actions["exportSolution"]->setEnabled(false);
        tools["startSolve"]->setEnabled(true);
        tools["cancelSolve"]->setEnabled(false);
        tools["exportSolution"]->setEnabled(false);
        statusBar->showMessage(QString("Problem ")+QString::fromStdString(panOpt->getModel()->getName())+QString(" loaded successfully."));

        drawMatrix();

        QList<QStandardItem*> row;
        row= QList<QStandardItem*>();
        row<< new QStandardItem(QString::fromStdString(panOpt->getModel()->getName()));
        row<< new QStandardItem(QString::number(panOpt->getModel()->constraintCount()));
        row<< new QStandardItem(QString::number(panOpt->getModel()->variableCount()));
        row<< new QStandardItem(QString::number(panOpt->getModel()->getMatrix().nonZeros()));
        row<< new QStandardItem(QString::number(100 * (double)panOpt->getModel()->getMatrix().nonZeros() / (double)(panOpt->getModel()->constraintCount() * panOpt->getModel()->variableCount())) + " %");
        for (int i = 0; i < 5; ++i) {
            row[i]->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        }

        QStandardItemModel* newModel = new QStandardItemModel;
        QStringList ql;
        ql.append("Name");
        ql.append("Rows");
        ql.append("Columns");
        ql.append("Non zeros");
        ql.append("Density");
        newModel->setVerticalHeaderLabels(ql);
        newModel->appendColumn(row);
        problemStats->setModel(newModel);
    } else {
        statusBar->showMessage(QString("Parse error at line ")+QString::number(loadResult.line)+QString(": ")+loadResult.str);
    }
}

void MainWindow::obtainSolution() {
    QList<QStandardItem*> row;
    row= QList<QStandardItem*>();
    row<< new QStandardItem(QString::fromStdString(panOpt->getModel()->getName()));
    QString output = panOpt->process()->readAllStandardError();
    if(output.contains("OPTIMAL SOLUTION found!")) {
        row<< new QStandardItem("OPTIMAL");
        int pos = output.indexOf("Total iterations:");
        output = output.right(output.length() - pos - 18);
        pos = output.indexOf("[REPORT ]");
        QString iterNum = output.left(pos - 2);
        pos = output.indexOf("Total solution time:");
        output = output.right(output.length() - pos - 21);
        pos = output.indexOf("[");
        QString solTime = output.left(pos - 1);
        pos = output.indexOf("Objective value:");
        output = output.right(output.length() - pos - 17);
        pos = output.indexOf("[");
        QString solution = output.left(pos - 1);
        row<< new QStandardItem(solution);
        row<< new QStandardItem(iterNum);
        row<< new QStandardItem(solTime);
        output = output.right(output.length() - pos);
        statusBar->showMessage(QString("Solution found for problem ")+QString::fromStdString(panOpt->getModel()->getName())+QString(" in ")+iterNum+QString(" iterations (")+solution+QString(")"));
    } else {
        row<< new QStandardItem("INFEASIBLE");
        row<< new QStandardItem("0");
        row<< new QStandardItem("0");
        row<< new QStandardItem("0");
        statusBar->showMessage(QString("No solution could be found for problem ")+QString::fromStdString(panOpt->getModel()->getName()));
    }
    QStringList lines;
    while(output.contains(":")) {
        int pos = output.indexOf("]");
        output = output.right(output.length() - pos - 1);
        pos = output.indexOf("[");
        if(output.contains(":")) lines << output.left(pos - 1).trimmed();
    }
    for (int i = 0; i < 5; ++i) {
        row[i]->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    QStandardItemModel* newModel = new QStandardItemModel;
    QStringList ql;
    ql.append("Name");
    ql.append("Result");
    ql.append("Objective value");
    ql.append("Total iterations");
    ql.append("Solution time");
    newModel->setVerticalHeaderLabels(ql);
    newModel->appendColumn(row);
    solutionStats->setModel(newModel);
    writeSolution(lines);
    actions["startSolve"]->setEnabled(true);
    actions["cancelSolve"]->setEnabled(false);
    actions["exportSolution"]->setEnabled(true);
    tools["startSolve"]->setEnabled(true);
    tools["cancelSolve"]->setEnabled(false);
    tools["exportSolution"]->setEnabled(true);
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



    textEdit=new QTextEdit();
    textEdit->setObjectName("textArea");
    textEdit->setText("");
    textEdit->setFont(QFont("Courier new"));
    felsoPanelLayout->addWidget(textEdit);
    felsoPanelLayout->addWidget(jobbPanel,0);


    jobbPanel->setFixedWidth(200);
    QList<int> list;

    splitter->addWidget(felsoPanel);

    QGroupBox* log=new QGroupBox;
    log->setTitle("Log");
    logLabel = new QLabel("", log);
    logLabel->setFixedWidth(600);
    logLabel->setMargin(20);

    splitter->addWidget(felsoPanel);
    splitter->addWidget(log);



    list= splitter->sizes();
    list.replace(0,splitter->height()/0.2);
    list.replace(1,splitter->height()/0.8);
    splitter->setSizes(list);



    //Jobb oldali felületek
    format = new QComboBox;
    format->addItem("LP");
    format->addItem("MPS");
    QPushButton * parseButton = new QPushButton("Parse");
    connect(parseButton, SIGNAL(clicked()), this, SLOT(loadFromEditor()));
    parseButton->setMinimumHeight(40);
    QWidget* upperRightPanel=new QWidget;
    QVBoxLayout* upperRightLayout = new QVBoxLayout(upperRightPanel);
    upperRightLayout->addWidget(format,0,Qt::AlignTop);
    upperRightLayout->addWidget(parseButton,0,Qt::AlignTop);
    jobbPanelLayout->addWidget(upperRightPanel, 0, Qt::AlignTop);
    problemStats = new QTableView;
    problemStats->setSelectionMode(QAbstractItemView::NoSelection);
    jobbPanelLayout->addWidget(problemStats, 0, Qt::AlignTop);


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
    row<< new QStandardItem("-");
    row<< new QStandardItem("0");
    row<< new QStandardItem("0");
    row<< new QStandardItem("0");
    row<< new QStandardItem("0%");
    for (int i = 0; i < 5; ++i) {
        row[i]->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    model->appendColumn(row);

    problemStats->setModel(model);
    problemStats->horizontalHeader()->hide();

    problemStats->setColumnWidth(0, 118);
    problemStats->setFixedHeight(152);

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
    unsigned height = 400;
    unsigned width = 600;
    if(ratio >= 1.5) {
        height = width / ratio;
    } else width = ratio * height;
    int blockSize = 1;
    if(matrix.rowCount() < height) blockSize = height / matrix.rowCount();
    scene = new QGraphicsScene;
    for(unsigned i = 0; i < matrix.rowCount(); i++) {
        for(SparseVector::NonzeroIterator it = matrix.row(i).beginNonzero();it < matrix.row(i).endNonzero(); ++it) {
            scene->addRect(QRectF(it.getIndex() * blockSize, i * blockSize, blockSize, blockSize), QPen(), QBrush(QColor(0,0,0)));
        }
    }
    scene->addRect(QRectF(0,0,matrixView->width(),matrixView->height()));
    matrixView->setScene(scene);
}

void MainWindow::initSolutionTab()
{
    QWidget* solutionTab=mainTabs->widget(3);
    QHBoxLayout * solutionTabLayout = new QHBoxLayout(solutionTab);
    QWidget* leftSideWidget =new QWidget;
    QWidget* rightSideWidget =new QWidget;
    leftSideWidget->setFixedWidth(300);
    solutionTabLayout->addWidget(leftSideWidget);
    solutionTabLayout->addWidget(rightSideWidget);
    QVBoxLayout* leftLayout = new QVBoxLayout(leftSideWidget);
    QVBoxLayout* rightLayout = new QVBoxLayout(rightSideWidget);
    solutionText = new QTextEdit;
    solutionText->setText("No solution.");
    solutionText->setReadOnly(true);
    rightLayout->addWidget(solutionText);

    solutionStats = new QTableView;
    solutionStats->setSelectionMode(QAbstractItemView::NoSelection);
    leftLayout->addWidget(solutionStats, 0, Qt::AlignTop);

    QStandardItemModel* model= new QStandardItemModel();
    QList<QStandardItem*> row;

    QStringList ql;
    ql.append("Name");
    ql.append("Result");
    ql.append("Objective value");
    ql.append("Total iterations");
    ql.append("Solution time");
    model->setVerticalHeaderLabels(ql);

    row= QList<QStandardItem*>();
    row<< new QStandardItem("-");
    row<< new QStandardItem("-");
    row<< new QStandardItem("-");
    row<< new QStandardItem("-");
    row<< new QStandardItem("-");
    for (int i = 0; i < 5; ++i) {
        row[i]->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
    }

    model->appendColumn(row);
    solutionStats->setModel(model);
    solutionStats->setColumnWidth(0, 185);
    solutionStats->setFixedHeight(152);
    solutionStats->horizontalHeader()->hide();
}

void MainWindow::writeSolution(QStringList vars)
{
    int lineCount = 35;
    QString solutionStr = "";
    solutionStr += "<table width=\"";
    int cols = (vars.length() / lineCount) + 1;
    solutionStr += QString::number(cols * 200);
    solutionStr += "\">";
    for(int i = 0; i < (vars.length() < lineCount ? vars.length() : lineCount); i++) {
        solutionStr += "<tr>";
        for(int j = 0; j < cols; j++) {
            if(j * 40 + i < vars.length()) {
                solutionStr += "<td width=\"200\" align=\"left\">";
                solutionStr += vars.at(j * lineCount + i);
                solutionStr += "</td>";
            }
        }
        solutionStr += "</tr>";
    }
    solutionStr += "<table>";
    solutionText->setText(solutionStr);
}

void MainWindow::loadFromEditor()
{
    QString filename("_temp.");
    filename.append(format->currentText());
    QFile tempFile(filename);
    if(tempFile.exists()) tempFile.remove();
    if ( tempFile.open(QIODevice::ReadWrite) )
    {
        QTextStream stream( &tempFile );
        stream << textEdit->toPlainText() << endl;
        tempFile.close();
        selectedFileName = filename;
        parseProblem();
        logLabel->setText(statusBar->currentMessage());
    }
}

void MainWindow::initParametersTab()
{
    //pagetabs
    QTabWidget* pageTabs=new QTabWidget;
    pageTabs->addTab(new QWidget(pageTabs),"page1");
    pageTabs->addTab(new QWidget(pageTabs),"page2");
    pageTabs->setAutoFillBackground(true);

    parameterTab=mainTabs->widget(1);
    parameterTabLayout = new QHBoxLayout(parameterTab);
    parameterTabLayout->addWidget(pageTabs);


    //createTolerances
    QGroupBox* verticalTol=new QGroupBox("Tolerances");
    QGridLayout *tolLayout=new QGridLayout;

    QLabel* e_pivot_label=new QLabel(tr("e_pivot"));
    QLineEdit* e_pivot=new QLineEdit;
    tolLayout->addWidget(e_pivot_label,1,0);
    tolLayout->addWidget(e_pivot,1,1);

    QLabel* e_feasibility_label=new QLabel(tr("e_feasibility"));
    QLineEdit* e_feasibility=new QLineEdit;
    tolLayout->addWidget(e_feasibility_label,2,0);
    tolLayout->addWidget(e_feasibility,2,1);

    QLabel* e_optimally_label=new QLabel(tr("e_optimally"));
    QLineEdit* e_optimally=new QLineEdit;
    tolLayout->addWidget(e_optimally_label,3,0);
    tolLayout->addWidget(e_optimally,3,1);

    verticalTol->setLayout(tolLayout);
    //sizepolicy
    QSizePolicy sizePolicy((QSizePolicy::Policy)QSizePolicy::Minimum,(QSizePolicy::Policy)QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    verticalTol->setSizePolicy( sizePolicy );

   // parameterTabLayout->addWidget(verticalTol);


    //createStarting
    QGroupBox* verticalStart=new QGroupBox("Starting");
    QVBoxLayout* sLayout=new QVBoxLayout;

    QCheckBox* presolve=new QCheckBox;
    QCheckBox* scaling=new QCheckBox;
    presolve->setText("Presolve");
    presolve->isEnabled();
    scaling->setText("Scaling");
    scaling->isEnabled();
    sLayout->addWidget(presolve);
    sLayout->addWidget(scaling);

    QGroupBox* basis=new QGroupBox("Basis");
    QVBoxLayout* _basis=new QVBoxLayout;

    QComboBox* starting_nonbasic_states=new QComboBox;
    starting_nonbasic_states->addItem("NONBASIC_TO_LOWER");
    starting_nonbasic_states->addItem("NONBASIC_TO_UPPER");
    starting_nonbasic_states->addItem("NONBASIC_TO_MIXED");
    QComboBox* starting_nonbasic_strategy=new QComboBox;
    starting_nonbasic_strategy->addItem("LOGICAL");
    starting_nonbasic_strategy->addItem("CRASH");
    QLabel* starting_nonbasic_states_label=new QLabel("starting_nonbasic_states");
    QLabel* starting_basis_strategy_label=new QLabel("starting_basis_strategy");

    _basis->addWidget(starting_nonbasic_states_label);
    _basis->addWidget(starting_nonbasic_states);
    _basis->addWidget(starting_basis_strategy_label);
    _basis->addWidget(starting_nonbasic_strategy);
    basis->setLayout(_basis);

    QVBoxLayout* all=new QVBoxLayout;
    all->addLayout(sLayout);
    all->addWidget(basis);

    verticalStart->setLayout(all);
    verticalStart->setFixedWidth(180);
    //verticalStart->setSizePolicy( sizePolicy );

    //createFactorization
    QGroupBox* verticalFact=new QGroupBox("Factorization");
    QGridLayout* fLayout=new QGridLayout;

    QComboBox* type=new QComboBox;
    type->addItem("PFI");
    type->addItem("LU");
    QLabel* reinversion_frequency_label=new QLabel("reinversion_frequency");
    QLabel* pivot_threshold_label=new QLabel("pivot_threshold");
    QLineEdit* reinversion_frequency=new QLineEdit("30");
    QLineEdit* pivot_threshold=new QLineEdit("0.1");

    fLayout->addWidget(type);
    fLayout->addWidget(reinversion_frequency_label,1,0);
    fLayout->addWidget(reinversion_frequency,1,1);
    fLayout->addWidget(pivot_threshold_label,2,0);
    fLayout->addWidget(pivot_threshold,2,1);

    QGroupBox* PFI=new QGroupBox("PFI");
    QVBoxLayout* combos=new QVBoxLayout;
    QComboBox* nontriangular_method=new QComboBox;
    QLabel* nontriangular_method_label=new QLabel("nontriangular_method");
    QLabel* nontriangular_pivot_rule_label=new QLabel("nontriangular_pivot_rule");
    nontriangular_method->addItem("SIMPLE KERNEL");
    nontriangular_method->addItem("BLOCK_PIVOT");
    nontriangular_method->addItem("BLOCK_ORDER");
    nontriangular_method->setCurrentIndex(2);
    QComboBox* nontriangular_pivot_rule=new QComboBox;
    nontriangular_pivot_rule->addItem("SIMPLE_PIVOT");
    nontriangular_pivot_rule->addItem("THRESHOLD_PRIVOT");
    nontriangular_pivot_rule->setCurrentIndex(1);
    combos->addWidget(nontriangular_method_label);
    combos->addWidget(nontriangular_method);
    combos->addWidget(nontriangular_pivot_rule_label);
    combos->addWidget(nontriangular_pivot_rule);
    PFI->setLayout(combos);

    QVBoxLayout* all1=new QVBoxLayout;
    all1->addLayout(fLayout);
    all1->addWidget(PFI);

    verticalFact->setLayout(all1);
    verticalFact->setSizePolicy( sizePolicy );
    //verticalFact->resize(200,200);
    verticalFact->baseSize();
   // parameterTabLayout->addWidget(verticalFact);


    //createPricing
    QGroupBox* verticalPric=new QGroupBox("Pricing");
    QLabel* type1_label=new QLabel("type");
    QComboBox* type1=new QComboBox;
    type1->addItem("DANTZIG");
    type1->addItem("DEVEX");
    type1->addItem("STEEPEST_EDGE");

    QGroupBox* simpri=new QGroupBox("Simpri");
    QGridLayout* phase=new QGridLayout;
    QLineEdit* phaseI_clusters= new QLineEdit("1");
    QLineEdit* phaseI_visit_clusters= new QLineEdit("1");
    QLineEdit* phaseI_improving_candidates= new QLineEdit("0");
    QLineEdit* phaseII_clusters= new QLineEdit("1");
    QLineEdit* phaseII_visit_clusters= new QLineEdit("1");
    QLineEdit* phaseII_improving_candidates= new QLineEdit("0");

    QLabel* phaseI_clusters_label= new QLabel("phaseI_clusters");
    QLabel* phaseI_visit_clusters_label= new QLabel("phaseI_visit_clusters");
    QLabel* phaseI_improving_candidates_label= new QLabel("phaseI_improving_candidates");
    QLabel* phaseII_clusters_label= new QLabel("phaseII_clusters");
    QLabel* phaseII_visit_clusters_label= new QLabel("phaseII_visit_clusters");
    QLabel* phaseII_improving_candidates_label= new QLabel("phaseII_improving_candidates");

    phase->addWidget(phaseI_clusters_label,1,0);
    phase->addWidget(phaseI_clusters,1,1);
    phase->addWidget(phaseI_visit_clusters_label,2,0);
    phase->addWidget(phaseI_visit_clusters,2,1);
    phase->addWidget(phaseI_improving_candidates_label,3,0);
    phase->addWidget(phaseI_improving_candidates,3,1);
    phase->addWidget(phaseII_clusters_label,4,0);
    phase->addWidget(phaseII_clusters,4,1);
    phase->addWidget(phaseII_visit_clusters_label,5,0);
    phase->addWidget(phaseII_visit_clusters,5,1);
    phase->addWidget(phaseII_improving_candidates_label,6,0);
    phase->addWidget(phaseII_improving_candidates,6,1);

    simpri->setLayout(phase);
    QVBoxLayout* all2=new QVBoxLayout;
    all2->addWidget(type1_label);
    all2->addWidget(type);
    all2->addWidget(simpri);

    verticalPric->setLayout(all2);
    verticalPric->setSizePolicy( sizePolicy );
   // parameterTabLayout->addWidget(verticalPric);

    //createRatiotest
    QGroupBox* verticalRatio=new QGroupBox("Ratiotest");
    QCheckBox* e_pivot_generation=new QCheckBox;
    e_pivot_generation->setText("e_pivot_generation");
    e_pivot_generation->isEnabled();
    e_pivot_generation->click();
    QLabel* nonlinear_primal_phaseI_function_label=new QLabel("nonlinear_primal_phaseI_function");
    QComboBox* nonlinear_primal_phaseI_function=new QComboBox;
    nonlinear_primal_phaseI_function->addItem("TRADITIONAL");
    nonlinear_primal_phaseI_function->addItem("PIECEWISE");
    nonlinear_primal_phaseI_function->addItem("PIECEWISE_THRESHOLD");
    nonlinear_primal_phaseI_function->setCurrentIndex(1);

    QLabel* nonlinear_dual_phaseI_function_label=new QLabel("nonlinear_dual_phaseI_function");
    QComboBox* nonlinear_dual_phaseI_function=new QComboBox;
    nonlinear_dual_phaseI_function->addItem("TRADITIONAL");
    nonlinear_dual_phaseI_function->addItem("PIECEWISE");
    nonlinear_dual_phaseI_function->addItem("PIECEWISE_THRESHOLD");
    nonlinear_dual_phaseI_function->setCurrentIndex(1);

    QLabel* nonlinear_dual_phaseII_function_label=new QLabel("nonlinear_dual_phaseII_function");
    QComboBox* nonlinear_dual_phaseII_function=new QComboBox;
    nonlinear_dual_phaseII_function->addItem("TRADITIONAL");
    nonlinear_dual_phaseII_function->addItem("PIECEWISE");
    nonlinear_dual_phaseII_function->addItem("PIECEWISE_THRESHOLD");
    nonlinear_dual_phaseII_function->setCurrentIndex(1);

    QCheckBox* enable_fake_feasibility=new QCheckBox;
    enable_fake_feasibility->setText("enable_fake_feasibility");
    enable_fake_feasibility->isEnabled();

    QVBoxLayout* rat=new QVBoxLayout;
    rat->addWidget(e_pivot_generation);
    rat->addWidget(nonlinear_primal_phaseI_function_label);
    rat->addWidget(nonlinear_primal_phaseI_function);
    rat->addWidget(nonlinear_dual_phaseI_function_label);
    rat->addWidget(nonlinear_dual_phaseI_function);
    rat->addWidget(nonlinear_dual_phaseII_function_label);
    rat->addWidget(nonlinear_dual_phaseII_function);
    rat->addWidget(enable_fake_feasibility);

    QGroupBox* expand=new QGroupBox("Expand");
    QGridLayout* ex=new QGridLayout;

    QLineEdit* multiplier=new QLineEdit("0.01");
    QLineEdit* divider=new QLineEdit("10000");
    QLabel* multiplier_label=new QLabel("multiplier");
    QLabel* divider_label=new QLabel("divider");
    QCheckBox* avoidthetamin=new QCheckBox;
    avoidthetamin->setText("avoidthetamin");
    avoidthetamin->isEnabled();
    QComboBox* type2=new QComboBox;
    QLabel* type2_label=new QLabel("type");
    type2->addItem("EXPAND");
    type2->addItem("INACTIVE");
    type2->addItem("HARRIS");
    type2->addItem("EXPANDING");
    type2->setCurrentIndex(3);

    ex->addWidget(multiplier_label,1,0);
    ex->addWidget(multiplier,1,1);
    ex->addWidget(divider_label,2,0);
    ex->addWidget(divider,2,1);
    ex->addWidget(avoidthetamin);
    ex->addWidget(type2_label,4,0);
    ex->addWidget(type2);
    expand->setLayout(ex);
    QVBoxLayout* all3=new QVBoxLayout;
    all3->addLayout(rat);
    all3->addWidget(expand);
    verticalRatio->setLayout(all3);
    verticalRatio->setSizePolicy( sizePolicy );
   // parameterTabLayout->addWidget(verticalRatio);

    //createPerturbation
    QGroupBox* verticalPert=new QGroupBox("Perturbation");

    QLabel* perturb_cost_vector_label=new QLabel("perturb_cost_vector");
    QComboBox* perturb_cost_vector=new QComboBox;
    perturb_cost_vector->addItem("INACTIVE");
    perturb_cost_vector->addItem("RANDOM");
    perturb_cost_vector->addItem("FEASIBLE");
    perturb_cost_vector->addItem("SIGN");
    perturb_cost_vector->addItem("KOBERSTEIN");
    perturb_cost_vector->setCurrentIndex(0);

    QLabel* perturb_target_label=new QLabel("perturb_target");
    QComboBox* perturb_target=new QComboBox;
    perturb_target->addItem("NONZEROS");
    perturb_target->addItem("ZEROS");
    perturb_target->addItem("ALL");
    perturb_cost_vector->setCurrentIndex(2);
    QCheckBox* perturb_logical=new QCheckBox("perturb_logical");
    QLabel* e_cost_vector_label=new QLabel("e_cost_vector");
    QLabel* xi_multiplier_label=new QLabel("xi_multiplier");
    QLabel* psi_label=new QLabel("psi");
    QLineEdit* e_cost_vector=new QLineEdit("1e-006");
    QLineEdit* xi_multiplier=new QLineEdit("100");
    QLineEdit* psi=new QLineEdit("1e-005");
    QLabel* weighting_label=new QLabel("weighting");
    QComboBox* weighting=new QComboBox;
    weighting->addItem("INACTIVE");
    weighting->addItem("WEIGHT");
    weighting->addItem("SET_TO_INTERVAL");
    QCheckBox* perturb_rhs=new QCheckBox("perturb_rhs");
    QLabel* e_rhs_label=new QLabel("e_rhs");
    QLineEdit* e_rhs=new QLineEdit("1e-006");
    QCheckBox* shift_bounds=new QCheckBox("shift_bounds");
    QLabel* e_bounds_label=new QLabel("e_bounds");
    QLineEdit* e_bounds=new QLineEdit("1e-006");

    QGridLayout* pertu=new QGridLayout;
    pertu->addWidget(perturb_cost_vector_label);
    pertu->addWidget(perturb_cost_vector);
    pertu->addWidget(perturb_target_label);
    pertu->addWidget(perturb_target);
    pertu->addWidget(perturb_logical);
    pertu->addWidget(e_cost_vector_label,5,0);
    pertu->addWidget(e_cost_vector,5,1);
    pertu->addWidget(xi_multiplier_label,6,0);
    pertu->addWidget(xi_multiplier,6,1);
    pertu->addWidget(psi_label,7,0);
    pertu->addWidget(psi,7,1);
    pertu->addWidget(weighting_label);
    pertu->addWidget(weighting);
    pertu->addWidget(perturb_rhs);
    pertu->addWidget(e_rhs_label,10,0);
    pertu->addWidget(e_rhs,10,1);
    pertu->addWidget(shift_bounds);
    pertu->addWidget(e_bounds_label,12,0);
    pertu->addWidget(e_bounds,12,1);

    verticalPert->setLayout(pertu);
    verticalPert->setSizePolicy( sizePolicy );
   // parameterTabLayout->addWidget(verticalPert);

    //createGlobal

    QGroupBox* verticalGlob=new QGroupBox("Global");
    QWidget* ezvan=pageTabs->widget(1);
    QGridLayout* all4=new QGridLayout(ezvan);

    QLabel* starting_algorithm_label=new QLabel("starting_algorithm");
    QComboBox* starting_algorithm=new QComboBox;
    starting_algorithm->addItem("PRIMAL");
    starting_algorithm->addItem("DUAL");
    starting_algorithm->setCurrentIndex(1);
    QLabel* switch_algorithm_label=new QLabel("switch:algorithm");
    QComboBox* switch_algorithm=new QComboBox;
    switch_algorithm->addItem("INACTIVE");
    switch_algorithm->addItem("SWITCH_BEFORE_INV");
    switch_algorithm->addItem("SWITCH_BEFORE_INV_PH2");
    switch_algorithm->addItem("SWITCH_WHEN_NO_IMPR");
    QLabel* repeat_solution_label=new QLabel("repeat_solution");
    QLineEdit* repeat_solution=new QLineEdit("0");
    QLabel* debug_level_label=new QLabel("debug_level");
    QComboBox* debug_level=new QComboBox;
    debug_level->addItem("0");
    debug_level->addItem("1");
    debug_level->addItem("2");
    debug_level->addItem("3");
    debug_level->setCurrentIndex(1);
    QLabel* iteration_limit_label=new QLabel("iteration_limit");
    QLineEdit* iteration_limit=new QLineEdit("200000");
    QLabel* time_limit_label=new QLabel("time_limit");
    QLineEdit* time_limit=new QLineEdit("3600");
    QCheckBox* batch_output=new QCheckBox("batch_output");
    QLabel* batch_size_label=new QLabel("batch_size");
    QLineEdit* batch_size=new QLineEdit("10");

    QGroupBox* save=new QGroupBox("SaveBasis");
    QVBoxLayout* saveBasis=new QVBoxLayout;
    QCheckBox* basis1=new QCheckBox("basis");
    QLabel* filename_label=new QLabel("filename");
    QLineEdit* filename=new QLineEdit("basis");
    QLabel* format_label=new QLabel("format");
    QComboBox* format=new QComboBox;
    format->addItem("PBF");
    format->addItem("BAS");

    all4->addWidget(starting_algorithm_label);
    all4->addWidget(starting_algorithm);
    all4->addWidget(switch_algorithm_label);
    all4->addWidget(switch_algorithm);
    all4->addWidget(repeat_solution_label,5,0);
    all4->addWidget(repeat_solution,5,1);
    all4->addWidget(debug_level_label);
    all4->addWidget(debug_level);
    all4->addWidget(iteration_limit_label,7,0);
    all4->addWidget(iteration_limit,7,1);
    all4->addWidget(time_limit_label,8,0);
    all4->addWidget(time_limit,8,1);
    all4->addWidget(batch_output);
    all4->addWidget(batch_size_label,10,0);
    all4->addWidget(batch_size,10,1);

    QCheckBox* last_basis=new QCheckBox("last_basis");
    QLabel* iteration_label=new QLabel("iteration");
    QLineEdit* iteration=new QLineEdit("0");
    QLabel* periodically_label=new QLabel("periodically");
    QLineEdit* periodically=new QLineEdit("0");
    saveBasis->addWidget(basis1);
    saveBasis->addWidget(filename_label);
    saveBasis->addWidget(filename);
    saveBasis->addWidget(format_label);
    saveBasis->addWidget(format);
    saveBasis->addWidget(last_basis);
    saveBasis->addWidget(iteration_label);
    saveBasis->addWidget(iteration);
    saveBasis->addWidget(periodically_label);
    saveBasis->addWidget(periodically);

    save->setLayout(saveBasis);

    QGroupBox* load=new QGroupBox("LoadBasis");
    QVBoxLayout* loadBasis=new QVBoxLayout;
    QCheckBox* basis2=new QCheckBox("basis");
    QLabel* filename_label1=new QLabel("filename");
    QLineEdit* filename1=new QLineEdit("basis");
    QLabel* format1_label=new QLabel("format");
    QComboBox* format1=new QComboBox;
    format1->addItem("PBF");
    format1->addItem("BAS");
    loadBasis->addWidget(basis2);
    loadBasis->addWidget(filename_label1);
    loadBasis->addWidget(filename1);
    loadBasis->addWidget(format1_label);
    loadBasis->addWidget(format1);

    load->setLayout(loadBasis);

    QGroupBox* _export=new QGroupBox("Export");
    QVBoxLayout* exportBasis=new QVBoxLayout;
    QCheckBox* enable=new QCheckBox("enable");
    QLabel* filename_label2=new QLabel("filename");
    QLineEdit* filename2=new QLineEdit("exported_result.txt");
    QLabel* type3_label=new QLabel("type");
    QComboBox* type3=new QComboBox;
    type3->addItem("REVISION_CHECK");
    exportBasis->addWidget(enable);
    exportBasis->addWidget(filename_label2);
    exportBasis->addWidget(filename2);
    exportBasis->addWidget(type3_label);
    exportBasis->addWidget(type3);

    _export->setLayout(exportBasis);

    all4->addWidget(save);
    all4->addWidget(load);
    all4->addWidget(_export);
    verticalGlob->setSizePolicy( sizePolicy );



    QHBoxLayout* tsfp=new QHBoxLayout;
    tsfp->addWidget(verticalTol);
    tsfp->addWidget(verticalStart);
    tsfp->addWidget(verticalFact);

    QHBoxLayout* rpg=new QHBoxLayout;
    rpg->addWidget(verticalPric);
    rpg->addWidget(verticalRatio);
    rpg->addWidget(verticalPert);



    QWidget* ezvan2=pageTabs->widget(0);
    QVBoxLayout* tg=new QVBoxLayout(ezvan2);
    tg->addLayout(tsfp);
    tg->addLayout(rpg);

    parameterTabLayout->addWidget(pageTabs);

}

