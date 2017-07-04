/********************************************************************************
** Form generated from reading UI file 'memorymanager.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MEMORYMANAGER_H
#define UI_MEMORYMANAGER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_MemoryManager
{
public:
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout;
    QLabel *AvailableLabel;
    QSpinBox *Input;
    QSpinBox *AvailableDisplay;
    QLabel *InputLabel;
    QSpacerItem *horizontalSpacer_3;
    QLabel *label;
    QLabel *label_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QPushButton *GrabOk;
    QPushButton *GrabCancel;
    QSpacerItem *horizontalSpacer_2;

    void setupUi(QDialog *MemoryManager)
    {
        if (MemoryManager->objectName().isEmpty())
            MemoryManager->setObjectName(QStringLiteral("MemoryManager"));
        MemoryManager->setWindowModality(Qt::NonModal);
        MemoryManager->resize(273, 110);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MemoryManager->sizePolicy().hasHeightForWidth());
        MemoryManager->setSizePolicy(sizePolicy);
        MemoryManager->setMinimumSize(QSize(200, 70));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/PCOApp.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MemoryManager->setWindowIcon(icon);
        MemoryManager->setModal(true);
        verticalLayout_2 = new QVBoxLayout(MemoryManager);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        AvailableLabel = new QLabel(MemoryManager);
        AvailableLabel->setObjectName(QStringLiteral("AvailableLabel"));
        AvailableLabel->setMinimumSize(QSize(111, 23));

        gridLayout->addWidget(AvailableLabel, 1, 0, 1, 1);

        Input = new QSpinBox(MemoryManager);
        Input->setObjectName(QStringLiteral("Input"));
        Input->setMinimumSize(QSize(72, 22));
        Input->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        Input->setButtonSymbols(QAbstractSpinBox::NoButtons);
        Input->setMaximum(5000);

        gridLayout->addWidget(Input, 0, 2, 1, 1);

        AvailableDisplay = new QSpinBox(MemoryManager);
        AvailableDisplay->setObjectName(QStringLiteral("AvailableDisplay"));
        AvailableDisplay->setMinimumSize(QSize(72, 22));
        QPalette palette;
        QBrush brush(QColor(214, 210, 208, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        AvailableDisplay->setPalette(palette);
        AvailableDisplay->setContextMenuPolicy(Qt::PreventContextMenu);
        AvailableDisplay->setAcceptDrops(false);
        AvailableDisplay->setAutoFillBackground(true);
        AvailableDisplay->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        AvailableDisplay->setReadOnly(true);
        AvailableDisplay->setButtonSymbols(QAbstractSpinBox::NoButtons);
        AvailableDisplay->setMaximum(999999999);

        gridLayout->addWidget(AvailableDisplay, 1, 2, 1, 1);

        InputLabel = new QLabel(MemoryManager);
        InputLabel->setObjectName(QStringLiteral("InputLabel"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(InputLabel->sizePolicy().hasHeightForWidth());
        InputLabel->setSizePolicy(sizePolicy1);
        InputLabel->setMinimumSize(QSize(114, 23));

        gridLayout->addWidget(InputLabel, 0, 0, 1, 1);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_3, 0, 1, 2, 1);

        label = new QLabel(MemoryManager);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 3, 1, 1);

        label_2 = new QLabel(MemoryManager);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 1, 3, 1, 1);


        verticalLayout_2->addLayout(gridLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);

        GrabOk = new QPushButton(MemoryManager);
        GrabOk->setObjectName(QStringLiteral("GrabOk"));

        horizontalLayout_2->addWidget(GrabOk);

        GrabCancel = new QPushButton(MemoryManager);
        GrabCancel->setObjectName(QStringLiteral("GrabCancel"));

        horizontalLayout_2->addWidget(GrabCancel);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout_2);


        retranslateUi(MemoryManager);
        QObject::connect(GrabCancel, SIGNAL(clicked()), MemoryManager, SLOT(reject()));
        QObject::connect(GrabOk, SIGNAL(clicked()), MemoryManager, SLOT(accept()));

        QMetaObject::connectSlotsByName(MemoryManager);
    } // setupUi

    void retranslateUi(QDialog *MemoryManager)
    {
        MemoryManager->setWindowTitle(QApplication::translate("MemoryManager", "Mem. Manager", 0));
        AvailableLabel->setText(QApplication::translate("MemoryManager", "Memory Available:", 0));
        InputLabel->setText(QApplication::translate("MemoryManager", "Memory to Assign:", 0));
        label->setText(QApplication::translate("MemoryManager", "Pictures", 0));
        label_2->setText(QApplication::translate("MemoryManager", "Pictures", 0));
        GrabOk->setText(QApplication::translate("MemoryManager", "Ok", 0));
        GrabCancel->setText(QApplication::translate("MemoryManager", "Cancel", 0));
    } // retranslateUi

};

namespace Ui {
    class MemoryManager: public Ui_MemoryManager {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MEMORYMANAGER_H
