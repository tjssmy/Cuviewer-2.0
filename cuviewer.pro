#-------------------------------------------------
#
# Project created by QtCreator 2018-06-27T00:19:47
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = CuViewer
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
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


HEADERS += \
        mainwindow.h \
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


FORMS += \
        mainwindow.ui \
        viewersettings.ui \
        printpreview.ui \
        showscene.ui \
        globalpreferences.ui \
        getimagesize.ui \
        moviedialog.ui \
        autoexport.ui

RESOURCES += \
        images.qrc \
        glshaders.qrc \
        htmlfiles.qrc

LIBS += -lOpenGL32 \
        -lGlu32
