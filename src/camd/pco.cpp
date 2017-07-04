#include "camd.h"
#include <thread>
#include "VersionNo.h"
#include "Cpco_com.h"
//#include "Cpco_grab_usb.h"
#include "file12.h"


namespace rts2camd{

class Pco:public Camera{
public:
                Pco(int in_argc, char **in_argv);
                void newThreadCallback(int *p);
 
                virtual int commandAuthorized(rts2core::Connection * conn);
                virtual ~Pco (void);
private:
		virtual int startExposure();
                
                virtual int doReadout();
                
                virtual int stopExposure ();
               
                virtual long isExposing ();
            
                virtual int setValue (rts2core::Value * old_value, rts2core::Value * new_value);

                virtual int initChips()
                  { 
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



             




			};//class pco

		};//namespace rts2camd
using namespace rts2camd;

Pco::Pco (int in_argc, char **in_argv):Camera (in_argc, in_argv)
{
std::cout<<"constructor calisti"<<std::endl;
logStream (MESSAGE_INFO) << " Pco constructor calisti  " <<sendLog;
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
Pco::~Pco()
{

}


int Pco::initHardware()
{
 return initChips ();

}

void Pco::newThreadCallback(int *p)
{
logStream(MESSAGE_INFO)<<"inside thread value parameter is "<<*p<<sendLog;
}

int Pco::commandAuthorized(rts2core::Connection * conn)
{
if(conn->isCommand("fast"))
{
}
for(int i=0;i<1000;i++)
{
//createFits(cam->getBuffer(),0.1);
std::thread t(&Pco::newThreadCallback,this,&i);
t.detach();
logStream(MESSAGE_INFO)<<"inside the main thread"<<i<<sendLog;
logStream(MESSAGE_INFO)<<"pos alindi poz numarasi "<<i<<" pivel value "<<sendLog;
}



return Camera::commandAuthorized(conn);


}
int Pco::startExposure()
{

logStream (MESSAGE_INFO) << " Pco startExposure calisti quedExpnumber is  "<<quedExpNumber->getValueInteger() <<sendLog;
return 0;
} 

int Pco::doReadout()
{



logStream (MESSAGE_INFO) << " Pco camera doreadout calisti  " <<sendLog;
  

return -2;
}

int Pco::stopExposure ()
{
        return Camera::stopExposure ();

}

long Pco::isExposing()
{

return -2;

}

/* ***** SET VALUES ***** */

int Pco::setValue (rts2core::Value * old_value, rts2core::Value * new_value)
{
    

return 0;

       }




int main(int argc, char **argv)

{

        Pco device (argc, argv);
        return device.run ();


}
