#include<iostream>
#include<iostream>
#include "block.h"
#include "client.h"
#include "command.h"


using namespace std;
class Rts2IsmailConnection:public rts2core::ConnClient
{
        private:
                void printStatus ();
                //Rts2IsmailMonitor * ismailMonitor;
   //           rts2core::Block * block;
   //           rts2core::connections_t *conns;

        public:
                Rts2IsmailConnection (rts2core::Block * _master, int _centrald_num, char *_name):rts2core::ConnClient (_master, _centrald_num, _name)
                {
                 std::cout<<"connection name is "<<_name<<endl;
                }
                virtual void commandReturn (rts2core::Command * command, int cmd_status);

};


void Rts2IsmailConnection::printStatus()
{
cout<<"print status calisti";

}

void Rts2IsmailConnection::commandReturn (rts2core::Command * cmd, int cmd_status)
{
cout<<"command return calisti";
}

class CommandInfo:public rts2core::Command
{

        public:
                CommandInfo (rts2core::Block * in_owner):rts2core::Command (in_owner, "info")
                {
                }
                virtual int commandReturnOK (rts2core::Connection * conn)
                {
                        owner->queAll ("base_info");
                        owner->queAll ("info");
                        return rts2core::Command::commandReturnOK (conn);
                }
};



class Rts2IsmailMonitor:public rts2core::Client
{
        protected:
                virtual rts2core::ConnClient * createClientConnection (char *_deviceName);
//               virtual rts2core::DevClient *createOtherType (rts2core::Connection * conn, int other_device_type);
 //              virtual rts2core::ConnCentraldClient *createCentralConn ();
                rts2core::Block * block;
                rts2core::connections_t *conns;


        public:
                Rts2IsmailMonitor (int in_argc, char **in_argv):rts2core::Client (in_argc, in_argv, "ismailmonitor") {};
                virtual int idle ();
                virtual int run ();
                virtual int init();
};

/*
rts2core::ConnCentraldClient * Rts2IsmailMonitor::createCentralConn ()
{
        return new CentralConn (this, getCentralLogin (), getName (), getCentralPassword (), getCentralHost (), getCentralPort ());
}
*/
int Rts2IsmailMonitor::init()
{
 cout<<"init calisti"<<endl;

 int ret;
        ret = rts2core::Client::init ();
        if (ret)
             {  cout<<"init edemedim sanirim"; return ret;}
  conns = getConnections();
  cout<<"size of conns is " <<conns->size()<<endl; 
  cout<<"connection size is "<<connectionSize()<<endl;
  rts2core::connections_t::iterator iter;
  cout<<"number of centrald connection is " <<getCentraldConns ()->size ()<<endl;
 for (iter = getCentraldConns ()->begin (); iter != getCentraldConns ()->end (); iter++)
        {
                //logStream(MESSAGE_INFO)<<"connection calisti"<<sendLog;
                cout<<"init icerisindeki connection calisti"<<endl;
                cout<<"connectin getname is " <<(*iter)->getName ()<<"connection getstateString is "<<(*iter)->getStateString ().c_str ()<<"connection getstate is "<<(*iter)->getState ();

                //LogStream(MESSAGE_INFO)<<"connection is "<<endl;
                //(*iter)->queCommand (new CommandInfo (this));
        }
cout<<"init sona erdi"<<endl;
return 0;
}

rts2core::ConnClient * Rts2IsmailMonitor::createClientConnection (char *_deviceName)
{
        cout<<"device name is "<<_deviceName;
        rts2core::NetworkAddress *addr = findAddress (_deviceName);
        if (addr == NULL)
                return NULL;
        return new Rts2IsmailConnection (this, addr->getCentraldNum (), _deviceName);
}


int Rts2IsmailMonitor::idle ()
{
        if (commandQueEmpty ())
                endRunLoop ();
        return rts2core::Client::idle ();
}


int Rts2IsmailMonitor::run ()
{
        cout<<"run calisti"<<endl;
        //logStream(MESSAGE_INFO)<<"run calisti"<<sendLog;
        rts2core::connections_t::iterator iter;
        for (iter = getCentraldConns ()->begin (); iter != getCentraldConns ()->end (); iter++)
        {
                logStream(MESSAGE_INFO)<<"connection calisti"<<sendLog;
                cout<<"connection calisti";
                //LogStream(MESSAGE_INFO)<<"connection is "<<endl;
                (*iter)->queCommand (new CommandInfo (this));
        }
        return rts2core::Client::run ();
}





int main(int argc, char **argv)
{
//std::cout<<"ismail"<<std::endl;
Rts2IsmailMonitor mon = Rts2IsmailMonitor(argc,argv);

return mon.run();
}
