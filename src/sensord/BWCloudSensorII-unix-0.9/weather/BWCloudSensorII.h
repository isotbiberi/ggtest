/*
    Copyright (c) 2007,2008 MyTelescope.com
    All rights reserved.

    This file is part of the BWCloudSensorII-unix package.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this package; if not, write to the
    Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA  02111-1307  USA.
 */
#ifndef BW_CLOUD_SENSOR_II_H
# define BW_CLOUD_SENSOR_II_H

# include <math.h>

/*
 * Interface to the CloudSensorII.
 *
 * There are two main classes:
 *     BWCloudSensorII - the driver; methods intended to be called by
 *         the application,
 * and BWCSII_Listener - passed to the driver so it can notify the application
 *         of certain events.  The application must implement the methods
 *         for the events it is interested (the default implementations do
 *         nothing).
 * The remaining classes/structures hold data.
 */

#include <time.h>
#include <inttypes.h>
#include <sys/time.h>

#include <iostream>
#include <fstream>
#include <string>
#include <deque>

#include "common/SerialConnection.h"

class BWCloudSensorII;
class BWCSII_Listener;
struct BWCSII_report;
struct BWCSII_reportReset;
struct BWCSII_reportThresholds;
struct BWCSII_reportWetnessCalibration;
struct BWCSII_reportWetnessData;
struct BWCSII_reportThermopileCalibration;
struct BWCSII_reportSensorData;
struct BWCSII_setThresholds;

/*
 * Structures for the various reports that are received from the CloudSensorII.
 */

struct BWCSII_report
{
    /* One value for each type of report (derived classes). */
    enum ReportType {
	RT__UNINITIALIZED = 0,
	RT_RESET,
	RT_THRESHOLDS,
	RT_WETNESS_CALIBRATION,
	RT_WETNESS_DATA,
	RT_THERMOPILE_CALIBRATION,
	RT_SENSOR_DATA,
    };
    ReportType reportType;
    time_t reportTime; /* When report was received. */

    BWCSII_report(ReportType reportType);
    virtual ~BWCSII_report();

    /* Debugging: print report to stream. */
    virtual void printState(std::ostream &os, int indent, int indentIncr) const;

    /* Parse the message part of a report frame from the csII
     * (returns false on failure).
     */
    virtual bool parseReport(const char *callersName, const struct timeval &tv,
	    const char *p) = 0;

    /* Convert the report to the standard message format
     * (not currently used).
     */
    virtual void reportToString(char *buf, size_t bufSize) const = 0;

    /* Get a header line that can be printed above the reportToString()
     * result.
     */
    virtual const char *reportHeader() const = 0;

    static const char *getReportTypeName(ReportType val);
};

struct BWCSII_reportReset : public BWCSII_report /* I report */
{
    unsigned int serialNumber;
    unsigned int version;
    unsigned int mcucsrv;
    unsigned int crashCode;
    unsigned int lastCyc;
    unsigned int eSendErrs;

    BWCSII_reportReset(); /* clears fields. */
    virtual ~BWCSII_reportReset();
    virtual void printState(std::ostream &os, int indent, int indentIncr) const;
    virtual bool parseReport(const char *callersName, const struct timeval &tv,
	    const char *p);
    virtual void reportToString(char *buf, size_t bufSize) const;
    virtual const char *reportHeader() const;
};

struct BWCSII_reportThresholds : public BWCSII_report /* T report */
{
    unsigned int serialNumber;
    unsigned int version;
    unsigned int eSendErrs;
    double eCloudyThresh;
    double eVeryCloudyThresh;
    double eWindyThresh;
    double eVeryWindyThresh;
    int eRainThresh;
    int eWetThresh;
    int eDaylightCode;
    int eDayThresh;
    int eVeryDayThresh;
    int unknown1;
    int unknown2;
    int unknown3;
    int unknown4;
    int unknown5;

    BWCSII_reportThresholds(); /* clears fields. */
    virtual ~BWCSII_reportThresholds();
    virtual void printState(std::ostream &os, int indent, int indentIncr) const;
    virtual bool parseReport(const char *callersName, const struct timeval &tv,
	    const char *p);
    virtual void reportToString(char *buf, size_t bufSize) const;
    virtual const char *reportHeader() const;
};

struct BWCSII_reportWetnessCalibration : public BWCSII_report /* K report */
{
    int eWetCal;
    double eWetOscFactor;
    unsigned int eRawWetAvg;
    double eCaseT;
    double eshtAmbientT;
    unsigned int enomOsc;
    int oscDry;
    int minWetAvg;
    int dif;
    char unknown1;

    BWCSII_reportWetnessCalibration(); /* clears fields. */
    virtual ~BWCSII_reportWetnessCalibration();
    virtual void printState(std::ostream &os, int indent, int indentIncr) const;
    virtual bool parseReport(const char *callersName, const struct timeval &tv,
	    const char *p);
    virtual void reportToString(char *buf, size_t bufSize) const;
    virtual const char *reportHeader() const;
};

struct BWCSII_reportWetnessData : public BWCSII_report /* W report */
{
    double caseVal;
    double ambT;
    unsigned int wAvgW;
    unsigned int wAvgC;
    unsigned int nomos;
    unsigned int rawWT;
    unsigned int wetAvg;

    BWCSII_reportWetnessData(); /* clears fields. */
    virtual ~BWCSII_reportWetnessData();
    virtual void printState(std::ostream &os, int indent, int indentIncr) const;
    virtual bool parseReport(const char *callersName, const struct timeval &tv,
	    const char *p);
    virtual void reportToString(char *buf, size_t bufSize) const;
    virtual const char *reportHeader() const;
};

struct BWCSII_reportThermopileCalibration : public BWCSII_report /* C report */
{
    int eThermopileCal;
    double eBestK;
    double eBestD;
    double eBestOffs;

    BWCSII_reportThermopileCalibration(); /* clears fields. */
    virtual ~BWCSII_reportThermopileCalibration();
    virtual void printState(std::ostream &os, int indent, int indentIncr) const;
    virtual bool parseReport(const char *callersName, const struct timeval &tv,
	    const char *p);
    virtual void reportToString(char *buf, size_t bufSize) const;
    virtual const char *reportHeader() const;
};

struct BWCSII_reportSensorData : public BWCSII_report /* D report */
{
    /* 1: humidstatTempCode: humidity and ambient temperature sensor
     *   0 = OK.
     *   1 = write failure for humidity.
     *   2 = measurement never finished for humidity.
     *   3 = write failure for ambient.
     *   4 = measurement never finished for ambient.
     *   5 = data line was not high for humidity,
     *   6 = data line was not high for ambient.
     */
    unsigned int humidstatTempCode;
    /* 2: cloudCond (dT = skyMinusAmbientTemperature):
     *   0 = unknown (e.g., sensor wet, still heating up, etc; see field 9)
     *   1 = clear (dt > eCloudyThresh)
     *   2 = cloudy (dT <= eCloudyThresh)
     *   3 = very cloudy (dT <= eVeryCloudyThresh)
     */
    unsigned int cloudCond;
    /* 3: windCond:
     *   0 = unknown (e.g., sensor wet)
     *   1 = ok (windSpeed < eWindyThresh)
     *   2 = windy (windSpeed >= eWindyThresh)
     *   3 = very windy (windSpeed >= eVeryWindyThresh)
     */
    unsigned int windCond;
    /* 4: rainCond:
     *   0 = unknown
     *   1 = not raining
     *   2 = recently raining
     *   3 = raining
     */
    unsigned int rainCond;
    /* 5: skyCond (for cloud Sensor I programs)
     *   0 unknown
     *   1 clear
     *   2 cloudy
     *   3 very cloudy
     *   4 wet
     */
    unsigned int skyCond;
    /* 6: roofCloseRequested: 0 normally, 1 if roof close
     * was requested on this cycle
     */
    unsigned int roofCloseRequested;
    /* 7: sky-ambient: degrees C; 999.9 saturated hot, -999.9 saturated cold;
     * -998.0 if sensor is wet.
     */
    double skyMinusAmbientTemperature;
    /* 8: ambientTemperature: degrees C; if -40 and humidity is 0
     * there is a problem with communication to those sensors
     * (likely water where it shouldn't be).
     */
    double ambientTemperature;
    /* 9: wind speed in km/hr, or:
     *    -1. if still heating up,
     *    -2. if wet,
     *    -3. if the A/D from the wind probe is bad,
     *    -4. if the probe is not heating.
     */
    double windSpeed;
    /* 10: wetSensor: 'N' when dry, 'W' when wet now, 'w' wet in
     * last minute.
     */
    char wetSensor;
    /* 11: rainSensor: 'N' when no rain, 'R' when rain drops hit
     * on this cycle, 'r' for drops in last minute.
     */
    char rainSensor;
    /* 12: relative humidity in % (range 0..100); see comment
     * about ambientTemperature.
     */
    int relativeHumidityPercentage;
    /* 13: dewPointTemperature: in degrees C */
    double dewPointTemperature;
    /* 14: thermopile case temperature, 999.9 saturated hot,
     * -999.9 saturated cold.
     */
    double caseTemperature;
    /* 15: */
    int rainHeaterPercentage;
    /* 16: calibration black body temperature (factory only),
     * 999.9 saturated hot, -99.9 saturated cold.
     */
    double blackBodyTemperature;
    /* 17: rainHeaterState: 
     *   0 if too hot,
     *   1 if at or nearly at requested temp.,
     *   2..5 if too cold,
     *   6 if cannot control due to a saturated case temperature
     *     (causes shutdown).
     *   7 is used by firmware (tmain) to indicate that normal control
     *     is being used instead of direct use of this.
     */
    int rainHeaterState;
    /* 18: voltage actually on the +24V at the sensor head */
    double powerVoltage;
    /* 19: anemometer tip temperature difference from ambient,
     * limited by reducing anemometer heater power when 25C
     * is reached.
     */
    double anemometerTemeratureDiff;
    /* 20: maximum drop in wetness oscillator counts this cycle
     * due to rain drops
     */
    int wetnessDrop;
    /* 21: wetness oscillator count difference from base dry value.  */
    int wetnessAvg;
    /* 22: wetness oscillator count difference for current dry from
     * base dry value
     */
    int wetnessDry;
    /* 23: rain heater PWM value */
    int rainHeaterPWM;
    /* 24: anemometer heater PWM value */
    int anemometerHeaterPWM;
    /* 25: thermopile raw A/D output */
    unsigned int thermopileADC;
    /* 26: thermopile thermistor raw A/D output */
    unsigned int thermistorADC;
    /* 27: power supply voltage monitor raw A/D output */
    unsigned int powerADC;
    /* 28: calibration block thermistor raw A/D output */
    unsigned int blockADC;
    /* 29: anemometer tip thermistor raw A/D output */
    unsigned int anemometerThermistorADC;
    /* 30: Davis vane raw A/D output (only for factory calibration).  */
    unsigned int davisVaneADC;
    /* 31: external anemometer used (only for factory calibration) */
    double dkMPH;
    /* 32: external anemometer wind direction (only for factory calibration) */
    int extAnemometerDirection;
    /* 33: raw counts from the wetness oscillator */
    int rawWetnessOsc;
    /* 34: DayCond value:
     *   3 full daylight
     *   2 twilight
     *   1 night
     *   0 unknown
     */
    unsigned int dayCond;
    /* 35: daylight photodiode raw A/D output
     * (0 means no light, 1023 max light).
     */
    unsigned int daylightADC;

    /*
     * Added fields / values.
     */
    enum {
	/* How long is "recent" - when can the skyMinusAmbientTemperature
	 * value be truested after being wet?
	 */
	RECENTLY_WET_SECS = 2 * 60,
    };
    /* Was the skyMinusAmbientTemperature value recently reporting wet?
     * Notable as readings just after it reports wet are suspect.
     */
    bool recentlyWet;

    BWCSII_reportSensorData(); /* clears fields. */
    virtual ~BWCSII_reportSensorData();
    virtual void printState(std::ostream &os, int indent, int indentIncr) const;
    virtual bool parseReport(const char *callersName, const struct timeval &tv,
	    const char *p);
    virtual void reportToString(char *buf, size_t bufSize) const;
    virtual const char *reportHeader() const;

    /* Helper routines. */
    /* Is ambientTemperature field valid?  See comment for ambientTemperature.
     */
    bool validAmbientTemperature() const {
	return humidstatTempCode == 0
	    /* work around (firmware 00050): code can be 0, but values
	     * not valid.
	     */
	    && (relativeHumidityPercentage != 0
		|| fabs(ambientTemperature - -40) > 0.01);
    }
    bool validRelativeHumidityPercentage() const {
	/* humidity and ambient temperature are part of same sensor unit. */
	return validAmbientTemperature();
    }
    /* Note: counts -999 as "valid" - sky is clear. */
    bool validSkyMinusAmbientTemperature() const {
	       /* uses ambient temperature, so it must be valid. */
	return validAmbientTemperature()
	    && (cloudCond != 0
		|| fabs(skyMinusAmbientTemperature - -999) < 0.01)
	    /* Don't trust result if sky is warmer than ambient... */
	    && skyMinusAmbientTemperature < 0
	    && !recentlyWet;
    }
    /* Is sensor saying it is wet?  See comments for recentlyWet flag.  */
    bool wetSkyMinusAmbientTemperature() const {
	return fabs(skyMinusAmbientTemperature - -998) < 0.01;
    }
    bool validWindSpeed() const {
	/* wind sensor uses ambient temperature, so it must be valid. */
	return windCond != 0 && windSpeed >= 0 && validAmbientTemperature();
    }
};

/* Information sent to the csII when configuring it. */
struct BWCSII_setThresholds
{
    /* relative sky temperature threshold in deg.C (1 significant digit). */
    double cloudyThresh;
    /* relative sky temperature threshold in deg.C (1 significant digit). */
    double veryCloudyThresh;
    /* too windy threshold in km/hr (1 significant digit). */
    double windyThresh;
    /* minimum drop of oscillator count in 16.384 msec required to
     * declare a rain drop.
     */
    int rainThresh;
    /* minimum drop of oscillator count in 16.384 msec from dry required
     * to declare the wetness sensor as wet.
     */
    int wetThresh;

    BWCSII_setThresholds(); /* clears fields. */
    void printState(std::ostream &os, int indent, int indentIncr) const;
};

/*
 * Class to be implemented by the application so it can
 * be notified of certain events.
 */
class BWCSII_Listener
{
  public:
    /* See csII_cmdOver(). */
    enum CmdType {
	CT__UNINITIALIZED,
	CT_GET_CALIBRATION, /* c */
	CT_GET_WETNESS_CALIBRATION, /* l */
	CT_SET_THRESHOLDS, /* t (values) */
	CT_SET_THRESHOLDS_DEFAULT, /* t (default) */
	CT_ROOF_CLOSE, /* b */
	CT_RAW, /* ... */
    };
    virtual ~BWCSII_Listener();
    /* See csII_cmdOver(). */
    enum CmdOverReason {
	COR_SUCCESS, /* Command successfully received by CSII */
	COR_TIMED_OUT, /* Timed out trying to send command to CSII */
	/* Command was purged (maybe at CSII's request or by the
	 * BWCloudSensorII's initiative).
	 */
	COR_PURGED,
    };

    /*
     * Default methods do nothing.
     */

    /* reasonChar is the code from the CSII (possible values not documented). */
    virtual void csII_purgeEvent(BWCloudSensorII *csII,
	    unsigned char reasonChar);
    /* Called when a memory dump is received from the CSII. */
    virtual void csII_memoryDump(BWCloudSensorII *csII, const uint8_t *memory,
	    unsigned int nBytes);

    /* Called when a requested command has either been successfully sent to 
     * the CloudSensorII, has failed (timed out), or has been purged;
     * "reason" indicates which of these is the case.
     */
    virtual void csII_cmdOver(BWCloudSensorII *csII, CmdType cmdType,
	    CmdOverReason reason);
    /* Called when a poll is received; generally not used by applications.  */
    virtual void csII_polled(BWCloudSensorII *csII);

    /*
     * These are called when the associated report is received.
     */
    virtual void csII_reportReset(BWCloudSensorII *csII,
	    const BWCSII_reportReset &report);
    virtual void csII_reportThresholds(BWCloudSensorII *csII,
	const BWCSII_reportThresholds &report);
    virtual void csII_reportWetnessCalibration(BWCloudSensorII *csII,
	const BWCSII_reportWetnessCalibration &report);
    virtual void csII_reportWetnessData(BWCloudSensorII *csII,
	const BWCSII_reportWetnessData &report);
    virtual void csII_reportThermopileCalibration(BWCloudSensorII *csII,
	const BWCSII_reportThermopileCalibration &report);
    virtual void csII_reportSensorData(BWCloudSensorII *csII,
	const BWCSII_reportSensorData &report);

    /* Called when CSII has decided that the roof should be closed (R cmd).  */
    virtual void csII_roofClose(BWCloudSensorII *csII);

    /* Called by readLoop() between reads; allows an opportunity to
     * call readStop().
     */
    virtual void csII_readLooping(BWCloudSensorII *csII);

    static const char *getCmdTypeName(CmdType val);
    static const char *getCmdOverReasonName(CmdOverReason val);
};

/*
 * Main CloudSensorII driver: the application should create a single
 * instance of this.  The most important methods are open(), readLoop(),
 * and readStop().
 */
class BWCloudSensorII
{
  public:
    enum DriverFlags {
	DF__UNINITIALIZED = 0,
	/* Keep a log of all charters sent/received on the serial link.  */
	DF_SERIAL_LOG = 1,
    };
    /*
     * listener: class to notify when various events happen;
     *     see comments in BWCSII_Listener.
     * applicationName: name of application program, which is entered in the
     *    log file.
     * csIILogDirectory: were csII communication logs are kept (format
     *	   intended to be the same as those kept by ClarityII; two types of
     *	   logs are kept: detailed ones which are rotated daily and kept for
     *	   5 days, and a long term one which is not rotated); if
     *	   csIILogDirectory is NULL, no logs are kept.
     * driverFlags: used to turn some options on/off - see DF_*.
     */
    BWCloudSensorII(BWCSII_Listener *listener, const char *applicationName,
	const char *csIILogDirectory,
	DriverFlags driverFlags = DF__UNINITIALIZED);
    void printState(std::ostream &os, int indent, int indentIncr) const;

    /* Attempt to open the serial connection to the CSII using
     * the given tty device (serial port).
     */
    void open(const char *ttyDev)
	throw (SerialConnectionException);
    /* returns true if serial connection is open (may not be working!). */
    bool isOpen() const;
    /* Close the serial connection to the CSII.
     * Returns true if close could be done; false if readLoop() is
     * still running (but this should should cause readLoop() to stop
     * immediately and close the connection).
     */
    bool close();

    /* Do a hardware reset of the sensor head. */
    void resetSensorHead()
	throw (SerialConnectionException);
    /* Ask CSII to start polling the PC; called automatically by 
     * checkForData() (and readLoop()) if needed.
     */
    void requestPolls()
	throw (SerialConnectionException);

    /* Enter a loop that waits for data to be available and then reads it.
     * Continues until readStop() is called.
     */
    void readLoop()
	throw (SerialConnectionException);
    /* Returns true if readLoop() has been called and has not returned. */
    bool inReadLoop();
    /* Causes readLoop() to stop looping. */
    // XXX threading...
    void readStop();
    /* Check if there is any data to read - if so, process it, respond
     * if appropriate and let listener know what is happening;
     * does not block.
     */
    void checkForData()
	throw (SerialConnectionException);
    /* Return the serial connection's file descriptor, or -1 if it
     * is not open.  Provided so poll(2)/select(2) can be used.
     */
    int getFileDescriptor() const;

    /* Get the last report received; the reportTime field will be 0
     * if no report has been received.
     */
    const BWCSII_reportReset &getLastReset() const;
    const BWCSII_reportThresholds &getLastThresholds() const;
    const BWCSII_reportWetnessCalibration &getLastWetnessCalibration() const;
    const BWCSII_reportWetnessData &getLastWetnessData() const;
    const BWCSII_reportThermopileCalibration &getLastThermopileCalibration()
	const;
    const BWCSII_reportSensorData &getLastSensorData() const;

    /* Ask CloudSensorII to send calibration data.
     * On completion, listener's csII_cmdOver(CT_GET_CALIBRATION) method
     * is called.
     * Then, if successful, listener's csII_reportThresholds(),
     * csII_reportWetnessCalibration() and csII_reportThermopileCalibration()
     * methods will be called.
     */
    void requestCalibrationData();
    /* Ask CloudSensorII to send calibration data.
     * On completion, listener's csII_cmdOver(CT_GET_WETNESS_CALIBRATION)
     * method is called.
     * Then, if successful,
     * listener's csII_reportWetnessCalibration() method will be called.
     */
    void requestWetnessCalibrationData();

    /* Tell CloudSensorII to use new threshold values.
     * On completion, listener's csII_cmdOver(CT_SET_THRESHOLDS) method
     * is called.
     */
    void setThresholds(const BWCSII_setThresholds &newThresholds);
    /* Tell CloudSensorII to restore default threshold values.
     * On completion, listener's csII_cmdOver(CT_SET_THRESHOLDS_DEFAULT) method
     * is called.
     */
    void setDefaultThresholds();

    /* Tell CloudSensorII to tell the adaptor box to close the roof.
     * ("Send a break to the PC.  This will cause the adaptor box to
     *	close the roof.  This could be used by PC software such as ACP.
     *	Note that it requires a completely functioning Cloud Sensor II
     *	to work.  A more direct approach might be safer.
     * ")
     * On completion, listener's csII_cmdOver(CT_ROOF_CLOSE) method is called.
     */
    void sendRoofClose();

    /* Send an arbitrary command to the CloudSensorII.
     * On completion, listener's csII_cmdOver(CT_RAW) method is called.
     */
    void sendRawCommand(const char *command);


    /*
     * Miscelanneous routines.
     */
    /* How many daily log files (containing csII communications) to keep;
     * default is 5.  A value of 0 means keep all logs, never delete them.
     */
    void setNDailyLogs(int nDailyLogs);

  private:
    enum {
	/* Default read timeout (used by readLoop()): how long to block
	 * reading before checking to see if readStop() or close() has
	 * been called.
	 */
	READ_TIMEOUT_MS = 1 * 1000,
	/* How long does the CSII take to re-set it self
	 * (recover from resetSensorHead()).
	 */
	RESET_TIME_SEC = 2, /* guess. */
	/* How long to wait for polls from the CSII before asking the CSII
	 * to send polls.  CSII times out after 10 seconds.
	 */
	POLL_TIMEOUT_SEC = 10 - RESET_TIME_SEC + 5,
	/* Max time allowed for parsing a frame/memory dump/purge/etc. */
	PARSE_TIMEOUT_SEC = 5,
	/* Max time allowed for parsing a memory dump/purge (delay
	 * before memory dump is around ? seconds;
	 * memory dump itself is around 10 seconds).
	 */
	PARSE_PURGE_TIMEOUT_SEC = 15,
	/* How long to hold onto a command before failing it. */
	CMD_TIMEOUT_SEC = 20,
    };
    /* Protocol related constants */
    enum {
	PURGE_START = '!',
	MEMORY_DUMP_START = '|',
	BOOT_LOADER_START = '^',
	REQUEST_POLL = 0x01, /* SOH */
	FRAME_START = 0x02, /* STX */
	FRAME_END = '\n',
	FRAME_MAX_LENGTH = 300, /* somewhat arbitrary (D around 160) */
	MAX_MEMORY_DUMP_SIZE = 0x800 * 4, /* is 0x800 but allow for more */
    };
    enum ReadLoopState {
	RLS_NONE,
	RLS_READING,
	RLS_STOP,
	RLS_CLOSE, /* stop & close */
    };
    enum ParseState {
	PS_NONE,
	PS_IN_PURGE,
	PS_SKIP_TO_FRAME_END,
	PS_IN_MEMORY_DUMP,
	PS_IN_FRAME,
    };
    enum ParseAction {
	PA_NONE,
	PA_SEND_ACK,
	PA_SEND_NACK,
	PA_POLL,
    };
    class CmdEntry {
      public:
	CmdEntry(BWCSII_Listener::CmdType cmdType, const char *cmd);
	void printState(std::ostream &os, int indent, int indentIncr) const;
	enum CmdState {
	    CS_INITIAL,
	    CS_PENDING, /* In the queue, not sent yet */
	    CS_SENT,	/* In the queue, sent, awaiting ack/nack/timeout. */
	    CS_DONE,	/* Have received ack (not in queue). */
	    CS_TIMED_OUT, /* Timed out (not in queue). */
	    CS_PURGED,	/* purged. */
	};
	static const char *getCmdStateName(CmdState val);

      public:
	BWCSII_Listener::CmdType cmdType;
	CmdState cmdState;
	unsigned int nSends;
	std::string cmd;
	struct timeval queuedTime;
	struct timeval sentTime;
	struct timeval ackedTime;
    };
    typedef std::deque<CmdEntry *> CmdQueue;
    class CSIILogger {
      public:
	enum {
	    SENSOR_DATA_HEADER_COUNT = 10,
	    N_DAILY_LOGS = 5,
	    /* Interval between "D" message entries in long term long. */
	    LONG_TERM_DATA_INTERVAL_SECS = 30 * 60,
	};
	CSIILogger(const char *applicationName, const char *logDirectory);
	~CSIILogger();
	void printState(std::ostream &os, int indent, int indentIncr) const;
	void logString(const char *code, const std::string &str,
		bool logToLongTerm = true);
	void logReceivedFrame(const struct timeval &readTime,
		const char *p, unsigned int nBytes,
		BWCSII_report::ReportType reportType,
		const char *reportHeader);
	void setNDailyLogs(int nDailyLogs);

      private:
	void checkOpenRotate(struct timeval now, const struct tm &nowTM,
		const std::string &timeStr);
	void printStartUpMessage(std::ostream *out,
		const std::string &timeStr);
	static void printTimezoneOffset(std::ostream *out,
		struct timeval now, const struct tm &nowTM,
		const std::string &timeStr);
	bool formatTime(struct timeval tv, struct tm *tm, std::string &str);

	/* Name of application program - entered in log files. */
	std::string applicationName;
        /* Directory to put log files in. */
	std::string logDirectory;

	/* Files opened for writting. */
	std::string dailyFileName;
	std::ofstream *dailyOut;
	std::string longTermFileName;
	std::ofstream *longTermOut;
	/* Day of the month that this log was started on (1..31). */
	int dailyDay;
	/* When the last "D" message was logged. */
	time_t longTermLastSensorDataTime;
	/* How many "D" messages have been reported since writing a header? */
	int dailySensorDataCount;
	int longTermSensorDataCount;
	/* 0 if haven't recorded a T report yet, 1 if have recorded one and
	 * not seen another, 2 if have seen more than 1 (don't report any more).
	 */
	int longTermThresholdsFlag;
	/* When last attempted to open the log files. */
	time_t lastOpenAttemptTime;
	/* Number of daily log files to keep; 0 means never delete. */
	int nDailyLogs;
    };


    /* cmd becomes owned by this. */
    void queueCmd(CmdEntry *cmd);
    void setParseState(ParseState ps, const struct timeval &tv);
    void startMemoryDump(const struct timeval &readTime);
    void doPurge(unsigned char reasonChar, const struct timeval &tv)
	throw (SerialConnectionException);
    void resetState(const struct timeval &tv);
    void parseMemoryDump();
    ParseAction parseFrame(const struct timeval &readTime)
	throw (SerialConnectionException);
    void parseFrameData(const struct timeval &readTime,
	    const char *p, unsigned int nBytes);
    void sendAck()
	throw (SerialConnectionException);
    void sendNack()
	throw (SerialConnectionException);
    void sendFrame(const char *p, unsigned int nBytes)
	throw (SerialConnectionException);
    void pollCheck(const struct timeval &tv)
	throw (SerialConnectionException);
    void ackReceived(const struct timeval &tv);
    void nackReceived(const struct timeval &tv, const char *crcStr);
    void flushCmdQueue(const struct timeval &tv);
    
  public:
    /* to ease testing. */
    static unsigned int calcCRC16(const void *buf, unsigned int nBytes);
  private:
    static int parseCRC(const char *p);
    static struct timeval timevalSub(const struct timeval &a,
	    const struct timeval &b);
    static unsigned int toHexDigit(unsigned char c) {
	return isdigit(c) ? c - '0' : tolower(c) - 'a' + 10;
    }
    static const char *getReadLoopStateName(ReadLoopState val);
    static const char *getParseStateName(ParseState val);
    static const char *getParseActionName(ParseAction val);

    static const char purgeCharSequence[];
    static const char driverName[];
    static const char driverVersion[];

    BWCSII_Listener *listener;
    CSIILogger csIILogger;
    DriverFlags driverFlags;
    std::string ttyDevice;
    SerialConnection *serial;
    unsigned int readTimeoutMS;
    ReadLoopState readLoopState;
    ParseState parseState;
    /* When entered this parseState. */
    struct timeval parseStateTime;
    /* This to avoid sending several responses when processing a number of
     * frames at once (such as may happen at start up, or if a long time
     * between calls to checkForData()); and to avoid sending a response
     * to a previous frame when in the middle of another frame (half duplex).
     */
    ParseAction parseAction;
    /* When last poll received. */
    struct timeval pollTime;
    /* When last requested CSII to start polling. */
    struct timeval requestPollTime;
    std::string inputBuf;
    CmdQueue cmdQueue;
    const char *nextPurgeChar;
    char purgeReasonChar;
    bool memoryDumpSawFirstEnd;
    time_t lastWetSkyMinusAmbientTime;

    BWCSII_reportReset lastReportI;
    BWCSII_reportThresholds lastReportT;
    BWCSII_reportWetnessCalibration lastReportK;
    BWCSII_reportWetnessData lastReportW;
    BWCSII_reportThermopileCalibration lastReportC;
    BWCSII_reportSensorData lastReportD;
};

#endif /* BW_CLOUD_SENSOR_II_H */
