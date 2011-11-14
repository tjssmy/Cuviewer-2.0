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

/* draws the quadri with GL commands */

#include <qgl.h>

#include "../include/cuvtags.h"
#include "sceneentity.h"
#include "../include/cuvvector.h"

void SceneEntity::drawQuadri(quadri *quadriPointer, cuv_float lastColor[4], cuv_float lastNormal[3])
{
	cuv_float temp3Vect1[3], temp3Vect2[3], temp3Vect3[3];
	cuv_float temp4Vect1[4], temp4Vect2[4], temp4Vect3[4], temp4Vect4[4];
	
	if (quadriPointer->flags & SNORMALS) // multiple normals calculated
	{
		if (quadriPointer->flags & SMULTICOLOR) // multiple normals calculated, multi-colour
		{
			if (quadriPointer->flags & STRANSPARENT) // multiple normals calculated, multi-colour, transparent
			{
				vcopy4(temp4Vect1, quadriPointer->color);
				vcopy4(temp4Vect2, (quadriPointer->color+4));
				vcopy4(temp4Vect3, (quadriPointer->color+8));
				vcopy4(temp4Vect4, (quadriPointer->color+12));
			}
			else // multiple normals calculated, multi-colour, opaque
			{
				vcopy3to4(temp4Vect1, quadriPointer->color);
				vcopy3to4(temp4Vect2, (quadriPointer->color+3));
				vcopy3to4(temp4Vect3, (quadriPointer->color+6));
				vcopy3to4(temp4Vect4, (quadriPointer->color+9));
			}

			// try to optimize colours; normals are not targetted for redundancy optimization
			if ( vequal4(lastColor, temp4Vect1) )
			{
				normOpt(quadriPointer->norm, lastNormal);
				glVertex3fv(quadriPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				normOpt((quadriPointer->norm+3), lastNormal);
				glVertex3fv(quadriPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				normOpt((quadriPointer->norm+6), lastNormal);
				glVertex3fv(quadriPointer->vect3);

				colorOpt(temp4Vect4, lastColor);
				normOpt((quadriPointer->norm+9), lastNormal);
				glVertex3fv(quadriPointer->vect4);
			}
			else if ( vequal4(lastColor, temp4Vect2) )
			{
				normOpt((quadriPointer->norm+3), lastNormal);
				glVertex3fv(quadriPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				normOpt((quadriPointer->norm+6), lastNormal);
				glVertex3fv(quadriPointer->vect3);

				colorOpt(temp4Vect4, lastColor);
				normOpt((quadriPointer->norm+9), lastNormal);
				glVertex3fv(quadriPointer->vect4);

				colorOpt(temp4Vect1, lastColor);
				normOpt(quadriPointer->norm, lastNormal);
				glVertex3fv(quadriPointer->vect1);
			}
			else if ( vequal4(lastColor, temp4Vect3) )
			{
				normOpt((quadriPointer->norm+6), lastNormal);
				glVertex3fv(quadriPointer->vect3);

				colorOpt(temp4Vect4, lastColor);
				normOpt((quadriPointer->norm+9), lastNormal);
				glVertex3fv(quadriPointer->vect4);

				colorOpt(temp4Vect1, lastColor);
				normOpt(quadriPointer->norm, lastNormal);
				glVertex3fv(quadriPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				normOpt((quadriPointer->norm+3), lastNormal);
				glVertex3fv(quadriPointer->vect2);
			}
			else if ( vequal4(lastColor, temp4Vect4) )
			{
				normOpt((quadriPointer->norm+9), lastNormal);
				glVertex3fv(quadriPointer->vect4);

				colorOpt(temp4Vect1, lastColor);
				normOpt(quadriPointer->norm, lastNormal);
				glVertex3fv(quadriPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				normOpt((quadriPointer->norm+3), lastNormal);
				glVertex3fv(quadriPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				normOpt((quadriPointer->norm+6), lastNormal);
				glVertex3fv(quadriPointer->vect3);
			}
			else
			{
				colorOpt(temp4Vect1, lastColor);
				normOpt(quadriPointer->norm, lastNormal);
				glVertex3fv(quadriPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				normOpt((quadriPointer->norm+3), lastNormal);
				glVertex3fv(quadriPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				normOpt((quadriPointer->norm+6), lastNormal);
				glVertex3fv(quadriPointer->vect3);

				colorOpt(temp4Vect4, lastColor);
				normOpt((quadriPointer->norm+9), lastNormal);
				glVertex3fv(quadriPointer->vect4);
			}
		}
		else // multiple normals calculated, single-colour
		{
			if (quadriPointer->flags & STRANSPARENT) // multiple normals calculated, single-colour, transparent
			{
				colorOpt(quadriPointer->color, lastColor);
			}
			else // multiple normals calculated, single-colour, opaque
			{
				vcopy3to4(temp4Vect1, quadriPointer->color);
				colorOpt(temp4Vect1, lastColor);
			}

			// try to optimize normals
			if ( vequal3(quadriPointer->norm, lastNormal) )
			{
				glVertex3fv(quadriPointer->vect1);

				normOpt((quadriPointer->norm+3), lastNormal);
				glVertex3fv(quadriPointer->vect2);

				normOpt((quadriPointer->norm+6), lastNormal);
				glVertex3fv(quadriPointer->vect3);

				normOpt((quadriPointer->norm+9), lastNormal);
				glVertex3fv(quadriPointer->vect4);
			}
			else if ( vequal3((quadriPointer->norm+3), lastNormal) )
			{
				glVertex3fv(quadriPointer->vect2);

				normOpt((quadriPointer->norm+6), lastNormal);
				glVertex3fv(quadriPointer->vect3);

				normOpt((quadriPointer->norm+9), lastNormal);
				glVertex3fv(quadriPointer->vect4);

				normOpt(quadriPointer->norm, lastNormal);
				glVertex3fv(quadriPointer->vect1);
			}
			else if ( vequal3((quadriPointer->norm+6), lastNormal) )
			{
				glVertex3fv(quadriPointer->vect3);

				normOpt((quadriPointer->norm+9), lastNormal);
				glVertex3fv(quadriPointer->vect4);

				normOpt(quadriPointer->norm, lastNormal);
				glVertex3fv(quadriPointer->vect1);

				normOpt((quadriPointer->norm+3), lastNormal);
				glVertex3fv(quadriPointer->vect2);
			}
			else if ( vequal3((quadriPointer->norm+9), lastNormal) )
			{
				glVertex3fv(quadriPointer->vect4);

				normOpt(quadriPointer->norm, lastNormal);
				glVertex3fv(quadriPointer->vect1);

				normOpt((quadriPointer->norm+3), lastNormal);
				glVertex3fv(quadriPointer->vect2);

				normOpt((quadriPointer->norm+6), lastNormal);
				glVertex3fv(quadriPointer->vect3);
			}
			else // no optimizations possible
			{
				normOpt(quadriPointer->norm, lastNormal);
				glVertex3fv(quadriPointer->vect1);

				normOpt((quadriPointer->norm+3), lastNormal);
				glVertex3fv(quadriPointer->vect2);

				normOpt((quadriPointer->norm+6), lastNormal);
				glVertex3fv(quadriPointer->vect3);

				normOpt((quadriPointer->norm+9), lastNormal);
				glVertex3fv(quadriPointer->vect4);
			}
		}
	}
	else // single normal uncalculated
	{
		vsub(temp3Vect1, quadriPointer->vect1, quadriPointer->vect2);
		vsub(temp3Vect2, quadriPointer->vect2, quadriPointer->vect3);
		
		if ( vcross(temp3Vect3, temp3Vect1, temp3Vect2) )
		{
			vnormalize(temp3Vect3);
			normOpt(temp3Vect3, lastNormal);
		}
		else
		{
			return; // something is screwy with this quad
		}
			
		if (quadriPointer->flags & SMULTICOLOR) // single normal uncalculated, multi-colour 
		{
			if (quadriPointer->flags & STRANSPARENT) // single normal uncalculated, multi-colour, transparent
			{
				vcopy4(temp4Vect1, quadriPointer->color);
				vcopy4(temp4Vect2, (quadriPointer->color+4));
				vcopy4(temp4Vect3, (quadriPointer->color+8));
				vcopy4(temp4Vect4, (quadriPointer->color+12));
			}
			else // single normal uncalculated, multi-colour, opaque
			{
				vcopy3to4(temp4Vect1, quadriPointer->color);
				vcopy3to4(temp4Vect2, (quadriPointer->color+3));
				vcopy3to4(temp4Vect3, (quadriPointer->color+6));
				vcopy3to4(temp4Vect4, (quadriPointer->color+9));
			}

			// try to optimize colours
			if ( vequal4(lastColor, temp4Vect1) )
			{
				glVertex3fv(quadriPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				glVertex3fv(quadriPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				glVertex3fv(quadriPointer->vect3);
				
				colorOpt(temp4Vect4, lastColor);
				glVertex3fv(quadriPointer->vect4);
			}
			else if ( vequal4(lastColor, temp4Vect2) )
			{
				glVertex3fv(quadriPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				glVertex3fv(quadriPointer->vect3);
				
				colorOpt(temp4Vect4, lastColor);
				glVertex3fv(quadriPointer->vect4);

				colorOpt(temp4Vect1, lastColor);
				glVertex3fv(quadriPointer->vect1);
			}
			else if ( vequal4(lastColor, temp4Vect3) )
			{
				glVertex3fv(quadriPointer->vect3);
				
				colorOpt(temp4Vect4, lastColor);
				glVertex3fv(quadriPointer->vect4);

				colorOpt(temp4Vect1, lastColor);
				glVertex3fv(quadriPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				glVertex3fv(quadriPointer->vect2);
			}
			else if ( vequal4(lastColor, temp4Vect4) )
			{
				glVertex3fv(quadriPointer->vect4);

				colorOpt(temp4Vect1, lastColor);
				glVertex3fv(quadriPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				glVertex3fv(quadriPointer->vect2);
				
				colorOpt(temp4Vect3, lastColor);
				glVertex3fv(quadriPointer->vect3);
			}
			else
			{
				colorOpt(temp4Vect1, lastColor);
				glVertex3fv(quadriPointer->vect1);

				colorOpt(temp4Vect2, lastColor);
				glVertex3fv(quadriPointer->vect2);

				colorOpt(temp4Vect3, lastColor);
				glVertex3fv(quadriPointer->vect3);
				
				colorOpt(temp4Vect4, lastColor);
				glVertex3fv(quadriPointer->vect4);
			}
		}
		else // single normal uncalculated, single-colour
		{
			if (quadriPointer->flags & STRANSPARENT) // single normal uncalculated, single-colour, transparent
			{
				colorOpt(quadriPointer->color, lastColor);
			}
			else // single normal uncalculated, single-colour, opaque
			{
				vcopy3to4(temp4Vect1, quadriPointer->color);
				colorOpt(temp4Vect1, lastColor);
			}

			glVertex3fv(quadriPointer->vect1);
			glVertex3fv(quadriPointer->vect2);
			glVertex3fv(quadriPointer->vect3);
			glVertex3fv(quadriPointer->vect4);
		}
	}
	return;
}

void SceneEntity::drawQuadriNullNorm(quadri *quadriPointer, cuv_float lastColor[4], cuv_float lastNormal[3])
{
	const cuv_float temp3Vect[3] = {0.0, 0.0, 0.0};
	cuv_float temp4Vect1[4], temp4Vect2[4], temp4Vect3[4], temp4Vect4[4];
	
	normOpt(temp3Vect, lastNormal);
	
	if (quadriPointer->flags & SMULTICOLOR) // multi-colour 
	{
		if (quadriPointer->flags & STRANSPARENT) // multi-colour, transparent
		{
			vcopy4(temp4Vect1, quadriPointer->color);
			vcopy4(temp4Vect2, (quadriPointer->color+4));
			vcopy4(temp4Vect3, (quadriPointer->color+8));
			vcopy4(temp4Vect4, (quadriPointer->color+12));
		}
		else // multi-colour, opaque
		{
			vcopy3to4(temp4Vect1, quadriPointer->color);
			vcopy3to4(temp4Vect2, (quadriPointer->color+3));
			vcopy3to4(temp4Vect3, (quadriPointer->color+6));
			vcopy3to4(temp4Vect4, (quadriPointer->color+9));
		}
		
		// try to optimize colours
		if ( vequal4(lastColor, temp4Vect1) )
		{
			glVertex3fv(quadriPointer->vect1);
			
			colorOpt(temp4Vect2, lastColor);
			glVertex3fv(quadriPointer->vect2);
			
			colorOpt(temp4Vect3, lastColor);
			glVertex3fv(quadriPointer->vect3);
			
			colorOpt(temp4Vect4, lastColor);
			glVertex3fv(quadriPointer->vect4);
		}
		else if ( vequal4(lastColor, temp4Vect2) )
		{
			glVertex3fv(quadriPointer->vect2);
			
			colorOpt(temp4Vect3, lastColor);
			glVertex3fv(quadriPointer->vect3);
			
			colorOpt(temp4Vect4, lastColor);
			glVertex3fv(quadriPointer->vect4);
			
			colorOpt(temp4Vect1, lastColor);
			glVertex3fv(quadriPointer->vect1);
		}
		else if ( vequal4(lastColor, temp4Vect3) )
		{
			glVertex3fv(quadriPointer->vect3);
			
			colorOpt(temp4Vect4, lastColor);
			glVertex3fv(quadriPointer->vect4);
			
			colorOpt(temp4Vect1, lastColor);
			glVertex3fv(quadriPointer->vect1);
			
			colorOpt(temp4Vect2, lastColor);
			glVertex3fv(quadriPointer->vect2);
		}
		else if ( vequal4(lastColor, temp4Vect4) )
		{
			glVertex3fv(quadriPointer->vect4);
			
			colorOpt(temp4Vect1, lastColor);
			glVertex3fv(quadriPointer->vect1);
			
			colorOpt(temp4Vect2, lastColor);
			glVertex3fv(quadriPointer->vect2);
			
			colorOpt(temp4Vect3, lastColor);
			glVertex3fv(quadriPointer->vect3);
		}
		else
		{
			colorOpt(temp4Vect1, lastColor);
			glVertex3fv(quadriPointer->vect1);
			
			colorOpt(temp4Vect2, lastColor);
			glVertex3fv(quadriPointer->vect2);
			
			colorOpt(temp4Vect3, lastColor);
			glVertex3fv(quadriPointer->vect3);
			
			colorOpt(temp4Vect4, lastColor);
			glVertex3fv(quadriPointer->vect4);
		}
	}
	else // single-colour
	{
		if (quadriPointer->flags & STRANSPARENT) // single-colour, transparent
		{
			colorOpt(quadriPointer->color, lastColor);
		}
		else // single-colour, opaque
		{
			vcopy3to4(temp4Vect1, quadriPointer->color);
			colorOpt(temp4Vect1, lastColor);
		}
		
		glVertex3fv(quadriPointer->vect1);
		glVertex3fv(quadriPointer->vect2);
		glVertex3fv(quadriPointer->vect3);
		glVertex3fv(quadriPointer->vect4);
	}
	
	return;
}

void SceneEntity::drawQuadriNullNormNoColor(quadri *quadriPointer, cuv_float lastNormal[3])
{
	const cuv_float temp3Vect[3] = {0.0, 0.0, 0.0};
	
	normOpt(temp3Vect, lastNormal);
		
	glVertex3fv(quadriPointer->vect1);
	glVertex3fv(quadriPointer->vect2);
	glVertex3fv(quadriPointer->vect3);
	glVertex3fv(quadriPointer->vect4);
	
	return;
}
