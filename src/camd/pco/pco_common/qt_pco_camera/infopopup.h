#ifndef INFOPOPUP_H
#define INFOPOPUP_H

#include <QDialog>
#include "ui_infopopup.h"

class InfoPopup : public QDialog, public Ui::InfoPopup
{
    Q_OBJECT

public:
    InfoPopup(QWidget *parent = 0);
    ~InfoPopup();
};

#endif // INFOPOPUP_H
