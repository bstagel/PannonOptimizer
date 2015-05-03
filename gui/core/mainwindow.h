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
#include <QGraphicsScene>
#include <QGraphicsView>

#include "constants.h"
#include "gui/modules/panopthandler.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT


private slots:
    void onMPSlistSelected(QString item);
private:
    void initProblemTab();
    void initMatrixTab();
    void drawMatrix();


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
    PanOptHandler* panOpt;

    QFileDialog fileDialog;
    QMap<QString, QAction*> actions;
    QMap<QString, QToolButton*> tools;
    QGraphicsScene* scene;
    QGraphicsView* matrixView;
};

#endif // MAINWINDOW_H
