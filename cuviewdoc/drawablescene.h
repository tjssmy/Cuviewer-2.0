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

#ifndef DRAWABLESCENE_H
#define DRAWABLESCENE_H

#include <qgl.h>

#ifdef Q_OS_MAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <qlist.h>
#include "../include/cuvcommonstructs.h"
#include "glrenderer.h"
#include <cfloat> // for FLT_MAX

//class GLRenderer;

class DrawableScene
{
  protected:
  typedef QList<GLuint> DisplayLists;
	
	DisplayLists dispLists;
	GLRenderer *glRenderer;
	extremities extrem;
	
	bool opaqSolidData;
	bool opaqWireData;
	bool outlineData;
	bool transSolidData;
	bool transWireData;
	
	void registerList(GLuint toRegister);
	void unregisterList(GLuint toUnregister);
	void unregisterLists();
	void newMinMax(const cuv_float vect[3]);
	
  public:
	DrawableScene(GLRenderer *glRendererWidget);
	virtual ~DrawableScene();
	const extremities &extrems();
	void pos(cuv_float toFill[3]);
	void halfSize(cuv_float toFill[3]);
	void compareMinMax(const extremities);
	const DisplayLists &usedDisplayLists();
	virtual void drawOpaqSolid()  = 0;
	virtual void drawOpaqWire()   = 0;
	virtual void drawOutline()    = 0;
	virtual void drawTransSolid() = 0;
	virtual void drawTransWire()  = 0;
	virtual void useDL(bool) = 0;
	
	void drawLight();
	bool opaqSolidExists();
	bool opaqWireExists();
	bool outlineExists();
	bool transSolidExists();
	bool transWireExists();
};

inline void DrawableScene::registerList(GLuint toRegister)
{
	dispLists.append(toRegister);
	glRenderer->registerList(toRegister);
}

inline void DrawableScene::unregisterList(GLuint toUnregister)
{
	glRenderer->releaseList(toUnregister);
  dispLists.removeAll(toUnregister);
}

inline void DrawableScene::unregisterLists()
{
	DisplayLists::Iterator it;
	for(it = dispLists.begin() ; it != dispLists.end() ; it++)
		glRenderer->releaseList(*it);
	
	dispLists.clear();
}

inline void DrawableScene::newMinMax(const cuv_float vect[3])
{
	if (vect[0] < extrem.xmin) extrem.xmin = vect[0];
	if (vect[0] > extrem.xmax) extrem.xmax = vect[0];
	if (vect[1] < extrem.ymin) extrem.ymin = vect[1];
	if (vect[1] > extrem.ymax) extrem.ymax = vect[1];
	if (vect[2] < extrem.zmin) extrem.zmin = vect[2];
	if (vect[2] > extrem.zmax) extrem.zmax = vect[2];
}

inline DrawableScene::DrawableScene(GLRenderer *glRendererWidget)
{
	opaqSolidData = false;
	opaqWireData = false;
	outlineData = false;
	transSolidData = false;
	transWireData = false;
		
	extrem.xmin = FLT_MAX; extrem.xmax = -FLT_MAX; // set to opposite extremities so that the real
	extrem.ymin = FLT_MAX; extrem.ymax = -FLT_MAX; // exremities can be found (the initialization
	extrem.zmin = FLT_MAX; extrem.zmax = -FLT_MAX; // value cannot be zero).
		
	glRenderer = glRendererWidget;
}

inline DrawableScene::~DrawableScene()
{
	unregisterLists();
}

inline const extremities &DrawableScene::extrems()
{
	return extrem;
}

inline void DrawableScene::pos(cuv_float toFill[3])
{
	toFill[0] = 0.5*extrem.xmax + 0.5*extrem.xmin;
	toFill[1] = 0.5*extrem.ymax + 0.5*extrem.ymin;
	toFill[2] = 0.5*extrem.zmax + 0.5*extrem.zmin;
}

inline void DrawableScene::halfSize(cuv_float toFill[3])
{
	toFill[0] = 0.5*(extrem.xmax - extrem.xmin);
	toFill[1] = 0.5*(extrem.ymax - extrem.ymin);
	toFill[2] = 0.5*(extrem.zmax - extrem.zmin);
}

inline void DrawableScene::compareMinMax(const extremities ex)
{
	if (extrem.xmin >  ex.xmin) extrem.xmin = ex.xmin; 
	if (extrem.xmax < ex.xmax) extrem.xmax = ex.xmax;
	if (extrem.ymin >  ex.ymin) extrem.ymin = ex.ymin;
	if (extrem.ymax < ex.ymax) extrem.ymax = ex.ymax;
	if (extrem.zmin >  ex.zmin) extrem.zmin = ex.zmin;
	if (extrem.zmax < ex.zmax) extrem.zmax = ex.zmax;
}

inline const DrawableScene::DisplayLists &DrawableScene::usedDisplayLists()
{
	return dispLists;
}

inline void DrawableScene::drawLight()
{
  for(int i=0;i<glRenderer->getLightSources();i++)
    if(glRenderer->fixedLight(i))
      glRenderer->drawLight(i);
}

inline bool DrawableScene::opaqSolidExists()
{
	return opaqSolidData;
}

inline bool DrawableScene::opaqWireExists()
{
	return opaqWireData;
}

inline bool DrawableScene::outlineExists()
{
	return outlineData;
}

inline bool DrawableScene::transSolidExists()
{
	return transSolidData;
}

inline bool DrawableScene::transWireExists()
{
	return transWireData;
}

#endif





