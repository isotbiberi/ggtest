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


#define WIDTH 30
#define HEIGHT 10
int startx =0;
int starty =0;
char *choices[] = {
                        "Devices",
                        "Commands",
                        "Values",
                        "Logs",
                        "Exit",
                  };
int n_choices = sizeof(choices) / sizeof(char *);

      
class NMonitor:public rts2core::Client
{
        public:
                
                //rts2core::Connection *connection;
                NMonitor (int argc, char **argv);
                virtual ~ NMonitor (void);

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

               
                WINDOW * cursesWin;
                WINDOW *menu_win;
                WINDOW *DevicesWindow;
                WINDOW *ValuesWindow;
 		WINDOW *CommandsWindow;
		WINDOW *LogsWindow;
                int selection = -1;
                int maxRow;
                int xpos=0;
                //Layout *masterLayout;
                /*
                LayoutBlock *daemonLayout;
                NDevListWindow *deviceList;
                NWindow *daemonWindow;
                NMsgWindow *msgwindow;
                NComWin *comWindow;
                NMenu *menu;

                NMsgBox *msgBox;

                NStatusWindow *statusWindow;

                rts2core::Command *oldCommand;

                std::list < NWindow * >windowStack;

                int cmd_col;
                char cmd_buf[CMD_BUF_LEN];

                int repaint ();

                messageAction msgAction;

                bool colorsOff;

                void showHelp ();
                */
                /**
                 * Display message box with OK button.
                 */
               
               /*
                void messageBox (const char *text);
                void messageBox (const char *query, messageAction action);
                void messageBoxEnd ();
                void menuPerform (int code);
                void leaveMenu ();
                                        
                       void changeActive (NWindow * new_active);
                void changeListConnection ();

                int old_lines;
                int old_cols;

                NWindow *getActiveWindow () { return *(--windowStack.end ()); }
                void setXtermTitle (const std::string &title) { std::cout << "\033]2;" << title << "\007"; }

                void sendCommand ();

                rts2core::SimbadTarget *tarArg;

                enum { ORDER_RTS2, ORDER_ALPHA } connOrder;
                bool hideDebugValues;
                NActionBool *hideDebugMenu;

                rts2core::connections_t orderedConn;

                void refreshConnections ();
               
                 */
                  /**
                 * Return connection at given number.
                 *
                 * @param i Number of connection which will be returned.
                 *
                 * @return NULL if connection with given number does not exists, or @see rts2core::Connection reference if it does.
                 */
                rts2core::Connection *connectionAt (unsigned int i);

                double refresh_rate;

                std::map <std::string, std::list <std::string> > initCommands;

                   
};

void NMonitor::message (rts2core::Message & msg)
{
        logStream(MESSAGE_INFO)<<"Message info is " << msg;
}

void NMonitor::checkConnections ()
 {
         rts2core::connections_t::iterator iter;
 
       //logStream(MESSAGE_INFO)<<"check connections calisti " <<sendLog;
       //orderedConn = *getConnections();
       int x=2;
       int y =5;
       int a=2;
       int b =5;
       int selectionNumber=0;
       //orderedConn = *getConnections();
       //std::cout<<"orderedconn name is "<<orderedConn.size();
       //connection = orderedConn[0];
       //for (rts2core::ValueVector::iterator iter = connection->valueBegin (); iter != connection->valueEnd (); iter++)
       // {
           
       // mvwprintw(ValuesWindow, y, x,"%s", (*iter)->getName ().c_str ());
      //  b++;  
        
   /*
                if (hide_debug == false || (*iter)->getDebugFlag () == false)
                {
                        maxrow++;
                        printValue (*iter);
                        displayValues.push_back (*iter);
                }
          */
      //  }
      //  wrefresh(ValuesWindow);
       //std::cout<<"orderedConnsize is "<<orderedConn.size()<<std::endl;
       maxRow = getConnections()->size();
       //std::cout<<"maxrow is "<<maxRow;
        for (iter = getConnections ()->begin (); iter != getConnections ()->end (); iter++)
        {
              std::cout<<"client connection is " <<(*iter)->getName()<<std::endl;
            
             if(selectionNumber==selection)
            {
             wattron(DevicesWindow, A_REVERSE);
             mvwprintw(DevicesWindow, y, x,"%s", (*iter)->getName());
             wattroff(DevicesWindow, A_REVERSE);
            }
            else
             mvwprintw(DevicesWindow, y, x,"%s", (*iter)->getName());
  
          //    wrefresh(DevicesWindow);
            selectionNumber++;
             y++;
        }
 wrefresh(DevicesWindow);

orderedConn = *getConnections();


if(orderedConn.size()>0)
{
connection = orderedConn[0];
       for (rts2core::ValueVector::iterator iter = connection->valueBegin (); iter != connection->valueEnd (); iter++)
        {

        mvwprintw(ValuesWindow, b, x,"%s", (*iter)->getName ().c_str ());
        b++;  

   /*
                if (hide_debug == false || (*iter)->getDebugFlag () == false)
                {
                        maxrow++;
                        printValue (*iter);
                        displayValues.push_back (*iter);
                }
          */
        }
        wrefresh(ValuesWindow);


}

//std::cout<<"orderedconn name is "<<orderedConn[0]->getName()<<std::endl;


}
void NMonitor::sendCommand()
{

//std::cout<<"sendCommand calisti";
//wmove (CommandsWindow, 0, 0);
xpos=getcurx(CommandsWindow);
//std::cout<<"x position is "<<xpos;
char command[xpos+1];
char *cmd_top = command;
mvwinnstr (CommandsWindow, 0, 0, command, xpos);
command[xpos]='\0';
//std::cout<<"command is "<<command<<"cmd_top is "<<cmd_top<<"connection is "<<orderedConn[0]->getName();
//mvwprintw(LogsWindow, 0, 0,"%s",command);
werase(LogsWindow);
mvwprintw(LogsWindow, 7, 10,"%s",cmd_top);
mvwprintw(LogsWindow, 5, 10,"%s","xpos is" );
mvwprintw(LogsWindow, 5, 25,"%d",xpos );
wrefresh(LogsWindow);
orderedConn[0] = NULL;
orderedConn =*getConnections();
orderedConn[0]->queCommand(new rts2core::Command(this,cmd_top));
werase(CommandsWindow);
wrefresh(CommandsWindow);
}
void NMonitor::changeSelectedRow(int i)
{
if(selection>=0) selection =selection+i;
//std::cout<<"selection is " <<selection<<std::endl;
}

/*
void NMonitor::print_menu(WINDOW *menu_win, int highlight)
{
        int x, y, i;

        x = 2;
        y = 2;
        box(menu_win, 0, 0);
        for(i = 0; i < n_choices; ++i)
        {       if(highlight == i + 1) 
                {       wattron(menu_win, A_REVERSE);
                        mvwprintw(menu_win, y, x, "%s", choices[i]);
                        wattroff(menu_win, A_REVERSE);
                }
                else
                        mvwprintw(menu_win, y, x, "%s", choices[i]);
                ++y;
        }
        wrefresh(menu_win);
}
*/


void NMonitor::postEvent (rts2core::Event *event)
{

/*
        switch (event->getType ())
        {
                case EVENT_MONITOR_REFRESH:
                        repaint ();
                        if (!std::isnan (refresh_rate) && refresh_rate >= 0)
                                addTimer (refresh_rate, event);
                        return;
        }
*/

logStream(MESSAGE_INFO)<<"postevent calisti"<<sendLog;  
      rts2core::Client::postEvent (event);
}


class NMonConn:public rts2core::ConnClient
	{
        public:
                NMonConn (NMonitor * _master, int _centrald_num, char *_name):rts2core::ConnClient (_master, _centrald_num, _name)
                {
                        master = _master;
                }

                virtual void commandReturn (rts2core::Command * cmd, int in_status)
                {
                        master->commandReturn (cmd, in_status);
                        return rts2core::ConnClient::commandReturn (cmd, in_status);
                }
        private:
                NMonitor * master;
};
class NMonCentralConn:public rts2core::ConnCentraldClient
{
        public:
                NMonCentralConn (NMonitor * in_master,
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
                NMonitor * master;
};

rts2core::ConnCentraldClient * NMonitor::createCentralConn ()
{
        return new NMonCentralConn (this, getCentralLogin (), getName (), getCentralPassword (), getCentralHost (), getCentralPort ());
}


NMonitor::NMonitor (int in_argc, char **in_argv):rts2core::Client (in_argc, in_argv, "testClientConnection")
{
connection= NULL;
   /*     masterLayout = NULL;
        daemonLayout = NULL;
        statusWindow = NULL;
        deviceList = NULL;
        daemonWindow = NULL;
        comWindow = NULL;
        menu = NULL;
        msgwindow = NULL;
        msgBox = NULL;
        cmd_col = 0;

        oldCommand = NULL;

        colorsOff = false;

        old_lines = 0;
        old_cols = 0;

        connOrder = ORDER_ALPHA;
        hideDebugValues = true;
        hideDebugMenu = NULL;

        rts2core::Configuration::instance ()->setShowMilliseconds (false);

#ifdef RTS2_HAVE_PGSQL
        tarArg = NULL;
#endif
        addOption (OPT_MONITOR_SHOW_DEBUG, "show-debug", 0, "show debug values");
                                                      369,1-8       49%
 addOption ('c', NULL, 0, "don't use colors");
        addOption ('r', NULL, 1, "refersh rate (in seconds)");
        addOption (OPT_MONITOR_COMMAND, "command", 1, "send command to device; separate command and device with .");
        addOption (OPT_MILISEC, "show-milliseconds", 0, "show milliseconds in time differences");
        addOption (OPT_NORTH_ZERO, "north-0", 0, "show azimuth starting from north (N=0, E=90,..)");

        char buf[HOST_NAME_MAX];

        gethostname (buf, HOST_NAME_MAX);

        std::ostringstream _os;
        _os << "rts2-mon@" << buf;

        setXtermTitle (_os.str ());

        refresh_rate = MONITOR_REFRESH;
*/
}

NMonitor::~NMonitor (void)
{
/*  
      erase ();
        refresh ();

        nocbreak ();
        echo ();
        endwin ();
  delete msgBox;
        delete statusWindow;

        delete masterLayout;

#ifdef RTS2_HAVE_PGSQL
        delete tarArg;
#endif
*/
}



void NMonitor::addPollSocks ()
{


        //logStream(MESSAGE_INFO)<<"addpollsocks calisti"<<sendLog;
        rts2core::Client::addPollSocks ();
        // add stdin for ncurses input
        addPollFD (1, POLLIN | POLLPRI);
}


void NMonitor::pollSuccess ()
{
        rts2core::Client::pollSuccess ();
        //logStream(MESSAGE_INFO)<<"poll success  calisti"<<sendLog;

//int k = 9;
//processKey(k);


        while (1)
        {
                //logStream(MESSAGE_INFO)<<"error deil"<<sendLog;

                int input = getch ();
                //logStream(MESSAGE_INFO)<<"input is "<<input<<sendLog;

                if (input == ERR)
                        break;
               // else logStream(MESSAGE_INFO)<<"error deil"<<sendLog;
                processKey (input);
                //waddch(CommandsWindow,input);
                //wrefresh(CommandsWindow);
        }
checkConnections();
}

void NMonitor::processKey (int key)
{
//logStream(MESSAGE_INFO)<<"process key calisti"<<key<<sendLog;
//std::cout<<"hbele";
//char *buf="iso";
//std::cout<<buf;

switch (key)
        {
                case '\t':
                case KEY_STAB:
                case KEY_DOWN:    
                                if(selection==-1) selection=0;
                                if(selection<8)changeSelectedRow(+1);
                                break;
                case KEY_UP:
                                
                                if(selection>0)changeSelectedRow(-1);
				sendCommand();
                                break;


                case KEY_F(10):
                sendCommand();
                break;
                default:
                waddch (CommandsWindow, key);
                wrefresh(CommandsWindow);
                //if (key == '\n')
                //sendCommand ();
        
         }


//int n = 10;
//mvwinnstr(cursesWin,0,0,buf,199);
//std::cout<<"buffer is "<<buf<<std::endl;

}

int NMonitor::init ()
{
        int ret;
        ret = rts2core::Client::init ();
        if (ret)
                return ret;
          


           // init ncurses
        cursesWin = initscr ();
        if (!cursesWin)
        {
                std::cerr << "ncurses not supported (initscr call failed)!" << std::
                        endl;
                return -1; 
        }       
                
        cbreak ();
        noecho ();
        nonl ();
        intrflush (stdscr, FALSE);
        keypad (stdscr, TRUE);
        timeout (0);
/*
        startx = (80 - WIDTH) / 2;
        starty = (24 - HEIGHT) / 2;
        //WINDOW *menu_win;
        int highlight = 1;
        int choice = 0;
        int c;

        menu_win = newwin(HEIGHT, WIDTH, starty, startx);
        keypad(menu_win, TRUE);
        mvprintw(0, 0, "Use arrow keys to go up and down, Press enter to select a choice");
        refresh();
        //print_menu(menu_win, highlight);
        
        //int a,b;
        //getyx(menu_win,y,x);
        //x+=getX();
	//y+=getY();
        setsyx(0,0);
        //wmove(menu_win, 0, 0);
        //refresh();
      
*/
          refresh();

         DevicesWindow=newwin(40, 20, 5,5);
         keypad(DevicesWindow, TRUE);
         box(DevicesWindow,0,0); 
         wrefresh(DevicesWindow);
         wattron(DevicesWindow, A_REVERSE);
         mvwprintw(DevicesWindow, 2, 2, "%s",choices[0]);
         wattroff(DevicesWindow, A_REVERSE);
         wrefresh(DevicesWindow);
        //refresh();
      

         CommandsWindow=newwin(5, 50, 40,25);
         keypad(CommandsWindow,TRUE);
         box(CommandsWindow,0,0);
         wrefresh(CommandsWindow);
         wattron(CommandsWindow, A_REVERSE);
         mvwprintw(CommandsWindow, 2, 2, "%s",choices[1]);
         wattroff(CommandsWindow, A_REVERSE);
         wrefresh(CommandsWindow);

 
         ValuesWindow=newwin(30, 50,5,25);
         box(ValuesWindow,0,0);
         wrefresh(ValuesWindow);
         wattron(ValuesWindow, A_REVERSE);
         mvwprintw(ValuesWindow, 2, 2, "%s",choices[2]);
         wattroff(ValuesWindow, A_REVERSE);
         wrefresh(ValuesWindow);

       
         LogsWindow=newwin(10,70,48,5);
         box(LogsWindow,0,0);
         wrefresh(LogsWindow);
         wattron(LogsWindow, A_REVERSE);
         mvwprintw(LogsWindow, 2, 2, "%s",choices[3]);
         wattroff(LogsWindow, A_REVERSE);
         wrefresh(LogsWindow);
         wmove (CommandsWindow, 0, 0);
 

          //rts2core::connections_t orderedConn; 
          //orderedConn = *getConnections();
          //logStream(MESSAGE_INFO)<<"size of ordered con is " <<orderedConn.size()<<sendLog;


          //std::cout<<"value öncesi";
         // connection=*(getCentraldConns ()->begin ());
          //rts2core::Value *valLng = connection->getValue ("longitude");
          //rts2core::Value *valLat = getConnection ()->getValue ("latitude");
          //std::cout<<"value is "<<valLng->getValueDouble();      


        rts2core::connections_t::iterator iter;
    
      
        for (iter = getCentraldConns ()->begin (); iter != getCentraldConns ()->end (); iter++)
                { 
                  (*iter)->queCommand (new rts2core::Command (this, "move 10 10"));
                 // std::cout<<"connection is "<<(*iter)->getName()<<std::endl;
                 //   std::cout<<"value size is  "<<(*iter)->valueSize()<<std::endl;

                 // rts2core::Value *valLng = (*iter)->getConnection()->getValue ("longitude");
                 //logStream(MESSAGE_INFO)<<valLng<<" bu logitude"<<sendLog;
                }
        
        setMessageMask (MESSAGE_MASK_ALL);
 

/*

          for (iter = getConnections ()->begin (); iter != getConnections ()->end (); iter++)
        {
                std::cout<<"client connection is " <<std::endl;
        }
        

          for (rts2core::clients_t::iterator cli = this->getClients ()->begin (); cli != this->getClients ()->end (); cli++)
        {
        std::cout<<"client is ";//<<(*cli)->getName ()<<std::endl;
        }

  */
         std::cout<<"init sonu"<<std::endl;

      return 0; 
}

rts2core::DevClient * NMonitor::createOtherType (rts2core::Connection * conn, int other_device_type)
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
        std::map <std::string, std::list <std::string> >::iterator iter = initCommands.find (conn->getName ());
        if (iter != initCommands.end ())
        {
                for (std::list <std::string>::iterator ci = iter->second.begin (); ci != iter->second.end (); ci++)
                        conn->queCommand (new rts2core::Command (this, ci->c_str ()));
        }
        return retC;
}


rts2core::ConnClient * NMonitor::createClientConnection (int _centrald_num, char *_deviceName)
{
        //logStream(MESSAGE_INFO)<<"createClientConnection calisti"<<_deviceName<<sendLog;
        return new NMonConn (this, _centrald_num, _deviceName);
}


void NMonitor::commandReturn (rts2core::Command * cmd, int cmd_status)
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








int main (int argc, char **argv)
{
        NMonitor monitor = NMonitor (argc, argv);
        return monitor.run ();
}

