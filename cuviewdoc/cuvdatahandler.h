/*
Copyright (C) 1999, 2000 Carleton University Department of Electronics

This file is part of CU-Viewer.

CU-Viewer is free software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the Free Software
Foundation; either version 2, or (at your option) any later version. 

CU-Viewer is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. 

You should have received a copy of the GNU General Public License along with
program; see the file COPYING. If not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef CUVDATAHANDLER_H
#define CUVDATAHANDLER_H

#include <qobject.h>
#include <qstring.h>
#include "../include/lessalloc.h" //__lessa_uint64 = unsigned long long

class QIODevice;
class ChangeableScene;
class CUViewDoc;

class CUVDataHandler: public QObject
{
	Q_OBJECT
		
  public:
	enum InitError {INoError, IErrorOpening, IErrorInitializing, IErrorGettingVersion,
					IErrorInFileFormat, IErrorWrongVersion};
	enum SectionError {SNoError, SErrorNoData, SErrorReading, SErrorBadTag};
	struct sceneStatus
	{
		SectionError status;
		__lessa_uint64 points;
		__lessa_uint64 lines;
		__lessa_uint64 trias;
		__lessa_uint64 quadris;
		__lessa_uint64 spheres;
		__lessa_uint64 sphoids;
		__lessa_uint64 texts;
	};
	
  public:
  CUVDataHandler(QIODevice *ioDeviceIn, CUViewDoc *ownerDoc);
	~CUVDataHandler();
	void setDevice(QIODevice *);
	InitError initializeData();
	int getSectionTag();
	sceneStatus readScene(ChangeableScene *toReadInto, QString &debuginfo);
	SectionError readView();
	
  protected:
	bool getDataOrder();
	InitError getVersion();
	bool readBinPalette();//this is obsolete, Jun 06 2003
	
	CUViewDoc *cuvDoc;
	QIODevice *deviceIn;
	bool bytesMirrored;
	bool dataInitialized;
	bool stillMoreToRead;
	int fileVersion;
};

#endif
