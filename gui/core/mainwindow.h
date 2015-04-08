#ifndef MAINWINDOW_H
#define MAINWINDOW_H

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

#include "constants.h"
#include "gui/modules/panopthandler.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

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

    PanOptHandler* panOpt;

    QFileDialog fileDialog;
    QMap<QString, QAction*> actions;
    QMap<QString, QToolButton*> tools;
};

#endif // MAINWINDOW_H
