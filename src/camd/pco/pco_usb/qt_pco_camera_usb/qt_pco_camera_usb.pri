HEADERS += \
    ./interface_includes.h \
    ./usb_grabthreadworker.h \
    ./usb_livethreadworker.h \
    ../../pco_classes/Cpco_com_usb.h \
    ../../pco_classes/Cpco_grab_usb.h \
    ../../pco_common/pco_classes/Cpco_com.h \
    ../../pco_common/pco_classes/pco_includes.h \
    ../../pco_common/qt_pco_camera/imageconvert.h \
    ../../pco_common/qt_pco_camera/qt_pco_camera.h \
    ../../pco_common/qt_pco_camera/simpledialog.h \
    ../../pco_common/qt_pco_camera/infopopup.h \
    ../../pco_common/qt_pco_camera/memorymanager.h \
    ../../pco_common/qt_pco_camera/errorpopup.h \
    ../../pco_common/qt_pco_camera/camerasetup.h \
    ../../pco_common/qt_pco_camera/cameradescription.h \
    ../../pco_common/qt_pco_camera/exposuretime.h

SOURCES += \
    ./usb_grabthreadworker.cpp \
    ./usb_livethreadworker.cpp \
    ../../pco_common/qt_pco_camera/main.cpp \
    ../../pco_common/qt_pco_camera/cameradescription.cpp \
    ../../pco_common/qt_pco_camera/camerasetup.cpp \
    ../../pco_common/qt_pco_camera/errorpopup.cpp \
    ../../pco_common/qt_pco_camera/memorymanager.cpp \
    ../../pco_common/qt_pco_camera/imageconvert.cpp \
    ../../pco_common/qt_pco_camera/infopopup.cpp \
    ../../pco_common/qt_pco_camera/qt_pco_camera.cpp \
    ../../pco_common/qt_pco_camera/simpledialog.cpp \
    ../../pco_common/qt_pco_camera/exposuretime.cpp


FORMS += \
    ../../pco_common/qt_pco_camera/cameradescription.ui \
    ../../pco_common/qt_pco_camera/camerasetup.ui \
    ../../pco_common/qt_pco_camera/errorpopup.ui \
    ../../pco_common/qt_pco_camera/memorymanager.ui \
    ../../pco_common/qt_pco_camera/infopopup.ui \
    ../../pco_common/qt_pco_camera/qt_pco_camera.ui \
    ../../pco_common/qt_pco_camera/simpledialog.ui \
    ../../pco_common/qt_pco_camera/imageconvert.ui \
    ../../pco_common/qt_pco_camera/exposuretime.ui

RESOURCES += ../../pco_common/qt_pco_camera/qt_pco_camera.qrc
