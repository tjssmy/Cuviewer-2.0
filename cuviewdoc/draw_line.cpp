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

/* draws the line with GL commands */

#include <qgl.h>

#include "../include/cuvtags.h"
#include "sceneentity.h"
#include "../include/cuvvector.h"

void SceneEntity::drawLineNullNorm(line *linePointer, cuv_float lastColor[4], cuv_float lastNormal[3])
{
	const cuv_float temp3Vect[3] = {0.0, 0.0, 0.0};
	cuv_float temp4Vect1[4], temp4Vect2[4];

	normOpt(temp3Vect, lastNormal);

	if (linePointer->flags & SMULTICOLOR) // multi-colour 
	{
		if (linePointer->flags & STRANSPARENT) // multi-colour, transparent
		{
			vcopy4(temp4Vect1, linePointer->color);
			vcopy4(temp4Vect2, (linePointer->color+4));
		}
		else // multi-colour, opaque
		{
			vcopy3to4(temp4Vect1, linePointer->color);
			vcopy3to4(temp4Vect2, (linePointer->color+3));
		}
		
		// try to optimize colours
		if ( vequal4(lastColor, temp4Vect1) )
		{
			glVertex3fv(linePointer->vect1);
			
			colorOpt(temp4Vect2, lastColor);
			glVertex3fv(linePointer->vect2);
		}
		else if ( vequal4(lastColor, temp4Vect2) )
		{
			glVertex3fv(linePointer->vect2);
			
			colorOpt(temp4Vect1, lastColor);
			glVertex3fv(linePointer->vect1);
		}
		else
		{
			colorOpt(temp4Vect1, lastColor);
			glVertex3fv(linePointer->vect1);
			
			colorOpt(temp4Vect2, lastColor);
			glVertex3fv(linePointer->vect2);
		}
	}
	else // single-colour
	{
		if (linePointer->flags & STRANSPARENT) // single-colour, transparent
		{
			colorOpt(linePointer->color, lastColor);
		}
		else // single-colour, opaque
		{
			vcopy3to4(temp4Vect1, linePointer->color);
			colorOpt(temp4Vect1, lastColor);
		}
		
		glVertex3fv(linePointer->vect1);
		glVertex3fv(linePointer->vect2);
	}
	
	return;
}

void SceneEntity::drawLineNullNormNoColor(line *linePointer, cuv_float lastNormal[3])
{
	const cuv_float temp3Vect[3] = {0.0, 0.0, 0.0};

	normOpt(temp3Vect, lastNormal);

	glVertex3fv(linePointer->vect1);
	glVertex3fv(linePointer->vect2);
	
	return;
}
