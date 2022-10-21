INCLUDEPATH += /usr/include/freetype2/

HEADERS += \
    $$PWD/ConvertUTF.h \
    $$PWD/SimpleIni.h \
    $$PWD/checkcode.h \
    $$PWD/fileopt.h \
    $$PWD/json.hpp \
    $$PWD/logging.h \
    $$PWD/md5.hpp \
    $$PWD/mongoose.h \
    #$$PWD/putpng.h \
    #$$PWD/puttext.h \
    #$$PWD/run_cmd.h \
    $$PWD/processinfo.h \
    $$PWD/run_cmd.h \
    $$PWD/string.h \
    $$PWD/time.h

SOURCES += \
    $$PWD/ConvertUTF.c \
    $$PWD/checkcode.cc \
    $$PWD/fileopt.cc \
    $$PWD/logging.cc \
    $$PWD/mongoose.c \
    #$$PWD/png.cc \
    #$$PWD/putpng.cc \
    #$$PWD/puttext.cc \
    $$PWD/processinfo.cc \
    $$PWD/run_cmd.cc \
    $$PWD/string.cc \
    $$PWD/time.cc

LIBS += -lmuduo_base

