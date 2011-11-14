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

/* draws the triangle with GL commands */

#include <qgl.h>

#include "../include/cuvtags.h"
#include "sceneentity.h"
#include "../include/cuvvector.h"

void SceneEntity::drawTria(tria *triaPointer, cuv_float lastColor[4], cuv_float lastNormal[3])
{
	cuv_float temp3Vect1[3], temp3Vect2[3], temp3Vect3[3];
	cuv_float temp4Vect1[4], temp4Vect2[4], temp4Vect3[4];
	
	if (triaPointer->flags & SNORMALS) // multiple normals calculated
	{
		if (triaPointer->flags & SMULTICOLOR) // multiple normals calculated, multi-colour
		{
			if (triaPointer->flags & STRANSPARENT) // multiple normals calculated, multi-colour, transparent
			{
				vcopy4(temp4Vect1, triaPointer->color);
				vcopy4(temp4Vect2, (triaPointer->color+4));
				vcopy4(temp4Vect3, (triaPointer->color+8));
			}
			else // multiple normals calculated, multi-colour, opaque
			{
				vcopy3to4(temp4Vect1, triaPointer->color);
				vcopy3to4(temp4Vect2, (triaPointer->color+3));
				vcopy3to4(temp4Vect3, (triaPointer->color+6));
			}

			// try to optimize colours; normals are not targetted for redundancy optimization
			if ( vequal4(lastColor, temp4Vect1) )
			{
				normOpt(triaPointer->norm, lastNormal);
				glVertex3fv(triaPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				normOpt((triaPointer->norm+3), lastNormal);
				glVertex3fv(triaPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				normOpt((triaPointer->norm+6), lastNormal);
				glVertex3fv(triaPointer->vect3);
			}
			else if ( vequal4(lastColor, temp4Vect2) )
			{
				normOpt((triaPointer->norm+3), lastNormal);
				glVertex3fv(triaPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				normOpt((triaPointer->norm+6), lastNormal);
				glVertex3fv(triaPointer->vect3);

				colorOpt(temp4Vect1, lastColor);
				normOpt(triaPointer->norm, lastNormal);
				glVertex3fv(triaPointer->vect1);
			}
			else if ( vequal4(lastColor, temp4Vect3) )
			{
				normOpt((triaPointer->norm+6), lastNormal);
				glVertex3fv(triaPointer->vect3);

				colorOpt(temp4Vect1, lastColor);
				normOpt(triaPointer->norm, lastNormal);
				glVertex3fv(triaPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				normOpt((triaPointer->norm+3), lastNormal);
				glVertex3fv(triaPointer->vect2);
			}
			else
			{
				colorOpt(temp4Vect1, lastColor);
				normOpt(triaPointer->norm, lastNormal);
				glVertex3fv(triaPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				normOpt((triaPointer->norm+3), lastNormal);
				glVertex3fv(triaPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				normOpt((triaPointer->norm+6), lastNormal);
				glVertex3fv(triaPointer->vect3);
			}
		}
		else // multiple normals calculated, single-colour
		{
			if (triaPointer->flags & STRANSPARENT) // multiple normals calculated, single-colour, transparent
			{
				colorOpt(triaPointer->color, lastColor);
			}
			else // multiple normals calculated, single-colour, opaque
			{
				vcopy3to4(temp4Vect1, triaPointer->color);
				colorOpt(temp4Vect1, lastColor);
			}

			// try to optimize normals
			if ( vequal3(triaPointer->norm, lastNormal) )
			{
				glVertex3fv(triaPointer->vect1);

				normOpt((triaPointer->norm+3), lastNormal);
				glVertex3fv(triaPointer->vect2);

				normOpt((triaPointer->norm+6), lastNormal);
				glVertex3fv(triaPointer->vect3);
			}
			else if ( vequal3((triaPointer->norm+3), lastNormal) )
			{
				glVertex3fv(triaPointer->vect2);

				normOpt((triaPointer->norm+6), lastNormal);
				glVertex3fv(triaPointer->vect3);

				normOpt(triaPointer->norm, lastNormal);
				glVertex3fv(triaPointer->vect1);
			}
			else if ( vequal3((triaPointer->norm+6), lastNormal) )
			{
				glVertex3fv(triaPointer->vect3);

				normOpt(triaPointer->norm, lastNormal);
				glVertex3fv(triaPointer->vect1);

				normOpt((triaPointer->norm+3), lastNormal);
				glVertex3fv(triaPointer->vect2);
			}
			else // no optimizations possible
			{
				normOpt(triaPointer->norm, lastNormal);
				glVertex3fv(triaPointer->vect1);

				normOpt((triaPointer->norm+3), lastNormal);
				glVertex3fv(triaPointer->vect2);

				normOpt((triaPointer->norm+6), lastNormal);
				glVertex3fv(triaPointer->vect3);
			}
		}
	}
	else // single normal uncalculated
	{
		vsub(temp3Vect1, triaPointer->vect1, triaPointer->vect2);
		vsub(temp3Vect2, triaPointer->vect2, triaPointer->vect3);
		
		if ( vcross(temp3Vect3, temp3Vect1, temp3Vect2) )
		{
			vnormalize(temp3Vect3);
			normOpt(temp3Vect3, lastNormal);
		}
		else
		{
			return; // something is screwy with this triangle
		}
			
		if (triaPointer->flags & SMULTICOLOR) // single normal uncalculated, multi-colour 
		{
			if (triaPointer->flags & STRANSPARENT) // single normal uncalculated, multi-colour, transparent
			{
				vcopy4(temp4Vect1, triaPointer->color);
				vcopy4(temp4Vect2, (triaPointer->color+4));
				vcopy4(temp4Vect3, (triaPointer->color+8));
			}
			else // single normal uncalculated, multi-colour, opaque
			{
				vcopy3to4(temp4Vect1, triaPointer->color);
				vcopy3to4(temp4Vect2, (triaPointer->color+3));
				vcopy3to4(temp4Vect3, (triaPointer->color+6));
			}

			// try to optimize colours
			if ( vequal4(lastColor, temp4Vect1) )
			{
				glVertex3fv(triaPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				glVertex3fv(triaPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				glVertex3fv(triaPointer->vect3);
			}
			else if ( vequal4(lastColor, temp4Vect2) )
			{
				glVertex3fv(triaPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				glVertex3fv(triaPointer->vect3);

				colorOpt(temp4Vect1, lastColor);
				glVertex3fv(triaPointer->vect1);
			}
			else if ( vequal4(lastColor, temp4Vect3) )
			{
				glVertex3fv(triaPointer->vect3);

				colorOpt(temp4Vect1, lastColor);
				glVertex3fv(triaPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				glVertex3fv(triaPointer->vect2);
			}
			else
			{
				colorOpt(temp4Vect1, lastColor);
				glVertex3fv(triaPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				glVertex3fv(triaPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				glVertex3fv(triaPointer->vect3);
			}
		}
		else // single normal uncalculated, single-colour
		{
			if (triaPointer->flags & STRANSPARENT) // single normal uncalculated, single-colour, transparent
			{
				colorOpt(triaPointer->color, lastColor);
			}
			else // single normal uncalculated, single-colour, opaque
			{
				vcopy3to4(temp4Vect1, triaPointer->color);
				colorOpt(temp4Vect1, lastColor);
			}

			glVertex3fv(triaPointer->vect1);
			glVertex3fv(triaPointer->vect2);
			glVertex3fv(triaPointer->vect3);
		}
	}
	return;
}

void SceneEntity::drawTriaNullNorm(tria *triaPointer, cuv_float lastColor[4], cuv_float lastNormal[3])
{
	const cuv_float temp3Vect[3] = {0.0, 0.0, 0.0};
	cuv_float temp4Vect1[4], temp4Vect2[4], temp4Vect3[4];

	normOpt(temp3Vect, lastNormal);

	if (triaPointer->flags & SMULTICOLOR) // multi-colour 
	{
		if (triaPointer->flags & STRANSPARENT) // multi-colour, transparent
		{
			vcopy4(temp4Vect1, triaPointer->color);
			vcopy4(temp4Vect2, (triaPointer->color+4));
			vcopy4(temp4Vect3, (triaPointer->color+8));
		}
		else // multi-colour, opaque
		{
			vcopy3to4(temp4Vect1, triaPointer->color);
			vcopy3to4(temp4Vect2, (triaPointer->color+3));
			vcopy3to4(temp4Vect3, (triaPointer->color+6));
		}
		
		// try to optimize colours
		if ( vequal4(lastColor, temp4Vect1) )
		{
			glVertex3fv(triaPointer->vect1);
			
			colorOpt(temp4Vect2, lastColor);
			glVertex3fv(triaPointer->vect2);
			
			colorOpt(temp4Vect3, lastColor);
			glVertex3fv(triaPointer->vect3);
		}
		else if ( vequal4(lastColor, temp4Vect2) )
		{
			glVertex3fv(triaPointer->vect2);
			
			colorOpt(temp4Vect3, lastColor);
			glVertex3fv(triaPointer->vect3);
			
			colorOpt(temp4Vect1, lastColor);
			glVertex3fv(triaPointer->vect1);
		}
		else if ( vequal4(lastColor, temp4Vect3) )
		{
			glVertex3fv(triaPointer->vect3);
			
			colorOpt(temp4Vect1, lastColor);
			glVertex3fv(triaPointer->vect1);
			
			colorOpt(temp4Vect2, lastColor);
			glVertex3fv(triaPointer->vect2);
		}
		else
		{
			colorOpt(temp4Vect1, lastColor);
			glVertex3fv(triaPointer->vect1);
			
			colorOpt(temp4Vect2, lastColor);
			glVertex3fv(triaPointer->vect2);
			
			colorOpt(temp4Vect3, lastColor);
			glVertex3fv(triaPointer->vect3);
		}
	}
	else // single-colour
	{
		if (triaPointer->flags & STRANSPARENT) // single-colour, transparent
		{
			colorOpt(triaPointer->color, lastColor);
		}
		else // single-colour, opaque
		{
			vcopy3to4(temp4Vect1, triaPointer->color);
			colorOpt(temp4Vect1, lastColor);
		}
		
		glVertex3fv(triaPointer->vect1);
		glVertex3fv(triaPointer->vect2);
		glVertex3fv(triaPointer->vect3);
	}
	
	return;
}

void SceneEntity::drawTriaNullNormNoColor(tria *triaPointer, cuv_float lastNormal[3])
{
	const cuv_float temp3Vect[3] = {0.0, 0.0, 0.0};

	normOpt(temp3Vect, lastNormal);

	glVertex3fv(triaPointer->vect1);
	glVertex3fv(triaPointer->vect2);
	glVertex3fv(triaPointer->vect3);
	
	return;
}
