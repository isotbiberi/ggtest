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

		virtual int setValue (rts2core::Value *oldValue, rts2core::Value *newValue);

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
                //=new asa::ASAMountModule_Remote("10.10.4.207",
                 //                             2400,
                  //                              "ASCII",
                   //                             "mountlog");

                  //MyMount = new asa::ASAMountModule_Remote("10.10.4.207",
                    //                            2400,
                      //                          "ASCII",
                        //                        "mountlog");


		bool mountConnected= false;


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
    /*
	createValue (sitechLogFile, "sitech_logfile", "SiTech logging file", false, RTS2_VALUE_WRITABLE);
	sitechLogFile->setValueString ("");
	sitechLogExpander = NULL;

	createValue (sitechVersion, "sitech_version", "SiTech controller firmware version", false);
	createValue (sitechSerial, "sitech_serial", "SiTech controller serial number", false);
    */
	createValue (r_az_pos, "R_AXAZ", "real AZ motor axis count", true);
	createValue (r_alt_pos, "R_AXALT", "real ALT motor axis count", true);

	createValue (t_az_pos, "T_AXAZ", "target AZ motor axis count", true, RTS2_VALUE_WRITABLE);
	createValue (t_alt_pos, "T_AXALT", "target ALT motor axis count", true, RTS2_VALUE_WRITABLE);

	createValue (trackingDist, "tracking_dist", "tracking error budget (bellow this value, telescope will start tracking", false, RTS2_VALUE_WRITABLE | RTS2_DT_DEG_DIST);

	createValue (trackingLook, "tracking_look", "[s] future position", false, RTS2_VALUE_WRITABLE | RTS2_DT_TIMEINTERVAL);
	createValue (userTrackingLook, "look_user", "user specified tracking lookahead", false, RTS2_VALUE_WRITABLE | RTS2_DT_ONOFF);

	trackingLook->setValueDouble (2);
	userTrackingLook->setValueBool (false);
	lastTrackingNum = 0;

	createValue (azErrPID, "az_err_PID", "PID parameters for AZ error correction", false, RTS2_VALUE_WRITABLE | RTS2_VALUE_AUTOSAVE);
	createValue (altErrPID, "alt_err_PID", "PID parameters for Alt error correction", false, RTS2_VALUE_WRITABLE | RTS2_VALUE_AUTOSAVE);

	azErrPID->setPID (0, 0.02, 0);
	altErrPID->setPID (0, 0.02, 0);

	// default to 1 arcsec
	trackingDist->setValueDouble (1 / 60.0 / 60.0);

	createValue (speedAngle, "speed_angle", "[deg] speed direction", false, RTS2_DT_DEG_DIST);
	createValue (errorAngle, "error_angle", "[deg] error direction", false, RTS2_DT_DEG_DIST);

	createValue (slowSyncDistance, "slow_track_distance", "distance for slow sync (at the end of movement, to catch with sky)", false, RTS2_VALUE_WRITABLE | RTS2_DT_DEG_DIST);
	slowSyncDistance->setValueDouble (0.1);  // 6 arcmin

	createValue (fastSyncSpeed, "fast_sync_speed", "fast speed factor (compared to siderial tracking) for fast alignment (above slow_track_distance)", false, RTS2_VALUE_WRITABLE);
	fastSyncSpeed->setValueFloat (4);

	createValue (trackingFactor, "tracking_factor", "tracking speed multiplier", false, RTS2_VALUE_WRITABLE);
	trackingFactor->setValueFloat (1);

	createValue (az_acceleration, "az_acceleration", "[deg/s^2] AZ motor acceleration", false);
	createValue (alt_acceleration, "alt_acceleration", "[deg/s^2] Alt motor acceleration", false);

	createValue (az_max_velocity, "az_max_v", "[deg/s] AZ axis maximal speed", false, RTS2_DT_DEGREES);
	createValue (alt_max_velocity, "alt_max_v", "[deg/s] ALT axis maximal speed", false, RTS2_DT_DEGREES);

	createValue (az_current, "az_current", "[A] AZ motor current", false);
	createValue (alt_current, "alt_current", "[A] ALT motor current", false);

	createValue (az_pwm, "az_pwm", "[W?] AZ motor PWM output", false);
	createValue (alt_pwm, "alt_pwm", "[W?] ALT motor PWM output", false);

	az_curr_PID = NULL;
	az_slew_PID = NULL;
	az_track_PID = NULL;
	alt_curr_PID = NULL;
	alt_slew_PID = NULL;
	alt_track_PID = NULL;

	createValue (az_enc, "ENCAZ", "AZ encoder readout", true);
	createValue (alt_enc, "ENCALT", "ALT encoder readout", true);

	createValue (extraBit, "extra_bits", "extra bits from axis status", false, RTS2_DT_HEX);
	createValue (autoModeAz, "auto_mode_az", "AZ axis auto mode", false, RTS2_DT_ONOFF | RTS2_VALUE_WRITABLE);
	createValue (autoModeAlt, "auto_mode_alt", "ALT axis auto mode", false, RTS2_DT_ONOFF | RTS2_VALUE_WRITABLE);
	createValue (mclock, "mclock", "millisecond board clocks", false);
	createValue (temperature, "temperature", "[C] board temperature (CPU)", false);
	createValue (az_worm_phase, "y_worm_phase", "AZ worm phase", false);

	createValue (az_last, "az_last", "AZ motor location at last AZ scope encoder location change", false);
	createValue (alt_last, "alt_last", "ALT motor location at last ALT scope encoder location change", false);

	createValue (az_errors, "az_errors", "AZ errors (only for FORCE ONE)", false);
	createValue (alt_errors, "alt_errors", "ALT errors (only for FORCE_ONE)", false);

	createValue (az_errors_val, "az_errors_val", "AZ errors value", false);
	createValue (alt_errors_val, "alt_erorrs_val", "ALT errors value", false);

	createValue (az_pos_error, "az_pos_error", "AZ positional error", false);
	createValue (alt_pos_error, "alt_pos_error", "ALT positional error", false);

	createValue (az_supply, "az_supply", "[V] AZ supply", false);
	createValue (alt_supply, "alt_supply", "[V] ALT supply", false);

	createValue (az_temp, "az_temp", "[F] AZ power board CPU temperature", false);
	createValue (alt_temp, "alt_temp", "[F] ALT power board CPU temperature", false);

	createValue (az_pid_out, "az_pid_out", "AZ PID output", false);
	createValue (alt_pid_out, "alt_pid_out", "ALT PID output", false);

	createValue (az_speed, "az_speed", "[deg/s] AZ speed (base rate), in counts per servo loop", false, RTS2_VALUE_WRITABLE | RTS2_DT_DEGREES);
	createValue (alt_speed, "alt_speed", "[deg/s] ALT speed (base rate), in counts per servo loop", false, RTS2_VALUE_WRITABLE | RTS2_DT_DEGREES);

	createValue (az_track_speed, "az_track_speed", "RA tracking speed (base rate), in counts per servo loop", false, RTS2_VALUE_WRITABLE);
	createValue (alt_track_speed, "alt_track_speed", "DEC tracking speed (base rate), in counts per servo loop", false, RTS2_VALUE_WRITABLE);

	createValue (az_sitech_speed, "az_sitech_speed", "speed in controller units", false);
	createValue (alt_sitech_speed, "alt_sitech_speed", "speed in controller units", false);

	createValue (az_sitech_speed_stat, "az_s_speed_stat", "sitech speed statistics", false);
	createValue (alt_sitech_speed_stat, "alt_s_speed_stat", "sitech speed statistics", false);

	firstSlewCall = true;
	wasStopped = false;

	last_loop = 0;

	addOption ('f', "telescope", 1, "telescope tty (ussualy /dev/ttyUSBx");
	addOption (OPT_SHOW_PID, "pid", 0, "allow PID read and edit");

	addParkPosOption ();
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
	
        struct ln_hrz_posn hrz;
        telAltAz->getAltAz (&hrz);

        int32_t taz = r_az_pos->getValueLong ();
        int32_t talt = r_alt_pos->getValueLong ();

        bool flip;

      //  int ret = hrz2counts (&hrz, taz, talt, 0, flip, false, 0, (getState () & TEL_PARKING) ? 1 : 0);
        
       logStream(MESSAGE_INFO)<<"taz is " <<taz<<" talt is "<<talt<<sendLog; 
     //  if (ret)
      //          return ret;

//      telSetTarget (taz, talt);








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
        //asa::ASAMountModule_Remote *MyMount=new asa::ASAMountModule_Remote("10.10.4.207",
                             //                   2400,
                              //                  "ASCII",
                               //                 "mountlog");


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
		


      /* 
        try
        {
         if (MyMount->MotorsStarted()) 
            {
            MyMount->StopMotors();
            logStream(MESSAGE_INFO)<<"Stopped Motors"<<sendLog;
             } 
         else 
             {
            MyMount->StartMotors();
             std::cout<<"Started Motors"<<std::endl;
            }
         
           logStream(MESSAGE_INFO)<<"Motor started status is "<<MyMount->MotorsStarted()<<sendLog;
         }   
          catch (std::exception &e)  
          {
           logStream(MESSAGE_INFO) << e.what()<<sendLog;
          }
        */
        MyMount->SlewToAltAz(270, 45);  
        logStream(MESSAGE_INFO)<<"altitude is  "<<MyMount->Altitude()<<" Azimuth is "<<MyMount->Azimuth()<<sendLog;
       
         return 0;
       }


	return AltAz::commandAuthorized (conn);
}

int ddm160::info ()
{
	struct ln_hrz_posn hrz;
	double un_az, un_zd;
	getTel (hrz.az, hrz.alt, un_az, un_zd);

	struct ln_equ_posn pos;

	getEquFromHrz (&hrz, getTelUTC1 + getTelUTC2, &pos);
	setTelRaDec (pos.ra, pos.dec);
	setTelUnAltAz (un_zd, un_az);

	return AltAz::infoUTC (getTelUTC1, getTelUTC2);
}

int ddm160::startResync ()
{
	getConfiguration ();

	return 0;

}

int ddm160::moveAltAz ()
{
	struct ln_hrz_posn hrz;
	telAltAz->getAltAz (&hrz);

	int32_t taz = r_az_pos->getValueLong ();
	int32_t talt = r_alt_pos->getValueLong ();

	bool flip;

	int ret = hrz2counts (&hrz, taz, talt, 0, flip, false, 0, (getState () & TEL_PARKING) ? 1 : 0);
	if (ret)
		return ret;

//	telSetTarget (taz, talt);
        logStream(MESSAGE_INFO)<<"movealtaz calisti"<<sendLog;

	return 0;
}

int ddm160::stopMove ()
{

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
	if (parkPos == NULL)
	{
		return 0;
	}
	setTargetAltAz (parkPos->getAlt (), parkPos->getAz ());
	wasStopped = false;
	return moveAltAz ();
}

void ddm160::runTracking ()
{
	if ((getState () & TEL_MASK_MOVING) != TEL_OBSERVING)
		return;
	scaleTrackingLook ();
	internalTracking (trackingLook->getValueFloat (), trackingFactor->getValueFloat ());
	AltAz::runTracking ();

	checkTracking (trackingDist->getValueDouble ());
}

int ddm160::setValue (rts2core::Value *oldValue, rts2core::Value *newValue)
{
	if (oldValue == t_az_pos)
	{
		telSetTarget (newValue->getValueLong (), t_alt_pos->getValueLong ());
		return 0;
	}
	else if (oldValue == t_alt_pos)
	{
		telSetTarget (t_az_pos->getValueLong (), newValue->getValueLong ());
		return 0;
	}
	else if (oldValue == autoModeAz)
	{
	//	telConn->siTechCommand ('Y', dynamic_cast <rts2core::ValueBool*> (newValue)->getValueBool () ? "A" : "M0");
		return 0;
	}
	else if (oldValue == autoModeAlt)
	{
	//	telConn->siTechCommand ('X', dynamic_cast <rts2core::ValueBool*> (newValue)->getValueBool () ? "A" : "M0");
		return 0;
	}
	else if (oldValue == trackingLook)
	{
		userTrackingLook->setValueBool (true);
		return 0;
	}
	else if (oldValue == sitechLogFile)
	{
		if (strlen (newValue->getValue ()) > 0)
		{
			/*
			delete sitechLogExpander;
			sitechLogExpander = new rts2core::Expander ();
			changeSitechLogFile ();
			*/
		}
		else
		{
			/*
			telConn->endLogging ();
			delete sitechLogExpander;
			sitechLogExpander = NULL;
			*/
		}
		return 0;
	}
	else if (oldValue == alt_slew_PID)
	{
		rts2core::ValuePID *newP = dynamic_cast <rts2core::ValuePID*> (newValue);
		/*
		telConn->setSiTechValue ('X', "PP", newP->getP ());
		telConn->setSiTechValue ('X', "II", newP->getI ());
		telConn->setSiTechValue ('X', "DD", newP->getD ());
		*/
	}
	else if (oldValue == alt_track_PID)
	{
		/*
		rts2core::ValuePID *newP = dynamic_cast <rts2core::ValuePID*> (newValue);
		telConn->setSiTechValue ('X', "P", newP->getP ());
		telConn->setSiTechValue ('X', "I", newP->getI ());
		telConn->setSiTechValue ('X', "D", newP->getD ());
		*/
	}
	else if (oldValue == az_slew_PID)
	{
		/*
		rts2core::ValuePID *newP = dynamic_cast <rts2core::ValuePID*> (newValue);
		telConn->setSiTechValue ('Y', "PP", newP->getP ());
		telConn->setSiTechValue ('Y', "II", newP->getI ());
		telConn->setSiTechValue ('Y', "DD", newP->getD ());
		*/
	}
	else if (oldValue == az_track_PID)
	{
		/*
		rts2core::ValuePID *newP = dynamic_cast <rts2core::ValuePID*> (newValue);
		telConn->setSiTechValue ('Y', "P", newP->getP ());
		telConn->setSiTechValue ('Y', "I", newP->getI ());
		telConn->setSiTechValue ('Y', "D", newP->getD ());
		*/
	}



	return AltAz::setValue (oldValue, newValue);
}

void ddm160::changeIdleMovingTracking ()
{
	changeSitechLogFile ();
}

void ddm160::scaleTrackingLook ()
{
	// change tracking lookahead
	if (userTrackingLook->getValueBool () == false && abs (trackingNum - lastTrackingNum) > 3)
	{
		float change = 0;

		// scale trackingLook as needed
		if (speedAngle->getStdev () > 2)
		{
			if (trackingLook->getValueFloat () < 1)
				change = 0.1;
			else if (trackingLook->getValueFloat () < 5)
				change = 0.5;
			else
				change = 2;
		}
		else if (trackingLook->getValueFloat () > 1)
		{
			if (trackingLook->getValueFloat () > 5)
				change = -2;
			else if (trackingLook->getValueFloat () > 1)
				change = -0.5;
			else
				change = -0.1;
		}

		if (change != 0)
		{
			trackingLook->setValueFloat (trackingLook->getValueFloat () + change);
			if (trackingLook->getValueFloat () > 15)
				trackingLook->setValueFloat (15);
			else if (trackingLook->getValueFloat () < 0.5)
				trackingLook->setValueFloat (0.5);
		}
		sendValueAll (trackingLook);
		lastTrackingNum = trackingNum;
	}
}


void ddm160::internalTracking (double sec_step, float speed_factor)
{
	info ();

	int32_t a_azc = r_az_pos->getValueLong ();
	int32_t a_altc = r_alt_pos->getValueLong ();

	int32_t azc_speed = 0;
	int32_t altc_speed = 0;

	int32_t aze_speed = 0;
	int32_t alte_speed = 0;

	double speed_angle = 0;
	double error_angle = 0;

	int ret = calculateTracking (getTelUTC1, getTelUTC2, sec_step, a_azc, a_altc, azc_speed, altc_speed, aze_speed, alte_speed, speed_angle, error_angle);
	if (ret)
	{
		if (ret < 0)
			logStream (MESSAGE_WARNING) << "cannot calculate next tracking, aborting tracking" << sendLog;
		stopTracking ();
		return;
	}

	int32_t az_change = 0;
	int32_t alt_change = 0;

	az_change = a_azc - r_az_pos->getValueLong ();
	alt_change = a_altc - r_alt_pos->getValueLong ();

	a_azc += az_change * 300;
	a_altc += alt_change * 300;

	if (a_azc > azMax->getValueLong ())
		a_azc = azMax->getValueLong ();
	if (a_azc < azMin->getValueLong ())
		a_azc = azMin->getValueLong ();

	if (a_altc > altMax->getValueLong ())
		a_altc = altMax->getValueLong ();
	if (a_altc < altMin->getValueLong ())
		a_altc = altMin->getValueLong ();

	double loop_sec = (mclock->getValueLong () - last_loop) / 1000.0;

	if (loop_sec < 1)
	{
		if (abs (aze_speed) > 25 || !isTracking ())
		{
			int32_t err_sp = azErrPID->loop (aze_speed, loop_sec);
			if (isTracking ())
			{
				int32_t err_cap = abs(azc_speed * 0.015);
				if (err_sp > err_cap)
					err_sp = err_cap;
				else if (err_sp < -err_cap)
					err_sp = -err_cap;
			}

			azc_speed += err_sp;
		}

		if (abs (alte_speed) > 25 || !isTracking ())
		{
			int32_t err_sp = altErrPID->loop (alte_speed, loop_sec);
			if (isTracking ())
			{
				int32_t err_cap = abs(altc_speed * 0.015);
				if (err_sp > err_cap)
					err_sp = err_cap;
				else if (err_sp < -err_cap)
					err_sp = -err_cap;
			}

			altc_speed += err_sp;
		}
	}

 last_loop = mclock->getValueLong ();


	//altaz_Xrequest.y_speed = labs (telConn->ticksPerSec2MotorSpeed (azc_speed * speed_factor));
	//altaz_Xrequest.x_speed = labs (telConn->ticksPerSec2MotorSpeed (altc_speed * speed_factor));

	//altaz_Xrequest.y_dest = a_azc;
	//altaz_Xrequest.x_dest = a_altc;

//	az_sitech_speed->setValueLong (altaz_Xrequest.y_speed);
//	alt_sitech_speed->setValueLong (altaz_Xrequest.x_speed);

//	az_sitech_speed_stat->addValue (altaz_Xrequest.y_speed, 15);
//	alt_sitech_speed_stat->addValue (altaz_Xrequest.x_speed, 15);

//	az_sitech_speed_stat->calculate ();
//	alt_sitech_speed_stat->calculate ();

//	altaz_Xrequest.x_bits = xbits;
//	altaz_Xrequest.y_bits = ybits;

//	xbits |= (0x01 << 4);


 /*
	// check that the entered trajactory is valid
	ret = checkTrajectory (getTelUTC1 + getTelUTC2, r_az_pos->getValueLong (), r_alt_pos->getValueLong (), altaz_Xrequest.y_dest, altaz_Xrequest.x_dest, az_change / sec_step / 2.0, alt_change / sec_step / 2.0, TRAJECTORY_CHECK_LIMIT, 2.0, 2.0, false);
	if (ret == 2 && speed_factor > 1) // too big move to future, keep target
	{
		logStream (MESSAGE_INFO) << "soft stop detected while running tracking, move from " << r_az_pos->getValueLong () << " " << r_alt_pos->getValueLong () << " only to " << altaz_Xrequest.y_dest << " " << altaz_Xrequest.x_dest << sendLog;
	}
	else if (ret != 0)
	{
		logStream (MESSAGE_WARNING) << "trajectory from " << r_az_pos->getValueLong () << " " << r_alt_pos->getValueLong () << " to " << altaz_Xrequest.y_dest << " " << altaz_Xrequest.x_dest << " will hit (" << ret << "), stopping tracking" << sendLog;
		stopTracking ();
		return;
	}

	t_az_pos->setValueLong (altaz_Xrequest.y_dest);
	t_alt_pos->setValueLong (altaz_Xrequest.x_dest);

	speedAngle->addValue (speed_angle, 15);
	errorAngle->addValue (error_angle, 15);

	speedAngle->calculate ();
	errorAngle->calculate ();

*/
/*
	try
	{

		if (telConn == NULL)
		{
			telConn = new rts2core::ConnSitech (tel_tty, this);
			telConn->setDebug (getDebug ());
			telConn->init ();

			telConn->flushPortIO ();
			telConn->getSiTechValue ('Y', "XY");
			getConfiguration ();
		}

		telConn->sendXAxisRequest (altaz_Xrequest);
		updateTelTime ();
		updateTrackingFrequency ();
	}
	catch (rts2core::Error &e)
	{
		delete telConn;
		telConn = NULL;

		logStream (MESSAGE_ERROR) << "closign connection to serial port" << sendLog;
		usleep (USEC_SEC / 15);
	}
	*/
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
	/*

//	if (telConn == NULL)
//		return;
	az_acceleration->setValueDouble (telConn->getSiTechValue ('Y', "R"));
	alt_acceleration->setValueDouble (telConn->getSiTechValue ('X', "R"));

	az_max_velocity->setValueDouble (telConn->motorSpeed2DegsPerSec (telConn->getSiTechValue ('Y', "S"), az_ticks->getValueLong ()));
	alt_max_velocity->setValueDouble (telConn->motorSpeed2DegsPerSec (telConn->getSiTechValue ('X', "S"), alt_ticks->getValueLong ()));

	az_current->setValueDouble (telConn->getSiTechValue ('Y', "C") / 100.0);
	alt_current->setValueDouble (telConn->getSiTechValue ('X', "C") / 100.0);

	az_pwm->setValueInteger (telConn->getSiTechValue ('Y', "O"));
	alt_pwm->setValueInteger (telConn->getSiTechValue ('X', "O"));
*/
}

int ddm160::sitechMove (int32_t azc, int32_t altc)
{
	logStream (MESSAGE_DEBUG) << "sitechMove " << r_az_pos->getValueLong () << " " << azc << " " << r_alt_pos->getValueLong () << " " << altc << " " << sendLog;

	double JD = ln_get_julian_from_sys ();

	int ret = calculateMove (JD, r_az_pos->getValueLong (), r_alt_pos->getValueLong (), azc, altc);
	if (ret < 0)
		return ret;

	telSetTarget (azc, altc);

	return ret;
}

void ddm160::telSetTarget (int32_t ac, int32_t dc)
{
	
	altaz_Xrequest.y_dest = ac;
	altaz_Xrequest.x_dest = dc;
      /*
	altaz_Xrequest.y_speed = labs (telConn->degsPerSec2MotorSpeed (az_speed->getValueDouble (), az_ticks->getValueLong (), 360) * SPEED_MULTI);
	altaz_Xrequest.x_speed = labs (telConn->degsPerSec2MotorSpeed (alt_speed->getValueDouble (), alt_ticks->getValueLong (), 360) * SPEED_MULTI);

	// clear bit 4, tracking
	xbits &= ~(0x01 << 4);

	altaz_Xrequest.x_bits = xbits;
	altaz_Xrequest.y_bits = ybits;

	telConn->sendXAxisRequest (altaz_Xrequest);


*/
	t_az_pos->setValueLong (ac);
	t_alt_pos->setValueLong (dc);

}

void ddm160::getTel ()
{
	/*
	// update data only if telescope is not tracking - if it is tracking, commands to set target will return last_status
	if (!isTracking ())
	{
		try
		{
			if (telConn == NULL)
			{
				telConn = new rts2core::ConnSitech (tel_tty, this);
				telConn->setDebug (getDebug ());
				telConn->init ();

				telConn->flushPortIO ();
				telConn->getSiTechValue ('Y', "XY");
				getConfiguration ();
			}
			telConn->getAxisStatus ('X');
			updateTelTime ();
		} catch (rts2core::Error &e)
		{
			delete telConn;
			telConn = NULL;

			logStream (MESSAGE_ERROR) << "closign connection to serial port" << sendLog;
			usleep (USEC_SEC / 15);
		}
	}

	az_enc->setValueLong (telConn->last_status.y_enc);
	alt_enc->setValueLong (telConn->last_status.x_enc);

	extraBit->setValueInteger (telConn->last_status.extra_bits);
	// not stopped, not in manual mode
	autoModeAz->setValueBool ((telConn->last_status.extra_bits & AUTO_Y) == 0);
	autoModeAlt->setValueBool ((telConn->last_status.extra_bits & AUTO_X) == 0);
	mclock->setValueLong (telConn->last_status.mclock);
	temperature->setValueInteger (telConn->last_status.temperature);

	az_worm_phase->setValueInteger (telConn->last_status.y_worm_phase);

	switch (telConn->sitechType)
	{
		case rts2core::ConnSitech::SERVO_I:
		case rts2core::ConnSitech::SERVO_II:
			r_az_pos->setValueLong (telConn->last_status.y_pos);
			r_alt_pos->setValueLong (telConn->last_status.x_pos);

			az_last->setValueLong (le32toh (*(uint32_t*) &(telConn->last_status.y_last)));
			alt_last->setValueLong (le32toh (*(uint32_t*) &(telConn->last_status.x_last)));
			break;
		case rts2core::ConnSitech::FORCE_ONE:
		{
			// upper nimble
			uint16_t az_val = telConn->last_status.y_last[0] << 4;
			az_val += telConn->last_status.y_last[1];

			uint16_t alt_val = telConn->last_status.x_last[0] << 4;
			alt_val += telConn->last_status.x_last[1];

			// find all possible errors
			switch (telConn->last_status.y_last[0] & 0x0F)
			{
				case 0:
					az_errors_val->setValueInteger (az_val);
					az_errors->setValueString (telConn->findErrors (az_val));
					if (az_last_errors != az_val)
					{
						if (az_val == 0)
						{
							clearHWError ();
							logStream (MESSAGE_REPORTIT | MESSAGE_INFO) << "azimuth axis controller error values cleared" << sendLog;
						}
						else
						{
							raiseHWError ();
							logStream (MESSAGE_ERROR) << "azimuth axis controller error(s): " << az_errors->getValue () << sendLog;
						}
						az_last_errors = az_val;
					}
					// stop if on limits
					if ((az_val & ERROR_LIMIT_MINUS) || (az_val & ERROR_LIMIT_PLUS))
						stopTracking ();
					break;

				case 1:
					az_current->setValueInteger (az_val);
					break;

				case 2:
					az_supply->setValueInteger (az_val);
					break;

				case 3:
					az_temp->setValueInteger (az_val);
					break;

				case 4:
					az_pid_out->setValueInteger (az_val);
					break;
			}


			switch (telConn->last_status.x_last[0] & 0x0F)
			{
				case 0:
					alt_errors_val->setValueInteger (alt_val);
					alt_errors->setValueString (telConn->findErrors (alt_val));
					if (alt_last_errors != alt_val)
					{
						if (alt_val == 0)
						{
							clearHWError ();
							logStream (MESSAGE_REPORTIT | MESSAGE_INFO) << "altitude axis controller error values cleared" << sendLog;
						}
						else
						{
							raiseHWError ();
							logStream (MESSAGE_ERROR) << "altitude axis controller error(s): " << alt_errors->getValue () << sendLog;
						}
						alt_last_errors = alt_val;
					}
					// stop if on limits
					if ((alt_val & ERROR_LIMIT_MINUS) || (alt_val & ERROR_LIMIT_PLUS))
						stopTracking ();
					break;

				case 1:
					alt_current->setValueInteger (alt_val);
					break;

				case 2:
					alt_supply->setValueInteger (alt_val);
					break;

				case 3:
					alt_temp->setValueInteger (alt_val);
					break;

				case 4:
					alt_pid_out->setValueInteger (alt_val);
					break;
			}

			int16_t az_err = *(int16_t*) &(telConn->last_status.y_last[2]);
			int16_t alt_err = *(int16_t*) &(telConn->last_status.x_last[2]);

			if (abs (az_err) <= 7)
				r_az_pos->setValueLong (telConn->last_status.y_pos);
			else
				r_az_pos->setValueLong (telConn->last_status.y_pos - az_err);

			if (abs (alt_err) <= 7)
				r_alt_pos->setValueLong (telConn->last_status.x_pos);
			else
				r_alt_pos->setValueLong (telConn->last_status.x_pos - alt_err);

			az_pos_error->addValue (az_err, 20);
			alt_pos_error->addValue (alt_err, 20);

			r_az_pos->setValueLong (telConn->last_status.y_pos);
			r_alt_pos->setValueLong (telConn->last_status.x_pos);

			az_pos_error->calculate ();
			alt_pos_error->calculate ();
			break;
		}
	}
	*/
}

void ddm160::getTel (double &telaz, double &telalt, double &un_telaz, double &un_telzd)
{
	getTel ();

	//counts2hrz (telConn->last_status.y_pos, telConn->last_status.x_pos, telaz, telalt, un_telaz, un_telzd);
}

void ddm160::getPIDs ()
{
	/*
	alt_curr_PID->setPID (telConn->getSiTechValue ('X', "PPP"), telConn->getSiTechValue ('X', "III"), telConn->getSiTechValue ('X', "DDD"));
	if (alt_slew_PID)
	{
		alt_slew_PID->setPID (telConn->getSiTechValue ('X', "PP"), telConn->getSiTechValue ('X', "II"), telConn->getSiTechValue ('X', "DD"));
	}
	if (alt_track_PID)
	{
		alt_track_PID->setPID (telConn->getSiTechValue ('X', "P"), telConn->getSiTechValue ('X', "I"), telConn->getSiTechValue ('X', "D"));
	}

	az_curr_PID->setPID (telConn->getSiTechValue ('Y', "PPP"), telConn->getSiTechValue ('Y', "III"), telConn->getSiTechValue ('Y', "DDD"));
	if (az_slew_PID)
	{
		az_slew_PID->setPID (telConn->getSiTechValue ('Y', "PP"), telConn->getSiTechValue ('Y', "II"), telConn->getSiTechValue ('Y', "DD"));
	}
	if (az_track_PID)
	{
		az_track_PID->setPID (telConn->getSiTechValue ('Y', "P"), telConn->getSiTechValue ('Y', "I"), telConn->getSiTechValue ('Y', "D"));
	}
	*/
}

void ddm160::changeSitechLogFile ()
{
	/*
	try
	{
		if (sitechLogExpander)
		{
			sitechLogExpander->setExpandDate ();
			telConn->startLogging (sitechLogExpander->expandPath (sitechLogFile->getValue ()).c_str ());
		}
	}
	catch (rts2core::Error er)
	{
		logStream (MESSAGE_WARNING) << "cannot expand " << sitechLogFile->getValue () << sendLog;
	}
	*/
}




int main (int argc, char **argv)
{
	ddm160 device (argc, argv);
	return device.run ();
}
