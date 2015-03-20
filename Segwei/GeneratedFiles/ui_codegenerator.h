/********************************************************************************
** Form generated from reading UI file 'codegenerator.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CODEGENERATOR_H
#define UI_CODEGENERATOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_codegenerator
{
public:

    void setupUi(QDialog *codegenerator)
    {
        if (codegenerator->objectName().isEmpty())
            codegenerator->setObjectName(QStringLiteral("codegenerator"));
        codegenerator->resize(400, 300);

        retranslateUi(codegenerator);

        QMetaObject::connectSlotsByName(codegenerator);
    } // setupUi

    void retranslateUi(QDialog *codegenerator)
    {
        codegenerator->setWindowTitle(QApplication::translate("codegenerator", "Dialog", 0));
    } // retranslateUi

};

namespace Ui {
    class codegenerator: public Ui_codegenerator {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CODEGENERATOR_H
