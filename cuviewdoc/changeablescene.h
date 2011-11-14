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

#ifndef CHANGEABLESCENE_H
#define CHANGEABLESCENE_H

#include "drawablescene.h"
#include "../include/cuvcommonstructs.h"

class QIODevice;
class GLRenderer;

class ChangeableScene: public DrawableScene
{
  public:
	inline ChangeableScene(GLRenderer *displayListWidget)
	: DrawableScene(displayListWidget)
	{
	}
	inline virtual ~ChangeableScene()
	{
	}

	inline void sendHasChanged()
	{
		glRenderer->reloadSceneAttribs();
	}

	virtual const point  *addPoint( QIODevice *ioDeviceIn, bool bytesMirrored) = 0;
	virtual const line   *addLine(  QIODevice *ioDeviceIn, bool bytesMirrored) = 0;
	virtual const tria   *addTria(  QIODevice *ioDeviceIn, bool bytesMirrored) = 0;
	virtual const quadri *addQuadri(QIODevice *ioDeviceIn, bool bytesMirrored) = 0;
	virtual const sphere *addSphere(QIODevice *ioDeviceIn, bool bytesMirrored) = 0;
	virtual const sphoid *addSphoid(QIODevice *ioDeviceIn, bool bytesMirrored) = 0;
	virtual const text  *addText( QIODevice *ioDeviceIn, bool bytesMirrored)  = 0;
	
	virtual void clear() = 0;
};

#endif
