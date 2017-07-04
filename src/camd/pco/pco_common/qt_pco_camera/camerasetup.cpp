//-----------------------------------------------------------------//
// Name        | camerasetup.cpp             | Type: (*) source    //
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
// Notes       | Camera Setup Dialog                               //
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

#include "camerasetup.h"


CameraSetup::CameraSetup(CPco_camera* camera,CPco_grabber* grabber, QWidget *parent) : QDialog(parent) {
    ui.setupUi(this);
    this->validdelay=NULL;
    this->validexposure=NULL;
    this->rectyellow=NULL;
    this->rectgreen=NULL;
    this->camera = camera;
    this->grabber = grabber;
    camera->PCO_GetCameraDescription(&cs_description);
    camera->PCO_GetCameraType(&camtype,&serialnumber);

    if(  (camtype==CAMERATYPE_PCO_DIMAX_STD)
       ||(camtype==CAMERATYPE_PCO_DIMAX_TV)
       ||(camtype==CAMERATYPE_PCO_DIMAX_AUTOMOTIVE)
      )
    {
     cs_description.dwGeneralCaps1|=GENERALCAPS1_ROI_VERT_SYMM_TO_HORZ_AXIS;
     cs_description.dwGeneralCaps1|=GENERALCAPS1_ROI_HORZ_SYMM_TO_VERT_AXIS;
    }

    //Set ROI Scene & GraphicsView
    roiscene = new QGraphicsScene(1,1,cs_description.wMaxHorzResStdDESC,cs_description.wMaxVertResStdDESC);
    //roiscene->setBackgroundBrush(QBrush(Qt::green, Qt::SolidPattern));
    rectyellow = new QGraphicsRectItem(0,0,1,1);
    rectyellow->setPen(QPen(Qt::black));
    rectyellow->setBrush(QBrush(Qt::yellow));
    rectyellow->setZValue(1);   //stack on top of green
    roiscene->addItem(rectyellow);
    rectgreen = new QGraphicsRectItem(0,0,1,1);
    rectgreen->setPen(QPen(Qt::black));
    rectgreen->setBrush(QBrush(Qt::green));
    rectgreen->setZValue(0);   //to the back
    roiscene->addItem(rectgreen);
    ui.ROIView->setContentsMargins(QMargins(0,0,0,0));
    ui.ROIView->setScene(roiscene);
    ui.ROIView->setRenderHint(QPainter::Antialiasing,true);
    ui.ROIView->setBaseSize(roiscene->height(), roiscene->width());
    ui.ROIView->setSceneRect(roiscene->sceneRect());
    ui.ROIView->ensureVisible(roiscene->sceneRect());
    ui.ROIView->setScene(roiscene);
    ui.ROIView->fitInView(roiscene->sceneRect(), Qt::IgnoreAspectRatio);
    ui.ROIView->adjustSize();


    //disable all pushbuttons from getting triggered everytime enter is pressed (especially in the ROI lineedits)
    QList<QPushButton *> buttonList = findChildren<QPushButton *>();
    foreach(QPushButton *pb, buttonList) {
        pb->setDefault(false);
        pb->setAutoDefault(false);
    }

    UpdateSettings(); // get values
    ui.tabWidget->setCurrentIndex(0); //set to first tab
}

CameraSetup::~CameraSetup() {
    if(validdelay)
     delete validexposure;
    if(validdelay)
     delete validdelay;
    if(rectyellow)
     delete rectyellow;
    if(rectgreen)
     delete rectgreen;
    roiscene->deleteLater();
}

void CameraSetup::UpdateSettings() {
    UpdateTimingTab();
    UpdateSensorSizeTab();
    UpdateSensorMiscTab();
    UpdateRecordingTab();
}

void CameraSetup::SetTimingTab() {
    DWORD delay, expos, timebase;
    WORD tb_expos, tb_delay;
    camera->PCO_GetDelayExposureTime(&delay, &expos, &tb_delay, &tb_expos);
    QString expstring = ui.ExposureLineEdit->text().remove(":");
    QString delaystring = ui.DelayLineEdit->text().remove(":");

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

    if (ui.DelayLineEdit->isEnabled()) {
        //if ((delaystring.toUInt()/timebase >= cs_description.dwMinDelayDESC) && (delaystring.toUInt()/timebase <= cs_description.dwMaxDelayDESC)) {
        delay = delaystring.toUInt()/timebase;
        //}
    }

    camera->PCO_SetDelayExposureTime(delay, expos, tb_delay, tb_expos);

    //triggermode
    WORD triggermode;
    if (ui.AutoSequence->isChecked())
        triggermode = TRIGGER_MODE_AUTOTRIGGER;
    if (ui.SoftTrigger->isChecked())
        triggermode = TRIGGER_MODE_SOFTWARETRIGGER;
    if (ui.ExternalExpStart->isChecked())
        triggermode = TRIGGER_MODE_EXTERNALTRIGGER;
    if (ui.ExternalExpCtrl->isChecked())
        triggermode = TRIGGER_MODE_EXTERNALEXPOSURECONTROL;
    camera->PCO_SetTriggerMode(triggermode);

}

void CameraSetup::UpdateTimingTab(){
    DWORD delay, expos;
    WORD tb_expos, tb_delay, triggermode;

    camera->PCO_GetDelayExposureTime(&delay, &expos, &tb_delay, &tb_expos);

    //set validator for exposure time box
    validexposure = new QIntValidator;
    validexposure->setRange(cs_description.dwMinExposureDESC, cs_description.dwMaxExposureDESC * 1000);
    ui.ExposureLineEdit->setValidator(validexposure);

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
    ui.ExposureCurrentValue->setText(exposurestring);

    ui.ExposureLineEdit->setText(QString::number((int) expos).rightJustified(10, '0'));

    //delay
    if ((cs_description.dwMaxDelayDESC + cs_description.dwMinDelayDESC) == 0) {
        //not possible, disable box
        ui.DelayLabel->setEnabled(FALSE);
        ui.DelayLineEdit->setEnabled(FALSE);
        ui.DelayCurrentValue->setEnabled(FALSE);
        ui.DelayCurrentValueLabel->setEnabled(FALSE);
        ui.DelayDescriptionLabel->setEnabled(FALSE);
    }
    else
    {
        ui.DelayLineEdit->setEnabled(TRUE);
        validdelay = new QIntValidator;
        validdelay->setRange(cs_description.dwMinDelayDESC, cs_description.dwMaxDelayDESC * 1000);
//        ui.DelayLineEdit->setValidator(validdelay);

        //fill exposure time box
        QString delaystring = QString::number((int)delay).rightJustified(10, '0');
        delaystring.insert(2, ':');
        delaystring.insert(5, ':');
        delaystring.insert(9, ':');
        ui.DelayCurrentValue->setText(delaystring);

        ui.DelayLineEdit->setText(QString::number((int) delay).rightJustified(10,'0'));

    }

    //Trigger modes
    camera->PCO_GetTriggerMode(&triggermode);
    switch (triggermode) {
    case TRIGGER_MODE_AUTOTRIGGER:
        ui.AutoSequence->setChecked(TRUE);
        break;
    case TRIGGER_MODE_SOFTWARETRIGGER:
        ui.SoftTrigger->setChecked(TRUE);
        break;
    case TRIGGER_MODE_EXTERNALTRIGGER:
        ui.ExternalExpStart->setChecked(TRUE);
        break;
    case TRIGGER_MODE_EXTERNALEXPOSURECONTROL:
        ui.ExternalExpCtrl->setChecked(TRUE);
        break;
    default:
        break;
    }
    //if externalexpctrl not possible, disable button
    if (cs_description.dwGeneralCaps1 & GENERALCAPS1_NO_EXTEXPCTRL) {
        ui.ExternalExpCtrl->setEnabled(FALSE);
    }
}

void CameraSetup::SetSensorSizeTab() {
    //set binning
    WORD binhorz, binvert;
    binhorz = (ui.HorizontalBox->currentText().toInt());
    binvert = (ui.VerticalBox->currentText().toInt());
    camera->PCO_SetBinning(binhorz,binvert);

    //check ROI boundaries
    if (ui.StandardButton->isChecked()) {
        if((cs_description.wMaxHorzResStdDESC/binhorz)<(wRoiX1-wRoiX0)) {
            wRoiX0 = 1;
            wRoiX1 = cs_description.wMaxHorzResStdDESC/binhorz;
        }
        if((cs_description.wMaxVertResStdDESC/binvert)<(wRoiY1-wRoiY0)) {
            wRoiY0 = 1;
            wRoiY1 = cs_description.wMaxVertResStdDESC/binvert;
        }
    }
    if (ui.AlternativeButton->isChecked()) {
        if((cs_description.wMaxHorzResExtDESC/binhorz)<(wRoiX1-wRoiX0)) {
            wRoiX0 = 1;
            wRoiX1 = cs_description.wMaxHorzResExtDESC/binhorz;
        }
        if((cs_description.wMaxVertResExtDESC/binvert)<(wRoiY1-wRoiY0)) {
            wRoiY0 = 1;
            wRoiY1 = cs_description.wMaxVertResExtDESC/binvert;
        }
    }

    camera->PCO_SetROI(wRoiX0, wRoiY0, wRoiX1, wRoiY1);

    //set sensor format
    if (ui.StandardButton->isChecked()) {
        camera->PCO_SetSensorFormat(0); //Standard Format
        cam_width=cs_description.wMaxHorzResStdDESC;
    }
    if (ui.AlternativeButton->isChecked()) {
        camera->PCO_SetSensorFormat(1); //Alternative Format
        cam_width=cs_description.wMaxHorzResExtDESC;
    }


}

void CameraSetup::UpdateSensorSizeTab() {
    //Horizontal binning
    //clear box
    ui.HorizontalBox->clear();
    //get stepping
    if (cs_description.wBinHorzSteppingDESC == 0 || cs_description.wBinHorzSteppingDESC == 2) {  //binary step
        int j = 0;
        for (int i = 1; i <= cs_description.wMaxBinHorzDESC; i <<= 1 ) {
            ui.HorizontalBox->insertItem(j++, QString::number(i));
        }
    }
    if (cs_description.wBinHorzSteppingDESC == 1) { //linear step
        for (int i = 1; i <= cs_description.wMaxBinHorzDESC; i++) {
            ui.HorizontalBox->insertItem(i, QString::number(i));
        }
    }

    //Vertical binning
    //clear box
    ui.VerticalBox->clear();
    //get stepping
    if (cs_description.wBinVertSteppingDESC == 0 || cs_description.wBinHorzSteppingDESC == 2) {  //binary step
        int j = 0;
        for (int i = 1; i <= cs_description.wMaxBinVertDESC; i <<= 1) {
            ui.VerticalBox->insertItem(j++, QString::number(i));
        }
    }
    if (cs_description.wBinVertSteppingDESC == 1) { //linear step
        for (int i = 1; i <= cs_description.wMaxBinVertDESC; i++) {
            ui.VerticalBox->insertItem(i, QString::number(i));
        }
    }
    //boxes filled, get current binning
    WORD binhorz, binvert;
    camera->PCO_GetBinning(&binhorz,&binvert);
    ui.HorizontalBox->setCurrentText(QString::number(binhorz));
    ui.VerticalBox->setCurrentText(QString::number(binvert));


    //sensor format
    camera->PCO_GetSensorFormat(&sensor);
    ui.StandardFormat->setText(QString::number(cs_description.wMaxHorzResStdDESC) + " x " + QString::number(cs_description.wMaxVertResStdDESC));
    ui.AlternativeFormat->setText(QString::number(cs_description.wMaxHorzResExtDESC) + " x " + QString::number(cs_description.wMaxVertResExtDESC));
    if ((cs_description.wMaxHorzResExtDESC!=cs_description.wMaxHorzResStdDESC) || (cs_description.wMaxVertResExtDESC!=cs_description.wMaxVertResStdDESC)) {
        ui.AlternativeFormat->setEnabled(TRUE);
        ui.AlternativeButton->setEnabled(TRUE);
    }
    else {
        ui.AlternativeFormat->setEnabled(FALSE);
        ui.AlternativeButton->setEnabled(FALSE);
    }
    if (sensor == 0) { // Standard
        ui.StandardButton->setChecked(TRUE);
        ui.AlternativeButton->setChecked(FALSE);
    }
    if (sensor == 1) { // alternative
        ui.AlternativeButton->setChecked(TRUE);
        ui.StandardButton->setChecked(FALSE);
    }

    if((cs_description.wRoiHorStepsDESC==0)||(cs_description.wRoiVertStepsDESC==0)) {
        ui.ROIFrame->setEnabled(false);
    }

    //if ROI must be sym. to horz/vert axis
    if(cs_description.dwGeneralCaps1 & GENERALCAPS1_ROI_HORZ_SYMM_TO_VERT_AXIS) {
        //disable ui buttons
        ui.ROIx1->setEnabled(false);
        //connect the two buttons
        connect(ui.ROIx0,SIGNAL(valueChanged(int)),this,SLOT(setROIx1sym(int)));
    }
    if(cs_description.dwGeneralCaps1 & GENERALCAPS1_ROI_VERT_SYMM_TO_HORZ_AXIS) {
        //disable ui buttons
        ui.ROIy1->setEnabled(false);
        //connect the two buttons
        connect(ui.ROIy0,SIGNAL(valueChanged(int)),this,SLOT(setROIy1sym(int)));
    }

    //if ROI has stepping
    if(cs_description.wRoiHorStepsDESC) {
        ui.ROIx0->setSingleStep(cs_description.wRoiHorStepsDESC);
        ui.ROIx1->setSingleStep(cs_description.wRoiHorStepsDESC);
    } else { // if stepping is zero, ROI other than max is not possible
        ui.ROIx0->setEnabled(false);
        ui.ROIx1->setEnabled(false);
    }

    if(cs_description.wRoiHorStepsDESC) {
        ui.ROIy0->setSingleStep(cs_description.wRoiVertStepsDESC);
        ui.ROIy1->setSingleStep(cs_description.wRoiVertStepsDESC);
    } else { // if stepping is zero, ROI other than max is not possible
        ui.ROIy0->setEnabled(false);
        ui.ROIy1->setEnabled(false);
    }

    camera->PCO_GetROI(&wRoiX0, &wRoiY0, &wRoiX1, &wRoiY1);
    //set ROI input limiters
    if(sensor == 0) {
        //if symmetrical ROI, set max to half resolution
        if(cs_description.dwGeneralCaps1 & GENERALCAPS1_ROI_VERT_SYMM_TO_HORZ_AXIS) {
            ui.ROIx0->setMaximum(cs_description.wMaxHorzResStdDESC/2);
        } else {
            ui.ROIx0->setMaximum(cs_description.wMaxHorzResStdDESC);
        }
        //if symmetrical ROI, set max to half resolution
        if(cs_description.dwGeneralCaps1 & GENERALCAPS1_ROI_HORZ_SYMM_TO_VERT_AXIS) {
            ui.ROIy0->setMaximum(cs_description.wMaxVertResStdDESC/2);
        } else {
            ui.ROIy0->setMaximum(cs_description.wMaxVertResStdDESC);
        }
        ui.ROIy0->setMaximum(cs_description.wMaxVertResStdDESC);
        ui.ROIx1->setMaximum(cs_description.wMaxHorzResStdDESC/binhorz);
        ui.ROIy1->setMaximum(cs_description.wMaxVertResStdDESC/binvert);
        //set green rectangle
        rectgreen->setRect(1,1,cs_description.wMaxHorzResStdDESC/binhorz,cs_description.wMaxVertResStdDESC/binvert);
        //roiscene->setSceneRect(1,1,cs_description.wMaxHorzResStdDESC/binhorz,cs_description.wMaxVertResStdDESC/binvert);
    }
    if(sensor == 1) {
        //if symmetrical ROI, set max to half resolution
        if(cs_description.dwGeneralCaps1 & GENERALCAPS1_ROI_VERT_SYMM_TO_HORZ_AXIS) {
            ui.ROIx0->setMaximum(cs_description.wMaxHorzResExtDESC/2);
        } else {
            ui.ROIx0->setMaximum(cs_description.wMaxHorzResExtDESC);
        }
        //if symmetrical ROI, set max to half resolution
        if(cs_description.dwGeneralCaps1 & GENERALCAPS1_ROI_HORZ_SYMM_TO_VERT_AXIS) {
            ui.ROIy0->setMaximum(cs_description.wMaxHorzResExtDESC/2);
        } else {
            ui.ROIy0->setMaximum(cs_description.wMaxHorzResExtDESC);
        }
        ui.ROIy0->setMaximum(cs_description.wMaxVertResExtDESC);
        ui.ROIx1->setMaximum(cs_description.wMaxHorzResExtDESC/binhorz);
        ui.ROIy1->setMaximum(cs_description.wMaxVertResExtDESC/binvert);
        //set green rectangle
        rectgreen->setRect(1,1,cs_description.wMaxHorzResExtDESC/binhorz,cs_description.wMaxVertResExtDESC/binvert);
        //roiscene->setSceneRect(1,1,cs_description.wMaxHorzResExtDESC/binhorz,cs_description.wMaxVertResExtDESC/binvert);
    }

    updateROIwindow();
}

void CameraSetup::updateROIwindow()
{
    //set boxes
    ui.ROIx0->setValue(wRoiX0);
    ui.ROIy0->setValue(wRoiY0);
    ui.ROIx1->setValue(wRoiX1);
    ui.ROIy1->setValue(wRoiY1);
    ui.ActualResolution->setText(QString::number(wRoiX1-wRoiX0+1)+" x "+QString::number(wRoiY1-wRoiY0+1));
    rectyellow->setRect(wRoiX0,wRoiY0,wRoiX1-wRoiX0,wRoiY1-wRoiY0);
    //ui.ROIView->setBaseSize(roiscene->height(), roiscene->width());
    //ui.ROIView->adjustSize();
    ui.ROIView->fitInView(1, 1, roiscene->width(), roiscene->height());
    roiscene->update();
    ui.ROIView->update();
}

void CameraSetup::setROIx1sym(int value) {
    if(sensor == 1) {
        wRoiX1 = cs_description.wMaxHorzResStdDESC-value+1;
        ui.ROIy1->setValue(wRoiX1);
    }
    if(sensor == 0) {
        wRoiX1 = cs_description.wMaxHorzResStdDESC-value+1;
        ui.ROIx1->setValue(wRoiX1);
    }
}

void CameraSetup::setROIy1sym(int value) {
    if(sensor == 1) {
        wRoiY1 = cs_description.wMaxVertResStdDESC-value+1;
        ui.ROIy1->setValue(wRoiY1);
    }
    if(sensor == 0) {
        wRoiY1 = cs_description.wMaxVertResStdDESC-value+1;
        ui.ROIy1->setValue(wRoiY1);
    }
}

void CameraSetup::SetSensorMiscTab() {
    //set AD converter
    if (ui.ADConverter->isEnabled()) {
        DWORD adcoperation;
        if (ui.ADConverterOne->isChecked())
            adcoperation = 1;
        else if (ui.ADConverterTwo->isChecked())
            adcoperation = 2;
        else
            adcoperation = 1;
        camera->PCO_SetADCOperation(adcoperation);
    }
    //set double image mode
    if (ui.DoubleImage->isEnabled()) {
        WORD mode;
        if (ui.DoubleImageOn->isChecked())
            mode = 1;
        else if (ui.DoubleImageOff->isChecked())
            mode = 0;
        else
            mode = 0;
        camera->PCO_SetDoubleImageMode(mode);
    }
    //set IR sensitivity
    if (ui.IRSensitivity->isEnabled()) {
        WORD IRsens;
        if (ui.IRSensitivityOn->isChecked())
            IRsens = 1;
        else if (ui.IRSensitivityOff->isChecked())
            IRsens = 0;
        else IRsens = 0;
        camera->PCO_SetIRSensitivity(IRsens);
    }
    //set offset control
    if (ui.OffsetControl->isEnabled()) {
        WORD mode;
        if (ui.OffsetControlAuto->isChecked())
            mode = OFFSET_MODE_AUTO;
        else if (ui.OffsetControlOff->isChecked())
            mode = OFFSET_MODE_OFF;
        else
            mode = OFFSET_MODE_AUTO;
        camera->PCO_SetOffsetMode(mode);
    }

    //set BW Noise Filter
    WORD mode;
    QString noisefiltertext = ui.BWNoiseFilter->currentText();
    if (!noisefiltertext.compare("on"))
        mode = NOISE_FILTER_MODE_ON;
    if (!noisefiltertext.compare("off"))
        mode = NOISE_FILTER_MODE_OFF;
    if (!noisefiltertext.compare("on + HotPixel Correction"))
        mode = NOISE_FILTER_MODE_REMOVE_HOT_DARK + NOISE_FILTER_MODE_ON;
    camera->PCO_SetNoiseFilterMode(mode);

    //set pixelrate
    DWORD pixelrate;
    QString currentrate = ui.PixelclockBox->currentText();
    pixelrate = currentrate.toInt()*1000000;
    //get closest match for pixelrate
    int x;
    for(x=0;x<4;x++)
    {
        if((pixelrate<cs_description.dwPixelRateDESC[x]+1000*1000)&&(pixelrate>cs_description.dwPixelRateDESC[x]-1000*1000))
        {
            //use exact pixelrate from descriptor
            pixelrate=cs_description.dwPixelRateDESC[x];
            break;
        }
    }
    //check if we had a match
    if(x<4) {
        camera->PCO_SetPixelRate(pixelrate);
    }
    else {
        //this should never happen
    }

    if(camtype ==CAMERATYPE_PCO_EDGE) {
        DWORD lut = 0;
        //Extra settings for Edge 5.5 rolling shutter
        PCO_SC2_CL_TRANSFER_PARAM clpar;
        camera->PCO_GetTransferParameter(&clpar,sizeof(clpar));
        if((cam_width>1920)&&(pixelrate>=286000000))
        {
            clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x12;
            lut = 0x1612;
        }
        else
        {
            clpar.DataFormat=SCCMOS_FORMAT_TOP_CENTER_BOTTOM_CENTER|PCO_CL_DATAFORMAT_5x16;
        }
        grabber->Set_DataFormat(clpar.DataFormat);
        camera->PCO_SetTransferParameter(&clpar,sizeof(clpar));
        camera->PCO_SetLut(lut,0);
    }

    //set conversion factor
    DWORD convfact;
    QString currentconvfact = ui.ConversionFactorBox->currentText();
    convfact = (int) (currentconvfact.toDouble()*100);
    camera->PCO_SetConversionFactor(convfact);
}

void CameraSetup::UpdateSensorMiscTab() {
    //AD Converter
    switch (cs_description.wNumADCsDESC) {
    case 0:
        ui.ADConverter->setEnabled(FALSE);
    case 1:
        ui.ADConverterOne->setChecked(TRUE);
        ui.ADConverter->setEnabled(FALSE);
        break;
    case 2:
        WORD adcoperation;
        camera->PCO_GetADCOperation(&adcoperation);
        if (adcoperation == 1)
            ui.ADConverterOne->setChecked(TRUE);
        if (adcoperation == 2)
            ui.ADConverterTwo->setChecked(TRUE);
        break;
    }
    //Double Image
    if (cs_description.wDoubleImageDESC == 1) {
        WORD mode;
        camera->PCO_GetDoubleImageMode(&mode);
        if (mode == DOUBLE_IMAGE_MODE_ON)
            ui.DoubleImageOn->setChecked(TRUE);
        else if (mode == DOUBLE_IMAGE_MODE_OFF)
            ui.DoubleImageOff->setChecked(TRUE);
    }
    else {
        ui.DoubleImage->setEnabled(FALSE);
        ui.DoubleImageOff->setChecked(TRUE);
    }
    //IR Sensitivity
    if (cs_description.wIRDESC == 1) {
        WORD mode;
        camera->PCO_GetIRSensitivity(&mode);
        if (mode == 1)
            ui.IRSensitivityOn->setChecked(TRUE);
        else if (mode == 0)
            ui.IRSensitivityOff->setChecked(TRUE);
    }
    else {
        ui.IRSensitivityOff->setChecked(TRUE);
        ui.IRSensitivity->setEnabled(FALSE);
    }

    //Offset Control
    if (cs_description.wOffsetRegulationDESC == 1) {
        WORD mode;
        camera->PCO_GetOffsetMode(&mode);
        if (mode == OFFSET_MODE_OFF) {
            ui.OffsetControlOff->setChecked(TRUE);
        }
        else if (mode == OFFSET_MODE_AUTO) {
            ui.OffsetControlAuto->setChecked(TRUE);
        }
    }
    else  {
        ui.OffsetControl->setEnabled(FALSE);
        ui.OffsetControlAuto->setChecked(TRUE);
    }

    //B/W Noise Filter
    if (cs_description.dwGeneralCaps1 & GENERALCAPS1_NOISE_FILTER) {
        WORD mode;
        camera->PCO_GetNoiseFilterMode(&mode);
        ui.BWNoiseFilter->clear();
        ui.BWNoiseFilter->addItem("off");
        ui.BWNoiseFilter->addItem("on");
        if (cs_description.dwGeneralCaps1 & GENERALCAPS1_HOTPIX_ONLY_WITH_NOISE_FILTER) {
            ui.BWNoiseFilter->addItem("on + HotPixel Correction");
        }
        switch (mode) {
        case NOISE_FILTER_MODE_OFF:
            ui.BWNoiseFilter->setCurrentIndex(0); // on
            break;
        case NOISE_FILTER_MODE_ON:
            ui.BWNoiseFilter->setCurrentIndex(1); // off
            break;
        case NOISE_FILTER_MODE_REMOVE_HOT_DARK+NOISE_FILTER_MODE_ON:
            ui.BWNoiseFilter->setCurrentIndex(2);
            break;
        default:
            break;
        }
    }
    else
        ui.BWNoise->setEnabled(FALSE);


    DWORD pixelrate;
    //fill pixel rate dropdown menu
    ui.PixelclockBox->clear();
    for (int x = 0; x<4; x++)
    {
        if (cs_description.dwPixelRateDESC[x] != 0) {
            //add item to dropdown menu
            ui.PixelclockBox->insertItem(x, QString::number(cs_description.dwPixelRateDESC[x] / 1000000));
        }
        else
            break;
    }

    //display current pixelrate
    camera->PCO_GetPixelRate(&pixelrate);
    ui.PixelclockBox->setCurrentText(QString::number(pixelrate / 1000000));

    //deactivate box if only 1 pixelrate is available
    if(ui.PixelclockBox->count()==1) {
        ui.PixelclockBox->setEnabled(0);
    }

    //display imagerate
    DWORD time_s, time_ns;
    double tim;
    QString imagerate = "";

    camera->PCO_GetCOCRuntime(&time_s, &time_ns);
    tim = time_ns;
    tim /= 1000000000;
    tim += time_s;
    imagerate.append(QString::number(1 / tim,'g',4));
    imagerate.append(" Hz, ");
    imagerate.append(QString::number(tim * 1000));
    imagerate.append(" ms");
    ui.Imagerate->setText(imagerate);

    //Conversion Factor
    ui.ConversionFactorBox->clear();
    QString convfacttext;
    for (int x = 0; x < 4; x++)
    {
        if (cs_description.wConvFactDESC[x] != 0) {
            convfacttext.sprintf("%.2f", (double) cs_description.wConvFactDESC[x]/100);
            ui.ConversionFactorBox->insertItem(x, convfacttext);
        }
        else
            break;
    }
    WORD convfact;
    camera->PCO_GetConversionFactor(&convfact);
    convfacttext.sprintf("%.2f", (double) convfact/100);
    ui.ConversionFactorBox->setCurrentText(convfacttext);

    //deactivate box if only 1 pixelrate is available
    if(ui.ConversionFactorBox->count()==1) {
        ui.ConversionFactorBox->setEnabled(0);
    }
}

void CameraSetup::SetRecordingTab() {
    //set recorder mode
    WORD recSubmode;
    if (ui.RecorderModeRingbuffer->isChecked()) {
        recSubmode = RECORDER_SUBMODE_RINGBUFFER;
    }
    if (ui.RecorderModeSequence->isChecked()) {
        recSubmode = RECORDER_SUBMODE_SEQUENCE;
    }
    camera->PCO_SetRecorderSubmode(recSubmode);
    //set timestamp if available
    if (!(cs_description.dwGeneralCaps1 & GENERALCAPS1_NO_TIMESTAMP)) {
        WORD timeStampmode;
        if (ui.TimestampNostamp->isChecked())
            timeStampmode = TIMESTAMP_MODE_OFF;
        if (ui.TimestampBinary->isChecked())
            timeStampmode = TIMESTAMP_MODE_BINARY;
        if (ui.TimestampBinaryASCII->isChecked())
            timeStampmode = TIMESTAMP_MODE_BINARYANDASCII;
        if (ui.TimestampASCII->isChecked())
            timeStampmode = TIMESTAMP_MODE_ASCII;
        camera->PCO_SetTimestampMode(timeStampmode);
    }
    //set acquire mode
    if (!(cs_description.dwGeneralCaps1 & GENERALCAPS1_NO_ACQUIREMODE)) {
        WORD acqmode;
        if (ui.AcquireModeAuto->isChecked())
            acqmode = ACQUIRE_MODE_AUTO;
        if (ui.AcquireModeExternal->isChecked())
            acqmode = ACQUIRE_MODE_EXTERNAL;
        if (ui.AcquireModeExtTrigger->isChecked())
            acqmode = ACQUIRE_MODE_EXTERNAL_FRAME_TRIGGER;
        camera->PCO_SetAcquireMode(acqmode);
    }

}

void CameraSetup::UpdateRecordingTab() {
    WORD recSubmode;
    camera->PCO_GetRecorderSubmode(&recSubmode);
    switch (recSubmode) {
    case 0:
        ui.RecorderModeSequence->setChecked(TRUE);
        break;
    case 1:
        ui.RecorderModeRingbuffer->setChecked(TRUE);
        break;
    default:
        break;
    }
    //check for timestamp availabilty
    if (!(cs_description.dwGeneralCaps1 & GENERALCAPS1_NO_TIMESTAMP)) {
        WORD timeStampmode;
        camera->PCO_GetTimestampMode(&timeStampmode);
        switch (timeStampmode) {
        case 0:
            ui.TimestampNostamp->setChecked(TRUE);
            break;
        case 1:
            ui.TimestampBinary->setChecked(TRUE);
            break;
        case 2:
            ui.TimestampBinaryASCII->setChecked(TRUE);
            break;
        case 3:
            ui.TimestampASCII->setChecked(TRUE);
            break;
        default:
            break;
        }
        //disable ASCII only if not possible
        if (!(cs_description.dwGeneralCaps1 & GENERALCAPS1_TIMESTAMP_ASCII_ONLY)) {
            ui.TimestampASCII->setEnabled(FALSE);
        }
    }
    else { //disable timestamps
        ui.Timestamp->setEnabled(FALSE);
    }

    //check for acquire mode
    if (!(cs_description.dwGeneralCaps1 & GENERALCAPS1_NO_ACQUIREMODE)) {
        WORD acqumode;
        camera->PCO_GetAcquireMode(&acqumode);
        switch (acqumode) {
        case (ACQUIRE_MODE_AUTO):
            ui.AcquireModeAuto->setChecked(TRUE);
            break;
        case (ACQUIRE_MODE_EXTERNAL):
            ui.AcquireModeExternal->setChecked(TRUE);
            break;
        case(ACQUIRE_MODE_EXTERNAL_FRAME_TRIGGER) :
            ui.AcquireModeExtTrigger->setChecked(TRUE);
            break;
        default:
            break;
        }
    }
    else {
        ui.AcquireModeAuto->setChecked(TRUE);
        ui.AcquireMode->setEnabled(FALSE);
    }
}

/* ROI LineEdits and Buttons */
void CameraSetup::on_ROIx0_editingFinished() {
    if(ui.ROIx0->value() > ui.ROIx1->value()) {
        //cap at upper value
        wRoiX0 = ui.ROIx1->value();
    } else {
        wRoiX0 = ui.ROIx0->value();
        //adjust ROI stepping, lower value starts at 1
        if(cs_description.wRoiHorStepsDESC > 1) {
            wRoiX0= ((wRoiX0/cs_description.wRoiHorStepsDESC)*cs_description.wRoiHorStepsDESC)+1;
        }
    }
    updateROIwindow();
}

void CameraSetup::on_ROIx1_editingFinished() {
    if(ui.ROIx1->value() < ui.ROIx0->value()) {
        //cap at lower value
        wRoiX1 = ui.ROIx0->value();
    } else {
        wRoiX1 = ui.ROIx1->value();
        //adjust ROI stepping
        if(cs_description.wRoiHorStepsDESC > 1) {
            wRoiX1= (wRoiX1/cs_description.wRoiHorStepsDESC)*cs_description.wRoiHorStepsDESC;
        }
    }
    updateROIwindow();
}


void CameraSetup::on_ROIy0_editingFinished() {
    if(ui.ROIy0->value() > ui.ROIy1->value()) {
        //cap at upper value
        wRoiY0 = ui.ROIy1->value();
    } else {
        wRoiY0 = ui.ROIy0->value();
        //adjust ROI stepping, lower value starts at 1
        if(cs_description.wRoiVertStepsDESC > 1) {
            wRoiY0= ((wRoiY0/cs_description.wRoiVertStepsDESC)*cs_description.wRoiVertStepsDESC)+1;
        }
    }
    updateROIwindow();
}


void CameraSetup::on_ROIy1_editingFinished() {
    if(ui.ROIy1->value() < ui.ROIy0->value()) {
        //cap at lower value
        wRoiY1 = ui.ROIy0->value();
    } else {
        wRoiY1 = ui.ROIy1->value();
        //adjust ROI stepping
        if(cs_description.wRoiVertStepsDESC > 1) {
            wRoiY1= (wRoiY1/cs_description.wRoiVertStepsDESC)*cs_description.wRoiVertStepsDESC;
        }
    }
    updateROIwindow();
}

void CameraSetup::on_ROImaxbutton_clicked() {
    wRoiX0 = wRoiY0 = 1;
    if(ui.AlternativeButton->isChecked()) {
        wRoiX1 = cs_description.wMaxHorzResExtDESC/ui.HorizontalBox->currentText().toInt();
        wRoiY1 = cs_description.wMaxVertResExtDESC/ui.VerticalBox->currentText().toInt();
    }
    if(ui.StandardButton->isChecked()) {
        wRoiX1 = cs_description.wMaxHorzResStdDESC/ui.HorizontalBox->currentText().toInt();
        wRoiY1 = cs_description.wMaxVertResStdDESC/ui.VerticalBox->currentText().toInt();
    }
    updateROIwindow();
}

void CameraSetup::on_ApplyButton_clicked()
{
    DWORD err;
    WORD recstate;

    SetTimingTab();
    SetSensorSizeTab();
    SetSensorMiscTab();
    SetRecordingTab();
    err = camera->PCO_GetRecordingState(&recstate);

    if (recstate == RUN)
    {
        recstate = STOP;
        err = camera->PCO_SetRecordingState(recstate);
    }

    if(err==PCO_NOERROR)
    {

        err = camera->PCO_ArmCamera();
        if (err != PCO_NOERROR)
        {
            ui.StatusText->setText("camera Error:" + QString::asprintf("0x%x",err));
        }
    }

    if(err==PCO_NOERROR)
    {
        err = grabber->PostArm();
        if (err != PCO_NOERROR)
        {
            ui.StatusText->setText("grabber Error:" + QString::asprintf("0x%x",err));
        }
    }

    if(err==PCO_NOERROR)
    {
        DWORD coc_sec,coc_nsec;
        int ImageTimeout;

        grabber->Get_Grabber_Timeout(&ImageTimeout);

        err=camera->PCO_GetCOCRuntime(&coc_sec,&coc_nsec);
        if((err==PCO_NOERROR)&&(coc_sec>0))
        {
         if((int)coc_sec*1000 > ImageTimeout-5000)
          ImageTimeout=5000 + coc_sec*1000;
         grabber->Set_Grabber_Timeout(ImageTimeout);
        }
    }

    if(err==PCO_NOERROR)
    {
     ui.StatusText->setText("OK");
     QTimer::singleShot(1000, this, SLOT(SetCheckboxToIdle()));
    }

    UpdateSettings();
}

void CameraSetup::showEvent(QShowEvent * event)
{
    ui.ROIView->fitInView(1,1,roiscene->width(),roiscene->height(), Qt::IgnoreAspectRatio);
    QWidget::showEvent(event);
}

void CameraSetup::SetCheckboxToIdle() {
    ui.StatusText->setText("Idle");
}

void CameraSetup::on_tabWidget_tabBarClicked(int index)
{
    //maximize view for sensor (size) tab
    if(index == 1) {
        ui.ROIView->fitInView(1, 1, roiscene->width(), roiscene->height());
    }
}

void CameraSetup::resizeEvent(QResizeEvent *) {
    ui.ROIView->fitInView(1, 1, roiscene->width(), roiscene->height());
}
