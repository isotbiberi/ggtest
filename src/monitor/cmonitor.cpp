#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include "block.h"
#include "client.h"
#include "command.h"

using namespace std;
#define EVENT_MOVE_OK              RTS2_LOCAL_EVENT+53
#define EVENT_EXPOSURE_OK          RTS2_LOCAL_EVENT+54
#define EVENT_CCD_READY         RTS2_LOCAL_EVENT+55
#define EVENT_MOUNT_READY         RTS2_LOCAL_EVENT+56
#define EVENT_STOP_PROGRAM	RTS2_LOCAL_EVENT+57
#define EVENT_MOVE_EXPOSE_COMPLETED RTS2_LOCAL_EVENT+58


class  Rts2CMonitor:public rts2core::Client
{
	protected:
		virtual rts2core::ConnClient * createClientConnection (char *_deviceName);
                rts2core::connections_t orderedConn;
                rts2core::Connection *connection;
                rts2core::Connection *ccdConnection;
                rts2core::Connection *mountConnection;


	public:

		Rts2CMonitor (int in_argc, char **in_argv):rts2core::Client (in_argc, in_argv, "cmonitor") {}
		virtual int idle ();
		virtual int run ();
                virtual int init(); 
        //        virtual rts2core::ConnClient *createClientConnection (int _centrald_num, char *_deviceName);

                virtual rts2core::DevClient *createOtherType (rts2core::Connection * conn, int other_device_type);
                virtual rts2core::ConnCentraldClient *createCentralConn ();
                void commandReturn (rts2core::Command * cmd, int cmd_status);

        private:
                        std::map <std::string, std::list <std::string> > initCommands;
                        virtual void addPollSocks ();
                        virtual void pollSuccess ();
                        virtual void postEvent (rts2core::Event *event);
                        bool ccdReady = false;
                        bool mountReady = false;
                        void startAcquisition(long num);
                        long exposeNumber=0;
} ;



class Rts2CMonitorCentralConn:public rts2core::ConnCentraldClient
{
        public:
                        Rts2CMonitorCentralConn (Rts2CMonitor * in_master,
                        const char *in_login,
                        const char *in_name,
                        const char *in_password,
                        const char *in_master_host,
                        const char
                        *in_master_port):rts2core::ConnCentraldClient (in_master,
                        in_login,
                        in_name,
                        in_password,
                        in_master_host,
                        in_master_port)
                {
                        master = in_master;
                       master->setMasterState (this, WR_MASK);
                      logStream(MESSAGE_INFO)<< "setmasterstate calisti"<<sendLog;
                }

                virtual void commandReturn (rts2core::Command * cmd, int in_status)
                {
                        master->commandReturn (cmd, in_status);
                        rts2core::ConnCentraldClient::commandReturn (cmd, in_status);
                }
  
        private:
                Rts2CMonitor * master;
};


class Rts2CMonitorConnection:public rts2core::ConnClient
{
	private:
		//void printStatus ();
                //rts2core::connections_t orderedConn;
                Rts2CMonitor * master;

	public:
		Rts2CMonitorConnection (Rts2CMonitor * _master, int _centrald_num, char *_name):rts2core::ConnClient (_master, _centrald_num, _name)
		{
		master = _master;
                }
                virtual void commandReturn (rts2core::Command * cmd, int in_status)
                {
                        master->commandReturn (cmd, in_status);
                        return rts2core::ConnClient::commandReturn (cmd, in_status);
                }

};






rts2core::ConnCentraldClient * Rts2CMonitor::createCentralConn ()
{
        return new Rts2CMonitorCentralConn (this, getCentralLogin (), getName (), getCentralPassword (), getCentralHost (), getCentralPort ());
}


rts2core::DevClient * Rts2CMonitor::createOtherType (rts2core::Connection * conn, int other_device_type)
{
       logStream(MESSAGE_INFO)<<"createother type calisti "<<conn->getName()<<sendLog; 
       rts2core::DevClient *retC = rts2core::Client::createOtherType (conn, other_device_type);
       
       //maskState (WEATHER_MASK, BAD_WEATHER, "ismail bad weather yapti");

/*
         if (other_device_type == DEVICE_TYPE_MOUNT && tarArg)
        {
                struct ln_equ_posn tarPos;
                tarArg->getSimbadPosition (&tarPos);
                conn->queCommand (new rts2core::CommandMove (this, (rts2core::DevClientTelescope *) retC, tarPos.ra, tarPos.dec));
        }
*/



        // queue in commands
      

/*
        std::string telCon ="T0";
        std::string camCon ="C0";
        std::list<std::string> mountCommandsString;
        std::list<std::string> camCommandsString;
        mountCommandsString.push_back("altaz 10 10");
        mountCommandsString.push_back("altaz 20 20");
        mountCommandsString.push_back("altaz 50 50");

         camCommandsString.push_back("fast 0.1 1");
         camCommandsString.push_back("fast 1 2");
         camCommandsString.push_back("fast 10 3");
         initCommands.insert(std::make_pair(telCon,mountCommandsString));
       // initCommands.insert(std::make_pair(camCon,camCommandsString));

        std::map <std::string, std::list <std::string> >::iterator iter = initCommands.find (conn->getName ());
        //logStream(MESSAGE_INFO)<<conn->getName ()<< conn->getStateString ().c_str ()<< conn->getState ()<< conn->getFullBopState ()<< "connection state is " <<std::hex<<conn->getState()<<" connection name is "<<conn->getName()<<sendLog; 
        if (iter != initCommands.end ())
        {
                for (std::list <std::string>::iterator ci = iter->second.begin (); ci != iter->second.end (); ci++)
                     // conn->queCommand (new rts2core::Command (this, ci->c_str ()));
                conn->queCommand (new rts2core::CommandMove (this, (rts2core::DevClientTelescope *) retC, 50, 50));

        }
        //getStatesTest(conn);
*/

       if (other_device_type == DEVICE_TYPE_MOUNT)
        {
                 
                 mountConnection = conn;
                 mountReady = true;
                 //conn->queCommand (new rts2core::Command (this, "altaz 20 20"));
                 
                 //mountConnection->queCommand (new rts2core::CommandMoveAltAz (this, (rts2core::DevClientTelescope *) retC,50 ,50));
                 //struct ln_hrz_posn *hrz = (struct ln_hrz_posn *) event->getArg ();
                 //mountConnection->queCommand (new rts2core::CommandMoveAltAz (mountConnection->getMaster (),  (rts2core::DevClientTelescope *) retC, 20, 90), BOP_TEL_MOVE);
                 //sleep(2);
                 postEvent (new rts2core::Event (EVENT_MOUNT_READY));

        }

               if (other_device_type == DEVICE_TYPE_CCD)
        {
            ccdConnection = conn;
            ccdReady=true; 
           // conn->queCommand (new rts2core::Command (this, "fast 11 11"));
           postEvent(new rts2core::Event (EVENT_CCD_READY));

       }

        return retC;
}



rts2core::ConnClient * Rts2CMonitor::createClientConnection (char *_deviceName)
{
	rts2core::NetworkAddress *addr = findAddress (_deviceName);
	
        if (addr == NULL)
	{
        logStream (MESSAGE_ERROR) << "adres is null " <<  sendLog;
	return NULL;
        }
	return new Rts2CMonitorConnection (this, addr->getCentraldNum (), _deviceName);
}

void Rts2CMonitor::commandReturn (rts2core::Command * cmd, int cmd_status)
{
//logStream(MESSAGE_INFO)<<"command return calisti--command is ---   "<<cmd->getText()<<"   status is--- " <<cmd_status<<sendLog;

       /* 
       if (oldCommand == cmd)
        {
                comWindow->commandReturn (cmd, cmd_status);
               // repaint ();
        }
       */
}



int Rts2CMonitor::idle ()
{
/*
	if (commandQueEmpty ())
		endRunLoop ();

*/
 
        //logStream (MESSAGE_ERROR) << "idle calisti " <<  sendLog;
        
/*
        orderedConn = *getConnections();
        if(orderedConn.size()==2)
        {

        endRunLoop();
        }
*/  
      //return 0;
	return rts2core::Client::idle ();
}

void Rts2CMonitor::postEvent (rts2core::Event *event)
{
        switch (event->getType ())
        {
                case EVENT_MOVE_OK :
                /*
                logStream(MESSAGE_INFO)<<"post event olustu event move ok"<<sendLog;
                if(!ccdReady)
               {
                logStream(MESSAGE_INFO)<<"ccd not ready"<<sendLog;

               } 
               //if(!ccdReady) logStream(MESSAGE_INFO)<<"ccd is not ready"<<sendLog;
                else
                ccdConnection->queCommand (new rts2core::Command (this, "fast 11 1"));
                //sleep(2);
                postEvent (new rts2core::Event (EVENT_EXPOSURE_OK));
 
                break;   
 
                case EVENT_EXPOSURE_OK :
                logStream(MESSAGE_INFO)<<"post event olustu event exposure ok"<<sendLog;
                mountConnection->queCommand (new rts2core::Command (this, "altaz 20 20"));
                //sleep(2);
                //postEvent (new rts2core::Event (EVENT_MOVE_OK));
                
                */

                case EVENT_CCD_READY :
                logStream(MESSAGE_INFO)<<"posted ccd_ready event"<<sendLog;
                //ccdReady = true;
                if(mountReady&&ccdReady)
                startAcquisition(0);
                else
                if(!mountReady)postEvent (new rts2core::Event (EVENT_MOUNT_READY));
 
                break;
                
                case EVENT_MOUNT_READY :
                logStream(MESSAGE_INFO)<<"posted mount_ready event"<<sendLog;
                //mountReady = true;
                if(ccdReady&&mountReady)
                startAcquisition(0);
                else
                if(!ccdReady)postEvent (new rts2core::Event (EVENT_CCD_READY));

                break;
               
               /*
 
                case EVENT_STOP_PROGRAM:
                endRunLoop ();


                case EVENT_MOVE_EXPOSE_COMPLETED:
                exposeNumber++;
                startAcquisition(exposeNumber);


                break; 
              */

        return;
        }
         //logStream(MESSAGE_INFO)<<"post event olustu"<<sendLog;

        rts2core::Client::postEvent (event);
}






int Rts2CMonitor::init ()
{

   logStream (MESSAGE_ERROR) << "init calisti " <<  sendLog;
    //<<"number of centrald connection is " <<getCentraldConns ()->size ()<<endl;
 rts2core::connections_t::iterator iter;

      for (iter = getCentraldConns ()->begin (); iter != getCentraldConns ()->end (); iter++)
        {
                logStream(MESSAGE_INFO)<<"initi cerisinde connection calisti"<<sendLog;
             //   setState (BOP_TEL_MOVE, "ismail state deistirdi");
             //  (*iter)->setState(BOP_TEL_MOVE,"ismail state deistiri");
               // cout<<"init icerisindeki connection calisti"<<endl;
               // cout<<"connectin getname is " <<(*iter)->getName ()<<"connection getstateString is "<<(*iter)->getStateString ().c_str ()<<"connection getstate is "<<(*iter)->getState ();

}
        int ret;
        ret = rts2core::Client::init ();
        if (ret)
                return ret;

logStream (MESSAGE_ERROR) << "init bitti " <<  sendLog;


return 0;

}

void Rts2CMonitor::addPollSocks ()
{
        rts2core::Client::addPollSocks ();
     //logStream (MESSAGE_ERROR) << "add pollSocks calisti " <<  sendLog;
   
     // add stdin for ncurses input
   //     addPollFD (1, POLLIN | POLLPRI);
}


void Rts2CMonitor::pollSuccess ()
{
        rts2core::Client::pollSuccess ();

//logStream (MESSAGE_ERROR) << "pollSuccess calisti " <<  sendLog;

/*
        while (1)
        {
                int input = getch ();
                if (input == ERR)
                        break;
                processKey (input);
        }
*/
}

void Rts2CMonitor::startAcquisition(long move_expose_number)
{

logStream(MESSAGE_INFO)<<"Acquisition number is "<<move_expose_number<<sendLog;
mountConnection->queCommand (new rts2core::Command (this, "altaz 20 20"));
//ccdConnection->queCommand (new rts2core::Command (this, "fast 1 1"));
//sleep(1);

/*calisiyor*/
/*
if(move_expose_number < 2)
postEvent (new rts2core::Event (EVENT_MOVE_EXPOSE_COMPLETED));
*/

//postEvent (new rts2core::Event (EVENT_STOP_PROGRAM));



}



int Rts2CMonitor::run ()
{
   logStream (MESSAGE_ERROR) << "run calisti " <<  sendLog;
   int ret =     rts2core::Client::run ();
   if(ret) return ret;

/*
rts2core::connections_t::iterator iter;

      for (iter = getCentraldConns ()->begin (); iter != getCentraldConns ()->end (); iter++)
        {
                logStream(MESSAGE_INFO)<<"run içinde connection calisti"<<sendLog;

               //(*iter)->setState (BOP_TEL_MOVE, "ismail");
               //master->setMasterState (this, BOP_TEL_MOVE);




               // cout<<"init icerisindeki connection calisti"<<endl;
                cout<<"connectin getname is " <<(*iter)->getName ()<<"connection getstateString is "<<(*iter)->getStateString ().c_str ()<<"connection getstate is "<<(*iter)->getState ();

             //     (*iter)->setState(WR_MASK,"ismail yazi");    


}
*/



        //createClientConnection("T0");
        //conn->queCommand (new rts2core::Command (this,"altaz 10 10"));
        
//orderedConn = *getConnections();

//while(orderedConn.size()<=0)
//{
//orderedConn = *getConnections();
//logStream (MESSAGE_ERROR) << "while calisiyoe "<<  sendLog;

//}

//sleep(5000);
//orderedConn = *getConnections();
//logStream (MESSAGE_ERROR) << "size of connection " <<orderedConn.size()<<  sendLog;
/*
if(orderedConn.size()>0)
{
       connection = orderedConn[0];
       logStream (MESSAGE_ERROR) << "connection var " <<connection->getName()<<  sendLog; 
       
      for (rts2core::ValueVector::iterator iter = connection->valueBegin (); iter != connection->valueEnd (); iter++)
        {

        }
}

	rts2core::connections_t::iterator iter;
	for (iter = getCentraldConns ()->begin (); iter != getCentraldConns ()->end (); iter++)
	{
//		(*iter)->queCommand (new CommandInfo (this));
//                logStream (MESSAGE_ERROR) << " testing  " << sendLog;

	}
*/

/*

        std::string telCon ="T0";
        std::string camCon ="C0";
        std::list<std::string> mountCommandsString;
        std::list<std::string> camCommandsString;
        mountCommandsString.push_back("altaz 10 10");
        mountCommandsString.push_back("altaz 20 20");
        mountCommandsString.push_back("altaz 50 50");

         camCommandsString.push_back("fast 0.1 1");
         camCommandsString.push_back("fast 1 2");
         camCommandsString.push_back("fast 10 3");
        initCommands.insert(std::make_pair(telCon,mountCommandsString));
       // initCommands.insert(std::make_pair(camCon,camCommandsString));

        std::map <std::string, std::list <std::string> >::iterator iter = initCommands.find (telCon);
        logStream(MESSAGE_INFO)<<orderedConn[0]->getName ()<< sendLog;
        //conn->getStateString ().c_str ()<< conn->getState ()<< conn->getFullBopState ()<< "connection state is " <<std::hex<<conn->getState()<<" connection name is "<<conn->getName()<<sendLog; 
        if (iter != initCommands.end ())
        {
                 logStream(MESSAGE_INFO)<<orderedConn[0]->getName ()<< sendLog;

                  for (std::list <std::string>::iterator ci = iter->second.begin (); ci != iter->second.end (); ci++)
                        { 
                         orderedConn[0]->queCommand (new rts2core::Command (this, ci->c_str ()));
                        logStream(MESSAGE_INFO)<<ci->c_str ()<< sendLog;

                         }
        }


*/

   sleep(1);

	return 0;
}
 
int main (int argc, char **argv)
{
	Rts2CMonitor monitor = Rts2CMonitor (argc, argv);
	return monitor.run ();
}
