//-----------------------------------------------------------------//
// Name        | imageconvert.h              | Type: ( ) source    //
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

#ifndef IMAGECONVERT_H
#define IMAGECONVERT_H

#include <QDialog>
#include "ui_imageconvert.h"
#include "qt_pco_camera.h"
#include "qline.h"
#include "pcocnv.h"


#define MID_VALUE_RECTANGLE_SIZE 2500
#define CONVERT_HEIGHT 65536
#define CONVERT_WIDTH 65536
#define CONVERT_HEIGHT_ADJUST 3

class ImageConvert : public QDialog, public Ui::ImageConvert
{
  Q_OBJECT

public:
  ImageConvert(BWLUT* lut, int* topvalue, int* botvalue, double* gamma, QWidget *parent = 0);
  ~ImageConvert();

private:
  QGraphicsScene* convertscene;
  int *topvalue, *botvalue;
  double* gamma;
  BWLUT* lut;

  QPen* blackpen;
  QPainterPath path;

  void updateMainWindow();
  void DrawScene();

  private slots:
  void on_TopValueBox_valueChanged(int topvalue);
  void on_BottomValueBox_valueChanged(int botvalue);
  void on_GammaDoubleBox_valueChanged(double gamma);
  void showEvent(QShowEvent * event);
  void updateValues(int min,int max);

signals:
  void ConversionChanged();


};

#endif // IMAGECONVERT_H
