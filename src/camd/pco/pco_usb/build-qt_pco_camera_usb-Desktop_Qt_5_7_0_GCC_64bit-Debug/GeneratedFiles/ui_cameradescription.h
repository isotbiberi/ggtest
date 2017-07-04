/********************************************************************************
** Form generated from reading UI file 'cameradescription.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERADESCRIPTION_H
#define UI_CAMERADESCRIPTION_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_CameraDescription
{
public:
    QVBoxLayout *verticalLayout_2;
    QGridLayout *gridLayout;
    QLabel *SensorType;
    QLabel *SensorTypeLabel;
    QLabel *SerialNumber;
    QLabel *CameraTypeLabel;
    QLabel *Temperature;
    QLabel *TemperatureLabel;
    QLabel *SerialNumberLabel;
    QLabel *CameraType;
    QLabel *PixelRate;
    QLabel *PixelRateLabel;
    QLabel *Resolution;
    QLabel *ResolutionLabel;
    QLabel *CameraHealthLabel;
    QLabel *CameraHealth;
    QLabel *FirmwareLabel;
    QLabel *Firmware;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout;
    QPushButton *CameraDescriptionOkButton;

    void setupUi(QDialog *CameraDescription)
    {
        if (CameraDescription->objectName().isEmpty())
            CameraDescription->setObjectName(QStringLiteral("CameraDescription"));
        CameraDescription->resize(360, 360);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(CameraDescription->sizePolicy().hasHeightForWidth());
        CameraDescription->setSizePolicy(sizePolicy);
        CameraDescription->setMaximumSize(QSize(420, 400));
        QFont font;
        font.setFamily(QStringLiteral("Monospace"));
        font.setPointSize(10);
        CameraDescription->setFont(font);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/PCOApp.ico"), QSize(), QIcon::Normal, QIcon::Off);
        CameraDescription->setWindowIcon(icon);
        verticalLayout_2 = new QVBoxLayout(CameraDescription);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        gridLayout->setHorizontalSpacing(6);
        gridLayout->setContentsMargins(0, -1, 80, -1);
        SensorType = new QLabel(CameraDescription);
        SensorType->setObjectName(QStringLiteral("SensorType"));

        gridLayout->addWidget(SensorType, 6, 1, 1, 1);

        SensorTypeLabel = new QLabel(CameraDescription);
        SensorTypeLabel->setObjectName(QStringLiteral("SensorTypeLabel"));
        SensorTypeLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(SensorTypeLabel, 6, 0, 1, 1);

        SerialNumber = new QLabel(CameraDescription);
        SerialNumber->setObjectName(QStringLiteral("SerialNumber"));

        gridLayout->addWidget(SerialNumber, 7, 1, 1, 1);

        CameraTypeLabel = new QLabel(CameraDescription);
        CameraTypeLabel->setObjectName(QStringLiteral("CameraTypeLabel"));
        CameraTypeLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(CameraTypeLabel, 4, 0, 1, 1);

        Temperature = new QLabel(CameraDescription);
        Temperature->setObjectName(QStringLiteral("Temperature"));

        gridLayout->addWidget(Temperature, 12, 1, 1, 1);

        TemperatureLabel = new QLabel(CameraDescription);
        TemperatureLabel->setObjectName(QStringLiteral("TemperatureLabel"));
        TemperatureLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(TemperatureLabel, 12, 0, 1, 1);

        SerialNumberLabel = new QLabel(CameraDescription);
        SerialNumberLabel->setObjectName(QStringLiteral("SerialNumberLabel"));
        SerialNumberLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(SerialNumberLabel, 7, 0, 1, 1);

        CameraType = new QLabel(CameraDescription);
        CameraType->setObjectName(QStringLiteral("CameraType"));

        gridLayout->addWidget(CameraType, 4, 1, 1, 1);

        PixelRate = new QLabel(CameraDescription);
        PixelRate->setObjectName(QStringLiteral("PixelRate"));
        PixelRate->setWordWrap(true);

        gridLayout->addWidget(PixelRate, 10, 1, 1, 1);

        PixelRateLabel = new QLabel(CameraDescription);
        PixelRateLabel->setObjectName(QStringLiteral("PixelRateLabel"));
        PixelRateLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(PixelRateLabel, 10, 0, 1, 1, Qt::AlignTop);

        Resolution = new QLabel(CameraDescription);
        Resolution->setObjectName(QStringLiteral("Resolution"));

        gridLayout->addWidget(Resolution, 9, 1, 1, 1);

        ResolutionLabel = new QLabel(CameraDescription);
        ResolutionLabel->setObjectName(QStringLiteral("ResolutionLabel"));
        ResolutionLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(ResolutionLabel, 9, 0, 1, 1);

        CameraHealthLabel = new QLabel(CameraDescription);
        CameraHealthLabel->setObjectName(QStringLiteral("CameraHealthLabel"));
        CameraHealthLabel->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);

        gridLayout->addWidget(CameraHealthLabel, 13, 0, 1, 1);

        CameraHealth = new QLabel(CameraDescription);
        CameraHealth->setObjectName(QStringLiteral("CameraHealth"));

        gridLayout->addWidget(CameraHealth, 13, 1, 1, 1);

        FirmwareLabel = new QLabel(CameraDescription);
        FirmwareLabel->setObjectName(QStringLiteral("FirmwareLabel"));

        gridLayout->addWidget(FirmwareLabel, 8, 0, 1, 1, Qt::AlignRight|Qt::AlignTop);

        Firmware = new QLabel(CameraDescription);
        Firmware->setObjectName(QStringLiteral("Firmware"));

        gridLayout->addWidget(Firmware, 8, 1, 1, 1);


        verticalLayout_2->addLayout(gridLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        CameraDescriptionOkButton = new QPushButton(CameraDescription);
        CameraDescriptionOkButton->setObjectName(QStringLiteral("CameraDescriptionOkButton"));
        CameraDescriptionOkButton->setMinimumSize(QSize(16, 41));
        CameraDescriptionOkButton->setMaximumSize(QSize(130, 41));

        verticalLayout->addWidget(CameraDescriptionOkButton, 0, Qt::AlignHCenter);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(CameraDescription);
        QObject::connect(CameraDescriptionOkButton, SIGNAL(clicked()), CameraDescription, SLOT(accept()));

        QMetaObject::connectSlotsByName(CameraDescription);
    } // setupUi

    void retranslateUi(QDialog *CameraDescription)
    {
        CameraDescription->setWindowTitle(QApplication::translate("CameraDescription", "Camera Description", 0));
        SensorType->setText(QApplication::translate("CameraDescription", "Sensor", 0));
        SensorTypeLabel->setText(QApplication::translate("CameraDescription", "Sensor Type:", 0));
        SerialNumber->setText(QApplication::translate("CameraDescription", "Number", 0));
        CameraTypeLabel->setText(QApplication::translate("CameraDescription", "Camera Type:", 0));
        Temperature->setText(QApplication::translate("CameraDescription", "0\302\260", 0));
        TemperatureLabel->setText(QApplication::translate("CameraDescription", "Temperature:", 0));
        SerialNumberLabel->setText(QApplication::translate("CameraDescription", "Serial Number:", 0));
        CameraType->setText(QApplication::translate("CameraDescription", "Type", 0));
        PixelRate->setText(QApplication::translate("CameraDescription", "0", 0));
        PixelRateLabel->setText(QApplication::translate("CameraDescription", "Pixelrates:", 0));
        Resolution->setText(QApplication::translate("CameraDescription", "0 x 0", 0));
        ResolutionLabel->setText(QApplication::translate("CameraDescription", "Resolution:", 0));
        CameraHealthLabel->setText(QApplication::translate("CameraDescription", "Camera Health:", 0));
        CameraHealth->setText(QApplication::translate("CameraDescription", "TextLabel", 0));
        FirmwareLabel->setText(QApplication::translate("CameraDescription", "Firmware:", 0));
        Firmware->setText(QApplication::translate("CameraDescription", "Version", 0));
        CameraDescriptionOkButton->setText(QApplication::translate("CameraDescription", "Ok", 0));
    } // retranslateUi

};

namespace Ui {
    class CameraDescription: public Ui_CameraDescription {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERADESCRIPTION_H
