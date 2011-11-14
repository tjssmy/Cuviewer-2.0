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

#ifndef DISPLAYLISTSCENE_H
#define DISPLAYLISTSCENE_H

#include <qgl.h>

#include "drawablescene.h"

class GLRenderer;

class DisplayListScene: public DrawableScene
{
  public:
	inline DisplayListScene(GLRenderer *displayListWidget)
		: DrawableScene(displayListWidget)
	{  }
	virtual ~DisplayListScene();
	
	void copyFromDrawable(DrawableScene *source);
	
	virtual void drawOpaqSolid();
	virtual void drawOpaqWire();
	virtual void drawOutline();
	virtual void drawTransSolid();
	virtual void drawTransWire();
	virtual void useDL(bool);
	
  protected:
	GLuint opaqSolidList;
	GLuint opaqWireList;
	GLuint outlineList;
	GLuint transSolidList;
	GLuint transWireList;	
};

#endif
