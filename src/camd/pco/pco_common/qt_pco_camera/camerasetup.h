//-----------------------------------------------------------------//
// Name        | camerasetup.h               | Type: ( ) source    //
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

#ifndef CAMERASETUP_H
#define CAMERASETUP_H

#include <QDialog>
#include <QGraphicsRectItem>
#include <qtimer.h>
#include <qvalidator.h>
#include "ui_camerasetup.h"
#include "qt_pco_camera.h"
#include "interface_includes.h"

class CameraSetup : public QDialog, public Ui::CameraSetup
{
  Q_OBJECT

public:
  CameraSetup(CPco_camera* camera, CPco_grabber* grabber, QWidget *parent = 0);
  ~CameraSetup();

private:
  QIntValidator *validexposure, *validdelay;
  SC2_Camera_Description_Response cs_description;
  CPco_camera* camera;
  CPco_grabber* grabber;
  DWORD cam_width,cam_height,serialnumber;
  WORD camtype;
  WORD wRoiX0,wRoiY0, wRoiX1, wRoiY1;
  QGraphicsScene* roiscene;
  QGraphicsRectItem *rectyellow, *rectgreen;
  WORD sensor;

  Ui::CameraSetup ui;
  void updateROIwindow();
  void showEvent(QShowEvent * event);

  void UpdateSettings();
  void UpdateTimingTab();
  void UpdateSensorSizeTab();
  void UpdateSensorMiscTab();
  void UpdateRecordingTab();

  void SetTimingTab();
  void SetSensorSizeTab();
  void SetSensorMiscTab();
  void SetRecordingTab();

  void resizeEvent(QResizeEvent *);

  private slots:
  void on_ROIx0_editingFinished();
  void on_ROIx1_editingFinished();
  void on_ROIy0_editingFinished();
  void on_ROIy1_editingFinished();
  void setROIx1sym(int value);
  void setROIy1sym(int value);
  void on_ROImaxbutton_clicked();
  void on_ApplyButton_clicked();
  void SetCheckboxToIdle();
  void on_tabWidget_tabBarClicked(int index);
};

#endif // CAMERASETUP_H
