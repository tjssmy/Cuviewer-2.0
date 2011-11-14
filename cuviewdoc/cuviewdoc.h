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

#ifndef CUVIEWDOC_H
#define CUVIEWDOC_H

#include <qmainwindow.h>
#include <QFrame>
#include <qpixmap.h>
#include <qgl.h>

#include "glrenderer.h"
#include "entityscene.h"

class GLRenderer;
class CUVDataHandler;
class ChangeableScene;

#define CUVIEWDOC_WIDTH_HINT 320
#define CUVIEWDOC_HEIGHT_HINT 240

class CUViewDoc: public QFrame
{
	Q_OBJECT
	
  private:
	static const int sizeWidthHint;
	static const int sizeHeightHint;
	float binpalmin, binpalmax;
	QString binpaltype;

  public:
  CUViewDoc(QGLFormat * glformat, QIODevice *ioDeviceIn, QWidget *parent = 0, Qt::WidgetAttribute f = Qt::WA_DeleteOnClose);
	~CUViewDoc();

	int getLightSources();
	inline bool fixedLight(int index)
	  { return glRenderer->fixedLight(index); }
	inline void setFixedLight(int index,bool fixed)
	  { glRenderer->setFixedLight(index,fixed); }
	bool hasTranslatedLight(int);
	void getTranslateLight(int,GLfloat toFill[3]);
	inline void resetTranslateLight(int index)
	  { glRenderer->resetTranslateLight(index); }
	void translateLight(int,cuv_float,cuv_float, cuv_float);
	bool getEditLightMode(int);
	bool readData();
	void setEditLightMode(int,bool);
	void showLightPosition(bool);
	inline const void lightColor(int index, GLfloat toFill[3]) const
	{ glRenderer->lightColor(index, toFill);	 }
	inline void setLightColor(int index, const GLfloat *lightColor)
	{ 
	  glRenderer->setLightColor(index, lightColor); 
	  glRenderer->redrawDoc();
	}

  bool hasTranslated(int index);
	bool hasScaled(int);
	bool hasRotated(int);
	bool hasTiles(int);

	void translateScene(int,cuv_float,cuv_float, cuv_float);
	void scaleScene(int,cuv_float);
	void scaleScene(int,cuv_float,cuv_float, cuv_float);
	void rotateScene(int,cuv_float,cuv_float, cuv_float);
	inline float xTileLength(int index)
	  { return entityScene->xTileLength(index); }
	inline float yTileLength(int index)
	  { return entityScene->yTileLength(index); }
	inline float zTileLength(int index)
	  { return entityScene->zTileLength(index); }
	void tileScenes(int,int,int,int,float,float,float,bool translate=true);

	int scenes();
	bool isVisible(int);
	void setSceneVisible(int,bool);
	bool getEditMode();
	void setEditMode(bool);
	bool isEditing(int);
	void setSceneEditing(int,bool);
	void editExtremeties(int,float,float,float,float,float,float);
	QString extremitiesInfo();
	QString sceneInfo();
	int scenePrimitives();
	void setDevice(QIODevice *);

	float getBinPalMin();
	float getBinPalMax();
	QString getBinPalType();
	void setBinPalMinMax(float min, float max);
	void setBinPalType(QString type);

	bool initializeData();
	inline GLRenderer *getGLRenderer(){ return glRenderer; };
	virtual QSize sizeHint() const
	{
		return QSize(sizeWidthHint, sizeHeightHint);
	}
	
  private:
	EntityScene *entityScene;
	GLRenderer *glRenderer;
	CUVDataHandler *dataHandler;
	void showWarningMessage(const QString &message);
	void showCriticalMessage(const QString &message);
	void readScene(QString &debuginfo);
	void readView(QString &debuginfo);
	
  public slots:
	inline void redrawDoc()
	{ glRenderer->redrawDoc(); }
	inline void setThreeButtonMouse(bool toggle)
	{ glRenderer->setThreeButtonMouse(toggle); }
	inline void setCachedImage(bool toggle)
	{ glRenderer->setCachedImage(toggle); }
	inline void setBoxWhileMoving(bool toggle)
	{ glRenderer->setBoxWhileMoving(toggle); }
	inline void setDrawBoxAlways(bool toggle)
	{ glRenderer->setDrawBoxAlways(toggle); }
	inline void setDrawAxis(bool toggle)
	{ glRenderer->setDrawAxis(toggle); }
        inline void setDrawAxisOrigin(bool toggle)
	{ glRenderer->setDrawAxisOrigin(toggle); }

	inline void resetViewState()
	{ glRenderer->resetViewState(); }
	
	inline void gotoViewpoint(cuv_ubyte view)
	{ glRenderer->gotoViewpoint(view); }
	
	inline void setTwoSided(bool toggle)
	{ glRenderer->setTwoSided(toggle); }
  	inline void setSmoothShaded(bool toggle)
	{ glRenderer->setSmoothShaded(toggle); }
	inline void setTransparency(bool toggle)
	{ glRenderer->setTransparency(toggle); }
	inline void setAntialias(bool toggle)
	{ glRenderer->setAntialias(toggle); }
	inline void setSphereTess(cuv_ubyte sphereTess)
	{ glRenderer->setSphereTess(sphereTess); }
	
	inline void setOutline(bool toggle)
	{ glRenderer->setOutline(toggle); }
  	inline void setWireframe(bool toggle)
	{ glRenderer->setWireframe(toggle); }
  	inline void setOpaqueWireframe(bool toggle)
	{ glRenderer->setOpaqueWireframe(toggle); }
	inline void setLineWidth(GLfloat lineWidth)
	{ glRenderer->setLineWidth(lineWidth); }
	inline void setOutlineColor(const GLfloat *outlineColor)
	{ glRenderer->setOutlineColor(outlineColor); }
	
  	inline void setLight(bool toggle)
	{ glRenderer->setLight(toggle); }
  	inline void setLighting(bool toggle)
	{ glRenderer->setLighting(toggle); }
	inline void setAmbient(GLfloat brightness)
	{ glRenderer->setAmbient(brightness); }
	inline void setDiffuse(GLfloat brightness)
	{ glRenderer->setDiffuse(brightness); }
	inline void setBackground(GLfloat brightness)
	{ glRenderer->setBackground(brightness); }
	
  	inline void setPerspective(bool toggle)
	{ glRenderer->setPerspective(toggle); }
	inline void setFovScale(GLfloat fov)
	{ glRenderer->setFovScale(fov); }
	inline void setFov(GLfloat fov)
	{ glRenderer->setFov(fov); }
	inline void cameraTranslate(const GLfloat *vect, bool absolute=false)
	{ glRenderer->cameraTranslate(vect, absolute); }
	inline void cameraRotate(const GLfloat *vect, bool absolute=false)
	{ glRenderer->cameraRotate(vect, absolute); }
	inline void cameraOrbit(const GLfloat *vect, const GLfloat *center, bool absolute=false)
	{ glRenderer->cameraOrbit(vect, center, absolute); }
	inline void setCameraAimAndUp(const GLfloat *cameraAim, const GLfloat *cameraUp)
	{ glRenderer->setCameraAimAndUp(cameraAim, cameraUp); }
	inline void setClipPerspNear(GLdouble clip)
	{ glRenderer->setClipPerspNear(clip); }
	inline void setClipOrthoNear(GLdouble clip)
	{ glRenderer->setClipOrthoNear(clip); }
	
	inline void setGamma(bool toggle)
	{ glRenderer->setGamma(toggle); }
	inline void setGammaLevel(float exponent)
	{ glRenderer->setGammaLevel(exponent); }
	
	inline void setBinWindow(bool toggle)
	{ glRenderer->setBinWindow(toggle); }
	inline void setBinWindowIndex(cuv_ubyte index)
	{ glRenderer->setBinWindowIndex(index); }
	inline void setBinWindowPalette(cuv_ubyte index, const bin_window_palette *palData)
	{ glRenderer->setBinWindowPalette(index, palData); }

  public:
	inline QPixmap renderPixmap(int w=0, int h=0)
	{ return glRenderer->renderPixmap(w, h); }
	
	inline bool threeButtonMouse() const
	{ return glRenderer->threeButtonMouse(); }
	inline bool cachedImage() const
	{ return glRenderer->cachedImage(); }
	inline bool boxWhileMoving() const
	{ return glRenderer->boxWhileMoving(); }
	inline bool drawBoxAlways() const
	{ return glRenderer->drawBoxAlways(); }
	inline bool drawAxis() const
	{ return glRenderer->getDrawAxis(); }
        inline bool drawAxisOrigin() const
	{ return glRenderer->getDrawAxisOrigin(); }
	
	inline bool twoSided() const
	{ return glRenderer->twoSided(); }
  	inline bool smoothShaded() const
	{ return glRenderer->smoothShaded(); }
  	inline bool transparency() const
	{ return glRenderer->transparency(); }
  	inline bool antialias() const
	{ return glRenderer->antialias(); }
	cuv_ubyte sphereTess() const
	{ return glRenderer->sphereTess(); }

  	inline bool outline() const
	{ return glRenderer->outline(); }
  	inline bool wireframe() const
	{ return glRenderer->wireframe(); }
  	inline bool opaqueWireframe() const
	{ return glRenderer->opaqueWireframe(); }
  	inline GLfloat lineWidth() const
	{ return glRenderer->lineWidth(); }
	inline const void outlineColor(GLfloat toFill[3]) const
	{ glRenderer->outlineColor(toFill);	}

	inline float specular() 
	{ return glRenderer->specular(); }
	inline void setSpecular(cuv_float value) 
	{ glRenderer->setSpecular(value); }

	inline float fogLevel() 
	{ return glRenderer->fogLevel(); }
	inline void setFogLevel(float value) 
	{ glRenderer->setFogLevel(value); }

	inline bool fog() 
	{ return glRenderer->fog(); }
	inline void setFog(bool toggle) 
	{ glRenderer->setFog(toggle); }

  	inline bool light() const
	{ return glRenderer->light(); }
  	inline bool lighting() const
	{ return glRenderer->lighting(); }
  	inline GLfloat ambient() const
	{ return glRenderer->ambient(); }
  	inline GLfloat diffuse() const
	{ return glRenderer->diffuse(); }
  	inline GLfloat background() const
	{ return glRenderer->background(); }
	
  	inline bool perspective() const
	{ return glRenderer->perspective(); }
  	inline GLfloat fov() const
	{ return glRenderer->fov(); }
	inline void cameraPos(GLfloat toFill[3]) const
	{ glRenderer->cameraPos(toFill); }
	inline void cameraAim(GLfloat toFill[3]) const
	{ glRenderer->cameraAim(toFill); }
	inline void cameraUp(GLfloat toFill[3]) const
	{ glRenderer->cameraUp(toFill); }
	inline GLdouble clipPerspNear() const
	{ return glRenderer->clipPerspNear(); }
	inline GLdouble clipOrthoNear() const
	{ return glRenderer->clipOrthoNear(); }

	inline bool gamma() const
	{ return glRenderer->gamma(); }
	inline float gammaLevel() const
	{ return glRenderer->gammaLevel(); }
	
	inline bool binWindow() const
	{ return glRenderer->binWindow(); }
	inline cuv_ubyte binWindowIndex() const
	{ return glRenderer->binWindowIndex(); }
	inline cuv_uint binWindowColors(cuv_ubyte index) const
	{ return glRenderer->binWindowColors(index); }
	// WARNING: make sure to free the data obtained when done!!!!
	inline bin_window_palette *binWindowPalette(cuv_ubyte index) const
	{ return glRenderer->binWindowPalette(index); }
	inline void freeBinWindowPalette(bin_window_palette *toFree) const
	{ return glRenderer->freeBinWindowPalette(toFree); }
	
	inline void getTranslatef(int index,GLfloat toFill[3]) const
	  { entityScene->getTranslatef(index,toFill); }
	inline void getScalef(int index,GLfloat toFill[3]) const
	  { entityScene->getScalef(index,toFill); }
	inline void getRotatef(int index,GLfloat toFill[3]) const
	  { entityScene->getRotatef(index,toFill); }
	inline void getTiles(int index,int toFill[3],float offset[3]) const
	  { entityScene->getTiles(index,toFill,offset); }

  signals:
	void extremitiesChanged();
	void threeButtonMouseChanged(bool state);
	void cachedImageChanged(bool state);
	void boxWhileMovingChanged(bool state);
	
	void twoSidedChanged(bool state);
	void smoothShadedChanged(bool state);
	void transparencyChanged(bool state);
	void antialiasChanged(bool state);
	void sphereTessChanged(cuv_ubyte state);
	
	void outlineChanged(bool state);
	void wireframeChanged(bool state);
	void opaqueWireframeChanged(bool state);
	void lineWidthChanged(GLfloat width);
	void outlineColorChanged(const GLfloat *color);
	
	void lightChanged(bool state);
	void lightingChanged(bool state);
	void ambientChanged(GLfloat brightness);
	void diffuseChanged(GLfloat brightness);
	void backgroundChanged(GLfloat brightness);
	
	void perspectiveChanged(bool state);
	void fovChanged(GLfloat fov);
	void cameraPosChanged(const GLfloat *cameraPos);
	void cameraAimAndUpChanged(const GLfloat *cameraAim, const GLfloat *cameraUp);
	void clipPerspNearChanged(GLdouble clip);
	void clipOrthoNearChanged(GLdouble clip);
	
	void gammaChanged(bool state);
	void gammaLevelChanged(float exponent);
	
	void binWindowChanged(bool state);
	void binWindowIndexChanged(cuv_ubyte index);
	void binWindowPaletteChanged(cuv_ubyte index, const bin_window_palette *palData);
	
};

#endif
