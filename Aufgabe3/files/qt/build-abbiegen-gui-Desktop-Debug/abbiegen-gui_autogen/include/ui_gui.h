/********************************************************************************
** Form generated from reading UI file 'gui.ui'
**
** Created by: Qt User Interface Compiler version 5.14.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GUI_H
#define UI_GUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_gui
{
public:
    QWidget *centralwidget;
    QPushButton *browse;
    QLineEdit *inputMax;
    QLabel *label;
    QCheckBox *approx;
    QTextBrowser *textBrowser;
    QPushButton *start;
    QCheckBox *defaultSave;
    QPushButton *saveAsResult;
    QPushButton *saveAsShort;
    QPushButton *saveAsMinimal;
    QLineEdit *inputSaveResult;
    QLineEdit *inputSaveShort;
    QLineEdit *inputSaveMinimal;
    QPushButton *ok;
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label_4;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *saveAll;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *gui)
    {
        if (gui->objectName().isEmpty())
            gui->setObjectName(QString::fromUtf8("gui"));
        gui->resize(800, 649);
        centralwidget = new QWidget(gui);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        browse = new QPushButton(centralwidget);
        browse->setObjectName(QString::fromUtf8("browse"));
        browse->setGeometry(QRect(40, 80, 121, 41));
        inputMax = new QLineEdit(centralwidget);
        inputMax->setObjectName(QString::fromUtf8("inputMax"));
        inputMax->setGeometry(QRect(40, 160, 121, 25));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(40, 130, 301, 21));
        QFont font;
        font.setPointSize(13);
        label->setFont(font);
        approx = new QCheckBox(centralwidget);
        approx->setObjectName(QString::fromUtf8("approx"));
        approx->setGeometry(QRect(40, 190, 241, 26));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Trebuchet MS"));
        font1.setPointSize(13);
        approx->setFont(font1);
        approx->setAutoFillBackground(false);
        textBrowser = new QTextBrowser(centralwidget);
        textBrowser->setObjectName(QString::fromUtf8("textBrowser"));
        textBrowser->setGeometry(QRect(20, 290, 761, 321));
        start = new QPushButton(centralwidget);
        start->setObjectName(QString::fromUtf8("start"));
        start->setGeometry(QRect(40, 230, 121, 41));
        defaultSave = new QCheckBox(centralwidget);
        defaultSave->setObjectName(QString::fromUtf8("defaultSave"));
        defaultSave->setEnabled(true);
        defaultSave->setGeometry(QRect(420, 80, 131, 31));
        QFont font2;
        font2.setPointSize(13);
        font2.setBold(false);
        font2.setWeight(50);
        font2.setKerning(true);
        defaultSave->setFont(font2);
        saveAsResult = new QPushButton(centralwidget);
        saveAsResult->setObjectName(QString::fromUtf8("saveAsResult"));
        saveAsResult->setGeometry(QRect(410, 120, 121, 41));
        saveAsShort = new QPushButton(centralwidget);
        saveAsShort->setObjectName(QString::fromUtf8("saveAsShort"));
        saveAsShort->setGeometry(QRect(410, 170, 121, 41));
        saveAsMinimal = new QPushButton(centralwidget);
        saveAsMinimal->setObjectName(QString::fromUtf8("saveAsMinimal"));
        saveAsMinimal->setGeometry(QRect(410, 220, 121, 41));
        inputSaveResult = new QLineEdit(centralwidget);
        inputSaveResult->setObjectName(QString::fromUtf8("inputSaveResult"));
        inputSaveResult->setGeometry(QRect(540, 130, 241, 25));
        inputSaveShort = new QLineEdit(centralwidget);
        inputSaveShort->setObjectName(QString::fromUtf8("inputSaveShort"));
        inputSaveShort->setGeometry(QRect(540, 180, 241, 25));
        inputSaveMinimal = new QLineEdit(centralwidget);
        inputSaveMinimal->setObjectName(QString::fromUtf8("inputSaveMinimal"));
        inputSaveMinimal->setGeometry(QRect(540, 230, 241, 25));
        ok = new QPushButton(centralwidget);
        ok->setObjectName(QString::fromUtf8("ok"));
        ok->setGeometry(QRect(170, 160, 31, 25));
        horizontalLayoutWidget = new QWidget(centralwidget);
        horizontalLayoutWidget->setObjectName(QString::fromUtf8("horizontalLayoutWidget"));
        horizontalLayoutWidget->setGeometry(QRect(40, 10, 741, 51));
        horizontalLayout = new QHBoxLayout(horizontalLayoutWidget);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label_4 = new QLabel(horizontalLayoutWidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        horizontalLayout->addWidget(label_4);

        label_2 = new QLabel(horizontalLayoutWidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QFont font3;
        font3.setPointSize(23);
        label_2->setFont(font3);

        horizontalLayout->addWidget(label_2);

        label_3 = new QLabel(horizontalLayoutWidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        horizontalLayout->addWidget(label_3);

        saveAll = new QPushButton(centralwidget);
        saveAll->setObjectName(QString::fromUtf8("saveAll"));
        saveAll->setGeometry(QRect(560, 70, 121, 41));
        gui->setCentralWidget(centralwidget);
        statusbar = new QStatusBar(gui);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        gui->setStatusBar(statusbar);

        retranslateUi(gui);

        QMetaObject::connectSlotsByName(gui);
    } // setupUi

    void retranslateUi(QMainWindow *gui)
    {
        gui->setWindowTitle(QCoreApplication::translate("gui", "Abbiegen", nullptr));
        browse->setText(QCoreApplication::translate("gui", "Browse", nullptr));
        label->setText(QCoreApplication::translate("gui", "maximale Verl\303\244ngerung in % (z.B. 20)", nullptr));
        approx->setText(QCoreApplication::translate("gui", " approximierte Suche", nullptr));
        start->setText(QCoreApplication::translate("gui", "Start", nullptr));
        defaultSave->setText(QCoreApplication::translate("gui", "Default save", nullptr));
        saveAsResult->setText(QCoreApplication::translate("gui", "Save Result As", nullptr));
        saveAsShort->setText(QCoreApplication::translate("gui", "Save Shortest As", nullptr));
        saveAsMinimal->setText(QCoreApplication::translate("gui", "Save Minimal As", nullptr));
        ok->setText(QCoreApplication::translate("gui", "OK", nullptr));
        label_4->setText(QString());
        label_2->setText(QCoreApplication::translate("gui", "         Abbiegen", nullptr));
        label_3->setText(QString());
        saveAll->setText(QCoreApplication::translate("gui", "Save All", nullptr));
    } // retranslateUi

};

namespace Ui {
    class gui: public Ui_gui {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GUI_H
