/**

  */
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qvariant.h>
#include <qwidget.h>
#include <qtimer.h>
#include <qstringlist.h>
#include <qspinbox.h>
#include <QScrollArea>
#include <qprinter.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qmessagebox.h>
#include <qmainwindow.h>
#include <qlistwidget.h>
#include <qinputdialog.h>
#include <qimage.h>
#include <QFileDialog>
#include <qcheckbox.h>
#include <qapplication.h>
#include <qtoolbutton.h>
#include <qlabel.h>
#include <qimage.h>
#include <qpixmap.h>
#include <QStatusBar>
#include <QDataStream>
#include <QImageWriter>
#include <QPrintDialog>
#include <QColorDialog>
#include <QPaintDevice>
#include <QDesktopServices>
#include <QUrl>

#ifdef Q_WS_WIN
#  include <math.h>
#  include <process.h>
#else
#  include <cmath>
#  include <unistd.h>
#endif

#include "binpalettes.h"
#include "imageprocessing.h"

#include "viewdata.h" //viewerdata: data structures for global preferences and etc
#include "viewersettings.h"

#include "autoexport.h"
#include "getimagesize.h"
#include "showscene.h"
#include "moviedialog.h"
#include "globalpreferences.h"
#include "cuviewdoc/cuviewdoc.h" //the OpenGL window
#include "config.h"
#include "helpwindow.h"
#include "script.h"
#include "printpreview.h"

/**
   Constructs a MainWindow as a child of 'parent', with the
   name 'name' and widget flags set to 'f'.
 */
MainWindow::MainWindow(QWidget* parent)
  : QMainWindow(parent)
{
    setupUi(this);

    (void)statusBar();
    init();
}

/**
   Destroys the object and frees any allocated resources
 */
MainWindow::~MainWindow()
{
  destroy();
}

/**
   Sets the strings of the subwidgets using the current
   language.
 */
void MainWindow::languageChange()
{
  retranslateUi(this);
}

/**
    @brief Initializes the main window.
  */
void MainWindow::init() //NOTE: init is run as the last command in mainwindow.cpp.
{
  timeout=500; //for movie, autoexport image.
//the opengl wrapper (a subclassed qmainwindow, with qglwidget as it's centralwidget)
  cuviewDoc = NULL;
  printer = NULL;
  timer = NULL; //movie, autoexport image
  filesOnStartup = FALSE; //ie. at prompt: cuviewer somefile.cuv, see main.cpp
  QScrollArea* scrollArea = new QScrollArea(viewerSettingsDockWidget);
  vs = new ViewerSettings(scrollArea); //gui
  ip = new ImageProcessing(this, vs); //image export, print, movie
  showSceneDialog = NULL;
  currentscene = -2; //this increments twice during initialization

  //add viewersettings widget to mainwindow through scrollArea widget
  viewerSettingsDockWidget->setWidget(scrollArea);
  scrollArea->resize(vs->size()); //Stretches scroll area to avoid any scroll bars
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setWidget(vs);
  scrollArea->show();
  vs->show();
  vs->setMouseTracking(TRUE);

  qApp->installEventFilter(this);

  //load cuviewrc
  setPrefData( getPreferences() );

  ip->setImageFile(filedata.autoimagefile,filedata.imagefile);
  ip->setStartFolder(startfolder.html,startfolder.image);
  bp = new BinPalettes(this, prefdata.binPalettes, vs);

  int w,h,x,y; //width and height of window; x,y position of top left corner of window
  //load cuviewer workspace settings file
  QFile file(QDir::home().path()+"/.cuviewer/cuviewer");
  if (QFile::exists(QDir::home().path()+"/.cuviewer/cuviewer")){
    if (file.open(QIODevice::ReadOnly)){
      QTextStream ts(&file);
      QString pad;
      int value;
      ts.readLine();
      ts >> pad >> value;
      slotViewerSettings(!value);
      ts >> pad >> x >> y >> w >> h;
      ts >> pad >> prefdata.ssd_x >> prefdata.ssd_y
         >> prefdata.ssd_dx >> prefdata.ssd_dy;
      file.close();
      if (prefdata.restoreWS){
        move(x,y);
        resize(w,h);
      }
    }
    else { //file could not be opened
      QMessageBox::information(this, "Can't open 'cuviewer'.",
           "Couldn't read file, check file permissions");
    }
  }
  else {
    qDebug("File doesn't exist. Settings not loaded.");
  }

  connectSlots();
}

void MainWindow::destroy()
{
}

/**
    @brief Set checkboxes, sliders, menus

    The setViewerSettingsActions method sets all the checkboxes,
    sliders, and menus to the cuviewDoc's settings.
  */
void MainWindow::setViewerSettingsActions()
{
  if (!cuviewDoc)
    return;

  outline->setChecked(cuviewDoc->outline());
  twoSided->setChecked(cuviewDoc->twoSided());
  shading->setChecked(cuviewDoc->smoothShaded());
  antialiasing->setChecked(cuviewDoc->antialias());
  transparency->setChecked(cuviewDoc->transparency());
  perspective->setChecked(cuviewDoc->perspective());
  fog->setChecked(cuviewDoc->fog());
  gamma->setChecked(cuviewDoc->gamma());

  wireframe->setChecked(cuviewDoc->wireframe());
  opaquewireframe->setChecked(cuviewDoc->opaqueWireframe());
  lighting->setChecked(cuviewDoc->lighting());
  diffuse->setChecked(cuviewDoc->light());
  binnedPalette->setChecked(cuviewDoc->binWindow());

  showBoundingBoxAction->setChecked(cuviewDoc->boxWhileMoving());
  showBoxAlwaysAction->setChecked(cuviewDoc->drawBoxAlways());
  drawAxisAction->setChecked(cuviewDoc->drawAxis());

  setClippingValue((int)(cuviewDoc->clipPerspNear()*20));
  setFOVValue((int)cuviewDoc->fov());
  setOutlineValue((int)cuviewDoc->lineWidth());
  setTesselValue((int)cuviewDoc->sphereTess());
  setAmbientValue((int)(cuviewDoc->ambient()*100));
  setDiffuseValue((int)(cuviewDoc->diffuse()*100));
  setBackgroundValue((int)(cuviewDoc->background()*100));
  setFogValue((int)(cuviewDoc->fogLevel()*200));
  setGammaValue((int)cuviewDoc->gammaLevel()*10);
  setSpecular((int)cuviewDoc->specular()*4);

  vs->editLightListBox->clearSelection();
}

/* Connecting the gui to uninitialized objects (like ip, vs, bp) tend to crash
the program :-(   therfore, connect() is called after the initialization step.
   For gui connections to class:MainWindow, connections are programmed into
mainwindow.ui (go through qt designer or by hand).
*/
void MainWindow::connectSlots()
{
  //SIGNAL(clicked())
  connect(vs->outlineColor,SIGNAL(clicked()),
      this,SLOT(slotOutlineColor()));
  connect(vs->paletteIndexSpinBox,SIGNAL(valueChanged(int)),
      bp,SLOT(setPaletteIndex(int)));

  //SIGNAL(toggled(bool))
  connect(vs->outlineCheck,SIGNAL(toggled(bool)),
      outline,SLOT(setChecked(bool)));
  connect(vs->twoSidedCheck,SIGNAL(toggled(bool)),
      twoSided,SLOT(setChecked(bool)));
  connect(vs->shadingCheck,SIGNAL(toggled(bool)),
      shading,SLOT(setChecked(bool)));
  connect(vs->antialiasingCheck,SIGNAL(toggled(bool)),
      antialiasing,SLOT(setChecked(bool)));
  connect(vs->transparencyCheck,SIGNAL(toggled(bool)),
      transparency,SLOT(setChecked(bool)));
  connect(vs->perspectiveCheck,SIGNAL(toggled(bool)),
      perspective,SLOT(setChecked(bool)));
  connect(vs->fogCheck,SIGNAL(toggled(bool)),
      fog,SLOT(setChecked(bool)));
  connect(vs->gammaCheck,SIGNAL(toggled(bool)),
      gamma,SLOT(setChecked(bool)));
  connect(vs->wireframeCheck,SIGNAL(toggled(bool)),
      wireframe,SLOT(setChecked(bool)));
  connect(vs->opaquewireframeCheck,SIGNAL(toggled(bool)),
      opaquewireframe,SLOT(setChecked(bool)));
  connect(vs->lightingCheck,SIGNAL(toggled(bool)),
      lighting,SLOT(setChecked(bool)));
  connect(vs->diffuseCheck,SIGNAL(toggled(bool)),
      diffuse,SLOT(setChecked(bool)));
  connect(vs->binnedPaletteCheck,SIGNAL(toggled(bool)),
      binnedPalette,SLOT(setChecked(bool)));
  connect(vs->binnedPaletteCheck,SIGNAL(toggled(bool)),
      paletteExportImage,SLOT(setChecked(bool)));

  connect(paletteExportImage,SIGNAL(toggled(bool)),
    vs->paletteExportImageCheckBox,SLOT(setChecked(bool)));
  connect(vs->paletteExportImageCheckBox,SIGNAL(toggled(bool)),
    paletteExportImage,SLOT(setChecked(bool)));

  connect(binnedPaletteColor,SIGNAL(activated()),
    vs->bwBinPalette,SLOT(toggle())); //QToolButton
  connect(binnedPaletteColor,SIGNAL(activated()),
      bp,SLOT(setPalette()));

  connect(vs->bpStartValueSpinBox,SIGNAL(valueChanged(int)),
      bp,SLOT(setStartPaletteValue(int)));
  connect(vs->bpEndValueSpinBox,SIGNAL(valueChanged(int)),
      bp,SLOT(setEndPaletteValue(int)));
  connect(vs->bpStart,SIGNAL(sliderReleased()),
      bp,SLOT(setBinPaletteStart()));
  connect(vs->bpEnd,SIGNAL(sliderReleased()),
      bp,SLOT(setBinPaletteEnd()));
  connect(vs->bwBinPalette,SIGNAL(clicked()),
      bp,SLOT(setPalette()));
  connect(vs->colorBinPalette,SIGNAL(clicked()),
      bp,SLOT(setPalette()));
  connect(vs->prevBinPaletteTB,SIGNAL(clicked()),
      vs->paletteIndexSpinBox,SLOT(stepDown()));
  connect(vs->nextBinPaletteTB,SIGNAL(clicked()),
      vs->paletteIndexSpinBox,SLOT(stepUp()));

  connect( viewerSettingsDockWidget,SIGNAL(visibilityChanged(bool)),
     this,SLOT(slotViewerSettings(bool)));
  //SIGNAL(valueChanged(int))
  connect(vs->outlineValue,SIGNAL(valueChanged(int)),
      this,SLOT(setOutlineValue(int)));
  connect(vs->presetNumber,SIGNAL(valueChanged(int)),
      this,SLOT(gotoViewpoint(int)));
  connect(vs->zeroClippingPlaneButton,SIGNAL(clicked()),
      this,SLOT(setZeroClipping()));
  connect(vs->clippingSlider,SIGNAL(valueChanged(int)),
      this,SLOT(setClippingValue(int)));
  connect(vs->fogSlider,SIGNAL(valueChanged(int)),
    this,SLOT(setFogValue(int)));
  connect(vs->gammaSlider,SIGNAL(valueChanged(int)),
      this,SLOT(setGammaValue(int)));
  connect(vs->fovSlider,SIGNAL(valueChanged(int)),
      this,SLOT(setFOVValue(int)));
  connect(vs->sphereTessSlider,SIGNAL(valueChanged(int)),
      this,SLOT(setTesselValue(int)));
  connect(vs->ambientSlider,SIGNAL(valueChanged(int)),
      this,SLOT(setAmbientValue(int)));
  connect(vs->diffuseSlider,SIGNAL(valueChanged(int)),
      this,SLOT(setDiffuseValue(int)));
  connect(vs->backgroundSlider,SIGNAL(valueChanged(int)),
      this,SLOT(setBackgroundValue(int)));
  connect(vs->blackBackgroundButton,SIGNAL(clicked()),
      this,SLOT(setBackgroundBlack()));
  connect(vs->whiteBackgroundButton,SIGNAL(clicked()),
      this,SLOT(setBackgroundWhite()));
  connect(vs->resetPositionPushButton,SIGNAL(clicked()),
    this,SLOT(resetLightPosition()));
  connect(vs->fixedSourceCheckBox,SIGNAL(toggled(bool)),
    this,SLOT(setFixedLight(bool)));
  connect(vs->editLightListBox,SIGNAL(itemSelectionChanged()),
    this,SLOT(setLightEditing()));

  connect(vs->lightSourceDial,SIGNAL(valueChanged(int)),
    this,SLOT(setLightPosition()));
  connect(vs->lightPitch,SIGNAL(valueChanged(int)),
    this,SLOT(setLightPosition()));
  connect(vs->lightColor,SIGNAL(clicked()),
      this,SLOT(slotLightColor()));

  connect(vs->specularSlider,
    SIGNAL(valueChanged(int)),SLOT(setSpecular(int)));

  connect(vs->paletteIndexSpinBox,SIGNAL(valueChanged(int)),
    vs->paletteView,SLOT(repaint()));
  connect(cyclebinpalettenext,SIGNAL(activated()),
    vs->paletteIndexSpinBox,SLOT(stepUp()));
  connect(vs->paletteIndexSpinBox,SIGNAL(valueChanged(int)),
    vs->paletteView,SLOT(repaint()));
  connect(cyclebinpaletteprev,SIGNAL(activated()),
    vs->paletteIndexSpinBox,SLOT(stepDown()));

  connect(vs->forwardPlayEndButton,SIGNAL(clicked()),
    SLOT(forwardPlayEnd()));
  connect(vs->reversePlayEndButton,SIGNAL(clicked()),
    SLOT(reversePlayEnd()));

  connect(vs->stopPlayEndButton,SIGNAL(clicked()),
    SLOT(stopPlay()));

  connect(exportImageAction, SIGNAL(activated()),
    ip,SLOT(exportImage()));
  connect(exportImageAtSize, SIGNAL(activated()),
    ip,SLOT(exportImageGetSize()));
  connect(autoExportImageAction, SIGNAL(toggled(bool)),
    ip,SLOT(autoExportImage(bool)));
}

/**
    @brief Previews print page and then sends the print command.

    The filePrint method previews the print page and then sends the print command.
    If the program does not know if the printer exists then filePrintSetup() is
    called to get the printer. A PrintPreview dialog is then opened and the dialog
    gives the user the opportunity to manipulate the to-be-printed cuviewDoc. Once
    the user selects Ok then the method prints an image of the cuviewDoc using the printer
    provided.

    @param width  the desired width of the printed cuviewDoc
    @param height the desired height of the printed cuviewDoc
  */
void MainWindow::filePrint( int width, int height )
{
  if (!cuviewDoc)
    return;

  //Check if printer exists
  bool hasPrinter = printer || filePrintSetup();
  if ( !hasPrinter ) return;
  ip->setPaintPalette(false);
  QPixmap pm = ip->getPixmap(width,height,FALSE);
  ip->setPaintPalette(true);
  if (pm.isNull()) return;

  PrintPreview view( this );
  QImage temp = pm.toImage();
  if ( temp.height()>640 || temp.width()>480 )
    temp = temp.scaled(480, 640, Qt::KeepAspectRatio, Qt::SmoothTransformation);

  //scale down to fit
  QPixmap temppix;
  temppix.convertFromImage( temp );
  view.setPixmap( temppix );
  if (vs->paletteExportImageCheckBox->isChecked()){
    view.setPaletteRect( 0, 0, 50, 350 );
    view.setPaletteView( vs->paletteView );
  }
  view.setWindowIcon(QPixmap("opengl/snapshot.xpm"));
  view.setWindowTitle("Preview");
  view.updatePreview();

  if ( view.exec() ) {
    QImage img = pm.toImage();
    //process image
    if ( view.checkInvert->isChecked() )
      img.invertPixels();
    if ( view.checkMirror->isChecked() )
      img = img.mirrored( TRUE, FALSE );
    if ( view.checkFlip->isChecked() )
      img = img.mirrored( FALSE, TRUE );
    pm.convertFromImage( img );
  }
  else return;

  //pass a pixmap (a paintdevice), paint palette on pixmap.
  QRect pr = view.getPaletteRect();
  vs->paletteView->paintPalette(&pm,pr.width(),pr.height(),pr.x(),pr.y());

  //set manual feed
  if (view.manualfeed->isChecked())
    printer->setPaperSource(QPrinter::Manual);

  //adjust pixmap to page size  (and keep aspect ratio)
  QImage img = pm.toImage();
  if (img.width()>printer->width() || img.height()>printer->height() ||
     view.maximum->isChecked())
  {
    img = img.scaled( printer->width() - view.sidemargins->value(),
                      printer->height() - view.topbottommargins->value(),
                      Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }
  QPoint point(0, 0);
  if (view.centered->isChecked()){
    point = QPoint( (printer->width()-img.width())/2,
                    (printer->height()-img.height())/2 );
  }

  QPainter painter;
  if ( !painter.begin( printer ) )
    return;

  painter.drawImage( point , img );
  painter.end();
}

void MainWindow::filePrintWindowRes()
{
  if (!cuviewDoc)
    return;
  filePrint( 0, 0 );
}

void MainWindow::filePrintLowRes()
{
  if (!cuviewDoc)
    return;
  filePrint( 640, 480 );
}

void MainWindow::filePrintMedRes()
{
  if (!cuviewDoc)
    return;
  filePrint( 1024, 768 );
}

void MainWindow::filePrintHighRes()
{
  if (!cuviewDoc)
    return;
  filePrint( 2048, 1536 );
}

//print setup screen
/**
    @brief Shows a printer dialog and gets the printer.

    The method filePrintSetup displays a printer dialog that asks the user which printer to use.
    The user's selection is remembered for the duration of the program's life.

    @return True if a printer was retrieved from user
    @return False if user cancelled request for printer
  */
bool MainWindow::filePrintSetup()
{
  if (!cuviewDoc)
    return false;

  bool newPrinter = !printer;
  if ( newPrinter ){ //make new printer
    printer = new QPrinter;
    //set some reasonable printer defaults
//    printer->setPrintProgram("lp");
    printer->setPageSize(QPrinter::Letter);
    printer->setColorMode(QPrinter::Color);
  }
  QPrintDialog pDialog(printer);
  if ( pDialog.exec() )
    return true;
  else {
    if ( newPrinter ) {
      delete printer;
      printer = 0;
    }
    return false;
  }
  return false;
}

//load list of filenames in separate views
//used on cuviewer application startup only
/**
    @brief Loads a list of filenames in separate views.

    The method loadSeparately loads a list of filenames into separate windows.
    This method can only be used at cuviewer application startup (i.e. at terminal or command-line)

    @param files  a list of QStrings containing filenames
  */
void MainWindow::loadSeparately( QStringList files ){
    if (files.count() > 0){
      filesOnStartup = TRUE;
    }
    for( int i=0; i<files.count(); i++){
      if (!cuviewDoc) load( & files[i] );
      else loadNewView( & files[i] );
    }
}

//load a list of filenames into a single view
/**
    @brief Loads a list of filenames into a single view.

    @param files  a list of QStrings containing filenames
  */
void MainWindow::loadAllInOne( QStringList files ){
  for(int i=0;i<files.count();i++){
    if (!QFile::exists(files[i])){
      qWarning("Script Warning - can't open: %s ", qPrintable(files[i]));
      statusBar()->showMessage(QString("Error loading cuvfile from script"));
    }
    else{
      if (cuviewDoc)
        fileOpenMerge(& files[i] );
      else
        load( & files[i] );
    }
  }
}

/**
    @brief Loads another file called loadthisfile into cuviewer or
           opens a dialog asking for the file if name of file was not given.

    The method loadNewView opens another window with a file
    called loadthisfile loaded into cuviewer.
    If loadthisfile is empty then a file dialog is opened asking
    for a .gl or .cuv file. The method only picks the first file
    selected by the user.

    This method will be called only if a CUViewDoc file is already opened.

    @param loadthisfile  the file to be loaded
  */
void MainWindow::loadNewView( QString * loadthisfile ){
  QString fn; //File name

  if (!loadthisfile){
    QFileDialog qfd(this, "Open cuviewer,gl file", startfolder.gl,
                    "CUViewer files (*.cuv *.cuv.* *.gl *.gl.*)");
    if (qfd.exec())
      fn = qfd.selectedFiles().at(0);
    startfolder.gl = qfd.directory().path();
  }else{
    fn = *loadthisfile;
  }
  if (fn.isEmpty())
    return;

  statusBar()->showMessage("Loading file: " + fn);
  MainWindow* mw = new MainWindow();
  mw->load(&fn);
  mw->show();
}

/**
    @brief Loads file if filename points to a valid file.

    The load method loads a file called loadthisfile into cuviewDOC if the file is valid.
    If loadthisfile name is '0' or does not exist then a file dialog opens and asks the user for
    a file.
    The method then prepares the mainwindow and panel view. Finally it loads the scriptfile and
    bin window palettes if available.

    @param loadthisfile
  */
void MainWindow::load( QString * loadthisfile ) {
  //load filename into cuviewDOC, prepare mainwindow and panel view
  //finally loads scriptfile and bin window palettes if available

  if (!loadthisfile){ //Opens file dialog if no file was given.
    QString filter("CUViewer files (*.cuv *.cuv.* *.gl *.gl.*)");
    QFileDialog qfd(this, "Open cuviewer,gl file", startfolder.gl, filter);
    if (qfd.exec())
      filedata.cuvfile = qfd.selectedFiles().at(0);
    else
      return;
    startfolder.gl = qfd.directory().path();
  }
  else
    filedata.cuvfile = *loadthisfile;

  if (!filedata.cuvfile.isEmpty()) {
    statusBar()->showMessage("Loading file: " + filedata.cuvfile);
    qDebug("Loading file: %s", qPrintable(filedata.cuvfile));
    QFile *fileIn = new QFile(filedata.cuvfile);
    if (!fileIn->open(QIODevice::ReadOnly)) {
      qWarning("Opening file failed, check read permissions");
      return;
    }

    QGLFormat * glformat;
    QGL::FormatOptions glflags = QGL::DoubleBuffer | QGL::DepthBuffer |
                                 QGL::Rgba | QGL::NoAlphaChannel |
                                 QGL::AccumBuffer | QGL::NoStencilBuffer |
                                 QGL::NoStereoBuffers | QGL::NoOverlay;
    if (prefdata.directRendering){
      qDebug("Direct Rendering mode");
      glflags |= QGL::DirectRendering;
    }
    else{
      qDebug("Indirect Rendering mode");
      glflags |= QGL::IndirectRendering;
    }

    glformat = new QGLFormat(glflags);
    glformat->setVersion(3,2);
    glformat->setProfile(QGLFormat::CoreProfile);
    cuviewDoc = new CUViewDoc(glformat, fileIn, this);
#ifdef Q_WS_WIN
    //BUG NOTE: hey why is this here?, this is a hack to fix some memmory allocation problems
    //in sceneenity that causes windows to crash
    //TODO: run this code though a syntax checker like Lint someday...
    new LessAlloc<sphoid>;
#endif

    if (cuviewDoc->initializeData()){
      qDebug("Initialized cuviewDoc");
      QString nameOfFile = filedata.cuvfile.mid(filedata.cuvfile.lastIndexOf('/')+1);
      setWindowTitle( QString("Carleton University Viewer: ")+nameOfFile );
      if (!cuviewDoc->readData()){
        fileIn->close();
        return;
      }
      fileIn->close();
      cuviewDoc->show();
      setCentralWidget( cuviewDoc );

      cuviewDoc->setThreeButtonMouse( prefdata.mouseButtons==3 );
      setDrawBoundingBoxAlways( prefdata.drawBoxAlways );
      setBoundingBox( prefdata.boxModel );
      cuviewDoc->setDrawAxis( prefdata.drawAxis );
      cuviewDoc->setDrawAxisOrigin( prefdata.drawAxisOrigin );

      if (cuviewDoc->getBinPalMax()==-1 || cuviewDoc->getBinPalMin()==-1)
        cuviewDoc->setBinPalMinMax(300.0,330); //set defaults:
      if (cuviewDoc->getBinPalType().isNull())
        cuviewDoc->setBinPalType("K"); //set defaults:

      bp->setCUViewDoc(cuviewDoc);
      bp->insertbinpalette();

      ip->setCUViewDoc(cuviewDoc);
      vs->paletteView->setMaxValue(QString::number(cuviewDoc->getBinPalMax()));
      vs->paletteView->setMinValue(QString::number(cuviewDoc->getBinPalMin()));
      vs->paletteView->setPostFix(cuviewDoc->getBinPalType());
      vs->maxValue->setText(QString::number(cuviewDoc->getBinPalMax()));
      vs->minValue->setText(QString::number(cuviewDoc->getBinPalMin()));
      vs->postFix->setText(cuviewDoc->getBinPalType());

      if (!prefdata.startupscriptfile.isEmpty()){
        qDebug("Startup script file is not empty");
        filedata.scriptfile = prefdata.startupscriptfile; //skips "load script" dialog
        slotLoadPreset();
      }
      else{ //default settings:
        qDebug("Startup script file is empty");
        setViewerSettingsActions();
        setOutlineChecked(FALSE);
        bp->setPaletteIndex(0);
        vs->editLightListBox->clearSelection();
      }
      updateScenes();
      if (prefdata.startUpRefresh)
        QTimer::singleShot(250,cuviewDoc, SLOT(redrawDoc()));
    }
    else{ //Initializing data failed.
      qDebug("Initializing data failed");
      filedata.cuvfile = "";
      delete cuviewDoc;
      cuviewDoc = NULL;
      return;
    }
  }
}

void MainWindow::fileOpenMerge( QString * loadthisfile )
{
  if (!cuviewDoc)
    return;

  QString file;
  if (!loadthisfile){
    QString filter("CUViewer files (*.cuv *.cuv.* *.gl *.gl.*)");
    QFileDialog qfd(this, "Open cuviewer,gl file", startfolder.glmerge, filter);
    if (qfd.exec()){
      //Select only one file, all other selected files are ignored
      file = qfd.selectedFiles().at(0);
    }
    startfolder.glmerge = qfd.directory().path();
  }
  else{
    file = *loadthisfile;
  }
  if (!file.isEmpty()) {
    QFile *fileIn = new QFile(file);
    if (!fileIn->open(QIODevice::ReadOnly)) {
      qWarning("Opening file failed, check read permissions");
      return;
    }
    cuviewDoc->setDevice(fileIn);
    if (cuviewDoc->initializeData()){
      filedata.mergedFiles += fileIn->fileName();
      cuviewDoc->readData();
    }
    if (moviestream.device() &&
        moviestream.device()->openMode() == QIODevice::WriteOnly )
    {
      moviestream << QString("loadmergescene %1 \n").arg(file);
    }
    fileIn->close();
    updateScenes();
  }
}

void MainWindow::loadFileOpenMerge()
{
    fileOpenMerge(0);
}

//reload current file
void MainWindow::reload()
{
  if (!cuviewDoc)
    return;

  //getpid() a unistd.h command, in win32 from process.h
  //winId() is from QWidget
#ifdef Q_WS_WIN
  filedata.scriptfile = QString("cuviewertemp%1-%2")
                        .arg(getpid()).arg((int)winId());
#else
  filedata.scriptfile = QString("/tmp/cuviewertemp%1-%2")
                        .arg(getpid()).arg((int)winId());
#endif
  prefdata.startupscriptfile = filedata.scriptfile;
  QFile * file = new QFile(filedata.scriptfile);
  Script s(file,cuviewDoc);
  QStringList filelist(filedata.cuvfile);
  filelist += filedata.mergedFiles;
  s.setFilenameList(filelist);
  s.writeScript();
  file->close(); //finish writing script

  filedata.mergedFiles.clear();
  cuviewDoc->hide();
  setCentralWidget( 0 );
  cuviewDoc->close();
  cuviewDoc = NULL;
  load(&(filedata.cuvfile)); //loads filedata.scriptfile
  file->remove();  //then it's okay to remove the temporary file.
  filedata.scriptfile="";
  prefdata.startupscriptfile="";
}

/** @fn slotOpen
    @brief Opens a CUViewDoc file.
  */
void MainWindow::slotOpen()
{
  if (!cuviewDoc)
    load(0);
  else
    loadNewView(0);
}

/**
    @brief Opens a message displaying a description of the original author and the program.
  */
void MainWindow::slotAbout()
{
  QMessageBox::about( this, tr("Carleton 3D Viewer ")+VERSION,
          tr("<center>This 3D viewer was made by"
             "<br>Carleton University Dept. of Electronics</br>"
             "<br>http://www.doe.carleton.ca/~bwan/cuview</br>"
             "<br>http://cuviewer.sf.net</br></center>"));
}

/**
    @brief Opens the default web browser to the contents of an HTML file displaying
           in-depth documentation and manuals of the cuviewer program.

    The slot slotHelpContents opens the user's default web browser displaying the
    contents of an HTML file that displays in-depth documentation and manuals on
    the cuviewer program.

    A warning message is displayed if it is unable to find the HTML file.

    @warning slotHelpContents opens the file "index.html" of whatever folder that
             was set as the folder containing the documents in QString "prefdata.docPath".
  */
void MainWindow::slotHelpContents()
{
  QUrl url("file:///" + prefdata.docPath + "/index.html");
  qDebug("Opening url: %s", qPrintable(url.toString()));
  if (!QDesktopServices::openUrl(url)){
    QMessageBox msgbox;
    msgbox.setIcon(QMessageBox::Warning);
    msgbox.setText("Opening documentation failed: "
                   "Check that you set the right folder in global preferences.");
    msgbox.exec();
  }
}

/**
    @brief Displays tips to using CUViewer

    The quickHelp method opens a window with a textbox which displays the keyboard and
    mouse shortcuts used for viewing the model
    as well as instructions on how to use a movie script file, how
    to edit scenes, and how to use CUViewer via the command prompt.
  */
void MainWindow::quickHelp()
{
  QMainWindow * qHelp = new QMainWindow(this);
  QTextEdit * te = new QTextEdit("helptext", qHelp);
  qHelp->setCentralWidget(te);
  qHelp->setWindowTitle("CUViewer Quick Help");
  qHelp->resize( 450, 600 );
  qHelp->show();
  te->setText(QUICK_HELP);
}

/**
    @brief Closes the window when close is clicked or
           'ctrl + w' or 'ctrl + q' is pressed

    The slotClose method closes the CUViewDoc file and writes to a file called
    "cuviewer" all of the workspace settings for the CUViewer program.
    Workspace settings saved to file are
      - whether Viewer Settings dock is shown or hidden
      - position and size of the MainWindow
      - position and size of the ShowScene dialog

    This method is called when the Close action is selected or when
    'Ctrl + W' or 'Ctrl + Q' is pressed (Cmd+W and Cmd+Q for Mac).
  */
void MainWindow::slotClose() //closes window
{
  closeFile();
  //write to $home/.cuview/cuviewer
  QDir cuview;
  if (!cuview.exists(QDir::homePath()+"/.cuviewer")){
    cuview.mkdir(QDir::homePath()+"/.cuviewer");
  }
  QFile *file = new QFile(QDir::homePath()+"/.cuviewer/cuviewer");
  if (file->exists())
      file->remove();
  file->setFileName(QDir::homePath()+"/.cuviewer/cuviewer");
  if (file->open(QIODevice::ReadWrite)){
    QTextStream ts(file);
    ts << QString("Workspace information for cuviewer, do not edit.\n");
    //Write 1 if ViewerSettings dock hidden, otherwise write 0
    ts << QString("vs_dock_hidden %1\n")
            .arg((int)viewerSettingsDockWidget->isHidden());
#ifdef SOLARIS
    ts << QString("mainwindow_size %1 %2 %3 %4\n")
                  .arg(x()-6).arg(y()-29).arg(width()).arg(height());
#else
    ts << QString("mainwindow_size %1 %2 %3 %4\n")
                  .arg(x()).arg(y()).arg(width()).arg(height());
#endif
    if (prefdata.ssd_x<0 || prefdata.ssd_y<0 ||
       prefdata.ssd_dx<0 ||prefdata.ssd_dy<0){
      //use defaults
      prefdata.ssd_x=0;
      prefdata.ssd_y=0;
      prefdata.ssd_dx=561;
      prefdata.ssd_dy=433;
    }
    ts << QString("showscenedialog %1 %2 %3 %4\n")
      .arg(prefdata.ssd_x)
      .arg(prefdata.ssd_y)
      .arg(prefdata.ssd_dx)
      .arg(prefdata.ssd_dy);
  }
  else{
    qWarning("can't save general preferences to file");
  }
  file->close();
  this->close();
  qDebug("Quitting cuviewer");
}

//activated by ctrl+c
void MainWindow::closeFile()
{
#ifdef Q_WS_WIN
//'delete cuviewDoc' crashes in windows, if file is loaded during startup
  if (filesOnStartup)
    return;
#endif
  prefdata.startupscriptfile="";
  setWindowTitle("Carleton University Viewer");
  delete cuviewDoc;
  cuviewDoc = NULL;
  bp->setCUViewDoc(NULL);
  ip->setCUViewDoc(NULL);
  filedata.mergedFiles.clear();

  if (showSceneDialog)
    showScenes();
  vs->editLightListBox->clearSelection();
}

void MainWindow::slotExit()
{
  slotClose();
  qApp->closeAllWindows();
}

/**
    @brief Creates a movie script file

    The slotCreateMovie method opens a file dialog asking for a name for the
    movie script file.

    Once a file name has been aquired a MovieDialog dialog window is shown
    asking for all script settings.

    Then when the script settings are aquired the method begins to prepare the
    script by writing all essential information related to the cuviewDoc object
    and starts the timer for a period given by the user during the MovieDialog
    dialog.
  */
void MainWindow::slotCreateMovie()
{
  if (!cuviewDoc){
    qWarning("can't make movie. open a file first");
    return;
  }

  //Open a file dialog asking for filename
  QFileDialog qfd(this, "Open movie script file",
                  startfolder.moviescript, "Movie Scripts (*)");
  qfd.setAcceptMode(QFileDialog::AcceptSave);
  QString filename;

  //Get filename from user and save folder path
  if (qfd.exec()){
    //Only allow one file to be selected
    filename = qfd.selectedFiles().at(0);
  }
  startfolder.moviescript = qfd.directory().path();
  if ( filename.isEmpty() ){
    return;
  }

  //Open a MovieDialog dialog and get all required info
  MovieDialog * md = new MovieDialog();
  md->show();
  md->moviefileLineEdit->setText(filename);
  if (md->exec() == QDialog::Accepted){
    moviefile = new QFile(md->moviefileLineEdit->text());
    if (!moviefile->open(QIODevice::ReadWrite)){
      qWarning("can't make moviefile, check write permission in this directory");
      return;
    }
    //Create and setup the script
    Script script(moviefile, cuviewDoc);
    QStringList filelist(filedata.cuvfile);
    filelist += filedata.mergedFiles;
    script.setFilenameList(filelist);
    script.writeScript();
    moviestream.setDevice(moviefile);
    moviedata.exportImages = md->exportImagesCheck->isChecked();
    moviedata.recordMovement = md->recordMovementCheck->isChecked();
    moviedata.movieframes = md->timeLimitSpinBox->value() * md->fpsSpinBox->value();
    moviestream << QString("Movie_Script_begins_here\n");
    //setup timer
    timer = new QTimer(this); //movie timer
    connect (timer, SIGNAL(timeout()), this, SLOT(slotWriteMovieFrame()));
    timer->start(1000/md->fpsSpinBox->value());
  }
}

/**
    @brief Reads a movie script file

  */
void MainWindow::slotReadMovieFile( QString filename )
{
  QFile * file = new QFile(filename);
  if (!file->exists()) {
    qWarning("%s doesn't exist, can't read movie file", qPrintable(filename));
    return;
  }

  Script s(file,cuviewDoc);
  s.readScript();
  QStringList templist = s.getFilenameList(), tomergelist;
  QStringList::Iterator it = templist.begin();
  qDebug("%s", qPrintable(filedata.cuvfile));
  qDebug("%s", qPrintable(*it));
  if (filedata.cuvfile!=*it){
    qWarning("warning scriptfile does not have the original cuvfiles");
  }
  for (++it; it != templist.end(); ++it ){ //omit first cuvfile
    tomergelist+=*it;
  }
  loadAllInOne(s.getFilenameList());
  setViewerSettingsActions();

  GLfloat toFill[3];
  GLfloat toFill2[3];
  QString pad;
  double value; //Used to store bool and int as well.
  QString str;
  moviestream.setDevice(file);
  int frame=0;
  while(!moviestream.atEnd()){
    moviestream >> pad;
    if (pad.contains("export")){
      //export image
      qDebug("frame %i",frame);
      slotExportImageFile( file->fileName() +
                           QString::number(frame).rightJustified(6,'0') +
                           "." + prefdata.imageFormat );
      ++frame;
    }else if (pad.contains("camera_position")){
      moviestream >> toFill[0] >> toFill[1] >> toFill[2];
      cuviewDoc->cameraTranslate(toFill,TRUE);
    }else if (pad.contains("camera_aim")){
      moviestream >> toFill[0] >> toFill[1] >> toFill[2];
    }else if (pad.contains("camera_up")){
      moviestream >> toFill2[0] >> toFill2[1] >> toFill2[2];
      cuviewDoc->setCameraAimAndUp(toFill,toFill2);
    }else if (pad.contains("outlinecolor")){
      moviestream >> toFill[0] >> toFill[1] >> toFill[2];
    }else if (pad.contains("outline")){
      moviestream >> value;
      cuviewDoc->setOutline((bool)value);
    }else if (pad.contains("twosided")){
      moviestream >> value;
      cuviewDoc->setTwoSided((bool)value);
    }else if (pad.contains("aa")){
      moviestream >> value;
      cuviewDoc->setAntialias((bool)value);
    }else if (pad.contains("view")){
      moviestream >> value;
      cuviewDoc->setPerspective((bool)value);
    }else if (pad.contains("smoothshaded")){
      moviestream >> value;
      cuviewDoc->setSmoothShaded((bool)value);
    }else if (pad.contains("transparency")){
      moviestream >> value;
      cuviewDoc->setTransparency((bool)value);
    }else if (pad.contains("fog")){
      moviestream >> value;
      cuviewDoc->setFog((bool)value);
    }else if (pad.contains("gamma")){
      moviestream >> value;
      cuviewDoc->setGamma((bool)value);
    }else if (pad.contains("wireframe")){
      moviestream >> value;
      cuviewDoc->setWireframe((bool)value);
    }else if (pad.contains("opaquewf")){
      moviestream >> value;
      cuviewDoc->setOpaqueWireframe((bool)value);
    }else if (pad.contains("lighting")){
      moviestream >> value;
      cuviewDoc->setLighting((bool)value);
    }else if (pad.contains("diffuse")){
      moviestream >> value;
      cuviewDoc->setLight((bool)value);
    }else if (pad.contains("binWindow")){
      moviestream >> value;
      cuviewDoc->setBinWindow((bool)value);
    }else if (pad.contains("linewidth")){
      moviestream >> value;
      cuviewDoc->setLineWidth((int)value);
    }else if (pad.contains("spheretess")){
      moviestream >> value;
      cuviewDoc->setSphereTess((unsigned char)(int)value);
    }else if (pad.contains("clipping")){
      moviestream >> value;
      cuviewDoc->setClipPerspNear(value);
      cuviewDoc->setClipOrthoNear(value/5.0);
    }else if (pad.contains("fov")){
      moviestream >> value;
      cuviewDoc->setFov(value);
    }else if (pad.contains("ambientv")){
      moviestream >> value;
      cuviewDoc->setAmbient(value);
    }else if (pad.contains("diffusev")){
      moviestream >> value;
      cuviewDoc->setDiffuse(value);
    }else if (pad.contains("background")){
      moviestream >> value;
      cuviewDoc->setBackground(value);
    }else if (pad.contains("gammav")){
      moviestream >> value;
      cuviewDoc->setGamma(value);
    }else if (pad.contains("binwindowindex")){
      moviestream >> value;
      cuviewDoc->setBinWindowIndex((int)value);
      bp->setPaletteIndex((int)value);
    }else if (pad.contains("scenes")){
      moviestream >> value;
      int scenes = (int)value;
      for(int i=0;i<scenes;i++){
        moviestream >> value;
        cuviewDoc->setSceneVisible(i,(bool)value);
      }
    }else if (pad.contains("loadmergescene")){
      moviestream >> str;
      QString * tempFile = new QString();
      *tempFile = str;
      fileOpenMerge(tempFile);
    }else if (pad.contains("movescene")){
      int index;
      float x, y, z;
      moviestream >> index >> x >> y >> z;
      cuviewDoc->translateScene(index, x, y, z);
    }else if (pad.contains("scalescene")){
      int index;
      float x, y, z;
      moviestream >> index >> x >> y >> z;
      cuviewDoc->scaleScene(index, x, y, z);
    }
    //rotate
  }
  moviestream.setDevice(0);
}

/**
    @brief Writes camera settings everytime this method is called.

    The slotWriteMovieFrame method writes camera settings such as camera
    position, camera aim, and camera up to the movie script file. The method
    keeps writing the settings until the number of movie frames left is
    depleted.

    This method is called periodically. The period which it is called is
    determined by
  */
void MainWindow::slotWriteMovieFrame()
{
  //write camera info
  GLfloat toFill[3];

  toFill[0] = toFill[1] = toFill[2] = 0;

  if (moviedata.exportImages){
    moviestream << QString("export\n");
    cuviewDoc->cameraPos(toFill);
  }
  if (moviedata.recordMovement){
    moviestream << QString("camera_position %1 %2 %3\n")
                      .arg(toFill[0], toFill[1], toFill[2]);
    cuviewDoc->cameraAim(toFill);
    moviestream << QString("camera_aim %1 %2 %3\n")
                      .arg(toFill[0], toFill[1], toFill[2]);
    cuviewDoc->cameraUp(toFill);
    moviestream << QString("camera_up %1 %2 %3\n")
                      .arg(toFill[0], toFill[1], toFill[2]);
  }
  if (!moviedata.movieframes){
    qDebug("Finished recording movie");
    timer->stop();
    timer->disconnect(); //don't use anymore
    moviefile->close();
    moviestream.setDevice(0);
  }
  --moviedata.movieframes;
}

/**
    @brief Opens the GlobalPreferences window with all settings loaded.
  */
void MainWindow::slotGlobalPrefs()
{
  PrefData* pd = getPreferences();
  GlobalPreferences* gp = new GlobalPreferences(0,"globalprefs");
  gp->restoreWSCheck->setChecked(pd->restoreWS);
  gp->showSplashScreenCheck->setChecked(pd->showSplashScreen);
  gp->docPathLineEdit->setText(pd->docPath);
  gp->updateDisplayButtonGroup->button(gp->updateDisplayButtonGroup->id(gp->sliderReleaseRadioButton))->setChecked(pd->updateRelease);
  gp->boxModelCheck->setChecked(pd->boxModel);
  gp->drawBoxAlwaysCheck->setChecked(pd->drawBoxAlways);
  gp->drawAxisCheck->setChecked(pd->drawAxis);
  gp->drawAxisOriginCheck->setChecked(pd->drawAxisOrigin);
  gp->mouseButtonsSpinBox->setValue(pd->mouseButtons);
  gp->imageFormatComboBox->setCurrentIndex(pd->saveImageFormat);
  gp->directRenderingCheck->setChecked(pd->directRendering);
  gp->openScriptFileCheck->setChecked(pd->executescript);
  gp->scriptFileNameLineEdit->setText(prefdata.startupscriptfile);
  //use startup scriptfile, instead of filedata.scriptfile (for "load script" dialog)
  gp->binPalettesSpinBox->setValue(pd->binPalettes);
  gp->startUpRefreshCheck->setChecked(pd->startUpRefresh);
  gp->show();
  if (gp->exec()==QDialog::Accepted)
      setPrefData( getPreferences() );
  delete pd;
}

/**
    @fn getPreferences
    @brief Reads global preferences from a saved file

    Reads in global preferences from a file found in folder called ".cuviewer"
    which is located in the home directory. The name of the file is called
    cuviewrc with the version number appended.

    @return pd The global preference data
  */
PrefData* MainWindow::getPreferences(){
  /*
    NOTE: The reason why I let QTextStream read and write the
    preferences instead of QSettings is because QSettings writes a
    file into the Windows registry. I didn't feel like writing to
    the registry because I might forget to remove it later on.
    - Thai
  */

  //read from $home/.cuviewer/cuviewrc, and then return PrefData
  PrefData* pd = new PrefData;
  QFile file(QDir::home().path()+"/.cuviewer/cuviewrc-"+VERSION);
  if (QFile::exists(QDir::home().path()+"/.cuviewer/cuviewrc-"+VERSION)){
    if (file.open(QIODevice::ReadOnly)){
      QTextStream ts(&file);
      QString pad;
      int value;
      QString str;

      ts.readLine(); //reads in "General preferences for cuviewer"
      ts >> pad >> value; //restore_ws
      pd->restoreWS = value;
      ts >> pad >> value; // showSplashScreen
      pd->showSplashScreen = value;
      ts >> pad >> str; //doc_path
      pd->docPath = str.trimmed();
      //value.toString();
      ts >> pad >> value; //update display
      pd->updateRelease = value;
      ts >> pad >> value; //box_model
      pd->boxModel = value;
      ts >> pad >> value; //box_model_always
      pd->drawBoxAlways = value;
      ts >> pad >> value; //drawaxis
      pd->drawAxis = value;
      ts >> pad >> value; //drawaxisorigin
      pd->drawAxisOrigin = value;
      ts >> pad >> value; //mouse_buttons
      pd->mouseButtons = value;
      ts >> pad >> str; //image_format
      QString format = str;
      int i = ip->supportedFormats.indexOf(format.toAscii());
      pd->saveImageFormat = i;
      ts >> pad >> value; //direct_rendering
      pd->directRendering = value;
      ts >> pad >> value; //bin_palettes
      pd->binPalettes = value;
      ts >> pad >> value; //startUpRefresh
      pd->startUpRefresh  = value;
      ts >> pad >> value; //execute_scriptfile
      pd->executescript = value;
      ts >> pad >> str; //scriptfile
      pd->startupscriptfile = str;
      file.close();
    } //endif open file
    else //file could not be opened
      QMessageBox::information(this, "Can't open file for reading.",
             "Couldn't read file, check file permissions");
  } //endif file exists
  else { //file does not exist
    //defaults
    pd->docPath = QString(DOCUMENTATION);
    pd->updateRelease = 1;
    pd->boxModel = 0;
    pd->drawBoxAlways = 0;
    pd->drawAxis = 1;
    pd->drawAxisOrigin = 1;
    pd->mouseButtons = 3;
    pd->saveImageFormat = 0;
    pd->directRendering = 1;
    pd->executescript = 0;
    pd->binPalettes = 16;
    pd->scriptload = true;
    pd->startUpRefresh = 0;
    pd->ssd_x=0;
    pd->ssd_y=0;
    pd->ssd_dx=561;
    pd->ssd_dy=433;
  }
  return pd;
}

/**
    @fn setPrefData
    @brief once global preferences dialog is done, data is set here
    @param pd  - global preference data
  */
void MainWindow::setPrefData( PrefData * pd )
{
  prefdata.restoreWS = pd->restoreWS;
  prefdata.docPath = pd->docPath;
  prefdata.updateRelease = pd->updateRelease;
  prefdata.boxModel = pd->boxModel;
  setBoundingBox(pd->boxModel);
  prefdata.drawBoxAlways = pd->drawBoxAlways;
  setDrawBoundingBoxAlways(pd->drawBoxAlways);
  prefdata.drawAxis = pd->drawAxis;
  prefdata.drawAxisOrigin = pd->drawAxisOrigin;
  if (cuviewDoc){
    drawAxisAction->setChecked(cuviewDoc->drawAxis());
    cuviewDoc->setDrawAxis( prefdata.drawAxis );
    cuviewDoc->setDrawAxisOrigin( prefdata.drawAxisOrigin );
  }

  prefdata.mouseButtons = pd->mouseButtons;
  prefdata.saveImageFormat = pd->saveImageFormat;
  prefdata.imageFormat = ip->supportedFormats.at(prefdata.saveImageFormat);
  ip->setPreferredImageFormat(prefdata.imageFormat);
  prefdata.directRendering = pd->directRendering;
  if (pd->executescript)
    prefdata.startupscriptfile = pd->startupscriptfile;
  prefdata.binPalettes = pd->binPalettes;
  vs->paletteIndexSpinBox->setMaximum(2*prefdata.binPalettes);
  prefdata.startUpRefresh = pd->startUpRefresh;
  if (cuviewDoc)
    cuviewDoc->setThreeButtonMouse( prefdata.mouseButtons==3 );
}

void MainWindow::setOutlineColor( QColor c )
{
  GLfloat newColor[3] = { (GLfloat)c.red()/255.0, (GLfloat)c.green()/255.0, (GLfloat)c.blue()/255.0 };
  GLfloat oldColor[3];
  if (cuviewDoc) {
    cuviewDoc->outlineColor(oldColor);
    if (c!=QColor((int)oldColor[0]*255,(int)oldColor[1]*255,(int)oldColor[2]*255)){
      if (moviestream.device())
      moviestream << QString("outlinecolor %1 %2 %3\n").arg(c.red())
                      .arg(c.green()).arg(c.blue());
      cuviewDoc->setOutlineColor(newColor);
      cuviewDoc->redrawDoc();
    }
  }
}

void MainWindow::setBoundingBox( bool c )
{
  showBoundingBoxAction->setChecked(c);
  if (cuviewDoc && cuviewDoc->boxWhileMoving() != c){
    cuviewDoc->setBoxWhileMoving( c );
    cuviewDoc->redrawDoc();
  }
}

void MainWindow::setDrawBoundingBoxAlways( bool c )
{
  showBoxAlwaysAction->setChecked(c);
  if (cuviewDoc && cuviewDoc->drawBoxAlways() != c) {
    cuviewDoc->setDrawBoxAlways( c );
    cuviewDoc->redrawDoc();
  }
}

void MainWindow::setOutlineChecked( bool c )
{
  vs->outlineCheck->setChecked(c);
  outline->setChecked(c);
  if (cuviewDoc && cuviewDoc->outline() != c) {
    if (moviestream.device())
      moviestream << QString("outline %1\n").arg((int)cuviewDoc->outline());
    cuviewDoc->setOutline(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setTwoSidedChecked( bool c )
{
  vs->twoSidedCheck->setChecked(c);
  twoSided->setChecked(c);
  if (cuviewDoc && cuviewDoc->twoSided() != c) {
    if (moviestream.device())
      moviestream << QString("twosided %1\n").arg((int)cuviewDoc->twoSided());
    cuviewDoc->setTwoSided(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setShadingChecked( bool c )
{
  vs->shadingCheck->setChecked(c);
  shading->setChecked(c);
  if (cuviewDoc && cuviewDoc->smoothShaded() != c) {
    if (moviestream.device())
      moviestream << QString("smoothshaded %1\n").arg((int)cuviewDoc->smoothShaded());
    cuviewDoc->setSmoothShaded(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setAntialiasingChecked( bool c )
{
  vs->antialiasingCheck->setChecked(c);
  antialiasing->setChecked(c);
  if (cuviewDoc && cuviewDoc->antialias() != c) {
    if (moviestream.device())
      moviestream << QString("aa %1\n").arg((int)cuviewDoc->antialias());
    cuviewDoc->setAntialias(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setWireframeChecked( bool c )
{
  vs->wireframeCheck->setChecked(c);
  wireframe->setChecked(c);
  if (cuviewDoc && cuviewDoc->wireframe() != c) {
    if (moviestream.device())
      moviestream << QString("wireframe %1\n").arg((int)cuviewDoc->wireframe());
    cuviewDoc->setWireframe(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setOpaqueWireframeChecked( bool c )
{
  vs->opaquewireframeCheck->setChecked(c);
  opaquewireframe->setChecked(c);
  if (cuviewDoc && cuviewDoc->opaqueWireframe() != c) {
    if (moviestream.device())
      moviestream << QString("opaquewf %1\n").arg((int)cuviewDoc->opaqueWireframe());
    cuviewDoc->setOpaqueWireframe(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setPerspectiveChecked( bool c )
{
  vs->perspectiveCheck->setChecked(c);
  perspective->setChecked(c);
  if (cuviewDoc && cuviewDoc->perspective() != c) {
    if (moviestream.device())
      moviestream << QString("view %1\n").arg((int)cuviewDoc->perspective());
    cuviewDoc->setPerspective(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setTransparencyChecked( bool c )
{
  vs->transparencyCheck->setChecked(c);
  transparency->setChecked(c);
  if (cuviewDoc && cuviewDoc->transparency() != c) {
    if (moviestream.device())
      moviestream << QString("transparency %1\n").arg((int)cuviewDoc->transparency());
    cuviewDoc->setTransparency(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setFogChecked( bool c )
{
  vs->fogCheck->setChecked(c);
  fog->setChecked(c);
  if (cuviewDoc && cuviewDoc->fog() != c) {
    cuviewDoc->setFog(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setGammaChecked( bool c )
{
  vs->gammaCheck->setChecked(c);
  gamma->setChecked(c);
  if (cuviewDoc && cuviewDoc->gamma() != c) {
    if (moviestream.device())
      moviestream << QString("gamma %1\n").arg((int)cuviewDoc->gamma());
    cuviewDoc->setGamma(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setLightingChecked( bool c )
{
  vs->lightingCheck->setChecked(c);
  lighting->setChecked(c);
  if (cuviewDoc && cuviewDoc->lighting() != c) {
    if (moviestream.device())
      moviestream << QString("lighting %1\n").arg((int)cuviewDoc->lighting());
    cuviewDoc->setLighting(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setDiffuseChecked( bool c )
{
  vs->diffuseCheck->setChecked(c);
  diffuse->setChecked(c);
  if (cuviewDoc && cuviewDoc->light() != c) {
    if (moviestream.device())
      moviestream << QString("diffuse %1\n").arg((int)cuviewDoc->light());
    cuviewDoc->setLight(c);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setBinnedPaletteChecked( bool c )
{
  vs->binnedPaletteCheck->setChecked(c);
  binnedPalette->setChecked(c);
  if (cuviewDoc && cuviewDoc->binWindow() != c) {
    if (moviestream.device())
      moviestream << QString("binWindow %1\n").arg((int)cuviewDoc->binWindow());
    cuviewDoc->setBinWindow(c);
    if (c)
      connect(cuviewDoc,SIGNAL(cachedImageChanged(bool)),bp,SLOT(paintBinPalette()));
    else
      disconnect(cuviewDoc,SIGNAL(cachedImageChanged(bool)),bp,SLOT(paintBinPalette()));

    cuviewDoc->redrawDoc();
  }
}

void MainWindow::setClippingValue( int v )
{
  vs->clippingSlider->setValue(v);
  if (cuviewDoc && cuviewDoc->clipPerspNear() != v*0.05){
    if (moviestream.device())
      moviestream << QString("clipping %1\n").arg((double)cuviewDoc->clipPerspNear());
    cuviewDoc->setClipPerspNear(v*.05);
    cuviewDoc->setClipOrthoNear(v*.01);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setFOVValue( int v )
{
  vs->fovSlider->setValue(v);
  if (cuviewDoc && cuviewDoc->fov() != v) {
    if (moviestream.device())
      moviestream << QString("fov %1\n").arg((float)cuviewDoc->fov());
    cuviewDoc->setFov((float)v);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setOutlineValue( int v ) //for pratical purposes, use int
{
  vs->outlineValue->setValue(v);
  if (cuviewDoc && cuviewDoc->lineWidth() != v) {
    if (moviestream.device())
      moviestream << QString("linewidth %1\n").arg((int)cuviewDoc->lineWidth());
    cuviewDoc->setLineWidth((float)v);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setTesselValue( int v )
{
  vs->sphereTessSlider->setValue(v);
  if (cuviewDoc && cuviewDoc->sphereTess() != v){
    if (moviestream.device())
      moviestream << QString("spheretess %1\n").arg((int)cuviewDoc->sphereTess());
    cuviewDoc->setSphereTess((unsigned char)v);
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setAmbientValue( int v )
{
  vs->ambientSlider->setValue(v);
  if (cuviewDoc && cuviewDoc->ambient() != v*0.01) {
    if (moviestream.device())
      moviestream << QString("ambientv %1\n").arg((float)cuviewDoc->ambient());
    cuviewDoc->setAmbient((float)(v*0.01)); //range 0 - 2.0
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setDiffuseValue( int v )
{
  vs->diffuseSlider->setValue(v);
  if (cuviewDoc && cuviewDoc->diffuse() != v*0.01) {
    if (moviestream.device())
      moviestream << QString("diffusev %1\n").arg((float)cuviewDoc->diffuse());
    cuviewDoc->setDiffuse((float)(v*0.01));
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setBackgroundValue( int v )
{
  vs->backgroundSlider->setValue(v);
  QColor bg;
  bg.setHsv(0,0,(int)(v*255/100.0));
  vs->paletteView->setBackgroundColor(bg);
  if (cuviewDoc && cuviewDoc->background() != v*0.01) {
    if (moviestream.device())
      moviestream << QString("background %1\n").arg((float)cuviewDoc->background());
    cuviewDoc->setBackground((float)(v*0.01));
    cuviewDoc->redrawDoc();
  }
}
void MainWindow::setBackgroundBlack() //convienient for setting background
{
    setBackgroundValue(0);
}
void MainWindow::setBackgroundWhite() //convienient for setting background
{
    setBackgroundValue(100);
}
void MainWindow::setFogValue( int v ){
  vs->fogSlider->setValue(v);
  if (cuviewDoc && (int)(cuviewDoc->fogLevel()*200) != v) {
  if (moviestream.device())
    moviestream << QString("fog %1\n").arg((int)cuviewDoc->fog());
  cuviewDoc->setFogLevel((float)(v/200.0)); //cuviewdoc foglevel range 0.0, 1.0
  cuviewDoc->redrawDoc();
  }
}
void MainWindow::setGammaValue( int v ){
  vs->gammaSlider->setValue(v);
  if (cuviewDoc && cuviewDoc->gammaLevel() != v*0.1) {
    if (moviestream.device())
      moviestream << QString("gammav %1\n").arg((float)cuviewDoc->gammaLevel());
    cuviewDoc->setGammaLevel((float)(v*0.1));
    cuviewDoc->redrawDoc();
  }
}

void MainWindow::slotOutlineColor()
{
  if (!cuviewDoc)
    return;

  GLfloat color[3];
  cuviewDoc->outlineColor(color);
  QColor c((int)color[0]*255, (int)color[1]*255, (int)color[2]*255);
  if (!c.isValid()){
    c = Qt::black;
  }else {
    c = QColorDialog::getColor( c, this, "Colour Dialog" );
  }

  setOutlineColor(c);
}

int MainWindow::terminateTime()
{
  return terminatetime;
}

void MainWindow::slotExportImageFile( QString filename )
{
  filedata.imagefile = filename;
  ip->setImageFile(filedata.imagefile);
  ip->setExportSize(width(),height());
  int value = vs->paletteIndexSpinBox->value();
  bp->setPaletteIndex(value);
  //need a delay here because image gets corrupted under X11
  terminatetime = 200+cuviewDoc->scenePrimitives()/800;
  QTimer::singleShot(terminatetime, ip, SLOT(exportImage()));
  if (terminatetime > 500){
    QTimer::singleShot(500, cuviewDoc, SLOT(redrawDoc()));
  }
}

void MainWindow::slotSetScriptFile( QString file, bool scriptload )
{
  prefdata.startupscriptfile = file; //startup-only script variable
  //*note* use filedata.scriptfile for "load script" dialog
  prefdata.scriptload = scriptload;
}

//TODO add save as..
//saves to script
void MainWindow::slotSavePreset()
{
  if (!cuviewDoc)
    return;

  QFileDialog qfd(this, "Save script file", startfolder.saveview, "Script files (*)");
  qfd.setAcceptMode(QFileDialog::AcceptSave);
  QString filename;
  if (qfd.exec())
    filename = qfd.selectedFiles().at(0); //Only select 1 file.
  startfolder.saveview = qfd.directory().path();

  if (filename.isEmpty()){
    qWarning("didn't save script file");
    return;
  }
  filedata.scriptfile = filename;
  QFile * file = new QFile(filedata.scriptfile);
  Script s(file,cuviewDoc);
  qDebug("%s written", qPrintable(filedata.scriptfile));
  QStringList filelist(filedata.cuvfile);
  filelist += filedata.mergedFiles;
  s.setFilenameList(filelist);
  s.writeScript();
  file->close();
  filedata.scriptfile = "";
}

//loads preset from script
void MainWindow::slotLoadPreset()
{
  if (!cuviewDoc)
    return;

  if (filedata.scriptfile.isEmpty()){
    //Get file from user
    QFileDialog qfd(this, "Load script file", startfolder.script, "Script files (*)");
    if (qfd.exec()){
      filedata.scriptfile = qfd.selectedFiles().at(0); //Select only 1 file.
    }
    startfolder.script = qfd.directory().path();
  }
  QFile * file = new QFile(filedata.scriptfile);
  if (!file->exists()){
    qWarning("didn't load script file");
    filedata.scriptfile="";
    return;
  }

  Script script(file, cuviewDoc);
  script.setFileLoad(prefdata.scriptload);
  script.readScript();

  //Check if script needs some objects from other files to be merged
  QStringList templist = script.getFilenameList();
  QStringList tomergelist;
  QStringList::Iterator it = templist.begin();
  qDebug("%s", qPrintable( templist.at(0) ));
  qDebug("%s", qPrintable(filedata.cuvfile));
  if (filedata.cuvfile != *it)
    qWarning("slotloadpreset: warning scriptfile does not have the original cuvfiles");
  for (++it; it != templist.end(); ++it ){ //omit first cuvfile
    tomergelist+=*it;
  }
  loadAllInOne(tomergelist);
  setViewerSettingsActions();
  bp->setPaletteIndex(cuviewDoc->binWindowIndex());

  file->close();
  filedata.scriptfile="";
}

/** @fn slotViewerSettings
    @brief Shows/hides the viewer settings tabs.
  */
void MainWindow::slotViewerSettings( bool showMe )
{
  bool currentlyHiding = viewerSettingsDockWidget->isHidden();
  if (showMe){
    if (currentlyHiding){
      viewerSettingsDockWidget->blockSignals(TRUE);
      viewerSettingsDockWidget->show();
      viewerSettingsDockWidget->blockSignals(FALSE);
      statusBar()->showMessage("Viewer settings shown");
    }
  }
  else{
    if (!currentlyHiding) {
      viewerSettingsDockWidget->blockSignals(TRUE);
      viewerSettingsDockWidget->hide();
      viewerSettingsDockWidget->blockSignals(FALSE);
      statusBar()->showMessage("Viewer settings hidden");
    }
  }

  //Make sure toolbar icon is in the desired state.
  //Icon pressed down means tabs are shown.
  //Icon not pressed means tabs are hidden.
  if (viewerSettings->isChecked()!=showMe){
    viewerSettings->blockSignals(TRUE);
    viewerSettings->setChecked(showMe);
    viewerSettings->blockSignals(FALSE);
  }
}

void MainWindow::redrawDocument()
{
  if (cuviewDoc)
    cuviewDoc->redrawDoc();
}

void MainWindow::setZeroClipping()
{
  if (cuviewDoc){
    vs->clippingSlider->setValue(0);
    cuviewDoc->setClipPerspNear(0);
    cuviewDoc->setClipOrthoNear(0);
    cuviewDoc->redrawDoc();
  }
}

/**
    @fn goto1
    @brief Changes view point to show the object's top.

    @fn goto2
    @brief Changes view point to show the object's front.

    @fn goto3
    @brief Changes view point to show the object's right.

    @fn goto4
    @brief Changes view point to show the object's bottom.

    @fn goto5
    @brief Changes view point to show the object's back.

    @fn goto6
    @brief Changes view point to show the object's left.

    @fn goto7
    @brief Changes view point to show the object's top back-left.

    @fn goto8
    @brief Changes view point to show the object's top back-right.

    @fn goto9
    @brief Changes view point to show the object's top front-left.

    @fn goto0
    @brief Changes view point to show the object's top front-right.
  */
void MainWindow::goto1() { gotoViewpoint(1); }
void MainWindow::goto2() { gotoViewpoint(2); }
void MainWindow::goto3() { gotoViewpoint(3); }
void MainWindow::goto4() { gotoViewpoint(4); }
void MainWindow::goto5() { gotoViewpoint(5); }
void MainWindow::goto6() { gotoViewpoint(6); }
void MainWindow::goto7() { gotoViewpoint(7); }
void MainWindow::goto8() { gotoViewpoint(8); }
void MainWindow::goto9() { gotoViewpoint(9); }
void MainWindow::goto0() { gotoViewpoint(0); }
void MainWindow::gotoViewpoint( int key )
{
  vs->presetNumber->blockSignals( TRUE );
  vs->presetNumber->setValue(key);
  vs->presetNumber->blockSignals( FALSE );
  if (cuviewDoc){
    cuviewDoc->gotoViewpoint((unsigned int)key);
    cuviewDoc->redrawDoc();
    statusBar()->showMessage(QString("Went to %1").arg(key));
  }
}

/**
    @fn showScenes
    @brief Displays dialog in which user can toggle scenes on/off

    The showScenes method displays a dialog which gives the user
    the ability to toggle selected scenes' visibility and to also
    rotate or translate highlighted scenes.

    Please note that this method saves the position and size of the
    dialog window, so if you manage to get the dialog window way out
    of the screen you will have to change settings on the cuviewer file.
  */
void MainWindow::showScenes()
{
  if (showSceneDialog){
    //Save position and size of ShowScene window before closing it.
    prefdata.ssd_x = showSceneDialog->x();
    prefdata.ssd_y = showSceneDialog->y();
    prefdata.ssd_dx = showSceneDialog->width();
    prefdata.ssd_dy = showSceneDialog->height();
    showSceneDialog->close();
    showSceneDialog = NULL;
  }
  else if (cuviewDoc) {
    qDebug("Opening ShowScene dialog");
    showSceneDialog = new ShowScene(0);
    updateScenes();

    showSceneDialog->editingCheckBox->blockSignals(TRUE);
    showSceneDialog->editingCheckBox->setChecked(editModeAction->isChecked());
    showSceneDialog->editingCheckBox->blockSignals(FALSE);

    QFile file(QDir::home().path()+"/.cuviewer/cuviewer");
    if (QFile::exists(QDir::home().path()+"/.cuviewer/cuviewer")&&
       file.open(QIODevice::ReadOnly) && prefdata.restoreWS){
      showSceneDialog->move(prefdata.ssd_x,prefdata.ssd_y);
      showSceneDialog->resize(prefdata.ssd_dx,prefdata.ssd_dy);
    }
    file.close();
    startScene();
    endScene();

    showSceneDialog->delaySpinBox->setValue(timeout);
    showSceneDialog->show();
    connect(showSceneDialog->visibleList,
            SIGNAL(itemSelectionChanged()),SLOT(setSceneVisible()));
    connect(showSceneDialog->editingList,
            SIGNAL(itemSelectionChanged()),SLOT(setSceneEditing()));

    connect(showSceneDialog->reversePlay,
            SIGNAL(clicked()),SLOT(reversePlayScene()));
    connect(showSceneDialog->forwardPlay,
            SIGNAL(clicked()),SLOT(forwardPlayScene()));

    connect(showSceneDialog->reverseStep,
            SIGNAL(clicked()),SLOT(reverseStep()));
    connect(showSceneDialog->forwardStep,
            SIGNAL(clicked()),SLOT(forwardStep()));

    connect(showSceneDialog->stopPlay,
            SIGNAL(clicked()),SLOT(stopPlayScene()));
    connect(showSceneDialog->scstartLineEdit,
            SIGNAL(returnPressed()),SLOT(startScene()));
    connect(showSceneDialog->scendLineEdit,
            SIGNAL(returnPressed()),SLOT(endScene()));

    connect(showSceneDialog->editingCheckBox,
            SIGNAL(toggled(bool)),editModeAction,SLOT(setOn(bool)));
    connect(showSceneDialog->applyTranslateButton,
            SIGNAL(clicked()),SLOT(transformScene()));
    connect(showSceneDialog->applyRotateButton,
            SIGNAL(clicked()),SLOT(rotateScene()));

    connect(showSceneDialog->dismissButton,
            SIGNAL(clicked()),SLOT(showScenes()));

    showSceneDialog->exec();
    showSceneDialog = NULL;
  }
}

void MainWindow::updateScenes()
{
  if (showSceneDialog){
    showSceneDialog->sceneInfoText->clear();
    showSceneDialog->extremInfoText->clear();
    QString sceneInfo = cuviewDoc->sceneInfo();
    QString extremInfo = cuviewDoc->extremitiesInfo();
    showSceneDialog->visibleList->clear();
    showSceneDialog->editingList->clear();
    int textIndex;

    for(int i=0;i<cuviewDoc->scenes();i++){
      showSceneDialog->visibleList->addItem(QString::number(i+1));
      showSceneDialog->editingList->addItem(QString::number(i+1));
      textIndex = sceneInfo.indexOf("<",1);
      showSceneDialog->sceneInfoText->addItem(sceneInfo.left(textIndex));
      sceneInfo=sceneInfo.right(sceneInfo.length()-textIndex); //remove last line
      textIndex = extremInfo.indexOf("<",1);
      showSceneDialog->extremInfoText->addItem(extremInfo.left(textIndex));
      extremInfo=extremInfo.right(extremInfo.length()-textIndex); //remove last line
      if (i==cuviewDoc->scenes()-1){
        showSceneDialog->extremInfoText->addItem(extremInfo);
      }
      showSceneDialog->visibleList->item(i)->setSelected(cuviewDoc->isVisible(i));
      showSceneDialog->editingList->item(i)->setSelected(cuviewDoc->isEditing(i));
    }
  }
  if (cuviewDoc)
    editModeAction->setChecked(cuviewDoc->getEditMode());
}

void MainWindow::setSceneVisible()
{
  if (!showSceneDialog)
    return;

  int i=0;
  QListWidgetItem * lbi = showSceneDialog->visibleList->item(i);
  while(lbi){
    cuviewDoc->setSceneVisible(showSceneDialog->visibleList->row(lbi),
                                            lbi->isSelected());
    i++;
    lbi = showSceneDialog->visibleList->item(i);
  }

  if (moviestream.device()){
    moviestream << QString("scenes %1").arg((int)cuviewDoc->scenes());
    for(int i=0;i<cuviewDoc->scenes();i++)
      moviestream << " " << QString::number((int)cuviewDoc->isVisible(i));
    moviestream << "\n";
  }
  cuviewDoc->redrawDoc();
}

void MainWindow::setDrawAxis( bool toggle )
{
  if (cuviewDoc)
    cuviewDoc->setDrawAxis( toggle );
}

void MainWindow::setEditMode( bool edit )
{
  if (showSceneDialog){
    showSceneDialog->editingCheckBox->blockSignals(TRUE);
    showSceneDialog->editingCheckBox->setChecked(edit);
    showSceneDialog->editingCheckBox->blockSignals(FALSE);
  }
  if (cuviewDoc)
    cuviewDoc->setEditMode(edit);
}

void MainWindow::setSceneEditing()
{
  if (!showSceneDialog)
    return;

  int i=0;
  QListWidgetItem * lbi = showSceneDialog->visibleList->item(i);
  while(lbi){
    cuviewDoc->setSceneVisible(showSceneDialog->visibleList->row(lbi),
                                            lbi->isSelected());
    i++;
    lbi = showSceneDialog->visibleList->item(i);
  }
}

void MainWindow::reversePlayScene()
{
  if (!showSceneDialog)
    return;

  showSceneDialog->forwardPlay->setChecked(FALSE);
  QListWidget * visibleList = showSceneDialog->visibleList;
  visibleList->blockSignals(TRUE);
  int count = (int)(visibleList->count());
  int start = showSceneDialog->scstartLineEdit->text().toInt();
  int end = showSceneDialog->scendLineEdit->text().toInt();
  timeout  = showSceneDialog->delaySpinBox->value();

  // % init
  if ( currentscene<=-2 ){
    if (showSceneDialog->forwardPlay->isChecked() ||
       !showSceneDialog->reversePlay->isChecked())
      return;

    for(int i=0;i<count;i++){
      if (i==start-1) {
        visibleList->item(i)->setSelected(true);
        cuviewDoc->setSceneVisible(i,TRUE);
      } else {
        visibleList->item(i)->setSelected(false);
        cuviewDoc->setSceneVisible(i,FALSE);
      }
    }

    cuviewDoc->redrawDoc();
    QTimer::singleShot(timeout,this, SLOT(reversePlayScene()));
    currentscene=end-1;
    return;
  }
  // % decrement
  if ((currentscene!=start-1)&&showSceneDialog->reversePlay->isChecked()) {
    visibleList->item(currentscene)->setSelected(false);
    cuviewDoc->setSceneVisible(currentscene,FALSE);
    currentscene--;
    visibleList->item(currentscene)->setSelected(true);
    cuviewDoc->setSceneVisible(currentscene,TRUE);
    cuviewDoc->redrawDoc();
    QTimer::singleShot(timeout,this, SLOT(reversePlayScene()));
  }
  // % or loop
  else{
    if (showSceneDialog->scloopCheckBox->isChecked()&&showSceneDialog->reversePlay->isChecked()){
      showSceneDialog->forwardPlay->setChecked(FALSE);
      visibleList->item(currentscene)->setSelected(false);
      cuviewDoc->setSceneVisible(currentscene,FALSE);
      currentscene=end-1;
      visibleList->item(currentscene)->setSelected(true);
      cuviewDoc->setSceneVisible(currentscene,TRUE);
      cuviewDoc->redrawDoc();
      QTimer::singleShot(timeout,this, SLOT(reversePlayScene()));
    }
    else{  // % end
      showSceneDialog->visibleList->blockSignals(FALSE);
      showSceneDialog->reversePlay->setChecked(FALSE);
      showSceneDialog->forwardPlay->setChecked(FALSE);
    }
  }
}

void MainWindow::forwardPlayScene()
{
  if (!showSceneDialog)
    return;

  showSceneDialog->reversePlay->setChecked(FALSE);

  QListWidget * visibleList = showSceneDialog->visibleList;
  visibleList->blockSignals(TRUE);
  int count = visibleList->count();
  int start = showSceneDialog->scstartLineEdit->text().toInt();
  int end = showSceneDialog->scendLineEdit->text().toInt();
  timeout = showSceneDialog->delaySpinBox->value();

  // % init
  if ( currentscene<=-2 ) {
    if (showSceneDialog->reversePlay->isChecked()||
       !showSceneDialog->forwardPlay->isChecked())
      return;

    for(int i=0;i<count;i++){
      if (i==start-1) {
        visibleList->item(i)->setSelected(true);
//  visibleList->setSelected(i,TRUE);
        cuviewDoc->setSceneVisible(i,TRUE);
      } else {
        visibleList->item(i)->setSelected(false);
//  visibleList->setSelected(i,FALSE);
        cuviewDoc->setSceneVisible(i,FALSE);
      }
    }
    currentscene=start-1;
  }
  // % increment
  else if ((currentscene!=end-1) && showSceneDialog->forwardPlay->isChecked() ||
          showSceneDialog->scloopCheckBox->isChecked() && showSceneDialog->forwardPlay->isChecked())
  {
    showSceneDialog->reversePlay->setChecked(false);
    visibleList->item(currentscene)->setSelected(false);
//    visibleList->setSelected(currentscene,false);
    cuviewDoc->setSceneVisible(currentscene,false);

    if (currentscene!=end-1)
      currentscene++;
    else //if (showSceneDialog->scloop->isChecked())
      currentscene = start-1;
    visibleList->item(currentscene)->setSelected(true);
//    visibleList->setSelected(currentscene,TRUE);
    cuviewDoc->setSceneVisible(currentscene,TRUE);
  }
  else { // % end
    showSceneDialog->visibleList->blockSignals(false);
    showSceneDialog->reversePlay->setChecked(false);
    showSceneDialog->forwardPlay->setChecked(false);
    return;
  }
  //redraw, export image, play next scene
  cuviewDoc->redrawDoc();
  if (showSceneDialog->scexportCheck->isChecked()) {
    QString dirPath = showSceneDialog->imgDirectoryLineEdit->text().trimmed();
    if (!dirPath.isEmpty()){
      //Remove directory path from file name
      QString fileName = filedata.cuvfile.mid(filedata.cuvfile.lastIndexOf('/')+1);
      //Remove file extension
      fileName = fileName.left(fileName.lastIndexOf('.'));
      //Append scene number with preferred image format
      fileName = fileName + QString().sprintf("%04i", currentscene+1) + "." +
                  ip->supportedFormats.at(getPreferences()->saveImageFormat);
      fileName = fileName.toLower();
      ip->setImageFile(dirPath + "/" + fileName);
      ip->exportImage();
    } else{ //No directory path given
      qDebug("Directory path is not set. Cannot export image.");
    }
  }
  QTimer::singleShot(timeout,this, SLOT(forwardPlayScene()));
}

void MainWindow::reverseStep()
{
  QListWidget * visibleList = showSceneDialog->visibleList;
  visibleList->blockSignals(TRUE);
  int count = visibleList->count();
  int start = showSceneDialog->scstartLineEdit->text().toInt();
  int end = showSceneDialog->scendLineEdit->text().toInt();
  showSceneDialog->reversePlay->setChecked(FALSE);
  showSceneDialog->forwardPlay->setChecked(FALSE);

  // % init
  if ( currentscene<=-2 ){
    for(int i=0;i<count;i++){
      if (i==end-1) {
        visibleList->item(i)->setSelected(true);
        cuviewDoc->setSceneVisible(i,TRUE);
      } else {
        visibleList->item(i)->setSelected(false);
        cuviewDoc->setSceneVisible(i,FALSE);
      }
    }

    cuviewDoc->redrawDoc();
    currentscene=end-1;
    return;
  }
  // % decrement
  if (currentscene!=start-1) {
    visibleList->item(currentscene)->setSelected(false);
    cuviewDoc->setSceneVisible(currentscene,FALSE);
    currentscene--;
    visibleList->item(currentscene)->setSelected(true);
    cuviewDoc->setSceneVisible(currentscene,TRUE);
    cuviewDoc->redrawDoc();
  }
  else{     // % or loop
    stopPlayScene();
    reverseStep();
  }
}

void MainWindow::forwardStep(){
  QListWidget * visibleList = showSceneDialog->visibleList;
  visibleList->blockSignals(TRUE);
  int count = visibleList->count();
  int start = showSceneDialog->scstartLineEdit->text().toInt();
  int end = showSceneDialog->scendLineEdit->text().toInt();
  showSceneDialog->reversePlay->setChecked(FALSE);
  showSceneDialog->forwardPlay->setChecked(FALSE);

  // % init
  if ( currentscene<=-2 ) {

    for(int i=0;i<count;i++){
      if (i==start-1) {
        visibleList->item(i)->setSelected(true);
        cuviewDoc->setSceneVisible(i,true);
      } else {
        visibleList->item(i)->setSelected(false);
        cuviewDoc->setSceneVisible(i,false);
      }
    }

    cuviewDoc->redrawDoc();
    currentscene=start-1;
    return;
  }
  // % increment
  if (currentscene!=end-1) {
    visibleList->item(currentscene)->setSelected(false);
    cuviewDoc->setSceneVisible(currentscene,FALSE);
    currentscene++;
    visibleList->item(currentscene)->setSelected(true);
    cuviewDoc->setSceneVisible(currentscene,TRUE);
    cuviewDoc->redrawDoc();
  }
  else{  // %  loop
    stopPlayScene();
    forwardStep();
  }
}

void MainWindow::stopPlayScene(){
  if (showSceneDialog) {
    showSceneDialog->reversePlay->setChecked(FALSE);
    showSceneDialog->forwardPlay->setChecked(FALSE);
    showSceneDialog->visibleList->blockSignals(FALSE);
    currentscene = -2;
    //don't set currentscene (functions as pause)
  }
}
void MainWindow::startScene(){
  QLineEdit * lineedit = showSceneDialog->scstartLineEdit;
  if (lineedit->text().isEmpty())
    lineedit->setText("1");
}
void MainWindow::endScene(){
  QLineEdit * lineedit = showSceneDialog->scendLineEdit;
  int count = (int)(showSceneDialog->visibleList->count());
  if (lineedit->text().isEmpty())
    lineedit->setText(QString::number(count));
}

void MainWindow::transformScene(){
    if (cuviewDoc&&showSceneDialog){
    bool editing;
    bool editmode=cuviewDoc->getEditMode();
    int index = 0;
    QListWidgetItem * lbi = showSceneDialog->editingList->item(index);
    cuviewDoc->setEditMode(true);
    while(lbi){
      if (lbi->isSelected()) {
        //Editing
        editing = cuviewDoc->isEditing(index);
        cuviewDoc->setSceneEditing(index,true);
        cuviewDoc->translateScene(index,
              showSceneDialog->translateXLineEdit->text().toFloat(),
              showSceneDialog->translateYLineEdit->text().toFloat(),
              showSceneDialog->translateZLineEdit->text().toFloat());
        cuviewDoc->scaleScene(index,
              showSceneDialog->xMirrorCheck->isChecked()?-1:1,
              showSceneDialog->yMirrorCheck->isChecked()?-1:1,
              showSceneDialog->zMirrorCheck->isChecked()?-1:1);
        cuviewDoc->setSceneEditing(index,editing);
        //Extremeties
        cuviewDoc->editExtremeties(index,
            showSceneDialog->extremXminLineEdit->text().toFloat(),
            showSceneDialog->extremYminLineEdit->text().toFloat(),
            showSceneDialog->extremZminLineEdit->text().toFloat(),
            showSceneDialog->extremXmaxLineEdit->text().toFloat(),
            showSceneDialog->extremYmaxLineEdit->text().toFloat(),
            showSceneDialog->extremZmaxLineEdit->text().toFloat());
        //Tiling
        //largest tile
        float  xtile=0, ytile=0, ztile=0;
        for(int i=0; i<cuviewDoc->scenes(); i++){
          if (cuviewDoc->xTileLength(index)>xtile)
            xtile = cuviewDoc->xTileLength(index);
          if (cuviewDoc->yTileLength(index)>ytile)
            ytile = cuviewDoc->yTileLength(index);
          if (cuviewDoc->zTileLength(index)>ztile)
            ztile = cuviewDoc->zTileLength(index);
        }
        QString xoffset = showSceneDialog->tileXoffsetLineEdit->text(),
                yoffset = showSceneDialog->tileYoffsetLineEdit->text(),
                zoffset = showSceneDialog->tileZoffsetLineEdit->text();
        cuviewDoc->tileScenes(index,
            showSceneDialog->tileXCountSpinBox->value(),
            showSceneDialog->tileYCountSpinBox->value(),
            showSceneDialog->tileZCountSpinBox->value(),
            (xoffset=="default_offset")?xtile:xoffset.toFloat(),
            (yoffset=="default_offset")?ytile:yoffset.toFloat(),
            (zoffset=="default_offset")?ztile:zoffset.toFloat());
      }
      lbi = showSceneDialog->editingList->item(++index);
    }
    //clear edit boxes
    showSceneDialog->translateXLineEdit->clear();
    showSceneDialog->translateYLineEdit->clear();
    showSceneDialog->translateZLineEdit->clear();
    showSceneDialog->xMirrorCheck->setChecked(false);
    showSceneDialog->yMirrorCheck->setChecked(false);
    showSceneDialog->zMirrorCheck->setChecked(false);
    //clear extrems
    showSceneDialog->extremXminLineEdit->clear();
    showSceneDialog->extremYminLineEdit->clear();
    showSceneDialog->extremZminLineEdit->clear();
    showSceneDialog->extremXmaxLineEdit->clear();
    showSceneDialog->extremYmaxLineEdit->clear();
    showSceneDialog->extremZmaxLineEdit->clear();
    //clear tile
    showSceneDialog->tileXCountSpinBox->setValue(1);
    showSceneDialog->tileYCountSpinBox->setValue(1);
    showSceneDialog->tileZCountSpinBox->setValue(1);
    showSceneDialog->tileXoffsetLineEdit->setText("default_offset");
    showSceneDialog->tileYoffsetLineEdit->setText("default_offset");
    showSceneDialog->tileZoffsetLineEdit->setText("default_offset");
    //restore previous edit mode
    cuviewDoc->setEditMode(editmode);
    cuviewDoc->redrawDoc();
    updateScenes();
  }
}
/**
    @brief Rotates all scenes selected for editing by an amount given by the user

    The method rotateScene rotates all scenes highlighted in the editing box
    about the X, Y, and Z axes by an amount that the user put in the input boxes.
  */
void MainWindow::rotateScene()
{
  if (cuviewDoc && showSceneDialog){
    bool editing;
    bool editmode = cuviewDoc->getEditMode();
    int index = 0;
    QListWidgetItem * lbi = showSceneDialog->editingList->item(index);
    cuviewDoc->setEditMode(true);
    while(lbi){
      if (lbi->isSelected()) {
        //Editing
        editing = cuviewDoc->isEditing(index);
        cuviewDoc->setSceneEditing(index,true);
        cuviewDoc->rotateScene(index,
                   showSceneDialog->rotateXLineEdit->text().toFloat(),
                   showSceneDialog->rotateYLineEdit->text().toFloat(),
                   showSceneDialog->rotateZLineEdit->text().toFloat());
      }
      lbi = showSceneDialog->editingList->item(++index);
    }
    //clear edit boxes
    showSceneDialog->rotateXLineEdit->clear();
    showSceneDialog->rotateYLineEdit->clear();
    showSceneDialog->rotateZLineEdit->clear();
    //restore previous edit mode
    cuviewDoc->setEditMode(editmode);
    cuviewDoc->redrawDoc();
    updateScenes();
  }
}

void MainWindow::setLightEditing()
{
  if (!cuviewDoc)
    return;

  bool hasItem = FALSE;
  int index = 0;
  QListWidgetItem * lbi = vs->editLightListBox->item(index);
  while(lbi){
    cuviewDoc->setEditLightMode(index, lbi->isSelected());
    if (lbi->isSelected())
      hasItem = TRUE;
    lbi = vs->editLightListBox->item(++index);
  }
  cuviewDoc->showLightPosition(hasItem);
  vs->fixedSourceCheckBox->setEnabled(hasItem);
  vs->lightColor->setEnabled(hasItem);
  vs->lightSourceDial->setEnabled(hasItem);
  vs->lightSourceDial->setNotchesVisible(hasItem);
  vs->lightPitch->setEnabled(hasItem);
  vs->lightPitch->setTickPosition(hasItem?(QSlider::TicksBothSides):(QSlider::NoTicks));
}

//specular can be thought of as amount of shine
/**
    @fn setSpecular
    @brief Changes the amount of "shininess" on the CUView
           object to a given value.
  */
void MainWindow::setSpecular( int value )
{
  vs->specularSlider->setValue(value); //slider range 0-100
  float scale=4;
  if (cuviewDoc && (cuviewDoc->specular() != value/scale)) {
    if (moviestream.device())
      moviestream << QString("specularv %1\n")
                      .arg((float)cuviewDoc->specular());
    cuviewDoc->setSpecular(value/scale); //specular range 0.001-25
    cuviewDoc->redrawDoc();
  }
}

/* This chunk of code was commented out since version 3.6f for reasons unknown. - Thai */
/*
void MainWindow::translateLights()
{
  if (cuviewDoc){
    QListBoxItem * lbi = vs->editLightListBox->firstItem();
    bool editing, editmode=cuviewDoc->getEditMode(); int index;
    cuviewDoc->setEditMode(true);
    while(lbi){
      if (lbi->isSelected()) {
        //Editing
        index = vs->editLightListBox->index(lbi);
        editing = cuviewDoc->getEditLightMode(index);
        cuviewDoc->setEditLightMode(index,true);
        cuviewDoc->translateLight(index,
                vs->translateXLight->text().toFloat(),
                vs->translateYLight->text().toFloat(),
                vs->translateZLight->text().toFloat());
        vs->translateXLight->clear();
        vs->translateYLight->clear();
        vs->translateZLight->clear();
        cuviewDoc->setEditLightMode(index,editing);
        }
      lbi = lbi->next();
    }
    cuviewDoc->setEditMode(editmode);
  }
}  */

void MainWindow::resetLightPosition()
{
  if (cuviewDoc){
    for(int i=0;i<cuviewDoc->getLightSources();i++)
      cuviewDoc->resetTranslateLight(i);
    cuviewDoc->redrawDoc();
  }
}

void MainWindow::setFixedLight(bool fixed)
{
  if (cuviewDoc){
    //fixes all the lights relative to the model, see drawablescene.h
    for(int i=0;i<cuviewDoc->getLightSources();i++)
      cuviewDoc->setFixedLight(i,fixed);
    cuviewDoc->redrawDoc();
  }
}

void MainWindow::setLightPosition()
{
  float dir = (vs->lightSourceDial->value()-270)*M_PI/180.0; //in radians
  float pitch = vs->lightPitch->value()*M_PI/180.0;
  float x = cos(dir)*sin(pitch);
  float y = sin(dir)*sin(pitch);
  float z = cos(pitch);

  if (cuviewDoc){
    bool editing;
    bool editmode=cuviewDoc->getEditMode();
    int index = 0;
    cuviewDoc->setEditMode(true);
    QListWidgetItem * lbi = vs->editLightListBox->item(index); //First item in editLightListBox
    while(lbi){
      if (lbi->isSelected()) {
        //Editing
        editing = cuviewDoc->getEditLightMode(index);
        cuviewDoc->setEditLightMode(index,true);
        cuviewDoc->resetTranslateLight(index);
        cuviewDoc->translateLight(index,x,y,z);
        cuviewDoc->setEditLightMode(index,editing);
      }
      lbi = vs->editLightListBox->item(++index);
    }
    cuviewDoc->setEditMode(editmode);
  }
}

void MainWindow::slotLightColor(){
  if (!cuviewDoc)
    return;

  int index = 0;
  QListWidgetItem * lbi = vs->editLightListBox->item(0);
  while(lbi){
    if (lbi->isSelected()) {
      //Editing
      GLfloat color[3];
      cuviewDoc->lightColor(index,color);
      QColor c((int)color[0]*255,(int)color[1]*255,(int)color[2]*255);
      if (!c.isValid()){
        c = Qt::black;
        setLightColor( index, c );
      }
      c = QColorDialog::getColor( c, this, "colordialog" );
      if (c.isValid())
        setLightColor( index, c );
      lbi = vs->editLightListBox->item(++index);
    }
  }
}

void MainWindow::setLightColor(int index, QColor c){
  GLfloat newColor[3] = { (GLfloat)c.red()/255.0, (GLfloat)c.green()/255.0, (GLfloat)c.blue()/255.0 };
  GLfloat oldColor[3];
  if (cuviewDoc) {
    cuviewDoc->lightColor(index,oldColor);
    if (c!=QColor((int)oldColor[0]*255,(int)oldColor[1]*255,(int)oldColor[2]*255)){
      if (moviestream.device())
      moviestream << QString("lightcolor %1 %2 %3\n").arg(c.red())
      .arg(c.green()).arg(c.blue());
      cuviewDoc->setLightColor(index,newColor);
      cuviewDoc->redrawDoc();
    }
  }
}

//animates bin palette upper and lower bound every half a second.
void MainWindow::reversePlayEnd()
{
  if (timer){
    timer->stop();
    timer->disconnect();
  }
  timeout=(int)(timeout/2.0);
  if (timeout<50)
    timeout=50;
  vs->reversePlayEndButton->setChecked(true);
  if (vs->forwardPlayEndButton->isChecked()){
    timeout=500;
    vs->forwardPlayEndButton->setChecked(false);
  }
  if (!timer)
    timer = new QTimer(this); //palette timer
  connect (timer, SIGNAL(timeout()), vs->bpEndValueSpinBox, SLOT(stepDown()));
  timer->start(timeout);
}

void MainWindow::forwardPlayEnd()
{
  if (timer){
    timer->stop();
    timer->disconnect();
  }
  timeout=(int)(timeout/2.0);
  if (timeout<50)
    timeout=50;
  vs->forwardPlayEndButton->setChecked(true);
  if (vs->reversePlayEndButton->isChecked()){
    timeout=500;
    vs->reversePlayEndButton->setChecked(false);
    statusBar()->showMessage(QString("set reverse %1").arg(timeout));
  }
  if (!timer)
    timer = new QTimer(this); //palette timer
  connect (timer, SIGNAL(timeout()), vs->bpEndValueSpinBox, SLOT(stepUp()));
  timer->start(timeout);
}

void MainWindow::stopPlay()
{
  if (timer){
    timeout=1000;
    timer->stop();
    timer->disconnect();
  }
  vs->forwardPlayEndButton->setChecked( false );
  vs->reversePlayEndButton->setChecked( false );
}

void MainWindow::loadScriptnoloadAction_activated()
{
    prefdata.scriptload = false;
    slotLoadPreset();
}
