#-------------------------------------------------
#
# Project created by QtCreator 2017-09-19T01:29:55
#
#-------------------------------------------------

QT       += core gui
CONFIG += C++11
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VetTSR
TEMPLATE = app


SOURCES += main.cpp\
#    vetlightpreprocessor.cpp \
#    vetlightdetection.cpp \
#    vetwarningsignpreprocessor.cpp \
#    vetwarningsigndetection.cpp \
#    vetfarroadsighpreprocess.cpp \
#    vetsigndetector.cpp \
#    vetfhog.cpp \
#    vetkcftracker.cpp \
#    vetmodelstore.cpp \
#    vetmultikcftracker.cpp \
    VetLight/vetlightdetection.cpp \
    VetLight/vetlightpreprocessor.cpp \
    VetMultiTracker/vetfhog.cpp \
    VetMultiTracker/vetkcftracker.cpp \
    VetMultiTracker/vetmultikcftracker.cpp \
    VetProhibition/vetfarroadsighpreprocess.cpp \
    VetProhibition/vetsigndetector.cpp \
    VetWarn/vetwarningsigndetection.cpp \
    VetWarn/vetwarningsignpreprocessor.cpp \
    VetMultiTracker/vettrackermodel.cpp

HEADERS  += \
    vetlightpreprocessor.h \
    vetlightdetection.h \
    vetwarningsignpreprocessor.h \
    vetwarningsigndetection.h \
    vetfarroadsighpreprocess.h \
    vetsigndetector.h \
    vetsvmhog.h \
    struct.h \
    vetffttools.hpp \
    vetfhog.hpp \
    vetkcftracker.hpp \
    vetmodelstore.h \
    vetmultikcftracker.h \
    VetLight/vetlightdetection.h \
    VetLight/vetlightpreprocessor.h \
    common/struct.h \
    VetMultiTracker/vetffttools.hpp \
    VetMultiTracker/vetfhog.hpp \
    VetMultiTracker/vetkcftracker.hpp \
    VetMultiTracker/vetmodelstore.h \
    VetMultiTracker/vetmultikcftracker.h \
    VetMultiTracker/vettracker.h \
    VetProhibition/vetfarroadsighpreprocess.h \
    VetProhibition/vetsigndetector.h \
    VetWarn/vetwarningsigndetection.h \
    VetWarn/vetwarningsignpreprocessor.h \
    VetMultiTracker/vetlabdata.hpp \
    VetMultiTracker/vetrecttools.hpp \
    VetMultiTracker/vettrackermodel.h \
    VetProhibition/vetsvmhog.h

FORMS    +=

LIBS += /usr/local/lib/libopencv_*.so \

INCLUDEPATH += /usr/local/include \
               /usr/local/include/opencv \
               /usr/local/include/opencv2 \
               ./VetLight\
               ./VetMultiTracker\
               ./VetProhibition\
               ./VetWarn\
               ./common\

DISTFILES +=
    /home/lee/Lee/opencv_space/VetTSR/circle_detect.xml
