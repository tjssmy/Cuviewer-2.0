#ifndef BINPALETTES_H
#define BINPALETTES_H

#include <qfile.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qgl.h>
#include <qwidget.h>

#ifdef Q_OS_MAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "viewdata.h" 
#include "viewersettings.h"
#include "include/lessalloc.h"
#include "config.h" 
#include "cuviewdoc/cuviewdoc.h" //the opengl window
#include "include/cuvtags.h"

class BinPalettes : public QObject
{
  Q_OBJECT

 public:
  BinPalettes(QWidget *parentWidget, int prefBinPalettes, ViewerSettings *viewersettings);
  ~BinPalettes();
  void setCUViewDoc(CUViewDoc* cuviewDoc);
  void setBinPalettes(int maxNumber);

  public slots:
  void setPaletteIndex(int index);
  void setStartPaletteValue(int startvalue);
  void setEndPaletteValue(int endvalue);

  void setBinPaletteStart();
  void setBinPaletteEnd();
  void insertbinpalette();
  void setPalette();
  void paintBinPalette();

 private:
  int binPalettes;
  QWidget *parent;
  ViewerSettings * vs;
  CUViewDoc* cuviewDoc;
};
#endif // BINPALETTES_H

