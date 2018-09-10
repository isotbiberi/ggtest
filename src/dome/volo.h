/* 

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

#ifndef __volo_h__
#define __volo_h__


//static const char* return_from_alarm_register(int alarm_code);
#define BUG_REPORT(_cond, _format, _args ...) \
    printf("\nLine %d: assertion error for '%s': " _format "\n", __LINE__, # _cond, ## _args)



#define ASSERT_TRUE(_cond, _format, __args...) {  \
    if (_cond) {                                  \
        printf("Read or write debugging is OK\n");                           \
    } else {                                      \
        BUG_REPORT(_cond, _format, ## __args);    \
    }                                             \
};


#include <bitset>
#include <modbus/modbus.h>


#define INPUTREGISTER1 0x40000//40001 aslinda
#define INPUTREGISTER2 0x40001
#define OUTPUTREGISTER1 0x40002
#define RIGHTDOMESLOPE 0x40008
#define LEFTDOMESLOPE 0x40009
#define HYDROLICTEMPERATURE 0x40022
#define DOMETEMPERATURE 0x40023
#define RIGHTDOMEPOSITIONCOMMAND 0x40006
#define LEFTDOMEPOSITIONCOMMAND 0x40007


#define CONTROLWORD1 0x40003

#define EMERGENCY_STOP 0
#define RESET_PLC 1
#define RUN_HYDROLIC 2
#define CLOSE_SHUTTER_LEFT 3
#define OPEN_SHUTTER_LEFT 4
#define CLOSE_SHUTTER_RIGHT 5
#define OPEN_SHUTTER_RIGHT 6
#define DOME_LEFT_POSITION 7
#define DOME_RIGHT_POSITION 8
#define HEATER 10
#define LIGHTS 12




#define STATUSWORD1 0x40004
#define CLOSING_SHUTTER_LEFT 0
#define CLOSING_SHUTTER_WRIGHT 1
#define OPENING_DOME_LEFT 2
#define OPENING_DOME_WRIGHT 3
#define ALARM 4
#define HEARTBIT 5

#define ALARMWORD1 0x40005
#define PHASE_PROTECTION_ALARM 0
#define HYDROLIC_MOTOR_ERROR 1
#define HYDROLIC_DIRT_ERROR 2
#define HYDROLIC_OIL_LEVEL_WARNING 3
#define HYDROLIC_OIL_LEVEL_ALARM 4
#define RAIN_SENSOR_ALARM 5
#define COMMUNICATION_ERROR 6
#define SHUTTER_TIME_OUT 7
#define ELECTRICITY_POWER_CUT 8
#define RIGHT_SHUTTER_SWITCH_ALARM 9
#define LEFT_SHUTTER_SWITCH_ALARM 10
#define RIGHT_SHUTTER_SLOPE_SENSOR_ALARM 11
#define LEFT_SHUTTER_SLOPE_SENSOR_ALARM 12


enum DomeStatus
{
     ERROR=-1, OPENED, CLOSED, OPENING, CLOSING

};

namespace rts2dome
{

class VOLO:public Dome
{
    private:
        const char * devFile;
        int fd, cmdSent;
        DomeStatus status;
        char junk[21];

        uint16_t * readControlRegister(modbus_t * mb,uint16_t UT_REGISTERS_ADDRESS,uint16_t* tab_rp_registers);
        uint16_t * readStatusRegister(modbus_t * mb,uint16_t UT_REGISTERS_ADDRESS,uint16_t* tab_rp_registers);
        uint16_t * readAlarmRegister(modbus_t * mb,uint16_t UT_REGISTERS_ADDRESS,uint16_t* tab_rp_registers);
        uint16_t * readRegister(modbus_t * mb,uint16_t UT_REGISTERS_ADDRESS,uint16_t* tab_rp_registers);
        const char* return_from_alarm_register(int alarm_code);
        const char* return_from_status_register(int status_code);
        void checkAlarms(uint16_t * tab_rp_registers);
        void checkStates(uint16_t * tab_rp_registers);
        int openLeftShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int closeLeftShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int openRightShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int closeRightShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int goLeftShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int goRightShutter(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int emergencyStop(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int resetButton(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int runHydrolic(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
	int closeLights(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
	int openLights(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);

	int setHeartBeat2(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int resetHeartBeat2(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
	int resetHeartBit(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
	int stopHydrolic(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int stopAll(uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int setRightDomePosition(uint16_t degree,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int setLeftDomePosition(uint16_t degree,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        int writeToControlRegister(int bitNumber,uint16_t * tab_rp_registers,modbus_t * mb, uint16_t UT_REGISTERS_ADDRESS);
        bool isLeftShutterClosing(uint16_t * tab_rp_registers);
        bool isLeftShutterOpening(uint16_t * tab_rp_registers);
        bool isRightShutterClosing(uint16_t * tab_rp_registers);
	bool isRightShutterOpening(uint16_t * tab_rp_registers);
	bool isThereAlarm(uint16_t * tab_rp_registers);

        int openALeaf();
        int openBLeaf();
        int closeALeaf();
        int closeBLeaf();
        char getHeartBeat();
        rts2core::ValueString *domeStatus;
        rts2core::ValueSelection *closeDome;
	rts2core::ValueString *alarmWord;
	rts2core::ValueString *statusWord;
	rts2core::ValueString *controlWord;
        rts2core::ValueString *rightAngle;
        rts2core::ValueString *leftAngle;

        modbus_t *mb=NULL;
        uint16_t *anyRegister;

    protected:
        virtual int processOption(int opt);
        virtual int setValue(rts2core::Value *old_value, rts2core::Value *new_value);
        virtual void postEvent (rts2core::Event * event);

	virtual int commandAuthorized (rts2core::Connection * conn);

        virtual int startOpen();
        virtual long isOpened();
        virtual int endOpen();

        virtual int startClose();
        virtual long isClosed();
        virtual int endClose();


    public:
        VOLO(int argc, char ** argv);
        virtual ~VOLO();
        virtual int init();
        virtual int info();
};
}


#endif
