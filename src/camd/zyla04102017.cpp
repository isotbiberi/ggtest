#include "camd.h"
#include "atcore.h"
#include "atutility.h"
#include "CameraOperations.h"
#include "FitsOperations.h"
#include "configuration.h"
#include "iniparser.h"
//#include <pthread.h>
#include <thread>
#include<map>
namespace rts2camd{

class Zyla:public Camera{
public:
                Zyla(int in_argc, char **in_argv);
                void newThreadCallback(int *p);
 
                virtual int commandAuthorized(rts2core::Connection * conn);
                void task1(std::string msg)
               {
                   std::cout<<"	task says "<<msg;
                   }
//
//                void startThread()
//		{
 //               pthread_create(&mThreadId,0,&Zyla::CreateFitsThread,this);
                       
//		}

             //   Andor_Camera *cam = NULL;
                virtual ~Zyla (void);
private:
                rts2core::connections_t orderedConn;
                rts2core::Connection *connection;
                std::map<std::string,std::pair<std::string,std::string>> header;
                rts2core::ValueAltAz    * altAz;
                Andor_Camera *cam = NULL;
                //cam->initializeController();

                pthread_t thread[100];
		int temp_arg[100] ;
                
		void * threadTest(void );
                void *threadHelper(void *context)
		{
                 return ((Zyla*)context)->threadTest();
		}
		virtual int startExposure();
                
                virtual int doReadout();
                
                virtual int stopExposure ();
               
                virtual long isExposing ();
            
                virtual int setValue (rts2core::Value * old_value, rts2core::Value * new_value);

              /*
                static unsigned short* getFits()
                 {
                 
                 return  cam->getBuffer();
 
                 }

               */
                int getHeader();
                virtual int initChips()
                  {
                  //cam=new Andor_Camera(0);
                  //cam->initializeController();
 
                  setSize (2560,2160, 0,0);
                  return Camera::initChips ();
                  }
                int initHardware();

                rts2core::ValueSelection *fanSpeed;
                rts2core::ValueSelection *shutterMode;
                rts2core::ValueSelection *adcRate;
 	//	rts2core::ValueSelection *pixelEncoding;
		rts2core::ValueSelection *bitDepth;
		rts2core::ValueSelection *triggerMode;
		rts2core::ValueSelection *simpleGain;
		rts2core::ValueSelection *cycleMode;
	        rts2core::ValueFloat *expTime;		  


                // TEMP control
                 rts2core::ValueFloat * tempRamp;
                 rts2core::ValueFloat * tempTarget;
                 rts2core::ValueSelection * tempStatus;


                //Andor_Camera * cam = new Andor_Camera(0);

             




			};//class zyla

		};//namespace rts2camd
/*
static void *  threadWriteFits(void * threadId)

{
logStream(MESSAGE_INFO)<<"thread calisti"<<sendLog;

return NULL;
}
*/
using namespace rts2camd;

Zyla::Zyla (int in_argc, char **in_argv):Camera (in_argc, in_argv)
{
std::cout<<"constructor calisti"<<std::endl;
logStream (MESSAGE_INFO) << " Zyla constructor calisti  " <<sendLog;
createValue (tempStatus, "TCSTATUS", "Thermoelectric cooling status", false);
        tempStatus->addSelVal ("OFF");
        tempStatus->addSelVal ("NOT_STABILIZED");
        tempStatus->addSelVal ("STABILIZED");
        tempStatus->addSelVal ("NOT_REACHED");
        tempStatus->addSelVal ("OUT_RANGE");
        tempStatus->addSelVal ("NOT_SUPPORTED");
        tempStatus->addSelVal ("DRIFT");
        tempStatus->addSelVal ("UNKNOWN");

createValue (adcRate, "ADCRATE", "Analog Digital Converter Rate", true,RTS2_VALUE_WRITABLE);
        adcRate->addSelVal ("MHz10");
        adcRate->addSelVal ("MHz100");
        adcRate->addSelVal ("MHz200");
        adcRate->addSelVal ("MHz280");
/*
createValue (pixelEncoding, "PIXELENCODING", "Set Pixel Encoding", true,RTS2_VALUE_WRITABLE);
        pixelEncoding->addSelVal ("Mono12");
        pixelEncoding->addSelVal ("Mono12Packed");
        pixelEncoding->addSelVal ("Mono16");
        pixelEncoding->addSelVal ("Mono32");
*/

createValue (bitDepth, "BitDepth", "Set Bit Depth", true,RTS2_VALUE_WRITABLE);
        bitDepth->addSelVal ("b11");
        bitDepth->addSelVal ("b16");
        bitDepth->addSelVal ("UNKNOWN");



createValue (triggerMode, "trigerMode", "Triger Mode", true,RTS2_VALUE_WRITABLE);
        triggerMode->addSelVal ("Internal");
        triggerMode->addSelVal ("ExternalLevelTransition");
        triggerMode->addSelVal ("ExternalStart");
	triggerMode->addSelVal ("ExternalExposure");
 	triggerMode->addSelVal ("Software");
	triggerMode->addSelVal ("Advanced");
        triggerMode->addSelVal ("External");



createValue (simpleGain, "simpleGain", "Simple Gain is", true,RTS2_VALUE_WRITABLE);
        simpleGain->addSelVal ("b11_hi_gain");
        simpleGain->addSelVal ("b11_low_gain");
        simpleGain->addSelVal ("b16_lh_gain");
	simpleGain->addSelVal ("none");

createValue (cycleMode, "CycleMode", "Cycle Mode", true,RTS2_VALUE_WRITABLE);
        cycleMode->addSelVal ("Fixed");
        cycleMode->addSelVal ("Continuous");
        cycleMode->addSelVal ("UNKNOWN");


createValue (fanSpeed, "FanSpeed", "Show Speed", true,RTS2_VALUE_WRITABLE);
        fanSpeed->addSelVal ("Off");
        fanSpeed->addSelVal ("Low");
        fanSpeed->addSelVal ("On");



createValue (shutterMode, "shutterMode", "shutter Mode is ",true,RTS2_VALUE_WRITABLE);
        shutterMode->addSelVal ("Rolling");
        shutterMode->addSelVal ("Global");
        shutterMode->addSelVal ("UNKNOWN");



}
Zyla::~Zyla()
{

}



int Zyla::initHardware()
{
cam=new Andor_Camera(0);
//cam->initializeController();
//cam->prepareAcq();
return initChips ();

}


/*
void *Zyla::threadTest(intx)
{
createFits(cam->getBuffer(),0.01);
// pthread_exit(NULL); 
}

*/

void Zyla::newThreadCallback(int *p)
{
logStream(MESSAGE_INFO)<<"inside thread value parameter is "<<*p<<sendLog;

createFits(cam->getBuffer(),header);
}

int Zyla::commandAuthorized(rts2core::Connection * conn)
{
if(conn->isCommand("fast"))
{
if (getDeviceBopState () & BOP_EXPOSURE)
logStream(MESSAGE_INFO)<<"Capture should be blocked"<<sendLog;
else logStream(MESSAGE_INFO)<<"Capture should NOT be blocked"<<sendLog;




logStream(MESSAGE_INFO)<<"fast capture started"<<sendLog;
 double exptime;
 long numberOfExposures; 
if (conn->paramNextDouble (&exptime) || conn->paramNextLong (&numberOfExposures) || !conn->paramEnd ())
                                return -2;
 
logStream(MESSAGE_INFO)<<"exptime is set to  "<<exptime<<"number of exposure is " << numberOfExposures<<sendLog;

if(blockingExposure ())

logStream(MESSAGE_INFO)<<"exposure is blocked"<<sendLog;

else

logStream(MESSAGE_INFO)<<"exposure is NOT blocked"<<sendLog;


logStream(MESSAGE_INFO)<<"device state is "<<std::hex <<getState()<<sendLog;


cam->initializeController();
cam->setExpTime(exptime);
//cam->m_exp_time=exptime; //in seconds
cam->setNumberOfFrames(numberOfExposures);
cam->afterInitialization();
//logStream(MESSAGE_INFO)<<"zyla cpp setting exp time "<<sendLog;

cam->prepareAcq();
//double exptime;
//cam->getExpTime(exptime);
//logStream(MESSAGE_INFO)<<exptime<<sendLog;

//for(int i=0;i<1;i++)
//{
//maskState (CAM_MASK_EXPOSE | CAM_MASK_READING | BOP_TEL_MOVE, CAM_NOEXPOSURE | CAM_READING,
//                           "chip extended readout started");

//  maskState (CAM_MASK_EXPOSE | CAM_MASK_READING | BOP_TEL_MOVE, CAM_EXPOSING | CAM_NOTREADING,
  //                         "extended readout finished");


cam->startAcq();

getHeader();

for(int k=0;k<=cam->getNumberOfFrames()-1;k++)
{
createFits(cam->getFitsBuffer(k),header);
}

cam->stopAcq();
//logStream(MESSAGE_INFO)<<"inside the main thread"<<i<<sendLog;


//logStream(MESSAGE_INFO)<<"Alt az is " <<" info time is " << val->getValueDouble()<<sendLog;



/*
for(int s=0;s<orderedConn.size();s++)
{
logStream(MESSAGE_INFO)<<"Connection name is " << orderedConn[s]-> getName()<<sendLog;
}
*/
/*
if(orderedConn.size()>0)
{
connection = orderedConn[0];
     
logStream(MESSAGE_INFO)<<"Connection name is " << connection-> getName()<<sendLog;

    for (rts2core::ValueVector::iterator iter = connection->valueBegin (); iter != connection->valueEnd (); iter++)
        {
     logStream(MESSAGE_INFO)<<"value name is " << (*iter)-> getName().c_str()<<"value value is "<< (*iter)-> getValue()<<sendLog;

     }
*/
/*get values from connection*/

/*
      connection = orderedConn[0];

        rts2core::Configuration *config = rts2core::Configuration::instance ();

        std::string  telescop;
        telescop[0] = '\0';
        std::string instrume;
        instrume[0] = '\0';
        std::string origin;
        origin[0] = '\0';

       int ret1= config->getString (connection->getName (), "instrume", instrume);
       int ret2= config->getString (connection->getName (), "telescop", telescop);
       int ret3= config->getString (connection->getName (), "origin", origin);
                       
       logStream(MESSAGE_INFO)<<"instrument  telescop is connection get name is "<<sendLog;
            
*/
//}

//}
//cam->stopAcq();//fixed oldugunda stop demelimiyiz?

}
return Camera::commandAuthorized(conn);


}
int Zyla::startExposure()
{

logStream (MESSAGE_INFO) << " Zyla startExposure calisti quedExpnumber is  "<<quedExpNumber->getValueInteger() <<sendLog;
//realTimeDataTransferCount =0;
//cam->initializeController();
cam->prepareAcq();
//for(int imageCount=0;imageCount<10;imageCount++)
//{
cam->startAcq();
logStream(MESSAGE_INFO)<<"pos aliniyor"<<sendLog;
//}
  // return realTimeDataTransferCount >= 0 ? 1 : 0;
return 0;
} 

int Zyla::doReadout()
{



logStream (MESSAGE_INFO) << " Zyla camera doreadout calisti  " <<sendLog;
  

unsigned short *buffer = cam->getBuffer();

/*
AT_ConvertBuffer((unsigned char *)buffer,
reinterpret_cast<unsigned char*>(unpackedBuffer),
2160, 2560,
5120, L"Mono12Packed", L"Mono16");
*/
int ret =  sendReadoutData((char*)buffer,11059200);//getWriteBinaryDataSize());
if(ret<0)
                {

                logStream (MESSAGE_ERROR) << "Send read out data yapilamadi " << sendLog;
               return -1;
                }

//cam->stopAcq();


return -2;
}
int Zyla::getHeader()
{
	/*get values from connection*/
   double expTime;
   cam->getExpTime(expTime);
   std::ostringstream exposureTime;
   exposureTime<<expTime;
   header["expTime"]=std::make_pair(exposureTime.str(),"Exposure time in seconds taken from zyla");

	/*
	orderedConn = *getConnections();
	logStream(MESSAGE_INFO)<<"Connection size is " << orderedConn.size()<<sendLog;
	rts2core::connections_t::iterator iter;


	  for (iter = getConnections ()->begin (); iter != getConnections ()->end (); iter++)
	        {
	              logStream(MESSAGE_INFO)<<"client connection is " <<(*iter)->getName()<<sendLog;
	              if(strlen((*iter)->getName())==0)  logStream(MESSAGE_INFO)<<"sanirim bu C1";
	        }

	*/

	/*
	connection = getConnection("ddm160");
	if(connection)
	{
	logStream(MESSAGE_INFO)<<"T0 connection is " <<connection->getName()<<sendLog;
	altAz = dynamic_cast<rts2core::ValueAltAz *> (connection->getValue ("TEL_"));
	rts2core::Value *val = connection->getValue ("infotime");
	rts2core::ValueRaDec *  raDec = dynamic_cast<rts2core::ValueRaDec *> (connection->getValue ("TEL"));
	}

	*/

	/*
	connection = getConnection("W0");
	if(connection)
	{
	rts2core::Value * filter =dynamic_cast<rts2core::Value *>(connection->getValue ("filter"));
	if(filter!=NULL)
	logStream(MESSAGE_INFO)<<"filter is "<< val->getValueInteger()<<sendLog;
	}
	else  logStream(MESSAGE_INFO)<<" there is no filterwheel  connection"<<sendLog;
	*/

	/*Focuser calismiyor crash oluyor*/
	/*
	connection = getConnection("F0");


	if(connection)
	{
	rts2core::Value * focusPos =dynamic_cast<rts2core::Value *>(connection->getValue ("FOC_POS"));
	if(focusPos!=NULL)
	logStream(MESSAGE_INFO)<<"focuse position is "<< focusPos->getValueInteger()<<sendLog;
	}
	else  logStream(MESSAGE_INFO)<<" there is no focuse position  connection"<<sendLog;

	*/


	/*FOCUSER*/

	/*
	if(val ==NULL)
	logStream(MESSAGE_INFO)<<" info time is null"<<sendLog;
	else
	logStream(MESSAGE_INFO)<<"info time is "<< val->getValueDouble()<<sendLog;



	if(altAz == NULL)
	logStream(MESSAGE_INFO)<<" altAz is null"<<sendLog;
	else
	{
	//alt=altAz->getAlt();
	std::ostringstream dblToString1,dblToString2;
	dblToString1 << altAz->getAlt();
	std::string alt = dblToString1.str();
	dblToString2 << altAz->getAz();
	std::string az = dblToString2.str();

	header["alt"]=std::make_pair(alt,"ALTITUDE TAKEN FROM RTS2");
	header["az"]=std::make_pair(az,"AZIMUTH TAKEN FROM RTS2");

	logStream(MESSAGE_INFO)<<"alt is bla bla  az is bla bla " <<sendLog;
	logStream(MESSAGE_INFO)<<"alt is"<<altAz->getAlt()<<" az is " <<altAz->getAz()<<sendLog;

	}
	if(raDec == NULL)
	logStream(MESSAGE_INFO)<<" radec is null"<<sendLog;
	else
	{
	std::ostringstream dblToString1,dblToString2;
	dblToString1 << raDec->getRa();
	std::string ra = dblToString1.str();
	dblToString2 << raDec->getDec();
	std::string dec = dblToString2.str();

	header["RA"]=std::make_pair(ra,"RA TAKEN FROM RTS2");
	header["DEC"]=std::make_pair(dec,"DEC TAKEN from RTS2");
	}


	*/

	//header["alt"] =  std::make_pair("45", "altitude value");
	//header["az"] =  std::make_pair("34", "azimuth vale");
	//createFits(cam->getBuffer(),0.1);

	/*thread kullanmak icin*/

	//std::thread t(&Zyla::newThreadCallback,this,&i);
	//t.detach();
	/*thread kullanmak icin*/
}
int Zyla::stopExposure ()
{
/*

       logStream(MESSAGE_INFO)<< "stopExposure calisti"<<sendLog; 
        nullExposureConn ();
         AT_Command(Hndl, L"AcquisitionStop");
         AT_Flush(Hndl);
  */     
 

     //   AbortAcquisition ();
     //   FreeInternalMemory ();
        return Camera::stopExposure ();
     //return 1;

}

long Zyla::isExposing()
{

long ret;
ret = Camera::isExposing();
if(ret>0)
return ret;
logStream(MESSAGE_INFO) <<"isExposing calisti"<<sendLog;
AT_BOOL is_exposing=false;
cam->getIsAcquiring(is_exposing);
//if(i_retCode !=AT_SUCCESS){logStream(MESSAGE_INFO)<<"Cannot get if camera is acquiring or not"<<sendLog;return -1;}
if(is_exposing) return 200;
return -2;

}

/* ***** SET VALUES ***** */

int Zyla::setValue (rts2core::Value * old_value, rts2core::Value * new_value)
{
    

/*
    if ((cap.ulSetFunctions & AC_SETFUNCTION_EMCCDGAIN) && (old_value == emccdgain))
                return setEMCCDGain (new_value->getValueInteger ()) == 0 ? 0 : -2;
        if (old_value == VSAmp)
                return setVSAmplitude (new_value->getValueInteger ()) == 0 ? 0 : -2;

        // EMON is purely an alias to adcMode (switching btw 2 modes)
        if (old_value == EMOn)
        {
                if (defaultADCMode == 0)
                        return 0;       // no EM capability

                int new_mode = ((rts2core::ValueBool *) new_value)->getValueBool () == 0 ? defaultADCMode : 0;
                changeValue (adcMode, new_mode);
  
*/

if (old_value == adcRate)  
{
Andor_Camera::A3_ReadOutRate readOutRate = static_cast<Andor_Camera::A3_ReadOutRate>(new_value->getValueInteger());
cam->setAdcRate(readOutRate);
logStream(MESSAGE_INFO)<<"adc rate degistirilmeye calisildi"<<sendLog; 
}
/*
if(old_value == pixelEncoding)
{
Andor_Camera::A3_PixelEncoding pixelEncodingNew = static_cast<Andor_Camera::A3_PixelEncoding>(new_value->getValueInteger());
//cam->setPixelEncoding(pixelEncodingNew);
logStream(MESSAGE_INFO)<<"pixelEncoding degistiriliyor"<<sendLog;
}
*/
if(old_value == bitDepth) 
{
Andor_Camera::A3_BitDepth bitDepthNew = static_cast<Andor_Camera::A3_BitDepth>(new_value->getValueInteger());
cam->setBitDepth(bitDepthNew);

logStream(MESSAGE_INFO)<<"bitDepth degistiriliyor"<<sendLog;

}
if(old_value == triggerMode)
{
logStream(MESSAGE_INFO)<<"triger mode degistiriliyor"<<sendLog;
Andor_Camera::A3_TriggerMode triggerModeNew = static_cast<Andor_Camera::A3_TriggerMode>(new_value->getValueInteger());
cam->setTriggerMode(triggerModeNew);

}
if(old_value == simpleGain)
{
logStream(MESSAGE_INFO)<<"simple gain degistiriliyor"<<sendLog;
Andor_Camera::A3_SimpleGain simpleGainNew = static_cast<Andor_Camera::A3_SimpleGain>(new_value->getValueInteger());
cam->setSimpleGain(simpleGainNew);

}
if(old_value == cycleMode)
{
logStream(MESSAGE_INFO)<<"cyclemode degistiriliyor"<<sendLog;
              
 
}
return 0;

       }




int main(int argc, char **argv)

{

        Zyla device (argc, argv);
        return device.run ();


/*
std::cout<<"ismail"<<std::endl;
        int i_retCode = AT_InitialiseLibrary();
        if (i_retCode != AT_SUCCESS) { std::cout << "Cannot initialize camera API"; }
        AT_64 iNumberDevices = 0;
        i_retCode = AT_GetInt(AT_HANDLE_SYSTEM, L"DeviceCount", &iNumberDevices);
        if (iNumberDevices <= 0)
        {
                std::cout << "no cameras Found"; exit;
        }
        
        else 
        {
         std::cout << "camera found" << std::endl;
         }

*/
}
