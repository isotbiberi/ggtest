/********************************************************************************
** Form generated from reading UI file 'errorpopup.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ERRORPOPUP_H
#define UI_ERRORPOPUP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_ErrorPopup
{
public:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *ErrorPopupText;
    QLabel *ErrorPopupNumber;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *ErrorPopupOk;

    void setupUi(QDialog *ErrorPopup)
    {
        if (ErrorPopup->objectName().isEmpty())
            ErrorPopup->setObjectName(QStringLiteral("ErrorPopup"));
        ErrorPopup->resize(294, 185);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ErrorPopup->sizePolicy().hasHeightForWidth());
        ErrorPopup->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/PCOApp.ico"), QSize(), QIcon::Normal, QIcon::Off);
        ErrorPopup->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(ErrorPopup);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        ErrorPopupText = new QLabel(ErrorPopup);
        ErrorPopupText->setObjectName(QStringLiteral("ErrorPopupText"));
        sizePolicy.setHeightForWidth(ErrorPopupText->sizePolicy().hasHeightForWidth());
        ErrorPopupText->setSizePolicy(sizePolicy);
        ErrorPopupText->setScaledContents(true);
        ErrorPopupText->setAlignment(Qt::AlignCenter);
        ErrorPopupText->setWordWrap(true);

        verticalLayout_2->addWidget(ErrorPopupText);

        ErrorPopupNumber = new QLabel(ErrorPopup);
        ErrorPopupNumber->setObjectName(QStringLiteral("ErrorPopupNumber"));

        verticalLayout_2->addWidget(ErrorPopupNumber, 0, Qt::AlignHCenter);


        verticalLayout->addLayout(verticalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetNoConstraint);
        ErrorPopupOk = new QPushButton(ErrorPopup);
        ErrorPopupOk->setObjectName(QStringLiteral("ErrorPopupOk"));
        QSizePolicy sizePolicy1(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(ErrorPopupOk->sizePolicy().hasHeightForWidth());
        ErrorPopupOk->setSizePolicy(sizePolicy1);
        ErrorPopupOk->setMinimumSize(QSize(70, 40));
        ErrorPopupOk->setMaximumSize(QSize(70, 40));

        horizontalLayout->addWidget(ErrorPopupOk);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(ErrorPopup);
        QObject::connect(ErrorPopupOk, SIGNAL(clicked()), ErrorPopup, SLOT(accept()));

        QMetaObject::connectSlotsByName(ErrorPopup);
    } // setupUi

    void retranslateUi(QDialog *ErrorPopup)
    {
        ErrorPopup->setWindowTitle(QApplication::translate("ErrorPopup", "Error", 0));
        ErrorPopupText->setText(QApplication::translate("ErrorPopup", "This is a mega super long Error message I don't know what to do with it omg its so long it never ends\n"
"Errornumber", 0));
        ErrorPopupNumber->setText(QString());
        ErrorPopupOk->setText(QApplication::translate("ErrorPopup", "Ok", 0));
    } // retranslateUi

};

namespace Ui {
    class ErrorPopup: public Ui_ErrorPopup {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ERRORPOPUP_H
