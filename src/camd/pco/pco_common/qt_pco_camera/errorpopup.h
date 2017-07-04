//-----------------------------------------------------------------//
// Name        | errorpopup.h                | Type: ( ) source    //
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

#ifndef ERRORPOPUP_H
#define ERRORPOPUP_H

#include <QDialog>
#include "ui_errorpopup.h"
#include "pco_includes.h"
#include "Cpco_log.h"

class ErrorPopup : public QDialog, public Ui::ErrorPopup
{
    Q_OBJECT

public:
    ErrorPopup(QWidget *parent = 0);
    ~ErrorPopup();

    void SetError();
    void SetError(QString text);
    void SetError(QString text, DWORD errornum);
    void setlog(CPco_Log* Clog);

private:
  QString errortext;
  int mainapp;

protected:
  CPco_Log* Clog;


};

#endif // ERRORPOPUP_H
