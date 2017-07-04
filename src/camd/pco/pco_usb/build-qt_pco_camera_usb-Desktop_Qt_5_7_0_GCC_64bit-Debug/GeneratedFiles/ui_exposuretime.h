/********************************************************************************
** Form generated from reading UI file 'exposuretime.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EXPOSURETIME_H
#define UI_EXPOSURETIME_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Exposuretime
{
public:
    QVBoxLayout *verticalLayout;
    QGridLayout *gridLayout;
    QLabel *ExposureDescriptionLabel;
    QLabel *DelayLabel;
    QLabel *DelayDescriptionLabel;
    QLabel *TimingLabel;
    QLabel *DelayCurrentValue;
    QLabel *ExposureCurrentValue;
    QLabel *DelayCurrentValueLabel;
    QLabel *ExposureLabel;
    QLineEdit *DelayLineEdit;
    QLabel *CurrentExposureLabel;
    QLineEdit *ExposureLineEdit;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QPushButton *CloseButton;
    QPushButton *ApplyButton;
    QLabel *label;

    void setupUi(QDialog *Exposuretime)
    {
        if (Exposuretime->objectName().isEmpty())
            Exposuretime->setObjectName(QStringLiteral("Exposuretime"));
        Exposuretime->resize(277, 219);
        verticalLayout = new QVBoxLayout(Exposuretime);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        gridLayout = new QGridLayout();
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        ExposureDescriptionLabel = new QLabel(Exposuretime);
        ExposureDescriptionLabel->setObjectName(QStringLiteral("ExposureDescriptionLabel"));
        ExposureDescriptionLabel->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ExposureDescriptionLabel, 3, 2, 1, 1);

        DelayLabel = new QLabel(Exposuretime);
        DelayLabel->setObjectName(QStringLiteral("DelayLabel"));

        gridLayout->addWidget(DelayLabel, 5, 0, 1, 1);

        DelayDescriptionLabel = new QLabel(Exposuretime);
        DelayDescriptionLabel->setObjectName(QStringLiteral("DelayDescriptionLabel"));
        DelayDescriptionLabel->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(DelayDescriptionLabel, 6, 2, 1, 1);

        TimingLabel = new QLabel(Exposuretime);
        TimingLabel->setObjectName(QStringLiteral("TimingLabel"));

        gridLayout->addWidget(TimingLabel, 0, 0, 1, 1);

        DelayCurrentValue = new QLabel(Exposuretime);
        DelayCurrentValue->setObjectName(QStringLiteral("DelayCurrentValue"));
        DelayCurrentValue->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(DelayCurrentValue, 4, 2, 1, 1);

        ExposureCurrentValue = new QLabel(Exposuretime);
        ExposureCurrentValue->setObjectName(QStringLiteral("ExposureCurrentValue"));
        ExposureCurrentValue->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(ExposureCurrentValue, 1, 2, 1, 1);

        DelayCurrentValueLabel = new QLabel(Exposuretime);
        DelayCurrentValueLabel->setObjectName(QStringLiteral("DelayCurrentValueLabel"));

        gridLayout->addWidget(DelayCurrentValueLabel, 4, 0, 1, 1);

        ExposureLabel = new QLabel(Exposuretime);
        ExposureLabel->setObjectName(QStringLiteral("ExposureLabel"));

        gridLayout->addWidget(ExposureLabel, 2, 0, 1, 1);

        DelayLineEdit = new QLineEdit(Exposuretime);
        DelayLineEdit->setObjectName(QStringLiteral("DelayLineEdit"));

        gridLayout->addWidget(DelayLineEdit, 5, 2, 1, 1);

        CurrentExposureLabel = new QLabel(Exposuretime);
        CurrentExposureLabel->setObjectName(QStringLiteral("CurrentExposureLabel"));

        gridLayout->addWidget(CurrentExposureLabel, 1, 0, 1, 1);

        ExposureLineEdit = new QLineEdit(Exposuretime);
        ExposureLineEdit->setObjectName(QStringLiteral("ExposureLineEdit"));

        gridLayout->addWidget(ExposureLineEdit, 2, 2, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(horizontalSpacer_2, 0, 1, 7, 1);


        verticalLayout->addLayout(gridLayout);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        CloseButton = new QPushButton(Exposuretime);
        CloseButton->setObjectName(QStringLiteral("CloseButton"));

        horizontalLayout->addWidget(CloseButton);

        ApplyButton = new QPushButton(Exposuretime);
        ApplyButton->setObjectName(QStringLiteral("ApplyButton"));

        horizontalLayout->addWidget(ApplyButton);


        verticalLayout->addLayout(horizontalLayout);

        label = new QLabel(Exposuretime);
        label->setObjectName(QStringLiteral("label"));
        label->setWordWrap(true);

        verticalLayout->addWidget(label);


        retranslateUi(Exposuretime);
        QObject::connect(CloseButton, SIGNAL(clicked()), Exposuretime, SLOT(accept()));

        QMetaObject::connectSlotsByName(Exposuretime);
    } // setupUi

    void retranslateUi(QDialog *Exposuretime)
    {
        Exposuretime->setWindowTitle(QApplication::translate("Exposuretime", "Dialog", 0));
        ExposureDescriptionLabel->setText(QApplication::translate("Exposuretime", "min:s:ms:\302\265s", 0));
        DelayLabel->setText(QApplication::translate("Exposuretime", "Delay", 0));
        DelayDescriptionLabel->setText(QApplication::translate("Exposuretime", "min:s:ms:\302\265s", 0));
        TimingLabel->setText(QApplication::translate("Exposuretime", "Timing", 0));
        DelayCurrentValue->setText(QApplication::translate("Exposuretime", "00:00:000:000", 0));
        ExposureCurrentValue->setText(QApplication::translate("Exposuretime", "00:00:000:000", 0));
        DelayCurrentValueLabel->setText(QApplication::translate("Exposuretime", "Current Delay", 0));
        ExposureLabel->setText(QApplication::translate("Exposuretime", "Exposure", 0));
        DelayLineEdit->setInputMask(QApplication::translate("Exposuretime", "00:00:000:000", 0));
        DelayLineEdit->setText(QApplication::translate("Exposuretime", "00:00:000:000", 0));
        CurrentExposureLabel->setText(QApplication::translate("Exposuretime", "Current Exposure", 0));
        ExposureLineEdit->setInputMask(QApplication::translate("Exposuretime", "00:00:000:000", 0));
        ExposureLineEdit->setText(QApplication::translate("Exposuretime", "00:00:000:000", 0));
        CloseButton->setText(QApplication::translate("Exposuretime", "Close", 0));
        ApplyButton->setText(QApplication::translate("Exposuretime", "Apply", 0));
        label->setText(QApplication::translate("Exposuretime", "Full setup is only available if the camera is not running!", 0));
    } // retranslateUi

};

namespace Ui {
    class Exposuretime: public Ui_Exposuretime {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EXPOSURETIME_H
