TEMPLATE = app
TARGET = qt_pco_camera_usb
DESTDIR = ../bin
QT += core widgets gui

DEFINES += QT_WIDGETS_LIB
INCLUDEPATH += ./GeneratedFiles \
    . \
    ../../pco_common/pco_include \
    ../../pco_common/pco_classes \
    ../../pco_common/qt_pco_camera \
    ./../pco_classes \

DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/moc
OBJECTS_DIR += objects
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(qt_pco_camera_usb.pri)

DEPENDPATH += $$PWD/../../pco_common/pco_lib

unix:!macx: LIBS += -L$$PWD/../../pco_common/pco_lib/ -lpcocnv -lpcofile -lpcolog -lpcocam_usb -lusb-1.0

unix:!macx: PRE_TARGETDEPS += $$PWD/../../pco_common/pco_lib/libpcocnv.a

unix:!macx: PRE_TARGETDEPS += $$PWD/../../pco_common/pco_lib/libpcofile.a

unix:!macx: PRE_TARGETDEPS += $$PWD/../../pco_common/pco_lib/libpcolog.a

unix:!macx: PRE_TARGETDEPS += $$PWD/../../pco_common/pco_lib/libpcocam_usb.a
