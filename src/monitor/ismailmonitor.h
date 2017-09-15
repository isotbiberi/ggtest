#include "block.h"
#include "client.h"
#include "command.h"


class NMonitor:public rts2core::Client
{
        public:
                NMonitor (int argc, char **argv);
                virtual ~ NMonitor (void);

                virtual int init ();
                virtual void postEvent (rts2core::Event *event);

                virtual rts2core::ConnClient *createClientConnection (int _centrald_num, char *_deviceName);
                virtual rts2core::DevClient *createOtherType (rts2core::Connection * conn, int other_device_type);

                virtual int deleteConnection (rts2core::Connection * conn);

                virtual void deleteClient (int p_centraldId);
 vke sure that update of connection state is notified in monitor.
 */
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

/**
 * Make sure that command end is properly reflected
 */
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
                                  virtual void message (rts2core::Message & msg);

                void resize ();

                void processKey (int key);

                void commandReturn (rts2core::Command * cmd, int cmd_status);

                virtual void addPollSocks ();
                virtual void pollSuccess ();

        protected:
                virtual int processOption (int in_opt);
                virtual int processArgs (const char *arg);

                virtual rts2core::ConnCentraldClient *createCentralConn ();

        private:
                WINDOW * cursesWin;
                Layout *masterLayout;
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
              


                 /**
                 * Display message box with OK button.
                 */
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
/**
 * Make sure that update of connection state is notified in monitor.
 */
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

/**
 * Make sure that command end is properly reflected
 */
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
