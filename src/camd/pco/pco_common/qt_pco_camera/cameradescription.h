//-----------------------------------------------------------------//
// Name        | cameradescription.h         | Type: ( ) source    //
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

#ifndef CAMERADESCRIPTION_H
#define CAMERADESCRIPTION_H

#define PCO_SENSOR_CREATE_OBJECT
#include "qt_pco_camera.h"
#undef PCO_SENSOR_CREATE_OBJECT
#include <QDialog>
#include "ui_cameradescription.h"


class CameraDescription : public QDialog, public Ui::CameraDescription
{
    Q_OBJECT

public:
    CameraDescription(CPco_camera* camera, QWidget *parent = 0);
    ~CameraDescription();

private:
  CPco_camera* camera;
  QTimer* temptimer;
  WORD camtype;
  DWORD serialnumber, err, cam_width, cam_height;
  char cameratype[100];
  QString sentype;
  char sensortype[100];
  SC2_Camera_Description_Response description;
  SC2_Firmware_Versions_Response response;
  short ccdtemp, camtemp, pstemp;
  unsigned int warn, error, status;
  QString firmwaretext;

private slots :
  void RefreshTemperature();

protected:

};

#endif // CAMERADESCRIPTION_H
