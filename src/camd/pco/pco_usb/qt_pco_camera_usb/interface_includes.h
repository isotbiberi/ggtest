//-----------------------------------------------------------------//
// Name        | interface_includes.h        | Type: ( ) source    //
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
// Notes       | All interface specific includes and defines       //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2014 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

#ifndef INTERFACE_INCLUDES_H
#define INTERFACE_INCLUDES_H

/* USB camera specific variables */
#define CPco_camera CPco_com_usb
#define CPco_grabber CPco_grab_usb

//USB includes
#include "pco_includes.h"
#include "Cpco_com_usb.h"
#include "Cpco_grab_usb.h"
#include "usb_grabthreadworker.h"
#include "usb_livethreadworker.h"


/* USB Logfile name*/
#define LOGNAME "pco_camera_usb.log"

/* USB Main Window Title extension*/
#define MAINWINDOWTITLE "USB"

#endif
