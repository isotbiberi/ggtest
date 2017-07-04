//-----------------------------------------------------------------//
// Name        | usb_livethreadworker.h      | Type: ( ) source    //
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

#ifndef LIVETHREADWORKER_H
#define LIVETHREADWORKER_H

#include <QThread>
#include "interface_includes.h"
#include "qt_pco_camera.h"
#include <qvector.h>


class LiveThreadWorker : public QThread
{
  Q_OBJECT

public:
    LiveThreadWorker(CPco_grabber* grabber, QVector<unsigned short*>& buffer);
    ~LiveThreadWorker();
    void stopLiveView();
    void sendPicture();

private:
  DWORD err;
  QVector<unsigned short*> buffer;
  volatile int liveviewstop;
  int sendpicture;
  CPco_grabber* grabber;

signals :
  void finishedPictureSignal(int);

protected:
  void run();

};

#endif //LIVETHREADWORKER_H
