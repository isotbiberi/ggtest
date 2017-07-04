//-----------------------------------------------------------------//
// Name        | qt_pco_camera.h             | Type: ( ) source    //
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
// Author      | MBL,EIJ PCO AG                                    //
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

#ifndef QT_PCO_CAMERA_H
#define QT_PCO_CAMERA_H

/* Camera & Grabber files */
#include "interface_includes.h"
#include "Cpco_com.h"

/* Qt headers */
#include <qmainwindow.h>
#include <qfiledialog.h>
#include <qthread.h>
#include <qtimer.h>
#include <qvector.h>
#include <qprogressdialog.h>

/* UI Elements */
#include "ui_qt_pco_camera.h"
#include "cameradescription.h"
#include "errorpopup.h"
#include "infopopup.h"
#include "camerasetup.h"
#include "memorymanager.h"
#include "simpledialog.h"
#include "imageconvert.h"
//#include "speedtest.h"
#include "exposuretime.h"

#include <sys/sysinfo.h>
#include "pcocnv.h"
#include "cnvbw.h"

#ifndef WIN32
#define O_BINARY 0
#define _CRT_SECURE_NO_WARNINGS
#endif


#define RUN  1
#define STOP 0

#define LIVEVIEW_BUFFERSIZE 4

class qt_pco_camera : public QMainWindow
{
    Q_OBJECT

public:

  qt_pco_camera(CPco_Log* Clog, CPco_camera* camera=NULL, CPco_grabber* grabber=NULL, QWidget *parent = 0);
  ~qt_pco_camera();

  //debug
    DWORD SetInitialValues();

private:
  CPco_camera* camera;
  CPco_grabber* grabber;
  CPco_Log* mylog;

  /* GLOBAL SWITCHES */
  int camera_is_open;
  int camera_is_running;
  int liveview_isrunning;

  /* CAMERA VALUES */
  DWORD cam_width, cam_height;
  WORD camtype;
  DWORD serialnumber;
  WORD Interfacetype;
  DWORD err;
  char infostr[100];
  WORD act_recstate;
  SC2_Camera_Description_Response description;
  int bitpix, picsize;
  int images_available, current_image;
  int board;
  WORD doubleimagemode;
  BWLUT* lut;
  int ImageTimeout;

  //Vector with all stored images from the Recorder
  QVector<unsigned short*> ImageVector;
  QVector<unsigned short*> ImageVectorLive;

  //single_image: Picture from single image grab
  //current_image_buffer: Pointer to the buffer with the current picture
  //current_image_converted: Holds the converted picture currently displayed

  unsigned short * __attribute__((__may_alias__)) single_image ;  //since this is always pointing to memory assigned via other buffers, disable compiler optimizations for this variable
  unsigned short *current_image_buffer;
  unsigned char *current_image_converted;

  //QT specific variables

  QImage pcoimage;
  QGraphicsScene* scene;
  QGraphicsPixmapItem *currentPixmap;



  //conversion variables
  int convertmin, convertmax;
  double convertgamma;


  Ui::qt_pco_cameraClass ui;
  void SetControlButtonStatus(BOOL status);
  void SetImageGrabButtonStatus(BOOL status);
  void SetImageConvertOptionsButtonStatus(BOOL status);
  void resizeEvent(QResizeEvent* = 0);
  void SetImageScrollButtonStatus(BOOL status);
  bool AssignMemory(int images);
  void FreeImageVector();




protected:

private slots :
  void on_OpenCameraButton_clicked();
  void on_GetDescriptionButton_clicked();
  void on_SetupCameraButton_clicked();
  void on_StartCameraButton_clicked();
  void on_GrabImageButton_clicked();
  void on_RecorderMemoryButton_clicked();
  void on_StartRecorderButton_clicked();
  void on_StartLiveViewButton_clicked();
  void on_ImageFitToBox_toggled();
  void on_Image1to1_toggled();
  void on_actionSave_triggered();
  void on_actionExit_triggered();
  void on_actionInfo_triggered();
  void on_actionPicstart_triggered();
  void on_actionPicend_triggered();
  void on_actionPicleft_triggered();
  void on_actionPicright_triggered();
  void displayLastImage(int imagenumber);
  void recorderError(int number);
  void displayRecordedImages(int imagenumber);
  void on_actionSaveb16tif_triggered();
  void on_OpenConvertDialogButton_clicked();
  void on_AutoRangeButton_clicked();
  void DisplayCurrentImage();
//  void on_actionSpeedTest_triggered();


signals:
  void autorange_clicked(int,int);

};

#endif // QT_PCO_CAMERA_H
