#-------------------------------------------------
#
# Project created by QtCreator 2015-05-08T20:24:50
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FacadeClipper
TEMPLATE = app

#Including opencv
INCLUDEPATH += /usr/include/opencv\
               methos/PW \
               methods/PW/argv \
               methods/PW/CSparse/Include \
               methods/PW/include \
               methods/PW/PINK

LIBS += -L/usr/lib -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_objdetect

QMAKE_CFLAGS += -g -Wall  -O4 -g -Wall  -ICSparse/Include
QMAKE_CXXFLAGS +=  -g -Wall -std=c++11 -O4 -Wno-deprecated -g -Wall  -ICSparse/Include  -IPINK  -Iargv  -Iinclude -c

#-lopencv_imgcodecs
#QMAKE_CXXFLAGS += `pkg-config --cflags --libs opencv`


SOURCES += main.cpp\
        view/facadeclipper.cpp \
        view/hough.cpp \
        view/floodfill.cpp \
        view/canny.cpp \
        view/perspectivecorrection.cpp \
        methods/perspective.cpp \
        methods/treatments.cpp \
        view/powerwatershed.cpp \
        classes/scene.cpp \
        methods/pw.cpp \
        methods/PW/PINK/larith.cpp \
        methods/PW/PINK/mccodimage.cpp \
        methods/PW/PINK/mcimage.cpp \
        methods/PW/PINK/mcindic.cpp \
        methods/PW/PINK/mclifo.cpp \
        methods/PW/PINK/mcrbt.cpp \
        methods/PW/src/cccodimage.cpp \
        methods/PW/src/ccsort.cpp \
        methods/PW/src/gageodesic.cpp \
        methods/PW/src/image_toolbox.cpp \
        methods/PW/src/lMSF.cpp \
        methods/PW/src/MSF_RW.cpp \
        methods/PW/src/random_walker.cpp \
        methods/PW/src/union_find.cpp \
        methods/PW/argv/argv.c \
        methods/PW/CSparse/Source/cs_add.c \
        methods/PW/CSparse/Source/cs_amd.c \
        methods/PW/CSparse/Source/cs_compress.c \
        methods/PW/CSparse/Source/cs_counts.c \
        methods/PW/CSparse/Source/cs_cumsum.c \
        methods/PW/CSparse/Source/cs_dfs.c \
        methods/PW/CSparse/Source/cs_etree.c \
        methods/PW/CSparse/Source/cs_fkeep.c \
        methods/PW/CSparse/Source/cs_ipvec.c \
        methods/PW/CSparse/Source/cs_leaf.c \
        methods/PW/CSparse/Source/cs_lsolve.c \
        methods/PW/CSparse/Source/cs_lu.c \
        methods/PW/CSparse/Source/cs_lusol.c \
        methods/PW/CSparse/Source/cs_malloc.c \
        methods/PW/CSparse/Source/cs_multiply.c \
        methods/PW/CSparse/Source/cs_permute.c \
        methods/PW/CSparse/Source/cs_post.c \
        methods/PW/CSparse/Source/cs_reach.c \
        methods/PW/CSparse/Source/cs_scatter.c \
        methods/PW/CSparse/Source/cs_spsolve.c \
        methods/PW/CSparse/Source/cs_sqr.c \
        methods/PW/CSparse/Source/cs_tdfs.c \
        methods/PW/CSparse/Source/cs_transpose.c \
        methods/PW/CSparse/Source/cs_usolve.c \
        methods/PW/CSparse/Source/cs_util.c \
        view/texturesearchengine.cpp \
        methods/texture.cpp \
    view/zonesextractor.cpp \
    classes/scene2.cpp \
    view/semiautomaticclipper.cpp \
    view/settings.cpp


HEADERS  += view/facadeclipper.h \
        view/hough.h \
        view/floodfill.h \
        view/canny.h \
        methods/asmopencv.h \
        view/perspectivecorrection.h \
        methods/perspective.h \
        methods/treatments.h \
        view/powerwatershed.h \
        classes/scene.h \
        methods/pw.h \
        methods/PW/argv/argv.h \
        methods/PW/CSparse/Include/cs.h \
        methods/PW/include/cccodimage.h \
        methods/PW/include/ccsort.h \
        methods/PW/include/gageodesic.h \
        methods/PW/include/image_toolbox.h \
        methods/PW/include/lMSF.h \
        methods/PW/include/MSF_RW.h \
        methods/PW/include/powerwatsegm.h \
        methods/PW/include/random_walker.h \
        methods/PW/include/union_find.h \
        methods/PW/PINK/larith.h \
        methods/PW/PINK/mccodimage.h \
        methods/PW/PINK/mcimage.h \
        methods/PW/PINK/mcindic.h \
        methods/PW/PINK/mclifo.h \
        methods/PW/PINK/mcrbt.h \
        methods/PW/PINK/mcutil.h \
        methods/PW/argv/argv_loc.h \
        methods/PW/argv/conf.h \
        methods/PW/argv/compat.h \
        view/texturesearchengine.h \
        methods/texture.h \
    view/zonesextractor.h \
    classes/scene2.h \
    view/semiautomaticclipper.h \
    view/settings.h

FORMS    += \
        ui/Hough.ui \
        ui/Floodfill.ui \
        ui/Canny.ui \
        ui/Facadeclipper.ui \
        ui/perspectivecorrection.ui \
        ui/powerwatershed.ui \
        ui/texturesearchengine.ui \
    ui/zonesextractor.ui \
    ui/semiautomaticclipper.ui \
    ui/settings.ui
