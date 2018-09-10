#!/bin/sh

# delete the old app folder
rm -dfR VimbaViewer.app

# create the folder
mkdir VimbaViewer.app
mkdir VimbaViewer.app/Contents
mkdir VimbaViewer.app/Contents/libs
mkdir VimbaViewer.app/Contents/MacOS
mkdir VimbaViewer.app/Contents/Resources
mkdir VimbaViewer.app/Contents/Frameworks

# copy the data allied vision files
cp binary/Darwin_x86_64bit/Release/libVimbaC.dylib              VimbaViewer.app/Contents/libs/
cp binary/Darwin_x86_64bit/Release/libVimbaCPP.dylib            VimbaViewer.app/Contents/libs/
cp binary/Darwin_x86_64bit/Release/libVimbaImageTransform.dylib VimbaViewer.app/Contents/libs/
cp VimbaGigETL.cti                                              VimbaViewer.app/Contents/libs/
# cp VimbaPCIeTL.cti                                              VimbaViewer.app/Contents/libs/
cp binary/Darwin_x86_64bit/Release/VimbaViewer                  VimbaViewer.app/Contents/MacOS/
cp ../../Source/Images/VimbaViewer.icns                         VimbaViewer.app/Contents/Resources/
cp Info.plist                                                   VimbaViewer.app/Contents/Info.plist

# copy the qt files
cp -R /opt/local/Library/Frameworks/QtCore.framework VimbaViewer.app/Contents/Frameworks
cp -R /opt/local/Library/Frameworks/QtGui.framework  VimbaViewer.app/Contents/Frameworks
cp -R /opt/local/Library/Frameworks/QtSvg.framework  VimbaViewer.app/Contents/Frameworks

#copy libpng
cp -R /opt/local/lib/libpng16.16.dylib               VimbaViewer.app/Contents/libs/

# change the dynamic lib pathes
install_name_tool -change libVimbaC.dylib              @executable_path/../libs/libVimbaC.dylib              VimbaViewer.app/Contents/libs/libVimbaCPP.dylib
install_name_tool -change libVimbaCPP.dylib            @executable_path/../libs/libVimbaCPP.dylib            VimbaViewer.app/Contents/MacOS/VimbaViewer
install_name_tool -change libVimbaImageTransform.dylib @executable_path/../libs/libVimbaImageTransform.dylib VimbaViewer.app/Contents/MacOS/VimbaViewer

# change the Qt frameworks pathes
install_name_tool -change /opt/local/Library/Frameworks/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore VimbaViewer.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore
install_name_tool -change /opt/local/Library/Frameworks/QtGui.framework/Versions/4/QtGui   @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui   VimbaViewer.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui
install_name_tool -change /opt/local/Library/Frameworks/QtSvg.framework/Versions/4/QtSvg   @executable_path/../Frameworks/QtSvg.framework/Versions/4/QtSvg   VimbaViewer.app/Contents/Frameworks/QtSvg.framework/Versions/4/QtSvg

install_name_tool -change /opt/local/Library/Frameworks/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore VimbaViewer.app/Contents/MacOS/VimbaViewer
install_name_tool -change /opt/local/Library/Frameworks/QtGui.framework/Versions/4/QtGui   @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui   VimbaViewer.app/Contents/MacOS/VimbaViewer
install_name_tool -change /opt/local/Library/Frameworks/QtSvg.framework/Versions/4/QtSvg   @executable_path/../Frameworks/QtSvg.framework/Versions/4/QtSvg   VimbaViewer.app/Contents/MacOS/VimbaViewer

install_name_tool -change /opt/local/Library/Frameworks/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore VimbaViewer.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui
install_name_tool -change /opt/local/Library/Frameworks/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore VimbaViewer.app/Contents/Frameworks/QtSvg.framework/Versions/4/QtSvg
install_name_tool -change /opt/local/Library/Frameworks/QtGui.framework/Versions/4/QtGui   @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui   VimbaViewer.app/Contents/Frameworks/QtSvg.framework/Versions/4/QtSvg

install_name_tool -change /opt/local/lib/libpng16.16.dylib                                 @executable_path/../libs/libpng16.16.dylib                        VimbaViewer.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui
