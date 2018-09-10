/*=============================================================================
  Copyright (C) 2012 - 2016 Allied Vision Technologies.  All Rights Reserved.

  Redistribution of this file, in original or modified form, without
  prior written consent of Allied Vision Technologies is prohibited.

-------------------------------------------------------------------------------

  File:        MainWindow.cpp

  Description: The main window framework. This contains of camera tree, a toolbar and logging
               

-------------------------------------------------------------------------------

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF TITLE,
  NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A PARTICULAR  PURPOSE ARE
  DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
  AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
  TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=============================================================================*/

#include "MainWindow.h"
#include "CameraObserver.h"
#include "SplashScreen.h"
#include <QSplitter>
#include "Version.h"
#include <algorithm>
#include <QRegExp>

using AVT::VmbAPI::ICameraListObserverPtr;
using AVT::VmbAPI::InterfacePtrVector;
using AVT::VmbAPI::StringVector;

MainWindow::MainWindow ( QWidget *parent, Qt::WindowFlags flags )
    : QMainWindow( parent, flags )
    , m_bIsCurrentModelChecked ( false )
    , m_Logger( NULL )
    , m_VimbaSystem( AVT::VmbAPI::VimbaSystem::GetInstance() )
    , m_bIsRightMouseClicked( false )
    , m_nOpenState( 0 )
    , m_bIsOpenByRightMouseClick( false )
    , m_bIsAutoAdjustPacketSize ( false )
    , m_bIsInitialized (false)
    , m_ActionCommandDialog( NULL )
    , m_ActionCommandFlag( false )
{
    /* create the main window */
    m_MainWindow.setupUi(this);
    connect(m_MainWindow.actionAbout, SIGNAL(triggered()), this, SLOT(about()));
    connect(m_MainWindow.actionAbout_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(m_MainWindow.actionExit, SIGNAL(triggered()), this, SLOT(close()));
    /* create splitter 
    *     _______________
    *    |       |       |
    *    |       |       |
    *    |       |       |
    *    |_______|_______|
    */

    QSplitter *splitter_H = new QSplitter(Qt::Horizontal, m_MainWindow.centralWidget);    
    splitter_H->setChildrenCollapsible(false);
    
    m_CameraTree = new CameraTreeWindow(splitter_H);
    connect(m_CameraTree, SIGNAL(cameraClicked(const QString &, const bool &)), this, SLOT(onCameraClicked(const QString &, const bool &)));
    connect(m_CameraTree, SIGNAL(rightMouseClicked(const bool &)), this, SLOT(onRightMouseClicked(const bool &)));

    m_Logger = new LoggerWindow(splitter_H);
    m_MainWindow.layout_H->addWidget(splitter_H);

    /* use setting position and geometry from the last session*/
    QSettings settings("Allied Vision Technologies", "Vimba Viewer");
    this->restoreGeometry(settings.value("geometrymainwindow").toByteArray());
    this->restoreState(settings.value("state").toByteArray(), 0);

    /* get VmbAPI version */
    VmbVersionInfo_t version = { 0, 0, 0 };
    VmbErrorType error = m_VimbaSystem.QueryVersion( version );
    m_sAPIVersion = "Vimba API Version: " + QString::number(version.major) + "." + QString::number(version.minor) + "." + QString::number(version.patch);
    ( VmbErrorSuccess != error )    ? m_Logger->logging(tr("QueryVersion() failed, Error: ") + QString::number(error) + " " + m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR)
                                    : m_Logger->logging(m_sAPIVersion, VimbaViewerLogCategory_INFO);

    /* OpenByID */
    m_OpenByIDDialog = new QDialog( this, windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinimizeButtonHint & ~Qt::WindowMaximizeButtonHint); 
    m_OpenByID.setupUi( m_OpenByIDDialog );

    /*Start Option (auto adjust packet size) */
    m_StartOptionDialog = new QDialog(this, windowFlags() & ~Qt::WindowContextHelpButtonHint & ~Qt::WindowMinimizeButtonHint & ~Qt::WindowMaximizeButtonHint);
    m_StartOption.setupUi(m_StartOptionDialog);
    m_MainWindow.ActionStartOptions->setEnabled(false);
    /* read the last saved value */
    QVariant result = settings.value("autoenable", true);
    m_bIsAutoAdjustPacketSize = result.toBool();
    m_StartOption.AutoAdjustPacketSize_CheckBox->setChecked(m_bIsAutoAdjustPacketSize); 

}

MainWindow::~MainWindow ( void )
{
    /* save setting position and geometry */
    QSettings settings("Allied Vision Technologies", "Vimba Viewer");
    settings.setValue("geometrymainwindow", saveGeometry());
    settings.setValue("state", saveState(0));
    settings.setValue("autoenable", m_bIsAutoAdjustPacketSize);

    /* delete Action Command dialog*/
    if( NULL != m_ActionCommandDialog )
    {
        delete m_ActionCommandDialog;
    }

    /* release API */
    m_VimbaSystem.Shutdown();
}

void MainWindow::about ( void )
{
    QString sAbout(tr("Vimba Viewer is an example application using the Vimba C++ API.") + QString("\n\n") + tr("Version: ") + QString(VIMBAVIEWER_VERSION) + QString("\n"));
    sAbout.append(m_sAPIVersion).append("\n").append("\nCopyright (c) Allied Vision Technologies 2012 - 2017");
    QMessageBox::about(this, tr("About Vimba Viewer"), sAbout);
}

/* this will be called on destruction of main window in case at least one viewer window is open */
void MainWindow::closeEvent ( QCloseEvent *event )
{
    if(!m_Viewer.empty())
    {
        QVector<ViewerWindow*>::iterator it = m_Viewer.begin();
        while(it != m_Viewer.end())
        {
            delete m_Viewer[0];
            m_Viewer[0] = NULL;
            m_Viewer.erase(it);
        }
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    QMainWindow::showEvent(event);

    if (!m_bIsInitialized)
    {
        QTreeWidgetItem* itemRoot = new QTreeWidgetItem(m_CameraTree);
        m_CameraTree->setText(itemRoot, "Please wait...");
        m_CameraTree->setEnabled(false);
                
        QTimer::singleShot(50, this, SLOT(onInitializeVimba()));        /* This slot will be called AFTER the main window is shown. This is needed, since the enumeration of CL cameras might take a while and it should not prevent the main window from being initially drawn */
    }
}

void MainWindow::onInitializeVimba()
{
    VmbErrorType error = m_VimbaSystem.Startup();
    if(VmbErrorSuccess != error)
    {
        m_Logger->logging(tr("Startup failed, Error: ")+QString::number(error)+" "+ m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR);
        return;
    }

    // get VimbaC feature to check if GigE TL is present
    AVT::VmbAPI::FeaturePtr gevFeature;
    error = m_VimbaSystem.GetFeatureByName( "GeVTLIsPresent", gevFeature );
    if( VmbErrorSuccess == error )
    {
        // check if GigE TL is present
        bool gevTLisPresent = false;
        error = gevFeature->GetValue( gevTLisPresent );
        if( (VmbErrorSuccess == error) && (true == gevTLisPresent) )
        {
            m_ActionCommandDialog = new ActionCommandDialog( m_VimbaSystem );
            m_ActionCommandFlag = true;
        }
    }

    /*    register the camera observer to get notification when camera list has been updated, e.g:
    *     - plug / unplug camera(s)
    *     - bus reset occurs 
    */
    CameraObserver *camObserver = new CameraObserver();
    ICameraListObserverPtr pDeviceObs(camObserver);
    
    error = m_VimbaSystem.RegisterCameraListObserver(pDeviceObs);
    if(VmbErrorSuccess == error)
    {
        connect(camObserver, SIGNAL(updateDeviceList( )), this, SLOT(onUpdateDeviceList()));
    }
    else
    {
        m_Logger->logging(tr("RegisterCameraListObserver Failed, Error: ")+QString::number(error)+" "+ m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR); 
    }

    try
    {
        searchCameras( );
       // m_CameraTree->expandAll();
    }
    catch(const std::exception &e)
    {
        m_Logger->logging("MainWindow <constructor> Exception: "+QString::fromStdString(e.what()), VimbaViewerLogCategory_ERROR); 
    }

    m_CameraTree->setEnabled(true);
    m_bIsInitialized = true;
}

void MainWindow::onUpdateDeviceList ( void )
{
    /*
    QPixmap pixmap( ":/VimbaViewer/Images/refresh.png" );
    SplashScreen splashScreen(pixmap, this, Qt::SplashScreen);
    int nW = ((this->width()/2) - splashScreen.width()/2);
    int nH = ((this->height()/2) - splashScreen.height()/2);
    splashScreen.setGeometry(nW,nH, splashScreen.width(),splashScreen.height());
    splashScreen.show();
    splashScreen.showMessage("Please wait..." , Qt::AlignHCenter | Qt::AlignVCenter, Qt::red);
    */
    std::vector<std::string> listOfDisconnects;
    CameraPtrVector currentListedCameras;
    
    VmbErrorType error;
    error = m_VimbaSystem.GetCameras(currentListedCameras);
    
    if(VmbErrorSuccess == error)
    {
        /* disconnect */
        if(currentListedCameras.size() < m_rCameras.size())
        {
            /* find out which camera is not available */
            for (unsigned int i = 0; i < m_rCameras.size(); i++)
            {
                std::string sCameraFromOldList;
                getCameraDisplayName(m_rCameras.at(i), sCameraFromOldList );

                for (unsigned int y = 0; y < currentListedCameras.size(); y++ )
                {
                    std::string sCameraFromCurrentList;
                    getCameraDisplayName(currentListedCameras.at(y), sCameraFromCurrentList );

                    if(0 == sCameraFromOldList.compare(sCameraFromCurrentList))
                    {
                        break;
                    }
                    else
                    {
                        listOfDisconnects.push_back(sCameraFromOldList);
                    }
                }
                
            }

            while(!listOfDisconnects.empty())
            {
                closeViewer ( QString::fromStdString(listOfDisconnects.back()) );
                listOfDisconnects.pop_back();
            }
        }
    }
    else
    {
        m_Logger->logging(tr("GetCameras Failed, Error: ")+QString::number(error)+" "+ m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR); 
    }

    //m_CameraTree->clear();
    m_rCameras.clear();
    searchCameras();
 //   m_CameraTree->expandAll();

    /* update tree */
    for(int i = 0; i<m_Viewer.size(); i++)
    {
        for(int x=0; x<m_GigE.size(); x++)
        {
            if(0 == m_GigE.at(x)->text(0).compare(m_Viewer.at(i)->getCameraID()))
                m_GigE.at(x)->setCheckState(0, Qt::Checked);
        }

        for(int x=0; x<m_1394.size(); x++)
        {
            if(0 == m_1394.at(x)->text(0).compare(m_Viewer.at(i)->getCameraID()))
                m_1394.at(x)->setCheckState(0, Qt::Checked);
        }

        for(int x=0; x<m_USB.size(); x++)
        {
            if(0 == m_USB.at(x)->text(0).compare(m_Viewer.at(i)->getCameraID()))
                m_USB.at(x)->setCheckState(0, Qt::Checked);
        }

        for(int x=0; x<m_CL.size(); x++)
        {
            if(0 == m_CL.at(x)->text(0).compare(m_Viewer.at(i)->getCameraID()))
                m_CL.at(x)->setCheckState(0, Qt::Checked);
        }
        
        for(int x=0; x<m_CSI2.size(); x++)
        {
            if(0 == m_CSI2.at(x)->text(0).compare(m_Viewer.at(i)->getCameraID()))
                m_CSI2.at(x)->setCheckState(0, Qt::Checked);
        }
    }
}

void MainWindow::searchCameras ( void )
{
    /* If update device list callback triggered: Recopy before clearing. If the viewer is already open, use the access list from CopyMap,
       this will avoid the change of menu access entry  info.
       Access information the viewer got when camera is opened will be kept.*/
    rightMouseMenu.close();
    m_CameraTree->clear();
    QMap <QString, QStringList> CamerasPermittedAccessCopyMap     = m_CamerasPermittedAccessMap; 
    QMap <QString, QStringList> PermittedAccessStateListCopyMap   = m_PermittedAccessStateListMap; 

    m_CamerasPermittedAccessMap.clear();
    m_PermittedAccessStateListMap.clear();
    
    QTreeWidgetItem *itemGigERoot = NULL;
    QTreeWidgetItem *item1394Root = NULL;
    QTreeWidgetItem *itemUSBRoot  = NULL;
    QTreeWidgetItem *itemCLRoot   = NULL;
    QTreeWidgetItem *itemCSI2Root = NULL;

    m_GigE.clear();
    m_1394.clear();
    m_USB.clear();
    m_CL.clear();
    m_CSI2.clear();

    QMap <QString, QString>    ifTypeMap;
    InterfacePtrVector ifPtrVec;
    std::string sInterfaceID;
    VmbErrorType error;

    /* list all interfaces found by VmbAPI */
    error = m_VimbaSystem.GetInterfaces(ifPtrVec);
    
    if(VmbErrorSuccess != error)
    {
        m_Logger->logging(tr("GetInterfaces Failed, Error: ")+QString::number(error)+" "+ m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR); 
        return;
    }

    /* check type of Interfaces */
    for(unsigned int i=0; i<ifPtrVec.size(); i++)
    {
        error = ifPtrVec.at(i)->GetID(sInterfaceID);
        if(VmbErrorSuccess != error)
        {
            m_Logger->logging("GetID <Interface "+ QString::number(i)+" Failed, Error: "+QString::number(error)+" "+ m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR); 
            continue;
        }
        
        VmbInterfaceType ifType = VmbInterfaceUnknown;
        VmbErrorType errorGetType = ifPtrVec.at(i)->GetType(ifType) ;
        if(VmbErrorSuccess != errorGetType)
        {
            m_Logger->logging("GetType <Interface "+ QString::number(i)+" Failed, Error: "+QString::number(errorGetType)+" "+ m_Helper.mapReturnCodeToString(errorGetType), VimbaViewerLogCategory_ERROR); 
            continue;
        }

        if((VmbErrorSuccess == errorGetType) && (VmbErrorSuccess == error))
        {
            switch(ifType)
            {
            case VmbInterfaceEthernet: ifTypeMap[QString::fromStdString(sInterfaceID)] = "GigE";
                if(NULL == itemGigERoot)
                {
                    itemGigERoot = new QTreeWidgetItem(m_CameraTree);
                    m_CameraTree->setText(itemGigERoot, "GigE");
                    m_MainWindow.ActionStartOptions->setEnabled(true);
                }
                break;

            case VmbInterfaceFirewire: ifTypeMap[QString::fromStdString(sInterfaceID)] = "1394";
                if(NULL == item1394Root)
                {
                    item1394Root = new QTreeWidgetItem(m_CameraTree);
                    m_CameraTree->setText(item1394Root, "1394");
                }
                break;

            case VmbInterfaceUsb: ifTypeMap[QString::fromStdString(sInterfaceID)] = "USB";
                if(NULL == itemUSBRoot)
                {
                    itemUSBRoot = new QTreeWidgetItem(m_CameraTree);
                    m_CameraTree->setText(itemUSBRoot, "USB");
                }
                break;

            case VmbInterfaceCL: ifTypeMap[QString::fromStdString(sInterfaceID)] = "CL";
                if(NULL == itemCLRoot)
                {
                    itemCLRoot = new QTreeWidgetItem(m_CameraTree);
                    m_CameraTree->setText(itemCLRoot, "CL");
                }
                break;

            case VmbInterfaceCSI2: ifTypeMap[QString::fromStdString(sInterfaceID)] = "CSI-2";
                if(NULL == itemCSI2Root)
                {
                    itemCSI2Root = new QTreeWidgetItem(m_CameraTree);
                    m_CameraTree->setText(itemCSI2Root, "CSI-2");
                }
                break;

            default: break;
            }
        }
    } /* for */

    /* sort the cameras */
    error = m_VimbaSystem.GetCameras(m_rCameras);
    if(VmbErrorSuccess != error)
    {
        m_Logger->logging(tr("GetCameras Failed, Error: ")+QString::number(error)+" "+ m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR); 
        return;
    }

    if(VmbErrorSuccess == error)
    {
        if(0 < m_rCameras.size())
            m_CameraTree->setCursor(Qt::PointingHandCursor);
        else
            m_CameraTree->setCursor(Qt::ArrowCursor);

        for(unsigned int i=0; i<m_rCameras.size(); i++)
        {
            error = m_rCameras.at(i)->GetInterfaceID(sInterfaceID);
            if(VmbErrorSuccess != error)
            {
                m_Logger->logging("GetInterfaceID <Camera "+ QString::number(i)+" Failed, Error: "+QString::number(error)+" "+ m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR); 
                continue;
            }

            /* get and sort the camera interface */
            QMapIterator <QString, QString> it(ifTypeMap);
            while(it.hasNext())
            {
                it.next();
                if( 0 == it.key().compare(QString::fromStdString(sInterfaceID)))
                {
                    std::string sCamID;
                    if ( VmbErrorSuccess != getCameraDisplayName(m_rCameras.at(i), sCamID))
                    {
                        break;
                    }

                    QString sAccess;
                    QStringList sListPermittedAccess;
                    QStringList sListPermittedAccessState;

                    /* Check if viewer already open */
                    bool bIsopen = false;
                    
                    QMap <QString, QString>::iterator itOpenstate;
                    for(itOpenstate = m_IsCameraOpenMap.begin(); itOpenstate != m_IsCameraOpenMap.end(); ++itOpenstate) 
                    {
                        if (0 == itOpenstate.key().compare(QString::fromStdString(sCamID)))
                        {
                            bIsopen = true;
                            break;
                        }
                    }

                    /*
                    for (unsigned int nPos=0; nPos<m_Viewer.size(); nPos++)
                    {
                        if( 0 == m_Viewer.at(nPos)->getCameraID().compare(QString::fromStdString(sCamModel)) )
                        {
                            bIsopen = true;
                            break;
                        }
                    }
                    */

                    /* camera is open, so keep the information */
                    if(bIsopen)
                    {
                        /* keep the last information */
                        sListPermittedAccess      = getStringListInfo(CamerasPermittedAccessCopyMap   , QString::fromStdString(sCamID));
                        sListPermittedAccessState = getStringListInfo(PermittedAccessStateListCopyMap , QString::fromStdString(sCamID));
                        
                        if(sListPermittedAccess.contains(tr("Open FULL ACCESS")) && sListPermittedAccess.contains(tr("Open READ ONLY")))
                        {
                            sAccess = "FULL ACCESS";
                        }
                        else if(sListPermittedAccess.contains(tr("Open READ ONLY")))
                        {
                            sAccess = "READ ONLY";
                        }
                    }
                    else /* camera is not open, so get the current supported access */ 
                    {
                        /* check access */
                        VmbAccessModeType accessType = VmbAccessModeFull;
                        if( VmbErrorSuccess == m_rCameras.at(i)->GetPermittedAccess( accessType ))
                        {
                            if( accessType & VmbAccessModeFull)
                            {
                                sAccess = "FULL ACCESS";
                                if(0 == it.value().compare("GigE"))
                                {
                                    sListPermittedAccess      << tr("Open FULL ACCESS") << tr("Open READ ONLY") << tr("Open CONFIG");
                                    sListPermittedAccessState << "false"            << "false"          << "false";
                                }
                                else if(0 == it.value().compare("1394"))
                                {
                                    sListPermittedAccess      << tr("Open FULL ACCESS") << tr("Open READ ONLY") ;
                                    sListPermittedAccessState << "false"            << "false"          ;
                                }
                                else if(0 == it.value().compare("USB"))
                                {
                                    sListPermittedAccess      << tr("Open FULL ACCESS") << tr("Open READ ONLY") ;
                                    sListPermittedAccessState << "false"            << "false"          ;
                                }
                                else if(0 == it.value().compare("CL"))
                                {
                                    sListPermittedAccess      << tr("Open FULL ACCESS");
                                    sListPermittedAccessState << "false";
                                }   
                                else if(0 == it.value().compare("CSI-2"))
                                {
                                    sListPermittedAccess      << tr("Open FULL ACCESS");
                                    sListPermittedAccessState << "false";
                                }
                            }
                            else if( accessType & VmbAccessModeRead)
                            {
                                sAccess = "READ ONLY";
                                if(0 == it.value().compare("GigE"))
                                {
                                    //sListPermittedAccess      << "Open READ ONLY" << "Open CONFIG";
                                    sListPermittedAccess      << tr("Open READ ONLY");
                                    sListPermittedAccessState << "false"          ;//<< "false";
                                }
                                else if(0 == it.value().compare("1394"))
                                {
                                    sListPermittedAccess      << tr("Open READ ONLY") ;
                                    sListPermittedAccessState << "false"          ;
                                }
                                else if(0 == it.value().compare("USB"))
                                {
                                    sListPermittedAccess      << tr("Open READ ONLY") ;
                                    sListPermittedAccessState << "false"          ;
                                }
                            }
                            else if( accessType & VmbAccessModeConfig)
                            {
                                sAccess = "DEVICE CONFIGURATION";
                                if(0 == it.value().compare("GigE"))
                                {
                                    sListPermittedAccess << tr("Open CONFIG") ;
                                    sListPermittedAccessState << "false"  ;
                                }
                            }
                            else if( accessType & VmbAccessModeLite)
                            {
                                sAccess = "RW WITH NO FEATURE ACCESS";
                                sListPermittedAccess << tr("Open LITE");
                                sListPermittedAccessState << "false";
                            }
                            else
                            {
                                sAccess = "NO ACCESS";
                            }
                        }
                    }

                    m_CamerasPermittedAccessMap  [QString::fromStdString(sCamID)]   = sListPermittedAccess;
                    m_PermittedAccessStateListMap[QString::fromStdString(sCamID)]   = sListPermittedAccessState;

                    if(0 == it.value().compare("GigE"))
                    {
                        QTreeWidgetItem *itemIF_GigE_ID = m_CameraTree->createItem(itemGigERoot, true);
                        itemIF_GigE_ID->setText(0, QString::fromStdString(sCamID) );
                        itemIF_GigE_ID->setIcon(0, QIcon(":/VimbaViewer/Images/stripes_256.png"));
                        if( sAccess == "READ ONLY" || sAccess== "NO ACCESS" )
                        {
                            itemIF_GigE_ID->setBackgroundColor(0, Qt::black);
                            itemIF_GigE_ID->setForeground(0, Qt::white);
                            itemIF_GigE_ID->setIcon(0, QIcon(":/VimbaViewer/Images/lock.png"));
                            if(sAccess =="NO ACCESS")
                            {
                                itemIF_GigE_ID->setDisabled(true);
                            }
                        }
                        if(bIsopen)
                        {
                            itemIF_GigE_ID->setCheckState(0, Qt::Checked);
                        }

                        m_GigE.append(itemIF_GigE_ID);
                    }
                    
                    if(0 == it.value().compare("1394"))
                    {
                        QTreeWidgetItem *itemIF_1394_ID = m_CameraTree->createItem(item1394Root, true);
                        itemIF_1394_ID->setText(0, QString::fromStdString(sCamID) );
                        itemIF_1394_ID->setIcon(0, QIcon(":/VimbaViewer/Images/stripes_256.png"));
                        if( sAccess == "READ ONLY" || sAccess == "NO ACCESS" )
                        {
                            itemIF_1394_ID->setBackgroundColor(0, Qt::black);
                            itemIF_1394_ID->setForeground(0, Qt::white);
                            itemIF_1394_ID->setIcon(0, QIcon(":/VimbaViewer/Images/lock.png"));
                            if(sAccess == "NO ACCESS")
                            {
                                itemIF_1394_ID->setDisabled(true);
                            }
                        }
                        if(bIsopen)
                        {
                            itemIF_1394_ID->setCheckState(0, Qt::Checked);
                        }
                        
                        m_1394.append(itemIF_1394_ID);
                    }
                    
                    if(0 == it.value().compare("USB"))
                    {
                        QTreeWidgetItem *itemIF_USB_ID = m_CameraTree->createItem(itemUSBRoot, true);
                        itemIF_USB_ID->setText(0, QString::fromStdString(sCamID) );
                        itemIF_USB_ID->setIcon(0, QIcon(":/VimbaViewer/Images/stripes_256.png"));
                        if( sAccess == "READ ONLY" || sAccess == "NO ACCESS")
                        {
                            itemIF_USB_ID->setBackgroundColor(0, Qt::black);
                            itemIF_USB_ID->setForeground(0, Qt::white);
                            itemIF_USB_ID->setIcon(0, QIcon(":/VimbaViewer/Images/lock.png"));
                            if(sAccess == "NO ACCESS")
                            {
                                itemIF_USB_ID->setDisabled(true);
                            }
                        }
                        if(bIsopen)
                        {
                            itemIF_USB_ID->setCheckState(0, Qt::Checked);
                        }

                        m_USB.append(itemIF_USB_ID);
                    }

                    if(0 == it.value().compare("CL"))
                    {
                        QTreeWidgetItem *itemIF_CL_ID = m_CameraTree->createItem(itemCLRoot, true);
                        itemIF_CL_ID->setText(0, QString::fromStdString(sCamID) );
                        itemIF_CL_ID->setIcon(0, QIcon(":/VimbaViewer/Images/stripes_256.png"));
                        if(!bIsopen)
                        {
                            if( 0 == sAccess.compare("READ ONLY") || 0 == sAccess.compare("NO ACCESS"))
                            {
                                itemIF_CL_ID->setBackgroundColor(0, Qt::black);
                                itemIF_CL_ID->setForeground(0, Qt::white);
                                itemIF_CL_ID->setIcon(0, QIcon(":/VimbaViewer/Images/lock.png"));
                                if(0 == sAccess.compare("NO ACCESS"))
                                    itemIF_CL_ID->setDisabled(true);
                            }
                        }

                        m_CL.append(itemIF_CL_ID);
                    }

                    if(0 == it.value().compare("CSI-2"))
                    {
                        QTreeWidgetItem *itemIF_CSI2_ID = m_CameraTree->createItem(itemCSI2Root, true);
                        itemIF_CSI2_ID->setText(0, QString::fromStdString(sCamID) );
                        itemIF_CSI2_ID->setIcon(0, QIcon(":/VimbaViewer/Images/stripes_256.png"));
                        if(!bIsopen)
                        {
                            if( 0 == sAccess.compare("READ ONLY") || 0 == sAccess.compare("NO ACCESS"))
                            {
                                itemIF_CSI2_ID->setBackgroundColor(0, Qt::black);
                                itemIF_CSI2_ID->setForeground(0, Qt::white);
                                itemIF_CSI2_ID->setIcon(0, QIcon(":/VimbaViewer/Images/lock.png"));
                                if(0 == sAccess.compare("NO ACCESS"))
                                    itemIF_CSI2_ID->setDisabled(true);
                            }
                        }
						if(bIsopen)
                        {
                            itemIF_CSI2_ID->setCheckState(0, Qt::Checked);
                        }

                        m_CSI2.append(itemIF_CSI2_ID);
                    }
                    break;
                }
            }
        }
    }

    m_CameraTree->expandAll();
}

void MainWindow::onRightMouseClicked ( const bool &bIsClicked )
{
    m_bIsRightMouseClicked = bIsClicked;
}

void MainWindow::rightMouseOpenCamera ( bool bOpenAccesState )
{
    QString sStatus;
    QObject *sender = QObject::sender();
    QString sSenderName = sender->objectName();
    m_sOpenAccessType = sSenderName;

    QStringList sAccessList = getStringListInfo(m_PermittedAccessStateListMap , m_sCurrentModel);
    unsigned int nPosition  = getAccessListPosition(sSenderName);
    QString sCurrentState = sAccessList.at(nPosition);
    bool bCurrentState = false;

    if(0 == sCurrentState.compare("false"))
        bCurrentState = false;

    if(0 == sCurrentState.compare("true"))
        bCurrentState = true;

    if(bOpenAccesState == bCurrentState)
    {
        return;
    }
    else
    {
        if(bCurrentState)
        {
            m_CameraTree->setCheckCurrentItem(false);
            sStatus = "false";
        }
        else
        {
            m_CameraTree->setCheckCurrentItem(true);
            sStatus = "true";
        }
    }

    updateAccessStateListMap(nPosition, sStatus, false); 
    m_bIsOpenByRightMouseClick = true;
    onCameraClicked(m_sCurrentModel, m_bIsCurrentModelChecked);
}

/* Find the camera and read all string in StringList within the map */
QStringList MainWindow::getStringListInfo ( const QMap<QString, QStringList> mInfoMap, const QString &sCamID )
{
    QStringList sListInfo;
    QMap<QString, QStringList>::const_iterator itr = mInfoMap.find(sCamID);
    
    /* keep the last information */
    while (itr != mInfoMap.constEnd()) 
    {
        if(0 == itr.key().compare(sCamID))
        {
            sListInfo = itr.value();
            break;
        }

        ++itr;
    }

    return sListInfo;
}

/* get the best access */
QString  MainWindow::getBestAccess ( const QString &sCamID )
{
    QString sBestAccess;
    QStringList sBestList;
    
    QMap<QString, QStringList>::const_iterator itr = m_CamerasPermittedAccessMap.find(sCamID);
    while (itr != m_CamerasPermittedAccessMap.constEnd()) 
    {
        if(0 == itr.key().compare(sCamID))
        {
            sBestList = itr.value();
            break;
        }

        ++itr;
    }

    if(0 != sBestList.size())
        sBestAccess = sBestList.at(0);

    return sBestAccess;
}
/* get the string position of the access in the list */
unsigned int  MainWindow::getAccessListPosition ( const QString &sAccessName )
{
    /* get position */
    QMap<QString, QStringList>::const_iterator itr = m_CamerasPermittedAccessMap.find(m_sCurrentModel);
    QStringList sPermittedAccess;

    while (itr != m_CamerasPermittedAccessMap.constEnd()) 
    {
        if(0 == itr.key().compare(m_sCurrentModel))
        {
            sPermittedAccess = itr.value();
            break;
        }

        ++itr;
    }

    unsigned int nPosition = 0;

    for (int i = 0; i < sPermittedAccess.size(); i++)
    {
        if(0 == sPermittedAccess.at(i).compare(sAccessName))
        {
            nPosition = i;
            break;
        }    
    }

    return nPosition;

}

/* updating the access list value (true or false) */
void MainWindow::updateAccessStateListMap ( const unsigned int &nPosition, const QString sStatus, const bool &bIsResetAll )
{
    QStringList sPermittedAccess;
    QMap<QString, QStringList>::const_iterator itr = m_PermittedAccessStateListMap.find(m_sCurrentModel);
    while (itr != m_PermittedAccessStateListMap.constEnd()) 
    {
        if(0 == itr.key().compare(m_sCurrentModel))
        {
            sPermittedAccess = itr.value();
            break;
        }

        ++itr;
    }

    QStringList sUpdateList;
    for (int i = 0; i < sPermittedAccess.size(); i++)
    {
        if(bIsResetAll)
        {
            sUpdateList << "false";
        }
        else
        {
            if( nPosition == i)
            {
                sUpdateList << sStatus;
                continue;
            }

            sUpdateList << sPermittedAccess.at(i);
        }    
    }

    m_PermittedAccessStateListMap[m_sCurrentModel] = sUpdateList;
}

void MainWindow::onCameraClicked ( const QString &sModel, const bool &bIsChecked )
{
    m_bIsCurrentModelChecked = bIsChecked;
    m_sCurrentModel = sModel;
 
    if( m_bIsRightMouseClicked)
    {
        m_bIsRightMouseClicked = false;
        /* show permitted access */
        if( !m_RightMouseAction.isEmpty())
        {
            for (int nPos = 0; nPos < m_RightMouseAction.size(); nPos++)
            {
                delete m_RightMouseAction.at(nPos);
            }

            m_RightMouseAction.clear();
        }
        
        QStringList sPermittedAccess = getStringListInfo(m_CamerasPermittedAccessMap, sModel);
        
        for (int i = 0; i < sPermittedAccess.size(); i++)
        {
            m_RightMouseAction.push_back(new QAction(this));
            m_RightMouseAction.last()->setObjectName(sPermittedAccess.at(i));
            m_RightMouseAction.last()->setCheckable(true);
            m_RightMouseAction.last()->setText(sPermittedAccess.at(i));
    
            connect(m_RightMouseAction.last(), SIGNAL(toggled(bool)), this, SLOT(rightMouseOpenCamera(bool)) );
            rightMouseMenu.addAction(m_RightMouseAction.last());
        }

        bool bIsOneOfThemChecked = false;
        QMap<QString, QStringList>::const_iterator it = m_PermittedAccessStateListMap.find(sModel);
        while (it != m_PermittedAccessStateListMap.constEnd()) 
        {
            QStringList sValueList = it.value();
            for(int i = 0; i < sValueList.size(); i++)
            {
                if(0 == sValueList.at(i).compare("true"))
                {
                    m_RightMouseAction.at(i)->setChecked(true);
                    bIsOneOfThemChecked = true;
                }

                else if(0 == sValueList.at(i).compare("false"))
                    m_RightMouseAction.at(i)->setChecked(false);
            }

            /* disabling other menus entries if one of already selected */
            if(bIsOneOfThemChecked)
            {
                for(int i = 0; i < sValueList.size(); i++)
                {
                    if(0 != sValueList.at(i).compare("true"))
                    {
                        m_RightMouseAction.at(i)->setEnabled(false);
                    }    
                }
            }

            break;
        }

        rightMouseMenu.exec(QCursor::pos());
    }
    else
    {
        try
        {
            bIsChecked ? openViewer(sModel):closeViewer(sModel);
        }
        catch(const std::exception &e)
        {
            m_Logger->logging("MainWindow <onCameraClicked> Exception: "+QString::fromStdString(e.what()), VimbaViewerLogCategory_ERROR); 
        }
    }

    if(!m_CameraTree->isEnabled())
        m_CameraTree->setEnabled(true);
    
}

void MainWindow::openViewer ( const QString &sCamID )
{
    try
    {
        /* find out what cam it is */
        for(unsigned int i=0; i<m_rCameras.size(); i++)
        {
            std::string sCamDisplayName;
            getCameraDisplayName(m_rCameras.at(i), sCamDisplayName );

            if (  0 == sCamID.compare(QString::fromStdString( sCamDisplayName )))
            {
                if( !m_IsCameraOpenMap.contains(QString::fromStdString( sCamDisplayName )) )
                {  
                    /* if it's not triggered from right mouse click menu, open with best case */
                    if(!m_bIsOpenByRightMouseClick )
                        m_sOpenAccessType = getBestAccess(QString::fromStdString(sCamDisplayName));
                
                    //keep in mind what viewer is open
                    m_IsCameraOpenMap.insert(QString::fromStdString(sCamDisplayName), "true");

                    m_Viewer.append(new ViewerWindow(this,0, sCamID, m_sOpenAccessType, m_bIsAutoAdjustPacketSize, m_rCameras.at((i))));

                    if(!connect(m_Viewer.back(), SIGNAL(closeViewer (const QString &)), this, SLOT(onCloseFromViewer(const QString &))))
                        m_Logger->logging("MainWindow: Failed connecting SIGNAL (<CameraTreeWindow>closeViewer) to SLOT(<MainWindow>onCloseFromViewer)", VimbaViewerLogCategory_ERROR);

                    if(!m_CameraTree->isEnabled())
                        m_CameraTree->setDisabled(false);

                    QString sBestAccess = getBestAccess(sCamID);

                    if(m_Viewer.back()->getCamOpenStatus())
                    {
                        m_Logger->logging(tr("Opening The Viewer:")+"\t"+sCamID, VimbaViewerLogCategory_INFO);
                        /*Use best case in case open camera directly with no  right mouse click */
                        if(!m_bIsOpenByRightMouseClick )
                        {
                            updateAccessStateListMap(0, "true", false);
                        }

                        QString sAdjustMsg;
                        if(m_Viewer.back()->getAdjustPacketSizeMessage (sAdjustMsg))
                        {
                            sAdjustMsg.contains("Failed", Qt::CaseInsensitive) ? m_Logger->logging(sAdjustMsg+sCamID, VimbaViewerLogCategory_INFO) : m_Logger->logging(sAdjustMsg+sCamID, VimbaViewerLogCategory_OK); 
                        }
                    }
                    else
                    {
                        unsigned int nPos;
                   
                        onCloseFromViewer ( sCamID );
                        nPos = getAccessListPosition(sBestAccess);
                        updateAccessStateListMap(nPos, "false", true); /* reset All */

                        if( VmbErrorInvalidAccess == m_Viewer.back()->getOpenError() )
                        {
                            m_Logger->logging(tr("Opening The Viewer:")+"\t"+ 
                                               sCamID + 
                                               " failed! <Cannot open the camera because it is already under control by another application>", VimbaViewerLogCategory_WARNING);
                        }
                        else
                        {
                            m_Logger->logging(tr("Opening The Viewer:")+"\t" + 
                                               sCamID + 
                                               " failed! - Error: " +
                                               QString::number(m_Viewer.back()->getOpenError()) +
                                               m_Helper.mapReturnCodeToString(m_Viewer.back()->getOpenError()), VimbaViewerLogCategory_WARNING); 
                        }

                        if (m_sOpenAccessType.isEmpty())
                        {
                            if(!m_Viewer.empty())
                                m_Viewer.pop_back();
                        }
                    }
                }
                else
                {
                    m_Logger->logging(  tr( "Opening The Viewer:" ) + "\t"+ 
                                        sCamID + 
                                        " failed! <Cannot open the same camera twice>", VimbaViewerLogCategory_WARNING);
                }
            }
        }
    }
    catch(const std::exception &e)
    {
        m_Logger->logging("MainWindow <openViewer> Exception: " + QString::fromStdString(e.what()), VimbaViewerLogCategory_ERROR); 
    }

    if(m_bIsOpenByRightMouseClick )
        m_bIsOpenByRightMouseClick = false;
}

struct IDPredicat
{
private:
    const QString& m_ID;
public:
    IDPredicat( const QString& id)
        : m_ID( id )
    {}
    bool operator () ( ViewerWindow *v) const
    {
        return m_ID == v->getCameraID();
    }
};
void MainWindow::closeViewer ( const QString &sCamID )
{
    /* copy ID for logging because sCamID will be deleted (ref) when m_Viewer destroyed */
    const QString sID = sCamID;
    QMap <QString, QString>::iterator findPos = m_IsCameraOpenMap.find( sID );
    if( m_IsCameraOpenMap.end() != findPos )
    {
        m_IsCameraOpenMap.erase( findPos);
    }

    QVector<ViewerWindow*>::iterator findWindowPos = std::find_if( m_Viewer.begin(),m_Viewer.end(), IDPredicat( sID ));
    if( m_Viewer.end() != findWindowPos)
    {
        delete *findWindowPos;
        *findWindowPos = NULL;
        m_Viewer.erase(findWindowPos);
        m_Logger->logging(tr("Closing The Viewer:") + "\t" + sID, VimbaViewerLogCategory_INFO); 
        updateAccessStateListMap(0, "false", true); /* reset all */ 
    }
    if(!m_CameraTree->isEnabled())
    {
        m_CameraTree->setDisabled(false);
    }

    searchCameras();
}

void MainWindow::onCloseFromViewer ( const QString &sModel )
{
    QTreeWidgetItemIterator it(  m_CameraTree );

    /* iterate the tree and find the position of the string, if found uncheck the checkbox */
    while( *it )
    {
        if( (*it)->text(0) == sModel )
        {
            if( Qt::Checked == (*it)->checkState(false) )
            {
                if( 0 == (*it)->text(0).compare(sModel) )
                {
                    (*it)->setSelected(true);
                    (*it)->setCheckState(0, Qt::Unchecked); 
                    //closeViewer(sModel);
                    break;
                }
            }    
        }

        ++it;
    }

    closeViewer(sModel);
}

void MainWindow::on_ActionDiscover_triggered ( void )
{
    for (int i=0; i < m_Viewer.size(); i++)
    {
        if(m_Viewer.at(i)->getCamOpenStatus())
        {
            closeViewer (m_Viewer.at(i)->getCameraID());
            i--;
        }
    }

    onUpdateDeviceList();
}

void MainWindow::on_ActionClear_triggered ( void )
{
    if(0 != m_Logger->count())
        m_Logger->clear();
}

class IsEqualCamera
{
private:
    CameraPtr m_pCam;
public:
    IsEqualCamera() {}
    IsEqualCamera( CameraPtr pCam ) : m_pCam( pCam ) {}
    bool operator() ( const CameraPtr pCam )
    {
        return SP_ISEQUAL( m_pCam, pCam );
    }
};

void MainWindow::on_ActionOpenByID_triggered ( void )
{
    if ( QDialog::Accepted == m_OpenByIDDialog->exec() )
    {
        CameraPtr pCam;
        std::string sDisplayName;
        VmbErrorType err = m_VimbaSystem.GetCameraByID( m_OpenByID.LineEdit_ID->text().toStdString().c_str(), pCam );

        if ( VmbErrorSuccess == err )
        {
            IsEqualCamera predicate( pCam );
            if ( m_rCameras.end() == std::find_if( m_rCameras.begin(), m_rCameras.end(), predicate ))
            {
                m_rCameras.push_back( pCam );
            }

            err = getCameraDisplayName( pCam, sDisplayName );
            if ( VmbErrorSuccess == err )
            {
                m_bIsOpenByRightMouseClick = true;
                m_sOpenAccessType = m_OpenByID.ComboBox_Access->currentText();
                VmbAccessModeType modes;
                err = pCam->GetPermittedAccess( modes );
                if ( VmbErrorSuccess == err )
                {
                    if ( VmbAccessModeFull == (VmbAccessModeFull & modes) )
                    {
                        m_CamerasPermittedAccessMap[QString(sDisplayName.c_str())] << tr("Open FULL ACCESS");
                    }
                    if ( VmbAccessModeRead == (VmbAccessModeRead & modes) )
                    {
                        m_CamerasPermittedAccessMap[QString(sDisplayName.c_str())] << tr("Open READ ONLY");
                    }
                    if ( VmbAccessModeConfig == (VmbAccessModeConfig & modes) )
                    {
                        m_CamerasPermittedAccessMap[QString(sDisplayName.c_str())] << tr("Open CONFIG");
                    }
                    openViewer( QString(sDisplayName.c_str()) );
                }
            }
        }

        if ( VmbErrorSuccess != err )
        {
            QMessageBox MsgBox;
            MsgBox.setText( "The device could not be found" );
            MsgBox.exec();
        }
    }
}

void MainWindow::on_ActionCommand_triggered ( void )
{
    if(true == m_ActionCommandFlag)
    {
        m_ActionCommandDialog->RunDialog();
    }
    else
    {
        m_Logger->logging(tr("Loading Action Command dialog failed. GigE TL could not be found"), VimbaViewerLogCategory_ERROR);
    }
}

void MainWindow::on_ActionStartOptions_triggered ( void )
{
    if( QDialog::Accepted == m_StartOptionDialog->exec() )
    {
        m_bIsAutoAdjustPacketSize = m_StartOption.AutoAdjustPacketSize_CheckBox->isChecked();
    }
    else
    {
        m_StartOption.AutoAdjustPacketSize_CheckBox->setChecked(m_bIsAutoAdjustPacketSize);
    }
}

VmbErrorType MainWindow::getCameraDisplayName( const CameraPtr &camera, std::string &sDisplayName )
{
    VmbErrorType  error;
    std::string sID;
    std::string sSN;
    
    error = camera->GetModel(sDisplayName);
    if(VmbErrorSuccess != error)
    {
        m_Logger->logging(tr("GetModel Failed, Error: ")+QString::number(error)+" "+ m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR); 
        return error;
    }
    else
    {
        error = camera->GetSerialNumber(sSN);
        if(VmbErrorSuccess != error)
        {
            m_Logger->logging(tr("GetSerialNumber Failed, Error: ")+QString::number(error)+" "+ m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR); 
            return error;
        }
        else
        {
            error = camera->GetID(sID);
            if(VmbErrorSuccess != error)
            {
                m_Logger->logging(tr("GetID Failed, Error: ")+QString::number(error)+" "+ m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR); 
                return error;
            }
            else
            {  
                std::string sDisplayNameEnding;

                sDisplayNameEnding.append("-");
                sDisplayNameEnding.append(sSN);

                if ( 0!=sSN.compare(sID) )
                {                            
                    sDisplayNameEnding.append("(");
                    sDisplayNameEnding.append(sID);
                    sDisplayNameEnding.append(")");
                }

                std::string sLegacyDisplayName = sDisplayName + sDisplayNameEnding;

                VmbInterfaceType cameraIFType;
                error = camera->GetInterfaceType(cameraIFType);  

                if(VmbErrorSuccess != error)
                {
                    m_Logger->logging(tr("GetInterfaceType Failed, Error: ")+QString::number(error)+" "+ m_Helper.mapReturnCodeToString(error), VimbaViewerLogCategory_ERROR); 
                    return error;
                }
                
                // camera interface type is GigE. update display name with IP address 
                if ( VmbInterfaceEthernet == cameraIFType )
                {                     
                    // lookup the IP address of the camera         
                    QString sIPAddress;                    
                    error = getIPAddress(camera, sIPAddress);

                    // replace the model ID with the IP address
                    if( VmbErrorSuccess == error && !sIPAddress.isEmpty() )
                    {                    
                        QString sTempDisplayName = QString::fromStdString(sDisplayName);
                        QRegExp regExp("\\(([^)]+)\\)");

                        // to account for cameras without model name in parenthesis
                        if( -1 == regExp.indexIn(sTempDisplayName) )
                        {
                            sDisplayName.append(sIPAddress.toStdString());
                        }
                        else
                        {
                            sTempDisplayName.replace(regExp, sIPAddress);
                            sDisplayName = sTempDisplayName.toStdString();
                        }

                        sDisplayName.append(sDisplayNameEnding);

                        // store display name with IP address to be available when interface offline
                        m_CameraDisplayNameMap.insert(sLegacyDisplayName, sDisplayName);
                    }
                    // camera disconnected or interface not available
                    else                 
                    {                 
                        // lookup cached version of display name with IP address
                        if ( m_CameraDisplayNameMap.contains(sLegacyDisplayName))
                        {
                            sDisplayName = m_CameraDisplayNameMap.value(sLegacyDisplayName);
                        }     
                        else
                        {
                            sDisplayName = sLegacyDisplayName;
                        }
                     }
                }
                // other camera interface types use legacy naming convention
                else
                {
                    sDisplayName = sLegacyDisplayName;
                }       
            }
        }
    }
    return error;
}

VmbErrorType MainWindow::getIPAddress( const AVT::VmbAPI::CameraPtr &camera, QString &sIPAdress)
{
    VmbErrorType error;    
    std::string sCameraID, sInterfaceID, sDeviceID;
    AVT::VmbAPI::InterfacePtr pInterface;    
    AVT::VmbAPI::FeaturePtr pSelectorFeature, pSelectedDeviceID, pSelectedIPAddress;
    VmbInt64_t nMinRange, nMaxRange, nIP;
    VmbInt32_t  nIP_32;

    // get the camera ID
    error = camera->GetID( sCameraID );
    if( VmbErrorSuccess == error )
    {
        // get the interface ID
        error = camera->GetInterfaceID( sInterfaceID );
        if( VmbErrorSuccess == error )
        {
            // get a pointer to the interface
            error = m_VimbaSystem.GetInterfaceByID( sInterfaceID.c_str(), pInterface );
            if( VmbErrorSuccess == error )
            {
                // open the interface 
                error = pInterface->Open();
                if( VmbErrorSuccess == error )
                {
                    // get the device selector
                    error = pInterface->GetFeatureByName( "DeviceSelector", pSelectorFeature );
                    if( VmbErrorSuccess == error )
                    {
                        // get the range of the available devices 
                        error = pSelectorFeature->GetRange( nMinRange, nMaxRange );
                        
                        // check for negative range in case requested feature contains no items
                        if( VmbErrorSuccess == error && nMaxRange >= 0 )
                        {
                            // get the device ID pointer
                            error = pInterface->GetFeatureByName( "DeviceID", pSelectedDeviceID );
                            if( VmbErrorSuccess == error )
                            {
                                // get IP addresses of all cameras connected to interface
                                error = pInterface->GetFeatureByName( "GevDeviceIPAddress", pSelectedIPAddress );
                                if( VmbErrorSuccess == error )
                                {
                                    // find the IP address of the desired camera 
                                    for( VmbInt64_t intNo = nMinRange; intNo <= nMaxRange; ++intNo )
                                    {
                                        error = pSelectorFeature->SetValue( intNo );
                                        if( VmbErrorSuccess == error )
                                        {
                                            error = pSelectedDeviceID->GetValue( sDeviceID );
                                            if( VmbErrorSuccess == error )
                                            {
                                                if( 0 == sDeviceID.compare( sCameraID ) )
                                                {
                                                    error = pSelectedIPAddress->GetValue(nIP);
                                                    if( VmbErrorSuccess == error )
                                                    {
                                                        nIP_32 = static_cast<VmbInt32_t>(nIP);

                                                        // format IP address string
                                                        sIPAdress = QString("(%1)").arg( Helper::IPv4ToString( qFromBigEndian( nIP_32 ) ) );               
                                                        
                                                        // close the interface
                                                        error = pInterface->Close();
                                                        break;
                                                    } 
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            sIPAdress.clear();
                            error = VmbErrorNotFound;
                        }

                    }
                }
            }
        }
    }                
        
    return error;
}