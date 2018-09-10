/*
 * Copyright (C) 2011 Lee Hicks
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#ifndef __kafka_h__
#define __kafka_h__


#include <stdio.h>
#include <iostream>
#include <curl/curl.h>
#include <unistd.h>
#define LID1OPENED "|0|1|0|1|32014|0|0|"
#define LID1CLOSED "|0|1|0|1|32015|0|0|"
#define LID2OPENED "|0|1|0|1|12|0|0|"
#define LID2CLOSED "|0|1|0|1|32015|0|0|"
#define DOMEREMOTE "|0|1|0|1|32009|0|0|"
#define DOMESTATUS "|0|1|0|2|32011|0|0|"
#define BACKUPRELAY "|0|1|0|2|13|0|0|"
#define DOMEOPEN "|c|-1||1|0|5|2826|1|0|1|"
#define CLEAROPENDOMEBIT "|c|-1||1|0|5|2826|1|0|0|"
#define DOMECLOSE "|c|-1||1|0|5|2827|1|0|1|"
#define CLEARCLOSEDOMEBIT "|c|-1||1|0|5|2827|1|0|0|"




enum DomeStatus
{
    ERROR=-1, OPENED, CLOSED, OPENING, CLOSING

};

namespace rts2dome
{

class kafka:public Dome
{
private:
    const char * ip;
    DomeStatus status;

    int openDomeAll(CURL *curl,CURLcode res);
    int closeDomeAll(CURL *curl,CURLcode res);
    int clearOpenDomeBit(CURL *curl,CURLcode res);
    int clearCloseDomeBit(CURL *curl,CURLcode res);
    int isLid1Closed(CURL *curl,CURLcode res,std::string responseString,char *copyString);
    int isLid1Opened(CURL *curl,CURLcode res,std::string responseString,char *copyString);
    int isLid2Closed(CURL *curl,CURLcode res,std::string responseString,char *copyString);
    int isLid2Opened(CURL *curl,CURLcode res,std::string responseString,char *copyString);
    int isDomeRemote(CURL *curl,CURLcode res,std::string responseString,char *copyString);
    int isDomeReady(CURL *curl,CURLcode res,std::string responseString,char *copyString);
    int isBackupRelay(CURL *curl,CURLcode res,std::string responseString,char *copyString);

    CURL *curl;
    CURLcode res;
    std::string responseString;
    char * copyString;


    rts2core::ValueString * domeStatus;
    rts2core::ValueSelection* closeDome;
    rts2core::ValueString * lid1Opened;
    rts2core::ValueString * lid2Opened;
    rts2core::ValueString * lid1Closed;
    rts2core::ValueString * lid1ClosedTest;
    rts2core::ValueString * lid2Closed;
    rts2core::ValueString * domeRemote;
    rts2core::ValueString * domeReady;
    rts2core::ValueString * backupRelay;




protected:
    virtual int processOption(int opt);
    virtual int setValue(rts2core::Value *old_value, rts2core::Value *new_value);

    virtual int startOpen();
    virtual long isOpened();
    virtual int endOpen();

    virtual int startClose();
    virtual long isClosed();
    virtual int endClose();


public:
    kafka(int argc, char ** argv);
    virtual ~kafka();
    virtual int init();
    virtual int info();
};
}


#endif
