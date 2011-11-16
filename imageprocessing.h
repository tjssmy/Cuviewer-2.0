#ifndef IMAGEPROCESSING_H
#define IMAGEPROCESSING_H

#include <qfile.h>
#include <qimage.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qwidget.h>
#include <qobject.h>
#include <QPixmap>
#include <QSignalMapper>

#include "viewdata.h" 
#include "viewersettings.h"
#include "include/lessalloc.h"
#include "config.h" 
#include "binpalettes.h"
#include "paletteview.h" 
#include "cuviewdoc/cuviewdoc.h" //the opengl window
#include "include/cuvtags.h"

class ImageProcessing : public QObject
{
    Q_OBJECT

 public:
  ImageProcessing(QWidget* parentWidget, ViewerSettings *viewersettings);
  ~ImageProcessing();
  void setCUViewDoc(CUViewDoc* cuviewDoc);
  int supportedFormatsCount();
  inline void setImageFile(QString autoimage, QString image){
    autoimagefile = autoimage;
    imagefile = image;
  };
  inline void setExportSize(int x, int y){
    xsize=x;
    ysize=y;
  };
  inline void setStartFolder(QString htmlfile, QString imagefile){
    startfolder_html = htmlfile;
    startfolder_image = imagefile;
  };
  bool getPaintPalette();
  void setPreferredImageFormat(int format);
  void setPreferredImageFormat(QString& format);
  /** @brief Contains a list of the supported image formats such as bmp and png. */
  QStringList supportedFormats;

public slots:
  void autoExportImage( bool c );
  void exportImageGetSize();
  void exportImage();
  void exportImage(QPixmap pm);
  QPixmap getPixmap( bool preview=TRUE );
  QPixmap getPixmap( int x, int y, bool preview=TRUE );
  void setPaintPalette(bool paint);
  void setAutoImageFile(QString file); 
  void setImageFile(QString file);
private slots:
    void autoExportImage();

 private:
  int xsize; 
  int ysize;
  CUViewDoc * cuviewDoc;
  QWidget * parent;
  ViewerSettings *vs;
//  QTimer * timer;
  int exportDelay;
  int imageFormat;
  QString autoimagefile;
  QString imagefile;
  QString startfolder_html;
  QString startfolder_image;
  QString filterlist;
  bool paintPalette;
  QStringList getSupportedFormats();
//  QSignalMapper *signalMapper;
};

#endif // IMAGEPROCESSING_H
