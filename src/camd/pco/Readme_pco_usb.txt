-------------------------------------------------------------------
                 P C O  AG
     TECHNICAL  INFORMATION  DOCUMENT
-------------------------------------------------------------------

README FOR SOFTWARE:  
pco_usb

FOR PCO PRODUCT:
pco.edge, pco.pixelfly or pco.camera with usb interface

DESCRIPTION:
This packages includes a communication and a grabber class and
simple testprograms, which show the use of the class functions.
Functionality is also shown with the QT-GUI program. 
Library libusb (version 1.0.19) is used to handle data through usb-port(s).

VERSION 1
SUBVERSION 1
SUBAGE 11
LNX 64

CONTENTS:

Archive File Name: PCO_usb_x64_1_01_xx.tar.gz

Installation Instructions:

Copy the pco_usb_x64_1_01_xx.tar.gz to a distinct directory (e.g. PCO).
Use "tar -xpvzf pco_usb_x64_1_01_xx.tar.gz" to get the sources.
After this you will find the following new directories and files as noted below.
To compile the example files change to directory pco_camera/pco_usb and type "make"
To build the the QT- program open the QT-Application, load the project file qt_pco_camera_usb.pro
from pco_camera/pco_common/qt_pco_camera and use the Qt-Build command.


./pco_camera/pco_common
Files which are used also from other Interface Implementations

./pco_camera/pco_common/pco_classes
Source code and header files for Class Cpco_com
Communication to any PCO camera through PCO telegram structures

./pco_camera/pco_common/pco_include
PCO header files

./pco_camera/pco_common/pco_lib
PCO library files

./pco_camera/pco_common/qt_pco_camera
Common source files for qt based application


./pco_camera/pco_usb/Makefile
Makefile to compile pco_usb libraries and testprograms.

./pco_camera/pco_usb/pco_usb.rules 
udev rules to allow user access to all pco_usb cameras
Must be copied to etc/udev/rules.d or appropriate directory in current distribution

./pco_camera/pco_usb/pco_classes
Source code and header files for Class Cpco_com_usb
Source code and header files for Class Cpco_grab_usb
Makefile for creating libraries

./pco_camera/pco_usb/pco_ser_con
Source files and makefile for creating console testprogram
Program for testing the camera communication

./pco_camera/pco_usb/pco_camera_grab
Source files and makefile for creating console testprograms
Programs for grabbing images from a pco.camera 

./pco_camera/pco_usb/listdev
Source files and makefile for creating console testprograms
Program to list all connected pco usb devices

./pco_camera/pco_usb/qt_pco_camera_me4
Specific source files and project file for creating Qt based application


/pco_usb/bin
executable binaries


SYSTEM_VERSION:
This package was sucessfully tested on
 Debian kernel-release 4.0.0
 Debian kernel-release 3.19.5
 Debian kernel-release 3.2.0-4-amd64 kernel-version SMP Debian 3.2.65


VERSION HISTORY:
ver01_01_11
PostArm() function added to grabber class

ver01_01_10
new directory layout
minor bug fixes

ver01_01_08
no more images lost on USB 3.0 PCI card (kernel >3.4 strongly recommended)
added Qt GUI project (requires Qt 5 (www.qt.io) )
libusb updated to 1.0.19 (not included in this package (www.libusb.info) )
cleaned up code
added all telegram functions

ver01_01_07
first version for PCO usb cameras
communication class splitted in separate source files
known issues:
when camera is running with maximum possible fps images are lost


ver01_01_05
added some camera commands
new headerfiles in PCO_Include
new pco libraries

ver01_01_04
added some camera commands
new headerfiles in PCO_Include
new pco libraries


ver01_01_03
with makefiles
added support for pco.edge GlobalShutter
new headerfiles in PCO_Include
new pco libraries


ver01_01_02
new example and new functions in class CPco_cl_com

ver01_01_01
initial project


KNOWN BUGS:

-------------------------------------------------------------------
 PCO AG
 DONAUPARK 11
 93309 KELHEIM / GERMANY
 PHONE +49 (9441) 20050
 FAX   +49 (9441) 200520
 info@pco.de, support@pco.de
 http://www.pco.de
-------------------------------------------------------------------
 DISCLAIMER
 THE ORIGIN OF THIS INFORMATION MAY BE INTERNAL OR EXTERNAL TO PCO.
 PCO MAKES EVERY EFFORT WITHIN ITS MEANS TO VERIFY THIS INFORMATION.
 HOWEVER, THE INFORMATION PROVIDED IN THIS DOCUMENT IS FOR YOUR
 INFORMATION ONLY. PCO MAKES NO EXPLICIT OR IMPLIED CLAIMS TO THE
 VALIDITY OF THIS INFORMATION.
-------------------------------------------------------------------
 Any trademarks referenced in this document are the property of
 their respective owners.
-------------------------------------------------------------------
