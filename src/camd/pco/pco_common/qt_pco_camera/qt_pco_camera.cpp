//-----------------------------------------------------------------//
// Name        | qt_pco_camera.cpp           | Type: (*) source    //
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
// Author      | MBL,EIJ, PCO AG                                   //
//-----------------------------------------------------------------//
// Revision    | rev. 1.00                                         //
//-----------------------------------------------------------------//
// Notes       | Main window                                       //
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

#include "qt_pco_camera.h"

/* GLOBALS */
GrabThreadWorker* grabthreadworker;
LiveThreadWorker* livethreadworker;
ErrorPopup* errorwindow;

qt_pco_camera::qt_pco_camera( CPco_Log* Clog,CPco_camera* camera,CPco_grabber* grabber,QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // reset variables to default
    camera_is_open = 0;
    camera_is_running = 0;
    act_recstate = 0;
    images_available = 0;
    current_image = 0;
    liveview_isrunning = 0;
    board = 0;
    doubleimagemode = 0;
    convertmin = 0;
    convertmax = 15000;
    convertgamma = 1;
    single_image = NULL;
    current_image_buffer = NULL;
    current_image_converted = NULL;
    err = PCO_NOERROR;
    lut = NULL;
    ImageTimeout=5000;

    //Set Title
    QString title = "CameraDemoQT";
#ifdef MAINWINDOWTITLE
    title.append(" - ");
    title.append(MAINWINDOWTITLE);
#endif
    this->setWindowTitle(title);
    //set camera&grabber
    this->camera = camera;
    this->grabber = grabber;
    mylog=Clog;
    //set errorpopup log
    errorwindow = new ErrorPopup();
    errorwindow->setlog(Clog);
    //disable all the buttons
    SetControlButtonStatus(FALSE);
    SetImageGrabButtonStatus(FALSE);
    //SetImageConvertOptionsButtonStatus(FALSE);
    SetImageScrollButtonStatus(FALSE);
    //enable open cam button
    ui.OpenCameraButton->setEnabled(TRUE);
    scene = new QGraphicsScene(this);
    ui.ImageView->setScene(scene);
    ui.Toolbar->setContextMenuPolicy(Qt::CustomContextMenu);
}

qt_pco_camera::~qt_pco_camera()
{
    if (grabthreadworker) {
        if (grabthreadworker->isRunning()) {
            //ask nicely and wait max 150 ms (2 pictures worst case, ~74 ms per picture)
            grabthreadworker->stopGrabbing();
            if (!grabthreadworker->wait(150)) {
                //thread didn't finish in 150 ms, quit forcefully
                grabthreadworker->exit();
                grabthreadworker->deleteLater();
            }
        }
    }

    if (livethreadworker) {
        if (livethreadworker->isRunning()) {
            //ask nicely and wait max 80 ms (finish current picture)
            livethreadworker->stopLiveView();
            if (!livethreadworker->wait(80)) {
                //thread didn't finish in 80 ms, quit forcefully
                livethreadworker->exit();
                livethreadworker->deleteLater();
            }
        }
    }
    scene->clear();
    FreeImageVector();

    if(single_image)
        free(single_image);

    if(current_image_converted)
        free(current_image_converted);

    if(lut) {
      pcocnv_delete_bwlut(lut);
    }

    if(grabber)
    {
     grabber->Close_Grabber();
     delete grabber;
     grabber=NULL;
    }

    if(camera)
    {
     camera->Close_Cam();
     delete camera;
     camera=NULL;
    }

    scene->deleteLater();
}

void qt_pco_camera::on_OpenCameraButton_clicked()
{
    if (camera_is_open)
    {
        if (camera_is_running)
        {
            errorwindow->SetError("Stop camera first!", err);
            errorwindow->exec();
            return;
        }
        if(grabber)
        {
         grabber->Close_Grabber();
         delete grabber;
         grabber=NULL;
        }

        if(camera)
        {
         camera->Close_Cam();
         delete camera;
         camera=NULL;
        }

        camera_is_open = 0;
        ui.OpenCameraButton->setText("Open Camera");
        SetControlButtonStatus(FALSE);
        //SetImageConvertOptionsButtonStatus(FALSE);
        SetImageGrabButtonStatus(FALSE);
        ui.OpenCameraButton->setEnabled(TRUE);
        return;
    }

    //set wait cursor
    this->setCursor(Qt::WaitCursor);
    QApplication::processEvents();

    camera= new CPco_camera();
    if(camera==NULL)
    {
        errorwindow->SetError("Could not create Camera");
        errorwindow->exec();
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    camera->SetLog(mylog);

    err = camera->Open_Cam(board);
    if (err != PCO_NOERROR)
    {
        delete camera;
        camera=NULL;

        errorwindow->SetError("Could not open Camera.Please make sure that the Camera is connected and turned on.", err);
        errorwindow->exec();
        this->setCursor(Qt::ArrowCursor);
        return;
    }

    err=camera->PCO_GetCameraType(&camtype,&serialnumber,&Interfacetype);
    if(err!=PCO_NOERROR)
    {
        camera->Close_Cam();
        delete camera;
        camera=NULL;
        errorwindow->SetError("Could not open Camera.Please make sure that the Camera is connected and turned on.", err);
        errorwindow->exec();
        this->setCursor(Qt::ArrowCursor);
        return;
    }

#ifdef CPCO_GRAB_CL_ME4_H
    if(Interfacetype==INTERFACE_CAMERALINK)
    {
     if(camtype==CAMERATYPE_PCO_EDGE)
     {
      grabber=new CPco_grab_cl_me4_edge((CPco_com_cl_me4*)camera);
     }
     else if(camtype==CAMERATYPE_PCO_EDGE_42)
     {
      grabber=new CPco_grab_cl_me4_edge42((CPco_com_cl_me4*)camera);
     }
     else if(camtype==CAMERATYPE_PCO_EDGE_GL)
     {
      grabber=new CPco_grab_cl_me4_edge_GL((CPco_com_cl_me4*)camera);
     }
     else
     {
      grabber=new CPco_grab_cl_me4_camera((CPco_com_cl_me4*)camera);
     }
    }
#endif

#ifdef CPCO_GRAB_USB_H
    if(Interfacetype==INTERFACE_USB)
    {
     grabber=new CPco_grab_usb((CPco_com_usb*)camera);
    }
    else if(Interfacetype==INTERFACE_USB3)
    {
     grabber=new CPco_grab_usb((CPco_com_usb*)camera);
    }
#endif

    if(grabber==NULL)
    {
        camera->Close_Cam();
        delete camera;
        camera=NULL;


        errorwindow->SetError("Could not create Grabber");
        errorwindow->exec();
        this->setCursor(Qt::ArrowCursor);
        return;
    }
    grabber->SetLog(mylog);


    err = grabber->Open_Grabber(board); // open grabber
    if (err != PCO_NOERROR)
    {
        errorwindow->SetError("Could not open Grabber. Please make sure that the Camera is connected and turned on.", err);
        errorwindow->exec();
        delete grabber;
        grabber=NULL;

        camera->Close_Cam();
        delete camera;
        camera=NULL;

        this->setCursor(Qt::ArrowCursor);
        return;
    }

    err = camera->PCO_GetCameraDescriptor(&description);
    bitpix = description.wDynResDESC;
    if (err != PCO_NOERROR)
    {
        errorwindow->SetError("Error getting Camera Description",err);
    }

    if (err == PCO_NOERROR)
    {
     err = camera->PCO_GetRecordingState(&act_recstate);
     if (err != PCO_NOERROR)
     {
        errorwindow->SetError("Get Recstate failed.");
     }
    }

    if (err == PCO_NOERROR)
    {
        if (act_recstate == RUN)
        {
            act_recstate = STOP;
            camera->PCO_SetRecordingState(act_recstate);
        }
    }

    //reset camera and set some default settings
    if (err == PCO_NOERROR)
    {
     err = camera->PCO_ResetSettingsToDefault();
     if (err != PCO_NOERROR)
     {
        errorwindow->SetError("PCO_ResetSettingsToDefault() failed.");
     }
    }

    if (err == PCO_NOERROR)
    {
     err=camera->PCO_SetCameraToCurrentTime();
     if (err != PCO_NOERROR)
     {
         errorwindow->SetError("PCO_SetCameraToCurrentTime() failed.");
     }
    }

    //create lookuptable for image conversion
    lut = pcocnv_create_bwlut(bitpix, 0, 255);

    if (err == PCO_NOERROR)
    {
     err = SetInitialValues();
     if (err != PCO_NOERROR)
     {
        errorwindow->SetError("Setting initial values failed.");
     }
    }

    // Don't change settings after ARM without calling ARM again
    if (err == PCO_NOERROR)
    {
     err = camera->PCO_ArmCamera();
     if (err != PCO_NOERROR)
     {
        errorwindow->SetError("PCO_ArmCamera() failed.");
     }
    }

    if (err == PCO_NOERROR)
    {
     err = grabber->PostArm(0);
     if (err != PCO_NOERROR)
     {
        errorwindow->SetError("PostArm() failed.");
     }
    }

    if (err == PCO_NOERROR)
    {
        //enable buttons
        SetControlButtonStatus(TRUE);
        camera_is_open = 1;
        ui.OpenCameraButton->setText("Close Camera");
    }
    else
    {
        grabber->Close_Grabber();
        delete grabber;
        grabber=NULL;

        camera->Close_Cam();
        delete camera;
        camera=NULL;

        errorwindow->exec();
    }

    //restore cursor
    this->setCursor(Qt::ArrowCursor);
}


void qt_pco_camera::on_GetDescriptionButton_clicked()
{
    CameraDescription *dialog = new CameraDescription(camera);
    dialog->setModal(true);
    dialog->exec();
    dialog->deleteLater();
}

void qt_pco_camera::on_SetupCameraButton_clicked()
{
    err = camera->PCO_GetRecordingState(&act_recstate);
    if (err != PCO_NOERROR) {
        errorwindow->SetError("Get Recstate failed.", err);
        errorwindow->exec();
    }
    if (act_recstate == RUN)
    {
        Exposuretime *dialog = new Exposuretime(camera,grabber,this);
        dialog->setModal(false);
        dialog->exec();
        dialog->deleteLater();
        return;
    }
    CameraSetup *setup = new CameraSetup(camera,grabber,this);
    setup->setModal(true);
    setup->exec();
    setup->deleteLater();
    //setup closed, reaquire settings for the grabber
    err = camera->PCO_GetActualSize(&cam_width, &cam_height);
    if (err != PCO_NOERROR) {
        errorwindow->SetError("Get Actual Size failed.", err);
        errorwindow->exec();
        return;
    }
    err = camera->PCO_GetDoubleImageMode(&doubleimagemode);
    picsize = cam_width*cam_height*((bitpix + 7) / 8);
    if (doubleimagemode) {
        picsize *= 2;
        cam_height *= 2;
    }

    //reassign memory
    if(single_image)
        free(single_image);
    single_image = (unsigned short*)calloc(1, picsize);
    if (!single_image) {
        errorwindow->SetError("Error assigning memory");
        errorwindow->exec();
        return;
    }
    if(current_image_converted)
        free(current_image_converted);
    current_image_converted = (unsigned char*)malloc(picsize * 3);
    if(!current_image_converted) {
        errorwindow->SetError("Error assigning memory");
        errorwindow->exec();
        return;
    }
    //invalidate recorder memory
    images_available = 0;
    FreeImageVector();

    err = grabber->Set_Grabber_Size(cam_width, cam_height);
    if (err != PCO_NOERROR) {
        errorwindow->SetError("Set Grabber size failed.", err);
        errorwindow->exec();
        return;
    }
}

void qt_pco_camera::on_StartCameraButton_clicked()
{
    //stop camera and reset params
    if (camera_is_running)
    {
        camera_is_running = 0;
        act_recstate = STOP;
        camera->PCO_SetRecordingState(act_recstate);
        ui.StartCameraButton->setText("Start Camera");
        SetImageGrabButtonStatus(FALSE);
        return;
    }
    act_recstate = RUN;
    err = camera->PCO_SetRecordingState(act_recstate);
    if (err != PCO_NOERROR)
    {
        errorwindow->SetError("Set Recording State failed", err);
        errorwindow->exec();
        return;
    }
    //assign memory for current image and set conversion lut
    if(current_image_converted)
        free(current_image_converted);
    current_image_converted = (unsigned char*)malloc(picsize * 3);
    pcocnv_convert_set_ex(lut, convertmin, convertmax, 1, convertgamma); //set a default conversion
    //enable grab buttons
    SetImageGrabButtonStatus(TRUE);
    ui.StartCameraButton->setText("Stop Camera");
    camera_is_running = 1;
}

void qt_pco_camera::on_GrabImageButton_clicked()
{
    //allocate memory for current picture
    if (!single_image)
    {
        single_image = (unsigned short*)calloc(1, picsize);
        if (!single_image)
        {
            errorwindow->SetError("Error assigning memory");
            errorwindow->exec();
            return;
        }
    }
    current_image_buffer = single_image;
    WORD mode;
    act_recstate = RUN;
    err = camera->PCO_SetRecordingState(act_recstate);
    if (err != PCO_NOERROR)
    {
        errorwindow->SetError("set recstate failed", err);
        errorwindow->exec();
        return;
    }
    err = camera->PCO_GetTriggerMode(&mode);
    if (mode == TRIGGER_MODE_SOFTWARETRIGGER)
    {
        WORD triggered;
        err = camera->PCO_ForceTrigger(&triggered);
        if (triggered == 0x000) {
            errorwindow->SetError("Software Trigger failed", err);
            errorwindow->exec();
            return;
        }
    }
    err = grabber->Acquire_Image(current_image_buffer);
    if (err != PCO_NOERROR)
    {
        errorwindow->SetError("Error grabbing image.", err);
        errorwindow->exec();
        return;
    }
    //grabbed image, convert & display;
    DisplayCurrentImage();
}

void qt_pco_camera::on_RecorderMemoryButton_clicked()
{

    unsigned int response = 0;
    int imanum;
#ifdef __linux__
    struct sysinfo info;
    sysinfo(&info);
    //raw memory size divided by picsize equals max. possible images
    long memsize = ((size_t)info.freeram * (size_t)info.mem_unit)/(unsigned long) picsize;
#else
    long memsize=-1;
#endif

    imanum=ImageVector.size();
    if(imanum==0)
     imanum=100;

    MemoryManager* memmngr  =new MemoryManager(imanum,memsize,this);
    if(memmngr->exec())
    {
        //user pressed OK
        response = memmngr->getValue();
        if(response == 0)
            return;

        //check if there are already images in memory
        if(!ImageVector.isEmpty())
        {
            SimpleDialog *askdelete = new SimpleDialog(this);
            askdelete->setWindowTitle("Confirm delete");
            askdelete->Question->setText("This will delete all stored pictures. Do you wish to continue ?");
            askdelete->setModal(TRUE);
            if (!askdelete->exec())
            {
                //user pressed no, abort
                return;
            }
        }
        //check if size actually changed
        if((unsigned int) ImageVector.size() != response)
        {
            AssignMemory(response);
        }
    }
}

void qt_pco_camera::on_StartRecorderButton_clicked()
{
    if (grabthreadworker)
    {
        if (grabthreadworker->isRunning())
        {
            //emit stop signal
            grabthreadworker->stopGrabbing();
            grabthreadworker->wait(500);
            grabthreadworker->exit();
            ui.StartRecorderButton->setText("Start Recorder");
            SetImageGrabButtonStatus(TRUE);
            SetControlButtonStatus(TRUE);
            return;
        }
    }

    if (!ImageVector.isEmpty())
    {
        WORD trigmode;
        err = camera->PCO_GetTriggerMode(&trigmode);
        if (trigmode == TRIGGER_MODE_SOFTWARETRIGGER || trigmode == TRIGGER_MODE_EXTERNALTRIGGER)
        {
            errorwindow->SetError("Recorder not available with Software trigger.", err);
            errorwindow->exec();
            return;
        }
        if (picsize == 0)
        {
            errorwindow->SetError("Not a valid Number or Camera Error");
            errorwindow->exec();
            return;
        }


        QProgressDialog* progress = new QProgressDialog("Grabbing in progress...", "Close", 0, ImageVector.size(), this);
        progress->setCancelButton(0);
        progress->setWindowModality(Qt::WindowModality(false));
        progress->setWindowTitle("Grabbing...");
        progress->setWindowIcon(QIcon(":/Resources/PCOApp.ico"));
        progress->setMinimumDuration(2000);
        progress->raise();
        progress->show();
        //delete old
        if(grabthreadworker)
        {
            delete grabthreadworker;
        }

        //create new
        grabthreadworker = new GrabThreadWorker(grabber, ImageVector);
        connect(grabthreadworker, SIGNAL(finishedAllSignal(int)), this, SLOT(displayRecordedImages(int)));
        connect(grabthreadworker, SIGNAL(finishedPicture(int)), progress, SLOT(setValue(int)));
        connect(grabthreadworker, SIGNAL(finishedAllSignal(int)), progress, SLOT(deleteLater()));
        connect(grabthreadworker, SIGNAL(recorderError(int)), progress, SLOT(deleteLater()));
        connect(grabthreadworker, SIGNAL(recorderError(int)), this, SLOT(recorderError(int)));
        //connect(grabthreadworker, SIGNAL(finished()), grabthreadworker, SLOT(deleteLater()));

        //disable grab buttons
        SetImageGrabButtonStatus(FALSE);
        SetControlButtonStatus(FALSE);
        //enable description&setup button
        ui.GetDescriptionButton->setEnabled(TRUE);
        ui.SetupCameraButton->setEnabled(TRUE);
        //set stop button
        ui.StartRecorderButton->setText("Stop Recorder");
        ui.StartRecorderButton->setEnabled(TRUE);

        grabthreadworker->start();

        act_recstate = RUN;
        err = camera->PCO_SetRecordingState(act_recstate);
        if (err != PCO_NOERROR)
        {
            errorwindow->SetError("set recstate failed", err);
            errorwindow->exec();
            ui.StartRecorderButton->setText("Start Recorder");
            SetImageGrabButtonStatus(TRUE);
            SetControlButtonStatus(TRUE);
            return;
        }

    }
    else
    {
        errorwindow->SetError("Please assign recorder memory first.");
        errorwindow->exec();
    }
}

void qt_pco_camera::on_StartLiveViewButton_clicked()
{
    //stop the live view thread if it's running
    if (livethreadworker)
    {
        if (livethreadworker->isRunning())
        {
            livethreadworker->stopLiveView();
            livethreadworker->wait(500);
            livethreadworker->exit();
            ui.StartLiveViewButton->setText("Start Live View");
            SetImageGrabButtonStatus(TRUE);
            SetControlButtonStatus(TRUE);
            return;
        }
    }
    WORD trigmode;
    err = camera->PCO_GetTriggerMode(&trigmode);
    if (trigmode == TRIGGER_MODE_SOFTWARETRIGGER || trigmode == TRIGGER_MODE_EXTERNALTRIGGER)
    {
        errorwindow->SetError("LiveView not available with Software trigger.", err);
        errorwindow->exec();
        return;
    }

    //invalidate recorder options + counter
/*
    if (images_available > 0)
    {
        //images recorded, ask if user wants to delete them
        SimpleDialog *askdelete = new SimpleDialog(this);
        askdelete->setWindowTitle("Confirm delete");
        askdelete->Question->setText("This will delete all stored pictures. Do you wish to continue ?");
        askdelete->setModal(TRUE);
        if (!askdelete->exec())
        {
            //user pressed no, abort
            return;
        }
    }
*/
//free Liveview buffers
    for(int i = 0;i<ImageVectorLive.size();i++)
    {
        if(ImageVectorLive[i])
         free(ImageVectorLive[i]);
    }
    ImageVectorLive.clear();
//and allocate new Liveview buffers
    for(int i = 0; i<LIVEVIEW_BUFFERSIZE;i++)
    {
        unsigned short* buffer;
        buffer = (unsigned short*)malloc(picsize);
        if(buffer)
        {
            memset(buffer,0,picsize);
            ImageVectorLive << buffer;
        }
    }

    if(ImageVectorLive.size()<1)
    {
        errorwindow->SetError("Error assigning Memory.");
        errorwindow->exec();
        return;
    }

/*
@@@todo mbl
camera is normally running so discard this step
or do better error handling 

    act_recstate = RUN;
    err = camera->PCO_SetRecordingState(act_recstate);
    if (err != PCO_NOERROR) {
        errorwindow->SetError("set recstate failed", err);
        errorwindow->exec();
        return;
    }
*/
    //change buttons
    ui.StartLiveViewButton->setText("Stop Live View");
    SetImageGrabButtonStatus(FALSE);
    SetControlButtonStatus(FALSE);
    ui.GetDescriptionButton->setEnabled(TRUE);
    ui.SetupCameraButton->setEnabled(TRUE);
    ui.StartLiveViewButton->setEnabled(TRUE);

    //start thread
    if(livethreadworker)
    {
        //delete old
        delete livethreadworker;
    }
    //create new
    livethreadworker = new LiveThreadWorker(grabber, ImageVectorLive);
    connect(livethreadworker, SIGNAL(finishedPictureSignal(int)), this, SLOT(displayLastImage(int)));
    livethreadworker->start();

}

void qt_pco_camera::on_OpenConvertDialogButton_clicked()
{
    static int created = 0;
    static ImageConvert* convertdialog;
    if (current_image_converted == NULL)
    {
        errorwindow->SetError("No picture found.");
        errorwindow->exec();
    }
    if(current_image_converted != NULL)
    {
        switch(created)
        {
         case 0:
            convertdialog = new ImageConvert(lut, &convertmin, &convertmax, &convertgamma, this);
            connect(convertdialog, SIGNAL(ConversionChanged()), this, SLOT(DisplayCurrentImage()));
            connect(this,SIGNAL(autorange_clicked(int,int)),convertdialog,SLOT(updateValues(int,int)));
            convertdialog->setModal(FALSE);
            convertdialog->show();
            ui.OpenConvertDialogButton->setText("Hide ConvertDialog");
            created = 1;
            break;

         case 1:
            if(convertdialog->isVisible())
            {
             ui.OpenConvertDialogButton->setText("Show ConvertDialog");
             convertdialog->hide();
            }
            else
            {
             ui.OpenConvertDialogButton->setText("Hide ConvertDialog");
             convertdialog->show();
            }
            break;

         default:
            errorwindow->SetError("Something very unexpected happened.");
            errorwindow->exec();
        }
    }
}

void qt_pco_camera::on_AutoRangeButton_clicked()
{
    if (current_image_converted == NULL) {
        errorwindow->SetError("No picture found.");
        errorwindow->exec();
    }

    if (current_image_converted != NULL)
    {
        DWORD x,y;
        convertmax = 0;
        convertmin = 65535;

        WORD* pw = (WORD*)current_image_buffer;

//discard first 10 lines with timestamp and last 10 lines
        for (y = 10; y < cam_height-10; y++)
        {
            pw=(WORD*)current_image_buffer;
            pw+=y*cam_width;

            for(x=0;x<cam_width;x++)
            {
             if (*pw > convertmax)
                convertmax = *pw;
             if (*pw < convertmin)
                 convertmin = *pw;
             pw++;
            }
        }

        if (convertmax<10)
            convertmax = 10;

        if (convertmin >= convertmax)
            convertmin = convertmax - 1;

        pcocnv_convert_set(lut, convertmin, convertmax, 1);
        emit autorange_clicked(convertmin,convertmax);
        DisplayCurrentImage();
    }

}

void qt_pco_camera::SetControlButtonStatus(BOOL status)
{
    ui.OpenCameraButton->setEnabled(status);
    ui.GetDescriptionButton->setEnabled(status);
    ui.SetupCameraButton->setEnabled(status);
    ui.StartCameraButton->setEnabled(status);
    //add additional buttons here
}

void qt_pco_camera::SetImageGrabButtonStatus(BOOL status)
{
    ui.GrabImageButton->setEnabled(status);
    ui.RecorderMemoryButton->setEnabled(status);
    ui.StartRecorderButton->setEnabled(status);
    ui.StartLiveViewButton->setEnabled(status);
}

void qt_pco_camera::SetImageConvertOptionsButtonStatus(BOOL status)
{
    ui.OpenConvertDialogButton->setEnabled(status);
    ui.AutoRangeButton->setEnabled(status);
}

void qt_pco_camera::SetImageScrollButtonStatus(BOOL status)
{
    ui.actionPicright->setEnabled(status);
    ui.actionPicleft->setEnabled(status);
    ui.actionPicstart->setEnabled(status);
    ui.actionPicend->setEnabled(status);
}


void qt_pco_camera::on_Image1to1_toggled()
{
    ui.ImageView->setTransform(QTransform());
}

void qt_pco_camera::on_ImageFitToBox_toggled()
{
    resizeEvent();
}

void qt_pco_camera::resizeEvent(QResizeEvent *)
{
    if (ui.ImageFitToBox->isChecked())
    {
        QRectF bounds = scene->itemsBoundingRect();
        //ui.ImageView->setSceneRect(scene->sceneRect());
        ui.ImageView->ensureVisible(scene->sceneRect());
        ui.ImageView->fitInView(bounds, Qt::KeepAspectRatio);
        //ui.ImageView->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
    }
}

void qt_pco_camera::on_actionSave_triggered()
{
    if (current_image_converted == NULL)
    {
        errorwindow->SetError("Nothing to save!");
        errorwindow->exec();
        return;
    }

    QString filename = QFileDialog::getSaveFileName(this, tr("Save Image to file"), "", tr("Bitmap (*.bmp);;PNG (*.png);;JPEG (*.jpg);;All Files (*)"));
    if(!(filename.endsWith(".bmp") || filename.endsWith(".png") || filename.endsWith(".jpg")))
    {
        //add default extension
        filename.append(".png");
    }

    if (filename.isEmpty())
    {
        errorwindow->SetError("No file name specified.");
        errorwindow->exec();
        return;
    }
    else
    {
        QImage pcoimage(current_image_converted, cam_width, cam_height, QImage::Format_RGB888);
        pcoimage.save(filename,0,100);
    }
}

void qt_pco_camera::on_actionExit_triggered()
{
    QApplication::instance()->exit();
}

void qt_pco_camera::on_actionInfo_triggered()
{
    InfoPopup *info = new InfoPopup(this);
    info->setModal(true);
    info->exec();
}

/*
void qt_pco_camera::on_actionSpeedTest_triggered()
{
    if(!camera_is_running)
    {
        errorwindow->SetError("Please start the camera first!");
        errorwindow->exec();
        return;
    }

    SpeedTest* testwindow = new SpeedTest(camera,grabber,picsize,this);
    testwindow->setModal(true);
    testwindow->exec();
    delete testwindow;
}
*/

void qt_pco_camera::displayLastImage(int imagenumber)
{
//    images_available = imagenumber + 1;
    current_image = imagenumber;
    current_image_buffer = ImageVectorLive[imagenumber];
    DisplayCurrentImage();
    //done converting, request next picture
    livethreadworker->sendPicture();
}

void qt_pco_camera::recorderError(int number)
{
    errorwindow->SetError("Recorder encountered an Error. See log for details.");
    errorwindow->show();
    displayRecordedImages(number);
}

void qt_pco_camera::displayRecordedImages(int imagenumber)
{
    if (imagenumber <= 0)
    {
        errorwindow->SetError("Recorder encountered an Error. See log for details.");
        errorwindow->exec();
        return;
    }
    else
    {
        images_available = imagenumber;
//        current_image = imagenumber - 1;
//        current_image_buffer = ImageVector[0imagenumber - 1];
        //show first image in buffer
        current_image = 0;
        current_image_buffer = ImageVector[0];
        ui.actionImagecounter->setText(QString::number(current_image+1) + " / " + QString::number(images_available));
        DisplayCurrentImage();
    }
    //enable buttons again if we are done
    SetImageGrabButtonStatus(true);
    SetControlButtonStatus(true);
    ui.StartRecorderButton->setText("Start Recorder");
    //enable scroll buttons
    SetImageScrollButtonStatus(TRUE);
}

void qt_pco_camera::DisplayCurrentImage()
{
    //clear scene
    scene->clear();
    //convert image
    pcocnv_conv_buf_12to24(0, cam_width, cam_height, current_image_buffer, current_image_converted, lut);
    //and display it
    pcoimage = QImage(current_image_converted, cam_width, cam_height, QImage::Format_RGB888);

    QPixmap pcoimagePixmap = QPixmap::fromImage(pcoimage);

    scene->setSceneRect(pcoimagePixmap.rect());

    currentPixmap = scene->addPixmap(pcoimagePixmap);
    //ui.ImageView->update();
    if (ui.ImageFitToBox->isChecked())
    {
        QRectF bounds = scene->itemsBoundingRect();
        ui.ImageView->fitInView(bounds, Qt::KeepAspectRatio);
    }
}

void qt_pco_camera::on_actionPicstart_triggered()
{
    if (!ImageVector.isEmpty())
    {
        current_image_buffer = ImageVector[0];
        current_image = 0;
        DisplayCurrentImage();
        ui.actionImagecounter->setText(QString::number(current_image+1) + " / " + QString::number(images_available));
    }
}

void qt_pco_camera::on_actionPicend_triggered()
{
    if (images_available > 0)
    {
        current_image_buffer = ImageVector[images_available - 1];
        current_image = images_available - 1;
        DisplayCurrentImage();
        ui.actionImagecounter->setText(QString::number(current_image+1) + " / " + QString::number(images_available));
    }
}

void qt_pco_camera::on_actionPicleft_triggered()
{
    if ((current_image > 0) && (images_available > 0))
    {
        current_image--;
        current_image_buffer = ImageVector[current_image];
        DisplayCurrentImage();
        ui.actionImagecounter->setText(QString::number(current_image+1) + " / " + QString::number(images_available));
    }
}

void qt_pco_camera::on_actionPicright_triggered()
{
    if ((current_image < (images_available - 1)) && (images_available > 0))
    {
        current_image++;
        current_image_buffer = ImageVector[current_image];
        DisplayCurrentImage();
        ui.actionImagecounter->setText(QString::number(current_image+1) + " / " + QString::number(images_available));
    }
}

void qt_pco_camera::on_actionSaveb16tif_triggered()
{
    if (current_image_buffer == NULL)
    {
        errorwindow->SetError("Nothing to save!");
        errorwindow->exec();
        return;
    }

    QString file = QFileDialog::getSaveFileName(this, tr("Save Image to file"), "", tr("b16 (*.b16);;tif (*.tif)"));
    if(!(file.endsWith(".b16") || file.endsWith(".tif")))
    {
        //add default extension
        file.append(".b16");
    }

    if (file.isEmpty() || (file.length() > 100))
    {
        errorwindow->SetError("No file name specified.");
        errorwindow->exec();
        return;
    }
    else if (file.endsWith(".b16"))
    {
        char filename[100];
        strcpy(filename, file.toStdString().c_str());
        store_b16(filename,cam_width,cam_height,0,current_image_buffer);
    }
    else if (file.endsWith(".tif"))
    {
        char filename[100];
        strcpy(filename, file.toStdString().c_str());
        store_tif(filename,cam_width,cam_height,0,current_image_buffer);
    }
}

bool qt_pco_camera::AssignMemory(int images)
{
    QProgressDialog* progress = NULL;

    double imagetotalsize = picsize * images;
    if (imagetotalsize >= 536870912.0)
    { //>500 Mb
        progress = new QProgressDialog("Assigning Memory...", "Close", 0, images, this);
        progress->setCancelButton(0);
        progress->setWindowModality(Qt::WindowModality(true));
        progress->setWindowTitle("Please Wait...");
        progress->setWindowIcon(QIcon(":/Resources/PCOApp.ico"));
        progress->setMinimumDuration(2000);
        progress->raise();
        progress->show();
    }
    //allocate memory,
    FreeImageVector();
    for(int i = 0; i<images;i++)
    {
        unsigned short* buffer;
        buffer = (unsigned short*)malloc(picsize);
        if(!buffer)
        {
            errorwindow->SetError("Error assigning Memory.");
            errorwindow->exec(); FreeImageVector();
            return false;
        }
        else
        {
            memset(buffer,0,picsize);
            ImageVector << buffer;
        }
        if(progress)
        {
            progress->setValue(i);
            QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
        }
    }
    //delete window
    if (progress)
        progress->deleteLater();

    ui.MemoryAssigned->setText("Mem: "+QString::number(ImageVector.size()));

    return true;
}

void qt_pco_camera::FreeImageVector()
{
    scene->clear();
    //set mem display to zero
    current_image = images_available = 0;
    ui.actionImagecounter->setText("0 / 0");
    ui.MemoryAssigned->setText("Mem: 0");
    //disable scroll buttons
    SetImageScrollButtonStatus(FALSE);

    for(int i = 0;i<ImageVector.size();i++)
    {
        if(ImageVector[i])
            free(ImageVector[i]);
    }
    ImageVector.clear();
}

/* TEST SETTINGS */


#define ERR_MASK_NOTSUPPORTED(err) { if((err&0xC000FFFF)==PCO_ERROR_FIRMWARE_NOT_SUPPORTED)err=PCO_NOERROR;}

DWORD qt_pco_camera::SetInitialValues()
{
    DWORD err;
    char errtext[50];

    //enable timestamp in images
    sprintf(errtext,"PCO_SetTimestampMode() failed");
    err = camera->PCO_SetTimestampMode(TIMESTAMP_MODE_BINARYANDASCII);
    ERR_MASK_NOTSUPPORTED(err)
    if (err == PCO_NOERROR)
    {
     sprintf(errtext,"PCO_SetBitAlignment(LSB) failed");
     err=camera->PCO_SetBitAlignment(BIT_ALIGNMENT_LSB);
     ERR_MASK_NOTSUPPORTED(err)
    }

    //camera without recorder return error NOT_SUPPORTED
    //set camera to sequenze
    WORD store;
    err=camera->PCO_GetStorageMode(&store);
    if((err==PCO_NOERROR)&&(store == STORAGE_MODE_FIFO_BUFFER ))
    {
     err=camera->PCO_SetStorageMode(STORAGE_MODE_RECORDER);
     ERR_MASK_NOTSUPPORTED(err)
    }

    if (err == PCO_NOERROR)
    {
     sprintf(errtext,"PCO_ArmCamera() failed");
     err = camera->PCO_ArmCamera();
    }

    if (err != PCO_NOERROR)
    {
     errorwindow->SetError(errtext);
     errorwindow->exec();
     return err;
    }

    sprintf(errtext,"PostArm() failed");
    err=grabber->PostArm();

    if (err == PCO_NOERROR)
    {
     sprintf(errtext,"PCO_GetActualSize() failed");
     err = camera->PCO_GetActualSize(&cam_width, &cam_height);
    }

    if (err == PCO_NOERROR)
    {
     sprintf(errtext,"PCO_GetDoubleImageMode() failed");
     err = camera->PCO_GetDoubleImageMode(&doubleimagemode);
     if((err&0xC000FFFF)==PCO_ERROR_FIRMWARE_NOT_SUPPORTED)
     {
         doubleimagemode=0;
         err=PCO_NOERROR;
     }
    }

    picsize = cam_width*cam_height*((bitpix + 7) / 8);
    if (doubleimagemode)
    {
     picsize *= 2;
     cam_height *= 2;
    }

    //allocate memory for current picture
    single_image = (unsigned short*)calloc(1, picsize);
    current_image_buffer = single_image;

    if (err != PCO_NOERROR)
    {
     errorwindow->SetError(errtext);
     errorwindow->exec();
     return err;
    }

    DWORD coc_sec,coc_nsec;
    err=camera->PCO_GetCOCRuntime(&coc_sec,&coc_nsec);
    if((err==PCO_NOERROR)&&(coc_sec>0))
     grabber->Set_Grabber_Timeout(ImageTimeout+coc_sec);

#ifdef CPCO_GRAB_USB_H
/*
    //get highest pixelrate
    int i;
    for (i = 0;i<4;i++) {
        if(description.dwPixelRateDESC[i+1] == 0) {
            break;
        }
    }
    //set highest pixelrate
    err = camera->PCO_SetPixelRate(description.dwPixelRateDESC[i]);
    if (err != PCO_NOERROR) {
        errorwindow->SetError("Set Pixelrate failed"); errorwindow->exec(); return err;
    }
    //  err = camera->PCO_SetDelayExposureTime(0, 5000, 1, 1);
    //  if (err != PCO_NOERROR) {
    //    errorwindow->SetError(); errorwindow->exec(); return;
    //  }
*/
#endif


    if(Interfacetype==INTERFACE_CAMERALINK)
    {
     //ME4 block
     PCO_SC2_CL_TRANSFER_PARAM clpar;
     err = camera->PCO_GetTransferParameter(&clpar, sizeof(clpar));

     if(  (camtype!=CAMERATYPE_PCO_EDGE)
        &&(camtype!=CAMERATYPE_PCO_EDGE_42)
        &&(camtype!=CAMERATYPE_PCO_EDGE_GL)
       )
     {
      if(clpar.Transmit!=1)
      {
       clpar.Transmit=1;
      }
      if(clpar.ClockFrequency!=80000000)
      {
       clpar.ClockFrequency=80000000;
      }
      err=camera->PCO_SetTransferParameter(&clpar,sizeof(clpar));
      err=camera->PCO_GetTransferParameter(&clpar,sizeof(clpar));

     }


     err = grabber->Allocate_Framebuffer(5);
     if(err != PCO_NOERROR)
     {
        errorwindow->SetError("framebuffer alloc failed.");
        errorwindow->exec();
     }
    }

    return err;
}

