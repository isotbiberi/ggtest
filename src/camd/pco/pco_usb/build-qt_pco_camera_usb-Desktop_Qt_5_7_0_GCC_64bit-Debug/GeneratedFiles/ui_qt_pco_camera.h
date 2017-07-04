/********************************************************************************
** Form generated from reading UI file 'qt_pco_camera.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QT_PCO_CAMERA_H
#define UI_QT_PCO_CAMERA_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_qt_pco_cameraClass
{
public:
    QAction *actionSave;
    QAction *actionInfo;
    QAction *actionExit;
    QAction *actionPicleft;
    QAction *actionPicright;
    QAction *actionPicstart;
    QAction *actionPicend;
    QAction *actionRecorded_Images;
    QAction *actionImagecounter;
    QAction *actionSaveb16tif;
    QAction *MemoryAssigned;
    QWidget *centralWidget;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *ButtonLayout;
    QFrame *frame_4;
    QVBoxLayout *verticalLayout_5;
    QLabel *CameraControlLabel;
    QPushButton *OpenCameraButton;
    QPushButton *GetDescriptionButton;
    QPushButton *SetupCameraButton;
    QPushButton *StartCameraButton;
    QFrame *frame_3;
    QVBoxLayout *verticalLayout_4;
    QLabel *ImageGrabTitle;
    QPushButton *GrabImageButton;
    QPushButton *StartLiveViewButton;
    QPushButton *RecorderMemoryButton;
    QPushButton *StartRecorderButton;
    QFrame *frame_2;
    QVBoxLayout *verticalLayout_2;
    QLabel *ImageConvertTitle;
    QPushButton *OpenConvertDialogButton;
    QPushButton *AutoRangeButton;
    QFrame *frame;
    QVBoxLayout *verticalLayout;
    QRadioButton *Image1to1;
    QRadioButton *ImageFitToBox;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer;
    QGraphicsView *ImageView;
    QMenuBar *menuBar;
    QMenu *menuFile;
    QMenu *menuHelp;
    QToolBar *Toolbar;

    void setupUi(QMainWindow *qt_pco_cameraClass)
    {
        if (qt_pco_cameraClass->objectName().isEmpty())
            qt_pco_cameraClass->setObjectName(QStringLiteral("qt_pco_cameraClass"));
        qt_pco_cameraClass->resize(976, 776);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/PCOApp.ico"), QSize(), QIcon::Normal, QIcon::Off);
        qt_pco_cameraClass->setWindowIcon(icon);
        actionSave = new QAction(qt_pco_cameraClass);
        actionSave->setObjectName(QStringLiteral("actionSave"));
        QIcon icon1;
        icon1.addFile(QStringLiteral(":/Resources/document-save-as-5.ico"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QStringLiteral(":/Resources/document-save-as-5.ico"), QSize(), QIcon::Normal, QIcon::On);
        actionSave->setIcon(icon1);
        actionInfo = new QAction(qt_pco_cameraClass);
        actionInfo->setObjectName(QStringLiteral("actionInfo"));
        QIcon icon2;
        icon2.addFile(QStringLiteral(":/Resources/help.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionInfo->setIcon(icon2);
        actionExit = new QAction(qt_pco_cameraClass);
        actionExit->setObjectName(QStringLiteral("actionExit"));
        QIcon icon3;
        icon3.addFile(QStringLiteral(":/Resources/application-exit.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionExit->setIcon(icon3);
        actionPicleft = new QAction(qt_pco_cameraClass);
        actionPicleft->setObjectName(QStringLiteral("actionPicleft"));
        QIcon icon4;
        icon4.addFile(QStringLiteral(":/Resources/arrow-left-2.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionPicleft->setIcon(icon4);
        actionPicright = new QAction(qt_pco_cameraClass);
        actionPicright->setObjectName(QStringLiteral("actionPicright"));
        QIcon icon5;
        icon5.addFile(QStringLiteral(":/Resources/arrow-right-2.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionPicright->setIcon(icon5);
        actionPicstart = new QAction(qt_pco_cameraClass);
        actionPicstart->setObjectName(QStringLiteral("actionPicstart"));
        QIcon icon6;
        icon6.addFile(QStringLiteral(":/Resources/arrow-left-double-2.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionPicstart->setIcon(icon6);
        actionPicend = new QAction(qt_pco_cameraClass);
        actionPicend->setObjectName(QStringLiteral("actionPicend"));
        QIcon icon7;
        icon7.addFile(QStringLiteral(":/Resources/arrow-right-double-2.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionPicend->setIcon(icon7);
        actionRecorded_Images = new QAction(qt_pco_cameraClass);
        actionRecorded_Images->setObjectName(QStringLiteral("actionRecorded_Images"));
        actionRecorded_Images->setCheckable(false);
        actionRecorded_Images->setEnabled(false);
        actionRecorded_Images->setMenuRole(QAction::NoRole);
        actionImagecounter = new QAction(qt_pco_cameraClass);
        actionImagecounter->setObjectName(QStringLiteral("actionImagecounter"));
        actionImagecounter->setEnabled(false);
        QFont font;
        actionImagecounter->setFont(font);
        actionImagecounter->setMenuRole(QAction::NoRole);
        actionSaveb16tif = new QAction(qt_pco_cameraClass);
        actionSaveb16tif->setObjectName(QStringLiteral("actionSaveb16tif"));
        QIcon icon8;
        icon8.addFile(QStringLiteral(":/Resources/document-save-as-5.ico"), QSize(), QIcon::Normal, QIcon::Off);
        actionSaveb16tif->setIcon(icon8);
        MemoryAssigned = new QAction(qt_pco_cameraClass);
        MemoryAssigned->setObjectName(QStringLiteral("MemoryAssigned"));
        MemoryAssigned->setEnabled(false);
        centralWidget = new QWidget(qt_pco_cameraClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        centralWidget->setAutoFillBackground(false);
        horizontalLayout = new QHBoxLayout(centralWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        ButtonLayout = new QVBoxLayout();
        ButtonLayout->setSpacing(4);
        ButtonLayout->setObjectName(QStringLiteral("ButtonLayout"));
        frame_4 = new QFrame(centralWidget);
        frame_4->setObjectName(QStringLiteral("frame_4"));
        frame_4->setFrameShape(QFrame::StyledPanel);
        frame_4->setFrameShadow(QFrame::Sunken);
        verticalLayout_5 = new QVBoxLayout(frame_4);
        verticalLayout_5->setSpacing(6);
        verticalLayout_5->setContentsMargins(11, 11, 11, 11);
        verticalLayout_5->setObjectName(QStringLiteral("verticalLayout_5"));
        CameraControlLabel = new QLabel(frame_4);
        CameraControlLabel->setObjectName(QStringLiteral("CameraControlLabel"));

        verticalLayout_5->addWidget(CameraControlLabel, 0, Qt::AlignHCenter);

        OpenCameraButton = new QPushButton(frame_4);
        OpenCameraButton->setObjectName(QStringLiteral("OpenCameraButton"));
        OpenCameraButton->setMinimumSize(QSize(16, 41));
        OpenCameraButton->setIconSize(QSize(32, 32));

        verticalLayout_5->addWidget(OpenCameraButton);

        GetDescriptionButton = new QPushButton(frame_4);
        GetDescriptionButton->setObjectName(QStringLiteral("GetDescriptionButton"));
        GetDescriptionButton->setMinimumSize(QSize(16, 41));

        verticalLayout_5->addWidget(GetDescriptionButton);

        SetupCameraButton = new QPushButton(frame_4);
        SetupCameraButton->setObjectName(QStringLiteral("SetupCameraButton"));
        SetupCameraButton->setMinimumSize(QSize(16, 41));

        verticalLayout_5->addWidget(SetupCameraButton);

        StartCameraButton = new QPushButton(frame_4);
        StartCameraButton->setObjectName(QStringLiteral("StartCameraButton"));
        StartCameraButton->setMinimumSize(QSize(16, 41));

        verticalLayout_5->addWidget(StartCameraButton);


        ButtonLayout->addWidget(frame_4);

        frame_3 = new QFrame(centralWidget);
        frame_3->setObjectName(QStringLiteral("frame_3"));
        frame_3->setFrameShape(QFrame::StyledPanel);
        frame_3->setFrameShadow(QFrame::Sunken);
        verticalLayout_4 = new QVBoxLayout(frame_3);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        ImageGrabTitle = new QLabel(frame_3);
        ImageGrabTitle->setObjectName(QStringLiteral("ImageGrabTitle"));

        verticalLayout_4->addWidget(ImageGrabTitle, 0, Qt::AlignHCenter);

        GrabImageButton = new QPushButton(frame_3);
        GrabImageButton->setObjectName(QStringLiteral("GrabImageButton"));
        GrabImageButton->setMinimumSize(QSize(0, 41));

        verticalLayout_4->addWidget(GrabImageButton);

        StartLiveViewButton = new QPushButton(frame_3);
        StartLiveViewButton->setObjectName(QStringLiteral("StartLiveViewButton"));
        StartLiveViewButton->setMinimumSize(QSize(0, 41));

        verticalLayout_4->addWidget(StartLiveViewButton);

        RecorderMemoryButton = new QPushButton(frame_3);
        RecorderMemoryButton->setObjectName(QStringLiteral("RecorderMemoryButton"));
        RecorderMemoryButton->setMinimumSize(QSize(0, 41));

        verticalLayout_4->addWidget(RecorderMemoryButton);

        StartRecorderButton = new QPushButton(frame_3);
        StartRecorderButton->setObjectName(QStringLiteral("StartRecorderButton"));
        StartRecorderButton->setMinimumSize(QSize(0, 41));

        verticalLayout_4->addWidget(StartRecorderButton);


        ButtonLayout->addWidget(frame_3);

        frame_2 = new QFrame(centralWidget);
        frame_2->setObjectName(QStringLiteral("frame_2"));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Sunken);
        verticalLayout_2 = new QVBoxLayout(frame_2);
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setContentsMargins(11, 11, 11, 11);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        ImageConvertTitle = new QLabel(frame_2);
        ImageConvertTitle->setObjectName(QStringLiteral("ImageConvertTitle"));

        verticalLayout_2->addWidget(ImageConvertTitle, 0, Qt::AlignHCenter);

        OpenConvertDialogButton = new QPushButton(frame_2);
        OpenConvertDialogButton->setObjectName(QStringLiteral("OpenConvertDialogButton"));
        OpenConvertDialogButton->setMinimumSize(QSize(16, 41));

        verticalLayout_2->addWidget(OpenConvertDialogButton);

        AutoRangeButton = new QPushButton(frame_2);
        AutoRangeButton->setObjectName(QStringLiteral("AutoRangeButton"));
        AutoRangeButton->setMinimumSize(QSize(16, 41));

        verticalLayout_2->addWidget(AutoRangeButton);


        ButtonLayout->addWidget(frame_2);

        frame = new QFrame(centralWidget);
        frame->setObjectName(QStringLiteral("frame"));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Sunken);
        verticalLayout = new QVBoxLayout(frame);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        Image1to1 = new QRadioButton(frame);
        Image1to1->setObjectName(QStringLiteral("Image1to1"));
        Image1to1->setChecked(true);

        verticalLayout->addWidget(Image1to1, 0, Qt::AlignHCenter);

        ImageFitToBox = new QRadioButton(frame);
        ImageFitToBox->setObjectName(QStringLiteral("ImageFitToBox"));

        verticalLayout->addWidget(ImageFitToBox, 0, Qt::AlignHCenter);


        ButtonLayout->addWidget(frame);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        ButtonLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(ButtonLayout);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        ImageView = new QGraphicsView(centralWidget);
        ImageView->setObjectName(QStringLiteral("ImageView"));
        ImageView->setMinimumSize(QSize(700, 0));
        ImageView->setAcceptDrops(false);
        ImageView->setFrameShape(QFrame::StyledPanel);
        ImageView->setFrameShadow(QFrame::Sunken);
        ImageView->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);

        horizontalLayout->addWidget(ImageView);

        qt_pco_cameraClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(qt_pco_cameraClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 976, 29));
        menuFile = new QMenu(menuBar);
        menuFile->setObjectName(QStringLiteral("menuFile"));
        menuHelp = new QMenu(menuBar);
        menuHelp->setObjectName(QStringLiteral("menuHelp"));
        qt_pco_cameraClass->setMenuBar(menuBar);
        Toolbar = new QToolBar(qt_pco_cameraClass);
        Toolbar->setObjectName(QStringLiteral("Toolbar"));
        Toolbar->setContextMenuPolicy(Qt::NoContextMenu);
        Toolbar->setMovable(false);
        Toolbar->setIconSize(QSize(24, 24));
        Toolbar->setFloatable(false);
        qt_pco_cameraClass->addToolBar(Qt::TopToolBarArea, Toolbar);

        menuBar->addAction(menuFile->menuAction());
        menuBar->addAction(menuHelp->menuAction());
        menuFile->addAction(actionSave);
        menuFile->addAction(actionSaveb16tif);
        menuFile->addAction(actionExit);
        menuHelp->addAction(actionInfo);
        Toolbar->addAction(actionSave);
        Toolbar->addSeparator();
        Toolbar->addAction(actionPicstart);
        Toolbar->addAction(actionPicleft);
        Toolbar->addAction(actionPicright);
        Toolbar->addAction(actionPicend);
        Toolbar->addSeparator();
        Toolbar->addAction(actionRecorded_Images);
        Toolbar->addAction(actionImagecounter);
        Toolbar->addAction(MemoryAssigned);
        Toolbar->addSeparator();

        retranslateUi(qt_pco_cameraClass);

        QMetaObject::connectSlotsByName(qt_pco_cameraClass);
    } // setupUi

    void retranslateUi(QMainWindow *qt_pco_cameraClass)
    {
        qt_pco_cameraClass->setWindowTitle(QApplication::translate("qt_pco_cameraClass", "CameraDemoQT", 0));
        actionSave->setText(QApplication::translate("qt_pco_cameraClass", "Save file", 0));
        actionInfo->setText(QApplication::translate("qt_pco_cameraClass", "Info", 0));
        actionExit->setText(QApplication::translate("qt_pco_cameraClass", "Exit", 0));
        actionPicleft->setText(QApplication::translate("qt_pco_cameraClass", "picleft", 0));
        actionPicleft->setIconText(QApplication::translate("qt_pco_cameraClass", "picleft", 0));
#ifndef QT_NO_TOOLTIP
        actionPicleft->setToolTip(QApplication::translate("qt_pco_cameraClass", "One picture to the left", 0));
#endif // QT_NO_TOOLTIP
        actionPicleft->setShortcut(QApplication::translate("qt_pco_cameraClass", "Left", 0));
        actionPicright->setText(QApplication::translate("qt_pco_cameraClass", "picright", 0));
        actionPicright->setIconText(QApplication::translate("qt_pco_cameraClass", "picright", 0));
#ifndef QT_NO_TOOLTIP
        actionPicright->setToolTip(QApplication::translate("qt_pco_cameraClass", "One picture to the right", 0));
#endif // QT_NO_TOOLTIP
        actionPicright->setShortcut(QApplication::translate("qt_pco_cameraClass", "Right", 0));
        actionPicstart->setText(QApplication::translate("qt_pco_cameraClass", "picstart", 0));
#ifndef QT_NO_TOOLTIP
        actionPicstart->setToolTip(QApplication::translate("qt_pco_cameraClass", "First picture", 0));
#endif // QT_NO_TOOLTIP
        actionPicstart->setShortcut(QApplication::translate("qt_pco_cameraClass", "Down", 0));
        actionPicend->setText(QApplication::translate("qt_pco_cameraClass", "picend", 0));
#ifndef QT_NO_TOOLTIP
        actionPicend->setToolTip(QApplication::translate("qt_pco_cameraClass", "Last picture", 0));
#endif // QT_NO_TOOLTIP
        actionPicend->setShortcut(QApplication::translate("qt_pco_cameraClass", "Up", 0));
        actionRecorded_Images->setText(QApplication::translate("qt_pco_cameraClass", "Recorded Images:", 0));
        actionImagecounter->setText(QApplication::translate("qt_pco_cameraClass", "0 / 0", 0));
        actionSaveb16tif->setText(QApplication::translate("qt_pco_cameraClass", "Save .b16/.tif file", 0));
        actionSaveb16tif->setShortcut(QApplication::translate("qt_pco_cameraClass", "Ctrl+S", 0));
        MemoryAssigned->setText(QApplication::translate("qt_pco_cameraClass", "Mem: 0", 0));
        CameraControlLabel->setText(QApplication::translate("qt_pco_cameraClass", "Camera Control", 0));
        OpenCameraButton->setText(QApplication::translate("qt_pco_cameraClass", "Open Camera", 0));
        GetDescriptionButton->setText(QApplication::translate("qt_pco_cameraClass", "Get Description", 0));
        SetupCameraButton->setText(QApplication::translate("qt_pco_cameraClass", "Setup Camera", 0));
        StartCameraButton->setText(QApplication::translate("qt_pco_cameraClass", "Start Camera", 0));
        ImageGrabTitle->setText(QApplication::translate("qt_pco_cameraClass", "Image Grab", 0));
        GrabImageButton->setText(QApplication::translate("qt_pco_cameraClass", "Grab Image", 0));
        StartLiveViewButton->setText(QApplication::translate("qt_pco_cameraClass", "Start Live View", 0));
        RecorderMemoryButton->setText(QApplication::translate("qt_pco_cameraClass", "Assign Rec. Memory", 0));
        StartRecorderButton->setText(QApplication::translate("qt_pco_cameraClass", "Start Recorder", 0));
        ImageConvertTitle->setText(QApplication::translate("qt_pco_cameraClass", "Image Convert Options", 0));
        OpenConvertDialogButton->setText(QApplication::translate("qt_pco_cameraClass", "Open Convert Dialog", 0));
        AutoRangeButton->setText(QApplication::translate("qt_pco_cameraClass", "Auto Range", 0));
        Image1to1->setText(QApplication::translate("qt_pco_cameraClass", "Image 1:1", 0));
        ImageFitToBox->setText(QApplication::translate("qt_pco_cameraClass", "Fit to Box", 0));
        menuFile->setTitle(QApplication::translate("qt_pco_cameraClass", "File", 0));
        menuHelp->setTitle(QApplication::translate("qt_pco_cameraClass", "Help", 0));
        Toolbar->setWindowTitle(QApplication::translate("qt_pco_cameraClass", "toolBar", 0));
    } // retranslateUi

};

namespace Ui {
    class qt_pco_cameraClass: public Ui_qt_pco_cameraClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QT_PCO_CAMERA_H
