//-----------------------------------------------------------------//
// Name        | Cpco_com_func_2.cpp         | Type: (*) source    //
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
// Author      | JEI, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.00                                         //
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
//  1.00     | 20.06.2014 |  new file                              //
//           |            |                                        //
//-----------------------------------------------------------------//


#include "Cpco_com.h"


/////////////////////////////////////////////////////////////////////
// storage control
/////////////////////////////////////////////////////////////////////

DWORD CPco_com::PCO_GetCameraRamSize(DWORD* dwRamSize, WORD* wPageSize)
{
    SC2_Simple_Telegram com;
    SC2_Camera_RAM_Size_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CAMERA_RAM_SIZE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *dwRamSize = resp.dwRamSize;
        *wPageSize = resp.wPageSize;
    }

    return err;
}

DWORD CPco_com::PCO_GetCameraRamSegmentSize(DWORD* dwRamSegSize)
{
    SC2_Simple_Telegram com;
    SC2_Camera_RAM_Segment_Size_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CAMERA_RAM_SEGMENT_SIZE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        dwRamSegSize[0]=resp.dwSegment1Size;
        dwRamSegSize[1]=resp.dwSegment2Size;
        dwRamSegSize[2]=resp.dwSegment3Size;
        dwRamSegSize[3]=resp.dwSegment4Size;
    }

    return err;
}

DWORD CPco_com::PCO_SetCameraRamSegmentSize(DWORD* dwRamSegSize)
{
    SC2_Set_Camera_RAM_Segment_Size com;
    SC2_Camera_RAM_Segment_Size_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_CAMERA_RAM_SEGMENT_SIZE;
    com.dwSegment1Size = dwRamSegSize[0];
    com.dwSegment2Size = dwRamSegSize[1];
    com.dwSegment3Size = dwRamSegSize[2];
    com.dwSegment4Size = dwRamSegSize[3];
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetActiveRamSegment(WORD* wActSeg)
{
    SC2_Simple_Telegram com;
    SC2_Active_RAM_Segment_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_ACTIVE_RAM_SEGMENT;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wActSeg = resp.wSegment;

    return err;
}

DWORD CPco_com::PCO_SetActiveRamSegment(WORD wActSeg)
{
    SC2_Set_Active_RAM_Segment com;
    SC2_Active_RAM_Segment_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_ACTIVE_RAM_SEGMENT;
    com.wSegment=wActSeg;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_ClearRamSegment(void)
{
    SC2_Simple_Telegram com;
    SC2_Clear_RAM_Segment_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=CLEAR_RAM_SEGMENT;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetNumberOfImagesInSegment(WORD wSegment,DWORD* dwValid,DWORD* dwMax)
{
    SC2_Number_of_Images com;
    SC2_Number_of_Images_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_NUMBER_OF_IMAGES_IN_SEGMENT;
    com.wSegment=wSegment;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *dwValid=resp.dwValid;
        *dwMax=resp.dwMax;
    }
    return err;
}

DWORD CPco_com::PCO_GetSegmentImageSettings(WORD wSegment,WORD* wRes_hor,WORD* wRes_ver,WORD* wBin_x,WORD* wBin_y,
                                            WORD* wRoi_x0,WORD* wRoi_y0,WORD* wRoi_x1,WORD* wRoi_y1)
{
    SC2_Segment_Image_Settings com;
    SC2_Segment_Image_Settings_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_SEGMENT_IMAGE_SETTINGS;
    com.wSegment=wSegment;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wRes_hor=resp.wRes_hor;
        *wRes_ver=resp.wRes_ver;
        *wBin_x=resp.wBin_x;
        *wBin_y=resp.wBin_y;
        *wRoi_x0=resp.wRoi_x0;
        *wRoi_y0=resp.wRoi_y0;
        *wRoi_x1=resp.wRoi_x1;
        *wRoi_y1=resp.wRoi_y1;
    }

    return err;
}

DWORD CPco_com::PCO_ReadImagesFromSegment(WORD wSegment,DWORD dwStartImage,DWORD dwLastImage)
{
    SC2_Read_Images_from_Segment com;
    SC2_Read_Images_from_Segment_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=READ_IMAGES_FROM_SEGMENT;
    com.wSize=sizeof(com);
    com.wSegment=wSegment;
    com.dwStartImage=dwStartImage;
    com.dwLastImage=dwLastImage;

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
    return err;
}


DWORD CPco_com::PCO_RequestImage()
{
  SC2_Request_Image com;
  SC2_Request_Image_Response resp;
  DWORD err;

  //send request to camera
  com.wCode=REQUEST_IMAGE;
  com.wSize=sizeof(SC2_Request_Image);
  err=Control_Command(&com,sizeof(SC2_Request_Image),
    &resp,sizeof(SC2_Request_Image_Response));


  if(err!=PCO_NOERROR)
    writelog(ERROR_M,hdriver,"Send_Request_Image: err 0x%x com 0x%x",err,com.wCode);

  return err;
}


DWORD CPco_com::PCO_RepeatImage(void)
{
    SC2_Repeat_Image com;
    SC2_Repeat_Image_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=REPEAT_IMAGE;
    com.Reserved[0]=0;
    com.Reserved[1]=0;
    com.Reserved[2]=0;
    com.Reserved[3]=0;

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}


DWORD CPco_com::PCO_CancelImageTransfer(void)
{
    SC2_Cancel_Image_Transfer com;
    SC2_Cancel_Image_Transfer_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=CANCEL_IMAGE_TRANSFER;
    com.Reserved=0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_CancelImage()
{
    DWORD err;
    SC2_Simple_Telegram com;
    SC2_Cancel_Image_Transfer_Response resp;

    com.wCode=CANCEL_IMAGE_TRANSFER;
    com.wSize=sizeof(SC2_Simple_Telegram);

    err=Control_Command(&com,sizeof(com),
                        &resp,sizeof(resp));
    return err;
}




DWORD CPco_com::PCO_PlayImagesFromSegment(WORD wSegment,WORD wInterface,WORD wMode,WORD wSpeed,DWORD dwRangeLow,
                                          DWORD dwRangeHigh,DWORD dwStartPos)
{
    SC2_Play_Images_from_Segment com;
    SC2_Play_Images_from_Segment_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=PLAY_IMAGES_FROM_SEGMENT;
    com.wSegment=wSegment;
    com.wInterface=wInterface;
    com.wMode=wMode;
    com.wSpeed=wSpeed;
    com.dwRangeLow=dwRangeLow;
    com.dwRangeHigh=dwRangeHigh;
    com.dwStartPos=dwStartPos;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetPlayPosition(WORD* wStatus,DWORD *dwPosition)
{
    SC2_Get_Play_Position com;
    SC2_Get_Play_Position_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_PLAY_POSITION;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wStatus=resp.wStatus;
        *dwPosition=resp.dwPosition;
    }
    return err;
}


DWORD CPco_com::PCO_SetVideoPayloadIdentifier(WORD wSegment,WORD wMode1,WORD wMode2,DWORD dwSetPos1,DWORD dwClrPos1,
                                              DWORD dwSetPos2,DWORD dwClrPos2,DWORD dwSetPos3,DWORD dwClrPos3,DWORD dwSetPos4,DWORD dwClrPos4)
{
    SC2_Set_Video_Payload_Identifier com;
    SC2_Set_Video_Payload_Identifier_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_VIDEO_PAYLOAD_IDENTIFIER;
    com.wSegment=wSegment;
    com.wMode1=wMode1;
    com.wMode2=wMode2;
    com.dwSetPos1=dwSetPos1;
    com.dwClrPos1=dwClrPos1;
    com.dwSetPos2=dwSetPos2;
    com.dwClrPos2=dwClrPos2;
    com.dwSetPos3=dwSetPos3;
    com.dwClrPos3=dwClrPos3;
    com.dwSetPos4=dwSetPos4;
    com.dwClrPos4=dwClrPos4;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}



DWORD CPco_com::PCO_GetColorSettings(SC2_Get_Color_Settings_Response* ColSetResp)
{
    SC2_Get_Color_Settings com;
    SC2_Get_Color_Settings_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_COLOR_SETTINGS;
    com.wReserved=0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if (err==PCO_NOERROR)
        memcpy(ColSetResp,&resp,sizeof(SC2_Get_Color_Settings_Response));

    return err;
}

DWORD CPco_com::PCO_SetColorSettings(SC2_Set_Color_Settings* SetColSet)
{
    SC2_Set_Color_Settings com;
    SC2_Set_Color_Settings_Response resp;
    DWORD err=PCO_NOERROR;

    memcpy(&com,SetColSet,sizeof(SC2_Set_Color_Settings));
    com.wCode=SET_COLOR_SETTINGS;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_DoWhiteBalance(WORD wMode)
{
    SC2_Do_White_Balance com;
    SC2_White_Balance_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=DO_WHITE_BALANCE;
    com.wMode=wMode;
    com.wParam[0]=0;
    com.wParam[1]=0;
    com.wParam[2]=0;
    com.wParam[3]=0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetWhiteBalanceStatus(WORD* wStatus,WORD* wColorTemp,SHORT* sTint)
{
    SC2_Get_White_Balance_Status com;
    SC2_White_Balance_Status_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_WHITE_BALANCE_STATUS;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if (err==PCO_NOERROR) {
        *wStatus=resp.wStatus;
        *wColorTemp=resp.wColorTemp;
        *sTint=resp.sTint;
    }
    return err;
}

DWORD CPco_com::PCO_GetImageTransferMode(WORD* wMode,WORD* wImageWidth,WORD* wImageHeight,WORD* wTxWidth,
                                         WORD* wTxHeight,WORD* wTxLineWordCnt,WORD* wParam,WORD* wParamLen)
{
    SC2_Get_Image_Transfer_Mode com;
    SC2_Image_Transfer_Mode_Response resp;
    DWORD err=PCO_NOERROR;
    WORD len;

    com.wCode=GET_IMAGE_TRANSFER_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
    {
        *wMode=resp.wMode;
        *wImageWidth=resp.wImageWidth;
        *wImageHeight=resp.wImageHeight;
        *wTxWidth=resp.wTxWidth;
        *wTxHeight=resp.wTxHeight;
        *wTxLineWordCnt=resp.wTxLineWordCnt;
        if(*wParamLen > 8)
            return err;
        else
            len=*wParamLen;

        memcpy(wParam,&resp.wParam,len*sizeof(WORD));
    }
    return err;
}

DWORD CPco_com::PCO_SetImageTransferMode(WORD wMode,WORD wImageWidth,WORD wImageHeight,WORD wTxWidth,
                                         WORD wTxHeight,WORD wTxLineWordCnt,WORD* wParam,WORD wParamLen)
{
    SC2_Set_Image_Transfer_Mode com;
    SC2_Image_Transfer_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_IMAGE_TRANSFER_MODE;
    com.wMode=wMode;
    com.wImageWidth=wImageWidth;
    com.wImageHeight=wImageHeight;
    com.wTxWidth=wTxWidth;
    com.wTxHeight=wTxHeight;
    com.wTxLineWordCnt=wTxLineWordCnt;
    com.wSize=sizeof(com);

    for (int i=0;i < wParamLen;i++) {
        com.wParam[i]=wParam[i];
    }

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}


DWORD CPco_com::PCO_GetLookupableInfo(WORD wLUTNum, WORD* wNumberOfLuts, char* Description,WORD wDescLen,
                                      WORD* wIdentifier, BYTE* bInputWidth,BYTE* bOutputWidth, WORD* wFormat)
{
    SC2_Get_Lookuptable_Info com;
    SC2_Get_Lookuptable_Info_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_LOOKUPTABLE_INFO;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wNumberOfLuts=resp.wLutNumber;
        memcpy(Description,&resp.LutDesc[wLUTNum].Description,wDescLen);
        *wIdentifier=resp.LutDesc[wLUTNum].wIdentifier;
        *bInputWidth=resp.LutDesc[wLUTNum].bInputWidth;
        *bOutputWidth=resp.LutDesc[wLUTNum].bOutputWidth;
        *wFormat=resp.LutDesc[wLUTNum].wFormat;
    }

    return err;
}

DWORD CPco_com::PCO_GetLut(WORD *Identifier,WORD *Parameter)
{
  SC2_Simple_Telegram com;
  SC2_Set_Lookuptable_Response resp;
  DWORD err;

  com.wCode=GET_LOOKUPTABLE;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
    *Identifier=resp.wIdentifier;
    *Parameter=resp.wParameter;
  }
  return err;
}


DWORD CPco_com::PCO_SetLut(WORD Identifier,WORD Parameter)
{
  DWORD err;

  SC2_Set_Lookuptable com;
  SC2_Set_Lookuptable_Response resp;

  com.wCode=SET_LOOKUPTABLE;
  com.wIdentifier=Identifier;
  com.wParameter=Parameter;

  com.wSize=sizeof(SC2_Set_Lookuptable);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}



DWORD CPco_com::PCO_GetBitAlignment(WORD *align)
{
  SC2_Bit_Alignment_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_BIT_ALIGNMENT;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  *align=resp.wAlignment;
  return err;

}

DWORD CPco_com::PCO_SetBitAlignment(WORD align)
{
  DWORD err;

  SC2_Set_Bit_Alignment com;
  SC2_Bit_Alignment_Response resp;

  com.wCode=SET_BIT_ALIGNMENT;
  com.wAlignment=align;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  return err;
}

DWORD CPco_com::PCO_GetTimestampMode(WORD *mode)
{
  SC2_Timestamp_Mode_Response resp;
  SC2_Simple_Telegram com;
  DWORD err=PCO_NOERROR;

  com.wCode=GET_TIMESTAMP_MODE;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  *mode=resp.wMode;

  return err;
}

DWORD CPco_com::PCO_SetTimestampMode(WORD mode)
{
  SC2_Timestamp_Mode_Response resp;
  SC2_Set_Timestamp_Mode com;
  DWORD err=PCO_NOERROR;

  com.wMode=mode;

  com.wCode=SET_TIMESTAMP_MODE;
  com.wSize=sizeof(com);
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  return err;
}

DWORD CPco_com::PCO_GetHotPixelCorrectionMode (WORD* wHotPixelCorrectionMode)
{
    SC2_Simple_Telegram com;
    SC2_Hot_Pixel_Correction_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_HOT_PIXEL_CORRECTION_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wHotPixelCorrectionMode = resp.wMode;

    return err;
}

DWORD CPco_com::PCO_SetHotPixelCorrectionMode (WORD wHotPixelCorrectionMode)
{
    SC2_Set_Hot_Pixel_Correction_Mode com;
    SC2_Hot_Pixel_Correction_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_HOT_PIXEL_CORRECTION_MODE;
    com.wMode=wHotPixelCorrectionMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetMetadataMode(WORD* wMode,WORD* wMetadataSize,WORD* wMetadataVersion)
{
    SC2_Get_Metadata_Mode com;
    SC2_Metadata_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_METADATA_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if (err==PCO_NOERROR) {
        *wMode=resp.wMode;
        *wMetadataSize=resp.wMetadataSize;
        *wMetadataVersion=resp.wMetadataVersion;
    }

    return err;
}

DWORD CPco_com::PCO_SetMetadataMode(WORD wMode,WORD* wMetadataSize,WORD* wMetadataVersion)
{
    SC2_Set_Metadata_Mode com;
    SC2_Metadata_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_METADATA_MODE;
    com.wMode=wMode;
    com.wReserved1=0;
    com.wReserved2=0;
    com.wSize=sizeof(com);
    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
    if (err==PCO_NOERROR) {
        *wMetadataSize=resp.wMetadataSize;
        *wMetadataVersion=resp.wMetadataVersion;
    }

    return err;

}




DWORD CPco_com::PCO_GetHWIOSignalCount(WORD *numSignals)
{
  DWORD err;
  SC2_Simple_Telegram com;
  SC2_Get_Num_HW_IO_Signals_Response resp;

  com.wCode=GET_NUMBER_HW_IO_SIGNALS;
  com.wSize=sizeof(SC2_Simple_Telegram);

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
    *numSignals=resp.wNumOfSignals;

  return err;
}


DWORD CPco_com::PCO_GetHWIOSignalDescriptor(WORD SignalNum,SC2_Get_HW_IO_Signal_Descriptor_Response *SignalDesc)
{
  DWORD err;
  SC2_Get_HW_IO_Signal_Descriptor com;
  SC2_Get_HW_IO_Signal_Descriptor_Response resp;

  com.wCode=GET_HW_IO_SIGNAL_DESCRIPTION;
  com.wSize=sizeof(SC2_Get_HW_IO_Signal_Descriptor);
  com.wNumSignal=SignalNum;

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err==PCO_NOERROR)
    memcpy(SignalDesc,&resp,sizeof(SC2_Get_HW_IO_Signal_Descriptor_Response));
  else
    memset(SignalDesc,0,sizeof(SC2_Get_HW_IO_Signal_Descriptor_Response));

  return err;
}


DWORD CPco_com::PCO_GetHWIOSignalDescriptor(WORD SignalNum,char *outbuf,int *size)
{
  DWORD err;
  char buffer[100];
  int done,len,siz,size_in;

  SC2_Get_HW_IO_Signal_Descriptor com;
  SC2_Get_HW_IO_Signal_Descriptor_Response resp;

  com.wCode=GET_HW_IO_SIGNAL_DESCRIPTION;
  com.wSize=sizeof(SC2_Get_HW_IO_Signal_Descriptor);
  com.wNumSignal=SignalNum;

  size_in=0;
  done=0;
  if(outbuf)
  {
    if(size)
      size_in=*size;
    else
      return PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
  }
  else
    siz=1;

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err==PCO_NOERROR)
  {
    sprintf(buffer,"HWIOSignalDescriptor for Output %d\n",SignalNum);
    len=istrlen(buffer);
    if(outbuf)
    {
      if(done+len+1<=size_in)
      {
        strcat(outbuf,buffer);
        done+=len;
      }
    }
    else
      siz+=len;

    sprintf(buffer,"possible selections:\n");
    len=istrlen(buffer);
    if(outbuf)
    {
      if(done+len+1<=size_in)
      {
        strcat(outbuf,buffer);
        done+=len;
      }
    }
    else
      siz+=len;

    if(strlen(resp.szSignalName[0]))
    {
      sprintf(buffer,"0: %s\n",resp.szSignalName[0]);
      len=istrlen(buffer);
      if(outbuf)
      {
        if(done+len+1<=size_in)
        {
          strcat(outbuf,buffer);
          done+=len;
        }
      }
      else
        siz+=len;
    }
    if(strlen(resp.szSignalName[1]))
    {
      sprintf(buffer,"1: %s\n",resp.szSignalName[1]);
      len=istrlen(buffer);
      if(outbuf)
      {
        if(done+len+1<=size_in)
        {
          strcat(outbuf,buffer);
          done+=len;
        }
      }
      else
        siz+=len;
    }
    if(strlen(resp.szSignalName[2]))
    {
      sprintf(buffer,"2: %s\n",resp.szSignalName[2]);
      len=istrlen(buffer);
      if(outbuf)
      {
        if(done+len+1<=size_in)
        {
          strcat(outbuf,buffer);
          done+=len;
        }
      }
      else
        siz+=len;

    }
    if(strlen(resp.szSignalName[3]))
    {
      sprintf(buffer,"3: %s\n",resp.szSignalName[3]);
      len=istrlen(buffer);
      if(outbuf)
      {
        if(done+len+1<=size_in)
        {
          strcat(outbuf,buffer);
          done+=len;
        }
      }
      else
        siz+=len;
    }

    sprintf(buffer,"selectable signal types:\n");
    len=istrlen(buffer);
    if(outbuf)
    {
      if(done+len+1<=size_in)
      {
        strcat(outbuf,buffer);
        done+=len;
      }
    }
    else
      siz+=len;

    memset(buffer,0,sizeof(buffer));
    if(resp.wSignalTypes&0x01)
      strcat(buffer,"TTL\n");
    if(resp.wSignalTypes&0x02)
      strcat(buffer,"High level TTL\n");
    if(resp.wSignalTypes&0x04)
      strcat(buffer,"Contact mode\n");
    if(resp.wSignalTypes&0x08)
      strcat(buffer,"RS485 differential\n");
    len=istrlen(buffer);
    if(len==0)
      strcat(buffer,"None\n");

    if(outbuf)
    {
      if(done+len+1<=size_in)
      {
        strcat(outbuf,buffer);
        done+=len;
      }
    }
    else
      siz+=len;

    sprintf(buffer,"selectable signal polarity:\n");
    len=istrlen(buffer);
    if(outbuf)
    {
      if(done+len+1<=size_in)
      {
        strcat(outbuf,buffer);
        done+=len;
      }
    }
    else
      siz+=len;

    memset(buffer,0,sizeof(buffer));
    if(resp.wSignalPolarity&0x01)
      strcat(buffer,"Low level active\n");
    if(resp.wSignalPolarity&0x02)
      strcat(buffer,"High level active\n");
    if(resp.wSignalPolarity&0x04)
      strcat(buffer,"Rising edge active\n");
    if(resp.wSignalPolarity&0x08)
      strcat(buffer,"Falling edge active\n");
    len=istrlen(buffer);
    if(len==0)
      strcat(buffer,"None\n");

    if(outbuf)
    {
      if(done+len+1<=size_in)
      {
        strcat(outbuf,buffer);
        done+=len;
      }
    }
    else
      siz+=len;

    sprintf(buffer,"selectable filter settings:\n");
    len=istrlen(buffer);
    if(outbuf)
    {
      if(done+len+1<=size_in)
      {
        strcat(outbuf,buffer);
        done+=len;
      }
    }
    else
      siz+=len;

    memset(buffer,0,sizeof(buffer));
    if(resp.wSignalPolarity&0x01)
      strcat(buffer,"Off\n");
    if(resp.wSignalPolarity&0x02)
      strcat(buffer,"Medium\n");
    if(resp.wSignalPolarity&0x04)
      strcat(buffer,"High\n");
    len=istrlen(buffer);
    if(len==0)
      strcat(buffer,"None\n");
    if(outbuf)
    {
      if(done+len+1<=size_in)
      {
        strcat(outbuf,buffer);
        done+=len;
      }
    }
    else
      siz+=len;
  }

  if(size)
    *size=siz;

  return err;
}


DWORD CPco_com::PCO_GetHWIOSignal(WORD SignalNum ATTRIBUTE_UNUSED,WORD *Enabled,WORD *Type,WORD *Polarity,WORD *FilterSetting,WORD *Selected)
{
  DWORD err;
  SC2_Get_HW_IO_Signal com;
  SC2_Get_HW_IO_Signal_Response resp;

  com.wCode=GET_HW_IO_SIGNAL;
  com.wSize=sizeof(SC2_Get_HW_IO_Signal);

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err==PCO_NOERROR)
  {
    if(Enabled)
      *Enabled=resp.wEnabled;
    if(Type)
      *Type=resp.wType;
    if(Polarity)
      *Polarity=resp.wPolarity;
    if(FilterSetting)
      *FilterSetting=resp.wFilterSetting;
    if(Selected)
      *Selected=resp.wSelected;
  }

  return err;
}

DWORD CPco_com::PCO_SetHWIOSignal(WORD SignalNum,WORD Enabled,WORD Type,WORD Polarity,WORD FilterSetting,WORD Selected)
{
  DWORD err;
  SC2_Set_HW_IO_Signal com;
  SC2_Set_HW_IO_Signal_Response resp;

  com.wCode=SET_HW_IO_SIGNAL;
  com.wSize=sizeof(SC2_Set_HW_IO_Signal);
  com.wNumSignal=SignalNum;
  com.wEnabled=Enabled;
  com.wType=Type;
  com.wPolarity=Polarity;
  com.wFilterSetting=FilterSetting;
  com.wSelected=Selected;

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

  if(err==PCO_NOERROR)
  {
    if(Enabled!=resp.wEnabled)
    {
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
      printf("returned wrong Enable status %d!=%d\n",Enabled,resp.wEnabled);
    }
    if(Polarity!=resp.wPolarity)
    {
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
      printf("returned wrong Polarity %d!=%d\n",Polarity,resp.wPolarity);
    }
    if(FilterSetting!=resp.wFilterSetting)
    {
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
      printf("returned wrong FilterSetting %d!=%d\n",FilterSetting,resp.wFilterSetting);
    }
    if(Selected!=resp.wSelected)
    {
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
      printf("returned wrong Selected %d!=%d\n",Selected,resp.wSelected);
    }
  }

  return err;
}



DWORD CPco_com::PCO_GetHWIOSignalTiming(WORD SignalNum,WORD Selection,DWORD *type,DWORD *Parameter)
{
  DWORD err;
  SC2_Get_HW_IO_Signal_Timing com;
  SC2_Get_HW_IO_Signal_Timing_Response resp;

  com.wCode=GET_HW_IO_SIGNAL_TIMING;
  com.wSize=sizeof(SC2_Get_HW_IO_Signal_Timing);
  com.wNumSignal=SignalNum;
  com.wSelected=Selection;

  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err==PCO_NOERROR)
  {
    if((SignalNum!=resp.wNumSignal)||(Selection!=resp.wSelected))
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
    else
    {
      if(type)
        *type=resp.dwType;
      if(Parameter)
        *Parameter=resp.dwParameter;
    }
  }

  return err;
}


DWORD CPco_com::PCO_SetHWIOSignalTiming(WORD SignalNum,WORD Selection,DWORD Parameter)
{
  DWORD err;
  SC2_Set_HW_IO_Signal_Timing com;
  SC2_Set_HW_IO_Signal_Timing_Response resp;


  com.wCode=SET_HW_IO_SIGNAL_TIMING;
  com.wSize=sizeof(SC2_Set_HW_IO_Signal_Timing);
  com.wNumSignal=SignalNum;
  com.wSelected=Selection;
  com.dwParameter=Parameter;
  com.dwReserved[0]=0;
  com.dwReserved[1]=0;
  com.dwReserved[2]=0;
  com.dwReserved[3]=0;
  err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
  if(err==PCO_NOERROR)
  {
    if(SignalNum!=resp.wNumSignal)
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
    if(Selection!=resp.wSelected)
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
    if(Parameter!=resp.dwParameter)
      err=PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
  }

  return err;
}

DWORD CPco_com::PCO_GetPowerDownMode(WORD* wPowerDownMode)
{
    SC2_Simple_Telegram com;
    SC2_Power_Down_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_POWER_DOWN_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wPowerDownMode = resp.wPdnMode;

    return err;
}

DWORD CPco_com::PCO_SetPowerDownMode(WORD wPowerDownMode)
{
    SC2_Set_Power_Down_Mode com;
    SC2_Power_Down_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_POWER_DOWN_MODE;
    com.wPdnMode=wPowerDownMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetUserPowerDownTime(DWORD* dwPdnTime)
{
    SC2_Simple_Telegram com;
    SC2_User_Power_Down_Time_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_USER_POWER_DOWN_TIME;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *dwPdnTime=resp.dwPdnTime;

    return err;
}

DWORD CPco_com::PCO_SetUserPowerDownTime(DWORD dwPdnTime)
{
    SC2_Set_User_Power_Down_Time com;
    SC2_User_Power_Down_Time_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_USER_POWER_DOWN_TIME;
    com.dwPdnTime=dwPdnTime;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetDelayExposureTimeTable(DWORD* dwDelay,DWORD* dwExposure,WORD* wTimeBaseDelay,WORD* wTimebaseExposure,WORD wCount)
{
    SC2_Simple_Telegram com;
    SC2_Delay_Exposure_Table_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_DELAY_EXPOSURE_TIME_TABLE;
    com.wSize=sizeof(com);

    err=PCO_GetTimebase(wTimeBaseDelay,wTimebaseExposure);
    if(err==PCO_NOERROR) {
        err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

        if(err==PCO_NOERROR) {
            if(wCount <= 16) {
                for(int i=0;i < wCount;i++) {
                    *dwDelay=resp.delexp[i].dwDelay;
                    dwDelay++;
                    *dwExposure=resp.delexp[i].dwExposure;
                    dwDelay++;
                }
            }
        }
    }
    return err;
}

DWORD CPco_com::PCO_SetDelayExposureTimeTable(DWORD* dwDelay,DWORD* dwExposure,WORD wTimeBaseDelay,WORD wTimeBaseExposure,WORD wCount)
{
    SC2_Set_Delay_Exposure_Table com;
    SC2_Delay_Exposure_Table_Response resp;
    DWORD err=PCO_NOERROR;

    err=PCO_SetTimebase(wTimeBaseDelay,wTimeBaseExposure);

    if(err==PCO_NOERROR) {

        com.wCode=SET_DELAY_EXPOSURE_TIME_TABLE;
        if(wCount <= 16) {
            for(int i=0;i < wCount;i++) {
                com.delexp[i].dwDelay=*dwDelay;
                dwDelay++;
                com.delexp[i].dwExposure=*dwExposure;
                dwExposure++;
            }
        }
        com.wSize=sizeof(com);

        err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
    }
    return err;
}

DWORD CPco_com::PCO_GetModulationMode(WORD *wModulationMode, DWORD* dwPeriodicalTime, WORD *wTimebasePeriodical, DWORD *dwNumberOfExposures, LONG *lMonitorOffset)
{
    SC2_Simple_Telegram com;
    SC2_Modulation_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_MODULATION_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wModulationMode     = resp.wModulationMode;
        *dwPeriodicalTime	  = resp.dwPeriodicalTime;
        *wTimebasePeriodical = resp.wTimebasePeriodical;
        *dwNumberOfExposures = resp.dwNumberOfExposures;
        *lMonitorOffset	  = resp.lMonitorOffset;
    }

    return err;
}

DWORD CPco_com::PCO_SetModulationMode(WORD wModulationMode, DWORD dwPeriodicalTime, WORD wTimebasePeriodical, DWORD dwNumberOfExposures, LONG lMonitorOffset)
{
    SC2_Set_Modulation_Mode com;
    SC2_Modulation_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_MODULATION_MODE;
    com.wModulationMode=wModulationMode;
    com.dwPeriodicalTime=dwPeriodicalTime;
    com.wTimebasePeriodical=wTimebasePeriodical;
    com.dwNumberOfExposures=dwNumberOfExposures;
    com.lMonitorOffset=lMonitorOffset;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetCameraSynchMode(WORD* wCameraSynchMode)
{
    SC2_Set_Camera_Sync_Mode com;
    SC2_Camera_Sync_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CAMERA_SYNC_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wCameraSynchMode = resp.wMode;

    return err;
}

DWORD CPco_com::PCO_SetCameraSynchMode(WORD wCameraSynchMode)
{
    SC2_Set_Camera_Sync_Mode com;
    SC2_Camera_Sync_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_CAMERA_SYNC_MODE;
    com.wMode=wCameraSynchMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;

}

DWORD CPco_com::PCO_GetCMOSLinetiming(WORD* wParameter,WORD* wTimebase,DWORD* dwLineTime)
{
    SC2_Get_CMOS_Line_Timing com;
    SC2_Get_CMOS_Line_Timing_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CMOS_LINETIMING;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wParameter=resp.wParameter;
        *wTimebase=resp.wTimebase;
        *dwLineTime=resp.dwLineTime;
    }
    return err;

}

DWORD CPco_com::PCO_SetCMOSLinetiming(WORD wParameter,WORD wTimebase,DWORD dwLineTime)
{
    SC2_Set_CMOS_Line_Timing com;
    SC2_Set_CMOS_Line_Timing_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_CMOS_LINETIMING;
    com.wParameter=wParameter;
    com.wTimebase=wTimebase;
    com.dwLineTime=dwLineTime;
    com.dwReserved[0]=0;
    com.dwReserved[1]=0;
    com.dwReserved[2]=0;
    com.dwReserved[3]=0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetCMOSLineExposureDelay(DWORD* dwExposureLines,DWORD* dwDelayLines)
{
    SC2_Get_CMOS_Line_Exposure_Delay com;
    SC2_Get_CMOS_Line_Exposure_Delay_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CMOS_LINE_EXPOSURE_DELAY;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *dwExposureLines=resp.dwExposureLines;
        *dwDelayLines=resp.dwDelayLines;
    }
    return err;
}

DWORD CPco_com::PCO_SetCMOSLineExposureDelay(DWORD dwExposureLines,DWORD dwDelayLines)
{
    SC2_Set_CMOS_Line_Exposure_Delay com;
    SC2_Set_CMOS_Line_Exposure_Delay_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_CMOS_LINE_EXPOSURE_DELAY;
    com.dwExposureLines=dwExposureLines;
    com.dwDelayLines=dwDelayLines;
    com.dwReserved[0] = 0;
    com.dwReserved[1] = 0;
    com.dwReserved[2] = 0;
    com.dwReserved[3] = 0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetFastTimingMode(WORD* wFastTimingMode)
{
    SC2_Get_Fast_Timing_Mode com;
    SC2_Get_Fast_Timing_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_FAST_TIMING_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wFastTimingMode = resp.wMode;

    return err;
}

DWORD CPco_com::PCO_SetFastTimingMode(WORD wFastTimingMode)
{
    SC2_Set_Fast_Timing_Mode com;
    SC2_Set_Fast_Timing_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_FAST_TIMING_MODE;
    com.wMode=wFastTimingMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetFirmwareVersion(char *outbuf,int *size)
{
  SC2_Get_Ext_Firmware_Versions com_ext;
  SC2_Firmware_Versions_Response resp;
  DWORD err;
  char buffer[100];
  int done,len,siz,size_in,devnum;
  DWORD old_timeout;

  old_timeout=tab_timeout.command;
  if(tab_timeout.command<400)
    tab_timeout.command=400;

  size_in=0;
  done=0;
  if(outbuf)
  {
    if(size)
      size_in=*size;
    else
      return PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
  }
  else
    siz=1;


  com_ext.wCode=GET_FIRMWARE_VERSIONS_EXT;
  com_ext.wSize=sizeof(SC2_Get_Ext_Firmware_Versions);
  com_ext.bNum=0;
  err=Control_Command(&com_ext,sizeof(com_ext),
    &resp,sizeof(SC2_Firmware_Versions_Response));

  if(err!=PCO_NOERROR)
  {
   SC2_Simple_Telegram com;
   com.wCode=GET_FIRMWARE_VERSIONS;
   com.wSize=sizeof(SC2_Simple_Telegram);
   err=Control_Command(&com,sizeof(SC2_Simple_Telegram),
    &resp,sizeof(SC2_Firmware_Versions_Response));
  }

  if(err==PCO_NOERROR)
  {
    int x,z;
    sprintf(buffer,"Firmware versions:\n");
    len=istrlen(buffer);
    if(outbuf)
    {
      if(done+len+1<=size_in)
      {
        strcat(outbuf,buffer);
        done+=len;
      }
    }
    else
      siz+=len;

    devnum=resp.DeviceNum;
    for(x=0;x<devnum;x++)
    {
//get next block
      if(x==10)
      {
       com_ext.bNum++;
       err=Control_Command(&com_ext,sizeof(com_ext),
       &resp,sizeof(SC2_Firmware_Versions_Response));

       if(err!=PCO_NOERROR)
       {
        printf("failed err 0x%x\n",err);
        break;
       }
      }

      z=x%10;
//workaround for some Dimax  
      if(resp.Device[z].szName[0]==-1)
      {
       continue;
      }
      if(resp.Device[z].szName[0]==0)
      {
       continue;
      }

//workaround pixelfly
      resp.Device[z].szName[15]=0; 


      sprintf(buffer,"%-16s: %03d %03d %04d\n",resp.Device[z].szName,resp.Device[z].bMajorRev,resp.Device[z].bMinorRev,resp.Device[z].wVariant);
      len=istrlen(buffer);
      if(outbuf)
      {
        if(done+len+1<=size_in)
        {
          strcat(outbuf,buffer);
          done+=len;
        }
      }
      else
        siz+=len;
    }
  }
  else
    writelog(ERROR_M,hdriver,"GET_FIRMWARE_VERSIONS: err 0x%x",err);

  if(size)
    *size=siz;

  tab_timeout.command=old_timeout;

  return err;
}


DWORD CPco_com::PCO_GetHardwareVersion(char *outbuf,int *size)
{
  SC2_Simple_Telegram com;
  SC2_Hardware_Versions_Response resp;
  DWORD err;
  char buffer[100];
  int done,len,siz,size_in;
  DWORD old_timeout;


  old_timeout=tab_timeout.command;
  if(tab_timeout.command<400)
    tab_timeout.command=400;


  com.wCode=GET_HARDWARE_VERSIONS;
  com.wSize=sizeof(SC2_Simple_Telegram);

  size_in=0;
  done=0;
  if(outbuf)
  {
    if(size)
      size_in=*size;
    else
      return PCO_ERROR_SDKDLL|SC2_ERROR_SDKDLL|PCO_ERROR_WRONGVALUE;
  }
  else
    siz=1;

  err=Control_Command(&com,sizeof(SC2_Simple_Telegram),
    &resp,sizeof(SC2_Hardware_Versions_Response));

  if(err==PCO_NOERROR)
  {
    int x;
    sprintf(buffer,"Hardware versions:\n");
    len=istrlen(buffer);
    if(outbuf)
    {
      if(done+len+1<=size_in)
      {
        strcat(outbuf,buffer);
        done+=len;
      }
    }
    else
      siz+=len;

    for(x=0;x<resp.BoardNum;x++)
    {
      sprintf(buffer,"%-16s: %04d %04d %04d\n",resp.Board[x].szName,resp.Board[x].wBatchNo,resp.Board[x].wRevision,resp.Board[x].wVariant);
      len=istrlen(buffer);
      if(outbuf)
      {
        if(done+len+1<=size_in)
        {
          strcat(outbuf,buffer);
          done+=len;
        }
      }
      else
        siz+=len;
    }
  }
  else
    writelog(ERROR_M,hdriver,"GET_HARDWARE_VERSIONS: err 0x%x",err);

  if(size)
    *size=siz;

  tab_timeout.command=old_timeout;

  return err;
}

DWORD CPco_com::PCO_GetFirmwareVersion(SC2_Firmware_Versions_Response* response)
{
    SC2_Get_Firmware_Versions com;
    DWORD err = PCO_NOERROR;

    com.wCode = GET_FIRMWARE_VERSIONS;
    com.wSize = sizeof(com);

    err = Control_Command(&com, sizeof(com), response, sizeof(*response));

    return err;

}

DWORD CPco_com::PCO_GetFirmwareVersionExt(BYTE bNum,SC2_Firmware_Versions_Response* response)
{
    SC2_Get_Ext_Firmware_Versions com;
    SC2_Get_Ext_Firmware_Versions resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_FIRMWARE_VERSIONS_EXT;
    com.wSize=sizeof(com);
    com.bNum=bNum;
    

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}


/////////////////////////////////////////////////////////////////////
// interface specific commands IEEE1394
/////////////////////////////////////////////////////////////////////

DWORD CPco_com::PCO_GetIEEE1394InterfaceParams(WORD* wMasterNode,WORD* wIsochChannel,WORD* wIsochPacketLen,WORD* wIsochPacketNum)
{
    SC2_Get_IEEE1394_Interface_Params com;
    SC2_IEEE1394_Interface_Params_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_IEEE1394_INTERFACE_PARAMS;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wMasterNode=resp.wMasterNode;
        *wIsochChannel=resp.wIsochChannel;
        *wIsochPacketLen=resp.wIsochPacketLen;
        *wIsochPacketNum=resp.wIsochPacketNum;
    }
    return err;
}

DWORD CPco_com::PCO_SetIEEE1394InterfaceParams(WORD wMasterNode,WORD wIsochChannel,WORD wIsochPacketLen,WORD wIsochPacketNum)
{
    SC2_Set_IEEE1394_Interface_Params com;
    SC2_IEEE1394_Interface_Params_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_IEEE1394_INTERFACE_PARAMS;
    com.wMasterNode=wMasterNode;
    com.wIsochChannel=wIsochChannel;
    com.wIsochPacketLen=wIsochPacketLen;
    com.wIsochPacketNum=wIsochPacketNum;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetIEEE1394ISOByteorder(WORD* wMode)
{
    SC2_Get_IEEE1394_Iso_Byte_Order com;
    SC2_IEEE1394_Iso_Byte_Order_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_IEEE1394_ISO_BYTEORDER;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wMode=resp.wMode;
    }
    return err;
}

DWORD CPco_com::PCO_SetIEEE1394ISOByteorder(WORD wMode)
{
    SC2_Set_IEEE1394_Iso_Byte_Order com;
    SC2_IEEE1394_Iso_Byte_Order_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_IEEE1394_ISO_BYTEORDER;
    com.wMode=wMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}


DWORD CPco_com::PCO_GetInterfaceOutputFormat(WORD wInterface,WORD* wFormat)
{
    SC2_Get_Interface_Output_Format com;
    SC2_Get_Interface_Output_Format_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_INTERFACE_OUTPUT_FORMAT;
    com.wInterface=wInterface;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR)
        *wFormat=resp.wFormat;

    return err;
}

DWORD CPco_com::PCO_SetInterfaceOutputFormat(WORD wInterface,WORD wFormat)
{
    SC2_Set_Interface_Output_Format com;
    SC2_Set_Interface_Output_Format_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_INTERFACE_OUTPUT_FORMAT;
    com.wInterface=wInterface;
    com.wFormat=wFormat;
    com.wReserved1=0;
    com.wReserved2=0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}


DWORD CPco_com::PCO_GetInterfaceStatus(WORD wInterface,DWORD* dwWarnings,DWORD* dwErrors,DWORD* dwStatus)
{
    SC2_Get_Interface_Status com;
    SC2_Get_Interface_Status_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_INTERFACE_STATUS;
    com.wInterface=wInterface;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *dwWarnings=resp.dwWarnings;
        *dwErrors=resp.dwErrors;
        *dwStatus=resp.dwStatus;
    }
    return err;
}



/////////////////////////////////////////////////////////////////////
// General Control/Status
/////////////////////////////////////////////////////////////////////


DWORD CPco_com::PCO_InitiateSelftestProcedure(DWORD* dwWarn, DWORD* dwErr)
{
    SC2_Simple_Telegram com;
    SC2_Initiate_Selftest_Procedure_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=INITIATE_SELFTEST_PROCEDURE;
    com.wSize=sizeof(com);
    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));


    if (err == PCO_NOERROR)
    {
        if (dwWarn) *dwWarn  = resp.dwWarnings;
        if (dwErr)   *dwErr  = resp.dwErrors;
    }
    else
    {
        if (dwWarn) *dwWarn = 0;
        if (dwErr)  *dwErr  = 0;
    }
    return err;
}

DWORD CPco_com::PCO_WriteHotPixelList(WORD wListNo, WORD wNumValid,WORD* wHotPixX, WORD* wHotPixY)
{
    SC2_WRITE_HOT_PIXEL_LIST com;
    SC2_WRITE_HOT_PIXEL_LIST_RESPONSE resp;
    DWORD err=PCO_NOERROR;

    com.wCode=WRITE_HOT_PIXEL_LIST;
    com.wListNo=wListNo;
    com.wIndex=0;
    com.wNumValid=wNumValid;
    for(int i = 0;i < wNumValid;i++) {
        com.strHotPix[i].wX = *wHotPixX;
        wHotPixX++;
        com.strHotPix[i].wY = *wHotPixY;
        wHotPixY++;
    }
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_ReadHotPixelList(WORD wListNo, WORD wArraySize,WORD* wNumValid, WORD* wNumMax, WORD* wHotPixX, WORD* wHotPixY)
{
    SC2_READ_HOT_PIXEL_LIST com;
    SC2_READ_HOT_PIXEL_LIST_RESPONSE resp;
    DWORD err=PCO_NOERROR;

    com.wCode=READ_HOT_PIXEL_LIST;
    com.wListNo=wListNo;
    com.wIndex=0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wNumValid = resp.wNumValid;
        *wNumMax   = resp.wNumMax;

        if(wArraySize > *wNumValid)
            wArraySize=*wNumValid;

        for(int i = 0;i < wArraySize;i++)
        {
            *wHotPixX = resp.strHotPix[i].wX;
            wHotPixX++;
            *wHotPixY = resp.strHotPix[i].wY;
            wHotPixY++;
        }
    }
    return err;
}

DWORD CPco_com::PCO_ClearHotPixelList(WORD wListNo, DWORD dwMagic1,DWORD dwMagic2)
{
    SC2_CLEAR_HOT_PIXEL_LIST com;
    SC2_CLEAR_HOT_PIXEL_LIST_RESPONSE resp;
    DWORD err=PCO_NOERROR;

    com.wCode=CLEAR_HOT_PIXEL_LIST;
    com.wListNo=wListNo;
    com.dwMagic1=dwMagic1;
    com.dwMagic2=dwMagic2;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_ClearHotPixelList(WORD wListNo)
{
    SC2_CLEAR_HOT_PIXEL_LIST com;
    SC2_CLEAR_HOT_PIXEL_LIST_RESPONSE resp;
    DWORD err=PCO_NOERROR;

    com.wCode=CLEAR_HOT_PIXEL_LIST;
    com.wListNo=wListNo;
    com.dwMagic1=0x1000AFFE;
    com.dwMagic2=0x2000ABBA;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}



DWORD CPco_com::PCO_LoadLookuptable(WORD wIdentifier,WORD wPacketNum,WORD wFormat,WORD wLength,BYTE* bData)
{
    SC2_Load_Lookuptable com;
    SC2_Load_Lookuptable_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=LOAD_LOOKUPTABLE;
    com.wIdentifier=wIdentifier;
    com.wPacketNum=wPacketNum;
    com.wFormat=wFormat;
    com.wLength=wLength;
    if(wLength <= 256)
        memcpy(&com.bData,bData,wLength);
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_ReadLookuptable(WORD wIdentifier,WORD wPacketNum,WORD* wFormat,WORD* wLength,BYTE* bData,WORD buflen)
{
    SC2_Read_Lookuptable com;
    SC2_Read_Lookuptable_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_LOOKUPTABLE_INFO;
    com.wIdentifier=wIdentifier;
    com.wPacketNum=wPacketNum;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));
    if(err==PCO_NOERROR) {
        *wLength=resp.wLength;
        *wFormat=resp.wFormat;
        if(buflen >= resp.wLength) {
            memcpy(bData,&resp.bData,resp.wLength);
        }
    }

    return err;
}


DWORD CPco_com::PCO_GetColorCorrectionMatrix(char* szCCM,WORD* len)
{
    SC2_Get_Color_Correction_Matrix com;
    SC2_Get_Color_Correction_Matrix_Response resp;
    DWORD err=PCO_NOERROR;
    WORD length;

    com.wCode=GET_COLOR_CORRECTION_MATRIX;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        if(*len >= 72)
            length = 72;
        else
            return PCO_ERROR_SDKDLL+PCO_ERROR_WRONGVALUE;

        memcpy(szCCM,&resp.szCCM,length*sizeof(char));
        *len = sizeof(szCCM)/sizeof(char);
    }
    return err;
}


DWORD CPco_com::PCO_GetBayerMultiplier(WORD* wMode,WORD* wMul)
{
    SC2_Get_Bayer_Multiplier com;
    SC2_Get_Bayer_Multiplier_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_BAYER_MULTIPLIER;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wMode = resp.wMode;
        memcpy(wMul,&resp.wMul,4*sizeof(WORD));
    }
    return err;
}

DWORD CPco_com::PCO_SetBayerMultiplier(WORD wMode,WORD* wMul)
{
    SC2_Set_Bayer_Multiplier com;
    SC2_Set_Bayer_Multiplier_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_BAYER_MULTIPLIER;
    com.wMode=wMode;
    memcpy(&com.wMul,wMul,4*sizeof(WORD));
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

/////////////////////////////////////////////////////////////////////
// special control edge
/////////////////////////////////////////////////////////////////////


DWORD CPco_com::PCO_GetHWLEDSignal(DWORD* dwParameter)
{
    SC2_Get_HW_LED_Signal com;
    SC2_Get_HW_LED_Signal_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_HW_LED_SIGNAL;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *dwParameter=resp.dwParameter;
    }
    return err;

}

DWORD CPco_com::PCO_SetHWLEDSignal(DWORD dwParameter)
{
    SC2_Set_HW_LED_Signal com;
    SC2_Set_HW_LED_Signal_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_HW_LED_SIGNAL;
    com.dwParameter=dwParameter;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetFanControlStatus(WORD* wFanMode,WORD* wFanMin,WORD* wFanMax,WORD* wStepSize,WORD* wSetValue,WORD* wActualValue)
{
    SC2_Get_Fan_Control_Status com;
    SC2_Get_Fan_Control_Status_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_FAN_CONTROL_STATUS;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wFanMode     = resp.wFanMode;
        *wFanMin      = resp.wFanMin;
        *wFanMax      = resp.wFanMax;
        *wStepSize    = resp.wStepSize;
        *wSetValue    = resp.wSetValue;
        *wActualValue = resp.wActualValue;
    }
    return err;
}


DWORD CPco_com::PCO_SetFanControlStatus(WORD wFanMode,WORD wSetValue)
{
    SC2_Set_Fan_Control_Params com;
    SC2_Set_Fan_Control_Params_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_FAN_CONTROL_PARAMS;
    com.wFanMode=wFanMode;
    com.wSetValue=wSetValue;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

/////////////////////////////////////////////////////////////////////
// special control dimax
/////////////////////////////////////////////////////////////////////

DWORD CPco_com::PCO_GetDSNUAdjustMode(WORD* wMode)
{
    SC2_Get_DSNU_Adjust_Mode com;
    SC2_Get_DSNU_Adjust_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_DSNU_ADJUST_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wMode = resp.wMode;
    }

    return err;
}


DWORD CPco_com::PCO_SetDSNUAdjustMode(WORD wMode)
{
    SC2_Set_DSNU_Adjust_Mode com;
    SC2_Get_DSNU_Adjust_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_DSNU_ADJUST_MODE;
    com.wMode=wMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}


DWORD CPco_com::PCO_InitDSNUAdjustment(WORD* wMode)
{
    SC2_Init_DSNU_Adjustment com;
    SC2_Init_DSNU_Adjustment_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=INIT_DSNU_ADJUSTMENT;
    com.wMode=*wMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wMode=resp.wMode;
    }

    return err;
}

DWORD CPco_com::PCO_GetCDIMode(WORD* wMode)
{
    SC2_Simple_Telegram com;
    SC2_Get_CDI_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_CDI_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wMode = resp.wMode;
    }

    return err;
}


DWORD CPco_com::PCO_SetCDIMode(WORD wMode)
{
    SC2_Set_CDI_Mode com;
    SC2_Get_CDI_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_DSNU_ADJUST_MODE;
    com.wMode=wMode;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetPowersaveMode(WORD* wMode,WORD* wDelayMinutes)
{
    SC2_Get_Powersave_Mode com;
    SC2_Powersave_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_POWERSAVE_MODE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if (err==PCO_NOERROR) {
        *wMode = resp.wMode;
        *wDelayMinutes = resp.wDelayMinutes;
    }
    return err;
}

DWORD CPco_com::PCO_SetPowersaveMode(WORD wMode,WORD wDelayMinutes)
{
    SC2_Set_Powersave_Mode com;
    SC2_Powersave_Mode_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_POWERSAVE_MODE;
    com.wMode=wMode;
    com.wDelayMinutes=wDelayMinutes;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));


    return err;

}

DWORD CPco_com::PCO_GetBatteryStatus(WORD* wBatteryType,WORD* wBatteryLevel,WORD* wPowerStatus)
{
    SC2_Get_Battery_Status com;
    SC2_Battery_Status_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_BATTERY_STATUS;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if (err==PCO_NOERROR) {
        *wBatteryType = resp.wBatteryType;
        *wBatteryLevel = resp.wBatteryLevel;
        *wPowerStatus = resp.wPowerStatus;
    }
    return err;

}

DWORD CPco_com::PCO_GetExternalRegister(WORD* wID,BYTE* bData,WORD *len)
{
    SC2_Get_External_Register com;
    SC2_Get_External_Register_Response resp;
    DWORD err=PCO_NOERROR;
    WORD length;

    com.wCode=GET_EXTERNAL_REGISTER;
    com.wID=*wID;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if (err==PCO_NOERROR) {
        if (*len < 256)
            length = *len;
        else
            length = 256;

        memcpy(bData,&resp.bData,length*sizeof(BYTE));
        *len = sizeof(bData)/sizeof(BYTE);
        *wID = resp.wID;

    }
    return err;

}

DWORD CPco_com::PCO_SetExternalRegister(WORD* wID,BYTE* bData,WORD* wDataSize)
{
    SC2_Set_External_Register com;
    SC2_Set_External_Register_Response resp;
    DWORD err=PCO_NOERROR;
    WORD length;

    com.wCode=SET_EXTERNAL_REGISTER;
    com.wID=*wID;
    com.wSize=sizeof(com);

    length = *wDataSize;

    memcpy(&com.bData,bData,length*sizeof(BYTE));

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wDataSize = resp.wDataSize;
        *wID = resp.wID;
    }
    return err;
}



DWORD CPco_com::PCO_WriteMailbox(WORD wMailboxNo,BYTE* bData,WORD len)
{
    SC2_Write_Mailbox com;
    SC2_Write_Mailbox_Response resp;
    DWORD err=PCO_NOERROR;
    com.wCode=WRITE_MAILBOX;
    com.wMailboxNo = wMailboxNo;
    com.wSize=sizeof(com);

    if (len > 11)
        len = 11;
    memcpy(&com.bData,bData,len*sizeof(BYTE)); // Rest nullen ?

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    // resp.wMailboxNo important ?

    return err;

}

DWORD CPco_com::PCO_ReadMailbox(WORD wMailboxNo,BYTE* bData,WORD* len)
{
    SC2_Read_Mailbox com;
    SC2_Read_Mailbox_Response resp;
    WORD length;

    DWORD err=PCO_NOERROR;
    com.wCode=READ_MAILBOX;
    com.wMailboxNo=wMailboxNo;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));


    if(err==PCO_NOERROR) {
        if (*len > 11)
            length = *len;
        else
            length = 11;
        memcpy(bData,&resp.bData,length*sizeof(BYTE));
        *len=sizeof(resp.bData)/sizeof(BYTE);
    }

    return err;
}

DWORD CPco_com::PCO_GetMailboxStatus(WORD* wNumberOfMailboxes,WORD* wMailboxStatus,WORD *len)
{
    SC2_Get_Mailbox_Status com;
    SC2_Get_Mailbox_Status_Response resp;
    DWORD err=PCO_NOERROR;
    WORD length;

    com.wCode=GET_MAILBOX_STATUS;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wNumberOfMailboxes = resp.wNumberOfMailboxes;
        if (*len > 8)
            length = *len;
        else
            length = 8;
        memcpy(wMailboxStatus,&resp.wMailboxStatus,length*sizeof(WORD));
        *len = sizeof(resp.wMailboxStatus)/sizeof(WORD);
    }

    return err;

}



/////////////////////////////////////////////////////////////////////
// pco.flim Commands, Group Code 0x001B
/////////////////////////////////////////////////////////////////////


DWORD CPco_com::PCO_GetFlimModulationParams(WORD* wSourceSelect,WORD* wOutputWaveform)
{
    SC2_Get_FLIM_Modulation_Params com;
    SC2_Get_FLIM_Modulation_Params_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_FLIM_MODULATION_PARAMS;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wSourceSelect=resp.wSourceSelect;
        *wOutputWaveform=resp.wOutputWaveform;
    }
    return err;
}

DWORD CPco_com::PCO_SetFlimModulationParams(WORD wSourceSelect ATTRIBUTE_UNUSED,WORD wOutputWaveform ATTRIBUTE_UNUSED)
{
    SC2_Set_FLIM_Modulation_Params com;
    SC2_Set_FLIM_Modulation_Params_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_FLIM_MODULATION_PARAMS;
    com.wReserved1=0;
    com.wReserved2=0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetFlimPhaseSequenceParams(WORD* wPhaseNumber,WORD* wPhaseSymmetry,WORD* wPhaseOrder,WORD* wTapSelect)
{
    SC2_Get_FLIM_Phase_Sequence_Params com;
    SC2_Get_FLIM_Phase_Sequence_Params_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_FLIM_PHASE_SEQUENCE_PARAMS;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wPhaseNumber=resp.wPhaseNumber;
        *wPhaseSymmetry=resp.wPhaseSymmetry;
        *wPhaseOrder=resp.wPhaseOrder;
        *wTapSelect=resp.wTapSelect;
    }

    return err;

}

DWORD CPco_com::PCO_SetFlimPhaseSequenceParams(WORD wPhaseNumber,WORD wPhaseSymmetry,WORD wPhaseOrder,WORD wTapSelect)
{
    SC2_Set_FLIM_Phase_Sequence_Params com;
    SC2_Set_FLIM_Phase_Sequence_Params_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_FLIM_PHASE_SEQUENCE_PARAMS;
    com.wPhaseNumber=wPhaseNumber;
    com.wPhaseSymmetry=wPhaseSymmetry;
    com.wPhaseOrder=wPhaseOrder;
    com.wTapSelect=wTapSelect;
    com.wReserved1=0;
    com.wReserved2=0;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetFlimImageProcessingFlow(WORD* wAsymmetryCorrection)
{
    SC2_Get_FLIM_Image_Processing_Flow com;
    SC2_Get_FLIM_Image_Processing_Flow_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_FLIM_IMAGE_PROCESSING_FLOW;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *wAsymmetryCorrection=resp.wAsymmetryCorrection;
        /* RESERVED FOR FUTURE USE
    *wCalculationMode=resp.wCalculationMode;
    *wReferencingMode=resp.wReferencingMode;
    *wThresholdLow=resp.wThresholdLow;
    *wThresholdHigh=resp.wThresholdHigh;
    *wOutputMode=resp.wOutputMode;
    */
    }

    return err;
}

DWORD CPco_com::PCO_SetFlimImageProcessingFlow(WORD wAsymmetryCorrection)
{
    SC2_Set_FLIM_Image_Processing_Flow com;
    SC2_Set_FLIM_Image_Processing_Flow_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_FLIM_IMAGE_PROCESSING_FLOW;
    com.wAsymmetryCorrection=wAsymmetryCorrection;
    //RESERVED FOR FUTURE USE
    com.wCalculationMode=0;
    com.wReferencingMode=0;
    com.wThresholdLow=0;
    com.wThresholdHigh=0;
    com.wOutputMode=0;
    com.wReserved1=0;
    com.wReserved2=0;
    com.wReserved3=0;
    com.wReserved4=0;
    //RESERVED END
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;

}


DWORD CPco_com::PCO_GetFlimMasterModulationFrequency(DWORD* dwFrequency)
{
    SC2_Get_FLIM_Master_Modulation_Frequency com;
    SC2_Get_FLIM_Master_Modulation_Frequency_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_FLIM_MASTER_MODULATION_FREQUENCY;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *dwFrequency=resp.dwFrequency;
    }
    return err;

}


DWORD CPco_com::PCO_SetFlimMasterModulationFrequency(DWORD dwFrequency)
{
    SC2_Set_FLIM_Master_Modulation_Frequency com;
    SC2_Set_FLIM_Master_Modulation_Frequency_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_FLIM_MASTER_MODULATION_FREQUENCY;
    com.dwFrequency=dwFrequency;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}

DWORD CPco_com::PCO_GetFlimRelativePhase(DWORD* dwPhaseMilliDeg)
{
    SC2_Get_FLIM_Relative_Phase com;
    SC2_Get_FLIM_Relative_Phase_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=GET_FLIM_RELATIVE_PHASE;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    if(err==PCO_NOERROR) {
        *dwPhaseMilliDeg=resp.dwPhaseMilliDeg;
    }
    return err;
}

DWORD CPco_com::PCO_SetFlimRelativePhase(DWORD dwPhaseMilliDeg)
{
    SC2_Set_FLIM_Relative_Phase com;
    SC2_Set_FLIM_Relative_Phase_Response resp;
    DWORD err=PCO_NOERROR;

    com.wCode=SET_FLIM_RELATIVE_PHASE;
    com.dwPhaseMilliDeg=dwPhaseMilliDeg;
    com.wSize=sizeof(com);

    err=Control_Command(&com,sizeof(com),&resp,sizeof(resp));

    return err;
}


