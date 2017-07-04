//-----------------------------------------------------------------//
// Name        | getversions.cpp             | Type: (*) source    //
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
// Revision    | rev. 1.03                                         //
//-----------------------------------------------------------------//
// Notes       | console program to read version information       //
//             | from all connected or selected cameras            //
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
//  1.05     | 16.10.2012 | new                                    //
//           |            |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//
//  0.0x     | xx.xx.2012 |                                        //
//           |            |                                        //
//-----------------------------------------------------------------//

#include "Cpco_com.h"
#include "Cpco_com_usb.h"
#include "VersionNo.h"

#define MAX_CAMNUM 8

using namespace std;

int check_camera(int board)
{
  DWORD err=PCO_NOERROR;
  char infostr[100];
  char *Firmwaretext=NULL;
  char *Hardwaretext=NULL;
  int size;
  WORD camtype;
  DWORD serialnumber;

  CPco_com_usb *cam;

  cam = new CPco_com_usb();
  
  printf("Try to open Camera %d\n",board);
  err=cam->Open_Cam(board);
  if(err!=PCO_NOERROR)
  {
   printf("error 0x%x in Open_Cam, close application\n",err);
   goto err_out_new;
  }
  err=cam->PCO_GetCameraType(&camtype,&serialnumber);
  if(err!=PCO_NOERROR)
   printf("PCO_GetCameraType() Error 0x%x\n",err);
  else
  {
   printf("CameraTyp  : 0x%x\n",camtype);
   printf("SerialNr.  : %d\n",serialnumber);
  }
  
  err=cam->PCO_GetInfo(1,infostr,sizeof(infostr));
  if(err!=PCO_NOERROR)
   printf("PCO_GetInfo(1,) Error 0x%x\n",err);
  else
   printf("Camera     : %s\n",infostr);

  err=cam->PCO_GetInfo(2,infostr,sizeof(infostr));
  if(err!=PCO_NOERROR)
   printf("PCO_GetInfo(2,) Error 0x%x\n",err);
  else
   printf("Sensor     : %s\n",infostr);

 
  err=cam->PCO_GetHardwareVersion(NULL,&size);
  if(err==PCO_NOERROR)
  {
   Hardwaretext=(char*)malloc(size);
   if(Hardwaretext)
   {
    memset(Hardwaretext,0,size);
    err=cam->PCO_GetHardwareVersion(Hardwaretext,&size);
    printf("%s",Hardwaretext);
    free(Hardwaretext);
   }
  }
  
  err=cam->PCO_GetFirmwareVersion(NULL,&size);
  if(err==PCO_NOERROR)
  {
//   printf("size of Firmware text is %d\n",size);
   Firmwaretext=(char*)malloc(size);
   if(Firmwaretext)
   {
    memset(Firmwaretext,0,size);
    err=cam->PCO_GetFirmwareVersion(Firmwaretext,&size);
    printf("%s",Firmwaretext);
    free(Firmwaretext);
   }
  }

  err=cam->Close_Cam();
  if(err!=PCO_NOERROR)
   printf("CloseCam() Error 0x%x\n",err);

err_out_new:
  delete cam;
 
  return err;
}  


int main(int argc, char *argv[])
{
  int help=0;
  int select=0;
  int num[MAX_CAMNUM];
  int x;

  for(x=0;x<MAX_CAMNUM;x++)
   num[x]=0;

  for(int i=argc;i>1;)
  {
   char *a;

   i--;

   if(strstr(argv[i],"-h"))
    help=1;
   if(strstr(argv[i],"-?"))
    help=1;
   if(strstr(argv[i],"?"))
    help=1;

   if((a=strstr(argv[i],"-b")))
   {
    x=atoi(a+2);
    if((x<MAX_CAMNUM))
    {
     num[x]=1;
     select=1;
    }
    else
    {
     printf("wrong Parameter -b%d\n",atoi(a+2));
     help=1;
    }
   }
  }

  if(help)
  {
   printf("usage: %s options\n"
          "options: \n"
          "-b[0...7] camera number\n"
          "-h,-?,? this message\n",argv[0]);
   exit(0);
  }


  if(select==0)
  {
   for(x=0;x<MAX_CAMNUM;x++)
    num[x]=1;
  }
    
  for(x=0;x<MAX_CAMNUM;x++)
  {
   if(num[x]==0)
    continue; 
   check_camera(x);
  }
  
  return 0;
}


