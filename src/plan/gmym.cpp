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

#define EVENT_CCD_READY  RTS2_LOCAL_EVENT+55
#define EVENT_MOUNT_READY  RTS2_LOCAL_EVENT+56
#define EVENT_SLOT_CHECK RTS2_LOCAL_EVENT+57
#define EVENT_FIRE_MOUNT RTS2_LOCAL_EVENT+58
#define EVENT_FIRE_CAMERA RTS2_LOCAL_EVENT+59
#include "ObservationPlan.h"
#include "valuearray.h"
#include "device.h"
#include "rts2script/execcli.h"
#include "rts2db/devicedb.h"
#include <iostream>
#include <sstream>


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


    rts2core::ValueInteger *mountLine;
    rts2core::ValueInteger *exposureLine;
    rts2core::ValueBool  *enableSlot;
    bool mountConnected = false;
    bool cameraConnected = false;
    rts2core::Connection *ccdConnection;
    rts2core::Connection *mountConnection;

    void stopSlot();
    void runSlot(int lineNumber);
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
    mountLine->setValueInteger(0);
    createValue (enableSlot, "enabled", "slot is enabled or disabled", false,RTS2_VALUE_WRITABLE);
    enableSlot->setValueBool(false);
    plan.readPlan();
    selectedSlot =  plan.slotsVect[0];

}

Gmym::~Gmym (void)
{
}

int Gmym::processOption (int in_opt)
{
    switch (in_opt)
    {
    default:
        return rts2core::Device::processOption (in_opt);
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

        return 0;
    }

    return rts2db::DeviceDb::setValue (oldValue, newValue);

}

rts2core::DevClient * Gmym::createOtherType (rts2core::Connection * conn, int other_device_type)
{
    logStream(MESSAGE_INFO)<<"createother type calisti "<<conn->getName()<<sendLog;


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




}

void Gmym::postEvent (rts2core::Event * event)
{
    switch (event->getType ())
    {
    case EVENT_MOUNT_READY:
    {
        logStream(MESSAGE_INFO)<<" mount ready event "<<sendLog;
        mountConnected=true;
        //if(cameraConnected&&mountConnected)
        //    postEvent(new rts2core::Event(EVENT_START_SLOT));

        break;
    }

    case EVENT_CCD_READY:
    {
        logStream(MESSAGE_INFO)<<" ccd ready event "<<sendLog;
        cameraConnected=true;
       // if(cameraConnected&&mountConnected)
       //     postEvent(new rts2core::Event(EVENT_START_SLOT));
        break;
    }
/*
    case EVENT_START_SLOT:
    {
        logStream(MESSAGE_INFO)<<"You can Start Slot "<<sendLog;
        //startSlot();

        break;
    }

    case EVENT_MOVE_OK:
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
  
  */
  
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

    return rts2core::Device::info ();
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
    plan.readPlan();
    selectedSlot =  plan.slotsVect[0];


}

void Gmym::runSlot(int lineNumber)
{

    logStream(MESSAGE_INFO)<<"runSlot baslangici aktif slot"<<lineNumber<<sendLog;
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
        if(waitCameraSec>0)addTimer(waitCameraSec,new rts2core::Event(EVENT_FIRE_CAMERA));

    }

    else

    {
        logStream(MESSAGE_INFO)<<"slot gec kaldı"<<sendLog;
        mountLine->setValueInteger(lineNumber+1);
        exposureLine->setValueInteger(lineNumber+1);
        sendValueAll(mountLine);
        sendValueAll(exposureLine);


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
