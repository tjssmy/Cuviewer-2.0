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

#ifndef ENTITYSCENE_H
#define ENTITYSCENE_H

#include <qstring.h>
#include "../include/cuvcommonstructs.h"
#include "drawablescene.h"
#include "changeablescene.h"
#include "sceneentity.h"

class QIODevice;
class GLRenderer;
class SceneEntity;

class EntityScene: public ChangeableScene
{
  public:
	EntityScene(GLRenderer *displayListWidget);
	virtual ~EntityScene();

	QString extremitiesInfo();
	float largestExtremUnit();
	QString sceneInfo();
	int scenePrimitives();
	void getTranslatef(int,GLfloat[3]);
	void getScalef(int,GLfloat[3]);
	void getRotatef(int,GLfloat[3]);
	void getTiles(int,int[3],float[3]);
	Lessa_Text * getTextScene(int index);
	
	bool isTile(int index);
	void translateScene(int,cuv_float,cuv_float, cuv_float);
	void scaleScene(int,cuv_float);
	void scaleScene(int,cuv_float,cuv_float, cuv_float);
	void rotateScene(int,const cuv_float[4]);
	void rotateScene(int,cuv_float,cuv_float,cuv_float);
	bool addScene();
	bool removeScene();
	void setSceneExtrems();
	int scenes();
	bool currentSceneHasData();
	void setCurrentScene(int);
	bool isVisible(int);
	void setSceneVisible(int,bool); 
	bool getEditMode();
	void setEditMode(bool);
	bool isEditing(int);
	void setSceneEditing(int,bool); 
	const extremities getExtremeties();
	void editExtremeties(int,float,float,float,float,float,float);
	float xTileLength(int index);
	float yTileLength(int index);
	float zTileLength(int index);
	void tileScenes(int,int,int,int,float,float,float,bool);

	virtual void drawOpaqSolid();
	virtual void drawOpaqWire();
	virtual void drawOutline();
	virtual void drawTransSolid();
	virtual void drawTransWire();
	virtual void useDL(bool);
	
	virtual const point  *addPoint( QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const line   *addLine(  QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const tria   *addTria(  QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const quadri *addQuadri(QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const sphere *addSphere(QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const sphoid *addSphoid(QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const text   *addText(  QIODevice *ioDeviceIn, bool bytesMirrored);
	
	virtual void clear();
	
  protected:
	GLRenderer *dlw;
	int totalScenes;
	bool editingMode;
	bool extremsChanged;
	SceneEntity *sceneEntity; //the current scenentity
	SceneEntity *sceneEntityList[MAX_SCENES];
	bool tile[MAX_SCENES];
	char tileXSceneCount[MAX_SCENES],tileYSceneCount[MAX_SCENES],tileZSceneCount[MAX_SCENES]; //max char size is 255
	float tileXSceneOffset[MAX_SCENES],tileYSceneOffset[MAX_SCENES],tileZSceneOffset[MAX_SCENES];
	bool sceneVisible[MAX_SCENES];
	bool sceneEditing[MAX_SCENES];
};

#endif
