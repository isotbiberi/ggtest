/********************************************************************************
** Form generated from reading UI file 'camerasetup.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CAMERASETUP_H
#define UI_CAMERASETUP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CameraSetup
{
public:
    QVBoxLayout *verticalLayout;
    QTabWidget *tabWidget;
    QWidget *TimingTab;
    QVBoxLayout *verticalLayout_14;
    QGridLayout *gridLayout;
    QFrame *BottomRightFrame;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer_7;
    QGridLayout *gridLayout_2;
    QLabel *DelayLabel;
    QLabel *TimingLabel;
    QLabel *ExposureLabel;
    QSpacerItem *verticalSpacer_2;
    QLineEdit *ExposureLineEdit;
    QLineEdit *DelayLineEdit;
    QLabel *ExposureCurrentValueLabel;
    QLabel *ExposureDescriptionLabel;
    QLabel *DelayDescriptionLabel;
    QLabel *DelayCurrentValueLabel;
    QLabel *DelayCurrentValue;
    QLabel *ExposureCurrentValue;
    QFrame *TopFrame;
    QVBoxLayout *verticalLayout_6;
    QGridLayout *gridLayout_3;
    QLabel *label_3;
    QFrame *BottomLeftFrame;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout_2;
    QLabel *TriggerModeLabel;
    QRadioButton *AutoSequence;
    QRadioButton *SoftTrigger;
    QRadioButton *ExternalExpStart;
    QRadioButton *ExternalExpCtrl;
    QSpacerItem *verticalSpacer_3;
    QSpacerItem *horizontalSpacer_6;
    QSpacerItem *verticalSpacer_11;
    QWidget *SensorSizeTab;
    QVBoxLayout *verticalLayout_15;
    QGridLayout *gridLayout_4;
    QFrame *SensorFormat;
    QGridLayout *gridLayout_7;
    QSpacerItem *SensorFormatSpacer;
    QRadioButton *StandardButton;
    QRadioButton *AlternativeButton;
    QLabel *StandardFormat;
    QLabel *SensorFormatLabel;
    QLabel *AlternativeFormat;
    QFrame *Binning;
    QGridLayout *gridLayout_5;
    QLabel *BinningLabel;
    QLabel *HorizontalLabel;
    QComboBox *VerticalBox;
    QLabel *VerticalLabel;
    QComboBox *HorizontalBox;
    QSpacerItem *verticalSpacer;
    QFrame *ROIFrame;
    QGridLayout *gridLayout_6;
    QLabel *ROIy1label;
    QLabel *ROIx0label;
    QSpacerItem *ROISpacer;
    QLabel *ROIy0label;
    QLabel *ROIx1label;
    QLabel *ROILabel;
    QGraphicsView *ROIView;
    QSpinBox *ROIx1;
    QSpinBox *ROIx0;
    QSpinBox *ROIy1;
    QSpinBox *ROIy0;
    QPushButton *ROImaxbutton;
    QSpacerItem *horizontalSpacer;
    QSpacerItem *horizontalSpacer_2;
    QLabel *ActualResolution;
    QSpacerItem *SensorSizeSpacer;
    QWidget *SensorMiscTab;
    QVBoxLayout *verticalLayout_16;
    QGridLayout *gridLayout_8;
    QFrame *MiddleFrame;
    QVBoxLayout *verticalLayout_13;
    QFrame *PixelclockFrame;
    QGridLayout *gridLayout_9;
    QComboBox *PixelclockBox;
    QLabel *PixelclockLabel;
    QLabel *PixelclockLabel2;
    QFrame *ImagerateFrame;
    QVBoxLayout *verticalLayout_3;
    QLabel *ImagerateLabel;
    QLabel *Imagerate;
    QSpacerItem *ImagerateSpacer;
    QSpacerItem *verticalSpacer_5;
    QFrame *LeftFrame;
    QVBoxLayout *verticalLayout_9;
    QFrame *ADConverter;
    QGridLayout *gridLayout_11;
    QLabel *ADConverterLabel;
    QRadioButton *ADConverterOne;
    QRadioButton *ADConverterTwo;
    QFrame *DoubleImage;
    QGridLayout *gridLayout_12;
    QRadioButton *DoubleImageOff;
    QRadioButton *DoubleImageOn;
    QLabel *DoubleImageLabel;
    QFrame *IRSensitivity;
    QGridLayout *gridLayout_15;
    QLabel *IRSensitivityLabel;
    QRadioButton *IRSensitivityOff;
    QRadioButton *IRSensitivityOn;
    QFrame *OffsetControl;
    QGridLayout *gridLayout_13;
    QRadioButton *OffsetControlAuto;
    QRadioButton *OffsetControlOff;
    QLabel *OffsetControlLabel;
    QFrame *BWNoise;
    QVBoxLayout *verticalLayout_11;
    QLabel *BWNoiseFilterLabel;
    QComboBox *BWNoiseFilter;
    QSpacerItem *verticalSpacer_10;
    QFrame *RightFrame;
    QVBoxLayout *verticalLayout_10;
    QFrame *ConversionFactor;
    QGridLayout *gridLayout_14;
    QComboBox *ConversionFactorBox;
    QLabel *ConversionFactordesc;
    QLabel *ConversionFactorLabel;
    QFrame *reserved;
    QVBoxLayout *verticalLayout_12;
    QSpacerItem *verticalSpacer_9;
    QSpacerItem *SensorMiscSpacer;
    QWidget *RecordingTab;
    QGridLayout *gridLayout_10;
    QFrame *AcquireMode;
    QVBoxLayout *verticalLayout_5;
    QLabel *AcquireModeLabel;
    QRadioButton *AcquireModeAuto;
    QRadioButton *AcquireModeExternal;
    QRadioButton *AcquireModeExtTrigger;
    QSpacerItem *AcquireModeSpacer;
    QFrame *Timestamp;
    QVBoxLayout *verticalLayout_8;
    QLabel *TimestampLabel;
    QRadioButton *TimestampNostamp;
    QRadioButton *TimestampBinary;
    QRadioButton *TimestampBinaryASCII;
    QRadioButton *TimestampASCII;
    QSpacerItem *TimestampSpacer;
    QFrame *RecorderMode;
    QVBoxLayout *verticalLayout_7;
    QLabel *RecorderModeLabel;
    QRadioButton *RecorderModeSequence;
    QRadioButton *RecorderModeRingbuffer;
    QSpacerItem *RecorderModeSpacer;
    QSpacerItem *RecordingSpacer;
    QHBoxLayout *ButtonLayout;
    QSpacerItem *ButtonSpacer;
    QPushButton *CloseButton;
    QPushButton *ApplyButton;
    QFrame *StatusLayout;
    QHBoxLayout *horizontalLayout_4;
    QLabel *StatusText;

    void setupUi(QWidget *CameraSetup)
    {
        if (CameraSetup->objectName().isEmpty())
            CameraSetup->setObjectName(QStringLiteral("CameraSetup"));
        CameraSetup->resize(640, 468);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(CameraSetup->sizePolicy().hasHeightForWidth());
        CameraSetup->setSizePolicy(sizePolicy);
        CameraSetup->setMinimumSize(QSize(640, 468));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/PCOApp.ico"), QSize(), QIcon::Normal, QIcon::Off);
        CameraSetup->setWindowIcon(icon);
        verticalLayout = new QVBoxLayout(CameraSetup);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        tabWidget = new QTabWidget(CameraSetup);
        tabWidget->setObjectName(QStringLiteral("tabWidget"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(tabWidget->sizePolicy().hasHeightForWidth());
        tabWidget->setSizePolicy(sizePolicy1);
        TimingTab = new QWidget();
        TimingTab->setObjectName(QStringLiteral("TimingTab"));
        verticalLayout_14 = new QVBoxLayout(TimingTab);
        verticalLayout_14->setSpacing(6);
        verticalLayout_14->setContentsMargins(11, 11, 11, 11);
        verticalLayout_14->setObjectName(QStringLiteral("verticalLayout_14"));
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(6);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        BottomRightFrame = new QFrame(TimingTab);
        BottomRightFrame->setObjectName(QStringLiteral("BottomRightFrame"));
        BottomRightFrame->setFrameShape(QFrame::StyledPanel);
        BottomRightFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout_2 = new QHBoxLayout(BottomRightFrame);
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalSpacer_7 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer_7);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setSpacing(6);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        DelayLabel = new QLabel(BottomRightFrame);
        DelayLabel->setObjectName(QStringLiteral("DelayLabel"));

        gridLayout_2->addWidget(DelayLabel, 5, 1, 1, 1);

        TimingLabel = new QLabel(BottomRightFrame);
        TimingLabel->setObjectName(QStringLiteral("TimingLabel"));

        gridLayout_2->addWidget(TimingLabel, 0, 1, 1, 2);

        ExposureLabel = new QLabel(BottomRightFrame);
        ExposureLabel->setObjectName(QStringLiteral("ExposureLabel"));

        gridLayout_2->addWidget(ExposureLabel, 2, 1, 1, 1);

        verticalSpacer_2 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_2->addItem(verticalSpacer_2, 7, 1, 1, 2);

        ExposureLineEdit = new QLineEdit(BottomRightFrame);
        ExposureLineEdit->setObjectName(QStringLiteral("ExposureLineEdit"));
        ExposureLineEdit->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(ExposureLineEdit, 2, 2, 1, 1, Qt::AlignHCenter);

        DelayLineEdit = new QLineEdit(BottomRightFrame);
        DelayLineEdit->setObjectName(QStringLiteral("DelayLineEdit"));
        DelayLineEdit->setEnabled(true);
        DelayLineEdit->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(DelayLineEdit, 5, 2, 1, 1);

        ExposureCurrentValueLabel = new QLabel(BottomRightFrame);
        ExposureCurrentValueLabel->setObjectName(QStringLiteral("ExposureCurrentValueLabel"));

        gridLayout_2->addWidget(ExposureCurrentValueLabel, 1, 1, 1, 1);

        ExposureDescriptionLabel = new QLabel(BottomRightFrame);
        ExposureDescriptionLabel->setObjectName(QStringLiteral("ExposureDescriptionLabel"));

        gridLayout_2->addWidget(ExposureDescriptionLabel, 3, 2, 1, 1, Qt::AlignHCenter);

        DelayDescriptionLabel = new QLabel(BottomRightFrame);
        DelayDescriptionLabel->setObjectName(QStringLiteral("DelayDescriptionLabel"));

        gridLayout_2->addWidget(DelayDescriptionLabel, 6, 2, 1, 1, Qt::AlignHCenter);

        DelayCurrentValueLabel = new QLabel(BottomRightFrame);
        DelayCurrentValueLabel->setObjectName(QStringLiteral("DelayCurrentValueLabel"));

        gridLayout_2->addWidget(DelayCurrentValueLabel, 4, 1, 1, 1);

        DelayCurrentValue = new QLabel(BottomRightFrame);
        DelayCurrentValue->setObjectName(QStringLiteral("DelayCurrentValue"));

        gridLayout_2->addWidget(DelayCurrentValue, 4, 2, 1, 1, Qt::AlignHCenter);

        ExposureCurrentValue = new QLabel(BottomRightFrame);
        ExposureCurrentValue->setObjectName(QStringLiteral("ExposureCurrentValue"));

        gridLayout_2->addWidget(ExposureCurrentValue, 1, 2, 1, 1, Qt::AlignHCenter);


        horizontalLayout_2->addLayout(gridLayout_2);


        gridLayout->addWidget(BottomRightFrame, 1, 1, 1, 1);

        TopFrame = new QFrame(TimingTab);
        TopFrame->setObjectName(QStringLiteral("TopFrame"));
        TopFrame->setFrameShape(QFrame::StyledPanel);
        TopFrame->setFrameShadow(QFrame::Sunken);
        verticalLayout_6 = new QVBoxLayout(TopFrame);
        verticalLayout_6->setSpacing(6);
        verticalLayout_6->setContentsMargins(11, 11, 11, 11);
        verticalLayout_6->setObjectName(QStringLiteral("verticalLayout_6"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_3 = new QLabel(TopFrame);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout_3->addWidget(label_3, 0, 0, 1, 1, Qt::AlignHCenter);


        verticalLayout_6->addLayout(gridLayout_3);


        gridLayout->addWidget(TopFrame, 0, 0, 1, 2);

        BottomLeftFrame = new QFrame(TimingTab);
        BottomLeftFrame->setObjectName(QStringLiteral("BottomLeftFrame"));
        BottomLeftFrame->setFrameShape(QFrame::StyledPanel);
        BottomLeftFrame->setFrameShadow(QFrame::Sunken);
        horizontalLayout = new QHBoxLayout(BottomLeftFrame);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        TriggerModeLabel = new QLabel(BottomLeftFrame);
        TriggerModeLabel->setObjectName(QStringLiteral("TriggerModeLabel"));

        verticalLayout_2->addWidget(TriggerModeLabel);

        AutoSequence = new QRadioButton(BottomLeftFrame);
        AutoSequence->setObjectName(QStringLiteral("AutoSequence"));

        verticalLayout_2->addWidget(AutoSequence);

        SoftTrigger = new QRadioButton(BottomLeftFrame);
        SoftTrigger->setObjectName(QStringLiteral("SoftTrigger"));

        verticalLayout_2->addWidget(SoftTrigger);

        ExternalExpStart = new QRadioButton(BottomLeftFrame);
        ExternalExpStart->setObjectName(QStringLiteral("ExternalExpStart"));

        verticalLayout_2->addWidget(ExternalExpStart);

        ExternalExpCtrl = new QRadioButton(BottomLeftFrame);
        ExternalExpCtrl->setObjectName(QStringLiteral("ExternalExpCtrl"));

        verticalLayout_2->addWidget(ExternalExpCtrl);

        verticalSpacer_3 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_2->addItem(verticalSpacer_3);


        horizontalLayout->addLayout(verticalLayout_2);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_6);


        gridLayout->addWidget(BottomLeftFrame, 1, 0, 1, 1);


        verticalLayout_14->addLayout(gridLayout);

        verticalSpacer_11 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_14->addItem(verticalSpacer_11);

        tabWidget->addTab(TimingTab, QString());
        SensorSizeTab = new QWidget();
        SensorSizeTab->setObjectName(QStringLiteral("SensorSizeTab"));
        verticalLayout_15 = new QVBoxLayout(SensorSizeTab);
        verticalLayout_15->setSpacing(6);
        verticalLayout_15->setContentsMargins(11, 11, 11, 11);
        verticalLayout_15->setObjectName(QStringLiteral("verticalLayout_15"));
        gridLayout_4 = new QGridLayout();
        gridLayout_4->setSpacing(6);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        SensorFormat = new QFrame(SensorSizeTab);
        SensorFormat->setObjectName(QStringLiteral("SensorFormat"));
        SensorFormat->setFrameShape(QFrame::StyledPanel);
        SensorFormat->setFrameShadow(QFrame::Sunken);
        gridLayout_7 = new QGridLayout(SensorFormat);
        gridLayout_7->setSpacing(6);
        gridLayout_7->setContentsMargins(11, 11, 11, 11);
        gridLayout_7->setObjectName(QStringLiteral("gridLayout_7"));
        SensorFormatSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_7->addItem(SensorFormatSpacer, 4, 0, 1, 1);

        StandardButton = new QRadioButton(SensorFormat);
        StandardButton->setObjectName(QStringLiteral("StandardButton"));

        gridLayout_7->addWidget(StandardButton, 2, 0, 1, 1);

        AlternativeButton = new QRadioButton(SensorFormat);
        AlternativeButton->setObjectName(QStringLiteral("AlternativeButton"));

        gridLayout_7->addWidget(AlternativeButton, 3, 0, 1, 1);

        StandardFormat = new QLabel(SensorFormat);
        StandardFormat->setObjectName(QStringLiteral("StandardFormat"));

        gridLayout_7->addWidget(StandardFormat, 2, 1, 1, 1);

        SensorFormatLabel = new QLabel(SensorFormat);
        SensorFormatLabel->setObjectName(QStringLiteral("SensorFormatLabel"));

        gridLayout_7->addWidget(SensorFormatLabel, 1, 0, 1, 1);

        AlternativeFormat = new QLabel(SensorFormat);
        AlternativeFormat->setObjectName(QStringLiteral("AlternativeFormat"));

        gridLayout_7->addWidget(AlternativeFormat, 3, 1, 1, 1);


        gridLayout_4->addWidget(SensorFormat, 1, 0, 1, 1);

        Binning = new QFrame(SensorSizeTab);
        Binning->setObjectName(QStringLiteral("Binning"));
        Binning->setFrameShape(QFrame::StyledPanel);
        Binning->setFrameShadow(QFrame::Sunken);
        gridLayout_5 = new QGridLayout(Binning);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        BinningLabel = new QLabel(Binning);
        BinningLabel->setObjectName(QStringLiteral("BinningLabel"));

        gridLayout_5->addWidget(BinningLabel, 0, 0, 1, 1);

        HorizontalLabel = new QLabel(Binning);
        HorizontalLabel->setObjectName(QStringLiteral("HorizontalLabel"));

        gridLayout_5->addWidget(HorizontalLabel, 1, 0, 1, 1);

        VerticalBox = new QComboBox(Binning);
        VerticalBox->setObjectName(QStringLiteral("VerticalBox"));

        gridLayout_5->addWidget(VerticalBox, 2, 1, 1, 1);

        VerticalLabel = new QLabel(Binning);
        VerticalLabel->setObjectName(QStringLiteral("VerticalLabel"));

        gridLayout_5->addWidget(VerticalLabel, 2, 0, 1, 1);

        HorizontalBox = new QComboBox(Binning);
        HorizontalBox->setObjectName(QStringLiteral("HorizontalBox"));

        gridLayout_5->addWidget(HorizontalBox, 1, 1, 1, 1);

        verticalSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_5->addItem(verticalSpacer, 3, 0, 1, 2);


        gridLayout_4->addWidget(Binning, 0, 0, 1, 1);

        ROIFrame = new QFrame(SensorSizeTab);
        ROIFrame->setObjectName(QStringLiteral("ROIFrame"));
        ROIFrame->setFrameShape(QFrame::StyledPanel);
        ROIFrame->setFrameShadow(QFrame::Sunken);
        gridLayout_6 = new QGridLayout(ROIFrame);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        ROIy1label = new QLabel(ROIFrame);
        ROIy1label->setObjectName(QStringLiteral("ROIy1label"));
        QSizePolicy sizePolicy2(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(ROIy1label->sizePolicy().hasHeightForWidth());
        ROIy1label->setSizePolicy(sizePolicy2);

        gridLayout_6->addWidget(ROIy1label, 5, 0, 1, 1, Qt::AlignBottom);

        ROIx0label = new QLabel(ROIFrame);
        ROIx0label->setObjectName(QStringLiteral("ROIx0label"));

        gridLayout_6->addWidget(ROIx0label, 8, 3, 1, 1, Qt::AlignLeft|Qt::AlignTop);

        ROISpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_6->addItem(ROISpacer, 7, 5, 1, 1);

        ROIy0label = new QLabel(ROIFrame);
        ROIy0label->setObjectName(QStringLiteral("ROIy0label"));
        sizePolicy2.setHeightForWidth(ROIy0label->sizePolicy().hasHeightForWidth());
        ROIy0label->setSizePolicy(sizePolicy2);

        gridLayout_6->addWidget(ROIy0label, 1, 0, 1, 1);

        ROIx1label = new QLabel(ROIFrame);
        ROIx1label->setObjectName(QStringLiteral("ROIx1label"));

        gridLayout_6->addWidget(ROIx1label, 8, 7, 1, 1, Qt::AlignRight|Qt::AlignTop);

        ROILabel = new QLabel(ROIFrame);
        ROILabel->setObjectName(QStringLiteral("ROILabel"));
        QSizePolicy sizePolicy3(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(ROILabel->sizePolicy().hasHeightForWidth());
        ROILabel->setSizePolicy(sizePolicy3);

        gridLayout_6->addWidget(ROILabel, 0, 0, 1, 2, Qt::AlignLeft);

        ROIView = new QGraphicsView(ROIFrame);
        ROIView->setObjectName(QStringLiteral("ROIView"));
        ROIView->setMinimumSize(QSize(272, 192));
        ROIView->setContextMenuPolicy(Qt::NoContextMenu);
        ROIView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ROIView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ROIView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
        ROIView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);

        gridLayout_6->addWidget(ROIView, 2, 2, 5, 6);

        ROIx1 = new QSpinBox(ROIFrame);
        ROIx1->setObjectName(QStringLiteral("ROIx1"));
        ROIx1->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        ROIx1->setMinimum(1);
        ROIx1->setMaximum(9999);

        gridLayout_6->addWidget(ROIx1, 7, 7, 1, 1);

        ROIx0 = new QSpinBox(ROIFrame);
        ROIx0->setObjectName(QStringLiteral("ROIx0"));
        ROIx0->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        ROIx0->setButtonSymbols(QAbstractSpinBox::UpDownArrows);
        ROIx0->setMinimum(1);
        ROIx0->setMaximum(9999);
        ROIx0->setValue(1);

        gridLayout_6->addWidget(ROIx0, 7, 3, 1, 1);

        ROIy1 = new QSpinBox(ROIFrame);
        ROIy1->setObjectName(QStringLiteral("ROIy1"));
        ROIy1->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        ROIy1->setMinimum(1);
        ROIy1->setMaximum(9999);
        ROIy1->setSingleStep(1);

        gridLayout_6->addWidget(ROIy1, 6, 0, 1, 1);

        ROIy0 = new QSpinBox(ROIFrame);
        ROIy0->setObjectName(QStringLiteral("ROIy0"));
        ROIy0->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        ROIy0->setMinimum(1);
        ROIy0->setMaximum(9999);

        gridLayout_6->addWidget(ROIy0, 2, 0, 1, 1);

        ROImaxbutton = new QPushButton(ROIFrame);
        ROImaxbutton->setObjectName(QStringLiteral("ROImaxbutton"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(ROImaxbutton->sizePolicy().hasHeightForWidth());
        ROImaxbutton->setSizePolicy(sizePolicy4);
        ROImaxbutton->setMinimumSize(QSize(80, 22));
        ROImaxbutton->setFocusPolicy(Qt::ClickFocus);

        gridLayout_6->addWidget(ROImaxbutton, 8, 5, 1, 1);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_6->addItem(horizontalSpacer, 8, 4, 1, 1);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout_6->addItem(horizontalSpacer_2, 8, 6, 1, 1);

        ActualResolution = new QLabel(ROIFrame);
        ActualResolution->setObjectName(QStringLiteral("ActualResolution"));
        sizePolicy3.setHeightForWidth(ActualResolution->sizePolicy().hasHeightForWidth());
        ActualResolution->setSizePolicy(sizePolicy3);
        ActualResolution->setAlignment(Qt::AlignCenter);

        gridLayout_6->addWidget(ActualResolution, 1, 3, 1, 5);


        gridLayout_4->addWidget(ROIFrame, 0, 1, 2, 1);


        verticalLayout_15->addLayout(gridLayout_4);

        SensorSizeSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_15->addItem(SensorSizeSpacer);

        tabWidget->addTab(SensorSizeTab, QString());
        SensorMiscTab = new QWidget();
        SensorMiscTab->setObjectName(QStringLiteral("SensorMiscTab"));
        verticalLayout_16 = new QVBoxLayout(SensorMiscTab);
        verticalLayout_16->setSpacing(6);
        verticalLayout_16->setContentsMargins(11, 11, 11, 11);
        verticalLayout_16->setObjectName(QStringLiteral("verticalLayout_16"));
        gridLayout_8 = new QGridLayout();
        gridLayout_8->setSpacing(6);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        MiddleFrame = new QFrame(SensorMiscTab);
        MiddleFrame->setObjectName(QStringLiteral("MiddleFrame"));
        MiddleFrame->setFrameShape(QFrame::StyledPanel);
        MiddleFrame->setFrameShadow(QFrame::Sunken);
        verticalLayout_13 = new QVBoxLayout(MiddleFrame);
        verticalLayout_13->setSpacing(6);
        verticalLayout_13->setContentsMargins(11, 11, 11, 11);
        verticalLayout_13->setObjectName(QStringLiteral("verticalLayout_13"));
        PixelclockFrame = new QFrame(MiddleFrame);
        PixelclockFrame->setObjectName(QStringLiteral("PixelclockFrame"));
        PixelclockFrame->setCursor(QCursor(Qt::ArrowCursor));
        PixelclockFrame->setFrameShape(QFrame::StyledPanel);
        PixelclockFrame->setFrameShadow(QFrame::Sunken);
        gridLayout_9 = new QGridLayout(PixelclockFrame);
        gridLayout_9->setSpacing(6);
        gridLayout_9->setContentsMargins(11, 11, 11, 11);
        gridLayout_9->setObjectName(QStringLiteral("gridLayout_9"));
        PixelclockBox = new QComboBox(PixelclockFrame);
        PixelclockBox->setObjectName(QStringLiteral("PixelclockBox"));

        gridLayout_9->addWidget(PixelclockBox, 1, 0, 1, 1);

        PixelclockLabel = new QLabel(PixelclockFrame);
        PixelclockLabel->setObjectName(QStringLiteral("PixelclockLabel"));

        gridLayout_9->addWidget(PixelclockLabel, 0, 0, 1, 1);

        PixelclockLabel2 = new QLabel(PixelclockFrame);
        PixelclockLabel2->setObjectName(QStringLiteral("PixelclockLabel2"));

        gridLayout_9->addWidget(PixelclockLabel2, 1, 1, 1, 1);


        verticalLayout_13->addWidget(PixelclockFrame);

        ImagerateFrame = new QFrame(MiddleFrame);
        ImagerateFrame->setObjectName(QStringLiteral("ImagerateFrame"));
        ImagerateFrame->setFrameShape(QFrame::StyledPanel);
        ImagerateFrame->setFrameShadow(QFrame::Sunken);
        verticalLayout_3 = new QVBoxLayout(ImagerateFrame);
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setContentsMargins(11, 11, 11, 11);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        ImagerateLabel = new QLabel(ImagerateFrame);
        ImagerateLabel->setObjectName(QStringLiteral("ImagerateLabel"));

        verticalLayout_3->addWidget(ImagerateLabel);

        Imagerate = new QLabel(ImagerateFrame);
        Imagerate->setObjectName(QStringLiteral("Imagerate"));

        verticalLayout_3->addWidget(Imagerate);

        ImagerateSpacer = new QSpacerItem(20, 68, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_3->addItem(ImagerateSpacer);


        verticalLayout_13->addWidget(ImagerateFrame);

        verticalSpacer_5 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_13->addItem(verticalSpacer_5);


        gridLayout_8->addWidget(MiddleFrame, 0, 1, 1, 1);

        LeftFrame = new QFrame(SensorMiscTab);
        LeftFrame->setObjectName(QStringLiteral("LeftFrame"));
        sizePolicy3.setHeightForWidth(LeftFrame->sizePolicy().hasHeightForWidth());
        LeftFrame->setSizePolicy(sizePolicy3);
        LeftFrame->setFrameShape(QFrame::StyledPanel);
        LeftFrame->setFrameShadow(QFrame::Sunken);
        verticalLayout_9 = new QVBoxLayout(LeftFrame);
        verticalLayout_9->setSpacing(6);
        verticalLayout_9->setContentsMargins(11, 11, 11, 11);
        verticalLayout_9->setObjectName(QStringLiteral("verticalLayout_9"));
        ADConverter = new QFrame(LeftFrame);
        ADConverter->setObjectName(QStringLiteral("ADConverter"));
        ADConverter->setFrameShape(QFrame::StyledPanel);
        ADConverter->setFrameShadow(QFrame::Sunken);
        gridLayout_11 = new QGridLayout(ADConverter);
        gridLayout_11->setSpacing(6);
        gridLayout_11->setContentsMargins(11, 11, 11, 11);
        gridLayout_11->setObjectName(QStringLiteral("gridLayout_11"));
        ADConverterLabel = new QLabel(ADConverter);
        ADConverterLabel->setObjectName(QStringLiteral("ADConverterLabel"));
        ADConverterLabel->setFrameShadow(QFrame::Sunken);

        gridLayout_11->addWidget(ADConverterLabel, 0, 0, 1, 2);

        ADConverterOne = new QRadioButton(ADConverter);
        ADConverterOne->setObjectName(QStringLiteral("ADConverterOne"));

        gridLayout_11->addWidget(ADConverterOne, 1, 0, 1, 1);

        ADConverterTwo = new QRadioButton(ADConverter);
        ADConverterTwo->setObjectName(QStringLiteral("ADConverterTwo"));

        gridLayout_11->addWidget(ADConverterTwo, 1, 1, 1, 1);


        verticalLayout_9->addWidget(ADConverter);

        DoubleImage = new QFrame(LeftFrame);
        DoubleImage->setObjectName(QStringLiteral("DoubleImage"));
        DoubleImage->setFrameShape(QFrame::StyledPanel);
        DoubleImage->setFrameShadow(QFrame::Sunken);
        gridLayout_12 = new QGridLayout(DoubleImage);
        gridLayout_12->setSpacing(6);
        gridLayout_12->setContentsMargins(11, 11, 11, 11);
        gridLayout_12->setObjectName(QStringLiteral("gridLayout_12"));
        DoubleImageOff = new QRadioButton(DoubleImage);
        DoubleImageOff->setObjectName(QStringLiteral("DoubleImageOff"));

        gridLayout_12->addWidget(DoubleImageOff, 1, 0, 1, 1);

        DoubleImageOn = new QRadioButton(DoubleImage);
        DoubleImageOn->setObjectName(QStringLiteral("DoubleImageOn"));

        gridLayout_12->addWidget(DoubleImageOn, 1, 1, 1, 1);

        DoubleImageLabel = new QLabel(DoubleImage);
        DoubleImageLabel->setObjectName(QStringLiteral("DoubleImageLabel"));

        gridLayout_12->addWidget(DoubleImageLabel, 0, 0, 1, 2);


        verticalLayout_9->addWidget(DoubleImage);

        IRSensitivity = new QFrame(LeftFrame);
        IRSensitivity->setObjectName(QStringLiteral("IRSensitivity"));
        IRSensitivity->setFrameShape(QFrame::StyledPanel);
        IRSensitivity->setFrameShadow(QFrame::Sunken);
        gridLayout_15 = new QGridLayout(IRSensitivity);
        gridLayout_15->setSpacing(6);
        gridLayout_15->setContentsMargins(11, 11, 11, 11);
        gridLayout_15->setObjectName(QStringLiteral("gridLayout_15"));
        IRSensitivityLabel = new QLabel(IRSensitivity);
        IRSensitivityLabel->setObjectName(QStringLiteral("IRSensitivityLabel"));

        gridLayout_15->addWidget(IRSensitivityLabel, 0, 0, 1, 2);

        IRSensitivityOff = new QRadioButton(IRSensitivity);
        IRSensitivityOff->setObjectName(QStringLiteral("IRSensitivityOff"));

        gridLayout_15->addWidget(IRSensitivityOff, 1, 0, 1, 1);

        IRSensitivityOn = new QRadioButton(IRSensitivity);
        IRSensitivityOn->setObjectName(QStringLiteral("IRSensitivityOn"));

        gridLayout_15->addWidget(IRSensitivityOn, 1, 1, 1, 1);


        verticalLayout_9->addWidget(IRSensitivity);

        OffsetControl = new QFrame(LeftFrame);
        OffsetControl->setObjectName(QStringLiteral("OffsetControl"));
        OffsetControl->setFrameShape(QFrame::StyledPanel);
        OffsetControl->setFrameShadow(QFrame::Sunken);
        gridLayout_13 = new QGridLayout(OffsetControl);
        gridLayout_13->setSpacing(6);
        gridLayout_13->setContentsMargins(11, 11, 11, 11);
        gridLayout_13->setObjectName(QStringLiteral("gridLayout_13"));
        OffsetControlAuto = new QRadioButton(OffsetControl);
        OffsetControlAuto->setObjectName(QStringLiteral("OffsetControlAuto"));

        gridLayout_13->addWidget(OffsetControlAuto, 1, 0, 1, 1);

        OffsetControlOff = new QRadioButton(OffsetControl);
        OffsetControlOff->setObjectName(QStringLiteral("OffsetControlOff"));

        gridLayout_13->addWidget(OffsetControlOff, 1, 1, 1, 1);

        OffsetControlLabel = new QLabel(OffsetControl);
        OffsetControlLabel->setObjectName(QStringLiteral("OffsetControlLabel"));

        gridLayout_13->addWidget(OffsetControlLabel, 0, 0, 1, 2);


        verticalLayout_9->addWidget(OffsetControl);

        BWNoise = new QFrame(LeftFrame);
        BWNoise->setObjectName(QStringLiteral("BWNoise"));
        BWNoise->setFrameShape(QFrame::StyledPanel);
        BWNoise->setFrameShadow(QFrame::Sunken);
        verticalLayout_11 = new QVBoxLayout(BWNoise);
        verticalLayout_11->setSpacing(6);
        verticalLayout_11->setContentsMargins(11, 11, 11, 11);
        verticalLayout_11->setObjectName(QStringLiteral("verticalLayout_11"));
        BWNoiseFilterLabel = new QLabel(BWNoise);
        BWNoiseFilterLabel->setObjectName(QStringLiteral("BWNoiseFilterLabel"));

        verticalLayout_11->addWidget(BWNoiseFilterLabel);

        BWNoiseFilter = new QComboBox(BWNoise);
        BWNoiseFilter->setObjectName(QStringLiteral("BWNoiseFilter"));

        verticalLayout_11->addWidget(BWNoiseFilter);


        verticalLayout_9->addWidget(BWNoise);

        verticalSpacer_10 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_9->addItem(verticalSpacer_10);


        gridLayout_8->addWidget(LeftFrame, 0, 0, 1, 1);

        RightFrame = new QFrame(SensorMiscTab);
        RightFrame->setObjectName(QStringLiteral("RightFrame"));
        RightFrame->setFrameShape(QFrame::StyledPanel);
        RightFrame->setFrameShadow(QFrame::Sunken);
        verticalLayout_10 = new QVBoxLayout(RightFrame);
        verticalLayout_10->setSpacing(6);
        verticalLayout_10->setContentsMargins(11, 11, 11, 11);
        verticalLayout_10->setObjectName(QStringLiteral("verticalLayout_10"));
        ConversionFactor = new QFrame(RightFrame);
        ConversionFactor->setObjectName(QStringLiteral("ConversionFactor"));
        ConversionFactor->setFrameShape(QFrame::StyledPanel);
        ConversionFactor->setFrameShadow(QFrame::Sunken);
        gridLayout_14 = new QGridLayout(ConversionFactor);
        gridLayout_14->setSpacing(6);
        gridLayout_14->setContentsMargins(11, 11, 11, 11);
        gridLayout_14->setObjectName(QStringLiteral("gridLayout_14"));
        ConversionFactorBox = new QComboBox(ConversionFactor);
        ConversionFactorBox->setObjectName(QStringLiteral("ConversionFactorBox"));

        gridLayout_14->addWidget(ConversionFactorBox, 1, 0, 1, 1);

        ConversionFactordesc = new QLabel(ConversionFactor);
        ConversionFactordesc->setObjectName(QStringLiteral("ConversionFactordesc"));

        gridLayout_14->addWidget(ConversionFactordesc, 1, 1, 1, 1);

        ConversionFactorLabel = new QLabel(ConversionFactor);
        ConversionFactorLabel->setObjectName(QStringLiteral("ConversionFactorLabel"));

        gridLayout_14->addWidget(ConversionFactorLabel, 0, 0, 1, 2);


        verticalLayout_10->addWidget(ConversionFactor);

        reserved = new QFrame(RightFrame);
        reserved->setObjectName(QStringLiteral("reserved"));
        reserved->setFrameShape(QFrame::StyledPanel);
        reserved->setFrameShadow(QFrame::Sunken);
        verticalLayout_12 = new QVBoxLayout(reserved);
        verticalLayout_12->setSpacing(6);
        verticalLayout_12->setContentsMargins(11, 11, 11, 11);
        verticalLayout_12->setObjectName(QStringLiteral("verticalLayout_12"));
        verticalSpacer_9 = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_12->addItem(verticalSpacer_9);


        verticalLayout_10->addWidget(reserved);


        gridLayout_8->addWidget(RightFrame, 0, 2, 1, 1);


        verticalLayout_16->addLayout(gridLayout_8);

        SensorMiscSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_16->addItem(SensorMiscSpacer);

        tabWidget->addTab(SensorMiscTab, QString());
        RecordingTab = new QWidget();
        RecordingTab->setObjectName(QStringLiteral("RecordingTab"));
        gridLayout_10 = new QGridLayout(RecordingTab);
        gridLayout_10->setSpacing(6);
        gridLayout_10->setContentsMargins(11, 11, 11, 11);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        AcquireMode = new QFrame(RecordingTab);
        AcquireMode->setObjectName(QStringLiteral("AcquireMode"));
        AcquireMode->setFrameShape(QFrame::StyledPanel);
        AcquireMode->setFrameShadow(QFrame::Sunken);
        verticalLayout_5 = new QVBoxLayout(AcquireMode);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        AcquireModeLabel = new QLabel(AcquireMode);
        AcquireModeLabel->setObjectName(QStringLiteral("AcquireModeLabel"));

        verticalLayout_5->addWidget(AcquireModeLabel);

        AcquireModeAuto = new QRadioButton(AcquireMode);
        AcquireModeAuto->setObjectName(QStringLiteral("AcquireModeAuto"));

        verticalLayout_5->addWidget(AcquireModeAuto);

        AcquireModeExternal = new QRadioButton(AcquireMode);
        AcquireModeExternal->setObjectName(QStringLiteral("AcquireModeExternal"));

        verticalLayout_5->addWidget(AcquireModeExternal);

        AcquireModeExtTrigger = new QRadioButton(AcquireMode);
        AcquireModeExtTrigger->setObjectName(QStringLiteral("AcquireModeExtTrigger"));

        verticalLayout_5->addWidget(AcquireModeExtTrigger);

        AcquireModeSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_5->addItem(AcquireModeSpacer);


        gridLayout_10->addWidget(AcquireMode, 1, 0, 1, 1);

        Timestamp = new QFrame(RecordingTab);
        Timestamp->setObjectName(QStringLiteral("Timestamp"));
        Timestamp->setFrameShape(QFrame::StyledPanel);
        Timestamp->setFrameShadow(QFrame::Sunken);
        verticalLayout_8 = new QVBoxLayout(Timestamp);
        verticalLayout_8->setSpacing(6);
        verticalLayout_8->setContentsMargins(11, 11, 11, 11);
        verticalLayout_8->setObjectName(QStringLiteral("verticalLayout_8"));
        TimestampLabel = new QLabel(Timestamp);
        TimestampLabel->setObjectName(QStringLiteral("TimestampLabel"));

        verticalLayout_8->addWidget(TimestampLabel);

        TimestampNostamp = new QRadioButton(Timestamp);
        TimestampNostamp->setObjectName(QStringLiteral("TimestampNostamp"));

        verticalLayout_8->addWidget(TimestampNostamp);

        TimestampBinary = new QRadioButton(Timestamp);
        TimestampBinary->setObjectName(QStringLiteral("TimestampBinary"));

        verticalLayout_8->addWidget(TimestampBinary);

        TimestampBinaryASCII = new QRadioButton(Timestamp);
        TimestampBinaryASCII->setObjectName(QStringLiteral("TimestampBinaryASCII"));

        verticalLayout_8->addWidget(TimestampBinaryASCII);

        TimestampASCII = new QRadioButton(Timestamp);
        TimestampASCII->setObjectName(QStringLiteral("TimestampASCII"));

        verticalLayout_8->addWidget(TimestampASCII);

        TimestampSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_8->addItem(TimestampSpacer);


        gridLayout_10->addWidget(Timestamp, 0, 1, 1, 1);

        RecorderMode = new QFrame(RecordingTab);
        RecorderMode->setObjectName(QStringLiteral("RecorderMode"));
        RecorderMode->setFrameShape(QFrame::StyledPanel);
        RecorderMode->setFrameShadow(QFrame::Sunken);
        verticalLayout_7 = new QVBoxLayout(RecorderMode);
        verticalLayout_7->setSpacing(6);
        verticalLayout_7->setContentsMargins(11, 11, 11, 11);
        verticalLayout_7->setObjectName(QStringLiteral("verticalLayout_7"));
        RecorderModeLabel = new QLabel(RecorderMode);
        RecorderModeLabel->setObjectName(QStringLiteral("RecorderModeLabel"));

        verticalLayout_7->addWidget(RecorderModeLabel);

        RecorderModeSequence = new QRadioButton(RecorderMode);
        RecorderModeSequence->setObjectName(QStringLiteral("RecorderModeSequence"));

        verticalLayout_7->addWidget(RecorderModeSequence);

        RecorderModeRingbuffer = new QRadioButton(RecorderMode);
        RecorderModeRingbuffer->setObjectName(QStringLiteral("RecorderModeRingbuffer"));

        verticalLayout_7->addWidget(RecorderModeRingbuffer);

        RecorderModeSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_7->addItem(RecorderModeSpacer);


        gridLayout_10->addWidget(RecorderMode, 0, 0, 1, 1);

        RecordingSpacer = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding);

        gridLayout_10->addItem(RecordingSpacer, 2, 0, 1, 2);

        tabWidget->addTab(RecordingTab, QString());

        verticalLayout->addWidget(tabWidget);

        ButtonLayout = new QHBoxLayout();
        ButtonLayout->setSpacing(6);
        ButtonLayout->setObjectName(QStringLiteral("ButtonLayout"));
        ButtonSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        ButtonLayout->addItem(ButtonSpacer);

        CloseButton = new QPushButton(CameraSetup);
        CloseButton->setObjectName(QStringLiteral("CloseButton"));

        ButtonLayout->addWidget(CloseButton);

        ApplyButton = new QPushButton(CameraSetup);
        ApplyButton->setObjectName(QStringLiteral("ApplyButton"));

        ButtonLayout->addWidget(ApplyButton, 0, Qt::AlignRight);

        StatusLayout = new QFrame(CameraSetup);
        StatusLayout->setObjectName(QStringLiteral("StatusLayout"));
        StatusLayout->setMinimumSize(QSize(100, 0));
        StatusLayout->setMaximumSize(QSize(100, 16777215));
        StatusLayout->setFrameShape(QFrame::StyledPanel);
        StatusLayout->setFrameShadow(QFrame::Sunken);
        horizontalLayout_4 = new QHBoxLayout(StatusLayout);
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setContentsMargins(11, 11, 11, 11);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        StatusText = new QLabel(StatusLayout);
        StatusText->setObjectName(QStringLiteral("StatusText"));

        horizontalLayout_4->addWidget(StatusText);


        ButtonLayout->addWidget(StatusLayout);


        verticalLayout->addLayout(ButtonLayout);


        retranslateUi(CameraSetup);
        QObject::connect(CloseButton, SIGNAL(clicked()), CameraSetup, SLOT(close()));

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(CameraSetup);
    } // setupUi

    void retranslateUi(QWidget *CameraSetup)
    {
        CameraSetup->setWindowTitle(QApplication::translate("CameraSetup", "CameraSetup", 0));
#ifndef QT_NO_ACCESSIBILITY
        TimingTab->setAccessibleName(QString());
#endif // QT_NO_ACCESSIBILITY
        DelayLabel->setText(QApplication::translate("CameraSetup", "Delay", 0));
        TimingLabel->setText(QApplication::translate("CameraSetup", "Timing", 0));
        ExposureLabel->setText(QApplication::translate("CameraSetup", "Exposure", 0));
        ExposureLineEdit->setInputMask(QApplication::translate("CameraSetup", "00:00:000:000", 0));
        ExposureLineEdit->setText(QApplication::translate("CameraSetup", "00:00:000:000", 0));
        DelayLineEdit->setInputMask(QApplication::translate("CameraSetup", "00:00:000:000", 0));
        DelayLineEdit->setText(QApplication::translate("CameraSetup", "00:00:000:000", 0));
        ExposureCurrentValueLabel->setText(QApplication::translate("CameraSetup", "Current Exposure", 0));
        ExposureDescriptionLabel->setText(QApplication::translate("CameraSetup", "min:s:ms:\302\265s", 0));
        DelayDescriptionLabel->setText(QApplication::translate("CameraSetup", "min:s:ms:\302\265s", 0));
        DelayCurrentValueLabel->setText(QApplication::translate("CameraSetup", "Current Delay", 0));
        DelayCurrentValue->setText(QApplication::translate("CameraSetup", "00:00:000:000", 0));
        ExposureCurrentValue->setText(QApplication::translate("CameraSetup", "00:00:000:000", 0));
        label_3->setText(QApplication::translate("CameraSetup", "Placeholder", 0));
        TriggerModeLabel->setText(QApplication::translate("CameraSetup", "Trigger Mode", 0));
        AutoSequence->setText(QApplication::translate("CameraSetup", "Auto Sequence", 0));
        SoftTrigger->setText(QApplication::translate("CameraSetup", "Soft Trigger", 0));
        ExternalExpStart->setText(QApplication::translate("CameraSetup", "External Exp. Start", 0));
        ExternalExpCtrl->setText(QApplication::translate("CameraSetup", "External Exp. Ctrl", 0));
        tabWidget->setTabText(tabWidget->indexOf(TimingTab), QApplication::translate("CameraSetup", "Timing", 0));
        StandardButton->setText(QApplication::translate("CameraSetup", "standard", 0));
        AlternativeButton->setText(QApplication::translate("CameraSetup", "alternative", 0));
        StandardFormat->setText(QApplication::translate("CameraSetup", "0 x 0", 0));
        SensorFormatLabel->setText(QApplication::translate("CameraSetup", "Sensor Format", 0));
        AlternativeFormat->setText(QApplication::translate("CameraSetup", "0 x 0", 0));
        BinningLabel->setText(QApplication::translate("CameraSetup", "Binning", 0));
        HorizontalLabel->setText(QApplication::translate("CameraSetup", "horizontal", 0));
        VerticalLabel->setText(QApplication::translate("CameraSetup", "vertical", 0));
        ROIy1label->setText(QApplication::translate("CameraSetup", "y1", 0));
        ROIx0label->setText(QApplication::translate("CameraSetup", "x0", 0));
        ROIy0label->setText(QApplication::translate("CameraSetup", "y0", 0));
        ROIx1label->setText(QApplication::translate("CameraSetup", "x1", 0));
        ROILabel->setText(QApplication::translate("CameraSetup", "ROI", 0));
        ROImaxbutton->setText(QApplication::translate("CameraSetup", "Set to max", 0));
        ActualResolution->setText(QApplication::translate("CameraSetup", "0 x 0", 0));
        tabWidget->setTabText(tabWidget->indexOf(SensorSizeTab), QApplication::translate("CameraSetup", "Sensor (size)", 0));
        PixelclockLabel->setText(QApplication::translate("CameraSetup", "Pixelclock", 0));
        PixelclockLabel2->setText(QApplication::translate("CameraSetup", "x1.0e+6 Pixel/s", 0));
        ImagerateLabel->setText(QApplication::translate("CameraSetup", "Imagerate:", 0));
        Imagerate->setText(QString());
        ADConverterLabel->setText(QApplication::translate("CameraSetup", "AD Converter", 0));
        ADConverterOne->setText(QApplication::translate("CameraSetup", "one", 0));
        ADConverterTwo->setText(QApplication::translate("CameraSetup", "two", 0));
        DoubleImageOff->setText(QApplication::translate("CameraSetup", "off", 0));
        DoubleImageOn->setText(QApplication::translate("CameraSetup", "on", 0));
        DoubleImageLabel->setText(QApplication::translate("CameraSetup", "Double Image", 0));
        IRSensitivityLabel->setText(QApplication::translate("CameraSetup", "IR Sensitivity", 0));
        IRSensitivityOff->setText(QApplication::translate("CameraSetup", "off", 0));
        IRSensitivityOn->setText(QApplication::translate("CameraSetup", "on", 0));
        OffsetControlAuto->setText(QApplication::translate("CameraSetup", "auto", 0));
        OffsetControlOff->setText(QApplication::translate("CameraSetup", "off", 0));
        OffsetControlLabel->setText(QApplication::translate("CameraSetup", "Offset Control", 0));
        BWNoiseFilterLabel->setText(QApplication::translate("CameraSetup", "B/W Noise Filter", 0));
        BWNoiseFilter->clear();
        BWNoiseFilter->insertItems(0, QStringList()
         << QApplication::translate("CameraSetup", "off", 0)
         << QApplication::translate("CameraSetup", "on", 0)
        );
        ConversionFactordesc->setText(QApplication::translate("CameraSetup", "e/count", 0));
        ConversionFactorLabel->setText(QApplication::translate("CameraSetup", "Conversion Factor", 0));
        tabWidget->setTabText(tabWidget->indexOf(SensorMiscTab), QApplication::translate("CameraSetup", "Sensor (misc)", 0));
        AcquireModeLabel->setText(QApplication::translate("CameraSetup", "Acquire Mode", 0));
        AcquireModeAuto->setText(QApplication::translate("CameraSetup", "Auto", 0));
        AcquireModeExternal->setText(QApplication::translate("CameraSetup", "External", 0));
        AcquireModeExtTrigger->setText(QApplication::translate("CameraSetup", "Ext. frame trigger", 0));
        TimestampLabel->setText(QApplication::translate("CameraSetup", "Timestamp", 0));
        TimestampNostamp->setText(QApplication::translate("CameraSetup", "No stamp", 0));
        TimestampBinary->setText(QApplication::translate("CameraSetup", "Binary", 0));
        TimestampBinaryASCII->setText(QApplication::translate("CameraSetup", "Binary + ASCII", 0));
        TimestampASCII->setText(QApplication::translate("CameraSetup", "ASCII", 0));
        RecorderModeLabel->setText(QApplication::translate("CameraSetup", "Recorder Mode", 0));
        RecorderModeSequence->setText(QApplication::translate("CameraSetup", "Sequence", 0));
        RecorderModeRingbuffer->setText(QApplication::translate("CameraSetup", "Ring Buffer", 0));
        tabWidget->setTabText(tabWidget->indexOf(RecordingTab), QApplication::translate("CameraSetup", "Recording", 0));
        CloseButton->setText(QApplication::translate("CameraSetup", "Close", 0));
        ApplyButton->setText(QApplication::translate("CameraSetup", "Apply", 0));
        StatusText->setText(QApplication::translate("CameraSetup", "Idle", 0));
    } // retranslateUi

};

namespace Ui {
    class CameraSetup: public Ui_CameraSetup {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CAMERASETUP_H
