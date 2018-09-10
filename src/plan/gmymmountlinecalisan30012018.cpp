/*
 * Executor body.
 * Copyright (C) 2003-2010 Petr Kubanek <petr@kubanek.net>
 * Copyright (C) 2010-2011 Petr Kubanek, Institute of Physics <kubanek@fzu.cz>
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

//#define EVENT_MOVE_OK              RTS2_LOCAL_EVENT+53
//#define EVENT_EXPOSURE_OK          RTS2_LOCAL_EVENT+54
#define EVENT_CCD_READY         RTS2_LOCAL_EVENT+55
#define EVENT_MOUNT_READY         RTS2_LOCAL_EVENT+56
#define EVENT_EXPOSE_CHECK      RTS2_LOCAL_EVENT+57
#define EVENT_MOUNT_CHECK RTS2_LOCAL_EVENT+58
#define EVENT_START_SLOT RTS2_LOCAL_EVENT+59
#define EVENT_SLOT_CHECK RTS2_LOCAL_EVENT+60
#define EVENT_FIRE_MOUNT RTS2_LOCAL_EVENT+61
#define EVENT_FIRE_CAMERA RTS2_LOCAL_EVENT+62
#include "ObservationPlan.h"
#include "valuearray.h"
#include "device.h"
#include "rts2script/execcli.h"
#include "rts2db/devicedb.h"
#include <iostream>
#include <sstream>

#define OPT_IGNORE_DAY    OPT_LOCAL + 100
#define OPT_DONT_DARK     OPT_LOCAL + 101
#define OPT_DISABLE_AUTO  OPT_LOCAL + 102

namespace rts2plan
{

/**
 * Executor class.
 *
 * @author Petr Kubanek <petr@kubanek.net>
 */
class Gmym:public rts2db::DeviceDb
{
public:
    Gmym (int argc, char **argv);
    virtual ~ Gmym (void);
    virtual rts2core::DevClient *createOtherType (rts2core::Connection * conn, int other_device_type);

    virtual void postEvent (rts2core::Event * event);
    virtual int commandAuthorized(rts2core::Connection * conn);

    virtual void deviceReady (rts2core::Connection * conn);
    virtual int idle ();

    virtual int info ();

    virtual void changeMasterState (rts2_status_t old_state, rts2_status_t new_state);



protected:
    virtual int processOption (int in_opt);
    virtual int init ();

    virtual int setValue (rts2core::Value *oldValue, rts2core::Value *newValue);

private:

    int mntLine=0;
    int expLine=0;

    rts2core::ValueInteger *mountLine;
    rts2core::ValueInteger *exposureLine;
    rts2core::ValueBool  *enableSlot;
    //rts2core::ValueString *slot;
    // rts2core::ValueInteger *slot;
    bool mountConnected = false;
    bool cameraConnected = false;
    rts2core::Connection *ccdConnection;
    rts2core::Connection *mountConnection;

    void startSlot();
    void stopSlot();
    void runSlot(int lineNumber);
    rts2core::Command *oldCommand = NULL;
    ObservationPlan plan;
    slot selectedSlot ;
    long long getEpochNow();
    bool waitingforMountMove=false;
    std::stringstream mountss;
    std::stringstream camerass;
};

}

using namespace rts2plan;

Gmym::Gmym (int in_argc, char **in_argv):rts2db::DeviceDb (in_argc, in_argv, DEVICE_TYPE_EXECUTOR, "GMYM1")

{
    logStream(MESSAGE_INFO)<<" constructor calisti " <<sendLog;

    createValue (mountLine, "mountLine", "current Mount Line", false, RTS2_VALUE_WRITABLE);
    createValue (exposureLine, "exposureLine", "Current Exposure Line", false,RTS2_VALUE_WRITABLE);
//createValue (slot, "current_slot", "current slot", false,RTS2_VALUE_WRITABLE);
    mountLine->setValueInteger(0);
    createValue (enableSlot, "enabled", "slot is enabled or disabled", false,RTS2_VALUE_WRITABLE);
    enableSlot->setValueBool(false);
    plan.readPlan();
    selectedSlot =  plan.slotsVect[0];
//plan.readPlan();

}

Gmym::~Gmym (void)
{
}

int Gmym::processOption (int in_opt)
{
    switch (in_opt)
    {
    default:
        return 0;
        //return rts2core::Device::processOption (in_opt);
    }
    return 0;
}

int Gmym::init ()
{
    int ret = rts2core::Device::init ();
//	int ret =0;
    if (ret)
        return ret;
    //ret = notifyConn->init ();
    //if (ret)
    //	return ret;

    //addConnection (notifyConn);

    return ret;
}


int Gmym::setValue (rts2core::Value *oldValue, rts2core::Value *newValue)
{

    if (oldValue == enableSlot && ((rts2core::ValueBool *) newValue)->getValueBool () == false)
    {
        logStream(MESSAGE_INFO)<<"stopping slot"<<sendLog;
        stopSlot();
        return 0;
    }
    else
    {
        logStream(MESSAGE_INFO)<<"starting slot"<<sendLog;
        enableSlot->setValueBool(true);
        postEvent (new rts2core::Event (EVENT_SLOT_CHECK));

	//runSlot(0);
        return 0;
    }

    return rts2db::DeviceDb::setValue (oldValue, newValue);

}

rts2core::DevClient * Gmym::createOtherType (rts2core::Connection * conn, int other_device_type)
{
    logStream(MESSAGE_INFO)<<"createother type calisti "<<conn->getName()<<sendLog;

    /*
     std::cout<<"ismail denedie"<<std::endl;
     logStream(MESSAGE_INFO)<<"createother type calisti "<<conn->getName()<<sendLog;

     logStream(MESSAGE_INFO)<<"createother type calisti "<<conn->getName()<<sendLog;
     //rts2core::DevClient *retC = rts2core::Client::createOtherType (conn, other_device_type);

             if (other_device_type == DEVICE_TYPE_MOUNT)
            {
                    //conn->queCommand (new rts2core::CommandMove (this, (rts2core::DevClientTelescope *) retC, tarPos.ra, tarPos.dec));
       rts2core::DevClient *retC =  new rts2core::DevClientTelescope (conn);

              conn->queCommand (new rts2core::CommandMove (this, (rts2core::DevClientTelescope *) retC, 50, 50));



            }



    */



    switch (other_device_type)
    {
    case DEVICE_TYPE_MOUNT:
    {
        //std::cout<<" mount baglandi" <<std::endl;
        logStream(MESSAGE_INFO)<<" mount connected"<<sendLog;

        mountConnection = conn;
        postEvent (new rts2core::Event (EVENT_MOUNT_READY));
        return new rts2core::DevClientTelescope (conn);

    }

    case DEVICE_TYPE_CCD:
    {
        logStream(MESSAGE_INFO)<<"ccd connected"<<sendLog;

        ccdConnection = conn;
        postEvent (new rts2core::Event (EVENT_CCD_READY));

        return new rts2core::DevClientCamera (conn);

    }
    /*
    		case DEVICE_TYPE_FOCUS:
    			return new rts2core::DevClientFocus (conn);
    	//	case DEVICE_TYPE_PHOT:
    	//		return new rts2script::DevClientPhotExec (conn);
    		case DEVICE_TYPE_DOME:
    		case DEVICE_TYPE_SENSOR:
    		case DEVICE_TYPE_ROTATOR:

    */
    default:

        return rts2core::Device::createOtherType (conn, other_device_type);

    }



//return rts2core::Device::createOtherType (conn, other_device_type);

}

void Gmym::postEvent (rts2core::Event * event)
{
    switch (event->getType ())
    {
    case EVENT_MOUNT_READY:
    {
        logStream(MESSAGE_INFO)<<" mount ready event "<<sendLog;
        mountConnected=true;
        if(cameraConnected&&mountConnected)
            postEvent(new rts2core::Event(EVENT_START_SLOT));

        break;
    }

    case EVENT_CCD_READY:
    {
        logStream(MESSAGE_INFO)<<" ccd ready event "<<sendLog;
        cameraConnected=true;
        if(cameraConnected&&mountConnected)
            postEvent(new rts2core::Event(EVENT_START_SLOT));
        break;
    }

    case EVENT_START_SLOT:
    {
        logStream(MESSAGE_INFO)<<"You can Start Slot "<<sendLog;
        //startSlot();

        break;
    }

    case EVENT_MOVE_OK:
        /*
                                if (waitState)
                                {
                                        postEvent (new rts2core::Event (EVENT_CLEAR_WAIT));
                                        break;
                                }
                                postEvent (new rts2core::Event (EVENT_OBSERVE));
                                break;
        */
    {
        logStream(MESSAGE_INFO)<<"moveok event olustu"<<sendLog;
        break;
    }

    case EVENT_EXPOSE_CHECK:

    {
        logStream(MESSAGE_INFO)<<"checking if exposing"<<sendLog;

        if (ccdConnection->getState () & (CAM_EXPOSING | CAM_READING))
        {
            logStream(MESSAGE_INFO)<<"is cam exposing ....yes it is"<<sendLog;
            postEvent(new rts2core::Event(EVENT_EXPOSE_CHECK));
            sleep(1);
            //addTimer (100, new rts2core::Event (EVENT_EXPOSE_CHECK));
        }
        break;
    }

    case EVENT_MOUNT_CHECK:
    {

        logStream(MESSAGE_INFO)<<"checking if mount busy "<<sendLog;

        if (mountConnection->getState () & (TEL_OBSERVING | TEL_MOVING))
        {
            logStream(MESSAGE_INFO)<<"is mount busy....yes it is"<<sendLog;
            postEvent(new rts2core::Event(EVENT_MOUNT_CHECK));
            sleep(1);
            //addTimer (100, new rts2core::Event (EVENT_MOUNT_CHECK));

        }
        break;
    }
    case EVENT_SLOT_CHECK:
    {
	logStream(MESSAGE_INFO)<<"Slot  check calisti waitingforMountMove is "<<waitingforMountMove<<sendLog;    
	if(waitingforMountMove==false)
	{
	logStream(MESSAGE_INFO)<<"waitingforMountMove is false"<<sendLog;
        if((mountConnection->getState () & TEL_MASK_MOVING) != TEL_MOVING)
	{
	logStream(MESSAGE_INFO)<<"running slot "<<mountLine->getValueInteger()<<sendLog;	
	runSlot(mountLine->getValueInteger());
	}
	}
	// addTimer (10, new Rts2Event (EVENT_MY_TIMER, this));
	if(selectedSlot.mounts.size()>mountLine->getValueInteger())
	addTimer(0.1,new rts2core::Event(EVENT_SLOT_CHECK));
	else
		logStream(MESSAGE_INFO)<<"end of the slot"<<sendLog;
        break;
    }
    case EVENT_FIRE_MOUNT:
    {
        logStream(MESSAGE_INFO)<<"fire event "<<mountss.str().c_str()<<sendLog;
   	mountConnection->queCommand (new rts2core::Command (this,mountss.str().c_str()));
	plan.waitSec(1);
	mountLine->setValueInteger(mountLine->getValueInteger()+1);
	sendValueAll(mountLine);
	waitingforMountMove=false;
	break;
    
    }

    case EVENT_FIRE_CAMERA:
    {

	logStream(MESSAGE_INFO)<<"fire event "<<mountss.str().c_str()<<sendLog;
       // ccdConnection->queCommand (new rts2core::Command (this,camerass.str().c_str()));
	ccdConnection->queCommand (new rts2core::Command (this, "expose"));
	exposureLine->setValueInteger(exposureLine->getValueInteger()+1);
        sendValueAll(exposureLine);
		//
	break;
    }
    }
    rts2core::Device::postEvent (event);
}

void Gmym::deviceReady (rts2core::Connection * conn)
{
//int iso = 3;

    logStream(MESSAGE_INFO)<<"device ready calisti info time is  "<<conn->getInfoTime()<<sendLog;
    logStream(MESSAGE_INFO)<<"state string is "<<conn->getStateString ().c_str ()<<sendLog;
    //postEvent(new rts2core::Event(EVENT_MOVE_OK));
    /*
    	if (currentTarget)
    		conn->postEvent (new rts2core::Event (EVENT_SET_TARGET, (void *) currentTarget));

    */
}

int Gmym::idle()
{
//logStream(MESSAGE_INFO)<<"idle calisti"<<sendLog;
// postEvent (new rts2core::Event (EVENT_MOUNT_CHECK));
// postEvent (new rts2core::Event (EVENT_EXPOSE_CHECK));


//mntLine++;
//mountLine->setValueInteger(mntLine);
//sendValueAll (mountLine);

//info();

#ifdef RTS2_HAVE_PGSQL
    return rts2db::DeviceDb::idle ();
#else
    return rts2core::Device::idle ();
#endif


}


int Gmym::info ()
{
    //char buf[2];
    logStream(MESSAGE_INFO)<<" info called mountLine is " <<mntLine<<" mountline " <<mountLine->getValueInteger()<<sendLog;
    //mountLine->setValueInteger(mntLine);

    return rts2core::Device::info ();
    //return 0;
}

void Gmym::changeMasterState (rts2_status_t old_state, rts2_status_t new_state)
{

}
void Gmym::stopSlot()
{
    logStream(MESSAGE_INFO)<<"Before cleaning slots size of the plan vector is "<<plan.slotsVect.size()<<sendLog;
    plan.clearSlots();
    logStream(MESSAGE_INFO)<<"stop called and size of the plan vector is "<<plan.slotsVect.size()<<sendLog;
    mountLine->setValueInteger(0);
    exposureLine->setValueInteger(0);
    sendValueAll(mountLine);
    sendValueAll(exposureLine);

}

void Gmym::runSlot(int lineNumber)
{

logStream(MESSAGE_INFO)<<"runSlot baslangici aktif slot"<<lineNumber<<sendLog;
    //plan.readPlan();
    //selectedSlot =  plan.slotsVect[0];

   // std::stringstream ss;
    mountss.str("");
    mountss << "altaz "<<selectedSlot.mounts[lineNumber][4].second<<" "<<selectedSlot.mounts[lineNumber][3].second;
  //  std::string moveTo=ss.str();
//    logStream(MESSAGE_INFO)<<"move command is "<<moveTo<<sendLog;
    camerass.str("");
    camerass<< "expose " << selectedSlot.images[lineNumber][3].second;
//    logStream(MESSAGE_INFO)<<"expose command is "<<ss.str()<<sendLog;
//    logStream(MESSAGE_INFO) << "Exptime is :"<< selectedSlot.images[lineNumber][3].second
//	                                     << "Number of exp is "<< selectedSlot.images[lineNumber][4].second << sendLog;


    if((std::stoll(selectedSlot.mounts[lineNumber][1].second)*1000+std::stoll(selectedSlot.mounts[lineNumber][2].second))-getEpochNow()>0)
     {
      //plan.waitMicroSec((std::stoll(selectedSlot.mounts[i][1].second)*1000+std::stoll(selectedSlot.mounts[i][2].second))-getEpochNow());	     
      long wait = (std::stoll(selectedSlot.mounts[lineNumber][1].second)*1000+std::stoll(selectedSlot.mounts[lineNumber][2].second))-getEpochNow();
      double waitSec= wait/1000000;
      logStream(MESSAGE_INFO)<<"wait micro is "<<wait<<" seconds is "<<waitSec<<sendLog;
      long waitCamera=(std::stoll(selectedSlot.images[lineNumber][1].second)*1000+std::stoll(selectedSlot.images[lineNumber][2].second))-getEpochNow();
      double waitCameraSec=waitCamera/1000000;
      waitingforMountMove=true;
      addTimer(waitSec,new rts2core::Event(EVENT_FIRE_MOUNT));
      addTimer(waitCameraSec,new rts2core::Event(EVENT_FIRE_CAMERA));

     }
    
     else
	     
     {
	     logStream(MESSAGE_INFO)<<"slot gec kaldı"<<sendLog;
	     mountLine->setValueInteger(lineNumber+1);
	     exposureLine->setValueInteger(lineNumber+1);
             sendValueAll(mountLine);
	     sendValueAll(exposureLine);


     }

    /*
    mountConnection->queCommand (new rts2core::Command (this,moveTo.c_str()));
    //ccdConnection->queCommand (new rts2core::Command (this, ss.str().c_str()));
    ccdConnection->queCommand (new rts2core::Command (this, "expose"));
//    logStream(MESSAGE_INFO)<<"mount line is"<<mountLine->getValueInteger()<<" slot size is "<<selectedSlot.mounts.size()<<sendLog;
    mountLine->setValueInteger(lineNumber+1);
    exposureLine->setValueInteger(lineNumber+1);
    //lineNumber++;
    sendValueAll(mountLine);
    sendValueAll(exposureLine);
    //infoAll();
    //info();
    */
}
void Gmym::startSlot()

{

    /*
    logStream(MESSAGE_INFO)<<"start Slot function run "<<sendLog;
     postEvent (new rts2core::Event (EVENT_MOUNT_CHECK));

      mountConnection->queCommand (new rts2core::Command (this,"altaz 45 270"));
    //mountConnection->queCommand (new rts2core::Command (this, "altaz 30 30"));
    //mountConnection->queCommand (new rts2core::Command (this, "altaz 40 40"));
    // postEvent (new rts2core::Event (EVENT_EXPOSE_CHECK));
     ccdConnection->queCommand (new rts2core::Command (this, "expose"));
     mountConnection->queCommand (new rts2core::Command (this,"altaz 75 70"));
     ccdConnection->queCommand (new rts2core::Command (this, "expose"));


    mntLine++;
    expLine++;
    mountLine->setValueInteger(mntLine);
    exposureLine->setValueInteger(expLine);
    logStream(MESSAGE_INFO)<<"mount connection name is "<<mountConnection->getName()<<sendLog;


    logStream(MESSAGE_INFO)<<"camera connection name is  "<<ccdConnection->getName()<<sendLog;
    enableSlot->setValueBool(false);
    sendValueAll(enableSlot);
    */

    plan.readPlan();
//plan.applyPlan(plan.slotsVect[0]);
    selectedSlot =  plan.slotsVect[0];

    //for (int i = 0; i < selectedSlot.mounts.size(); i++)

    for (int i = 0; i < 1; i++)

    {


        logStream(MESSAGE_INFO) << "mount time is in epoch ms " << selectedSlot.mounts[i][1].second<<sendLog;
        logStream(MESSAGE_INFO) << "mount time is in microseconds " << selectedSlot.mounts[i][2].second<<sendLog;
        logStream(MESSAGE_INFO)<< "mount is moving to azimuth : "<< selectedSlot.mounts[i][3].second << " Elevation : "
                               << selectedSlot.mounts[i][4].second << sendLog;
        logStream(MESSAGE_INFO) << "exp start time time is in epoch ms "<< selectedSlot.images[i][1].second<<sendLog;
        logStream(MESSAGE_INFO) << "exp start time microseconds is "<< selectedSlot.images[i][2].second<<sendLog;
        logStream(MESSAGE_INFO)	<< "Exptime is :"<< selectedSlot.images[i][3].second
                                << "Number of exp is "<< selectedSlot.images[i][4].second << sendLog;
        logStream(MESSAGE_INFO) << "difference is between mount move and image exp is  "
                                << std::stoll(selectedSlot.mounts[i][1].second)- std::stoll(selectedSlot.images[i][1].second)
                                << sendLog;

        logStream(MESSAGE_INFO)<<"get epcoh now is " <<getEpochNow()<<sendLog;
        logStream(MESSAGE_INFO)<<"difference time to wait is  "<<getEpochNow() - std::stoll(selectedSlot.mounts[i][1].second)*1000+std::stoll(selectedSlot.mounts[i][2].second)<<sendLog;
        /*
                std::stringstream ss;
        	ss << "altaz "<<selectedSlot.mounts[i][4].second<<" "<<selectedSlot.mounts[i][3].second;
                std::string moveTo=ss.str();
                // mountConnection->queCommand (new rts2core::Command (this,moveTo.c_str()));
                logStream(MESSAGE_INFO)<<"command is "<<moveTo<<sendLog;

          */

        if((std::stoll(selectedSlot.mounts[i][1].second)*1000+std::stoll(selectedSlot.mounts[i][2].second))-getEpochNow()>0)
        {
            plan.waitMicroSec((std::stoll(selectedSlot.mounts[i][1].second)*1000+std::stoll(selectedSlot.mounts[i][2].second))-getEpochNow());
            logStream(MESSAGE_INFO)<<"move line "<<i<<sendLog;
            std::stringstream ss;
            ss << "altaz "<<selectedSlot.mounts[i][4].second<<" "<<selectedSlot.mounts[i][3].second;
            std::string moveTo=ss.str();
            logStream(MESSAGE_INFO)<<"move command is "<<moveTo<<sendLog;
            ss.str("");
            ss<< "expose " << selectedSlot.images[i][3].second;
            logStream(MESSAGE_INFO)<<"expose command is "<<ss.str()<<sendLog;

             
            mountConnection->queCommand (new rts2core::Command (this,moveTo.c_str()));
            ccdConnection->queCommand (new rts2core::Command (this, ss.str().c_str()));

        }
        else
        {
            logStream(MESSAGE_INFO)<<"Cannot move line "<<i<<sendLog;

        }

    }


}

int Gmym::commandAuthorized(rts2core::Connection * conn)
{
    if(conn->isCommand("telque"))
    {
        double alt;
        double az;
        if (conn->paramNextDouble (&alt) || conn->paramNextDouble (&az) || !conn->paramEnd ())
            return -2;
        std::ostringstream cmnd;
        cmnd <<"altaz "<< alt <<" " <<az;
        mountConnection->queCommand (new rts2core::Command (this,cmnd.str().c_str()));

    }

    return rts2db::DeviceDb::commandAuthorized (conn);

}
long long Gmym::getEpochNow()
{
    auto time_point = std::chrono::system_clock::now();//is it time point

    long long epochNowinMS = time_point.time_since_epoch()/ std::chrono::microseconds(1) ;//+ std::atof(snext) * 1000.0f;
    return epochNowinMS-10800000;
}
int main (int argc, char **argv)
{

    Gmym gmym (argc, argv);
    return gmym.run ();
}
