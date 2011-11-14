#ifndef VIEWDATA
#define VIEWDATA

#include <qcolor.h>
#include <qlist.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qtimer.h>
////#include "include/cuvcommonstructs.h"

struct PrefData{
  bool restoreWS, showSplashScreen, updateRelease, boxModel, drawAxis, drawAxisOrigin,drawBoxAlways, 
       directRendering, executescript, scriptload, startUpRefresh;
  QString docPath, startupscriptfile, imageFormat; //scriptfile here is used for startup
  int ssd_x,ssd_y,ssd_dx,ssd_dy; //showscene dialog window position and size
  int mouseButtons,saveImageFormat,binPalettes;
};

struct FileData{
  QString imagefile, autoimagefile, cuvfile, scriptfile; //these variables are set with dialogs
  QStringList mergedFiles;
};

struct StartFolder{
  QString image, script, saveview, moviescript, html, gl, glmerge;
};

struct MovieData
{
  int movieframes;
  bool exportImages, recordMovement;
};

#endif
