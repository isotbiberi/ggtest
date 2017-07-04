//-----------------------------------------------------------------//
// Name        | exposuretime.h              | Type: ( ) source    //
//-------------------------------------------|       (*) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera                                        //
//-----------------------------------------------------------------//
// Author      | EIJ, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    |                                                   //
//-----------------------------------------------------------------//
// Notes       |                                                   //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2014 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

#ifndef EXPOSURETIME_H
#define EXPOSURETIME_H

#include <QDialog>
#include "qt_pco_camera.h"
#include "interface_includes.h"

namespace Ui {
class Exposuretime;
}

class Exposuretime : public QDialog
{
    Q_OBJECT

public:
    explicit Exposuretime(CPco_camera *camera,CPco_grabber* grabber, QWidget *parent = 0);
    ~Exposuretime();

private slots:
    void on_ApplyButton_clicked();

private:
    Ui::Exposuretime *ui;
    QIntValidator *validexposure, *validdelay;
    CPco_camera* camera;
    CPco_grabber* grabber;
    SC2_Camera_Description_Response cs_description;

    void UpdateTimingTab();
    void SetTimingTab();
};

#endif // EXPOSURETIME_H
