//-----------------------------------------------------------------//
// Name        | errorpopup.cpp              | Type: (*) source    //
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
// Notes       | Error popup window/error logging                  //
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

#include "errorpopup.h"

ErrorPopup::ErrorPopup(QWidget *parent)
    : QDialog(parent)
{
    mainapp = 0x1000;
    setupUi(this);
    setModal(true);
}

ErrorPopup::~ErrorPopup()
{

}

void ErrorPopup::setlog(CPco_Log* Clog)
{
    if (Clog)
        this->Clog = Clog;
}

void ErrorPopup::SetError()
{
    ErrorPopupText->setText("I am Error");
}

void ErrorPopup::SetError(QString text)
{
    ErrorPopupText->setText(text);
    if (Clog)
        Clog->writelog(ERROR_M,(PCO_HANDLE)mainapp,text.toLatin1());
}


void ErrorPopup::SetError(QString text, DWORD errornum)
{
    text.append("\n\nErrornumber: ");
    text.append(QString::asprintf("0x%x",errornum));
    ErrorPopupText->setText(text);
    //2 lines are not needed for logging
    text.replace("\n", " ");
    if (Clog)
        Clog->writelog(ERROR_M,(PCO_HANDLE)mainapp,text.toLatin1());
}
