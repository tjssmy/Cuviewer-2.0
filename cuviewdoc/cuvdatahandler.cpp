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

#include <qbytearray.h>
#include <qiodevice.h>
#include "../include/cuvtags.h"
#include "changeablescene.h"
#include "cuvdatahandler.h"
#include "../include/cuvdatautils.h"
#include "cuviewdoc.h"
#include <qmessagebox.h>

using namespace Qt;

CUVDataHandler::CUVDataHandler(QIODevice *ioDeviceIn, CUViewDoc *ownerDoc)
  : QObject(ownerDoc)
{
	cuvDoc = ownerDoc;
	deviceIn = ioDeviceIn;
	dataInitialized = false;
	stillMoreToRead = true;
	fileVersion = 0;
}

CUVDataHandler::~CUVDataHandler()
{
}

void CUVDataHandler::setDevice(QIODevice *iod)
{
	deviceIn = iod;
	dataInitialized = false;
	stillMoreToRead = true;
	fileVersion = 0;
}

bool CUVDataHandler::getDataOrder()
{
	cuv_data_order_tag testData;

  if ( deviceIn->read((char *)(&testData), (uint) sizeof(testData)) != (int)sizeof(testData) )
	{
		return false;
	}
	
	if (testData == (cuv_data_order_tag) DATA_NORMAL_ORDER)
	{
		bytesMirrored = false;
	}
	else if (testData == (cuv_data_order_tag) DATA_MIRROR_ORDER)
	{
		bytesMirrored = true;
	}
	else
	{
		return false;
	}
	
	return true;
}

// puts version into fileVersion
CUVDataHandler::InitError CUVDataHandler::getVersion()
{
	cuv_tag tempTag;
  QByteArray version(SHORT_VERSION_STRING);
#ifdef Q_WS_WIN
    //next line crashes in solaris
	char * ccompVersion = new char(version.length()+1);
#else
    //nextline doesn't work in win32
	char ccompVersion[version.length()+1];
#endif
  QByteArray compVersion;
	int pos = -1;

	// check the BEGIN tag
	if ( !getCUVTag(deviceIn, tempTag) )
	{
		return IErrorGettingVersion;
	}
	else if (tempTag != (cuv_tag) BEGIN_VERSION)
	{
		return IErrorInFileFormat;
	}

#ifdef Q_WS_WIN
//this is a hack that works, and I don't have a clue how it works in windows
	new LessAlloc<sphoid>; //need this here, I don't know why!!
#endif
	// read in version stuff
  if ( deviceIn->read(ccompVersion, (uint) version.length()) != (int) version.length() )
	{
		return IErrorGettingVersion;
	}

	// let's compare strings...
  ccompVersion[version.length()] = '\0'; // terminate the string
#ifdef Q_WS_WIN
//another hack that works, the QString constructor doesn't mutate the original string
	QString ccv(ccompVersion);
  compVersion = ccv.toLatin1();
#else
	//for some reason, the next line under windows changes ccompversion in the QCString = operator
	compVersion = ccompVersion;
#endif
	if (compVersion != version)
	{
		return IErrorInFileFormat;
	}

	// good, the text stuff matches, now lets get the version #....
	do
	{
    pos++;
    if ( deviceIn->read(ccompVersion+pos, sizeof(*ccompVersion)) != (int)sizeof(*ccompVersion) )
    {
      return IErrorGettingVersion;
    }
	} while ( (pos < (int) version.length()) && ((cuv_tag) ccompVersion[pos] != (cuv_tag) END_VERSION) );

	if ( ((cuv_tag) ccompVersion[pos] != (cuv_tag) END_VERSION) || (pos == 0) ) // oops, something went wrong
	{
		return IErrorInFileFormat;
	}

	ccompVersion[pos] = '\0'; // terminate the string

	// now, all that has to be done is a # extraction...
	compVersion = ccompVersion;
	fileVersion = (int) (compVersion.toDouble() * 100.0);

	qDebug("File version: %i", fileVersion);
	
	return INoError;
}

CUVDataHandler::InitError CUVDataHandler::initializeData() 
{
	InitError returnValue;
  cuv_tag tag;

	if (deviceIn == NULL)
    if (!deviceIn->isOpen() || (!deviceIn->isReadable()) ||
    deviceIn->atEnd() ) // check to make sure it can be used
    return IErrorOpening;
	if (!getDataOrder())
    return IErrorInitializing;
	if ( !getCUVTag(deviceIn, tag) )
    return IErrorInitializing;
  returnValue = getVersion(); // will eat BEGIN and END tags (or return an error)
	if (returnValue)
	{
		return returnValue; // propogate the error out
  }
	if (fileVersion > 360) // check for unsupported file version
	{
		return IErrorWrongVersion;
  }
#ifndef Q_WS_WIN
//  #  warning "Need to make program stable when dataInitialized != true..."
#endif
	dataInitialized = true;
	return INoError; // all version testing OK; version # in fileVersion
}

int CUVDataHandler::getSectionTag()
{
	cuv_tag tag=0;
	
	if (stillMoreToRead && dataInitialized)
	{
		for (;;)
		{
			if ( getCUVTag(deviceIn, tag) )
			{			
				switch (tag)
				{
				  case ((cuv_tag) BEGIN_IMG_SET):
					return (int) BEGIN_IMG_SET;
					break;
					
				  case ((cuv_tag) BEGIN_MOV_SET):
					return (int) BEGIN_MOV_SET;
					break;
					
				  case ((cuv_tag) BEGIN_SCENE):
					return (int) BEGIN_SCENE;
					break;
					
				  case ((cuv_tag) USER_INTERACT):
					return (int) USER_INTERACT;
					break;
					
				  case ((cuv_tag) BEGIN_VIEW):
					return (int) BEGIN_VIEW;
					break;

				  case ((cuv_tag) RESET_VIEW):
					return (int) RESET_VIEW;
					break;
				
				  case ((cuv_tag) GRAB_IMAGE):
					return (int) GRAB_IMAGE;
					break;
				
				  case ((cuv_tag) GRAB_FRAME):
					return (int) GRAB_FRAME;
					break;
				
				  case ((cuv_tag) END_DATA_STAY):
					stillMoreToRead = false;
					return (int) END_DATA_STAY;
					break;

				  case ((cuv_tag) END_DATA_EXIT):
					stillMoreToRead = false;
					return (int) END_DATA_EXIT;
					break;

				  default: // something isn't right...
					return -2;
					break;
				}
			}
			else
			{
				return -3;
			}

		}
	}
	else
	{
		return -1;
	}
}

CUVDataHandler::sceneStatus CUVDataHandler::readScene(ChangeableScene *toReadInto, QString &debuginfo)
{
	cuv_tag tag=0;
	sceneStatus status;
	status.points = 0;
	status.lines = 0;
	status.trias = 0;
	status.quadris = 0;
	status.spheres = 0;
	status.sphoids = 0;
	status.texts = 0;

	debuginfo = debuginfo + QString("");
	
	if (stillMoreToRead && dataInitialized && toReadInto)
	{
		for (;;)
		{
			if ( getCUVTag(deviceIn, tag) )
			{			
				switch (tag)
				{
				  case ((cuv_tag) SPOINT):
					if (toReadInto->addPoint(deviceIn, bytesMirrored))
					{
					  //debuginfo = debuginfo + QString("SPOINT hit\n");
						status.points++;
					}
					else
					{
						status.status = SErrorReading;
						return status;
					}
					break;
					
				  case ((cuv_tag) SLINE):
					if (toReadInto->addLine(deviceIn, bytesMirrored))
					{
						status.lines++;
					}
					else
					{
						status.status = SErrorReading;
						return status;
					}
					break;

				  case ((cuv_tag) STRIA):
					if (toReadInto->addTria(deviceIn, bytesMirrored))
					{
						status.trias++;
					}
					else
					{
						status.status = SErrorReading;
						return status;
					}
					break;

				  case ((cuv_tag) SQUADRI):
					if (toReadInto->addQuadri(deviceIn, bytesMirrored))
					{
					  // debuginfo = debuginfo + QString("SQUADRI hit\n");
						status.quadris++;
					}
					else
					{
						status.status = SErrorReading;
						return status;
					}
					break;

				  case ((cuv_tag) SSPHERE):
					if (toReadInto->addSphere(deviceIn, bytesMirrored))
					{
						status.spheres++;
					}
					else
					{
						status.status = SErrorReading;
						return status;
					}
					break;

				  case ((cuv_tag) SSPHOID):
					if (toReadInto->addSphoid(deviceIn, bytesMirrored))
					{
						status.sphoids++;
					}
					else
					{
						status.status = SErrorReading;
						return status;
					}
					break;

				  case ((cuv_tag) STEXT):
					if (toReadInto->addText(deviceIn, bytesMirrored))
					{
					  //debuginfo = debuginfo + QString("STEXT hit\n");
						status.texts++;
					}
					else
					{
						status.status = SErrorReading;
						return status;
					}
					break;
					
				  case ((cuv_tag) END_SCENE):
				    if(!(status.points)&&!(status.lines)&&
				       !(status.trias)&&!(status.quadris)&&
				       !(status.spheres)&&!(status.sphoids)&&!(status.texts))
				        status.status = SErrorNoData;
				    else
				        status.status = SNoError;
				    return status;
				    break;
				
				  default: // something isn't right...
				    /*
				      debuginfo = debuginfo + 
					  QString("read cuv tag %1 \n")
					  .arg(tag);
				    */
					status.status = SErrorBadTag;
					return status;
					break;
				}
			}
			else
			{
				status.status = SErrorReading;
				return status;
			}
		}
	}
	else
	{
		status.status = SErrorNoData;
		return status;
	}
}
bool CUVDataHandler::readBinPalette() //this is obsolete, Jun 06 2003
{
	cuv_ubyte paletteNumber;

	cuv_ubyte nameSize;
	bin_window_palette binWinPal;
	
	if ( getCUVUByte(deviceIn, paletteNumber) && // get palette number
		 getCUVUInt(deviceIn, binWinPal.count, bytesMirrored) ) // get palette size
	{
		if (fileVersion < 360) // throw out the old type of palette
		{
			cuv_float temp6DVect[6]; // for reading old palettes and throwing them away

			for (unsigned int i=0 ; i < binWinPal.count ; i++)
			{
				if ( !getCUVFloats(deviceIn, temp6DVect, 6, false) )
				{
					return false;
				}
			}
		}
		else // can handle the new format
		{
			if (binWinPal.count)
			{
				// if not clearing a palette
				if ( getCUVUByte(deviceIn, nameSize) ) // get palette number
				{
					if (nameSize)
					{
						if ( !getRawUTF16String(deviceIn, binWinPal.name,
												nameSize, bytesMirrored) )
						{
							// could not read in the name of the palette
							return false;
						}
						// else got the name in Unicode UTF16
					}
					
					try
					{
						binWinPal.fromPaletteStart = new GLubyte[3U*3U*binWinPal.count];
						binWinPal.fromPaletteEnd = binWinPal.fromPaletteStart +
							3U * binWinPal.count;
						binWinPal.toPalette = binWinPal.fromPaletteEnd +
							3U * binWinPal.count;
						
						// read the palette info
						if ( getCUVUBytes(deviceIn, binWinPal.fromPaletteStart,
										  3U * 3U * binWinPal.count) )
						{
							cuvDoc->setBinWindowPalette(paletteNumber, &binWinPal);
							
							delete[] binWinPal.fromPaletteStart;
						}
						else
						{
							delete[] binWinPal.fromPaletteStart;
							return false;
						}
					}
					catch (...)
					{
						qWarning("Out of memory trying to read in a palette.");
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				// need not initialize any pointers if clearing
				cuvDoc->setBinWindowPalette(paletteNumber, &binWinPal);
			}
		}
	}
	else
	{
		return false;
	}
	
	return true;
}

CUVDataHandler::SectionError CUVDataHandler::readView()
{
	cuv_tag tag;
	cuv_ubyte property;
	cuv_float tempFloat;
	cuv_float temp3DVect[3];
	cuv_float temp4DVect[4];
	
	if (stillMoreToRead && dataInitialized)
	{
		for (;;)
		{
			if ( getCUVTag(deviceIn, tag) )
			{			
				switch (tag)
				{
				  case ((cuv_tag) VCAM_TRANS):
					if ( getCUVUByte(deviceIn, property) && 
						 getCUVFloats(deviceIn, temp3DVect, 3, bytesMirrored) )
					{
						cuvDoc->cameraTranslate(temp3DVect, property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VCAM_ROTATE):
					if ( getCUVUByte(deviceIn, property) && 
						 getCUVFloats(deviceIn, temp4DVect, 4, bytesMirrored) )
					{
						cuvDoc->cameraRotate(temp4DVect, property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VCAM_ORBIT):
					if ( getCUVUByte(deviceIn, property) && 
						 getCUVFloats(deviceIn, temp4DVect, 4, bytesMirrored) )
					{
						cuvDoc->cameraOrbit(temp4DVect, temp3DVect, property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VOUTLINE_CLR):
					if ( getCUVFloats(deviceIn, temp3DVect, 3, bytesMirrored) )
					{
						cuvDoc->setOutlineColor(temp3DVect);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VPRESET_VIEW):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->gotoViewpoint(property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VVIEW_MODE):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setPerspective((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VFOV):
					if ( getCUVFloats(deviceIn, &tempFloat, 1, bytesMirrored) )
					{
						cuvDoc->setFov(tempFloat);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VCLIP_PLANE):
					if ( getCUVFloats(deviceIn, &tempFloat, 1, bytesMirrored) )
					{
						if ( cuvDoc->perspective() )
							cuvDoc->setClipPerspNear(tempFloat);
						else
							cuvDoc->setClipOrthoNear(tempFloat);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VLINE_WIDTH):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setLineWidth(property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VSHADING):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setSmoothShaded((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VBIN_PAL_SEL):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setBinWindowIndex(property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VBIN_PAL_MAP):
					if (!readBinPalette()) // reads and fills up the palette if possible
						return SErrorReading;
					break;
					
				  case ((cuv_tag) VSPHERE_TESS):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setSphereTess(property);
					}
					else
					{
						return SErrorReading;
					}
					break;

				  case ((cuv_tag) VBIN_PAL_MIN_MAX):
				    if ( getCUVFloats(deviceIn, temp3DVect, 2, bytesMirrored) )
					{
					  cuvDoc->setBinPalMinMax(temp3DVect[0],temp3DVect[1]);
					}
					else
					{
						return SErrorReading;
					}
					break;

				  case ((cuv_tag) VBIN_PAL_TYPE):
				    {
				        property = 1;
					QString typestr;
				        while( property != '\0' ) {
					  if ( getCUVUByte(deviceIn, property) )
  					  {
					        typestr += (char)property;
					  }
					  else
					  {
						return SErrorReading;
					  }
				        };
			                cuvDoc->setBinPalType(typestr);
					break;
				    }

				  case ((cuv_tag) VLIGHTING):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setLighting((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VLIGHT):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setLight((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VAMB_LIGHT):
					if ( getCUVFloats(deviceIn, &tempFloat, 1, bytesMirrored) )
					{
						cuvDoc->setAmbient(tempFloat);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VDIFF_LIGHT):
					if ( getCUVFloats(deviceIn, &tempFloat, 1, bytesMirrored) )
					{
						cuvDoc->setDiffuse(tempFloat);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VBG_LIGHT):
					if ( getCUVFloats(deviceIn, &tempFloat, 1, bytesMirrored) )
					{
						cuvDoc->setBackground(tempFloat);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VGAMMA_LEVEL):
					if ( getCUVFloats(deviceIn, &tempFloat, 1, bytesMirrored) )
					{
						cuvDoc->setGammaLevel(tempFloat);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VWIREFRAME):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setWireframe((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VOUTLINES):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setOutline((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VTWO_SIDED):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setTwoSided((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VTRANSPARENT):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setTransparency((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VBIN_PAL):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setBinWindow((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VANTIALIAS):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setAntialias((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VGAMMA):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setGamma((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;
					
				  case ((cuv_tag) VOPAQUE_WIRE):
					if ( getCUVUByte(deviceIn, property) )
					{
						cuvDoc->setOpaqueWireframe((bool) property);
					}
					else
					{
						return SErrorReading;
					}
					break;

				  case ((cuv_tag) VFOG):
					if ( getCUVUByte(deviceIn, property) )
					{
					    cuvDoc->setFog(property == VON);
					}
					else
					{
						return SErrorReading;
					}
					break;

				  case ((cuv_tag) VFOG_LEVEL):
					if ( getCUVUByte(deviceIn, property) )
					{
					    cuvDoc->setFogLevel(property);
					}
					else
					{
						return SErrorReading;
					}
					break;

				  case ((cuv_tag) END_VIEW):
					return SNoError;
					break;
				
				  default: // something isn't right...
					return SErrorBadTag;
					break;
				}
			}
			else
			{
				return SErrorReading;
			}

		}
	}
	else
	{
		return SErrorNoData;
	}
}
