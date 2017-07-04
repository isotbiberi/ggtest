//-----------------------------------------------------------------//
// Name        | memorymanager.h             | Type: ( ) source    //
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
// Notes       |                                                   //
//             |                                                   //
//             |                                                   //
//-----------------------------------------------------------------//
// (c) 2010 - 2014 PCO AG                                          //
// Donaupark 11 D-93309  Kelheim / Germany                         //
// Phone: +49 (0)9441 / 2005-0   Fax: +49 (0)9441 / 2005-20        //
// Email: info@pco.de                                              //
//-----------------------------------------------------------------//

#ifndef GRABTHREAD_H
#define GRABTHREAD_H

#include <QDialog>
#include "ui_memorymanager.h"

class MemoryManager : public QDialog, public Ui::MemoryManager
{
    Q_OBJECT

public:
    MemoryManager(int currentsize, long picmax, QWidget *parent = 0);
    ~MemoryManager();
    unsigned int getValue();

private:
    int response;
    unsigned long picmax;



    private slots:
    void on_Input_valueChanged(int value);
};

#endif // GRABTHREAD_H
