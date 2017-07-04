//-----------------------------------------------------------------//
// Name        | pco_ser_con.cpp             | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - CameraLink Communication             //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.05                                         //
//-----------------------------------------------------------------//
// Notes       | main console program to show how to use           //
//             | class Cpco_com                                    //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2012 PCO AG                                          //
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
// Free Software Foundation, Inc., 59 Temple Place - Suite 330,    //
// Boston, MA 02111-1307, USA                                      //
//-----------------------------------------------------------------//

//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  1.01     | 08.11.2010 | ported from Windows SDK                //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.02     | 14.01.2011 |                                        //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.03     | 24.01.2012 | logging class from library             //
//           |            |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.2012 |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//

#include "Cpco_com.h"
#include "Cpco_com_usb.h"
#include "VersionNo.h"


CPco_Log mylog("pco_ser_con.log");

using namespace std;


int camera_new(int board)
{

  DWORD err=PCO_NOERROR;
  char infostr[100];

  CPco_com_usb *cam;

  cam = new CPco_com_usb;

  mylog.set_logbits(0x0F00F);
  cam->SetLog(&mylog);

  printf("%s()\n",__FUNCTION__);
  printf("Try to open Camera %d\n",board);
  err=cam->Open_Cam(board);
  if(err!=PCO_NOERROR)
  {
   printf("error 0x%x in Open_Cam, close application\n",err);
   goto err_out_new;
  }

  printf("Close Camera\n");
  err=cam->Close_Cam();
  if(err!=PCO_NOERROR)
  {
   printf("error 0x%x in Close_Cam, close application\n",err);
   goto err_out_new;
  }

  printf("Try to open Camera %d\n",board);
  err=cam->Open_Cam(board);
  if(err!=PCO_NOERROR)
  {
   printf("error 0x%x in Open_Cam, close application\n",err);
   goto err_out_new;
  }

  err=cam->PCO_GetInfo(1,infostr,sizeof(infostr));
  if(err!=PCO_NOERROR)
   printf("PCO_GetInfo() Error 0x%x\n",err);
  else
   printf("Camera name : %s\n",infostr);

  err=cam->Close_Cam();
  if(err!=PCO_NOERROR)
   printf("CloseCam() Error 0x%x\n",err);

err_out_new:
  delete cam;
 
  printf("%s() return 0x%x\n",__FUNCTION__,err);
  return err;
}  

int camera_static(int board)
{
  CPco_com_usb camera;
  DWORD err=PCO_NOERROR;
  char infostr[100];
  DWORD exp_time,delay_time;
  DWORD width,height;
  SC2_Camera_Description_Response description;
  char *Firmwaretext=NULL;
  char *Hardwaretext=NULL;
  int size;


  printf("%s()\n",__FUNCTION__);
  printf("Version Info: %02d.%02d.%02d\n",VERSION,SUBVERSION,SUBAGE);

  
  mylog.set_logbits(0x0F00F);
  camera.SetLog(&mylog);

  printf("Try to open Camera %d\n",board);
  err=camera.Open_Cam(board);
  if(err!=PCO_NOERROR)
  {

   printf("error 0x%x in Open_Cam, close application\n",err);
   goto err_out_static;
  }

  printf("Close Camera\n");
  err=camera.Close_Cam();
  if(err!=PCO_NOERROR)
  {
   printf("error 0x%x in Close_Cam, close application\n",err);
   goto err_out_static;
  }


  printf("Try to open Camera %d\n",board);
  err=camera.Open_Cam(board);
  if(err!=PCO_NOERROR)
  {
   printf("error 0x%x in Open_Cam, close application",err);
   goto err_out_static;
  }

  exp_time=5000;
  delay_time=1000;
  
  err=camera.PCO_GetCameraDescriptor(&description);
  if(err!=PCO_NOERROR)
   printf("PCO_GetCameraDescriptor() Error 0x%x\n",err);

  err=camera.PCO_GetInfo(1,infostr,sizeof(infostr));
  if(err!=PCO_NOERROR)
   printf("PCO_GetInfo() Error 0x%x\n",err);
  else
   printf("Camera name : %s\n",infostr);

  err=camera.PCO_GetFirmwareVersion(NULL,&size);
  if(err==PCO_NOERROR)
  {
//   printf("size of Firmware text is %d\n",size);
   Firmwaretext=(char*)malloc(size);
   if(Firmwaretext)
   {
    memset(Firmwaretext,0,size);
    err=camera.PCO_GetFirmwareVersion(Firmwaretext,&size);
    printf("%s\n",Firmwaretext);
    free(Firmwaretext);
   }
  }
  
  err=camera.PCO_GetHardwareVersion(NULL,&size);
  if(err==PCO_NOERROR)
  {
//   printf("size of Hardware text is %d\n",size);
   Hardwaretext=(char*)malloc(size);
   if(Hardwaretext)
   {
    memset(Hardwaretext,0,size);
    err=camera.PCO_GetHardwareVersion(Hardwaretext,&size);
    printf("%s\n",Hardwaretext);
    free(Hardwaretext);
   }
  }
  
//set RecordingState to STOP
  err=camera.PCO_SetRecordingState(0);
  if(err!=PCO_NOERROR)
   printf("PCO_SetRecordingState() Error 0x%x\n",err);

  err=camera.PCO_SetTimestampMode(2);
  if(err!=PCO_NOERROR)
   printf("PCO_SetTimestampMode() Error 0x%x\n",err);

//set camera timebase to us
  err=camera.PCO_SetTimebase(1,1);
  if(err!=PCO_NOERROR)
   printf("PCO_SetTimebase() Error 0x%x\n",err);

  err=camera.PCO_SetDelayExposure(delay_time,exp_time);
  if(err!=PCO_NOERROR)
   printf("PCO_SetDelayExposure() Error 0x%x\n",err);

//prepare Camera for recording
  err=camera.PCO_ArmCamera();
  if(err!=PCO_NOERROR)
   printf("PCO_ArmCamera() Error 0x%x\n",err);

  err=camera.PCO_GetActualSize(&width,&height);
  if(err!=PCO_NOERROR)
   printf("PCO_GetActualSize() Error 0x%x\n",err);

  printf("Actual Resolution %d x %d\n",width,height);
  printf("\n");


  char buffer[4000];
  WORD numSignals;
  err=camera.PCO_GetHWIOSignalCount(&numSignals);
  if(err!=PCO_NOERROR)
   printf("PCO_GetHWIOSignalCount() Error 0x%x\n",err);
  else
  {
   printf("PCO_GetHWIOSignalCount() returned %d\n",numSignals);
   printf("\n");
    
   for(int x=0;x<numSignals;x++)
   {
    size=4000;
    memset(buffer,0,sizeof(buffer)); 
    err=camera.PCO_GetHWIOSignalDescriptor(x,buffer,&size);
    if(err==PCO_NOERROR)
     printf("%s",buffer);
    else
    {
     printf("PCO_GetHWIOSignalDescriptor(%d) Error 0x%x\n",x,err);
     break;
    }
    printf("\n");
   }

   WORD Enabled,Type,Polarity,FilterSetting,Selected;
   DWORD SignalType,Parameter;
  
   Enabled=1;
   Type=1;
   Polarity=1;
   FilterSetting=0;
   Selected=0;
   printf("PCO_SetHWIOSignal(3,%d,%d,%d,%d,%d)\n",Enabled,Type,Polarity,FilterSetting,Selected);
   printf(" Enabled  %d\n Type %d\n Polarity %d\n Filter %d\n Selected %d\n",Enabled,Type,Polarity,FilterSetting,Selected);
   err=camera.PCO_SetHWIOSignal(3,Enabled,Type,Polarity,FilterSetting,Selected);
   if(err!=PCO_NOERROR)
    printf("PCO_SetHWIOSignal() Error 0x%x\n",err);
   else
   {
    err=camera.PCO_GetHWIOSignal(3,&Enabled,&Type,&Polarity,&FilterSetting,&Selected);
    if(err!=PCO_NOERROR)
     printf("PCO_GetHWIOSignal(3,) Error 0x%x\n",err);
    else
     printf("PCO_GetHWIOSignal(3,) returned:\n Enabled  %d\n Type %d\n Polarity %d\n Filter %d\n Selected %d\n",Enabled,Type,Polarity,FilterSetting,Selected);
   }
  
//  DWORD PCO_SetHWIOSignalTiming(WORD SignalNum,WORD Selection,DWORD Parameter)
   for(Parameter=1;Parameter<=4;Parameter++)
   {
    printf("PCO_SetHWIOSignalTiming(3,0,%d)\n",Parameter);
//   printf("Parameter %d\n",Parameter);
    err=camera.PCO_SetHWIOSignalTiming(3,0,Parameter);
    if(err!=PCO_NOERROR)
    {
     printf("PCO_SetHWIOSignalTiming() Error 0x%x\n",err);
     break;
    }
     
    err=camera.PCO_GetHWIOSignalTiming(3,0,&SignalType,&Parameter);
    if(err!=PCO_NOERROR)
    {
     printf("PCO_GetHWIOSignalTiming() Error 0x%x\n",err);
     break;
    }
    printf("PCO_GetHWIOSignalTiming(3,0,)\n SignalType %d 0x%x\n Parameter %d\n",SignalType,SignalType,Parameter);
   }
  }
  
  err=camera.PCO_ArmCamera();
  if(err!=PCO_NOERROR)
   printf("PCO_ArmCamera() Error 0x%x\n",err);

  err=camera.Close_Cam();
  if(err!=PCO_NOERROR)
   printf("CloseCam() Error 0x%x\n",err);

err_out_static:
  printf("%s() return 0x%x\n",__FUNCTION__,err);
  return err;
}

int main(int argc, char *argv[])
{
  int board=0;
  int help=0;
  int err;
  
  for(int x=argc;x>1;)
  {
   char *a;

   x--;

   if(strstr(argv[x],"-h"))
    help=1;
   if(strstr(argv[x],"-?"))
    help=1;
   if(strstr(argv[x],"?"))
    help=1;

   if((a=strstr(argv[x],"-b")))
   {
    board=atoi(a+2);
   }
  }

  if(help)
  {
   printf("usage: %s options\n"
          "options: \n"
          "-b[0...7] actual camera number\n"
          "-h,-?,? this message\n",argv[0]);
   exit(0);
  }


  err=camera_new(board);
  printf("\n");
  if(err!=PCO_NOERROR)
   return err; 
  
  err=camera_static(board);
  printf("\n");
  return err;
}


