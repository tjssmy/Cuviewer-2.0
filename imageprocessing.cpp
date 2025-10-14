/** imageprocessing.cpp
  * Carleton University Department of Electronics
  * Copyright (C) 2003 Bryan Wan
  *
  * Description of class: handles all the image file formats, settings, and exporting images.
  * 
  */

#include <qaction.h>
#include <qbuttongroup.h>
#include <qcheckbox.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qimage.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qstatusbar.h>
#include <qpainter.h>
#include <qtimer.h>
#include <QPixmap>

#include <QImageWriter>

#include "autoexport.h"
#include "getimagesize.h"
#include "config.h" 
#include "mainwindow.h" 
#include "imageprocessing.h" 
#include "paletteview.h" 
#include "printpreview.h"
#include "cuviewdoc/cuviewdoc.h" //the opengl window

/**
    @brief Constructs an ImageProcessing object.
    @param parentWidget  the parent widget
    @param viewerSettings the viewersettings object where all of
                          its settings will be used to process an image
  */
ImageProcessing::ImageProcessing(QWidget* parentWidget, ViewerSettings *viewersettings)
  : QObject ( parentWidget )
{
  parent = parentWidget;
  vs = viewersettings;
  timer = NULL;
  cuviewDoc = NULL;
  imageFormat = 0;
  filterlist = "Images (";
  paintPalette = true;
  xsize=0; 
  ysize=0;
  supportedFormats = getSupportedFormats();

  //Generate a list of supported image formats in QString format
  //Used because QFileDialog only accepts QString, not QStringList.
  int numFormats = supportedFormatsCount();
  for ( int i = 0; i < numFormats; i++ ) {
    filterlist += "*.";
    filterlist += supportedFormats.at(i);
    filterlist += " ";
  }
   filterlist += ")";
}

/**
    @brief Destructs the ImageProcessing object.
  */
ImageProcessing::~ImageProcessing()
{
}

void ImageProcessing::setCUViewDoc(CUViewDoc* cuviewdocument)
{
  cuviewDoc = cuviewdocument;
}

/**
    @brief Returns the number of supported image formats.
  */
int ImageProcessing::supportedFormatsCount()
{
  /* This method may seem useless at first glance but
     the real value in this method is that it is now possible
     to add in a variety of supported formats that may not be
     supported by Qt itself.
   */
  return QImageWriter::supportedImageFormats().count();
}

/**
    @brief Returns a list of formats that are supported.
  */
QStringList ImageProcessing::getSupportedFormats()
{
  /* This method may seem useless at first glance but
     the real value in this method is that it is now possible
     to add in a variety of supported formats that may not be
     supported by Qt itself.
   */
  QStringList supportedFormats;

  for (int i=0; i<supportedFormatsCount(); i++){
    supportedFormats << QImageWriter::supportedImageFormats().at(i);
  }

  return supportedFormats;
}

/**
    @brief Automatically save the current state of cuviewDoc
           and continuously resaves the state after a given period of time.

    The method autoExportImage saves the current state of the
    opened cuviewDoc into an image with the image format given by
    global preferences. If the file does not exist then the method
    asks the user for a filename and the path of where the file will
    be saved.

    The method then opens an AutoExport dialog where it gets the

    @param c
  */
void ImageProcessing::autoExportImage( bool c )
{
   if(!c||!cuviewDoc){ //shut off autoexport
     ((QMainWindow*)parent)->statusBar()->showMessage("Autoexport is off");
     if(timer){
       timer->stop();
       timer->disconnect();
     }
//     filedata.autoimagefile = "";
     ((MainWindow*)parent)->autoExportImageAction->blockSignals(true);
     ((MainWindow*)parent)->autoExportImageAction->setChecked(false);
     ((MainWindow*)parent)->autoExportImageAction->blockSignals(false);
     return;
   }

   int id = this->imageFormat;
   QString format = supportedFormats.at((id!=-1)?id:0);

   QString absfilename;
   if(autoimagefile.isEmpty()){
     //Get image filename
     QFileDialog qfd(parent, "Save autoexport image", startfolder_html, format);
     qfd.setAcceptMode(QFileDialog::AcceptSave);
     if (qfd.exec()){
       //Only pick the first name as the filename. Ignore other names given.
       absfilename = qfd.selectedFiles().at(0);
     }
     startfolder_html = qfd.directory().path();

     if ( absfilename.isEmpty() ){
       //Switch autoexport check to off.
       qDebug("Autoimage file name is empty");
       ((MainWindow*)parent)->autoExportImageAction->blockSignals(true);
       ((MainWindow*)parent)->autoExportImageAction->setChecked(false);
       ((MainWindow*)parent)->autoExportImageAction->blockSignals(false);
       return;
     }
     autoimagefile = absfilename;
     if (!(autoimagefile.mid(autoimagefile.lastIndexOf('/'),
                             autoimagefile.length())).contains('.')){
       //Append image format to string if string originally didn't contain extension.
       autoimagefile += '.' + format;
     }
     //Bring up dialog asking: create webpage? refresh time?
     AutoExport ae(parent);
     ae.show();
     int refreshTime, exportDelay;
     bool createWebpage;
     if (ae.exec()==QDialog::Accepted){
       refreshTime = ae.refreshTime->value();
       exportDelay = ae.exportDelay->value();
       createWebpage = ae.createWebpage->isChecked();
     }else {
       //Remove any values stored and uncheck action.
       autoimagefile.clear();
       ((MainWindow*)parent)->autoExportImageAction->blockSignals(true);
       ((MainWindow*)parent)->autoExportImageAction->setChecked(false);
       ((MainWindow*)parent)->autoExportImageAction->blockSignals(false);
       return;
     }

     if (createWebpage){
       //write html, and set up timer to activate this function.
       int start = autoimagefile.lastIndexOf('/');
       int end = autoimagefile.lastIndexOf('.');
       QString nameOfFile = autoimagefile.mid(start+1, end-start-1);
       QString abshtmlfilename = autoimagefile.left(autoimagefile.lastIndexOf('.'))+".html";
       QFile * htmlfile = new QFile(abshtmlfilename);
       htmlfile->remove();
       htmlfile->setFileName(abshtmlfilename);
       qDebug( "Writing HTML file: %s",qPrintable(abshtmlfilename));
       if(htmlfile->open(QIODevice::ReadWrite)){
       QTextStream ts(htmlfile);
       ts << QString("<html><head>\n<script language=\"JavaScript\"><!-- \nfunction refreshIt() { \nif (!document.images) return; \ndocument.images['%1'].src = '%2.%3?' + Math.random(); \nsetTimeout(\'refreshIt()\',%4); \n } \n//--></script>\n</head> \n<body onLoad=\" setTimeout(\'refreshIt()\',%5)\"> \n<img src=\"%6.%7\" name=\"%8\"> <pre>%9</pre></body> \n</html>\n")
                  .arg(nameOfFile)
                  .arg(nameOfFile)
                  .arg(format)
                  .arg(QString::number(refreshTime))
                  .arg(QString::number(refreshTime))
                  .arg(nameOfFile)
                  .arg(format)
                  .arg(nameOfFile)
                  .arg(QString(VERSION_MESSAGE));
       htmlfile->close();
       }
     }

     //setup timer
     if(!timer){
       timer = new QTimer(parent); //timer
       connect(timer, SIGNAL(timeout()), SLOT(autoExportImage()));
     }
     timer->start(exportDelay);
   }

   if(QFile::exists(autoimagefile)){
     QFile::remove(autoimagefile); //remove file.
   }
   qDebug("Autoexport image to %s",qPrintable(autoimagefile));

   // set preview mode to false
   getPixmap(false).save( autoimagefile, qPrintable(format) );
}

void ImageProcessing::autoExportImage(){
  if(QFile::exists(autoimagefile)){
    QFile::remove(autoimagefile); //remove file.
  }
  qDebug("Autoexport image to %s",qPrintable(autoimagefile));
  getPixmap(false).save(autoimagefile);
}

//actived from file->"export image at size ..."
/**
    @brief Displays a GetImageSize dialog for the size of the image and saves the image.

    The exportImageGetSize method opens a GetImageSize dialog which asks for the
    width and height of the image to be created and if the user accepts the settings
    then the method exports the image.

    This method is called in MainWindow by clicking File > Export image at size...
  */
void ImageProcessing::exportImageGetSize()
{
  if(!cuviewDoc)
    return;

  GetImageSize gis(parent);
  gis.xValueSpinBox->setValue(cuviewDoc->width());
  gis.yValueSpinBox->setValue(cuviewDoc->height());
  gis.show();
  if(gis.exec()==QDialog::Accepted){
    exportImage(getPixmap(gis.xValueSpinBox->value(),
                          gis.yValueSpinBox->value()));
  }
}

/**
    @brief Saves an image to file.

    This method is an overloaded method of the exportImage(QPixmap pm) method.

    This method saves an image of the cuviewDoc and saves it to a file.
  */
void ImageProcessing::exportImage(){
  if(!cuviewDoc)
    return;
  exportImage(getPixmap());
}

/**
    @brief Saves an image to file.

    The exportImage method saves QPixmap pm to a
    file with a name given by imagefile.

    If imagefile is empty then a file dialog is opened which asks
    the user for a file name and the image format (.bmp, .png, etc)

    @param pm - The pixmap that is to be saved to file
  */
//save image to file
void ImageProcessing::exportImage(QPixmap pm){
  if(pm.isNull()) return;

  QString filename;

  if(imagefile.isEmpty()) {
    //Get a file from user through a file dialog
    QFileDialog qfd(parent, "Save images", startfolder_image, filterlist);
    qfd.setAcceptMode(QFileDialog::AcceptSave);

    //Generate a list of supported formats for the user to select
    for (int i=0; i<supportedFormats.count(); i++){
      qfd.setNameFilters(qfd.nameFilters() << supportedFormats.at(i));
    }

    //Setup list to select preferred image format
    //Adding 1 to imageFormat to skip the "Images(*.bmp *.jpg ...etc)" text
    qfd.selectNameFilter( qfd.nameFilters().at(imageFormat + 1) );

    if(qfd.exec()){
      filename = qfd.selectedFiles().at(0); //Get name of file.
      filename += "." + qfd.selectedNameFilter(); //Append the selected image format at the end.
    }
    startfolder_image = qfd.directory().path();
  }
  else {
    filename = imagefile; //specified filename at console.
    qDebug("imagefile: %s", qPrintable(filename));
  }

  if ( !filename.isEmpty() ){
    if(QFile::exists(filename)){
      QFile::remove(filename);
      qWarning("warning overwriting: %s", qPrintable(filename));
    }
    //Save image with the given name, given image format, and with default quality factor
    pm.save( filename );
    ((QMainWindow*)parent)->statusBar()->showMessage("Saved image at " + filename);
  }
}

// Method is removed as of Oct 13, 2011 (Version 3.6g) as it is considered
// obsolete and was used only in imageprocessing.cpp anyways
//QString ImageProcessing::getFormat(QString filename, QString currentfilter)
//{
//  QComboBox * filtercb = ((MainWindow*)parent)->saveImageFormat;
//  int count = filtercb->count();

//  bool gotformat=false;
//  for(int i=0;i<count;i++){
//    gotformat=filename.contains(QString("."+filtercb->text(i)),Qt::CaseSensitive/*case-sensitive*/);
//    if( !gotformat && filtercb->text(i).contains(QString("jpeg"),Qt::CaseSensitive/*case-sensitive*/) )
//      gotformat=filename.contains(QString(".jpg"),Qt::CaseSensitive/*case-sensitive*/);
//    if (gotformat)
//      return filtercb->text(i);
//  }

//  if(currentfilter.contains(QString("image files"),Qt::CaseSensitive/*case-sensitive*/))
//    return filtercb->currentText(); //goto default (selected in mainwidow toolbar)

//  return currentfilter.right(2); //cut off "*.", to get format
//}

/**
    @brief Returns a pixmap of the same size as the picture seen on the window.

    @param preview - A boolean value which is true if an image
                      preview is wanted but false otherwise
  */
QPixmap ImageProcessing::getPixmap( bool preview )
{
    return getPixmap( 0, 0, preview ); //window size
}

/**
    @brief Returns pixmap for image export or printing.

      The getPixMap method returns a Qpixmap object that looks
      like the cuviewDoc object scaled to a width x and height y.
      The method also shows a preview dialog if preview is set.

    @param x - The desired width of the picture
    @param y - The desired height of the picture
    @param preview - A boolean value that is true if a preview of the image is desired but otherwise false
    @return pixmap - The picture in QPixmap form
  */
QPixmap ImageProcessing::getPixmap( int x, int y, bool preview )
{
  QPixmap pm;
  if ( !cuviewDoc || !cuviewDoc->getGLRenderer()->inherits("GLRenderer") ) {
    qDebug("could not get pixmap");
    return pm;
  }

  if(preview) { //show preview dialog
    setPaintPalette(false);
    QImage img = cuviewDoc->getGLRenderer()->grabFrameBuffer();
    if( x>0 && y>0 ){
      // tjs
      img.scaledToWidth(x);
      img.scaledToHeight(y);
    }
    pm.convertFromImage( img );
    setPaintPalette(true);
    if(vs->paletteExportImageCheckBox->isChecked() && paintPalette){
      PrintPreview view( parent );
      QImage temp = pm.toImage();
      if( temp.height()>640 || temp.width()>480 ){
        temp = temp.scaled(480, 640, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        temp = temp.scaledToWidth(640);
        temp = temp.scaledToHeight(480);
      }
      //scale down to fit
      QPixmap tempPix;
      tempPix.convertFromImage( temp );
      view.setPixmap(tempPix);
      view.setPaletteRect(0, 0, 50, 350);
      view.setPaletteView(vs->paletteView);
      view.setWindowIcon(tempPix);
      view.setWindowTitle("Preview");
      view.updatePreview();
      view.PrintButtonGroup->setEnabled( false );
      view.buttonOk->setText("Ok");
      if ( view.exec() ) {
        QImage img = pm.toImage();
        //process image
        if ( view.checkInvert->isChecked() )
          //Invert RGB values, including alpha channel
          img.invertPixels(QImage::InvertRgba);
        if ( view.checkMirror->isChecked() )
          img = img.mirrored( true, false );
        if ( view.checkFlip->isChecked() )
          img = img.mirrored( false, true );
        pm.convertFromImage( img );
      }
      else{
        pm = pm.copy(0,0,0,0); //set to null pixmap
        return pm;
      }
      //pass a pixmap (a paintdevice), paint palette on pixmap.
      QRect pr = view.getPaletteRect();
      vs->paletteView->paintPalette(&pm,pr.width(),pr.height(),pr.x(),pr.y());
    }
  }
  else { //no preview
    QImage img = cuviewDoc->getGLRenderer()->grabFrameBuffer();
    if( x>0 && y>0 ){
      // tjs
      img.scaledToWidth(x);
      img.scaledToHeight(y);
    }
    pm.convertFromImage( img );
    if(vs->paletteExportImageCheckBox->isChecked() && paintPalette) {
      //pass a pixmap (a paintdevice), with w=50 and h=350 onto export image
      vs->paletteView->paintPalette(&pm,50,350);
    }
  }
  return pm;
}

/**
    @brief Returns the paint palette.
    @return The paint palette
  */
bool ImageProcessing::getPaintPalette()
{
  return paintPalette;
}

/**
    @brief Sets the paint palette
    @param paint
  */
void ImageProcessing::setPaintPalette(bool paint)
{
  paintPalette = paint;
}

/**
    @brief Sets the file path where the image will be saved for the autoImageExport method.
    @param file
  */
void ImageProcessing::setAutoImageFile(QString file)
{
  autoimagefile = file; 
}

/**
    @brief Sets the file path where the image will be saved.
    @param file
  */
void ImageProcessing::setImageFile(QString file)
{
  imagefile = file;
}

/**
    @brief Sets the preferred image format.

    The setPreferredImageFormat method takes the given format and uses
    it to set the preferred format when saving an image.

    @param format  An integer that corresponds to the image format
    @author Thai Nguyen, Department of Electronics, Carleton University, Oct 2011
  */
void ImageProcessing::setPreferredImageFormat(int format){
  imageFormat = format;
}

/**
    @brief Sets the preferred image format.

    An overloaded method of setPreferredImageFormat(int format)

    @param format  A string denoting the image format (ex. bmp, png, jpg)
    @see setPreferredImageFormat(int format)
    @author Thai Nguyen, Department of Electronics, Carleton University, Oct 2011
  */
void ImageProcessing::setPreferredImageFormat(QString& format){
  imageFormat = supportedFormats.indexOf(format);
}
