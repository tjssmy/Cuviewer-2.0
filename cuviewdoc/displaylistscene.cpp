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

#include <qgl.h>

#include <qlist.h>
#include "glrenderer.h"
#include "drawablescene.h"
#include "displaylistscene.h"

void DisplayListScene::copyFromDrawable(DrawableScene *source)
{
	int genListCount = 0;
	GLuint listBase;
	unregisterLists();
	
	if (source->opaqSolidExists())
	{
		opaqSolidData = true;
		genListCount++;
	}
	else
	{
		opaqSolidData = false;
	}

	if (source->opaqWireExists())
	{
		opaqWireData = true;
		genListCount++;
	}
	else
	{
		opaqWireData = false;
	}

	if (source->outlineExists())
	{
		outlineData = true;
		genListCount++;
	}
	else
	{
		outlineData = false;
	}

	if (source->transSolidExists())
	{
		transSolidData = true;
		genListCount++;
	}
	else
	{
		transSolidData = false;
	}

	if (source->transWireExists())
	{
		transWireData = true;
		genListCount++;
	}
	else
	{
		transWireData = false;
	}
	
	glRenderer->makeCurrent();
	listBase = glGenLists(genListCount);
	
	if (listBase)
	{
		if (opaqSolidData)
		{
			registerList(listBase);
			opaqSolidList = listBase++;
			
			glNewList(opaqSolidList, GL_COMPILE);
			source->drawOpaqSolid();
			glEndList();
		}
			
		if (opaqWireData)
		{
			registerList(listBase);
			opaqWireList = listBase++;
			
			glNewList(opaqWireList, GL_COMPILE);
			source->drawOpaqWire();
			glEndList();
		}
		
		if (outlineData)
		{
			registerList(listBase);
			outlineList = listBase++;
			
			glNewList(outlineList, GL_COMPILE);
			source->drawOutline();
			glEndList();
		}
		
		if (transSolidData)
		{
			registerList(listBase);
			transSolidList = listBase++;
			
			glNewList(transSolidList, GL_COMPILE);
			source->drawTransSolid();
			glEndList();
		}
		
		if (transWireData)
		{
			registerList(listBase);
			transWireList = listBase;
			
			glNewList(transWireList, GL_COMPILE);
			source->drawTransWire();
			glEndList();
		}
		
		extrem = source->extrems(); // copy extremities
		
		// reserve these list since needed to draw (in case source is deleted)
		const DisplayLists &srcLists = source->usedDisplayLists();
		DisplayLists::ConstIterator it;
		for (it = srcLists.begin() ; it != srcLists.end() ; it++)
			registerList(*it);
	}
	else
	{
		opaqSolidData = false;
		opaqWireData = false;
		outlineData = false;
		transSolidData = false;
		transWireData = false;
		
		qWarning("Could not allocate the needed display lists.");
	}
}

DisplayListScene::~DisplayListScene()
{
}

void DisplayListScene::drawOpaqSolid()
{
	if (opaqSolidData)
		glCallList(opaqSolidList);
}

void DisplayListScene::drawOpaqWire()
{
	if (opaqWireData)
		glCallList(opaqWireList);
}

void DisplayListScene::drawOutline()
{
	if (outlineData)
		glCallList(outlineList);
}

void DisplayListScene::drawTransSolid()
{
	if (transSolidData)
		glCallList(transSolidList);
}

void DisplayListScene::drawTransWire()
{
	if (transWireData)
		glCallList(transWireList);
}

void DisplayListScene::useDL(bool)
{
}


