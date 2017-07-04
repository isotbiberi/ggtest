/********************************************************************************
** Form generated from reading UI file 'imageconvert.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_IMAGECONVERT_H
#define UI_IMAGECONVERT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDoubleSpinBox>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_ImageConvert
{
public:
    QGridLayout *gridLayout_2;
    QGridLayout *GridLeft;
    QLabel *LabelBottomfarleft;
    QLabel *LabelBottomleft;
    QLabel *LabelMidleft;
    QLabel *LabelTopleft;
    QLabel *LabelBottomright;
    QLabel *LabelBottomMiddle;
    QGraphicsView *ConvertGraphicsView;
    QGridLayout *GridRight;
    QLabel *GammaLabel;
    QSpinBox *BottomValueBox;
    QSpinBox *TopValueBox;
    QSpacerItem *Spacer;
    QDoubleSpinBox *GammaDoubleBox;

    void setupUi(QDialog *ImageConvert)
    {
        if (ImageConvert->objectName().isEmpty())
            ImageConvert->setObjectName(QStringLiteral("ImageConvert"));
        ImageConvert->resize(380, 200);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(ImageConvert->sizePolicy().hasHeightForWidth());
        ImageConvert->setSizePolicy(sizePolicy);
        ImageConvert->setMinimumSize(QSize(380, 200));
        ImageConvert->setMaximumSize(QSize(380, 200));
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/PCOApp.ico"), QSize(), QIcon::Normal, QIcon::Off);
        ImageConvert->setWindowIcon(icon);
        gridLayout_2 = new QGridLayout(ImageConvert);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        GridLeft = new QGridLayout();
        GridLeft->setSpacing(6);
        GridLeft->setObjectName(QStringLiteral("GridLeft"));
        LabelBottomfarleft = new QLabel(ImageConvert);
        LabelBottomfarleft->setObjectName(QStringLiteral("LabelBottomfarleft"));

        GridLeft->addWidget(LabelBottomfarleft, 2, 0, 1, 1, Qt::AlignRight|Qt::AlignBottom);

        LabelBottomleft = new QLabel(ImageConvert);
        LabelBottomleft->setObjectName(QStringLiteral("LabelBottomleft"));

        GridLeft->addWidget(LabelBottomleft, 3, 1, 1, 1, Qt::AlignTop);

        LabelMidleft = new QLabel(ImageConvert);
        LabelMidleft->setObjectName(QStringLiteral("LabelMidleft"));

        GridLeft->addWidget(LabelMidleft, 1, 0, 1, 1, Qt::AlignRight);

        LabelTopleft = new QLabel(ImageConvert);
        LabelTopleft->setObjectName(QStringLiteral("LabelTopleft"));

        GridLeft->addWidget(LabelTopleft, 0, 0, 1, 1, Qt::AlignRight|Qt::AlignTop);

        LabelBottomright = new QLabel(ImageConvert);
        LabelBottomright->setObjectName(QStringLiteral("LabelBottomright"));

        GridLeft->addWidget(LabelBottomright, 3, 3, 1, 1, Qt::AlignRight|Qt::AlignTop);

        LabelBottomMiddle = new QLabel(ImageConvert);
        LabelBottomMiddle->setObjectName(QStringLiteral("LabelBottomMiddle"));

        GridLeft->addWidget(LabelBottomMiddle, 3, 2, 1, 1, Qt::AlignHCenter|Qt::AlignTop);

        ConvertGraphicsView = new QGraphicsView(ImageConvert);
        ConvertGraphicsView->setObjectName(QStringLiteral("ConvertGraphicsView"));
        ConvertGraphicsView->setMinimumSize(QSize(229, 145));
        ConvertGraphicsView->setMaximumSize(QSize(229, 145));
        ConvertGraphicsView->setRenderHints(QPainter::Antialiasing|QPainter::HighQualityAntialiasing|QPainter::TextAntialiasing);

        GridLeft->addWidget(ConvertGraphicsView, 0, 1, 3, 3);


        gridLayout_2->addLayout(GridLeft, 0, 0, 2, 1);

        GridRight = new QGridLayout();
        GridRight->setSpacing(6);
        GridRight->setObjectName(QStringLiteral("GridRight"));
        GammaLabel = new QLabel(ImageConvert);
        GammaLabel->setObjectName(QStringLiteral("GammaLabel"));

        GridRight->addWidget(GammaLabel, 1, 0, 1, 1, Qt::AlignHCenter);

        BottomValueBox = new QSpinBox(ImageConvert);
        BottomValueBox->setObjectName(QStringLiteral("BottomValueBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(BottomValueBox->sizePolicy().hasHeightForWidth());
        BottomValueBox->setSizePolicy(sizePolicy1);
        BottomValueBox->setMinimumSize(QSize(86, 0));
        BottomValueBox->setMaximumSize(QSize(86, 16777215));
        BottomValueBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        BottomValueBox->setMaximum(65535);

        GridRight->addWidget(BottomValueBox, 4, 0, 1, 1);

        TopValueBox = new QSpinBox(ImageConvert);
        TopValueBox->setObjectName(QStringLiteral("TopValueBox"));
        sizePolicy1.setHeightForWidth(TopValueBox->sizePolicy().hasHeightForWidth());
        TopValueBox->setSizePolicy(sizePolicy1);
        TopValueBox->setMinimumSize(QSize(86, 0));
        TopValueBox->setMaximumSize(QSize(86, 16777215));
        TopValueBox->setBaseSize(QSize(0, 0));
        TopValueBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        TopValueBox->setMaximum(65535);

        GridRight->addWidget(TopValueBox, 0, 0, 1, 1);

        Spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        GridRight->addItem(Spacer, 3, 0, 1, 1);

        GammaDoubleBox = new QDoubleSpinBox(ImageConvert);
        GammaDoubleBox->setObjectName(QStringLiteral("GammaDoubleBox"));
        sizePolicy1.setHeightForWidth(GammaDoubleBox->sizePolicy().hasHeightForWidth());
        GammaDoubleBox->setSizePolicy(sizePolicy1);
        GammaDoubleBox->setMinimumSize(QSize(86, 0));
        GammaDoubleBox->setMaximumSize(QSize(86, 16777215));
        GammaDoubleBox->setAlignment(Qt::AlignRight|Qt::AlignTrailing|Qt::AlignVCenter);
        GammaDoubleBox->setDecimals(2);
        GammaDoubleBox->setMinimum(0.01);
        GammaDoubleBox->setMaximum(100);
        GammaDoubleBox->setSingleStep(0.1);
        GammaDoubleBox->setValue(1);

        GridRight->addWidget(GammaDoubleBox, 2, 0, 1, 1);


        gridLayout_2->addLayout(GridRight, 0, 2, 2, 1);


        retranslateUi(ImageConvert);

        QMetaObject::connectSlotsByName(ImageConvert);
    } // setupUi

    void retranslateUi(QDialog *ImageConvert)
    {
        ImageConvert->setWindowTitle(QApplication::translate("ImageConvert", "ImageConvert", 0));
        LabelBottomfarleft->setText(QApplication::translate("ImageConvert", "0", 0));
        LabelBottomleft->setText(QApplication::translate("ImageConvert", "0", 0));
        LabelMidleft->setText(QApplication::translate("ImageConvert", "127", 0));
        LabelTopleft->setText(QApplication::translate("ImageConvert", "255", 0));
        LabelBottomright->setText(QApplication::translate("ImageConvert", "65535", 0));
        LabelBottomMiddle->setText(QApplication::translate("ImageConvert", "32768", 0));
        GammaLabel->setText(QApplication::translate("ImageConvert", "Gamma", 0));
    } // retranslateUi

};

namespace Ui {
    class ImageConvert: public Ui_ImageConvert {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_IMAGECONVERT_H
