/*!
 * @file Driver for ASA ddm160 telescope.
 *
 *
 *
 * @author ismail
 */

#include "altaz.h"
#define OPT_SHOW_PID	OPT_LOCAL + 7
#include "ASAMountModule_Remote2.h"
#include "configuration.h"



namespace rts2teld
{

class ddm160:public AltAz
{
public:
		ddm160 (int argc, char **argv);
		~ddm160 (void);

	protected:
		virtual int processOption (int in_opt);

		virtual int initHardware ();

		virtual int commandAuthorized (rts2core::Connection *conn);

		virtual int info ();

		virtual int startResync ();

		virtual int moveAltAz ();

		virtual int stopMove ();

		virtual int startPark ();

		virtual int endPark ()
		{
			return 0;
		}

		virtual int isMoving ();

		virtual int isMovingFixed ()
		{
			return isMoving ();
		}

		virtual int isParking ();

		virtual int isOffseting ();

		virtual void runTracking ();

//		virtual int setValue (rts2core::Value *oldValue, rts2core::Value *newValue);

		virtual double estimateTargetTime ()
		{
			return getTargetDistance () * 2.0;
		}

		virtual void changeIdleMovingTracking ();

	private:
		void scaleTrackingLook ();
		void internalTracking (double sec_step, float speed_factor);

		void updateTelTime ();

		const char *tel_tty;
		asa::ASAMountModule_Remote *MyMount;
                asa::mountstatus MountStatus;
                bool mountconnected;
                double decValue,raValue;



		bool mountConnected= false;

                int sayac=0;
		rts2core::ValueString *sitechLogFile;


		rts2core::ValueDouble *sitechVersion;
		rts2core::ValueInteger *sitechSerial;

		rts2core::ValueLong *r_az_pos;
		rts2core::ValueLong *r_alt_pos;

		rts2core::ValueLong *t_az_pos;
		rts2core::ValueLong *t_alt_pos;

		rts2core::ValueDouble *trackingDist;
		rts2core::ValueFloat *trackingLook;
		rts2core::ValueBool *userTrackingLook;
		rts2core::ValuePID *azErrPID;
		rts2core::ValuePID *altErrPID;
		int lastTrackingNum;
		rts2core::ValueDoubleStat *speedAngle;
		rts2core::ValueDoubleStat *errorAngle;
		rts2core::ValueDouble *slowSyncDistance;
		rts2core::ValueFloat *fastSyncSpeed;
		rts2core::ValueFloat *trackingFactor;

		rts2core::ValuePID *az_curr_PID;
		rts2core::ValuePID *az_slew_PID;
		rts2core::ValuePID *az_track_PID;
		rts2core::ValuePID *alt_curr_PID;
		rts2core::ValuePID *alt_slew_PID;
		rts2core::ValuePID *alt_track_PID;

		rts2core::ValueLong *az_enc;
		rts2core::ValueLong *alt_enc;

		rts2core::ValueInteger *extraBit;
		rts2core::ValueBool *autoModeAz;
		rts2core::ValueBool *autoModeAlt;
		rts2core::ValueLong *mclock;
		rts2core::ValueInteger *temperature;

		rts2core::ValueInteger *az_worm_phase;

		rts2core::ValueLong *az_last;
		rts2core::ValueLong *alt_last;

		rts2core::ValueString *az_errors;
		rts2core::ValueString *alt_errors;

		rts2core::ValueInteger *az_errors_val;
		rts2core::ValueInteger *alt_errors_val;

		rts2core::ValueDoubleStat *az_pos_error;
		rts2core::ValueDoubleStat *alt_pos_error;

		rts2core::ValueInteger *az_supply;
		rts2core::ValueInteger *alt_supply;

		rts2core::ValueInteger *az_temp;
		rts2core::ValueInteger *alt_temp;

		rts2core::ValueInteger *az_pid_out;
		rts2core::ValueInteger *alt_pid_out;

		// request values - speed,..
		rts2core::ValueDouble *az_speed;
		rts2core::ValueDouble *alt_speed;

		// tracking speed in controller units
		rts2core::ValueDouble *az_track_speed;
		rts2core::ValueDouble *alt_track_speed;

		rts2core::ValueLong *az_sitech_speed;
		rts2core::ValueLong *alt_sitech_speed;

		rts2core::ValueDoubleStat *az_sitech_speed_stat;
		rts2core::ValueDoubleStat *alt_sitech_speed_stat;

		rts2core::ValueDouble *az_acceleration;
		rts2core::ValueDouble *alt_acceleration;

		rts2core::ValueDouble *az_max_velocity;
		rts2core::ValueDouble *alt_max_velocity;

		rts2core::ValueDouble *az_current;
		rts2core::ValueDouble *alt_current;

		rts2core::ValueInteger *az_pwm;
		rts2core::ValueInteger *alt_pwm;

		rts2core::ValueInteger *countUp;
		rts2core::ValueDouble *PIDSampleRate;

		void getConfiguration ();

		int sitechMove (int32_t azc, int32_t altc);

		void telSetTarget (int32_t ac, int32_t dc);
		void derSetTarget ();

		bool firstSlewCall;
		bool wasStopped;

		// JD used in last getTel call
		double getTelUTC1;
		double getTelUTC2;

		/**
		 * Retrieve telescope counts, convert them to RA and Declination.
		 */
		void getTel ();
		void getTel (double &telra, double &teldec, double &un_telra, double &un_telzd);

		void getPIDs ();

		void changeSitechLogFile ();

		uint8_t xbits;
		uint8_t ybits;

		int az_last_errors;
		int alt_last_errors;

		long last_loop;
};

};

using namespace rts2teld;

/*!
 * Reads some data directly from tel_desc.
 *
 * Log all flow as LOG_DEBUG to syslog
 *
 * @exception EIO when there aren't data from tel_desc
 *
 * @param buf 		buffer to read in data
 * @param count 	how much data will be readed
 *
 * @return -1 on failure, otherwise number of read data
 */

ddm160::ddm160 (int argc, char **argv):AltAz (argc,argv, true, true, true, true, false)
{
	unlockPointing ();

#ifndef RTS2_LIBERFA
	setCorrections (true, true, true, true);
#endif
/*
	tel_tty = "/dev/ttyUSB0";
	telConn = NULL;
*/
	az_last_errors = 0;
	alt_last_errors = 0;
}


ddm160::~ddm160(void)
{
	/*
	delete telConn;
	telConn = NULL;

	delete sitechLogExpander;
	sitechLogExpander = NULL;
	*/
}

int ddm160::processOption (int in_opt)
{
	switch (in_opt)
	{
		case 'f':
			tel_tty = optarg;
			break;

		case OPT_SHOW_PID:
			createValue (az_curr_PID, "PID_AZ", "Azimuth current PID", false);
			createValue (az_slew_PID, "PID_az_slew", "AZ slew PID", false, RTS2_VALUE_WRITABLE);
			createValue (az_track_PID, "PID_az_track", "AZ tracking PID", false, RTS2_VALUE_WRITABLE);
			createValue (alt_curr_PID, "PID_ALT", "Altitude current PID", false);
			createValue (alt_slew_PID, "PID_alt_slew", "Alt slew PID", false, RTS2_VALUE_WRITABLE);
			createValue (alt_track_PID, "PID_alt_track", "Alt tracking PID", false, RTS2_VALUE_WRITABLE);

			updateMetaInformations (az_slew_PID);
			updateMetaInformations (az_track_PID);
			updateMetaInformations (alt_slew_PID);
			updateMetaInformations (alt_track_PID);
			break;

		default:
			return Telescope::processOption (in_opt);
	}
	return 0;
}

int ddm160::initHardware ()
{
	


        int ret;
        rts2core::Configuration *config;
        config = rts2core::Configuration::instance ();
        config->loadFile ();

        setTelLongLat (config->getObserver ()->lng, config->getObserver ()->lat);
        setTelAltitude (config->getObservatoryAltitude ());


	 MyMount = new asa::ASAMountModule_Remote("10.10.4.207",
	                                        2400,
	                                        "ASCII",
	                                        "mountlog");


           try
                {
                    MyMount->Connect();
                }
            catch (std::exception &e)
                {
                logStream(MESSAGE_INFO) << e.what()<<sendLog;
                //return;
                }
               logStream(MESSAGE_INFO) << "Connected to the mount " << MyMount->ConnectedToServer() << sendLog;



	getConfiguration ();

	return 0;
}

int ddm160::commandAuthorized (rts2core::Connection *conn)
{
	if (conn->isCommand ("zero_motor"))
	{

		return 0;
	}
	else if (conn->isCommand ("reset_errors"))
	{

		return 0;
	}
	else if (conn->isCommand ("reset_controller"))
	{

		getConfiguration ();
		return 0;
	}
	else if (conn->isCommand ("go_auto"))
	{
		getConfiguration ();
		return 0;
	}
	else if (conn->isCommand ("pids"))
	{

		return 0;
	}
        
        else if (conn->isCommand("testaltaz"))
        {
       try 
                {
                    MyMount->Connect();
                } 
            catch (std::exception &e) 
                {
                logStream(MESSAGE_INFO) << e.what()<<sendLog;
                //return;
                }
               logStream(MESSAGE_INFO) << "Connected to the mount " << MyMount->ConnectedToServer() << sendLog;
              // mountConnected=MyMount->ConnectedToServer();
		

        MyMount->SlewToAltAz(270, 45);  
        logStream(MESSAGE_INFO)<<"altitude is  "<<MyMount->Altitude()<<" Azimuth is "<<MyMount->Azimuth()<<sendLog;
       
         return 0;
       }


	return AltAz::commandAuthorized (conn);
}

int ddm160::info ()
{

try
{
                   // MyMount->RightAscensionTimestamp(timeStamp,raValue);
               raValue=    MyMount->RightAscension();
               decValue=    MyMount->Declination();
}//try


catch (std::exception &e)
                {
                logStream(MESSAGE_INFO) << "info get mount status problem "<<e.what()<<sendLog;
                //return;
                }



        
logStream(MESSAGE_INFO) << "Right Ascension is  "<<raValue<<" declination is "<<decValue<<sendLog;
         telFlip->setValueInteger (1);

        setTelRaDec(raValue,decValue);
        sayac=sayac+5;

        logStream(MESSAGE_INFO) << "RiGetTelRADEC test  "<<getTelRa ()<<" declination is "<<getTelDec () << " sayac is " << sayac<<sendLog;

//        return Telescope::info ();


	return AltAz::infoUTC (getTelUTC1, getTelUTC2);
}

int ddm160::startResync ()
{

       struct ln_equ_posn tar;
       getTarget (&tar);
       logStream(MESSAGE_INFO)<<" target ra is "<<tar.ra<<" target dec is "<<tar.dec<<sendLog;

 
 try
                {
                    MyMount->Connect();
                }
            catch (std::exception &e)
                {
                logStream(MESSAGE_INFO) << e.what()<<sendLog;
                //return;
                }
               logStream(MESSAGE_INFO) << "Connected to the mount " << MyMount->ConnectedToServer() << sendLog;


          try
{
        //MyMount->SlewToAltAz(270, 80);
        MyMount->SlewToCoordinates(tar.ra,tar.dec);
         MyMount->AbortTracking();
 
}

catch(std::exception &e)
{
 logStream(MESSAGE_INFO) << e.what()<<sendLog;

}

       logStream(MESSAGE_INFO)<<"altitude is  "<<MyMount->Altitude()<<" Azimuth is "<<MyMount->Azimuth()<<sendLog;




	return 0;

}

int ddm160::moveAltAz ()
{

        struct ln_hrz_posn hrz;
        telAltAz->getAltAz (&hrz);

 logStream(MESSAGE_INFO) << "horizontal coordinates alt is " <<hrz.alt <<"az is"<<hrz.az<< sendLog;

//struct ln_hrz_posn hrz;
//telAltAz->getAltAz (&hrz);

//	int32_t taz = r_az_pos->getValueLong ();
//	int32_t talt = r_alt_pos->getValueLong ();

//	bool flip;

//	int ret = hrz2counts (&hrz, taz, talt, 0, flip, false, 0, (getState () & TEL_PARKING) ? 1 : 0);
//	if (ret)
//		return ret;

//	telSetTarget (taz, talt);
         
        try
                {
                    MyMount->Connect();
                } 
            catch (std::exception &e)
                {
                logStream(MESSAGE_INFO) << e.what()<<sendLog;
                //return;
                }
               logStream(MESSAGE_INFO) << "Connected to the mount " << MyMount->ConnectedToServer() << sendLog;

  
        logStream(MESSAGE_INFO)<<"movealtaz calisti"<<sendLog;
        MyMount->SlewToAltAz(hrz.alt, hrz.az);
        logStream(MESSAGE_INFO)<<"altitude is  "<<MyMount->Altitude()<<" Azimuth is "<<MyMount->Azimuth()<<sendLog;

	return 0;
}

int ddm160::stopMove ()
{
      
logStream(MESSAGE_INFO)<< "Stop Move Command Sent "<<sendLog;

try
{
MyMount->TelStop();
}
catch(std::exception &e)
{
logStream(MESSAGE_INFO)<< "problem with stop move "<<e.what()<<sendLog;
}
	return 0;
}

int ddm160::isMoving ()
{
	return -2;
}

int ddm160::isParking ()
{


	return -2;
}


int ddm160::isOffseting ()
{

	return -2;
}

int ddm160::startPark ()
{
	
try{
 MyMount->Park();
}
catch(std::exception &e)
{
logStream(MESSAGE_INFO)<<"Problem while park"<<e.what()<<sendLog;
}

}

void ddm160::runTracking ()
{


//	return AltAz::setValue (oldValue, newValue);
}

void ddm160::changeIdleMovingTracking ()
{
	//changeSitechLogFile ();
}

void ddm160::scaleTrackingLook ()
{
}


void ddm160::internalTracking (double sec_step, float speed_factor)
{
}

void ddm160::updateTelTime ()
{
#ifdef RTS2_LIBERFA
	getEraUTC (getTelUTC1, getTelUTC2);
#else
	getTelUTC1 = ln_get_julian_from_sys ();
	getTelUTC2 = 0;
#endif
}

void ddm160::getConfiguration ()
{
}

int ddm160::sitechMove (int32_t azc, int32_t altc)
{
}

void ddm160::telSetTarget (int32_t ac, int32_t dc)
{


}

void ddm160::getTel ()
{

}

void ddm160::getTel (double &telaz, double &telalt, double &un_telaz, double &un_telzd)
{
	getTel ();

	//counts2hrz (telConn->last_status.y_pos, telConn->last_status.x_pos, telaz, telalt, un_telaz, un_telzd);
}

void ddm160::getPIDs ()
{
}

void ddm160::changeSitechLogFile ()
{
}




int main (int argc, char **argv)
{
	ddm160 device (argc, argv);
	return device.run ();
}
