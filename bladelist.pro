TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \

QMAKE_CFLAGS += -Wno-deprecated-declarations
QMAKE_CXXFLAGS += -Wno-deprecated-declarations

LIBS += -lbladeRF
