/********************************************************************************
** Form generated from reading UI file 'infopopup.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INFOPOPUP_H
#define UI_INFOPOPUP_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QFrame>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_InfoPopup
{
public:
    QVBoxLayout *verticalLayout_4;
    QVBoxLayout *verticalLayout;
    QLabel *label;
    QFrame *line;
    QVBoxLayout *verticalLayout_3;
    QLabel *label_2;
    QFrame *line_2;
    QSpacerItem *verticalSpacer;
    QVBoxLayout *verticalLayout_2;
    QPushButton *pushButton;

    void setupUi(QDialog *InfoPopup)
    {
        if (InfoPopup->objectName().isEmpty())
            InfoPopup->setObjectName(QStringLiteral("InfoPopup"));
        InfoPopup->resize(497, 378);
        QSizePolicy sizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(InfoPopup->sizePolicy().hasHeightForWidth());
        InfoPopup->setSizePolicy(sizePolicy);
        QIcon icon;
        icon.addFile(QStringLiteral(":/Resources/PCOApp.ico"), QSize(), QIcon::Normal, QIcon::Off);
        InfoPopup->setWindowIcon(icon);
        verticalLayout_4 = new QVBoxLayout(InfoPopup);
        verticalLayout_4->setSpacing(6);
        verticalLayout_4->setContentsMargins(11, 11, 11, 11);
        verticalLayout_4->setObjectName(QStringLiteral("verticalLayout_4"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(InfoPopup);
        label->setObjectName(QStringLiteral("label"));
        label->setWordWrap(true);

        verticalLayout->addWidget(label);


        verticalLayout_4->addLayout(verticalLayout);

        line = new QFrame(InfoPopup);
        line->setObjectName(QStringLiteral("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout_4->addWidget(line);

        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(6);
        verticalLayout_3->setObjectName(QStringLiteral("verticalLayout_3"));
        label_2 = new QLabel(InfoPopup);
        label_2->setObjectName(QStringLiteral("label_2"));
        label_2->setTextFormat(Qt::RichText);
        label_2->setWordWrap(true);
        label_2->setOpenExternalLinks(true);

        verticalLayout_3->addWidget(label_2);


        verticalLayout_4->addLayout(verticalLayout_3);

        line_2 = new QFrame(InfoPopup);
        line_2->setObjectName(QStringLiteral("line_2"));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);

        verticalLayout_4->addWidget(line_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout_4->addItem(verticalSpacer);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(6);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        pushButton = new QPushButton(InfoPopup);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        pushButton->setMinimumSize(QSize(70, 40));
        pushButton->setMaximumSize(QSize(70, 40));

        verticalLayout_2->addWidget(pushButton, 0, Qt::AlignHCenter);


        verticalLayout_4->addLayout(verticalLayout_2);


        retranslateUi(InfoPopup);
        QObject::connect(pushButton, SIGNAL(clicked()), InfoPopup, SLOT(accept()));

        QMetaObject::connectSlotsByName(InfoPopup);
    } // setupUi

    void retranslateUi(QDialog *InfoPopup)
    {
        InfoPopup->setWindowTitle(QApplication::translate("InfoPopup", "InfoPopup", 0));
        label->setText(QApplication::translate("InfoPopup", " This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.\n"
"\n"
"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. \n"
"\n"
" You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA   ", 0));
        label_2->setText(QApplication::translate("InfoPopup", " &copy; 2010 - 2015 PCO AG<br>Donaupark 11 D-93309  Kelheim / Germany<br> Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20 <br> Email: <a href=\"mailto:info@pco.de\">info@pco.de</a>", 0));
        pushButton->setText(QApplication::translate("InfoPopup", "Ok", 0));
    } // retranslateUi

};

namespace Ui {
    class InfoPopup: public Ui_InfoPopup {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INFOPOPUP_H
