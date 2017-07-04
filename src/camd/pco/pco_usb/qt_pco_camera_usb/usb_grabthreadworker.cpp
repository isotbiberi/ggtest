//-----------------------------------------------------------------//
// Name        |usb_grabthreadworker.cpp     | Type: (*) source    //
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
// Notes       |                                                   //
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

#include "usb_grabthreadworker.h"
#include "qt_pco_camera.h"

GrabThreadWorker::GrabThreadWorker(CPco_grabber* grabber, QVector<unsigned short*> &buffer) {
  err = PCO_NOERROR;
  this->buffer = buffer;
  this->grabber = grabber;
  stop = 0;
}

GrabThreadWorker::~GrabThreadWorker() {

}

void GrabThreadWorker::run() {
  err = PCO_NOERROR;
  int count = buffer.size();
  for (int i = 0; i < count; i++) {
      //if stop signal was given wait for the last image to be valid (pco.pixelfly) and exit
      if (stop) {
          err = grabber->Acquire_Image_Async_wait(buffer[i]); //this waits until the last image is valid
          emit finishedAllSignal(i);
          return;
      }
      //last picture, wait for them to be valid (pco.pixelfly)
      else if (i == (count-1)) {
          err = grabber->Acquire_Image_Async_wait(buffer[i]); //this waits until the last image is valid
      }
      //no stop signal, no error, get regular image
      else {
          err = grabber->Acquire_Image_Async(buffer[i]);
          emit finishedPicture(i);
      }
      //if we run into an error, abort
      if (err != PCO_NOERROR) {
          emit recorderError(i-1);
          return;
      }
  }
  emit finishedAllSignal(count);
}

void GrabThreadWorker::stopGrabbing() {
    stop = 1;
}
