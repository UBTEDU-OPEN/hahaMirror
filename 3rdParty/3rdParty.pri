INCLUDEPATH += \
    $$PWD/googletest/include \
#    $$PWD/jsonrpccpp/include \
    $$PWD/muduo/include \
    $$PWD/spdlog/include \
    $$PWD/websocketpp/include

LIBS += -L/usr/lib/x86_64-linux-gnu/ -lboost_system

LIBS += \
#    -lboost_system
    -L$$PWD/googletest/lib/ -lgtest \
#    -L$$PWD/jsonrpccpp/lib -ljsonrpccpp-client -ljsonrpccpp-common\
    -L$$PWD/muduo/lib -lmuduo_base -lmuduo_net\
    -L$$PWD/spdlog/lib -lspdlog \


