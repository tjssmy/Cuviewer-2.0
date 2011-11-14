#############################################################################
# Makefile for building: cuviewer.app/Contents/MacOS/cuviewer
# Generated by qmake (2.01a) (Qt 4.7.4) on: Mon Nov 14 07:36:40 2011
# Project:  cuviewer.pro
# Template: app
# Command: /usr/bin/qmake -o cuviewer.xcodeproj/project.pbxproj cuviewer.pro
#############################################################################

MOC       = /Developer/Tools/Qt/moc
UIC       = /Developer/Tools/Qt/uic
LEX       = flex
LEXFLAGS  = 
YACC      = yacc
YACCFLAGS = -d
DEFINES       = -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
INCPATH       = -I/usr/local/Qt4.7/mkspecs/macx-xcode -I. -I/Library/Frameworks/QtCore.framework/Versions/4/Headers -I/usr/include/QtCore -I/Library/Frameworks/QtGui.framework/Versions/4/Headers -I/usr/include/QtGui -I/Library/Frameworks/QtOpenGL.framework/Versions/4/Headers -I/usr/include/QtOpenGL -I/usr/include -I/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers -I/System/Library/Frameworks/AGL.framework/Headers -I. -I. -I/usr/local/include -I/System/Library/Frameworks/CarbonCore.framework/Headers -F/Library/Frameworks
DEL_FILE  = rm -f
MOVE      = mv -f

IMAGES = 
PARSERS =
preprocess: $(PARSERS) compilers
clean preprocess_clean: parser_clean compiler_clean

parser_clean:
check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compilers: ./moc_mainwindow.cpp ./moc_viewersettings.cpp ./moc_paletteview.cpp\
	 ./moc_imageprocessing.cpp ./moc_binpalettes.cpp ./moc_glrenderer.cpp\
	 ./moc_cuviewdoc.cpp ./moc_cuvdatahandler.cpp ./moc_printpreview.cpp\
	 ./moc_pixmaplabel.cpp ./moc_showscene.cpp ./moc_globalpreferences.cpp\
	 ./moc_getimagesize.cpp ./moc_moviedialog.cpp ./moc_autoexport.cpp ./qrc_images.cpp ./qrc_glshaders.cpp ./qrc_htmlFiles.cpp ./ui_mainwindow.h ./ui_viewersettings.h ./ui_printpreview.h\
	 ./ui_showscene.h ./ui_globalpreferences.h ./ui_getimagesize.h\
	 ./ui_moviedialog.h ./ui_autoexport.h
compiler_objective_c_make_all:
compiler_objective_c_clean:
compiler_moc_header_make_all: moc_mainwindow.cpp moc_viewersettings.cpp moc_paletteview.cpp moc_imageprocessing.cpp moc_binpalettes.cpp moc_glrenderer.cpp moc_cuviewdoc.cpp moc_cuvdatahandler.cpp moc_printpreview.cpp moc_pixmaplabel.cpp moc_showscene.cpp moc_globalpreferences.cpp moc_getimagesize.cpp moc_moviedialog.cpp moc_autoexport.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_mainwindow.cpp moc_viewersettings.cpp moc_paletteview.cpp moc_imageprocessing.cpp moc_binpalettes.cpp moc_glrenderer.cpp moc_cuviewdoc.cpp moc_cuvdatahandler.cpp moc_printpreview.cpp moc_pixmaplabel.cpp moc_showscene.cpp moc_globalpreferences.cpp moc_getimagesize.cpp moc_moviedialog.cpp moc_autoexport.cpp
moc_mainwindow.cpp: binpalettes.h \
		viewdata.h \
		viewersettings.h \
		ui_viewersettings.h \
		paletteview.h \
		include/cuvcommonstructs.h \
		include/cuvtags.h \
		include/lessalloc.h \
		config.h \
		cuviewdoc/cuviewdoc.h \
		cuviewdoc/glrenderer.h \
		include/cuvvector.h \
		cuviewdoc/entityscene.h \
		cuviewdoc/drawablescene.h \
		cuviewdoc/changeablescene.h \
		cuviewdoc/sceneentity.h \
		imageprocessing.h \
		showscene.h \
		ui_showscene.h \
		helpwindow.h \
		ui_mainwindow.h \
		mainwindow.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ mainwindow.h -o moc_mainwindow.cpp

moc_viewersettings.cpp: ui_viewersettings.h \
		paletteview.h \
		include/cuvcommonstructs.h \
		include/cuvtags.h \
		include/lessalloc.h \
		viewersettings.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ viewersettings.h -o moc_viewersettings.cpp

moc_paletteview.cpp: include/cuvcommonstructs.h \
		include/cuvtags.h \
		include/lessalloc.h \
		paletteview.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ paletteview.h -o moc_paletteview.cpp

moc_imageprocessing.cpp: viewdata.h \
		viewersettings.h \
		ui_viewersettings.h \
		paletteview.h \
		include/cuvcommonstructs.h \
		include/cuvtags.h \
		include/lessalloc.h \
		config.h \
		binpalettes.h \
		cuviewdoc/cuviewdoc.h \
		cuviewdoc/glrenderer.h \
		include/cuvvector.h \
		cuviewdoc/entityscene.h \
		cuviewdoc/drawablescene.h \
		cuviewdoc/changeablescene.h \
		cuviewdoc/sceneentity.h \
		imageprocessing.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ imageprocessing.h -o moc_imageprocessing.cpp

moc_binpalettes.cpp: viewdata.h \
		viewersettings.h \
		ui_viewersettings.h \
		paletteview.h \
		include/cuvcommonstructs.h \
		include/cuvtags.h \
		include/lessalloc.h \
		config.h \
		cuviewdoc/cuviewdoc.h \
		cuviewdoc/glrenderer.h \
		include/cuvvector.h \
		cuviewdoc/entityscene.h \
		cuviewdoc/drawablescene.h \
		cuviewdoc/changeablescene.h \
		cuviewdoc/sceneentity.h \
		binpalettes.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ binpalettes.h -o moc_binpalettes.cpp

moc_glrenderer.cpp: include/cuvtags.h \
		include/cuvcommonstructs.h \
		include/lessalloc.h \
		include/cuvvector.h \
		cuviewdoc/glrenderer.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ cuviewdoc/glrenderer.h -o moc_glrenderer.cpp

moc_cuviewdoc.cpp: cuviewdoc/glrenderer.h \
		include/cuvtags.h \
		include/cuvcommonstructs.h \
		include/lessalloc.h \
		include/cuvvector.h \
		cuviewdoc/entityscene.h \
		cuviewdoc/drawablescene.h \
		cuviewdoc/changeablescene.h \
		cuviewdoc/sceneentity.h \
		cuviewdoc/cuviewdoc.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ cuviewdoc/cuviewdoc.h -o moc_cuviewdoc.cpp

moc_cuvdatahandler.cpp: include/lessalloc.h \
		cuviewdoc/cuvdatahandler.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ cuviewdoc/cuvdatahandler.h -o moc_cuvdatahandler.cpp

moc_printpreview.cpp: ui_printpreview.h \
		paletteview.h \
		include/cuvcommonstructs.h \
		include/cuvtags.h \
		include/lessalloc.h \
		pixmaplabel.h \
		printpreview.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ printpreview.h -o moc_printpreview.cpp

moc_pixmaplabel.cpp: paletteview.h \
		include/cuvcommonstructs.h \
		include/cuvtags.h \
		include/lessalloc.h \
		pixmaplabel.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ pixmaplabel.h -o moc_pixmaplabel.cpp

moc_showscene.cpp: ui_showscene.h \
		showscene.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ showscene.h -o moc_showscene.cpp

moc_globalpreferences.cpp: ui_globalpreferences.h \
		globalpreferences.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ globalpreferences.h -o moc_globalpreferences.cpp

moc_getimagesize.cpp: ui_getimagesize.h \
		getimagesize.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ getimagesize.h -o moc_getimagesize.cpp

moc_moviedialog.cpp: ui_moviedialog.h \
		moviedialog.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ moviedialog.h -o moc_moviedialog.cpp

moc_autoexport.cpp: ui_autoexport.h \
		autoexport.h
	/Developer/Tools/Qt/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__ autoexport.h -o moc_autoexport.cpp

compiler_rcc_make_all: qrc_images.cpp qrc_glshaders.cpp qrc_htmlFiles.cpp
compiler_rcc_clean:
	-$(DEL_FILE) qrc_images.cpp qrc_glshaders.cpp qrc_htmlFiles.cpp
qrc_images.cpp: images.qrc
	/Developer/Tools/Qt/rcc -name images images.qrc -o qrc_images.cpp

qrc_glshaders.cpp: glshaders.qrc
	/Developer/Tools/Qt/rcc -name glshaders glshaders.qrc -o qrc_glshaders.cpp

qrc_htmlFiles.cpp: htmlFiles.qrc
	/Developer/Tools/Qt/rcc -name htmlFiles htmlFiles.qrc -o qrc_htmlFiles.cpp

compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_uic_make_all: ui_mainwindow.h ui_viewersettings.h ui_printpreview.h ui_showscene.h ui_globalpreferences.h ui_getimagesize.h ui_moviedialog.h ui_autoexport.h
compiler_uic_clean:
	-$(DEL_FILE) ui_mainwindow.h ui_viewersettings.h ui_printpreview.h ui_showscene.h ui_globalpreferences.h ui_getimagesize.h ui_moviedialog.h ui_autoexport.h
ui_mainwindow.h: mainwindow.ui
	/Developer/Tools/Qt/uic mainwindow.ui -o ui_mainwindow.h

ui_viewersettings.h: viewersettings.ui
	/Developer/Tools/Qt/uic viewersettings.ui -o ui_viewersettings.h

ui_printpreview.h: printpreview.ui
	/Developer/Tools/Qt/uic printpreview.ui -o ui_printpreview.h

ui_showscene.h: showscene.ui
	/Developer/Tools/Qt/uic showscene.ui -o ui_showscene.h

ui_globalpreferences.h: globalpreferences.ui
	/Developer/Tools/Qt/uic globalpreferences.ui -o ui_globalpreferences.h

ui_getimagesize.h: getimagesize.ui
	/Developer/Tools/Qt/uic getimagesize.ui -o ui_getimagesize.h

ui_moviedialog.h: moviedialog.ui
	/Developer/Tools/Qt/uic moviedialog.ui -o ui_moviedialog.h

ui_autoexport.h: autoexport.ui
	/Developer/Tools/Qt/uic autoexport.ui -o ui_autoexport.h

compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean compiler_rcc_clean compiler_uic_clean 

