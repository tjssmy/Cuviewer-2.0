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

/* draws the spheroid with GL commands */

#include <qgl.h>

#include "../include/cuvtags.h"
#include "sceneentity.h"
#include "../include/cuvvector.h"

void SceneEntity::drawSphoid(sphoid *sphoidPointer, cuv_float lastColor[4])
{
	cuv_float temp4Vect[4];

	if (sphoidPointer->flags & STRANSPARENT) // transparent
	{
		colorOpt(sphoidPointer->color, lastColor);
	}
	else // opaque
	{
		vcopy3to4(temp4Vect, sphoidPointer->color);
		colorOpt(temp4Vect, lastColor);
	}
	
	glPushMatrix();

	glTranslatef(sphoidPointer->vect1[0], sphoidPointer->vect1[1], sphoidPointer->vect1[2]);
	if ( sphoidPointer->rotate[0] && (sphoidPointer->rotate[1] || sphoidPointer->rotate[2] || sphoidPointer->rotate[3]) )
		glRotatef(sphoidPointer->rotate[0], sphoidPointer->rotate[1], sphoidPointer->rotate[2], sphoidPointer->rotate[3]);
	glEnable(GL_NORMALIZE);
	glScalef(sphoidPointer->size[0], sphoidPointer->size[1], sphoidPointer->size[2]);
	glCallList(glRenderer->getSolidSphereList());
	glDisable(GL_NORMALIZE);

	glPopMatrix();

	return;
}

void SceneEntity::drawSphoidNullNorm(sphoid *sphoidPointer, cuv_float lastColor[4], cuv_float lastNormal[3])
{
	const cuv_float temp3Vect[3] = {0.0, 0.0, 0.0};
	cuv_float temp4Vect[4];

	normOpt(temp3Vect, lastNormal);

	if (sphoidPointer->flags & STRANSPARENT) // transparent
	{
		colorOpt(sphoidPointer->color, lastColor);
	}
	else // opaque
	{
		vcopy3to4(temp4Vect, sphoidPointer->color);
		colorOpt(temp4Vect, lastColor);
	}
	
	glPushMatrix();

	glTranslatef(sphoidPointer->vect1[0], sphoidPointer->vect1[1], sphoidPointer->vect1[2]);
	if ( sphoidPointer->rotate[0] && (sphoidPointer->rotate[1] || sphoidPointer->rotate[2] || sphoidPointer->rotate[3]) )
		glRotatef(sphoidPointer->rotate[0], sphoidPointer->rotate[1], sphoidPointer->rotate[2], sphoidPointer->rotate[3]);
	glScalef(sphoidPointer->size[0], sphoidPointer->size[1], sphoidPointer->size[2]);
	glCallList(glRenderer->getWireSphereList());

	glPopMatrix();

	return;
}

void SceneEntity::drawSphoidNullNormNoColor(sphoid *sphoidPointer, cuv_float lastNormal[3])
{
	const cuv_float temp3Vect[3] = {0.0, 0.0, 0.0};

	normOpt(temp3Vect, lastNormal);

	glPushMatrix();

	glTranslatef(sphoidPointer->vect1[0], sphoidPointer->vect1[1], sphoidPointer->vect1[2]);
	if ( sphoidPointer->rotate[0] && (sphoidPointer->rotate[1] || sphoidPointer->rotate[2] || sphoidPointer->rotate[3]) )
		glRotatef(sphoidPointer->rotate[0], sphoidPointer->rotate[1], sphoidPointer->rotate[2], sphoidPointer->rotate[3]);
	glScalef(sphoidPointer->size[0], sphoidPointer->size[1], sphoidPointer->size[2]);
	glCallList(glRenderer->getWireSphereList());

	glPopMatrix();
	
	return;
}
