#include <iostream>
#include <fstream>
#include <iomanip>
#include <algorithm>

#include "block.h"
#include "client.h"
#include "command.h"

using namespace std;

class Rts2CMonitor:public rts2core::Client
{
	protected:
		virtual rts2core::ConnClient * createClientConnection (char *_deviceName);
                rts2core::connections_t orderedConn;
                 rts2core::Connection *connection;

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
                        conn->queCommand (new rts2core::Command (this, ci->c_str ()));
        }
        //getStatesTest(conn);
*/
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


int Rts2CMonitor::init ()
{

   logStream (MESSAGE_ERROR) << "init calisti " <<  sendLog;

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
     logStream (MESSAGE_ERROR) << "add pollSocks calisti " <<  sendLog;
   
     // add stdin for ncurses input
   //     addPollFD (1, POLLIN | POLLPRI);
}


void Rts2CMonitor::pollSuccess ()
{
        rts2core::Client::pollSuccess ();

logStream (MESSAGE_ERROR) << "pollSuccess calisti " <<  sendLog;

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





int Rts2CMonitor::run ()
{
   logStream (MESSAGE_ERROR) << "run calisti " <<  sendLog;
   int ret =     rts2core::Client::run ();
   if(ret) return ret;
        //createClientConnection("T0");
        //conn->queCommand (new rts2core::Command (this,"altaz 10 10"));
        
orderedConn = *getConnections();

//while(orderedConn.size()<=0)
//{
//orderedConn = *getConnections();
//logStream (MESSAGE_ERROR) << "while calisiyoe "<<  sendLog;

//}

//sleep(5000);
//orderedConn = *getConnections();
logStream (MESSAGE_ERROR) << "size of connection " <<orderedConn.size()<<  sendLog;
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




   sleep(1);

	return 0;
}
 
int main (int argc, char **argv)
{
	Rts2CMonitor monitor = Rts2CMonitor (argc, argv);
	return monitor.run ();
}
