/*
 * Dome driver for 7 Foot Astrohaven Enterprise Domes w/ Vision 130 controller
 * Should work for 12', 16', and 20' domes (untested)
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

#include "dome.h"
#include "kafka.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
using namespace rts2dome;
//static const char *ip= "http://192.168.0.11/index.htm";


size_t writefunc(void *contents, size_t size, size_t nmemb, std::string *s)
{
    size_t newLength = size*nmemb;
    size_t oldLength = s->size();
    try
    {
        s->resize(oldLength + newLength);
    }
    catch(std::bad_alloc &e)
    {
        //handle memory problem
        return 0;
    }

    std::copy((char*)contents,(char*)contents+newLength,s->begin()+oldLength);
    return size*nmemb;
}




int kafka::openDomeAll(CURL *curl,CURLcode res)
{
    /* First set the URL that is about to receive our POST. This URL can
     *      *      *              *        just as well be a https:// URL if that is what should receive the
     *           *           *                           *               data. */

    curl_easy_setopt(curl, CURLOPT_URL,ip);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, DOMEOPEN);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return -1;
    }
    /* always cleanup */
    curl_easy_cleanup(curl);

    return 0;
    /*
        if(readSerial(CMD_A_OPEN, response, POLL_A_OPENED) > MAX_COMMANDS)
        {
            return -1;
        }

        return 1;
    */
}

int kafka::closeDomeAll(CURL *curl,CURLcode res)
{

    /* First set the URL that is about to receive our POST. This URL can
     *      *      *      *              *        just as well be a https:// URL if that is what should receive the
     *           *           *           *                           *               data. */

    curl_easy_setopt(curl, CURLOPT_URL,ip);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, DOMECLOSE);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return -1;
    }
    /* always cleanup */
    curl_easy_cleanup(curl);

    return 0;

    /*
        if(readSerial(CMD_B_OPEN, response, POLL_B_OPENED) > MAX_COMMANDS)
        {
            return -1;
        }

        return 1;
    */
}


int kafka::clearOpenDomeBit(CURL *curl,CURLcode res)
{
    /* First set the URL that is about to receive our POST. This URL can
     *      *      *      *      *              *        just as well be a https:// URL if that is what should receive the
     *           *           *           *           *                           *               data. */

    curl_easy_setopt(curl, CURLOPT_URL, ip);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, CLEAROPENDOMEBIT);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return -1;
    }
    /* always cleanup */
    curl_easy_cleanup(curl);

    return 0;

}

int kafka::clearCloseDomeBit(CURL *curl,CURLcode res)
{
    /* First set the URL that is about to receive our POST. This URL can
    *      *      *      *      *      *              *        just as well be a https:// URL if that is what should receive the
    *           *           *           *           *           *                           *               data. */

    curl_easy_setopt(curl, CURLOPT_URL,ip);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, CLEARCLOSEDOMEBIT);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return -1;
    }
    /* always cleanup */
    curl_easy_cleanup(curl);

    return 0;

}

int kafka::isLid1Closed(CURL *curl,CURLcode res,std::string responseString,char *copyString)
{
    /* First set the URL that is about to receive our POST. This URL can
     *      *              *        just as well be a https:// URL if that is what should receive the
     *           *                           *               data. */

    curl_easy_setopt(curl, CURLOPT_URL,ip);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, LID1CLOSED);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */

    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return -1;
    }
    strcpy(copyString,responseString.c_str());
    /* always cleanup */
    curl_easy_cleanup(curl);
    return 1;
}



int kafka::isLid1Opened(CURL *curl,CURLcode res,std::string responseString,char *copyString)
{
    /* First set the URL that is about to receive our POST. This URL can
     *      *      *              *        just as well be a https:// URL if that is what should receive the
     *           *           *                           *               data. */

    curl_easy_setopt(curl, CURLOPT_URL,ip);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, LID1OPENED);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */

    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return -1;
    }
    strcpy(copyString,responseString.c_str());
    /* always cleanup */
    curl_easy_cleanup(curl);
    return 1;
}

int kafka::isLid2Closed(CURL *curl,CURLcode res,std::string responseString,char *copyString)

{
    /* First set the URL that is about to receive our POST. This URL can
     *      *      *              *        just as well be a https:// URL if that is what should receive the
     *           *           *                           *               data. */

    curl_easy_setopt(curl, CURLOPT_URL, ip);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, LID2CLOSED);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);


    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
    {   fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return -1;
    }
    strcpy(copyString,responseString.c_str());

    /* always cleanup */
    curl_easy_cleanup(curl);
    return 1;
}


int kafka::isLid2Opened(CURL *curl,CURLcode res,std::string responseString,char *copyString)
{
    /* First set the URL that is about to receive our POST. This URL can
     *      *      *              *        just as well be a https:// URL if that is what should receive the
     *           *           *                           *               data. */

    curl_easy_setopt(curl, CURLOPT_URL, ip);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, LID2OPENED);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return -1;
    }
    strcpy(copyString,responseString.c_str());
    /* always cleanup */
    curl_easy_cleanup(curl);
    return 1;
}


int kafka::isDomeRemote(CURL *curl,CURLcode res,std::string responseString,char *copyString)
{
    /* First set the URL that is about to receive our POST. This URL can
     *      *      *              *        just as well be a https:// URL if that is what should receive the
     *           *           *                           *               data. */

    curl_easy_setopt(curl, CURLOPT_URL,ip);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, DOMEREMOTE);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return -1;
    }
    strcpy(copyString,responseString.c_str());
    /* always cleanup */
    curl_easy_cleanup(curl);
    return 1;
}

int kafka::isDomeReady(CURL *curl,CURLcode res,std::string responseString,char *copyString)
{
    /* First set the URL that is about to receive our POST. This URL can
     *      *      *              *        just as well be a https:// URL if that is what should receive the
     *           *           *                           *               data. */
    curl_easy_setopt(curl, CURLOPT_URL, ip);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, DOMESTATUS);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);
    //logStream(MESSAGE_INFO)<<"domeready is "<<responseString.c_str()<<sendLog;

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return -1;
    }
    //logStream(MESSAGE_INFO)<<"domeready is "<<responseString.c_str()<<sendLog;
    strcpy(copyString,responseString.c_str());
    /* always cleanup */
    curl_easy_cleanup(curl);
    return 1;
}


int kafka::isBackupRelay(CURL *curl,CURLcode res,std::string responseString,char *copyString)
{
    /* First set the URL that is about to receive our POST. This URL can
     *      *      *              *        just as well be a https:// URL if that is what should receive the
     *           *           *                           *               data. */

    curl_easy_setopt(curl, CURLOPT_URL, ip);
    /* Now specify the POST data */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, BACKUPRELAY);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,  writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseString);

    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    /* Check for errors */
    if(res != CURLE_OK)
    {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
        return -1;
    }
    strcpy(copyString,responseString.c_str());

    /* always cleanup */
    curl_easy_cleanup(curl);
    return 1;
}


/*
int AHE::readSerial(const char send, char reply, const char exitState)
{//since the controller write a heart beat have to iterate through it
 //all to find what we are looking for, reason why whe couldn't cleanly
 //use readPort (char *rbuf, int b_len, char endChar)

    cmdSent = 0;
    reply = 'L';

    //read all the extra hearbeat junk
    while(sconn->readPortNoBlock(&reply, 1) != 0)
    {
        //tiny bust loop :(
    }

    while(reply != exitState && cmdSent < MAX_COMMANDS)
    {
        sconn->writePort(send);
        usleep(SERIAL_SLEEP);
        sconn->readPort(reply);
        cmdSent ++;
    }

    return cmdSent;
}

*/
int kafka::processOption(int opt)
{
    switch (opt)
    {
    case 'k':
        ip = optarg;
        break;
    default:
        return Dome::processOption (opt);
    }
    return 0;
}

int kafka::setValue(rts2core::Value *old_value, rts2core::Value *new_value)
{
    if(old_value == closeDome)
    {
        if(new_value->getValueInteger() == 1)
        {
            startOpen();
        }
        else if(new_value->getValueInteger() == 0)
        {
            startClose();
        }
    }
    /*
    else if(old_value == ignoreComm)
    {
        if(new_value->getValueInteger() == 0)
        {//false
            ignoreCommands = false;
        }
        else if(new_value->getValueInteger() == 1)
        {//true
            ignoreCommands = true;
        }

    }
    */
    return Dome::setValue(old_value, new_value);
}

int kafka::init()
{



    int ret = Dome::init();
    if(ret)
    {
        return ret;
    }
    status = CLOSED;

    return 0;


    /*
    sconn = new rts2core::ConnSerial(devFile, this, rts2core::BS9600, rts2core::C8, rts2core::NONE, 50);
    sconn->setDebug (getDebug ());
    sconn->init();

    int ret = Dome::init();
    if(ret)
    {
        return ret;
    }

    return 0;
    */
}

/*
char AHE::getHeartBeat()
{
    response = '\x00';
    while(sconn->readPort(response) == -1)
    {
    //    usleep(SERIAL_SLEEP);
    }

    return response;

}
*/

int kafka::info()
{
    /*
        	response = getHeartBeat();

       switch(response){
           case STATUS_AB_CLOSED:
    domeStatus->setValueString("Closed");
               status = CLOSED;
               break;
           case STATUS_A_OPEN:
               domeStatus->setValueString("A Open");
               status = OPENED;
               break;
           case STATUS_B_OPEN:
               domeStatus->setValueString("B Open");
               status = OPENED;
               break;
           case STATUS_AB_OPEN:
               domeStatus->setValueString("Open");
               status = OPENED;
       }

     */
    //logStream(MESSAGE_INFO)<<"info called"<<sendLog;

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl)
    {
        isDomeReady(curl,res,responseString,copyString);
        //logStream(MESSAGE_INFO)<<"info responseString is "<<responseString<<"copystring "<<copyString<<sendLog;

        domeReady->setValueString(copyString);
        sendValueAll(domeReady);
    }
    else logStream(MESSAGE_INFO)<<"info curl problem"<<sendLog;
    curl = curl_easy_init();
    if(curl)
    {


        isDomeRemote(curl,res,responseString,copyString);
       // logStream(MESSAGE_INFO)<<"info responseString is "<<responseString<<"copystring "<<copyString<<sendLog;

        domeRemote->setValueString(copyString);
        sendValueAll(domeRemote);
    }
    else logStream(MESSAGE_INFO)<<"info curl problem"<<sendLog;



    curl = curl_easy_init();


    if(curl)
    {
        isLid1Closed(curl,res,responseString,copyString);
        //logStream(MESSAGE_INFO)<<"info responseString of is lid1closed  is "<<responseString<<"copystring "<<copyString<<sendLog;

        //lid1Closed->setValueString("deneme");
        lid1ClosedTest->setValueString(copyString);
        sendValueAll(lid1ClosedTest);

    }
    else logStream(MESSAGE_INFO)<<"info curl problem"<<sendLog;






    curl=curl_easy_init();
    if(curl)
    {


        isLid2Closed(curl,res,responseString,copyString);
        //logStream(MESSAGE_INFO)<<"info responseString is "<<responseString<<"copystring "<<copyString<<sendLog;

        lid2Closed->setValueString(copyString);
        sendValueAll(lid2Closed);
    }
    else logStream(MESSAGE_INFO)<<"info curl problem"<<sendLog;


    curl=curl_easy_init();
    if(curl)
    {


        isLid1Opened(curl,res,responseString,copyString);
        //logStream(MESSAGE_INFO)<<"info responseString is "<<responseString<<"copystring "<<copyString<<sendLog;

        lid1Opened->setValueString(copyString);
        sendValueAll(lid1Opened);
    }
    else logStream(MESSAGE_INFO)<<"info curl problem"<<sendLog;


    curl=curl_easy_init();
    if(curl)
    {


        isLid2Opened(curl,res,responseString,copyString);
        //logStream(MESSAGE_INFO)<<"info responseString is "<<responseString<<"copystring "<<copyString<<sendLog;

        lid2Opened->setValueString(copyString);
        sendValueAll(lid2Opened);
    }
    else logStream(MESSAGE_INFO)<<"info curl problem"<<sendLog;

    curl=curl_easy_init();
    if(curl)
    {


        isBackupRelay(curl,res,responseString,copyString);
        //logStream(MESSAGE_INFO)<<"info responseString is "<<responseString<<"copystring "<<copyString<<sendLog;

        backupRelay->setValueString(copyString);
        sendValueAll(backupRelay);
    }
    else logStream(MESSAGE_INFO)<<"info curl problem"<<sendLog;
    
    if((lid1Opened->getValueString().compare("|1|")==0)&&(lid2Opened->getValueString().compare("|1|")==0))
    {
	    status = OPENED;
	    if((domeStatus->getValueString().compare("Opening")==0))
	    {
	    domeStatus->setValueString("Opened");
	    sendValueAll(domeStatus);
	    //logStream(MESSAGE_INFO)<<"lid1opened and lid2opened 1"<<sendLog;
	    }
    }  
    
   if( (lid1ClosedTest->getValueString().compare("|1|")==0)&&(lid2Closed->getValueString().compare("|1|")==0))
   {
	   status = CLOSED;
	   if((domeStatus->getValueString().compare("Closing")==0))
	   {
           domeStatus->setValueString("Closed"); 
	   sendValueAll(domeStatus);
	   }
   }


    
    Dome::info();
}


int kafka::startOpen()
{

    status = OPENING;
    domeStatus->setValueString("Opening");
    sendValueAll(domeStatus);
    int ret;
    curl = curl_easy_init();
    if(curl)
    {
        ret=openDomeAll(curl,res);

        if(ret)
        {
            status = ERROR;
            domeStatus->setValueString("Error opening");
        }

    }
    usleep(200000);
    curl = curl_easy_init();

    if(curl)
    {
        ret=clearOpenDomeBit(curl,res);
        if(ret)
        {
            status = ERROR;
            domeStatus->setValueString("Error opening");
        }

    }
    status=OPENED;
    return 1;


}

int kafka::startClose()
{
    logStream(MESSAGE_INFO)<<"started to close" <<sendLog;
    status = CLOSING;
    domeStatus->setValueString("Closing");
    sendValueAll(domeStatus);

    int ret;
    curl = curl_easy_init();
    if(curl)
    {
        ret=closeDomeAll(curl,res);

        if(ret)
        {
            status = ERROR;
            domeStatus->setValueString("Error closing");
        }

    }
    usleep(200000);
    curl = curl_easy_init();

    if(curl)
    {
        ret=clearCloseDomeBit(curl,res);
        if(ret)
        {
            status = ERROR;
            domeStatus->setValueString("Error closing");
        }

    }
    logStream(MESSAGE_INFO)<<"closing ended " <<sendLog;

    status = CLOSED;
    return 1;


}

long kafka::isOpened()
{
    
        if (status == OPENED)
        {
            return -2;
        }

        return 2000;
    
}

long kafka::isClosed()
{
    
    if (status == CLOSED)
    {
        return -2;
    }

    return 2000;
    
}

int kafka::endOpen()
{
    //do nothing
    return 0;
}



int kafka::endClose()
{
    //do nothing
    domeStatus->setValueString("Closed");
    return 0;
}


kafka::kafka(int argc, char **argv):Dome(argc, argv)
{
    createValue(domeStatus, "dome_status", "current dome status", true);
    createValue(closeDome, "state", "switch state of dome", false, RTS2_VALUE_WRITABLE);
    createValue(lid1Opened, "lid1opened", "current lid1 status", true);
    createValue(lid2Opened, "lid2opened", "current lid2 status", true);
    createValue(lid1ClosedTest, "lid1closed", "current llid1 status", true);

    createValue(lid2Closed, "lid2closed", "current lid2 status", true);
    createValue(domeRemote, "domeremote", "is dome remote", true);
    createValue(domeReady, "domeready", "is dome ready", true);
    createValue(backupRelay, "backuprelay", "is backup relay", true);

    addOption('k',NULL,1, "ip(or web) address of plc");





    closeDome->addSelVal("Close");
    closeDome->addSelVal("Open");




    setIdleInfoInterval(5);
}

kafka::~kafka()
{
}

int main(int argc, char **argv)
{
    kafka device(argc, argv);
    return device.run();
}
