#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QToolButton>
#include <QPushButton>
#include <QTabWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QDebug>
#include <QWidget>
#include <QGridLayout>
#include <QFileDialog>
#include <QLabel>
#include <QRadioButton>
#include <QTextEdit>
#include <QSplitter>
#include <QGroupBox>
#include <QtCore>
#include <QtGui>
#include <QComboBox>
#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QLineEdit>
#include <QCheckBox>
#include <QGraphicsScene>
#include <QGraphicsView>

#include "constants.h"
#include "gui/modules/panopthandler.h"
#include "simplex/simplexparameterhandler.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT


private slots:
    void loadFromEditor();
    void drawMatrix();
private:
    void initProblemTab();
    void initParametersTab();
    void initMatrixTab();
    void initSolutionTab();
    void writeSolution(QStringList vars);


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:

    void newProblem();
    void loadProblem();
    void saveProblem();
    void exitProgram();
    void startSolve();
    void cancelSolve();
    void exportSolution();
    void parseProblem();

    void obtainSolution();



private:

    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QTabWidget *mainTabs;
    QStatusBar *statusBar;
    QVBoxLayout *tabLayout;
    QGridLayout *sourceLayout, *paramLayout, *matrixLayout, *solutionLayout;
    QTextEdit *textEdit;
    QTextEdit *solutionText;
    QTableView *solutionStats;
    QComboBox *format;
    QComboBox *zoomBox;
    QTableView *problemStats;
    QLabel *logLabel;
    PanOptHandler* panOpt;

    QFileDialog fileDialog;
    QString selectedFileName;
    QMap<QString, QAction*> actions;
    QMap<QString, QToolButton*> tools;
    QVBoxLayout* matrixRightLayout;
    QGraphicsScene* scene;
    QGraphicsView* matrixView;
    double matrixZoom;

    //parameters
        //Tolerances
        QWidget* parameterTab;
        QHBoxLayout * parameterTabLayout;
};

#endif // MAINWINDOW_H
