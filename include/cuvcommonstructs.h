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

#ifndef CUVCOMMONSTRUCT_H
#define CUVCOMMONSTRUCT_H

#include <climits>
#include <qstring.h>
#ifdef WIN32
#  include <qgl.h>
#else
#  include <OpenGL/gl.h>
#endif

#include "cuvtags.h"
#include "lessalloc.h"

#define BIN_WINDOW_SIZE UCHAR_MAX+1
#define MAX_SCENES 1024
#define MAX_LIGHTSOURCES 8

struct point
{
	cuv_tag flags;
	cuv_float vect1[3], *color;
};

struct line
{
	cuv_tag flags;
	cuv_float vect1[3], vect2[3], *color;
};

struct tria
{
	cuv_tag flags;
	cuv_float vect1[3], vect2[3], vect3[3], *color, *norm;
};

struct quadri
{
	cuv_tag flags;
	cuv_float vect1[3], vect2[3], vect3[3], vect4[3], *color, *norm;
};

struct sphere
{
	cuv_tag flags;
	cuv_float vect1[3], radius, *color; // normals are not applicable
};

struct sphoid
{
	cuv_tag flags;
	cuv_float vect1[3], size[3], rotate[4], *color; // normals are not applicable
};

struct text
{
	cuv_tag flags;
	cuv_float vect1[3], *color;
        QString maintext;
};

struct extremities
{
	cuv_float xmin, xmax, ymin, ymax, zmin, zmax;
};

struct proj_info
{
	GLdouble left, right, bottom, top, nearp, farp;
};

struct bin_window_palette
{
	cuv_uint count;
	QString name;
	GLubyte *fromPaletteStart;
	GLubyte *fromPaletteEnd;
	GLubyte *toPalette;
};

struct view_state
{
	bool twoSidedEnabled;
	bool smoothShadedEnabled;
	bool transparencyEnabled;
	bool antialiasEnabled;
	cuv_ubyte sphereTess;
	
	bool outlineEnabled;
	bool wireframeEnabled;
	bool opaqueWireframeEnabled;
	GLfloat lineWidth;
	GLfloat outlineColor[3];
	
	bool lightEnabled;
	bool lightingEnabled;
	GLfloat ambient;
	GLfloat diffuse;
	GLfloat background;
	
	bool perspectiveEnabled;
	GLfloat fov;
	GLfloat cameraPos[3];
	GLfloat cameraAim[3];
	GLfloat cameraUp[3];
	GLdouble clipPerspNear;
	GLdouble clipOrthoNear;
	
	bool gammaEnabled;
	float gammaExponent;
	GLubyte gammaLookup[256];
  float specular;

  bool fogEnabled;
  GLfloat fogLevel;

	bool binWindowEnabled;
	cuv_ubyte binWindowIndex;
	cuv_uint binWindowCount[BIN_WINDOW_SIZE];
	QString *binWindowName[BIN_WINDOW_SIZE];
	GLubyte *binWindowPalette[BIN_WINDOW_SIZE];
  
  int lightSources;
  bool fixedLightSource[MAX_LIGHTSOURCES];
	bool lightEditing[MAX_LIGHTSOURCES];
	float lightXTranslate[MAX_LIGHTSOURCES],
        lightYTranslate[MAX_LIGHTSOURCES],
        lightZTranslate[MAX_LIGHTSOURCES];
  float lightColor[MAX_LIGHTSOURCES][3];
  bool showLightPosition;
};

struct entity_data
{
	LessAlloc<point>  lstPoint;
	LessAlloc<line>   lstLine;
	LessAlloc<tria>   lstTria;
	LessAlloc<quadri> lstQuadri;
	LessAlloc<sphere> lstSphere;
	LessAlloc<sphoid> lstSphoid;
	
	LessAlloc<cuv_float[1][3]> lstColSingOpaq;
	LessAlloc<cuv_float[1][4]> lstColSingTrans;
	LessAlloc<cuv_float[2][3]> lstColDoubOpaq;
	LessAlloc<cuv_float[2][4]> lstColDoubTrans;
	LessAlloc<cuv_float[3][3]> lstColTriplOpaq;
	LessAlloc<cuv_float[3][4]> lstColTriplTrans;
	LessAlloc<cuv_float[4][3]> lstColQuadOpaq;
	LessAlloc<cuv_float[4][4]> lstColQuadTrans;
	
	LessAlloc<cuv_float[3][3]> lstNormTripl;
	LessAlloc<cuv_float[4][3]> lstNormQuad;
};

struct glrenderer_options
{
	bool useThreeButtons;
	bool useCachedImage;
	bool drawBoxWhileMoving;
	bool drawBoxAlways;
  bool drawAxis;
	bool drawAxisOrigin;
};

#endif
