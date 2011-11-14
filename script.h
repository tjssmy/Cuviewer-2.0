/** script.h
  * Carleton University Department of Electronics
  * Copyright (C) 2001 Bryan Wan and Cliff Dugal
  */
#ifndef SCRIPT_INCLUDE
#define SCRIPT_INCLUDE

#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include "include/cuvcommonstructs.h"
#include "cuviewdoc/cuviewdoc.h"
#include "include/cuvtags.h"

class Script
{
  public:
    Script(QFile*,CUViewDoc*);
    ~Script();
    QStringList getFilenameList();
    void setFilenameList(QStringList);
    void setFileLoad(bool allow);
    void writeScript();
    void readScript();
    void SetAllowLoad();
    
  private:
	QStringList filenamelist;
	QFile * scriptfile;
	CUViewDoc * cuviewDoc;
	bool allowLoad;
};
#endif // SCRIPT_INCLUDE
