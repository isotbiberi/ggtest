/********************************************************************************
** Form generated from reading UI file 'simpledialog.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SIMPLEDIALOG_H
#define UI_SIMPLEDIALOG_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_SimpleDialog
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *Question;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *SimpleDialog)
    {
        if (SimpleDialog->objectName().isEmpty())
            SimpleDialog->setObjectName(QStringLiteral("SimpleDialog"));
        SimpleDialog->resize(257, 115);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/PCOApp.ico"), QSize(), QIcon::Normal, QIcon::Off);
        SimpleDialog->setWindowIcon(icon);
        verticalLayout_2 = new QVBoxLayout(SimpleDialog);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        Question = new QLabel(SimpleDialog);
        Question->setObjectName(QStringLiteral("Question"));
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Question->sizePolicy().hasHeightForWidth());
        Question->setSizePolicy(sizePolicy);
        Question->setAlignment(Qt::AlignCenter);
        Question->setWordWrap(true);

        verticalLayout->addWidget(Question);

        buttonBox = new QDialogButtonBox(SimpleDialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox, 0, Qt::AlignHCenter);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(SimpleDialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), SimpleDialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), SimpleDialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(SimpleDialog);
    } // setupUi

    void retranslateUi(QDialog *SimpleDialog)
    {
        SimpleDialog->setWindowTitle(QString());
        Question->setText(QApplication::translate("SimpleDialog", "Ask Question here", 0));
    } // retranslateUi

};

namespace Ui {
    class SimpleDialog: public Ui_SimpleDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SIMPLEDIALOG_H
