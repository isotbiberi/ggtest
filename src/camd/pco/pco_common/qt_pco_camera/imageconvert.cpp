//-----------------------------------------------------------------//
// Name        | imageconvert.cpp            | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux, Windows                                    //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | Qt project part                                   //
//-----------------------------------------------------------------//
// Author      | EIJ, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.00                                         //
//-----------------------------------------------------------------//
// Notes       | Image convert dialog                              //
//             |                                                   //
//             |                                                   //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2015 PCO AG                                                 //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// This program is free software; you can redistribute it and/or   //
// modify it under the terms of the GNU General Public License as  //
// published by the Free Software Foundation; either version 2 of  //
// the License, or (at your option) any later version.             //
//                                                                 //
// This program is distributed in the hope that it will be useful, //
// but WITHOUT ANY WARRANTY; without even the implied warranty of  //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the    //
// GNU General Public License for more details.                    //
//                                                                 //
// You should have received a copy of the GNU General Public       //
// License along with this program; if not, write to the           //
// Free Software Foundation, Inc., 59 Temple Place- Suite 330,     //
// Boston, MA 02111-1307, USA                                      //
//-----------------------------------------------------------------//


#include "imageconvert.h"


ImageConvert::ImageConvert(BWLUT* lut, int* botvalue, int* topvalue, double* gamma, QWidget *parent)
    : QDialog(parent)
{
    setupUi(this);
    blackpen = new QPen(Qt::black);

    this->topvalue = topvalue;
    this->botvalue = botvalue;
    this->gamma = gamma;

    //create scene
    convertscene = new QGraphicsScene(0,0,CONVERT_WIDTH,CONVERT_HEIGHT);
    ConvertGraphicsView->scale(1, -1);

    if (lut == NULL) {
      ErrorPopup *error = new ErrorPopup;
      error->setModal(true);
      QString errortext = "No LUT found.";
      error->ErrorPopupText->setText(errortext);
      error->exec();
    }
    else
      this->lut = lut;

    blackpen->setWidth(650);

    DrawScene();

    ConvertGraphicsView->setBaseSize(convertscene->height(), convertscene->width());
    ConvertGraphicsView->setSceneRect(convertscene->sceneRect());
    ConvertGraphicsView->ensureVisible(convertscene->sceneRect());
    ConvertGraphicsView->setScene(convertscene);
    ConvertGraphicsView->fitInView(convertscene->sceneRect(), Qt::IgnoreAspectRatio);
    ConvertGraphicsView->adjustSize();
    ConvertGraphicsView->setRenderHint(QPainter::Antialiasing,true);

    TopValueBox->setValue(*topvalue);
    BottomValueBox->setValue(*botvalue);
    GammaDoubleBox->setValue(*gamma);


}

ImageConvert::~ImageConvert()
{
    delete blackpen;
}

void ImageConvert::on_TopValueBox_valueChanged(int value) {
  //top value < bot value
  if (value > *botvalue) {
    *topvalue = value;
  }
  else {
    *topvalue = *botvalue; // cap at botvalue
    TopValueBox->setValue(*topvalue);
  }
  DrawScene();
}


void ImageConvert::on_BottomValueBox_valueChanged(int value) {
  //bot value > top value AND invert checked
  if (value < *topvalue) {
    *botvalue = value;
  }
  else {
    *botvalue = *topvalue; // cap at botvalue
    BottomValueBox->setValue(*botvalue);
  }
  DrawScene();
}

void ImageConvert::on_GammaDoubleBox_valueChanged(double value) {
  *gamma = value;
  DrawScene();
}

void ImageConvert::updateValues(int min,int max) {
    *topvalue = max;
    *botvalue = min;
    BottomValueBox->setValue(*botvalue);
    TopValueBox->setValue(*topvalue);
}

void ImageConvert::updateMainWindow() {
  pcocnv_convert_set_ex(lut, *botvalue, *topvalue, 1, *gamma);
  //emit changed signal
  emit ConversionChanged();
}

void ImageConvert::showEvent(QShowEvent * event)
{
  ConvertGraphicsView->fitInView(0, 0, convertscene->width(), convertscene->height());
  QWidget::showEvent(event);
}

void ImageConvert::DrawScene() {
    updateMainWindow();
    convertscene->clear();
    path = QPainterPath();
    //move to start
    path.moveTo(0, 0);
    //draw middle curve from LUT
    void* pointer = lut->ptr;
    unsigned int lastvalue = (*(static_cast<quint8*>(pointer)))*256;
    for(int i = 0; i < lut->size;i++) {
        unsigned int value = (*(static_cast<quint8*>(pointer)+i))*256;
            if(value != lastvalue) {
                //only draw if something changed
                 path.lineTo(i-1, lastvalue); //draw line to last unchanged point
                 path.lineTo(i, value); //draw line to changed point
            }
            lastvalue = value;
    }
    //close out path
    path.lineTo(CONVERT_WIDTH, CONVERT_HEIGHT);

    convertscene->addPath(path, *blackpen);

    convertscene->addRect(lut->mid-(MID_VALUE_RECTANGLE_SIZE/2),(CONVERT_HEIGHT/2)-(MID_VALUE_RECTANGLE_SIZE/2),MID_VALUE_RECTANGLE_SIZE,MID_VALUE_RECTANGLE_SIZE,QPen(Qt::red),QBrush(Qt::red));

    ConvertGraphicsView->update();
}
