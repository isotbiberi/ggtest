//-----------------------------------------------------------------//
// Name        | Cpco_com_func.cpp           | Type: (*) source    //
//-------------------------------------------|       ( ) header    //
// Project     | pco.camera                  |       ( ) others    //
//-----------------------------------------------------------------//
// Platform    | Linux                                             //
//-----------------------------------------------------------------//
// Environment |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// Purpose     | pco.camera - Communication                        //
//-----------------------------------------------------------------//
// Author      | MBL, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | In this file are all functions which use the      //
//             | telegram structures for communication with        //
//             | pco devices                                       //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2014 PCO AG                                          //
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

//-----------------------------------------------------------------//
// Revision History:                                               //
//-----------------------------------------------------------------//
// Rev.:     | Date:      | Changed:                               //
// --------- | ---------- | ---------------------------------------//
//  1.01     | 08.10.2010 | from Me4 class                         //
//           |            |                                        //
// --------- | ---------- | ---------------------------------------//
//  1.02     | 25.02.2014 | changes for usb cameras                //
//           |            | new functions                          //
// --------- | ---------- | ---------------------------------------//
//  1.03     | 20.06.2015 | bugfixes                               //
//           |            | comments in header for doxygen         //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.2009 |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//


#include "Cpco_com.h"

DWORD CPco_com::PCO_GetRecordingState(WORD *recstate)
{
  SC2_Recording_State_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_RECORDING_STATE;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(SC2_Simple_Telegram),
    &resp,sizeof(SC2_Recording_State_Response));


  *recstate=resp.wState;
  return err;

}

#define REC_WAIT_TIME 500
DWORD CPco_com::PCO_SetRecordingState(WORD recstate)
{
  WORD g_state,x;
  DWORD err=PCO_NOERROR;

  PCO_GetRecordingState(&g_state);

  if(g_state!=recstate)
  {
    DWORD s,ns;
    SC2_Set_Recording_State com;
    SC2_Recording_State_Response resp;

    com.wCode=SET_RECORDING_STATE;
    com.wState=recstate;
    com.wSize=sizeof(SC2_Set_Recording_State);
    err=Control_Command(&com,sizeof(SC2_Set_Recording_State),
      &resp,sizeof(SC2_Recording_State_Response));

    if(err!=0)
      return err;

    PCO_GetCOCRuntime(&s,&ns);

    ns/=1000000;
    ns+=1;
    ns+=s*1000;

    ns+=REC_WAIT_TIME;
    ns/=50;

    for(x=0;x<ns;x++)
    {
      PCO_GetRecordingState(&g_state);
      if(g_state==recstate)
        break;

      Sleep_ms(50);
    }
    if(x>=ns)
      err=PCO_ERROR_TIMEOUT;
  }
  return err;
}

DWORD CPco_com::PCO_ArmCamera(void)
{
  SC2_Arm_Camera_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  //must increase timeout for arm command
  tab_timeout.command=15000;
  com.wCode=ARM_CAMERA;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(SC2_Simple_Telegram),
    &resp,sizeof(SC2_Arm_Camera_Response));

  tab_timeout.command=PCO_SC2_COMMAND_TIMEOUT;

  return err;
}

DWORD CPco_com::PCO_GetActualSize(DWORD *width,DWORD *height)
{
  DWORD err;

  SC2_Simple_Telegram com;
  SC2_ROI_Response resp;

  com.wCode=GET_ROI;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
    *width=resp.wROI_x1-resp.wROI_x0+1;
    *height=resp.wROI_y1-resp.wROI_y0+1;
  }

  return err;
}


DWORD CPco_com::PCO_ResetSettingsToDefault()
{
  DWORD err;
  SC2_Simple_Telegram com;
  SC2_Reset_Settings_To_Default_Response resp;

  com.wCode=RESET_SETTINGS_TO_DEFAULT;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_com::PCO_SetCameraToCurrentTime()
{
  DWORD err;
  SC2_Set_Date_Time com;
  SC2_Date_Time_Response resp;

  struct tm st;

#ifdef WIN32
  time_t curtime;
  curtime=time(NULL);
  localtime_s(&st,&curtime);
#else
  struct timeval tv;

  gettimeofday(&tv,NULL);

  localtime_r(&tv.tv_sec,&st);
#endif

  com.wCode=SET_DATE_TIME;
  com.wSize=sizeof(SC2_Set_Date_Time);
  com.bDay=st.tm_mday;
  com.bMonth=st.tm_mon+1;
  com.wYear=st.tm_year+1900;
  com.wHours=st.tm_hour;
  com.bMinutes=st.tm_min;
  com.bSeconds=st.tm_sec;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_com::PCO_SetDateTime(struct tm *st)
{
  DWORD err;
  SC2_Set_Date_Time com;
  SC2_Date_Time_Response resp;

  com.wCode=SET_DATE_TIME;
  com.wSize=sizeof(SC2_Set_Date_Time);
  com.bDay=st->tm_mday;
  com.bMonth=st->tm_mon+1;
  com.wYear=st->tm_year+1900;
  com.wHours=st->tm_hour;
  com.bMinutes=st->tm_min;
  com.bSeconds=st->tm_sec;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_com::PCO_GetCameraSetup(WORD *Type,DWORD *Setup,WORD *Len)
{
  DWORD err=PCO_NOERROR;
  SC2_Simple_Telegram com;
  SC2_Get_Camera_Setup_Response resp;
  WORD len;

  if(*Len < NUMSETUPFLAGS)
    len = *Len;
  else
    len = NUMSETUPFLAGS;
  //   len=max(*Len,NUMSETUPFLAGS);


  com.wCode=GET_CAMERA_SETUP;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  *Type=resp.wType;
  if(Setup)
    memcpy(Setup,resp.dwSetupFlags,len*sizeof(DWORD));
  *Len=sizeof(resp.dwSetupFlags)/sizeof(DWORD);
  return err;
}

DWORD CPco_com::PCO_SetCameraSetup(WORD Type, DWORD *Setup,WORD Len)
{
  DWORD err=PCO_NOERROR;
  SC2_Set_Camera_Setup com;
  SC2_Get_Camera_Setup_Response resp;
  WORD len;

  if(Len < NUMSETUPFLAGS)
    len = Len;
  else
    len = NUMSETUPFLAGS;

  //   len=max(Len,NUMSETUPFLAGS);

  com.wCode=SET_CAMERA_SETUP;
  com.wSize=sizeof(SC2_Set_Camera_Setup);
  com.wType=Type;
  memcpy(com.dwSetupFlags,Setup,len*sizeof(DWORD));
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  return err;
}


DWORD CPco_com::PCO_RebootCamera()
{
  SC2_Simple_Telegram    Reboot_Com;
   
  Reboot_Com.wCode      = 0x1704;
  Reboot_Com.wSize      = sizeof(Reboot_Com);
  
  Control_Command(&Reboot_Com,sizeof(Reboot_Com),NULL,0);
  return PCO_NOERROR;
}


DWORD CPco_com::PCO_RebootCamera_Wait(DWORD connect_time)
{
  SC2_Simple_Telegram   Reboot_Com;
  SC2_Simple_Telegram   com;
  SC2_Camera_Type_Response resp;
  struct timeval t,t1;
  DWORD time_done;
  DWORD err;

  Reboot_Com.wCode      = 0x1704;
  Reboot_Com.wSize      = sizeof(Reboot_Com);
  
  Control_Command(&Reboot_Com,sizeof(Reboot_Com),NULL,0);

  Sleep_ms(6000);

  gettimeofday(&t,NULL);

  do
  {
   Sleep_ms(200);

   com.wCode=GET_CAMERA_TYPE;
   com.wSize=sizeof(SC2_Simple_Telegram);
   err=Control_Command(&com,sizeof(com),&resp,sizeof(SC2_Camera_Type_Response));


   gettimeofday(&t1,NULL);
   if(t1.tv_sec>t.tv_sec)
    t1.tv_usec+=(1000000*(t1.tv_sec-t.tv_sec));
   else if(t1.tv_sec<t.tv_sec)
    t1.tv_usec+=1000000;
   time_done=(t1.tv_usec-t.tv_usec)/1000;
  }
  while((err!=PCO_NOERROR)&&(time_done<connect_time));

  if(time_done<connect_time)
   err=PCO_ERROR_DRIVER | PCO_ERROR_TIMEOUT;

  return err;
}

DWORD CPco_com::PCO_GetCameraType(WORD* wCamType,DWORD* dwSerialNumber,WORD* wIfType)
{
    SC2_Simple_Telegram com;
    SC2_Camera_Type_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CAMERA_TYPE;
    com.wSize=sizeof(SC2_Simple_Telegram);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
    {
        *wCamType=resp.wCamType;
        *dwSerialNumber=resp.dwSerialNumber;
        if(wIfType)
         *wIfType=resp.wInterfaceType;
        writelog(INIT_M,hdriver,"PCO_GetCameraType:  camType 0x%04x resp.wInterfaceType 0x%04x",resp.wCamType,resp.wInterfaceType);

    }

    return err;

}

DWORD CPco_com::PCO_GetCameraDescriptor(SC2_Camera_Description_Response *descript)
{
  memcpy(descript,&description,sizeof(SC2_Camera_Description_Response));
  return PCO_NOERROR;
}


DWORD CPco_com::PCO_GetCameraDescription(SC2_Camera_Description_Response* response)
{
    SC2_Get_Camera_Description com;
    SC2_Camera_Description_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CAMERA_DESCRIPTION;
    com.wType=0x0000;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
    {
        memcpy(response,&resp,sizeof(resp));
        memcpy(&description,&resp,sizeof(resp));
    }
    return err;
}

DWORD CPco_com::PCO_GetCameraDescription(SC2_Camera_Description_2_Response* response)
{
    SC2_Get_Camera_Description com;
    SC2_Camera_Description_2_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CAMERA_DESCRIPTION_EX;
    com.wType=0x0001;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
    {
        memcpy(response,&resp,sizeof(resp));
    }

    return err;
}

DWORD CPco_com::PCO_GetCameraName(void* buf,int length)
{
  DWORD err;
  int len;
  SC2_Camera_Name_Response resp;
  SC2_Simple_Telegram com;

  com.wCode=GET_CAMERA_NAME;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),
    &resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
    len=(int)strlen(resp.szName);
    if(len<length)
    {
      memcpy(buf,resp.szName,len);
      memset((char*)buf+len,0,length-len);
    }
    else
      memset(buf,0,length);
  }
  return err;
}

DWORD CPco_com::PCO_GetInfo(DWORD typ,void* buf,int length)
{
  DWORD err;
  int len;

  SC2_Get_Info_String_Response resp;
  SC2_Get_Info_String com;

  com.wCode=GET_INFO_STRING;
  com.wSize=sizeof(SC2_Get_Info_String);
  com.dwType=typ;
  err=Control_Command(&com,sizeof(SC2_Get_Info_String),
    &resp,sizeof(SC2_Get_Info_String_Response));


  if(err==PCO_NOERROR)
  {
    len=(int)strlen(resp.szName);
    if(len<length)
    {
      memcpy(buf,resp.szName,len);
      memset((char*)buf+len,0,length-len);
    }
    else
      memset(buf,0,length);
  }
  else
  {
    if(((err&~PCO_ERROR_DEVICE_MASK)==PCO_ERROR_FIRMWARE_NOT_SUPPORTED)&&(typ==1))
    {
      err=PCO_GetCameraName(buf,length);
    }
  }

  return err;
}

DWORD CPco_com::PCO_GetTemperature(SHORT *sCCDTemp,SHORT *sCAMTemp,SHORT *sExtTemp)
{
  SC2_Temperatures_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_TEMPERATURE;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
    *sCCDTemp=resp.sCCDtemp;
    *sCAMTemp=resp.sCamtemp;
    *sExtTemp=resp.sPStemp;
  }

  return err;
}

DWORD CPco_com::PCO_GetHealthStatus(unsigned int *dwWarnings,
                                    unsigned int *dwErrors,
                                    unsigned int *dwStatus)
{
  DWORD err;

  SC2_Simple_Telegram com;
  SC2_Camera_Health_Status_Response resp;

  com.wCode=GET_CAMERA_HEALTH_STATUS;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  // look at the response somehow
  if (err == PCO_NOERROR)
  {
    if (dwWarnings) *dwWarnings = resp.dwWarnings;
    if (dwErrors  ) *dwErrors   = resp.dwErrors;
    if (dwStatus  ) *dwStatus   = resp.dwStatus;
  }
  else
  {
    // clear the bits? Would it be better to leave them alone?
    if (dwWarnings) *dwWarnings = 0;
    if (dwErrors  ) *dwErrors   = 0;
    if (dwStatus  ) *dwStatus   = 0;
  }
  return err;
} // PCO_GetHealthStatus



DWORD CPco_com::PCO_GetSensorSignalStatus(DWORD *status,DWORD *imagecount)
{
  DWORD err;

  SC2_Simple_Telegram com;
  SC2_Camera_Sensor_Signal_Status_Response resp;

  com.wCode=GET_SENSOR_SIGNAL_STATUS;
  com.wSize=sizeof(SC2_Simple_Telegram);

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  *status=resp.dwStatus;
  *imagecount=resp.dwImageCount;

  return err;
}


DWORD CPco_com::PCO_GetCameraBusyStatus(WORD* camera_busy)
{
    SC2_Simple_Telegram com;
    SC2_Camera_Busy_Status_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CAMERA_BUSY_STATUS;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *camera_busy = resp.wStatus;

    return err;
}


DWORD CPco_com::PCO_GetExpTrigSignalStatus(WORD* exptrgsignal)
{
    SC2_Simple_Telegram com;
    SC2_ExpTrig_Signal_Status_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_EXP_TRIG_SIGNAL_STATUS;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *exptrgsignal = resp.wStatus;

    return err;
}

DWORD CPco_com::PCO_GetCOCRuntime(DWORD *time_s,DWORD *time_ns)
{
  SC2_COC_Runtime_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_COC_RUNTIME;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
    *time_s=resp.dwtime_s;
    *time_ns=resp.dwtime_ns;
  }

  return err;
}

DWORD CPco_com::PCO_GetCOCExptime(DWORD* time_s,DWORD* time_ns)
{
    SC2_Simple_Telegram com;
    SC2_COC_Exptime_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_COC_EXPTIME;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *time_s=resp.dwtime_s;
        *time_ns=resp.dwtime_ns;
    }
    return err;
}

DWORD CPco_com::PCO_GetImageTiming (SC2_Get_Image_Timing_Response *image_timing)
{
    SC2_Simple_Telegram com;
    SC2_Get_Image_Timing_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_IMAGE_TIMING;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        memcpy(image_timing,&resp,sizeof(SC2_Get_Image_Timing_Response));

    return err;
}

DWORD CPco_com::PCO_GetTriggerMode(WORD *mode)
{
  DWORD err;
  SC2_Simple_Telegram com;
  SC2_Trigger_Mode_Response resp;

  com.wCode=GET_TRIGGER_MODE;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
    *mode=resp.wMode;

  return err;
}

DWORD CPco_com::PCO_SetTriggerMode(WORD mode)
{
  DWORD err;
  SC2_Set_Trigger_Mode com;
  SC2_Trigger_Mode_Response resp;

  com.wCode=SET_TRIGGER_MODE;
  com.wMode=mode;
  com.wSize=sizeof(SC2_Set_Trigger_Mode);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_com::PCO_ForceTrigger(WORD *trigger)
{
  DWORD err;
  SC2_Simple_Telegram com;
  SC2_Force_Trigger_Response resp;

  com.wCode=FORCE_TRIGGER;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  *trigger=resp.wReturn;
  return err;
}

DWORD CPco_com::PCO_GetPixelRate(DWORD *PixelRate)
{
  DWORD err;

  SC2_Simple_Telegram com;
  SC2_Pixelrate_Response resp;

  com.wCode=GET_PIXELRATE;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err==PCO_NOERROR)
    *PixelRate=resp.dwPixelrate;

  return err;
}


DWORD CPco_com::PCO_SetPixelRate(DWORD PixelRate)
{
  DWORD err;

  SC2_Set_Pixelrate com;
  SC2_Pixelrate_Response resp;

  com.wCode=SET_PIXELRATE;
  com.dwPixelrate=PixelRate;
  com.wSize=sizeof(SC2_Set_Pixelrate);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_com::PCO_GetDelayExposureTime(DWORD *delay,DWORD *expos,WORD *tb_delay,WORD *tb_expos)
{
  DWORD err;

  err=PCO_GetTimebase(tb_delay,tb_expos);
  if(err==PCO_NOERROR)
    err=PCO_GetDelayExposure(delay,expos);
  return err;
}

DWORD CPco_com::PCO_SetDelayExposureTime(DWORD delay,DWORD expos,WORD tb_delay,WORD tb_expos)
{
  DWORD err;

  err=PCO_SetTimebase(tb_delay,tb_expos);
  if(err==PCO_NOERROR)
    err=PCO_SetDelayExposure(delay,expos);

  return err;
}

DWORD CPco_com::PCO_GetDelayExposure(DWORD *delay,DWORD *expos)
{
  DWORD err;

  SC2_Simple_Telegram com;
  SC2_Delay_Exposure_Response resp;

  com.wCode=GET_DELAY_EXPOSURE_TIME;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
    *delay=resp.dwDelay;
    *expos=resp.dwExposure;
  }

  return err;
}

DWORD CPco_com::PCO_SetDelayExposure(DWORD delay,DWORD expos)
{
  DWORD err;

  SC2_Set_Delay_Exposure com;
  SC2_Delay_Exposure_Response resp;

  com.wCode=SET_DELAY_EXPOSURE_TIME;
  com.wSize=sizeof(SC2_Set_Delay_Exposure);
  com.dwDelay=delay;
  com.dwExposure=expos;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_com::PCO_GetTimebase(WORD *delay,WORD *expos)
{
  DWORD err;

  SC2_Simple_Telegram com;
  SC2_Timebase_Response resp;

  com.wCode=GET_TIMEBASE;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
    *delay=resp.wTimebaseDelay;
    *expos=resp.wTimebaseExposure;
  }

  return err;
}

DWORD CPco_com::PCO_SetTimebase(WORD delay,WORD expos)
{
  DWORD err;

  SC2_Set_Timebase com;
  SC2_Timebase_Response resp;

  com.wCode=SET_TIMEBASE;
  com.wSize=sizeof(SC2_Set_Timebase);
  com.wTimebaseDelay=delay;
  com.wTimebaseExposure=expos;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}


DWORD CPco_com::PCO_GetFrameRate (WORD* wFrameRateStatus, DWORD* dwFrameRate, DWORD* dwFrameRateExposure)
{
    SC2_Get_Framerate com;
    SC2_Get_Framerate_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_FRAMERATE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wFrameRateStatus    = resp.wStatus;
        *dwFrameRate   	  = resp.dwFramerate;
        *dwFrameRateExposure = resp.dwExposure;
    }
    return err;
}

DWORD CPco_com::PCO_SetFrameRate (WORD* wFrameRateStatus, WORD wFramerateMode, DWORD* dwFrameRate, DWORD* dwFrameRateExposure)
{
    SC2_Set_Framerate com;
    SC2_Set_Framerate_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_FRAMERATE;
    com.wMode=wFramerateMode;
    com.dwFramerate=*dwFrameRate;
    com.dwExposure=*dwFrameRateExposure;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wFrameRateStatus=resp.wStatus;
        *dwFrameRate=resp.dwFramerate;
        *dwFrameRateExposure=resp.dwExposure;
    }

    return err;
}

DWORD CPco_com::PCO_GetFPSExposureMode(WORD* wFPSExposureMode,DWORD* dwFPSExposureTime)
{
    SC2_Simple_Telegram com;
    SC2_FPS_Exposure_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_FPS_EXPOSURE_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wFPSExposureMode  = resp.wMode;
        *dwFPSExposureTime = resp.dwExposure;
    }
    return err;
}

DWORD CPco_com::PCO_SetFPSExposureMode(WORD wFPSExposureMode,DWORD* dwFPSExposureTime)
{
    SC2_Set_FPS_Exposure_Mode com;
    SC2_FPS_Exposure_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_FPS_EXPOSURE_MODE;
    com.wMode=wFPSExposureMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *dwFPSExposureTime = resp.dwExposure;

    return err;
}

DWORD CPco_com::PCO_GetSensorFormat(WORD *wSensor)
{
  DWORD err;
  SC2_Simple_Telegram com;
  SC2_Sensor_Format_Response resp;

  com.wCode=GET_SENSOR_FORMAT;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
    *wSensor=resp.wFormat;
  return err;
}

DWORD CPco_com::PCO_SetSensorFormat(WORD wSensor)
{
  DWORD err;
  SC2_Set_Sensor_Format com;
  SC2_Sensor_Format_Response resp;

  com.wCode=SET_SENSOR_FORMAT;
  com.wSize=sizeof(com);
  com.wFormat=wSensor;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_com::PCO_GetROI(WORD *RoiX0,WORD *RoiY0,WORD *RoiX1,WORD *RoiY1)
{
  DWORD err;

  SC2_Simple_Telegram com;
  SC2_ROI_Response resp;

  com.wCode=GET_ROI;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
    *RoiX0=resp.wROI_x0;
    *RoiX1=resp.wROI_x1;
    *RoiY0=resp.wROI_y0;
    *RoiY1=resp.wROI_y1;
  }
  return err;
}

DWORD CPco_com::PCO_SetROI(WORD RoiX0,WORD RoiY0,WORD RoiX1,WORD RoiY1)
{
  DWORD err;

  SC2_Set_ROI com;
  SC2_ROI_Response resp;
  com.wCode=SET_ROI;
  com.wSize=sizeof(SC2_Set_ROI);
  com.wROI_x0=RoiX0;
  com.wROI_x1=RoiX1;
  com.wROI_y0=RoiY0;
  com.wROI_y1=RoiY1;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_com::PCO_GetBinning(WORD *BinHorz,WORD *BinVert)
{
  DWORD err;

  SC2_Simple_Telegram com;
  SC2_Binning_Response resp;

  com.wCode=GET_BINNING;
  com.wSize=sizeof(SC2_Simple_Telegram);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
    *BinHorz=resp.wBinningx;
    *BinVert=resp.wBinningy;
  }
  return err;
}

DWORD CPco_com::PCO_SetBinning(WORD BinHorz,WORD BinVert)
{
  DWORD err;

  SC2_Set_Binning com;
  SC2_Binning_Response resp;

  com.wCode=SET_BINNING;
  com.wSize=sizeof(SC2_Set_Binning);
  com.wBinningx=BinHorz;
  com.wBinningy=BinVert;

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_com::PCO_GetADCOperation(WORD* wADCOperation)
{
    SC2_Simple_Telegram com;
    SC2_ADC_Operation_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_ADC_OPERATION;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wADCOperation = resp.wMode;

    return err;
}

DWORD CPco_com::PCO_SetADCOperation(WORD num)
{
  DWORD err;

  SC2_Set_ADC_Operation com;
  SC2_ADC_Operation_Response resp;

  com.wCode=SET_ADC_OPERATION;
  com.wMode=num;
  com.wSize=sizeof(SC2_Set_ADC_Operation);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_com::PCO_GetDoubleImageMode(WORD* wDoubleImage)
{
    SC2_Simple_Telegram com;
    SC2_Double_Image_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_DOUBLE_IMAGE_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wDoubleImage = resp.wMode;

    return err;

}

DWORD CPco_com::PCO_SetDoubleImageMode(WORD wDoubleImage)
{
    SC2_Set_Double_Image_Mode com;
    SC2_Double_Image_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_DOUBLE_IMAGE_MODE;
    com.wMode=wDoubleImage;
    com.wSize=sizeof(com);


    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));


    return err;
}

DWORD CPco_com::PCO_GetNoiseFilterMode (WORD* wNoiseFilterMode)
{
    SC2_Simple_Telegram com;
    SC2_Noise_Filter_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_NOISE_FILTER_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wNoiseFilterMode = resp.wMode;

    return err;
}


DWORD CPco_com::PCO_SetNoiseFilterMode (WORD wNoiseFilterMode)
{
    SC2_Set_Noise_Filter_Mode com;
    SC2_Noise_Filter_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_NOISE_FILTER_MODE;
    com.wMode=wNoiseFilterMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetConversionFactor(WORD* wConvFact)
{
    SC2_Simple_Telegram com;
    SC2_Conversion_Factor_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CONVERSION_FACTOR;
    com.wSize=sizeof(com);
    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wConvFact = resp.wGain;

    return err;
}

DWORD CPco_com::PCO_SetConversionFactor(WORD wConvFact)
{
    SC2_Set_Conversion_Factor com;
    SC2_Conversion_Factor_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_CONVERSION_FACTOR;
    com.wGain=wConvFact;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetIRSensitivity(WORD* wIR)
{
    SC2_Simple_Telegram com;
    SC2_IR_Sensitivity_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_IR_SENSITIVITY;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wIR = resp.wMode;

    return err;
}


DWORD CPco_com::PCO_SetIRSensitivity(WORD wIR)
{
    SC2_Set_IR_Sensitivity com;
    SC2_IR_Sensitivity_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_IR_SENSITIVITY;
    com.wMode=wIR;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetOffsetMode (WORD* wOffsetRegulation)
{
    SC2_Simple_Telegram com;
    SC2_Offset_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_OFFSET_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wOffsetRegulation = resp.wMode;

    return err;
}

DWORD CPco_com::PCO_SetOffsetMode (WORD wOffsetRegulation)
{
    SC2_Set_Offset_Mode com;
    SC2_Offset_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_OFFSET_MODE;
    com.wMode=wOffsetRegulation;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetCoolingSetpointTemperature(SHORT* sCoolSet)
{
    SC2_Simple_Telegram com;
    SC2_Cooling_Setpoint_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_COOLING_SETPOINT_TEMPERATURE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *sCoolSet = resp.sTemp;

    return err;
}


DWORD CPco_com::PCO_SetCoolingSetpointTemperature(SHORT sCoolSet)
{
    SC2_Set_Cooling_Setpoint com;
    SC2_Cooling_Setpoint_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_COOLING_SETPOINT_TEMPERATURE;
    com.sTemp=sCoolSet;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetCoolingSetpoints(WORD wBlockID,SHORT* sSetPoints,WORD* wValidSetPoints)
{
    SC2_Get_Cooling_Setpoints com;
    SC2_Get_Cooling_Setpoints_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_COOLING_SETPOINTS;
    com.wBlockID=wBlockID;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        if(*wValidSetPoints < resp.wValidSetPoints) {
            // Function call error
        }
        else {
            memcpy(sSetPoints,&resp.sSetPoints,resp.wValidSetPoints);
            *wValidSetPoints = resp.wValidSetPoints;
        }
    }
    return err;
}

DWORD CPco_com::PCO_GetStorageMode(WORD* wStorageMode)
{
    SC2_Simple_Telegram com;
    SC2_Storage_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_STORAGE_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wStorageMode = resp.wMode;

    return err;
}


DWORD CPco_com::PCO_SetStorageMode(WORD wStorageMode)
{
    SC2_Set_Storage_Mode com;
    SC2_Storage_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_STORAGE_MODE;
    com.wMode=wStorageMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;

}

DWORD CPco_com::PCO_GetRecorderSubmode(WORD* wRecSubmode)
{
    SC2_Simple_Telegram com;
    SC2_Recorder_Submode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_RECORDER_SUBMODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wRecSubmode = resp.wMode;

    return err;


}

DWORD CPco_com::PCO_SetRecorderSubmode(WORD wRecSubmode)
{
    SC2_Set_Recorder_Submode com;
    SC2_Recorder_Submode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_RECORDER_SUBMODE;
    com.wMode=wRecSubmode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetAcquireMode(WORD* wAcquMode)
{
    SC2_Simple_Telegram com;
    SC2_Acquire_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_ACQUIRE_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wAcquMode = resp.wMode;

    return err;

}

DWORD CPco_com::PCO_SetAcquireMode(WORD wAcquMode)
{
    SC2_Set_Acquire_Mode com;
    SC2_Acquire_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_ACQUIRE_MODE;
    com.wMode=wAcquMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetAcquireModeEx(WORD* wAcquMode, DWORD* dwNumberImages)
{
    SC2_Get_Acquire_Mode_Ex com;
    SC2_Acquire_Mode_Ex_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_ACQUIRE_MODE_EX;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wAcquMode      = resp.wMode;
        *dwNumberImages = resp.dwNumberImages;
    }
    return err;

}

DWORD CPco_com::PCO_SetAcquireModeEx(WORD wAcquMode, DWORD dwNumberImages)
{
    SC2_Set_Acquire_Mode_Ex com;
    SC2_Acquire_Mode_Ex_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_ACQUIRE_MODE_EX;
    com.wMode=wAcquMode;
    com.dwNumberImages=dwNumberImages;
    com.dwReserved[0]=0;
    com.dwReserved[1]=0;
    com.dwReserved[2]=0;
    com.dwReserved[3]=0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetAcquireControl(WORD* wMode)
{
    SC2_Get_Acquire_Control com;
    SC2_Acquire_Control_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_ACQUIRE_CONTROL;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wMode=resp.wMode;

    return err;
}

DWORD CPco_com::PCO_SetAcquireControl(WORD wMode)
{
    SC2_Set_Acquire_Control com;
    SC2_Acquire_Control_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_ACQUIRE_CONTROL;
    com.wMode=wMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetAcqEnblSignalStatus(WORD* wAcquEnableState)
{
    SC2_Simple_Telegram com;
    SC2_acqenbl_Signal_Status_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_ACQ_ENBL_SIGNAL_STATUS;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wAcquEnableState=resp.wStatus;

    return err;
}


DWORD CPco_com::PCO_GetRecordStopEvent(WORD* wRecordStopEventMode,DWORD* dwRecordStopDelayImages)
{
    SC2_Get_Record_Stop_Event com;
    SC2_Record_Stop_Event_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_RECORD_STOP_EVENT;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wRecordStopEventMode=resp.wMode;
        *dwRecordStopDelayImages=resp.dwDelayImages;
    }

    return err;
}

DWORD CPco_com::PCO_SetRecordStopEvent(WORD wRecordStopEventMode,DWORD dwRecordStopDelayImages)
{
    SC2_Set_Record_Stop_Event com;
    SC2_Record_Stop_Event_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_RECORD_STOP_EVENT;
    com.wMode=wRecordStopEventMode;
    com.dwDelayImages=dwRecordStopDelayImages;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_StopRecord(WORD* wReserved0, DWORD *dwReserved1)
{
    SC2_Stop_Record com;
    SC2_Stop_Record_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=STOP_RECORD;
    com.wReserved0=*wReserved0;
    com.dwReserved1=*dwReserved1;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;

}

DWORD CPco_com::PCO_GetEventMonConfiguration(WORD* wConfig)
{
    SC2_Get_Event_Monitor_Configuration com;
    SC2_Event_Monitor_Configuration_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_EVENT_MON_CONFIGURATION;
    com.wReserved=0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wConfig=resp.wConfig;

    return err;

}

DWORD CPco_com::PCO_SetEventMonConfiguration(WORD wConfig)
{
    SC2_Set_Event_Monitor_Configuration com;
    SC2_Event_Monitor_Configuration_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_EVENT_MON_CONFIGURATION;
    com.wConfig=wConfig;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetEventList(WORD wIndex,WORD *wMaxEvents,WORD* wValidEvents,WORD* wValidEventsInTelegram,
                                 SC2_EVENT_LIST_ENTRY* list)
{
    SC2_Get_Event_List com;
    SC2_Get_Event_List_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_EVENT_LIST;
    com.wIndex=wIndex;
    com.wReserved=0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wMaxEvents=resp.wMaxEvents;
        *wValidEvents=resp.wValidEvents;
        *wValidEventsInTelegram=resp.wValidEventsInTelegram;
        for (int i = 0;i < resp.wValidEventsInTelegram;i++) {
            list[i] = resp.strEvent[i];
        }
    }

    return err;
}





//@@@





