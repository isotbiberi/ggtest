//-----------------------------------------------------------------//
// Name        | memorymanager.cpp           | Type: (*) source    //
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
// Author      | EIJ, PCO AG                                       //
//-----------------------------------------------------------------//
// Revision    | rev. 1.00                                         //
//-----------------------------------------------------------------//
// Notes       | memory manager dialog                             //
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

#include "memorymanager.h"
#include "errorpopup.h"



MemoryManager::MemoryManager(int currentsize, long picmax, QWidget *parent) : QDialog(parent)
{
    setupUi(this);
    this->picmax = picmax;
    response = 0;
    if(picmax < 0) {
        AvailableDisplay->setDisabled(TRUE);
    }
    else {
        AvailableDisplay->setValue(picmax+currentsize);
        Input->setMaximum(picmax);
        Input->setValue(currentsize);
    }
}

MemoryManager::~MemoryManager()
{

}

void MemoryManager::on_Input_valueChanged(int value) {
    value = abs(value);

    if((unsigned int) value > picmax) {
        response = picmax;
    }
    else {
        response = value;
    }
    Input->setValue(value);
}

unsigned int MemoryManager::getValue() {
    return response;
}
