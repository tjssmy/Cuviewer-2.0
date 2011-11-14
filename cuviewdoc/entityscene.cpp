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
#include <qapplication.h>
#include <qstring.h>
#include <qiodevice.h>
#include <qgl.h>
#include <qmessagebox.h>

#include "../include/cuvtags.h"
#include "../include/cuvcommonstructs.h"
#include "../include/cuvdatautils.h"
#include "glrenderer.h"
#include "changeablescene.h"
#include "sceneentity.h"
#include "entityscene.h"

EntityScene::EntityScene(GLRenderer *displayListWidget) : ChangeableScene(displayListWidget)
{
	editingMode = false;
	dlw = displayListWidget;
	totalScenes = 0;
	sceneEntity = NULL;
	extremsChanged = FALSE;
	for(int i=0;i<MAX_SCENES;i++){
	  tile[i]=0;
	  tileXSceneCount[i]=1;
	  tileYSceneCount[i]=1;
	  tileZSceneCount[i]=1;
	  tileXSceneOffset[i]=-1; //default to xunit step
	  tileYSceneOffset[i]=-1;
	  tileZSceneOffset[i]=-1;
	}
}

EntityScene::~EntityScene()
{
  //qt automatically delete dlw:QGLWidget
  for(int i = 0; i<totalScenes;i++) {
    delete sceneEntityList[i];
  }

}

QString EntityScene::extremitiesInfo()
{
	QString info;
	for(int i = 0;i<scenes();i++){
		info += QString("<Scene%1> ").arg(i+1);
		info += sceneEntityList[i]->extremitiesInfo();
	}
	info += QString("<All scenes>X min%1 max%2, Y min%3 max%4, Z: min%5 max%6\n")
		.arg((float)extrem.xmin).arg((float)extrem.xmax)
		.arg((float)extrem.ymin).arg((float)extrem.ymax)
		.arg((float)extrem.zmin).arg((float)extrem.zmax);
	return info;
}

float EntityScene::largestExtremUnit(){
  float largest = sceneEntityList[0]->largestExtremUnit();
  for(int i = 0;i<scenes();i++){
    if (largest < sceneEntityList[i]->largestExtremUnit()){
      largest = sceneEntityList[i]->largestExtremUnit();
    }
  }
  return largest;
}

QString EntityScene::sceneInfo()
{
	QString info;
	for(int i = 0;i<scenes();i++){
		info += QString("<Scene%1> ").arg(i+1);
		info += sceneEntityList[i]->sceneInfo();
	}
	return info;
}

int EntityScene::scenePrimitives()
{
  int primitives=0;
  for(int i = 0;i<scenes();i++){
    primitives += sceneEntityList[i]->scenePrimitives();
  }
  return primitives;
}

void EntityScene::getTranslatef(int index,GLfloat toFill[3])
{
  if(index<MAX_SCENES)
    sceneEntityList[index]->getTranslatef(toFill);
}

void EntityScene::getScalef(int index,GLfloat toFill[3])
{
  if(index<MAX_SCENES)
    sceneEntityList[index]->getScalef(toFill);
}

void EntityScene::getRotatef(int index,GLfloat toFill[3])
{
  if(index<MAX_SCENES)
    sceneEntityList[index]->getRotatef(toFill);
}

void EntityScene::getTiles(int index,int toFill[3],float offset[3])
{
  if(index<MAX_SCENES){
    toFill[0]=tileXSceneCount[index];
    toFill[1]=tileYSceneCount[index];
    toFill[2]=tileZSceneCount[index];
    offset[0]=tileXSceneOffset[index];
    offset[1]=tileYSceneOffset[index];
    offset[2]=tileZSceneOffset[index];
  }
}

Lessa_Text * EntityScene::getTextScene(int index)
{
  //index is the scene index
  //returns 0 if there are no scenes, or no text items
  Lessa_Text * lstText = sceneEntityList[index]->getLstText();
  if(lstText && lstText->size()){
    return lstText;
  }
  return 0;
}

bool EntityScene::isTile(int index){
  if(index<MAX_SCENES)
    return tile[index];
  return false;
}

void EntityScene::translateScene(int scene,cuv_float x,cuv_float y, cuv_float z)
{
  if(scene>-1 && scene<totalScenes){
    if(editingMode||sceneEditing[scene]){
      sceneEntityList[scene]->translate(x,y,z);
      extremsChanged = TRUE;
    }
  }
}

void EntityScene::scaleScene(int scene,cuv_float s)
{
  if(scene>-1 && scene<totalScenes){
    if(editingMode||sceneEditing[scene]){
      sceneEntityList[scene]->scale(s);
      extremsChanged = TRUE;
    }
  }
}

void EntityScene::scaleScene(int scene,cuv_float x,cuv_float y, cuv_float z)
{
  if(scene>-1 && scene<totalScenes){
    if(editingMode||sceneEditing[scene]){
      sceneEntityList[scene]->scale(x,y,z);
      extremsChanged = TRUE;
    }
  }
}

void EntityScene::rotateScene(int scene,const cuv_float rot[4]){ 
  if(scene>-1 && scene<totalScenes){
    if(editingMode||sceneEditing[scene]){
      sceneEntityList[scene]->rotate(rot);
      extremsChanged = TRUE;
    }
  }
}

void EntityScene::rotateScene(int scene,cuv_float x,cuv_float y,cuv_float z){ 
  if(scene>-1 && scene<totalScenes){
    if(editingMode||sceneEditing[scene]){
      sceneEntityList[scene]->rotate(x,y,z);
      extremsChanged = TRUE;
    }
  }
}

//instances a new SceneEntity, makes it current for reading in new data
bool EntityScene::addScene()
{
  if(totalScenes>=MAX_SCENES)
    return false;
  sceneEntityList[totalScenes] = new SceneEntity(dlw);
  setCurrentScene(totalScenes); //set sceneEntity
  sceneVisible[totalScenes] = true;
  //first scene is at index 0
  ++totalScenes;
  return true;
}

bool EntityScene::removeScene()
{
  if(!sceneEntity||!totalScenes)
    return false;
  qDebug("removing scene %i",totalScenes);
  delete sceneEntity;
  sceneEntityList[totalScenes-1]=NULL;
  sceneVisible[totalScenes-1] = false;
  //first scene is at index 0
  --totalScenes;
  sceneEntity = (totalScenes>0) ? sceneEntityList[totalScenes-1]:NULL;
  return true;
}

void EntityScene::setSceneExtrems()
{
  if(extremsChanged){
    extrem.xmin = FLT_MAX; extrem.xmax = -FLT_MAX; // set to opposite extremities so that the real
    extrem.ymin = FLT_MAX; extrem.ymax = -FLT_MAX; // exremities can be found (the initialization
    extrem.zmin = FLT_MAX; extrem.zmax = -FLT_MAX; // value cannot be zero).
    for(int i=0;i<totalScenes;i++) 
		//if(sceneEntityList[i]->isVisible())
      compareMinMax(sceneEntityList[i]->extrems());
    extremsChanged = FALSE;
  }
}

void EntityScene::setCurrentScene(int index)
{
	sceneEntity = sceneEntityList[index];
}

void EntityScene::setSceneVisible(int index,bool visible)
{
	if(index>totalScenes-1)
		return;
	sceneVisible[index] = visible;
}

bool EntityScene::isVisible(int index)
{
	if(index>totalScenes-1)
		return FALSE;
	return sceneVisible[index];
}

//editingMode==true when any entity is selected for editing.
bool EntityScene::getEditMode(){
  return editingMode;
}

void EntityScene::setEditMode(bool edit){
  editingMode = edit;
}

void EntityScene::setSceneEditing(int index,bool editing)
{
	if(index>totalScenes-1)
	  return;
	sceneEntityList[index]->setEditEntity(editing);
}

const extremities EntityScene::getExtremeties()
{
  return extrem;
}

void EntityScene::editExtremeties(int index,float xmin,float ymin,float zmin,float xmax,float ymax,float zmax)
{
	if(index>totalScenes-1)
	  return;
	sceneEntityList[index]->editExtremeties(xmin, ymin, zmin, xmax, ymax, zmax);
}

//TODO make use of index, which refers to the scene index
float EntityScene::xTileLength(int index){
	index=0;
  return (float)(extrem.xmax-extrem.xmin);
}

float EntityScene::yTileLength(int index){
	index = 0;
  return (float)(extrem.ymax-extrem.ymin);
}

float EntityScene::zTileLength(int index){
		index=0;
  return (float)(extrem.zmax-extrem.zmin);
}

void EntityScene::tileScenes(int index,int xCount,int yCount,int zCount,
			     float xoffset,float yoffset,float zoffset, bool translate)
{
  if(index>totalScenes-1)
    return;
  tileXSceneCount[index]=xCount;
  tileYSceneCount[index]=yCount;
  tileZSceneCount[index]=zCount;
  tileXSceneOffset[index]=xoffset;
  tileYSceneOffset[index]=yoffset;
  tileZSceneOffset[index]=zoffset;
  //  bool useoffset=xoffset>0||yoffset>0||zoffset>0;
    
  float xunit, yunit, zunit;
  GLfloat trans[3] = {0,0,0};
  sceneEntityList[index]->getTranslatef(trans);
  xunit = extrem.xmax-extrem.xmin+trans[0];
  yunit = extrem.ymax-extrem.ymin+trans[1];
  zunit = extrem.zmax-extrem.zmin+trans[2];
  /*  qDebug("i %i %f %f %f\n", index, trans[0], trans[1], trans[2]); 
  qDebug("-> %f %f %f\n", xunit, yunit, zunit); 
  */
  for(int i=1;i<=xCount;i++)
    for(int j=1;j<=yCount;j++)
      for(int k=1;k<=zCount;k++)
	if(!((i==1)&&(j==1)&&(k==1))){
	  tile[totalScenes]=TRUE; //identify current scene as a tile.
	  addScene();
	  //sceneEntity is the newly added scene
	  sceneEntity->setEditEntity(true);
	  sceneEntityList[index]->copyScene(sceneEntity);
	  //	  qDebug("%i  %i %i %i",index,i,j,k);
	  //	  if(useoffset){
	  //	    sceneEntity->getEntityUnit(&xunit,&yunit,&zunit);
	    xoffset = tileXSceneOffset[index]>0 ? xoffset : xunit;
	    yoffset = tileYSceneOffset[index]>0 ? yoffset : yunit;
	    zoffset = tileZSceneOffset[index]>0 ? zoffset : zunit;
	    /*	    qDebug("%i offsets %f %f %f",index,xoffset,yoffset,zoffset);
	    qDebug("%i trans %f %f %f",index,(i-1)*xoffset,(j-1)*yoffset,(k-1)*zoffset);
	    */
	    if(translate)
	      sceneEntity->translate((i-1)*xoffset+trans[0],
				     (j-1)*yoffset+trans[1],
				     (k-1)*zoffset+trans[2]);
	    /*	  }
	  else //translate in X,Y,Z by sceneentity units
	    if(translate)
	      sceneEntity->translateUnits(i-1,j-1,k-1);
	    */
	  extremsChanged = TRUE;
	  sceneEntity->setEditEntity(false);
	}
}

bool EntityScene::isEditing(int index)
{
	if(index>totalScenes-1)
	  return FALSE;
	return sceneEntityList[index]->getEditEntity();
}

int EntityScene::scenes()
{
	return totalScenes;
}

bool EntityScene::currentSceneHasData()
{
  if(sceneEntity)
     return sceneEntity->hasData();
  else
    return false;
}

void EntityScene::drawOpaqSolid()
{
	for(int i = 0; i<totalScenes;i++)
	if (sceneVisible[i]){
		glPushMatrix();
		sceneEntityList[i]->transform();
		sceneEntityList[i]->drawOpaqSolid();
		glPopMatrix();
	}
}

void EntityScene::drawOpaqWire()
{
	for(int i = 0; i<totalScenes;i++)
	if (sceneVisible[i]){
		glPushMatrix();
		sceneEntityList[i]->transform();
	    sceneEntityList[i]->drawOpaqWire();
		glPopMatrix();
	}
}

void EntityScene::drawOutline()
{
	for(int i = 0; i<totalScenes;i++)
	if (sceneVisible[i]){
		glPushMatrix();
		sceneEntityList[i]->transform();
	    sceneEntityList[i]->drawOutline();
		glPopMatrix();
	}
}

void EntityScene::drawTransSolid()
{
	for(int i = 0; i<totalScenes;i++)
	if (sceneVisible[i]){
		glPushMatrix();
		sceneEntityList[i]->transform();
	    sceneEntityList[i]->drawTransSolid();
		glPopMatrix();
	}
}

void EntityScene::drawTransWire()
{
	for(int i = 0; i<totalScenes;i++)
	if (sceneVisible[i]){
		glPushMatrix();
		sceneEntityList[i]->transform();
	    sceneEntityList[i]->drawTransWire();
		glPopMatrix();
	}
}

void EntityScene::useDL(bool usedl)
{
  for(int i = 0; i<totalScenes;i++)
    if (sceneVisible[i])
      sceneEntityList[i]->useDL(usedl);
}

const point *EntityScene::addPoint(QIODevice *ioDeviceIn, bool bytesMirrored)
{
	const point *toReturn;
	cuv_tag flags;
	
	if (!getCUVTag(ioDeviceIn, flags))
	{
		return NULL;
	}
	
	toReturn = sceneEntity->addPoint(flags, ioDeviceIn, bytesMirrored);
	if (toReturn)
	{
		opaqSolidData = opaqSolidData||sceneEntity->opaqSolidExists();
		opaqWireData = opaqWireData||sceneEntity->opaqWireExists();
		outlineData = outlineData||sceneEntity->outlineExists();
		transSolidData = transSolidData||sceneEntity->transSolidExists();
		transWireData = transWireData||sceneEntity->transWireExists();
		compareMinMax(sceneEntity->extrems());
//		extrem = sceneEntity->extrems();
	}

	return toReturn;
}

const line *EntityScene::addLine(QIODevice *ioDeviceIn, bool bytesMirrored)
{
	const line *toReturn;
	cuv_tag flags;
	
	if (!getCUVTag(ioDeviceIn, flags))
	{
		return NULL;
	}
	
	toReturn = sceneEntity->addLine(flags, ioDeviceIn, bytesMirrored);
	if (toReturn)
	{
		opaqSolidData = opaqSolidData||sceneEntity->opaqSolidExists();
		opaqWireData = opaqWireData||sceneEntity->opaqWireExists();
		outlineData = outlineData||sceneEntity->outlineExists();
		transSolidData = transSolidData||sceneEntity->transSolidExists();
		transWireData = transWireData||sceneEntity->transWireExists();
		compareMinMax(sceneEntity->extrems());
//		extrem = sceneEntity->extrems();
	}
	
	return toReturn;
}

const tria *EntityScene::addTria(QIODevice *ioDeviceIn, bool bytesMirrored)
{
	const tria *toReturn;
	cuv_tag flags;
	
	if (!getCUVTag(ioDeviceIn, flags))
	{
		return NULL;
	}
	
	toReturn = sceneEntity->addTria(flags, ioDeviceIn, bytesMirrored);
	if (toReturn)
	{
		opaqSolidData = opaqSolidData||sceneEntity->opaqSolidExists();
		opaqWireData = opaqWireData||sceneEntity->opaqWireExists();
		outlineData = outlineData||sceneEntity->outlineExists();
		transSolidData = transSolidData||sceneEntity->transSolidExists();
		transWireData = transWireData||sceneEntity->transWireExists();
		compareMinMax(sceneEntity->extrems());
//		extrem = sceneEntity->extrems();
	}
	
	return toReturn;
}

const quadri *EntityScene::addQuadri(QIODevice *ioDeviceIn, bool bytesMirrored)
{
	const quadri *toReturn;
	cuv_tag flags;
	
	if (!getCUVTag(ioDeviceIn, flags))
	{
		return NULL;
	}
	
	toReturn = sceneEntity->addQuadri(flags, ioDeviceIn, bytesMirrored);
	if (toReturn)
	{
		opaqSolidData = opaqSolidData||sceneEntity->opaqSolidExists();
		opaqWireData = opaqWireData||sceneEntity->opaqWireExists();
		outlineData = outlineData||sceneEntity->outlineExists();
		transSolidData = transSolidData||sceneEntity->transSolidExists();
		transWireData = transWireData||sceneEntity->transWireExists();
		compareMinMax(sceneEntity->extrems());
//		extrem = sceneEntity->extrems();
	}
	
	return toReturn;
}

const sphere *EntityScene::addSphere(QIODevice *ioDeviceIn, bool bytesMirrored)
{
	const sphere *toReturn;
	cuv_tag flags;
	
	if (!getCUVTag(ioDeviceIn, flags))
	{
		return NULL;
	}
	
	toReturn = sceneEntity->addSphere(flags, ioDeviceIn, bytesMirrored);
	if (toReturn)
	{
		opaqSolidData = opaqSolidData||sceneEntity->opaqSolidExists();
		opaqWireData = opaqWireData||sceneEntity->opaqWireExists();
		outlineData = outlineData||sceneEntity->outlineExists();
		transSolidData = transSolidData||sceneEntity->transSolidExists();
		transWireData = transWireData||sceneEntity->transWireExists();
		compareMinMax(sceneEntity->extrems());
//		extrem = sceneEntity->extrems();
	}
	
	return toReturn;
}

const sphoid *EntityScene::addSphoid(QIODevice *ioDeviceIn, bool bytesMirrored)
{
	const sphoid *toReturn;
	cuv_tag flags;
	
	if (!getCUVTag(ioDeviceIn, flags))
	{
		return NULL;
	}
	
	toReturn = sceneEntity->addSphoid(flags, ioDeviceIn, bytesMirrored);
	if (toReturn)
	{
		opaqSolidData = opaqSolidData||sceneEntity->opaqSolidExists();
		opaqWireData = opaqWireData||sceneEntity->opaqWireExists();
		outlineData = outlineData||sceneEntity->outlineExists();
		transSolidData = transSolidData||sceneEntity->transSolidExists();
		transWireData = transWireData||sceneEntity->transWireExists();
		compareMinMax(sceneEntity->extrems());
//		extrem = sceneEntity->extrems();
	}

	return toReturn;
}

const text *EntityScene::addText(QIODevice *ioDeviceIn, bool bytesMirrored)
{
	const text *toReturn;
	cuv_tag flags;
	
	if (!getCUVTag(ioDeviceIn, flags))
	{
		return NULL;
	}
	
	toReturn = sceneEntity->addText(flags, ioDeviceIn, bytesMirrored);
	if (toReturn)
	{
		opaqSolidData = opaqSolidData||sceneEntity->opaqSolidExists();
		opaqWireData = opaqWireData||sceneEntity->opaqWireExists();
		outlineData = outlineData||sceneEntity->outlineExists();
		transSolidData = transSolidData||sceneEntity->transSolidExists();
		transWireData = transWireData||sceneEntity->transWireExists();
		compareMinMax(sceneEntity->extrems());
//		extrem = sceneEntity->extrems();
	}

	return toReturn;
}

//clears current sceneEntity
void EntityScene::clear()
{
	sceneEntity->clear();
	extrem.xmin = FLT_MAX; extrem.xmax = -FLT_MAX; // set to opposite extremities so that the real
	extrem.ymin = FLT_MAX; extrem.ymax = -FLT_MAX; // exremities can be found (the initialization
	extrem.zmin = FLT_MAX; extrem.zmax = -FLT_MAX; // value cannot be zero).
}
