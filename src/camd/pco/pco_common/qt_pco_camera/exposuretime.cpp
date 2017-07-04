//-----------------------------------------------------------------//
// Name        | exposuretime.cpp            | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux, Windows                                    //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | Qt project part         -                         //
//-----------------------------------------------------------------//
// Author      | EIJ, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.00                                         //
//-----------------------------------------------------------------//
// Notes       | Exposure time dialog when recstate is [run]       //
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


#include "exposuretime.h"
#include "ui_exposuretime.h"

Exposuretime::Exposuretime(CPco_camera *camera, CPco_grabber* grabber, QWidget *parent) : QDialog(parent), ui(new Ui::Exposuretime) {
    ui->setupUi(this);
    this->camera = camera;
    this->grabber=grabber;
    camera->PCO_GetCameraDescription(&cs_description);
    UpdateTimingTab();

}

Exposuretime::~Exposuretime() {
    delete ui;
}

void Exposuretime::UpdateTimingTab() {
    DWORD delay, expos;
    WORD tb_expos, tb_delay;

    camera->PCO_GetDelayExposureTime(&delay, &expos, &tb_delay, &tb_expos);

    //set validator for exposure time box
    validexposure = new QIntValidator;
    validexposure->setRange(cs_description.dwMinExposureDESC, cs_description.dwMaxExposureDESC * 1000);
    ui->ExposureLineEdit->setValidator(validexposure);

    switch(tb_expos) {
    case TIMEBASE_MS:                 //Timebase display is in µs
        expos = expos*1000;
        break;
    case TIMEBASE_US:                 //Display in µs
        break;
    case TIMEBASE_NS:                 //ns to µs
        //no displaymode yet
        break;
    default:
        break;
    }

    switch(tb_delay) {
    case TIMEBASE_MS:
        delay = delay*1000;
        break;
    case TIMEBASE_US:                 //Display in µs
        break;
    case TIMEBASE_NS:
        //no displaymode yet     //ns to µs
        break;
    default:
        break;
    }

    //fill exposure time box
    QString exposurestring = QString::number((int) expos).rightJustified(10, '0');
    exposurestring.insert(2, ':');
    exposurestring.insert(5, ':');
    exposurestring.insert(9, ':');
    ui->ExposureCurrentValue->setText(exposurestring);

    ui->ExposureLineEdit->setText(QString::number((int) expos).rightJustified(10, '0'));

    //delay
    if ((cs_description.dwMaxDelayDESC + cs_description.dwMinDelayDESC) == 0) {
        //not possible, disable box
        ui->DelayLabel->setEnabled(FALSE);
        ui->DelayLineEdit->setEnabled(FALSE);
        ui->DelayCurrentValue->setEnabled(FALSE);
        ui->DelayCurrentValueLabel->setEnabled(FALSE);
        ui->DelayDescriptionLabel->setEnabled(FALSE);
    }
    else {
        validdelay = new QIntValidator(cs_description.dwMinDelayDESC, cs_description.dwMaxDelayDESC * 1000);
        ui->DelayLineEdit->setValidator(validdelay);
        ui->DelayLineEdit->setText(QString::number((int) delay).rightJustified(10,'0'));
        //fill exposure time box
        QString delaystring = QString::number((int)delay).rightJustified(10, '0');
        delaystring.insert(2, ':');
        delaystring.insert(5, ':');
        delaystring.insert(9, ':');
        ui->DelayCurrentValue->setText(delaystring);
    }
}

void Exposuretime::SetTimingTab() {
    DWORD delay, expos, timebase;
    WORD tb_expos, tb_delay;
    camera->PCO_GetDelayExposureTime(&delay, &expos, &tb_delay, &tb_expos);
    QString expstring = ui->ExposureLineEdit->text().remove(":");
    QString delaystring = ui->DelayLineEdit->text().remove(":");

    switch(tb_expos) {
    case TIMEBASE_MS:                 //Timebase display is in µs
        timebase = 1000;
        break;
    case TIMEBASE_US:                 //Display in µs
        timebase = 1;
        break;
    case TIMEBASE_NS:
        timebase = 1;    //Display in µs NYI
        break;
    default:
        timebase = 1;
        break;
    }
    //if ((expstring.toUInt()/timebase*1000*1000 >= cs_description.dwMinExposureDESC) && (expstring.toUInt()/timebase*1000 <= cs_description.dwMaxExposureDESC)) {
    expos = expstring.toUInt()/timebase;
    //}

    switch(tb_delay) {
    case TIMEBASE_MS:
        timebase = 1000;
        break;
    case TIMEBASE_US:                 //Display in µs
        timebase = 1;
        break;
    case TIMEBASE_NS:
        timebase = 1;     //Display in µs NYI
        break;
    default:
        timebase = 1;
        break;
    }

    if (ui->DelayLineEdit->isEnabled()) {
        //if ((delaystring.toUInt()/timebase >= cs_description.dwMinDelayDESC) && (delaystring.toUInt()/timebase <= cs_description.dwMaxDelayDESC)) {
        delay = delaystring.toUInt()/timebase;
        //}
    }
    DWORD err;

    err=camera->PCO_SetDelayExposureTime(delay, expos, tb_delay, tb_expos);

    if(err==PCO_NOERROR)
    {
        DWORD coc_sec,coc_nsec;
        int ImageTimeout;

        grabber->Get_Grabber_Timeout(&ImageTimeout);

        err=camera->PCO_GetCOCRuntime(&coc_sec,&coc_nsec);
        if((err==PCO_NOERROR)&&(coc_sec>0))
        {
         if((int)coc_sec*1000 > ImageTimeout-5000)
          ImageTimeout = 5000 + coc_sec*1000;
         grabber->Set_Grabber_Timeout(ImageTimeout);
        }
    }



}

void Exposuretime::on_ApplyButton_clicked()
{
    SetTimingTab();
    UpdateTimingTab();
}
