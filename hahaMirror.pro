QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
# INCLUDEPATH += /home/ubt/intel/openvino/opencv/include



include($$PWD/3rdParty/3rdParty.pri)
include($$PWD/common/common.pri)
include($$PWD/algorithm/algorithm.pri)

SOURCES += \
    # consumer_exception.cc \
    # consumer_identify.cc \
    # consumer_mjpeg.cc \
    facedetect.cpp \
    faceidentify.cpp \
    hahacore.cpp \
    http_client.cc \
    image.cpp \
    imageoverlay.cpp \
    main.cpp \
    mainwindow.cpp \
    playcamera.cpp \
    producer_record_impl.cc \
    websocket_server.cc

HEADERS += \
    consumer_base.h \
    # consumer_exception.h \
  #  consumer_identify.h \
  #  consumer_mjpeg.h \
    facedetect.h \
    faceidentify.h \
    hahacore.h \
    http_client.h \
    image.h \
    imageoverlay.h \
    mainwindow.h \
    playcamera.h \
    producer_base.h \
    producer_record_impl.h \
    websocket_server.h

FORMS += \
    mainwindow.ui

LIBS += -lcurl
LIBS += -L$$PWD/lib/opencv -lopencv_calib3d -lopencv_core -lopencv_dnn -lopencv_features2d -lopencv_flann -lopencv_gapi -lopencv_highgui -lopencv_imgcodecs -lopencv_imgproc -lopencv_video -lopencv_videoio -lopencv_photo



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
