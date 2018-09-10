/*
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
#include "volo.h"
#define EVENT_RESET_HEART_BEAT  RTS2_LOCAL_EVENT+1350

using namespace rts2dome;

const char* VOLO::return_from_alarm_register(int alarm_code)
{

    const char *alarm;

    switch (alarm_code)
    {
    case PHASE_PROTECTION_ALARM:
        alarm = "Error with the electricity phases";
        break;
    case HYDROLIC_MOTOR_ERROR:
        alarm = "Hydrolic motor problem";
        break;
    case HYDROLIC_DIRT_ERROR :
        alarm = "Hydrolic dirty problem";
        break;
    case HYDROLIC_OIL_LEVEL_WARNING:
        alarm = "Hydrolic oil level warning";
        break;
    case HYDROLIC_OIL_LEVEL_ALARM:
        alarm = "Hydrolic oil level warning";
        break;
    case RAIN_SENSOR_ALARM:
        alarm = "RAIN SENSOR ALARM";
        break;
    case COMMUNICATION_ERROR:
        alarm = "COMMUNICATION ERROR";
        break;
    case SHUTTER_TIME_OUT:
        alarm = "SHUTER TIME OUT ALARM";
        break;
    case ELECTRICITY_POWER_CUT:
        alarm = "ELECTRICITY POWER CUT";
        break;
    case RIGHT_SHUTTER_SWITCH_ALARM:
        alarm = "RIGTH_SHUTTER_SWITCH ALARM";
        break;
    case LEFT_SHUTTER_SWITCH_ALARM:
        alarm = "LEFT SHUTTER SWITCH ALARM";
        break;
    case RIGHT_SHUTTER_SLOPE_SENSOR_ALARM:
        alarm = "RIGHT SHUTTER SLOPE SENSOR ALARM";
        break;
    case LEFT_SHUTTER_SLOPE_SENSOR_ALARM:
        alarm = "LEFT SHUTTER SLOPE ALARM";
        break;

    default:
        alarm = "Unknown alarm";
        break;
    }

    return alarm;

}
const char* VOLO::return_from_status_register(int status_code)
{

    const char *status;

    switch (status_code)
    {
    case CLOSING_SHUTTER_LEFT:
        status = "Left shutter is closing";
        break;
    case CLOSING_SHUTTER_WRIGHT:
        status = "Right shutter is closing";
        break;
    case OPENING_DOME_LEFT:
        status = "Left shutter is opening";
        break;
    case OPENING_DOME_WRIGHT:
        status = "Right shutter is opening";
        break;
    case ALARM:
        status = "ALARM";
        break;

    default:
        status = "Unknown state";
        break;
    }

    return status;

}



void VOLO::checkAlarms(uint16_t * tab_rp_registers)
{
    std::bitset<16> IntBits = tab_rp_registers[0];
    bool is_set;
    for (int i = 0; i < 16; i++) {
        is_set = IntBits.test(i);
        if (is_set)
            logStream(MESSAGE_INFO)<< return_from_alarm_register(i) << sendLog;
    }
}

void VOLO::checkStates(uint16_t * tab_rp_registers)
{
    std::bitset<16> IntBits = tab_rp_registers[0];
    bool is_set;
    for (int i = 0; i < 16; i++) {
        is_set = IntBits.test(i);
        if (is_set)
            logStream(MESSAGE_INFO)<< return_from_status_register(i) << sendLog;
    }
}


int VOLO::openLeftShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(OPEN_SHUTTER_LEFT);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"Open Left Shutter bit is Set "<<sendLog;

    return rc;
}

int VOLO::goLeftShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(DOME_LEFT_POSITION);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"Open Left Shutter bit is Set "<<sendLog;

    return rc;
}

int VOLO::closeLeftShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(CLOSE_SHUTTER_LEFT);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"Close Left shutter bit is set "<<sendLog;
    //ASSERT_TRUE(rc == 1, "");
    return rc;//returns 1 if modbus write  is ok
}


int VOLO::openRightShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(OPEN_SHUTTER_RIGHT);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"Open right shutter bit is set: "<<sendLog;

    return rc;
}

int VOLO::goRightShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(DOME_RIGHT_POSITION);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"Open Left Shutter bit is Set"<<sendLog;
    ASSERT_TRUE(rc == 1, "");
    return rc;
}



int VOLO::closeRightShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(CLOSE_SHUTTER_RIGHT);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"Close right shutter bit is set:"<<sendLog;

    return rc;
}


int  VOLO::emergencyStop(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(EMERGENCY_STOP);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"Emergency stop bit is set: "<<sendLog;

    return rc;
}

int VOLO::resetButton(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(RESET_PLC);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"Reset button bit is set: "<<sendLog;

    return rc;
}

int VOLO::runHydrolic(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(RUN_HYDROLIC);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"run Hydrolic bit is set:"<<sendLog;

    return rc;
}

int VOLO::openLights(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(LIGHTS);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"lights bit is set:"<<sendLog;

    return rc;
}

int VOLO::closeLights(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.reset(LIGHTS);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<" lights bit is reset:"<<sendLog;

    return rc;
}


int VOLO::setHeartBeat2(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(9);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    //logStream(MESSAGE_INFO)<<"reset heartbeat bit is reset: "<<sendLog;

    return rc;
}

int VOLO::resetHeartBeat2(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.reset(9);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    //logStream(MESSAGE_INFO)<<"reset heartbeat bit is reset: "<<sendLog;

    return rc;
}



int VOLO::resetHeartBit(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    std::cout<<bitSet.test(HEARTBIT)<<std::endl;
    bitSet.reset(HEARTBIT);
    std::cout<<bitSet.test(HEARTBIT)<<std::endl;
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"heartBeat reset "<<sendLog;

    return rc;

}

int VOLO::stopHydrolic(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.reset(RUN_HYDROLIC);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"run Hydrolic bit is reset: "<<sendLog;

    return rc;
}

int VOLO::stopAll(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    //std::bitset<16> bitSet = tab_rp_registers[0];
    //bitSet.reset(OPEN_SHUTTER_LEFT);
    tab_rp_registers[0]=0x0000;
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, tab_rp_registers[0]);
    logStream(MESSAGE_INFO)<<"run Stop All: "<<sendLog;

    return rc;
}




int VOLO::setRightDomePosition(uint16_t degree,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, degree);
    logStream(MESSAGE_INFO)<<"Write to Control register function ... "<<sendLog;

    return rc;
}

int VOLO::setLeftDomePosition(uint16_t degree,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, degree);
    logStream(MESSAGE_INFO)<<"Write to Control register function ... "<<sendLog;

    return rc;

}
int VOLO::writeToControlRegister(int bitNumber,uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS)
{

    std::bitset<16> bitSet = tab_rp_registers[0];
    bitSet.set(bitNumber);
    int rc = modbus_write_register(mb, UT_REGISTERS_ADDRESS, bitSet.to_ulong());
    logStream(MESSAGE_INFO)<<"Write to Control register function ... "<<sendLog;

    return rc;

}

uint16_t * VOLO::readControlRegister(modbus_t * mb,uint16_t UT_REGISTERS_ADDRESS,uint16_t* tab_rp_registers)
{
    int rc = modbus_read_registers(mb, UT_REGISTERS_ADDRESS,1, tab_rp_registers);

    //ASSERT_TRUE(rc == 1, "FAILED (read Control register %d)\n", rc);
    //logStream(MESSAGE_INFO)<<"hex number of control register is "<<std::hex<<tab_rp_registers[0]<<" The address is  "<<UT_REGISTERS_ADDRESS<<sendLog;
    return tab_rp_registers;
}

uint16_t * VOLO::readStatusRegister(modbus_t * mb,uint16_t UT_REGISTERS_ADDRESS,uint16_t* tab_rp_registers)
{
    int rc = modbus_read_registers(mb, UT_REGISTERS_ADDRESS,
                                   1, tab_rp_registers);

    //ASSERT_TRUE(rc == 1, "FAILED (read Status register %d)\n", rc);

    //logStream(MESSAGE_INFO)<<"hex number of status register is "<<std::hex<<tab_rp_registers[0]<<" The address is  "<<UT_REGISTERS_ADDRESS<<" binary value is "<<std::bitset<16>(tab_rp_registers[0])<<sendLog;
    return tab_rp_registers;
}

uint16_t * VOLO::readAlarmRegister(modbus_t * mb,uint16_t UT_REGISTERS_ADDRESS,uint16_t* tab_rp_registers)
{
    int rc = modbus_read_registers(mb, UT_REGISTERS_ADDRESS,
                                   1, tab_rp_registers);

    //ASSERT_TRUE(rc == 1, "FAILED (readAlarmRegister %d)\n", rc);
    //logStream(MESSAGE_INFO)<<"hex number of alarm register is "<<std::hex<<tab_rp_registers[0]<<" The address is  "<<UT_REGISTERS_ADDRESS<<" binary value is "<<std::bitset<16>(tab_rp_registers[0])<<sendLog;
    return tab_rp_registers;
}


uint16_t * VOLO::readRegister(modbus_t * mb,uint16_t UT_REGISTERS_ADDRESS,uint16_t* tab_rp_registers)
{
    int rc = modbus_read_registers(mb, UT_REGISTERS_ADDRESS,
                                   1, tab_rp_registers);

    //ASSERT_TRUE(rc == 1, "FAILED (read Status register %d)\n", rc);

    //logStream(MESSAGE_INFO)<<"hex number read register is "<<std::hex<<tab_rp_registers[0]<<" The address is  "<<UT_REGISTERS_ADDRESS<<" binary value is "<<std::bitset<16>(tab_rp_registers[0])<<sendLog;
    return tab_rp_registers;
}



bool VOLO::isLeftShutterClosing(uint16_t * tab_rp_registers)
{
    bool value = false;
    std::bitset<16> bitSet = tab_rp_registers[0];
    value = bitSet.test(0);
    return value;

}

bool VOLO::isLeftShutterOpening(uint16_t * tab_rp_registers)
{
    bool value = false;
    std::bitset<16> bitSet = tab_rp_registers[0];
    value = bitSet.test(2);
    return value;
}

bool VOLO::isRightShutterClosing(uint16_t * tab_rp_registers)
{
    bool value = false;
    std::bitset<16> bitSet = tab_rp_registers[0];
    value = bitSet.test(1);
    return value;
}

bool VOLO::isRightShutterOpening(uint16_t * tab_rp_registers)
{
    bool value = false;
    std::bitset<16> bitSet = tab_rp_registers[0];
    value = bitSet.test(3);
    return value;
}

bool VOLO::isThereAlarm(uint16_t * tab_rp_registers)
{
    bool value = false;
    std::bitset<16> bitSet = tab_rp_registers[0];
    value = bitSet.test(4);
    return value;
}



int VOLO::openALeaf()
{


    //anyRegister =  (uint16_t*) malloc(sizeof(uint16_t));
    //memset(anyRegister, 0, sizeof(uint16_t));

    readControlRegister(mb,CONTROLWORD1,anyRegister);

    /*open left shutter*/

    int err = openLeftShutter(anyRegister,mb, CONTROLWORD1);

    /*open left shutter*/

    return 1;
}

int VOLO::openBLeaf()
{


    //uint16_t *anyRegister =  (uint16_t*) malloc(sizeof(uint16_t));
    //memset(anyRegister, 0, sizeof(uint16_t));

    readControlRegister(mb,CONTROLWORD1,anyRegister);


    /*open right shutter*/


    int err = openRightShutter(anyRegister,mb, CONTROLWORD1);



    /*open right shutter*/



    return 1;
}

int VOLO::closeALeaf()
{

    //uint16_t *anyRegister =  (uint16_t*) malloc(sizeof(uint16_t));
    //memset(anyRegister, 0, sizeof(uint16_t));

    readControlRegister(mb,CONTROLWORD1,anyRegister);



    /*close left shutter*/

    int err = closeLeftShutter(anyRegister,mb, CONTROLWORD1);


    /*close left shutter*/





    return 1;
}

int VOLO::closeBLeaf()
{




    //uint16_t *anyRegister =  (uint16_t*) malloc(sizeof(uint16_t));
    //memset(anyRegister, 0, sizeof(uint16_t));

    readControlRegister(mb,CONTROLWORD1,anyRegister);


    /*close right shutter*/


    int err = closeLeftShutter(anyRegister,mb, CONTROLWORD1);


    /*close right shutter*/



    return 1;
}


int VOLO::processOption(int opt)
{
    switch (opt)
    {
    case 'f':
        devFile = optarg;
        break;
    default:
        return Dome::processOption (opt);
    }
    return 0;
}

int VOLO::setValue(rts2core::Value *old_value, rts2core::Value *new_value)
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

    return Dome::setValue(old_value, new_value);
}

int VOLO::init()
{


    uint32_t old_response_to_sec;
    uint32_t old_response_to_usec;
    uint32_t new_response_to_sec;
    uint32_t new_response_to_usec;


    mb = modbus_new_tcp("192.168.2.11", 502);
    //mb = modbus_new_tcp("127.0.0.1", 1502);
    if(mb==NULL)
    {
        logStream(MESSAGE_INFO)<<"unable to allocate libmodbus"<<sendLog;
        return -1;
    }

    modbus_set_debug(mb, FALSE);//false yaptım data paketini ekrana yazıyor
    modbus_set_error_recovery(mb, MODBUS_ERROR_RECOVERY_PROTOCOL);
    modbus_set_error_recovery(mb, MODBUS_ERROR_RECOVERY_LINK);

    modbus_get_response_timeout(mb, &old_response_to_sec, &old_response_to_usec);


    if (modbus_connect(mb) == -1)
    {
        logStream(MESSAGE_INFO)<<"Connection failed: "<<modbus_strerror(errno)<<sendLog;
        modbus_free(mb);
        return -1;
    }

    modbus_get_response_timeout(mb, &new_response_to_sec, &new_response_to_usec);



    //printf("1/1 No response timeout modification on connect: ");
    //ASSERT_TRUE(old_response_to_sec == new_response_to_sec &&
    //           old_response_to_usec == new_response_to_usec, "");


    anyRegister =  (uint16_t*) malloc(sizeof(uint16_t));
    memset(anyRegister, 0, sizeof(uint16_t));


    info();
    addTimer(1,new rts2core::Event(EVENT_RESET_HEART_BEAT));//start HeartBeat

    int ret = Dome::init();
    if(ret)
    {
        return ret;
    }

    return 0;
}

char VOLO::getHeartBeat()
{


}

int VOLO::info()
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
    logStream(MESSAGE_INFO)<<" info is called " <<sendLog;
    memset(anyRegister, 0, sizeof(uint16_t));

    readRegister(mb, CONTROLWORD1, anyRegister);
    std::stringstream sstream;
    sstream << std::bitset<16>(anyRegister[0]);
    std::string result = sstream.str();
    controlWord->setValueString(result.c_str());


    memset(anyRegister, 0, sizeof(uint16_t));



    readRegister(mb, ALARMWORD1, anyRegister);
    std::stringstream sstream1;
    sstream1 << std::bitset<16>(anyRegister[0]);
    result = sstream1.str();
    alarmWord->setValueString(result.c_str());


    memset(anyRegister, 0, sizeof(uint16_t));

    readRegister(mb,STATUSWORD1,anyRegister);
    std::stringstream sstream2;
    sstream2 << std::bitset<16>(anyRegister[0]);
    result = sstream2.str();
    statusWord->setValueString(result.c_str());

    memset(anyRegister, 0, sizeof(uint16_t));

    readRegister(mb,LEFTDOMESLOPE,anyRegister);
    std::stringstream sstream3;
    sstream3 << anyRegister[0];
    result = sstream3.str();
    leftAngle->setValueString(result.c_str());

    memset(anyRegister, 0, sizeof(uint16_t));

    readRegister(mb,RIGHTDOMESLOPE,anyRegister);
    std::stringstream sstream4;
    sstream4 << anyRegister[0];
    result = sstream4.str();
    rightAngle->setValueString(result.c_str());

    logStream(MESSAGE_INFO)<<"controlword is "<<controlWord->getValueString()<<sendLog;
    logStream(MESSAGE_INFO)<<"alarmword is "<<alarmWord->getValueString()<<sendLog;
    logStream(MESSAGE_INFO)<<"statusword is "<<statusWord->getValueString()<<sendLog;
    logStream(MESSAGE_INFO)<<"leftangle is "<<leftAngle->getValueString()<<sendLog;
    logStream(MESSAGE_INFO)<<"rightangle is "<<rightAngle->getValueString()<<sendLog;




    sendValueAll(controlWord);
    sendValueAll(alarmWord);
    sendValueAll(statusWord);
    sendValueAll(rightAngle);
    sendValueAll(leftAngle);
    return Dome::info();
}


int VOLO::startOpen()
{




    status = OPENING;
    domeStatus->setValueString("Opening....");
    sendValueAll(domeStatus);
    if(openBLeaf() && openALeaf())
    {
        status = OPENED;
        domeStatus->setValueString("Opened");
        closeDome->setValueString("Open");

        return 0;
    }
    else
    {
        status = ERROR;
        domeStatus->setValueString("Error opening");
    }


    return 1;
}

int VOLO::startClose()
{



    status = CLOSING;
    domeStatus->setValueString("Closing....");
    sendValueAll(domeStatus);
    if(closeBLeaf() && closeALeaf())
    {
        status = CLOSED;
        domeStatus->setValueString("Closed");
        closeDome->setValueString("Close");
        return 0;
    }
    else
    {
        status = ERROR;
        domeStatus->setValueString("Error closing");
    }




    return 1;

}

long VOLO::isOpened()
{
    if (status == OPENED)
    {
        return -2;
    }

    return 2000;
}

long VOLO::isClosed()
{
    if (status == CLOSED)
    {
        return -2;
    }

    return 2000;
}

int VOLO::endOpen()
{
    //do nothing
    return 0;
}



int VOLO::endClose()
{
    //do nothing
    domeStatus->setValueString("Closed");
    return 0;
}


VOLO::VOLO(int argc, char **argv):Dome(argc, argv)
{
    logStream (MESSAGE_DEBUG) << "VOLO Dome constructor called" << sendLog;


    createValue(domeStatus, "dome_status", "current dome status", true);
    createValue(closeDome, "state", "switch state of dome", false, RTS2_VALUE_WRITABLE);

    closeDome->addSelVal("Close");
    closeDome->addSelVal("Open");

    createValue(alarmWord, "ALARMWORD", "complete alarm word modbus register", true);
    createValue(controlWord, "CONTROLWORD", "complete control word modbus register", true);
    createValue(statusWord, "STATUSWORD", "complete statusword modbus register", true);
    createValue(rightAngle, "RIGHTANGLE", "right leaf angle sensor value", true);
    createValue(leftAngle, "LEFTANGLE", "lef leaf angle sensor value", true);



    setIdleInfoInterval(5);
}

VOLO::~VOLO()
{
}

int VOLO::commandAuthorized (rts2core::Connection * conn)
{
    if (conn->isCommand ("openleft"))
    {
        readControlRegister(mb,CONTROLWORD1,anyRegister);
        int err = openLeftShutter(anyRegister,mb, CONTROLWORD1);
        if(err ==-1)
        {
            logStream(MESSAGE_INFO)<<"error while opening left"<<sendLog;;
            return -1;
        }
        return 0;
    }
    else if (conn->isCommand ("openright"))
    {
        readControlRegister(mb,CONTROLWORD1,anyRegister);
        int err = openRightShutter(anyRegister,mb, CONTROLWORD1);
        if(err ==-1)
        {
            logStream(MESSAGE_INFO)<<"error while opening right"<<sendLog;;
            return -1;
        }
        return 0;
    }
    else if(conn->isCommand("closeleft"))
    {
        readControlRegister(mb,CONTROLWORD1,anyRegister);
        int err = closeLeftShutter(anyRegister,mb, CONTROLWORD1);
        if(err ==-1)
        {
            logStream(MESSAGE_INFO)<<"error while closing left"<<sendLog;;
            return -1;
        }
        return 0;
    }
    else if(conn->isCommand("closeright"))
    {
        readControlRegister(mb,CONTROLWORD1,anyRegister);
        int err = closeRightShutter(anyRegister,mb, CONTROLWORD1);
        if(err ==-1)
        {
            logStream(MESSAGE_INFO)<<"error while closing right"<<sendLog;;
            return -1;
        }
        return 0;
    }
    else if(conn->isCommand("runhydrolic"))
    {
        readControlRegister(mb,CONTROLWORD1,anyRegister);
        int err = runHydrolic(anyRegister,mb, CONTROLWORD1);
        if(err ==-1)
        {
            logStream(MESSAGE_INFO)<<"error while closing left"<<sendLog;;
            return -1;
        }
        return 0;
    }

    else if(conn->isCommand("openLights"))
    {
        readControlRegister(mb,CONTROLWORD1,anyRegister);
        int err = openLights(anyRegister,mb, CONTROLWORD1);
        if(err ==-1)
        {
            logStream(MESSAGE_INFO)<<"error while closing left"<<sendLog;;
            return -1;
        }
        return 0;
    }

    else if(conn->isCommand("closeLights"))
    {
        readControlRegister(mb,CONTROLWORD1,anyRegister);
        int err = closeLights(anyRegister,mb, CONTROLWORD1);
        if(err ==-1)
        {
            logStream(MESSAGE_INFO)<<"error while closing left"<<sendLog;;
            return -1;
        }
        return 0;
    }





    else if(conn->isCommand("setleft"))
    {
        int position;

        if (conn->paramNextInteger (&position) || !conn->paramEnd ())
            return -2;

        int err = setLeftDomePosition(position, mb,LEFTDOMEPOSITIONCOMMAND);
        if (err == -1)
        {
            logStream(MESSAGE_INFO)<<"Error while set left dome position "<<sendLog;
            return -1;
        }

        return 0;
    }
    else if(conn->isCommand("setright"))
    {
        int position;

        if (conn->paramNextInteger (&position) || !conn->paramEnd ())
            return -2;

        int err = setRightDomePosition(position, mb,RIGHTDOMEPOSITIONCOMMAND);
        if (err == -1)
        {
            logStream(MESSAGE_INFO)<<"Error while set right dome position "<<sendLog;
            return -1;
        }
        return 0;
    }

    else if(conn->isCommand("stophydrolic"))
    {
        readControlRegister(mb,CONTROLWORD1,anyRegister);
        int err = stopHydrolic(anyRegister, mb, CONTROLWORD1);
        if(err ==-1)
        {
            logStream(MESSAGE_INFO)<<"error while stoping hydrolic"<<sendLog;;
            return -1;
        }
        return 0;
    }

    else if(conn->isCommand("stopall"))
    {
        readControlRegister(mb,CONTROLWORD1,anyRegister);
        int err = stopAll(anyRegister, mb, CONTROLWORD1);
        if(err ==-1)
        {
            logStream(MESSAGE_INFO)<<"error while stoping ALL"<<sendLog;;
            return -1;
        }
        return 0;
    }

    else if(conn->isCommand("heartbeat"))
    {
        return 0;
    }

    else if(conn->isCommand("clearalarms"))
    {
        readControlRegister(mb,CONTROLWORD1,anyRegister);
        int err = resetButton(anyRegister, mb, CONTROLWORD1);
        if(err ==-1)
        {
            logStream(MESSAGE_INFO)<<"clear alarms"<<sendLog;;
            return -1;
        }
        return 0;
    }
    return Dome::commandAuthorized (conn);
}
void VOLO::postEvent (rts2core::Event * event)
{
    switch (event->getType ())
    {

    case EVENT_RESET_HEART_BEAT:
    {


        uint16_t *anyRegister2 =  (uint16_t*) malloc(sizeof(uint16_t));
        memset(anyRegister2, 0, sizeof(uint16_t));
        readControlRegister(mb, CONTROLWORD1, anyRegister2);
        int err = setHeartBeat2(anyRegister2, mb, CONTROLWORD1);
        if (err == -1) {
            logStream(MESSAGE_INFO)<<"error while setting heartbeat"<<sendLog;

        }
        else
        {
            //logStream(MESSAGE_INFO)<<"setted heartbeat"<<sendLog;
        }

        readControlRegister(mb, CONTROLWORD1, anyRegister2);
        err = resetHeartBeat2(anyRegister2, mb, CONTROLWORD1);

        if (err == -1) {
            logStream(MESSAGE_INFO)<<"error while resetting heartbeat"<<sendLog;

        }
        else
        {
            //logStream(MESSAGE_INFO)<<"Resetted heartbeat\n"<<sendLog;
        }

        //addTimer (3, event);
        addTimer(1,new rts2core::Event(EVENT_RESET_HEART_BEAT));

        break;
    }

    rts2core::Device::postEvent (event);
    }

}

int main(int argc, char **argv)
{
    VOLO device(argc, argv);
    return device.run();
}
