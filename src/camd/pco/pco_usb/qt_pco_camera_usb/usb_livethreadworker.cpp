//-----------------------------------------------------------------//
// Name        | usb_livethreadworker.cpp    | Type: (*) source    //
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

#include "usb_livethreadworker.h"

LiveThreadWorker::LiveThreadWorker(CPco_grabber* grabber, QVector<unsigned short*>& buffer)
{
  this->grabber = grabber;
  this->buffer = buffer;
  liveviewstop = 0;
  sendpicture = 1;
  err = PCO_NOERROR;
}

LiveThreadWorker::~LiveThreadWorker()
{

}

void LiveThreadWorker::run() {
  int i = 0;
  int count = buffer.size();
  while (!liveviewstop) {
    // Single Grab is enough, no need for async
    err = grabber->Acquire_Image(buffer[i]);
    if (sendpicture) {
      sendpicture = 0;
      emit finishedPictureSignal(i);
    }
    i++;
    if (i == count) //reached max number of buffers, reset
      i = 0;
  }
}

void LiveThreadWorker::stopLiveView() {
  liveviewstop = 1;
}

void LiveThreadWorker::sendPicture() {
  sendpicture = 1;
}
