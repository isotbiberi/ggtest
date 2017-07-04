//-----------------------------------------------------------------//
// Name        | cameradescription.cpp       | Type: (*) source    //
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
// Notes       | Camera Description Dialog                         //
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

#include "cameradescription.h"

CameraDescription::CameraDescription(CPco_camera *camera, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    this->camera = camera;
    err = camera->PCO_GetCameraType(&camtype, &serialnumber);
    err += camera->PCO_GetInfo(INFO_STRING_CAMERA , cameratype, sizeof(cameratype));
    err += camera->PCO_GetActualSize(&cam_width, &cam_height);
    err += camera->PCO_GetTemperature(&ccdtemp, &camtemp, &pstemp);
    err += camera->PCO_GetCameraDescription(&description);
    err += camera->PCO_GetHealthStatus(&warn,&error,&status);
    err += camera->PCO_GetFirmwareVersion(&response);
    if(err != PCO_NOERROR) {
        CameraType->setText("Error while getting informations!");
        return;
    }
    err += camera->PCO_GetInfo(INFO_STRING_SENSOR , sensortype, sizeof(sensortype));
    //if((err&~PCO_ERROR_DEVICE_MASK)==PCO_ERROR_FIRMWARE_NOT_SUPPORTED) {
    //read sensor data from cam descriptor
    WORD wSentype = description.wSensorTypeDESC+description.wSensorSubTypeDESC;
    //sprintf(sensortype,"0x%4x",wSentype);
    //translate sensortype into String (see sc2_defs.h)
    for(int i=0;i<PCO_SENSOR_TYPE_DEF_NUM;i++) {
        if(wSentype == pco_sensor[i].wTypdef) {
            sentype.append(pco_sensor[i].szName);
            break;
        }
    }
    if(sentype.isEmpty()) {
        //read info string as fallback
        err = camera->PCO_GetInfo(INFO_STRING_SENSOR , sensortype, sizeof(sensortype));
        sentype.append(sensortype);
    }


    CameraType->setText(cameratype);
    SensorType->setText(sentype);
    SerialNumber->setText(QString::number(serialnumber));

/*
    for (int i = 0; i < response.DeviceNum; i++) {
        firmwaretext.append(response.Device[i].szName);
        firmwaretext.append(QString::number(response.Device[i].bMajorRev) + ".");
        firmwaretext.append(QString::number(response.Device[i].bMinorRev) + ".");
        firmwaretext.append(QString::number(response.Device[i].wVariant)+"\n");
    }
*/
    {
     char *Firmwaretext;
     int size;
     err=camera->PCO_GetFirmwareVersion(NULL,&size);
     if(err==PCO_NOERROR)
     {
      Firmwaretext=(char*)malloc(size+100);
      if(Firmwaretext)
      {
       memset(Firmwaretext,0,size);
       err=camera->PCO_GetFirmwareVersion(Firmwaretext,&size);
       firmwaretext.append(Firmwaretext);
       free(Firmwaretext);
      }
     }
    }
    Firmware->setText((QString) firmwaretext);


    Resolution->setText(QString::number(cam_width) + " x " + QString::number(cam_height));
    //get all pixelrates
    QString pixelrates;
    for (int x = 0; x<4; x++)
    {
        if (description.dwPixelRateDESC[x] != 0) {
            pixelrates.append(QString::number(description.dwPixelRateDESC[x]));
            pixelrates.append("\n");
        }
        else
            break;
    }
    PixelRate->setText(pixelrates);
    temptimer = new QTimer(this);
    connect(temptimer, SIGNAL(timeout()), this, SLOT(RefreshTemperature()));
    temptimer->start(1000);
    RefreshTemperature();
    QString warning = "Warning:\n";
    QString errortext = "Error:\n";
    //Camera Health
    if (warn != 0) {
        if (warn & 0x0001)
            warning.append("Power Supply Voltage Range\n");
        if (warn & 0x0002)
            warning.append("Power Supply Temperature\n");
        if (warn & 0x0004)
            warning.append("Camera Temperature (board/FPGA)\n");
    }
    if (error != 0) {
        if (error & ERROR_POWERSUPPLYVOLTAGERANGE)
            errortext.append("Power Supply Voltage Range\n");
        if (error & ERROR_POWERSUPPLYTEMPERATURE)
            errortext.append("Power Supply Temperature\n");
        if (error & ERROR_CAMERATEMPERATURE)
            errortext.append("Camera temperature (board/FPGA)\n");
        if (error & ERROR_SENSORTEMPERATURE)
            errortext.append("Image Sensor temperature\n");
        if (error & ERROR_CAMERAINTERFACE)
            errortext.append("Camera Interface failure\n");
        if (error & ERROR_CAMERARAM)
            errortext.append("Camera RAM module failure\n");
        if (error & ERROR_CAMERAMAINBOARD)
            errortext.append("Camera Main Board failure\n");
        if (error & ERROR_CAMERAHEADBOARD)
            errortext.append("Camera Head Board failure\n");
    }
    if ((error + warn) != 0) {
        CameraHealth->setText(warning + errortext);
    }
    else CameraHealth->setText("OK");
    if (err != PCO_NOERROR) {  }
}

CameraDescription::~CameraDescription()
{
    temptimer->stop();
    temptimer->deleteLater();
}

void CameraDescription::RefreshTemperature() {
    err += camera->PCO_GetTemperature(&ccdtemp, &camtemp, &pstemp);
    if (camtemp) {
        QString TempDescription = "Temperature:";
        QString TempValues = "";
        TempValues.append(QString::number(camtemp));
        TempValues.append(QChar(0xb0)); // degree Symbol
        if (ccdtemp != (SHORT)(-32768)) {
            TempDescription.append("\nCCD temp:");
            TempValues.append("\n" + QString::number(((double)ccdtemp) / 10));
            TempValues.append(QChar(0xb0)); // degree Symbol
        }
        if (pstemp != (SHORT)(-32768)) {
            TempDescription.append("\nPower Supply temp:");
            TempValues.append("\n" + QString::number(pstemp));
            TempValues.append(QChar(0xb0)); // degree Symbol
        }
        TemperatureLabel->setText(TempDescription);
        Temperature->setText(TempValues);
    }
    else {
        Temperature->setText("Please wait...");
    }
}
