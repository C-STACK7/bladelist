TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        common/src/conversions.c \
        common/src/dc_calibration.c \
        common/src/devcfg.c \
        common/src/log.c \
        common/src/osx/clock_gettime.c \
        common/src/parse.c \
        common/src/range.c \
        common/src/sha256.c \
        common/src/str_queue.c \
        common/src/windows/clock_gettime.c \
        common/src/windows/getopt_long.c \
        common/src/windows/gettimeofday.c \
        common/src/windows/mkdtemp.c \
        common/src/windows/nanosleep.c \
        common/src/windows/setenv.c \
        common/test/dc_calibration/src/main.c \
        main.c

DISTFILES += \
    common/CMakeLists.txt \
    common/include/host_config.h.in \
    common/test/CMakeLists.txt \
    common/test/dc_calibration/CMakeLists.txt \
    common/thirdparty/ad936x/CMakeLists.txt

HEADERS += \
    common/include/conversions.h \
    common/include/dc_calibration.h \
    common/include/devcfg.h \
    common/include/iterators.h \
    common/include/log.h \
    common/include/minmax.h \
    common/include/osx/clock_gettime.h \
    common/include/parse.h \
    common/include/range.h \
    common/include/rel_assert.h \
    common/include/sha256.h \
    common/include/str_queue.h \
    common/include/thread.h \
    common/include/windows/c99/inttypes.h \
    common/include/windows/c99/stdbool.h \
    common/include/windows/clock_gettime.h \
    common/include/windows/getopt.h \
    common/include/windows/gettimeofday.h \
    common/include/windows/mkdtemp.h \
    common/include/windows/nanosleep.h \
    common/include/windows/ptw32_timespec.h \
    common/include/windows/setenv.h
