/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QPushButton *pushButton;
    QGraphicsView *planeView;
    QTextEdit *output;
    QPushButton *pauseButton;
    QWidget *valueChartContainer;
    QWidget *voltageChartContainer;
    QWidget *temperatureChartContainer;
    QMenuBar *menubar;
    QMenu *menuSensor_Analysis_Tool;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 1158);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        pushButton = new QPushButton(centralwidget);
        pushButton->setObjectName("pushButton");
        pushButton->setGeometry(QRect(500, 10, 105, 24));
        planeView = new QGraphicsView(centralwidget);
        planeView->setObjectName("planeView");
        planeView->setGeometry(QRect(30, 40, 391, 331));
        output = new QTextEdit(centralwidget);
        output->setObjectName("output");
        output->setGeometry(QRect(440, 40, 321, 331));
        pauseButton = new QPushButton(centralwidget);
        pauseButton->setObjectName("pauseButton");
        pauseButton->setGeometry(QRect(630, 10, 80, 24));
        valueChartContainer = new QWidget(centralwidget);
        valueChartContainer->setObjectName("valueChartContainer");
        valueChartContainer->setGeometry(QRect(30, 400, 731, 191));
        voltageChartContainer = new QWidget(centralwidget);
        voltageChartContainer->setObjectName("voltageChartContainer");
        voltageChartContainer->setGeometry(QRect(30, 600, 731, 191));
        temperatureChartContainer = new QWidget(centralwidget);
        temperatureChartContainer->setObjectName("temperatureChartContainer");
        temperatureChartContainer->setGeometry(QRect(30, 810, 731, 261));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 21));
        menuSensor_Analysis_Tool = new QMenu(menubar);
        menuSensor_Analysis_Tool->setObjectName("menuSensor_Analysis_Tool");
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        menubar->addAction(menuSensor_Analysis_Tool->menuAction());

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        pushButton->setText(QCoreApplication::translate("MainWindow", "Start Data Stream", nullptr));
        pauseButton->setText(QCoreApplication::translate("MainWindow", "Stop", nullptr));
        menuSensor_Analysis_Tool->setTitle(QCoreApplication::translate("MainWindow", "Sensor Analysis Tool", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
