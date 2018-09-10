#include "block.h"
#include "client.h"
#include "command.h"
#include "simbadtarget.h"
#include <libnova/libnova.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <list>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include<curses.h>
//#include<conio.h>


      
class commandSender:public rts2core::Client
{
        public:
                
                //rts2core::Connection *connection;
                commandSender (int argc, char **argv);
                virtual ~ commandSender (void);

                virtual int init ();
                virtual void postEvent (rts2core::Event *event);

                virtual rts2core::ConnClient *createClientConnection (int _centrald_num, char *_deviceName);
                virtual rts2core::DevClient *createOtherType (rts2core::Connection * conn, int other_device_type);

                //virtual int deleteConnection (rts2core::Connection * conn);

                //virtual void deleteClient (int p_centraldId);

                //virtual void message (rts2core::Message & msg);
                void sendCommand();
                void resize ();
                void checkConnections();
                void changeSelectedRow(int i);
                void processKey (int key);
                void commandReturn (rts2core::Command * cmd, int cmd_status);
                virtual void message (rts2core::Message & msg);
                virtual void addPollSocks ();
                virtual void pollSuccess ();
                //void print_menu(WINDOW *menu_win, int highlight);


        protected:
            //    virtual int processOption (int in_opt);

            //    virtual int processArgs (const char *arg);
               void getStatesTest(rts2core::Connection * conn);

                virtual rts2core::ConnCentraldClient *createCentralConn ();
          //      rts2core::Connection *getConnection ()
          //      {
          //              return connection;
           //     }


	private:
                rts2core::Block * block;
                rts2core::connections_t *conns;
                rts2core::connections_t orderedConn;
                rts2core::SimbadTarget *tarArg =NULL;
                rts2core::Connection *connection;

               
                rts2core::Connection *connectionAt (unsigned int i);

                double refresh_rate;

                std::map <std::string, std::list <std::string> > initCommands;

                   
};

void commandSender::message (rts2core::Message & msg)
{
        logStream(MESSAGE_INFO)<<"Message info is " << msg;
}

void commandSender::checkConnections ()
 {
         rts2core::connections_t::iterator iter;
 
        for (iter = getConnections ()->begin (); iter != getConnections ()->end (); iter++)
        {
              std::cout<<"sonsuz client connection is " <<(*iter)->getName()<<std::endl;
            
        }

orderedConn = *getConnections();


if(orderedConn.size()>0)
{
connection = orderedConn[0];
       for (rts2core::ValueVector::iterator iter = connection->valueBegin (); iter != connection->valueEnd (); iter++)
        {

        }


}

//std::cout<<"orderedconn name is "<<orderedConn[0]->getName()<<std::endl;


}
void commandSender::sendCommand()
{

}
void commandSender::changeSelectedRow(int i)
{
//std::cout<<"selection is " <<selection<<std::endl;
}


void commandSender::postEvent (rts2core::Event *event)
{

logStream(MESSAGE_INFO)<<"postevent calisti"<<sendLog;  
      rts2core::Client::postEvent (event);
}


class commandSenderConn:public rts2core::ConnClient
	{
        public:
                commandSenderConn (commandSender * _master, int _centrald_num, char *_name):rts2core::ConnClient (_master, _centrald_num, _name)
                {
                        master = _master;
                }

                virtual void commandReturn (rts2core::Command * cmd, int in_status)
                {
                        master->commandReturn (cmd, in_status);
                        return rts2core::ConnClient::commandReturn (cmd, in_status);
                }
        private:
                commandSender * master;
};
class commandSenderCentralConn:public rts2core::ConnCentraldClient
{
        public:
                commandSenderCentralConn (commandSender * in_master,
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
                commandSender * master;
};

rts2core::ConnCentraldClient * commandSender::createCentralConn ()
{
        return new commandSenderCentralConn (this, getCentralLogin (), getName (), getCentralPassword (), getCentralHost (), getCentralPort ());
}


commandSender::commandSender (int in_argc, char **in_argv):rts2core::Client (in_argc, in_argv, "sequencer")
{
connection= NULL;
}

commandSender::~commandSender (void)
{
}



void commandSender::addPollSocks ()
{


        //logStream(MESSAGE_INFO)<<"addpollsocks calisti"<<sendLog;
        rts2core::Client::addPollSocks ();
        // add stdin for ncurses input
        addPollFD (1, POLLIN | POLLPRI);
}


void commandSender::pollSuccess ()
{
        rts2core::Client::pollSuccess ();



//checkConnections(); //pollSuccess surekli calisiyor
}

void commandSender::processKey (int key)
{

}

int commandSender::init ()
{
        int ret;
        ret = rts2core::Client::init ();
        if (ret)
                return ret;
          




        rts2core::connections_t::iterator iter;
    
      
        for (iter = getCentraldConns ()->begin (); iter != getCentraldConns ()->end (); iter++)
                { 
                 // (*iter)->queCommand (new rts2core::Command (this, "move 10 10"));
                  std::cout<<"connection is "<<(*iter)->getName()<<std::endl;
                 //   std::cout<<"value size is  "<<(*iter)->valueSize()<<std::endl;

                 // rts2core::Value *valLng = (*iter)->getConnection()->getValue ("longitude");
                 //logStream(MESSAGE_INFO)<<valLng<<" bu logitude"<<sendLog;
                }
        
        setMessageMask (MESSAGE_MASK_ALL);
 



          for (iter = getConnections ()->begin (); iter != getConnections ()->end (); iter++)
        {
                std::cout<<"client connection is " <<std::endl;
        }
        

          for (rts2core::clients_t::iterator cli = this->getClients ()->begin (); cli != this->getClients ()->end (); cli++)
        {
        std::cout<<"client is ";//<<(*cli)->getName ()<<std::endl;
        }

  
         std::cout<<"init sonu"<<std::endl;

      return 0; 
}

rts2core::DevClient * commandSender::createOtherType (rts2core::Connection * conn, int other_device_type)
{
       //logStream(MESSAGE_INFO)<<"createother type calisti "<<conn->getName()<<sendLog; 
       rts2core::DevClient *retC = rts2core::Client::createOtherType (conn, other_device_type);
        if (other_device_type == DEVICE_TYPE_MOUNT && tarArg)
        {
                struct ln_equ_posn tarPos;
                tarArg->getSimbadPosition (&tarPos);
                conn->queCommand (new rts2core::CommandMove (this, (rts2core::DevClientTelescope *) retC, tarPos.ra, tarPos.dec));
        }
        // queue in commands
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
        getStatesTest(conn); 
        
        return retC;
}


rts2core::ConnClient * commandSender::createClientConnection (int _centrald_num, char *_deviceName)
{
      //  logStream(MESSAGE_INFO)<<"createClientConnection calisti"<<_deviceName<<sendLog;
        return new commandSenderConn (this, _centrald_num, _deviceName);
}


void commandSender::commandReturn (rts2core::Command * cmd, int cmd_status)
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



void commandSender::getStatesTest(rts2core::Connection * conn)
{

logStream(MESSAGE_INFO)<<"get name calisti"<<conn->getName()<<sendLog;

logStream(MESSAGE_INFO)<<"getstate  calisti"<<conn->getState()<<" hex is " <<std::hex<<conn->getState()<<sendLog;

logStream(MESSAGE_INFO)<<"isGoodWeather  calisti"<<conn->isGoodWeather()<<sendLog;

logStream(MESSAGE_INFO)<<"isIdle  calisti"<<conn->isIdle()<<sendLog;

logStream(MESSAGE_INFO)<<"canMove  calisti"<<conn->canMove()<<sendLog;

logStream(MESSAGE_INFO)<<"getErrorState  calisti"<<conn->getErrorState()<<" hex is " <<std::hex<<conn->getErrorState()<<sendLog;

logStream(MESSAGE_INFO)<<"getbopstate  calisti"<<conn->getBopState()<<" hex is " <<std::hex<<conn->getBopState()<<sendLog;

logStream(MESSAGE_INFO)<<"getfullbop  calisti"<<conn->getFullBopState()<<" hex is " <<std::hex<<conn->getFullBopState()<<sendLog;

logStream(MESSAGE_INFO)<<"getrealstate  calisti"<<conn->getRealState()<<" hex is " <<std::hex<<conn->getRealState()<<sendLog;

logStream(MESSAGE_INFO)<<"getstate string calisti"<<conn->getStateString()<<sendLog;


}




int main (int argc, char **argv)
{
        commandSender monitor = commandSender (argc, argv);
        return monitor.run ();
}

