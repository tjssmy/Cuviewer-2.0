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

#include <stdio.h>

#include <QtGui>
#include <qmainwindow.h>
#include <qdatastream.h>
#include <qmessagebox.h>
#include <QVBoxLayout>
//Added by qt3to4:
#include <QFrame>
#include "../include/cuvtags.h"
#include "cuvdatahandler.h"
#include "cuviewdoc.h"
#include "entityscene.h"
#include "glrenderer.h"

const int CUViewDoc::sizeWidthHint  = CUVIEWDOC_WIDTH_HINT;
const int CUViewDoc::sizeHeightHint = CUVIEWDOC_HEIGHT_HINT;


CUViewDoc::CUViewDoc(QGLFormat * glformat, QIODevice *ioDeviceIn, QWidget *parent, Qt::WidgetAttribute f)
  : QFrame(parent)
{
	int tempInt;
	binpalmin = -1;
	binpalmax = -1;
	
  this->setAttribute(f);

  QVBoxLayout *box = new QVBoxLayout(this);
  glRenderer = new GLRenderer(glformat,this);
  glRenderer->setFocusPolicy(Qt::StrongFocus);
  box->setContentsMargins(0,0,0,0);
  box->addWidget(glRenderer);

	tempInt = glRenderer->getInitError();
	if (tempInt != GLRenderer::GNoError)
	{
		if (tempInt & GLRenderer::GErrorNoRendContext)
		{
      glRenderer->close();
			glRenderer = 0;
		}
	}
	
	entityScene = new EntityScene(glRenderer);
	
	dataHandler = new CUVDataHandler(ioDeviceIn, this);

	connect( glRenderer, SIGNAL(threeButtonMouseChanged(bool)),
						 SIGNAL(threeButtonMouseChanged(bool)) );
	connect( glRenderer, SIGNAL(cachedImageChanged(bool)),
						 SIGNAL(cachedImageChanged(bool)) );
	connect( glRenderer, SIGNAL(boxWhileMovingChanged(bool)),
						 SIGNAL(boxWhileMovingChanged(bool)) );
	
	connect( glRenderer, SIGNAL(twoSidedChanged(bool)),
						 SIGNAL(twoSidedChanged(bool)) );
	connect( glRenderer, SIGNAL(smoothShadedChanged(bool)),
						 SIGNAL(smoothShadedChanged(bool)) );
	connect( glRenderer, SIGNAL(transparencyChanged(bool)),
						 SIGNAL(transparencyChanged(bool)) );
	connect( glRenderer, SIGNAL(antialiasChanged(bool)),
						 SIGNAL(antialiasChanged(bool)) );
	connect( glRenderer, SIGNAL(sphereTessChanged(cuv_ubyte)),
						 SIGNAL(sphereTessChanged(cuv_ubyte)) );
	
	connect( glRenderer, SIGNAL(outlineChanged(bool)),
						 SIGNAL(outlineChanged(bool)) );
	connect( glRenderer, SIGNAL(wireframeChanged(bool)),
						 SIGNAL(wireframeChanged(bool)) );
	connect( glRenderer, SIGNAL(opaqueWireframeChanged(bool)),
						 SIGNAL(opaqueWireframeChanged(bool)) );
	connect( glRenderer, SIGNAL(lineWidthChanged(GLfloat)),
						 SIGNAL(lineWidthChanged(GLfloat)) );
	connect( glRenderer, SIGNAL(outlineColorChanged(const GLfloat*)),
						 SIGNAL(outlineColorChanged(const GLfloat*)) );

	connect( glRenderer, SIGNAL(lightChanged(bool)),
						 SIGNAL(lightChanged(bool)) );
	connect( glRenderer, SIGNAL(lightingChanged(bool)),
						 SIGNAL(lightingChanged(bool)) );
	connect( glRenderer, SIGNAL(ambientChanged(GLfloat)),
						 SIGNAL(ambientChanged(GLfloat)) );
	connect( glRenderer, SIGNAL(diffuseChanged(GLfloat)),
						 SIGNAL(diffuseChanged(GLfloat)) );
	connect( glRenderer, SIGNAL(backgroundChanged(GLfloat)),
						 SIGNAL(backgroundChanged(GLfloat)) );
	
	connect( glRenderer, SIGNAL(perspectiveChanged(bool)),
						 SIGNAL(perspectiveChanged(bool)) );
	connect( glRenderer, SIGNAL(fovChanged(GLfloat)),
						 SIGNAL(fovChanged(GLfloat)) );
	connect( glRenderer, SIGNAL(cameraPosChanged(const GLfloat*)),
						 SIGNAL(cameraPosChanged(const GLfloat*)) );
	connect( glRenderer, SIGNAL(cameraAimAndUpChanged(const GLfloat*, const GLfloat*)),
						 SIGNAL(cameraAimAndUpChanged(const GLfloat*, const GLfloat*)) );
	connect( glRenderer, SIGNAL(clipPerspNearChanged(GLdouble)),
						 SIGNAL(clipPerspNearChanged(GLdouble)) );
	connect( glRenderer, SIGNAL(clipOrthoNearChanged(GLdouble)),
						 SIGNAL(clipOrthoNearChanged(GLdouble)) );
	
	connect( glRenderer, SIGNAL(gammaChanged(bool)),
						 SIGNAL(gammaChanged(bool)) );
	connect( glRenderer, SIGNAL(gammaLevelChanged(float)),
						 SIGNAL(gammaLevelChanged(float)) );

	connect( glRenderer, SIGNAL(binWindowChanged(bool)),
						 SIGNAL(binWindowChanged(bool)) );
	connect( glRenderer, SIGNAL(binWindowIndexChanged(cuv_ubyte)),
						 SIGNAL(binWindowIndexChanged(cuv_ubyte)) );
	connect( glRenderer, SIGNAL(binWindowPaletteChanged(cuv_ubyte, const bin_window_palette*)),
						 SIGNAL(binWindowPaletteChanged(cuv_ubyte, const bin_window_palette*)) );
	
	// QWorspace in Qt 2.1.1 does not work (almost) correctly
	// unless the following line is put in. It has a problem
	// figuring out how to size an object that isn't sized yet.
	// (Only when making new doc and previous is maximized.)
  resize(sizeWidthHint, sizeHeightHint);
}

CUViewDoc::~CUViewDoc()
{
  //qt automatically deletes glRenderer::QGLWidget and dataHandler:QObject
	delete entityScene;
}

void CUViewDoc::setDevice(QIODevice *iod)
{
	 dataHandler->setDevice(iod);
}

float CUViewDoc::getBinPalMin(){
  return binpalmin;
}

float CUViewDoc::getBinPalMax(){
  return binpalmax;
}

QString CUViewDoc::getBinPalType(){
  return binpaltype;
}

void CUViewDoc::setBinPalMinMax(float min, float max){
  binpalmin = min;
  binpalmax = max;
}

void CUViewDoc::setBinPalType(QString type){
  binpaltype = type;
}

QString CUViewDoc::extremitiesInfo()
{
	return entityScene->extremitiesInfo();
}

QString CUViewDoc::sceneInfo()
{
	return entityScene->sceneInfo();
}

int CUViewDoc::scenePrimitives()
{
  return entityScene->scenePrimitives();
}

int CUViewDoc::getLightSources()
{
  return glRenderer->getLightSources();
}

bool CUViewDoc::hasTranslatedLight(int index)
{
  return glRenderer->hasTranslatedLight(index);
}

void CUViewDoc::getTranslateLight(int index,cuv_float toFill[3])
{
  glRenderer->getTranslateLightf(index,toFill);
}

void CUViewDoc::translateLight(int index,cuv_float x,cuv_float y, cuv_float z)
{
  glRenderer->translateLight(index,x,y,z);
}

bool CUViewDoc::getEditLightMode(int index){
  return glRenderer->getEditLightMode(index);
}

void CUViewDoc::setEditLightMode(int index,bool mode){ 
  glRenderer->setEditLightMode(index,mode);
}

void CUViewDoc::showLightPosition(bool show){
  glRenderer->showLightPosition(show);
  glRenderer->redrawDoc();
}

bool CUViewDoc::hasTranslated(int index)
{
  GLfloat toFill[3];
  entityScene->getTranslatef(index,toFill);
  return (toFill[0]||toFill[1]||toFill[2]);
}

bool CUViewDoc::hasScaled(int index)
{
  GLfloat toFill[3];
  entityScene->getScalef(index,toFill);
  return ((toFill[0]!=1)||(toFill[1]!=1)||(toFill[2]!=1));
}

bool CUViewDoc::hasRotated(int index)
{
  GLfloat toFill[3];
  entityScene->getRotatef(index,toFill);
  return (toFill[0]||toFill[1]||toFill[2]);
}

bool CUViewDoc::hasTiles(int index)
{
  int toFill[3]; float offset[3];
  entityScene->getTiles(index,toFill,offset);
  return (toFill[0]>1||toFill[1]>1||toFill[2]>1);
}

void CUViewDoc::translateScene(int scene,cuv_float x,cuv_float y, cuv_float z)
{
	entityScene->translateScene(scene,x,y,z);
}

void CUViewDoc::scaleScene(int scene,cuv_float s)
{
	entityScene->scaleScene(scene,s);
}

void CUViewDoc::scaleScene(int scene,cuv_float x,cuv_float y, cuv_float z)
{
	entityScene->scaleScene(scene,x,y,z);
}

void CUViewDoc::rotateScene(int scene,cuv_float x,cuv_float y, cuv_float z)
{
  entityScene->rotateScene(scene,x,y,z);
}

void CUViewDoc::tileScenes(int index,int xCount,int yCount,int zCount,
			   float xoffset,float yoffset,float zoffset,bool translate)
{
  entityScene->tileScenes(index, xCount, yCount, zCount,xoffset,yoffset,zoffset,translate);
}

int CUViewDoc::scenes()
{	
  return entityScene->scenes();
}

void CUViewDoc::setSceneVisible(int i, bool v)
{	
  entityScene->setSceneVisible(i, v);
}

bool CUViewDoc::isVisible(int i)
{	
  return entityScene->isVisible(i);
}

void CUViewDoc::setEditMode(bool edit){
  glRenderer->setEditMode(edit);
}

bool CUViewDoc::getEditMode(){
  return glRenderer->getEditMode();
}

void CUViewDoc::setSceneEditing(int i, bool e)
{	
  entityScene->setSceneEditing(i, e);
}

void CUViewDoc::editExtremeties(int index,float xmin,float ymin,float zmin,float xmax,float ymax,float zmax)
{
	entityScene->editExtremeties(index, xmin, ymin, zmin, xmax, ymax, zmax);
}

bool CUViewDoc::isEditing(int i)
{	
  return entityScene->isEditing(i);
}

bool CUViewDoc::initializeData()
{
	CUVDataHandler::InitError initError;
	int glInitState;
	
	if (glRenderer)
  {
		glInitState = glRenderer->getInitError();
		if (glInitState & GLRenderer::GErrorRGBABuff)
			showWarningMessage("Could not get a RGBA buffer; incorrect results may occur.");
		if (glInitState & GLRenderer::GErrorDephBuff)
			showWarningMessage("Could not get depth buffer; incorrect results may occur.");
		if (glInitState & GLRenderer::GErrorDubBuff)
			showWarningMessage("Could not get double buffer; incorrect results may occur.");
		if (glInitState & GLRenderer::GErrorAccumBuff)
			showWarningMessage("Could not get accumulation buffer; incorrect results may occur.");
	}
	else
	{
		showCriticalMessage("Could not obtain a valid GL rendering context.");
  }
  initError = dataHandler->initializeData();
	switch (initError)
	{
	  case CUVDataHandler::INoError:
		break;
		
	  case CUVDataHandler::IErrorOpening:
		showWarningMessage("Error opening data.");
		break;
		
	  case CUVDataHandler::IErrorInitializing:
		showWarningMessage("Error initializing data.");
		break;
		
	  case CUVDataHandler::IErrorGettingVersion:
		showWarningMessage("Error getting version.");
		break;
		
	  case CUVDataHandler::IErrorInFileFormat:
		showWarningMessage("Invalid file format.");
		break;
		
	  case CUVDataHandler::IErrorWrongVersion:
		showWarningMessage("Unsupported file version; only version 2 and 3 are supported.");
		break;
		
	  default:
		break;
  }
	return (initError == CUVDataHandler::INoError);
}

bool CUViewDoc::readData()
{
  int currentTag;
  QString debuginfo = QString();
  bool continueReading = true;
  do
  {
    currentTag = dataHandler->getSectionTag();
    if (currentTag < 0)
    {
      if (currentTag == -2)
      {
        if(debuginfo.isEmpty())
        {
          showWarningMessage("Missing BEGIN_SCENE tag");
        }
        else
        {
          showWarningMessage(debuginfo +
                             "Invalid section tag.");
        }
      }
      else if (currentTag == -3)
      {
        showWarningMessage(debuginfo +
                           "Error reading data or missing END_DATA_*.");
      }
      else {
        showWarningMessage(debuginfo +
                           QString("Unknown error."));
      }

      return false;
    }
    else if (currentTag > -1)
    {
      cuv_tag tag = (cuv_tag) currentTag;
      switch (tag)
	    {
        case ((cuv_tag) BEGIN_IMG_SET):
          debuginfo = debuginfo + "BEGIN_IMG_SET section hit.\n";
          break;

        case ((cuv_tag) BEGIN_MOV_SET):
          debuginfo = debuginfo + "BEGIN_MOV_SET section hit.\n";
          break;

        case ((cuv_tag) BEGIN_SCENE):
          debuginfo = debuginfo + "BEGIN_SCENE section hit.\n";
          qDebug("BEGIN_SCENE section hit.");
          //add scene if this is the first scene
          if(entityScene->scenes()&&
             !entityScene->currentSceneHasData()&&
             !entityScene->isTile(entityScene->scenes()-1)&&
             !entityScene->removeScene())
          {
            qWarning("Error trying to remove scene %i",
                     entityScene->scenes()+1);
          }
          if(entityScene->addScene())
          {
            readScene(debuginfo);
          }
          else
          {
            debuginfo = debuginfo + QString("Maximum Scenes hit: %1\n").arg(MAX_SCENES);
            qWarning( "Maximum Scenes hit: %i\n", MAX_SCENES );
          }
          break;

        case ((cuv_tag) USER_INTERACT):
          debuginfo = debuginfo + "USER_INTERACT section hit.\n";
          qDebug("USER_INTERACT section hit.");
          continueReading = false;
          break;

        case ((cuv_tag) BEGIN_VIEW):
          debuginfo = debuginfo + "BEGIN_VIEW section hit\n";
          qDebug("BEGIN_VIEW section hit.");
          readView(debuginfo);
          break;

        case ((cuv_tag) RESET_VIEW):
          debuginfo = debuginfo + "RESET_VIEW hit\n";
          glRenderer->resetViewState();
          break;

        case ((cuv_tag) GRAB_IMAGE):
          debuginfo = debuginfo + "GRAB_IMAGE section hit.\n";
          break;

        case ((cuv_tag) GRAB_FRAME):
          debuginfo = debuginfo + "GRAB_FRAME section hit.\n";
          break;

        case ((cuv_tag) END_DATA_STAY):
          debuginfo = debuginfo + "END_DATA_STAY section hit.\n";
          qDebug("END_DATA_STAY section hit.");
          continueReading = false;
          break;

        case ((cuv_tag) END_DATA_EXIT):
          debuginfo = debuginfo + "END_DATA_EXIT section hit.\n";
          qDebug("END_DATA_EXIT section hit.");
          continueReading = false;
          break;

        case ((cuv_tag) END_IMG_SET):
          debuginfo = debuginfo + "END_IMG_SET section hit.\n";
          break;

        case ((cuv_tag) END_MOV_SET):
          debuginfo = debuginfo + "END_MOV_SET section hit.\n";
          break;

        case ((cuv_tag) END_SCENE):
          debuginfo = debuginfo + "END_SCENE section hit.\n";
          break;

        case ((cuv_tag) END_VIEW):
          debuginfo = debuginfo + "END_VIEW section hit.\n";
          break;

        case ((cuv_tag) VBIN_PAL_MAP):
          debuginfo = debuginfo + "VBIN_PAL_MAP section hit.\n";
          break;

        case ((cuv_tag) 0x00):
          debuginfo = debuginfo + "0x00 hit.\n";
          break;

        default: // something isn't right...
          debuginfo = debuginfo + "Unknown tag???\n";
          showWarningMessage("Unknown tag???");
          break;
	    }
    }
  } while (continueReading);

  //check last scene has data, if not, remove
  if(!entityScene->currentSceneHasData())
    if(!entityScene->removeScene())
      qWarning("Error trying to remove scene %i",entityScene->scenes()+1);

  if(currentTag==(cuv_tag) END_DATA_EXIT)
  {
    close();
  }
  return true;
}

void CUViewDoc::readScene(QString &debuginfo)
{
  CUVDataHandler::sceneStatus sceneStat = dataHandler->readScene(entityScene,debuginfo);
  switch (sceneStat.status)
  {
    case CUVDataHandler::SNoError :
      debuginfo = debuginfo + "END_SCENE hit\n";
      break;
    case CUVDataHandler::SErrorNoData : 
      debuginfo = debuginfo + "Warning, hit a scene with no section data\n";
      qWarning("Warning, hit a scene with no section data");
      break;
    case CUVDataHandler::SErrorReading : 
      debuginfo = debuginfo + "Error, reading scene " +
                  QString::number(entityScene->scenes())+"\n";
      qWarning("Error, reading scene %i",entityScene->scenes());
      break;
    case CUVDataHandler::SErrorBadTag:
      debuginfo = debuginfo + "Error bad scene tag or missing END_SCENE \n"+
              QString("%1point %2line %3triangle %4quad %5sphere %6sphoid %7text\n")
              .arg((int)sceneStat.points).arg((int)sceneStat.lines)
              .arg((int)sceneStat.trias).arg((int)sceneStat.quadris)
              .arg((int)sceneStat.spheres).arg((int)sceneStat.sphoids)
              .arg((int)sceneStat.texts);
      qWarning("Error bad scene tag or missing END_SCENE \n");
      break;
  }
  entityScene->setSceneExtrems();
  glRenderer->useEntityScene(entityScene);
}

void CUViewDoc::readView(QString &debuginfo)
{
  CUVDataHandler::SectionError sectionError = dataHandler->readView();
  switch(sectionError)
  {
    case(CUVDataHandler::SErrorBadTag):
      debuginfo = debuginfo + "Error bad view tag or missing END_VIEW\n";
      break;
    case(CUVDataHandler::SErrorReading):
      debuginfo = debuginfo + "Error reading view data\n";
      break;
    case(CUVDataHandler::SNoError):
      debuginfo = debuginfo + "END_VIEW hit\n";
      break;
	  case(CUVDataHandler::SErrorNoData):
		  debuginfo = debuginfo + "No Data\n";
		  break;
  };
}

void CUViewDoc::showWarningMessage(const QString &message)
{
	QMessageBox::critical(this, VERSION_STRING, message, QMessageBox::Abort
						  | QMessageBox::Default | QMessageBox::Escape, 0);
}

void CUViewDoc::showCriticalMessage(const QString &message)
{
	QMessageBox::warning(this, VERSION_STRING, message, QMessageBox::Ok
						 | QMessageBox::Default | QMessageBox::Escape, 0);
	close();
}
