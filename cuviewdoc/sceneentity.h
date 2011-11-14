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

#ifndef SCENEENTITY_H
#define SCENEENTITY_H

#include <qgl.h>
//#ifdef Q_OS_MAC
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>
//#endif

#include <qstring.h>
#include "../include/lessalloc.h"
#include "../include/cuvcommonstructs.h"
#include "../include/cuvvector.h"
#include "drawablescene.h"

#define FORCE_STATE_REFRESH 1000 // to overcome the limitations of the Solaris systems we use
class QIODevice;
class GLRenderer;

// alias template classes for easier reading later on
typedef LessAlloc<point>  Lessa_Point;
typedef LessAlloc<line>   Lessa_Line;
typedef LessAlloc<tria>   Lessa_Tria;
typedef LessAlloc<quadri> Lessa_Quadri;
typedef LessAlloc<sphere> Lessa_Sphere;
typedef LessAlloc<sphoid> Lessa_Sphoid;
typedef LessAlloc<text>   Lessa_Text;
	
typedef LessAlloc<cuv_float[1][3]> Lessa_Cfloat_1x3;
typedef LessAlloc<cuv_float[1][4]> Lessa_Cfloat_1x4;
typedef LessAlloc<cuv_float[2][3]> Lessa_Cfloat_2x3;
typedef LessAlloc<cuv_float[2][4]> Lessa_Cfloat_2x4;
typedef LessAlloc<cuv_float[3][3]> Lessa_Cfloat_3x3;
typedef LessAlloc<cuv_float[3][4]> Lessa_Cfloat_3x4;
typedef LessAlloc<cuv_float[4][3]> Lessa_Cfloat_4x3;
typedef LessAlloc<cuv_float[4][4]> Lessa_Cfloat_4x4;


class SceneEntity: public DrawableScene
{
  protected:
	Lessa_Point  *lstPoint;
	Lessa_Line   *lstLine;
	Lessa_Tria   *lstTria;
	Lessa_Quadri *lstQuadri;
	Lessa_Sphere *lstSphere;
	Lessa_Sphoid *lstSphoid;
	Lessa_Text   *lstText;
	
	Lessa_Cfloat_1x3 *lstColSingOpaq;
	Lessa_Cfloat_1x4 *lstColSingTrans;
	Lessa_Cfloat_2x3 *lstColDoubOpaq;
	Lessa_Cfloat_2x4 *lstColDoubTrans;
	Lessa_Cfloat_3x3 *lstColTriplOpaq;
	Lessa_Cfloat_3x4 *lstColTriplTrans;
	Lessa_Cfloat_4x3 *lstColQuadOpaq;
	Lessa_Cfloat_4x4 *lstColQuadTrans;
	
	Lessa_Cfloat_3x3 *lstNormTripl;
	Lessa_Cfloat_4x3 *lstNormQuad;
	
	bool useDisplayLists;
	bool editEntity;
	cuv_float translatef[3];
	cuv_float scalef[3];
	cuv_float rotatef[3];
	
	GLuint opaqSolidList;
	GLuint opaqWireList;
	GLuint outlineList;
	GLuint transSolidList;
	GLuint transWireList;
	
	bool normOpt__forceNormal;
	bool colorOpt__forceColor;

  public:
	SceneEntity(GLRenderer *displayListWidget);
	virtual ~SceneEntity();
	
	QString extremitiesInfo();	
	bool hasData();
	QString sceneInfo();
	int scenePrimitives();
	void setEditEntity(bool edit);
	bool getEditEntity();
	void editExtremeties(float,float,float,float,float,float);
	void getEntityUnit(float*,float*,float*);
	float largestExtremUnit();

	void getTranslatef(GLfloat[3]);
	void getScalef(GLfloat[3]);
	void getRotatef(GLfloat[3]);
	Lessa_Text * getLstText();

	void translate(cuv_float x,cuv_float y, cuv_float z);
	void translateUnits(int,int,int);
	void scale(cuv_float scaleValue);
	void scale(cuv_float x,cuv_float y, cuv_float z);
	void rotate(const cuv_float[4]);
	void rotate(cuv_float x,cuv_float y, cuv_float z);
	void computeRotationExtrems(cuv_float x,cuv_float y,cuv_float z);
	void transform();
	const float * getTranslation();
	void copyScene(SceneEntity*);
	void setBooleans(bool,bool,bool,bool,bool,bool);
	void setDisplayLists(GLuint,GLuint,GLuint,GLuint,GLuint);

	virtual void drawOpaqSolid();
	virtual void drawOpaqWire();
	virtual void drawOutline();
	virtual void drawTransSolid();
	virtual void drawTransWire();
	virtual void useDL(bool);
	
	virtual const point  *addPoint( cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const line   *addLine(  cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const tria   *addTria(  cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const quadri *addQuadri(cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const sphere *addSphere(cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const sphoid *addSphoid(cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored);
	virtual const text   *addText(  cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored);

	virtual void clear();
	void clearDispListExtrem();
	
  protected:
	
	void drawPointNullNorm(point *pointPointer, cuv_float lastColor[4], cuv_float lastNormal[3]);
	void drawPointNullNormNoColor(point *pointPointer, cuv_float lastNormal[3]);
	void drawLineNullNorm(line *linePointer, cuv_float lastColor[4], cuv_float lastNormal[3]);
	void drawLineNullNormNoColor(line *linePointer, cuv_float lastNormal[3]);
	void drawTria(tria *triaPointer, cuv_float lastColor[4], cuv_float lastNormal[3]);
	void drawTriaNullNorm(tria *triaPointer, cuv_float lastColor[4], cuv_float lastNormal[3]);
	void drawTriaNullNormNoColor(tria *triaPointer, cuv_float lastNormal[3]);
	void drawQuadri(quadri *quadriPointer, cuv_float lastColor[4], cuv_float lastNormal[3]);
	void drawQuadriNullNorm(quadri *quadriPointer, cuv_float lastColor[4], cuv_float lastNormal[3]);
	void drawQuadriNullNormNoColor(quadri *quadriPointer, cuv_float lastNormal[3]);
	void drawSphere(sphere *spherePointer, cuv_float lastColor[4]);
	void drawSphereNullNorm(sphere *spherePointer, cuv_float lastColor[4], cuv_float lastNormal[3]);
	void drawSphereNullNormNoColor(sphere *spherePointer, cuv_float lastNormal[3]);
	void drawSphoid(sphoid *sphoidPointer, cuv_float lastColor[4]);
	void drawSphoidNullNorm(sphoid *sphoidPointer, cuv_float lastColor[4], cuv_float lastNormal[3]);
	void drawSphoidNullNormNoColor(sphoid *sphoidPointer, cuv_float lastNormal[3]);
	void drawTextNullNorm(text *textPointer, cuv_float lastColor[4], cuv_float lastNormal[3]);
	void drawTextNullNormNoColor(text *textPointer, cuv_float lastNormal[3]);

	inline void normOpt(const cuv_float norm[3], cuv_float lastNormal[3], const bool clear = false)
	{
		// I might as well repeat normals too for the same reasons as colors below.
		static int forceNormalForBrokenOpenGL = 0;
		
		if (clear)
		{
			normOpt__forceNormal = true;
			lastNormal[0] = lastNormal[1] = lastNormal[2] = 0.0;
		}
		else
		{
			if ( normOpt__forceNormal || !vequal3(norm, lastNormal) || !forceNormalForBrokenOpenGL )
			{
				vcopy3(lastNormal, norm);
				glNormal3fv(lastNormal);
				normOpt__forceNormal = false;
				forceNormalForBrokenOpenGL = FORCE_STATE_REFRESH;
			}
			else // adjust the shapes with the same normal counter down
			{
				forceNormalForBrokenOpenGL--;
			}
		}
	
		return;
	}

	inline void colorOpt(const cuv_float color[4], cuv_float lastColor[4], const bool clear = false)
	{
		// We have Solarises that need to have the color told to them every once in a while or they will die.
		static int forceColorForBrokenOpenGL = 0;
		
		if (clear)
		{
			colorOpt__forceColor = true;
			lastColor[0] = lastColor[1] = lastColor[2] = lastColor[3] = 0.0;
		}
		else
		{
			if ( colorOpt__forceColor || !vequal4(color, lastColor) || !forceColorForBrokenOpenGL )
			{
				vcopy4(lastColor, color);
				glColor4fv(lastColor);
				colorOpt__forceColor = false;
				forceColorForBrokenOpenGL = FORCE_STATE_REFRESH;
			}
			else // adjust the shapes with the same color counter down
			{
				forceColorForBrokenOpenGL--;
			}
		}
	
		return;
	}
};

#endif
