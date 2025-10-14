#-------------------------------------------------
#
# Project created by QtCreator 2011-09-21T11:01:39
#
#-------------------------------------------------

QT += core gui opengl widgets printsupport

TARGET = cuviewer
TEMPLATE = app

CONFIG += c++11 sdk_no_version_check

SOURCES += main.cpp\
           mainwindow.cpp \
    viewersettings.cpp \
    paletteview.cpp \
    imageprocessing.cpp \
    binpalettes.cpp \
    cuviewdoc/sceneentity.cpp \
    cuviewdoc/glrenderer.cpp \
    cuviewdoc/entityscene.cpp \
    cuviewdoc/draw_tria.cpp \
    cuviewdoc/draw_text.cpp \
    cuviewdoc/draw_sphoid.cpp \
    cuviewdoc/draw_sphere.cpp \
    cuviewdoc/draw_quadri.cpp \
    cuviewdoc/draw_point.cpp \
    cuviewdoc/draw_line.cpp \
    cuviewdoc/displaylistscene.cpp \
    cuviewdoc/cuviewdoc.cpp \
    cuviewdoc/cuvdatahandler.cpp \
    include/cuvvector.cpp \
    printpreview.cpp \
    pixmaplabel.cpp \
    showscene.cpp \
    globalpreferences.cpp \
    getimagesize.cpp \
    moviedialog.cpp \
    script.cpp \
    autoexport.cpp

HEADERS   += mainwindow.h \
    viewersettings.h \
    paletteview.h \
    viewdata.h \
    imageprocessing.h \
    binpalettes.h \
    cuviewdoc/sceneentity.h \
    cuviewdoc/glrenderer.h \
    cuviewdoc/entityscene.h \
    cuviewdoc/drawablescene.h \
    cuviewdoc/displaylistscene.h \
    cuviewdoc/cuviewdoc.h \
    cuviewdoc/cuvdatahandler.h \
    cuviewdoc/changeablescene.h \
    include/lessalloc.h \
    include/cuvvector.h \
    include/cuvtags.h \
    include/cuvdatautils.h \
    include/cuvcommonstructs.h \
    printpreview.h \
    pixmaplabel.h \
    showscene.h \
    globalpreferences.h \
    getimagesize.h \
    moviedialog.h \
    script.h \
    autoexport.h

FORMS     += mainwindow.ui \
    viewersettings.ui \
    printpreview.ui \
    showscene.ui \
    globalpreferences.ui \
    getimagesize.ui \
    moviedialog.ui \
    autoexport.ui

RESOURCES += images.qrc \
    glshaders.qrc \
    htmlFiles.qrc

OTHER_FILES += \
    test.vert \
    test.frag





























