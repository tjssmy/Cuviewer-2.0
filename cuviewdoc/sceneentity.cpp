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

#include <qapplication.h>
#include <qstring.h>
#include <qiodevice.h>

#include "../include/lessalloc.h" //__lessa_uint64 = unsigned long long
#include "../include/cuvtags.h"
#include "../include/cuvcommonstructs.h"
#include "../include/cuvdatautils.h"
#include "../include/cuvvector.h"
#include "glrenderer.h"
#include "drawablescene.h"
#include "sceneentity.h"

#ifdef Q_WS_WIN
#  include <math.h>
#else
#  include <cmath>
#endif

SceneEntity::SceneEntity(GLRenderer *displayListWidget) : DrawableScene(displayListWidget)
{
  // set the pointers to NULL and only initialize pointers when used
  lstPoint = NULL;
  lstLine = NULL;
  lstTria = NULL;
  lstQuadri = NULL;
  lstSphere = NULL;
  lstSphoid = NULL;
  lstText = NULL;

  lstColSingOpaq = NULL;
  lstColSingTrans = NULL;
  lstColDoubOpaq = NULL;
  lstColDoubTrans = NULL;
  lstColTriplOpaq = NULL;
  lstColTriplTrans = NULL;
  lstColQuadOpaq = NULL;
  lstColQuadTrans = NULL;

  lstNormTripl = NULL;
  lstNormQuad = NULL;

  useDisplayLists = true;
  editEntity = false;
  translatef[0] = 0;
  translatef[1] = 0;
  translatef[2] = 0;
  scalef[0] = 1;
  scalef[1] = 1;
  scalef[2] = 1;
  rotatef[0] = 0;
  rotatef[1] = 0;
  rotatef[2] = 0;

  opaqSolidList = 0;
  opaqWireList = 0;
  outlineList = 0;
  transSolidList = 0;
  transWireList = 0;
}

SceneEntity::~SceneEntity()
{
  if (lstPoint)
    delete lstPoint;
  if (lstLine)
    delete lstLine;
  if (lstTria)
    delete lstTria;
  if (lstQuadri)
    delete lstQuadri;
  if (lstSphere)
    delete lstSphere;
  if (lstSphoid)
    delete lstSphoid;

  if (lstColSingOpaq)
    delete lstColSingOpaq;
  if (lstColSingTrans)
    delete lstColSingTrans;
  if (lstColDoubOpaq)
    delete lstColDoubOpaq;
  if (lstColDoubTrans)
    delete lstColDoubTrans;
  if (lstColTriplOpaq)
    delete lstColTriplOpaq;
  if (lstColTriplTrans)
    delete lstColTriplTrans;
  if (lstColQuadOpaq)
    delete lstColQuadOpaq;
  if (lstColQuadTrans)
    delete lstColQuadTrans;

  if (lstNormTripl)
    delete lstNormTripl;
  if (lstNormQuad)
    delete lstNormQuad;

  // display lists are auto-deleted by DrawableScene
}

QString SceneEntity::extremitiesInfo()
{
  return QString(" X min%1 max%2, Y min%3 max%4, Z: min%5 max%6\n")
                .arg((float)extrem.xmin).arg((float)extrem.xmax)
                .arg((float)extrem.ymin).arg((float)extrem.ymax)
                .arg((float)extrem.zmin).arg((float)extrem.zmax);
}

bool SceneEntity::hasData(){

  return (lstPoint?(*lstPoint).size():0   || lstLine?(*lstLine).size():0     ||
          lstTria?(*lstTria).size():0     || lstQuadri?(*lstQuadri).size():0 ||
          lstSphere?(*lstSphere).size():0 || lstSphoid?(*lstSphoid).size():0 ||
          lstText?(*lstText).size():0);
}

QString SceneEntity::sceneInfo()
{
  QString info;
  cuv_float toFill[3];
  halfSize(toFill);
  if (lstPoint)
    info += QString(" %1 Points, ").arg((long unsigned int)(*lstPoint).size());
  if (lstLine)
    info += QString(" %1 Lines, ").arg((long unsigned int)(*lstLine).size());
  if (lstTria)
    info += QString(" %1 Triangles, ").arg((long unsigned int)(*lstTria).size());
  if (lstQuadri)
    info += QString(" %1 Quadrilaterals, ").arg((long unsigned int)(*lstQuadri).size());
  if (lstSphere)
    info += QString(" %1 Spheres, ").arg((long unsigned int)(*lstSphere).size());
  if (lstSphoid)
    info += QString(" %1 Sphoids, ").arg((long unsigned int)(*lstSphoid).size());
  if (lstText)
    info += QString(" %1 Texts, ").arg((long unsigned int)(*lstText).size());
  info += QString(" Lengths: x%1 y%2 z%3\n")
    .arg((float)toFill[0]*2)
    .arg((float)toFill[1]*2)
    .arg((float)toFill[2]*2);
  return info;
}

int SceneEntity::scenePrimitives()
{
  int primitives=0;

  if (lstPoint)
    primitives += (int)(*lstPoint).size();
  if (lstLine)
    primitives += (int)(*lstLine).size();
  if (lstTria)
    primitives += (int)(*lstTria).size();
  if (lstQuadri)
    primitives += (int)(*lstQuadri).size();
  if (lstSphere)
    primitives += (int)(*lstSphere).size();
  if (lstSphoid)
    primitives += (int)(*lstSphoid).size();
  if (lstText)
    primitives += (int)(*lstText).size();

  return primitives;
}

void SceneEntity::setEditEntity(bool edit){
  editEntity = edit;
}

bool SceneEntity::getEditEntity() {
  return editEntity;
}

//increase extremeties by the given input amounts.
void SceneEntity::editExtremeties(float xmin,float ymin,float zmin,float xmax,float ymax,float zmax)
{
  extrem.xmin += xmin; extrem.xmax += xmax;
  extrem.ymin += ymin; extrem.ymax += ymax;
  extrem.zmin += zmin; extrem.zmax += zmax;
}

float SceneEntity::largestExtremUnit(){
  float * xunit = new float;
  float * yunit = new float;
  float * zunit = new float;
  getEntityUnit(xunit,yunit,zunit);
  float largest=*xunit;
  if(*xunit<*yunit)
    largest=*yunit;
  if(largest<*zunit)
    largest=*zunit;
  return largest;
}

void SceneEntity::getEntityUnit(float* xunit,float* yunit,float* zunit)
{
  *xunit=extrem.xmax-extrem.xmin;
  *yunit=extrem.ymax-extrem.ymin;
  *zunit=extrem.zmax-extrem.zmin;
}

void SceneEntity::getTranslatef(GLfloat toFill[3])
{
  vcopy3(toFill,translatef);
}

void SceneEntity::getScalef(GLfloat toFill[3])
{
  vcopy3(toFill,scalef);
}

void SceneEntity::getRotatef(GLfloat toFill[3])
{
  vcopy3(toFill,rotatef);
}

Lessa_Text * SceneEntity::getLstText()
{
  return (Lessa_Text *)lstText;
}

void SceneEntity::translate(cuv_float x,cuv_float y, cuv_float z)
{
  if(editEntity){
    translatef[0] += x;
    translatef[1] += y;
    translatef[2] += z;
    extrem.xmin += x; extrem.xmax += x;
    extrem.ymin += y; extrem.ymax += y;
    extrem.zmin += z; extrem.zmax += z;
  }
}

//translates scene in x,y,z direction by sceneentity units
//make sure editEntity is set to true
void SceneEntity::translateUnits(int x,int y,int z)
{
  translate(x*(extrem.xmax-extrem.xmin),
        y*(extrem.ymax-extrem.ymin),
        z*(extrem.zmax-extrem.zmin));
}

void SceneEntity::scale(cuv_float scaleValue){
  scale(scaleValue,scaleValue,scaleValue);
}

void SceneEntity::scale(cuv_float x,cuv_float y, cuv_float z)
{
  if(editEntity){
    if(x && y && z){
    scalef[0] *= x;
    scalef[1] *= y;
    scalef[2] *= z;
    float halfsize[3] = {(extrem.xmax-extrem.xmin)/2,
                         (extrem.ymax-extrem.ymin)/2,
                         (extrem.zmax-extrem.zmin)/2 };
    extrem.xmin -= (x-1)*halfsize[0];
    extrem.xmax += (x-1)*halfsize[0];
    extrem.ymin -= (y-1)*halfsize[1];
    extrem.ymax += (y-1)*halfsize[1];
    extrem.zmin -= (z-1)*halfsize[2];
    extrem.zmax += (z-1)*halfsize[2];
    }
  }
}

void SceneEntity::rotate(const cuv_float rot[4]) //rot={rotation,+-1,+-1,+-1}
{
  if(rot[1])
    rotate(rot[0]*rot[1],0,0);
  else if(rot[2])
    rotate(0,rot[0]*rot[2],0);
  else if(rot[3])
    rotate(0,0,rot[0]*rot[3]);
}

void SceneEntity::rotate(cuv_float x,cuv_float y, cuv_float z) //{rotation in x,y,z}
{
  if(editEntity){
    rotatef[0] -= x;
    rotatef[1] -= y;
    rotatef[2] -= z;
//    computeRotationExtrems(x,y,z);
  }
}

void SceneEntity::computeRotationExtrems(cuv_float x,cuv_float y,cuv_float z)
{
  cuv_float bbcorners[8][3]; //eight points (x,y,z)
  bbcorners[0][0]=extrem.xmin; bbcorners[0][1]=extrem.ymin; bbcorners[0][2]=extrem.zmin;
  bbcorners[1][0]=extrem.xmax; bbcorners[1][1]=extrem.ymin; bbcorners[1][2]=extrem.zmin;
  bbcorners[2][0]=extrem.xmin; bbcorners[2][1]=extrem.ymax; bbcorners[2][2]=extrem.zmin;
  bbcorners[3][0]=extrem.xmin; bbcorners[3][1]=extrem.ymin; bbcorners[3][2]=extrem.zmax;
  bbcorners[4][0]=extrem.xmax; bbcorners[4][1]=extrem.ymax; bbcorners[4][2]=extrem.zmin;
  bbcorners[5][0]=extrem.xmax; bbcorners[5][1]=extrem.ymin; bbcorners[5][2]=extrem.zmax;
  bbcorners[6][0]=extrem.xmin; bbcorners[6][1]=extrem.ymax; bbcorners[6][2]=extrem.zmax;
  bbcorners[7][0]=extrem.xmax; bbcorners[7][1]=extrem.ymax; bbcorners[7][2]=extrem.zmax;  
  cuv_float temp3[3];
  cuv_float temp4[3][4] = {{x,1,0,0},{y,0,1,0},{z,0,0,1}};
  for(int i=0;i<8;i++){
    vrotate(temp3,bbcorners[i],temp4[0]);
    vrotate(temp3,bbcorners[i],temp4[1]);
    vrotate(temp3,bbcorners[i],temp4[2]);
    vcopy3(bbcorners[i],temp3);
    extrem.xmin = bbcorners[i][0]; extrem.xmax = bbcorners[i][0]; 
    extrem.ymin = bbcorners[i][1]; extrem.ymax = bbcorners[i][1]; 
    extrem.zmin = bbcorners[i][2]; extrem.zmax = bbcorners[i][2]; 
  }
}

void SceneEntity::transform() { //transform model and lighting
  glTranslatef(translatef[0],translatef[1],translatef[2]);
  glRotatef(rotatef[0],1,0,0);
  glRotatef(rotatef[1],0,1,0);
  glRotatef(rotatef[2],0,0,1);
  glScalef(scalef[0],scalef[1],scalef[2]);

  //for fixed light sources.
  drawLight(); //transforms all light sources see drawablescene.h::drawLight()
}

const float * SceneEntity::getTranslation() {
  return translatef;
}

void SceneEntity::copyScene(SceneEntity* scene)
{
  scene->setDisplayLists(opaqSolidList,opaqWireList,outlineList,
              transSolidList,transWireList);
  scene->setBooleans(useDisplayLists,opaqSolidData,opaqWireData,outlineData,transSolidData,transWireData);
  scene->compareMinMax(extrem);
}

void SceneEntity::setBooleans(bool udl,bool osolid,bool owire,
                              bool outline,bool tsolid,bool twire)
{
  useDisplayLists=udl;
  opaqSolidData=osolid;
  opaqWireData=owire;
  outlineData=outline;
  transSolidData=tsolid;
  transWireData=twire;
}

void SceneEntity::setDisplayLists(GLuint osolid,GLuint owire,GLuint outline,GLuint tsolid,GLuint twire)
{
  opaqSolidList=osolid;
  opaqWireList=owire;
  outlineList=outline;
  transSolidList=tsolid;
  transWireList=twire;
}

void SceneEntity::drawOpaqSolid()
{
  if (opaqSolidData)
  {
    if (useDisplayLists && opaqSolidList)
    {
      glCallList(opaqSolidList);
    }
    else
    {
      __lessa_uint64 tempSize;
      point  *pointPtr;
      line   *linePtr;
      tria   *triaPtr;
      quadri *quadriPtr;
      sphere *spherePtr;
      sphoid *sphoidPtr;


      cuv_float lastColor[4];
      cuv_float lastNormal[3];

      if (useDisplayLists && !opaqSolidList)
      {
        opaqSolidList = glGenLists(1);
        if (opaqSolidList)
        {
          registerList(opaqSolidList);
          glNewList(opaqSolidList, GL_COMPILE_AND_EXECUTE);
        }
        // else just silently forget about making a display list
      }

      // flag that the current color/normal state is not based on the last one
      normOpt(NULL, lastNormal, true);
      colorOpt(NULL, lastColor, true);

      if (lstPoint)
      {
        tempSize = (*lstPoint).size();

        if (tempSize)
        {
          glBegin(GL_POINTS);
          pointPtr = (*lstPoint)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, pointPtr = ++(*lstPoint) )
          {
            if ( (pointPtr->flags & SFILL) &&
                 !(pointPtr->flags & STRANSPARENT) ) // solid, opaque
            {
              drawPointNullNorm(pointPtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstLine)
      {
        tempSize = (*lstLine).size();
        if (tempSize)
        {
          glBegin(GL_LINES);
          linePtr = (*lstLine)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, linePtr = ++(*lstLine) )
          {
            if ( (linePtr->flags & SFILL) &&
                 !(linePtr->flags & STRANSPARENT) ) // solid, opaque
            {
              drawLineNullNorm(linePtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstTria)
      {
        tempSize = (*lstTria).size();
        if (tempSize)
        {
          glBegin(GL_TRIANGLES);
          triaPtr = (*lstTria)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             ++i, triaPtr = ++(*lstTria) )
          {
            if (  (triaPtr->flags & SFILL) &&
                !(triaPtr->flags & STRANSPARENT) ) // solid, opaque
            {
              drawTria(triaPtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstQuadri)
      {
        tempSize = (*lstQuadri).size();

        if (tempSize)
        {
          glBegin(GL_QUADS);
          quadriPtr = (*lstQuadri)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, quadriPtr = ++(*lstQuadri) )
          {
            if (  (quadriPtr->flags & SFILL) &&
                !(quadriPtr->flags & STRANSPARENT) ) // solid, opaque
            {
              drawQuadri(quadriPtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      // Any glu stuff should be last since normals are not returned
      if (lstSphere)
      {
        tempSize = (*lstSphere).size();

        if (tempSize)
        {
          spherePtr = (*lstSphere)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, spherePtr = ++(*lstSphere) )
          {
            if (  (spherePtr->flags & SFILL) &&
                !(spherePtr->flags & STRANSPARENT) ) // solid, opaque
            {
              drawSphere(spherePtr, lastColor);
            }
          }
        }
      }

      if (lstSphoid)
      {
        tempSize = (*lstSphoid).size();

        if (tempSize)
        {
          sphoidPtr = (*lstSphoid)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, sphoidPtr = ++(*lstSphoid) )
          {
            if (  (sphoidPtr->flags & SFILL) &&
                !(sphoidPtr->flags & STRANSPARENT) ) // solid, opaque
            {
              drawSphoid(sphoidPtr, lastColor);
            }
          }
        }
      }

      if (useDisplayLists && opaqSolidList)
      {
        glEndList();
      }
    }

  }
}

void SceneEntity::drawOpaqWire()
{
  if (opaqWireData)
  {
    if (useDisplayLists && opaqWireList)
    {
      glCallList(opaqWireList);
    }
    else
    {
      __lessa_uint64 tempSize;
      point  *pointPtr;
      line   *linePtr;
      tria   *triaPtr;
      quadri *quadriPtr;
      sphere *spherePtr;
      sphoid *sphoidPtr;

      if (useDisplayLists && !opaqWireList)
      {
        opaqWireList = glGenLists(1);
        if (opaqWireList)
        {
          registerList(opaqWireList);
          glNewList(opaqWireList, GL_COMPILE_AND_EXECUTE);
        }
        // else just silently forget about making a display list
      }

      // flag that the current color/normal state is not based on the last one
      cuv_float lastColor[4];
      cuv_float lastNormal[3];

      normOpt(NULL, lastNormal, true);
      colorOpt(NULL, lastColor, true);

      if (lstPoint)
      {
        tempSize = (*lstPoint).size();

        if (tempSize)
        {
          glBegin(GL_POINTS);
          pointPtr = (*lstPoint)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, pointPtr = ++(*lstPoint) )
          {
            /*
            if (!(pointPtr->flags & STRANSPARENT) && //opaque, no fill, wire
                ( (!(pointPtr->flags & SFILL) &&
             (pointPtr->flags & SOUTLINE)) ||
            ( (pointPtr->flags & SFILL) &&
              !(pointPtr->flags & SOUTLINE)) ) ) // no fill, wire, opaque
            {
            */
            if ( !(pointPtr->flags & SFILL) &&
               (pointPtr->flags & SOUTLINE) &&
               !(pointPtr->flags & STRANSPARENT) ) // no fill, wire, opaque
            {
              drawPointNullNorm(pointPtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstLine)
      {
        tempSize = (*lstLine).size();

        if (tempSize)
        {
          glBegin(GL_LINES);
          linePtr = (*lstLine)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, linePtr = ++(*lstLine) )
          {
            /*
            if ( !(linePtr->flags & STRANSPARENT) && //opaque, no fill, wire
                 ( (!(linePtr->flags & SFILL) &&
              (linePtr->flags & SOUTLINE)) ||
                   ( (linePtr->flags & SFILL) &&
               !(linePtr->flags & SOUTLINE)) ) ) //opaque, fill, no wire
            {
            */
            if ( !(linePtr->flags & SFILL) &&
               (linePtr->flags & SOUTLINE) &&
               !(linePtr->flags & STRANSPARENT) ) // no fill, wire, opaque
            {
              drawLineNullNorm(linePtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstTria)
      {
        tempSize = (*lstTria).size();

        if (tempSize)
        {
          glBegin(GL_TRIANGLES);
          triaPtr = (*lstTria)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, triaPtr = ++(*lstTria) )
          {
            if ( !(triaPtr->flags & SFILL) &&
               (triaPtr->flags & SOUTLINE) &&
               !(triaPtr->flags & STRANSPARENT) ) // no fill, wire, opaque
            {
              drawTriaNullNorm(triaPtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstQuadri)
      {
        tempSize = (*lstQuadri).size();

        if (tempSize)
        {
          glBegin(GL_QUADS);
          quadriPtr = (*lstQuadri)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, quadriPtr = ++(*lstQuadri) )
          {
            if ( !(quadriPtr->flags & SFILL) &&
               (quadriPtr->flags & SOUTLINE) &&
               !(quadriPtr->flags & STRANSPARENT) ) // no fill, wire, opaque
            {
              drawQuadriNullNorm(quadriPtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstSphere)
      {
        tempSize = (*lstSphere).size();

        if (tempSize)
        {
          spherePtr = (*lstSphere)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, spherePtr = ++(*lstSphere) )
          {
            if ( !(spherePtr->flags & SFILL) &&
               (spherePtr->flags & SOUTLINE) &&
               !(spherePtr->flags & STRANSPARENT) ) // no fill, wire, opaque
            {
              drawSphereNullNorm(spherePtr, lastColor, lastNormal);
            }
          }
        }
      }

      if (lstSphoid)
      {
        tempSize = (*lstSphoid).size();

        if (tempSize)
        {
          sphoidPtr = (*lstSphoid)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, sphoidPtr = ++(*lstSphoid) )
          {
            if ( !(sphoidPtr->flags & SFILL) &&
               (sphoidPtr->flags & SOUTLINE) &&
               !(sphoidPtr->flags & STRANSPARENT) ) // no fill, wire, opaque
            {
              drawSphoidNullNorm(sphoidPtr, lastColor, lastNormal);
            }
          }
        }
      }

      if (useDisplayLists && opaqWireList)
      {
        glEndList();
      }
    }

  }
}

void SceneEntity::drawOutline()
{
  if (outlineData)
  {
    if (useDisplayLists && outlineList)
    {
      glCallList(outlineList);
    }
    else
    {
      __lessa_uint64 tempSize;
      point  *pointPtr;
      line   *linePtr;
      tria   *triaPtr;
      quadri *quadriPtr;
      sphere *spherePtr;
      sphoid *sphoidPtr;

      cuv_float lastColor[4];
      cuv_float lastNormal[3];

      if (useDisplayLists && !outlineList)
      {
        outlineList = glGenLists(1);
        if (outlineList)
        {
          registerList(outlineList);
          glNewList(outlineList, GL_COMPILE_AND_EXECUTE);
        }
        // else just silently forget about making a display list
      }

      // flag that the current color/normal state is not based on the last one
      normOpt(NULL, lastNormal, true);
      colorOpt(NULL, lastColor, true);

      if (lstPoint)
      {
        tempSize = (*lstPoint).size();

        if (tempSize)
        {
          glBegin(GL_POINTS);
          pointPtr = (*lstPoint)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, pointPtr = ++(*lstPoint) )
          {
            if ( (  (pointPtr->flags & SFILL) &&
                (pointPtr->flags & SOUTLINE) ) || // fill, wire
               ( !(pointPtr->flags & SFILL) &&
                 !(pointPtr->flags & SOUTLINE) ) ) // no-fill, no-wire
            {
              drawPointNullNormNoColor(pointPtr, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstLine)
      {
        tempSize = (*lstLine).size();

        if (tempSize)
        {
          glBegin(GL_LINES);
          linePtr = (*lstLine)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, linePtr = ++(*lstLine) )
          {
            if ( (  (linePtr->flags & SFILL) &&
                (linePtr->flags & SOUTLINE) ) || // fill, wire
               ( !(linePtr->flags & SFILL) &&
                 !(linePtr->flags & SOUTLINE) ) ) // no-fill, no-wire
            {
              drawLineNullNormNoColor(linePtr, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstTria)
      {
        tempSize = (*lstTria).size();

        if (tempSize)
        {
          glBegin(GL_TRIANGLES);
          triaPtr = (*lstTria)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, triaPtr = ++(*lstTria) )
          {
            if ( (  (triaPtr->flags & SFILL) &&
                (triaPtr->flags & SOUTLINE) ) || // fill, wire
               ( !(triaPtr->flags & SFILL) &&
                 !(triaPtr->flags & SOUTLINE) ) ) // no-fill, no-wire
            {
              drawTriaNullNormNoColor(triaPtr, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstQuadri)
      {
        tempSize = (*lstQuadri).size();

        if (tempSize)
        {
          glBegin(GL_QUADS);
          quadriPtr = (*lstQuadri)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, quadriPtr = ++(*lstQuadri) )
          {
            if ( (  (quadriPtr->flags & SFILL) &&
                (quadriPtr->flags & SOUTLINE) ) || // fill, wire
               ( !(quadriPtr->flags & SFILL) &&
                 !(quadriPtr->flags & SOUTLINE) ) ) // no-fill, no-wire
            {
              drawQuadriNullNormNoColor(quadriPtr, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstSphere)
      {
        tempSize = (*lstSphere).size();

        if (tempSize)
        {
          spherePtr = (*lstSphere)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, spherePtr = ++(*lstSphere) )
          {
            if ( (  (spherePtr->flags & SFILL) &&
                (spherePtr->flags & SOUTLINE) ) || // fill, wire
               ( !(spherePtr->flags & SFILL) &&
                 !(spherePtr->flags & SOUTLINE) ) ) // no-fill, no-wire
            {
              drawSphereNullNormNoColor(spherePtr, lastNormal);
            }
          }
        }
      }

      if (lstSphoid)
      {
        tempSize = (*lstSphoid).size();

        if (tempSize)
        {
          sphoidPtr = (*lstSphoid)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, sphoidPtr = ++(*lstSphoid) )
          {
            if ( (  (sphoidPtr->flags & SFILL) &&
                (sphoidPtr->flags & SOUTLINE) ) || // fill, wire
               ( !(sphoidPtr->flags & SFILL) &&
                 !(sphoidPtr->flags & SOUTLINE) ) ) // no-fill, no-wire
            {
              drawSphoidNullNormNoColor(sphoidPtr, lastNormal);
            }
          }
        }
      }

      if (useDisplayLists && outlineList)
      {
        glEndList();
      }
    }

  }
}

void SceneEntity::drawTransSolid()
{
  if (transSolidData)
  {
    if (useDisplayLists && transSolidList)
    {
      glCallList(transSolidList);
    }
    else
    {
      __lessa_uint64 tempSize;
      tria   *triaPtr;
      quadri *quadriPtr;
      sphere *spherePtr;
      sphoid *sphoidPtr;

      cuv_float lastColor[4];
      cuv_float lastNormal[3];

      if (useDisplayLists && !transSolidList)
      {
        transSolidList = glGenLists(1);
        if (transSolidList)
        {
          registerList(transSolidList);
          glNewList(transSolidList, GL_COMPILE_AND_EXECUTE);
        }
        // else just silently forget about making a display list
      }

      // flag that the current color/normal state is not based on the last one
      normOpt(NULL, lastNormal, true);
      colorOpt(NULL, lastColor, true);

      if (lstTria)
      {
        tempSize = (*lstTria).size();

        if (tempSize)
        {
          glBegin(GL_TRIANGLES);
          triaPtr = (*lstTria)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             ++i, triaPtr = ++(*lstTria) )
          {
            if (  (triaPtr->flags & SFILL) &&
                (triaPtr->flags & STRANSPARENT) ) // solid, trans
            {
              drawTria(triaPtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstQuadri)
      {
        tempSize = (*lstQuadri).size();

        if (tempSize)
        {
          glBegin(GL_QUADS);
          quadriPtr = (*lstQuadri)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, quadriPtr = ++(*lstQuadri) )
          {
            if (  (quadriPtr->flags & SFILL) &&
                (quadriPtr->flags & STRANSPARENT) ) // solid, trans
            {
              drawQuadri(quadriPtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      // Any glu stuff should be last since normals are not returned
      if (lstSphere)
      {
        tempSize = (*lstSphere).size();

        if (tempSize)
        {
          spherePtr = (*lstSphere)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, spherePtr = ++(*lstSphere) )
          {
            if (  (spherePtr->flags & SFILL) &&
                (spherePtr->flags & STRANSPARENT) ) // solid, trans
            {
              drawSphere(spherePtr, lastColor);
            }
          }
        }
      }

      if (lstSphoid)
      {
        tempSize = (*lstSphoid).size();

        if (tempSize)
        {
          sphoidPtr = (*lstSphoid)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, sphoidPtr = ++(*lstSphoid) )
          {
            if (  (sphoidPtr->flags & SFILL) &&
                (sphoidPtr->flags & STRANSPARENT) ) // solid, trans
            {
              drawSphoid(sphoidPtr, lastColor);
            }
          }
        }
      }

      if (useDisplayLists && transSolidList)
      {
        glEndList();
      }
    }

  }
}

void SceneEntity::drawTransWire()
{
  if (transWireData)
  {
    if (useDisplayLists && transWireList)
    {
      glCallList(transWireList);
    }
    else
    {
      __lessa_uint64 tempSize;
      point  *pointPtr;
      line   *linePtr;
      tria   *triaPtr;
      quadri *quadriPtr;
      sphere *spherePtr;
      sphoid *sphoidPtr;

      cuv_float lastColor[4];
      cuv_float lastNormal[3];

      if (useDisplayLists && !transWireList)
      {
        transWireList = glGenLists(1);
        if (transWireList)
        {
          registerList(transWireList);
          glNewList(transWireList, GL_COMPILE_AND_EXECUTE);
        }
        // else just silently forget about making a display list
      }

      // flag that the current color/normal state is not based on the last one
      normOpt(NULL, lastNormal, true);
      colorOpt(NULL, lastColor, true);

      if (lstPoint)
      {
        tempSize = (*lstPoint).size();

        if (tempSize)
        {
          glBegin(GL_POINTS);
          pointPtr = (*lstPoint)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, pointPtr = ++(*lstPoint) )
          {
            if (     (pointPtr->flags & STRANSPARENT) &&
                 ( (!(pointPtr->flags & SFILL) &&
                  (pointPtr->flags & SOUTLINE)) ||
                   ( (pointPtr->flags & SFILL) &&
                   !(pointPtr->flags & SOUTLINE)) ) ) // no fill, wire, trans
            {
              drawPointNullNorm(pointPtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstLine)
      {
        tempSize = (*lstLine).size();

        if (tempSize)
        {
          glBegin(GL_LINES);
          linePtr = (*lstLine)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, linePtr = ++(*lstLine) )
          {
            if (     (linePtr->flags & STRANSPARENT) &&
                 ( (!(linePtr->flags & SFILL) &&
                  (linePtr->flags & SOUTLINE)) ||
                   ( (linePtr->flags & SFILL) &&
                   !(linePtr->flags & SOUTLINE)) ) ) // no fill, wire, trans
            {
              drawLineNullNorm(linePtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstTria)
      {
        tempSize = (*lstTria).size();

        if (tempSize)
        {
          glBegin(GL_TRIANGLES);
          triaPtr = (*lstTria)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, triaPtr = ++(*lstTria) )
          {
            if ( !(triaPtr->flags & SFILL) &&
               (triaPtr->flags & SOUTLINE) &&
               (triaPtr->flags & STRANSPARENT) ) // no fill, wire, trans
            {
              drawTriaNullNorm(triaPtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstQuadri)
      {
        tempSize = (*lstQuadri).size();

        if (tempSize)
        {
          glBegin(GL_QUADS);
          quadriPtr = (*lstQuadri)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, quadriPtr = ++(*lstQuadri) )
          {
            if ( !(quadriPtr->flags & SFILL) &&
               (quadriPtr->flags & SOUTLINE) &&
               (quadriPtr->flags & STRANSPARENT) ) // no fill, wire, trans
            {
              drawQuadriNullNorm(quadriPtr, lastColor, lastNormal);
            }
          }
          glEnd();
        }
      }

      if (lstSphere)
      {
        tempSize = (*lstSphere).size();

        if (tempSize)
        {
          spherePtr = (*lstSphere)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, spherePtr = ++(*lstSphere) )
          {
            if ( !(spherePtr->flags & SFILL) &&
               (spherePtr->flags & SOUTLINE) &&
               (spherePtr->flags & STRANSPARENT) ) // no fill, wire, trans
            {
              drawSphereNullNorm(spherePtr, lastColor, lastNormal);
            }
          }
        }
      }

      if (lstSphoid)
      {
        tempSize = (*lstSphoid).size();

        if (tempSize)
        {
          sphoidPtr = (*lstSphoid)[0];
          for (__lessa_uint64 i = 0 ; i < tempSize ;
             i++, sphoidPtr = ++(*lstSphoid) )
          {
            if ( !(sphoidPtr->flags & SFILL) &&
               (sphoidPtr->flags & SOUTLINE) &&
               (sphoidPtr->flags & STRANSPARENT) ) // no fill, wire, trans
            {
              drawSphoidNullNorm(sphoidPtr, lastColor, lastNormal);
            }
          }
        }
      }

      if (useDisplayLists && transWireList)
      {
        glEndList();
      }
    }
  }
}

void SceneEntity::useDL(bool usedl)
{
  useDisplayLists = usedl;
}


const point *SceneEntity::addPoint(cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored)
{
  point *pointPtr=0;

  try
  {
    if (!lstPoint)
      lstPoint = new Lessa_Point();

    pointPtr = (*lstPoint).grab();
  }
  catch(...)
  {
    return NULL;
  }

  pointPtr->flags = flags;
  if ( !getCUVFloats(ioDeviceIn, pointPtr->vect1, 3, bytesMirrored) )
  {
    (*lstPoint).remove();
    return NULL;
  }

  if ( (!(pointPtr->flags & SFILL) && (pointPtr->flags & SOUTLINE)) ||
     ((pointPtr->flags & SFILL) && !(pointPtr->flags & SOUTLINE)) ) // only get colours when needed.
  {
    if (pointPtr->flags & STRANSPARENT) // read single colour & transparent
    {
      try
      {
        if (!lstColSingTrans)
          lstColSingTrans = new Lessa_Cfloat_1x4();

        pointPtr->color = (cuv_float *) (*lstColSingTrans).grab();
      }
      catch(...)
      {
        (*lstPoint).remove();
        return NULL;
      }

      if ( !getCUVFloats(ioDeviceIn, pointPtr->color, 4, bytesMirrored) )
      {
        (*lstPoint).remove();
        (*lstColSingTrans).remove();
        return NULL;
      }

      transWireData = true;
      if (transWireList)
        unregisterList(transWireList);
    }
    else // read single colour & opaque
    {
      try
      {
        if (!lstColSingOpaq)
          lstColSingOpaq = new Lessa_Cfloat_1x3();

        pointPtr->color = (cuv_float *) (*lstColSingOpaq).grab();
      }
      catch(...)
      {
        (*lstPoint).remove();
        return NULL;
      }

      if ( !getCUVFloats(ioDeviceIn, pointPtr->color, 3, bytesMirrored) )
      {
        (*lstPoint).remove();
        (*lstColSingOpaq).remove();
        return NULL;
      }

      opaqWireData = true;
      if (opaqWireList)
        unregisterList(opaqWireList);
    }
  }
  else
  {
    pointPtr->color = NULL;

    outlineData = true;
    if (outlineList)
      unregisterList(outlineList);
  }

  newMinMax(pointPtr->vect1);
  return pointPtr;
}

const line *SceneEntity::addLine(cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored)
{
  line *linePtr=0;

  try
  {
    if (!lstLine)
      lstLine = new Lessa_Line();

    linePtr = (*lstLine).grab();
  }
  catch(...)
  {
    return NULL;
  }

  linePtr->flags = flags;
  if ( !getCUVFloats(ioDeviceIn, linePtr->vect1, 3, bytesMirrored) ||
     !getCUVFloats(ioDeviceIn, linePtr->vect2, 3, bytesMirrored) )
  {
    (*lstLine).remove();
    return NULL;
  }

  if ( (!(linePtr->flags & SFILL) && (linePtr->flags & SOUTLINE)) ||
     ((linePtr->flags & SFILL) && !(linePtr->flags & SOUTLINE)) ) // only get colours when needed.
  {
    if (linePtr->flags & SMULTICOLOR) // finding out how many colours to read
    {
      if (linePtr->flags & STRANSPARENT) // read multicolour & transparent
      {
        try
        {
          if (!lstColDoubTrans)
            lstColDoubTrans = new Lessa_Cfloat_2x4();

          linePtr->color = (cuv_float *) (*lstColDoubTrans).grab();
        }
        catch(...)
        {
          (*lstLine).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, linePtr->color, 8, bytesMirrored) )
        {
          (*lstLine).remove();
          (*lstColDoubTrans).remove();
          return NULL;
        }

        transWireData = true;
        if (transWireList)
          unregisterList(transWireList);
      }
      else // read multicolour & opaque
      {
        try
        {
          if (!lstColDoubOpaq)
            lstColDoubOpaq = new Lessa_Cfloat_2x3();

          linePtr->color = (cuv_float *) (*lstColDoubOpaq).grab();
        }
        catch(...)
        {
          (*lstLine).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, linePtr->color, 6, bytesMirrored) )
        {
          (*lstLine).remove();
          (*lstColDoubOpaq).remove();
          return NULL;
        }

        opaqWireData = true;
        if (opaqWireList)
          unregisterList(opaqWireList);
      }
    }
    else
    {
      if (linePtr->flags & STRANSPARENT) // read single colour & transparent
      {
        try
        {
          if (!lstColSingTrans)
            lstColSingTrans = new Lessa_Cfloat_1x4();

          linePtr->color = (cuv_float *) (*lstColSingTrans).grab();
        }
        catch(...)
        {
          (*lstLine).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, linePtr->color, 4, bytesMirrored) )
        {
          (*lstLine).remove();
          (*lstColSingTrans).remove();
          return NULL;
        }

        transWireData = true;
        if (transWireList)
          unregisterList(transWireList);
      }
      else // read single colour & opaque
      {
        try
        {
          if (!lstColSingOpaq)
            lstColSingOpaq = new Lessa_Cfloat_1x3();

          linePtr->color = (cuv_float *) (*lstColSingOpaq).grab();
        }
        catch(...)
        {
          (*lstLine).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, linePtr->color, 3, bytesMirrored) )
        {
          (*lstLine).remove();
          (*lstColSingOpaq).remove();
          return NULL;
        }

        opaqWireData = true;
        if (opaqWireList)
          unregisterList(opaqWireList);
      }
    }
  }
  else
  {
    linePtr->color = NULL;

    outlineData = true;
    if (outlineList)
      unregisterList(outlineList);
  }

  newMinMax(linePtr->vect1);
  newMinMax(linePtr->vect2);
  return linePtr;
}

const tria *SceneEntity::addTria(cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored)
{
  tria   *triaPtr=0;

  bool opaqSolidFound = false;
  bool opaqWireFound = false;
  bool outlineFound = false;
  bool transSolidFound = false;
  bool transWireFound = false;

  try
  {
    if (!lstTria)
      lstTria = new Lessa_Tria();

    triaPtr = (*lstTria).grab();
  }
  catch(...)
  {
    return NULL;
  }

  triaPtr->flags = flags;
  if ( !getCUVFloats(ioDeviceIn, triaPtr->vect1, 3, bytesMirrored) ||
     !getCUVFloats(ioDeviceIn, triaPtr->vect2, 3, bytesMirrored) ||
     !getCUVFloats(ioDeviceIn, triaPtr->vect3, 3, bytesMirrored) )
  {
    (*lstTria).remove();
    return NULL;
  }

  if ( (triaPtr->flags & SFILL) || (triaPtr->flags & SOUTLINE) ) // only get colours when needed.
  {
    if (triaPtr->flags & SMULTICOLOR) // finding out how many colours to read
    {
      if (triaPtr->flags & STRANSPARENT) // read multicolour & transparent
      {
        try
        {
          if (!lstColTriplTrans)
            lstColTriplTrans = new Lessa_Cfloat_3x4();

          triaPtr->color = (cuv_float *) (*lstColTriplTrans).grab();
        }
        catch(...)
        {
          (*lstTria).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, triaPtr->color, 12, bytesMirrored) )
        {
          (*lstTria).remove();
          (*lstColTriplTrans).remove();
          return NULL;
        }

        // determine what to flag as existing if no failures
        if (triaPtr->flags & SFILL)
          transSolidFound = true;
        if (triaPtr->flags & SOUTLINE)
        {
          if (triaPtr->flags & SFILL)
            outlineFound = true;
          else
            transWireFound = true;
        }
      }
      else // read multicolour & opaque
      {
        try
        {
          if (!lstColTriplOpaq)
            lstColTriplOpaq = new Lessa_Cfloat_3x3();

          triaPtr->color = (cuv_float *) (*lstColTriplOpaq).grab();
        }
        catch(...)
        {
          (*lstTria).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, triaPtr->color, 9, bytesMirrored) )
        {
          (*lstTria).remove();
          (*lstColTriplOpaq).remove();
          return NULL;
        }

        // determine what to flag as existing if no failures
        if (triaPtr->flags & SFILL)
          opaqSolidFound = true;
        if (triaPtr->flags & SOUTLINE)
        {
          if (triaPtr->flags & SFILL)
            outlineFound = true;
          else
            opaqWireFound = true;
        }
      }
    }
    else
    {
      if (triaPtr->flags & STRANSPARENT) // read single colour & transparent
      {
        try
        {
          if (!lstColSingTrans)
            lstColSingTrans = new Lessa_Cfloat_1x4();

          triaPtr->color = (cuv_float *) (*lstColSingTrans).grab();
        }
        catch(...)
        {
          (*lstTria).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, triaPtr->color, 4, bytesMirrored) )
        {
          (*lstTria).remove();
          (*lstColSingTrans).remove();
          return NULL;
        }

        // determine what to flag as existing if no failures
        if (triaPtr->flags & SFILL)
          transSolidFound = true;
        if (triaPtr->flags & SOUTLINE)
        {
          if (triaPtr->flags & SFILL)
            outlineFound = true;
          else
            transWireFound = true;
        }
      }
      else // read single colour & opaque
      {
        try
        {
          if (!lstColSingOpaq)
            lstColSingOpaq = new Lessa_Cfloat_1x3();

          triaPtr->color = (cuv_float *) (*lstColSingOpaq).grab();
        }
        catch(...)
        {
          (*lstTria).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, triaPtr->color, 3, bytesMirrored) )
        {
          (*lstTria).remove();
          (*lstColSingOpaq).remove();
          return NULL;
        }

        // determine what to flag as existing if no failures
        if (triaPtr->flags & SFILL)
          opaqSolidFound = true;
        if (triaPtr->flags & SOUTLINE)
        {
          if (triaPtr->flags & SFILL)
            outlineFound = true;
          else
            opaqWireFound = true;
        }
      }
    }
  }
  else
  {
    triaPtr->color = NULL;

    // determine what to flag as existing if no failures
    outlineFound = true;
  }

  if (triaPtr->flags & SNORMALS) // check for normals
  {
    try
    {
      if (!lstNormTripl)
        lstNormTripl = new Lessa_Cfloat_3x3();

      triaPtr->norm = (cuv_float *) (*lstNormTripl).grab();
    }
    catch(...)
    {
      (*lstTria).remove();

      if (triaPtr->flags & SMULTICOLOR)
      {
        if (triaPtr->flags & STRANSPARENT)
          (*lstColTriplTrans).remove();
        else
          (*lstColTriplOpaq).remove();
      }
      else
      {
        if (triaPtr->flags & STRANSPARENT)
          (*lstColSingTrans).remove();
        else
          (*lstColSingOpaq).remove();
      }

      return NULL;
    }

    if ( !getCUVFloats(ioDeviceIn, triaPtr->norm, 9, bytesMirrored) )
    {
      (*lstTria).remove();

      if (triaPtr->flags & SMULTICOLOR)
      {
        if (triaPtr->flags & STRANSPARENT)
          (*lstColTriplTrans).remove();
        else
          (*lstColTriplOpaq).remove();
      }
      else
      {
        if (triaPtr->flags & STRANSPARENT)
          (*lstColSingTrans).remove();
        else
          (*lstColSingOpaq).remove();
      }

      (*lstNormTripl).remove();

      return NULL;
    }
  }

  if (opaqSolidFound)
  {
    opaqSolidData = true;
    if (opaqSolidList)
      unregisterList(opaqSolidList);
  }
  if (opaqWireFound)
  {
    opaqWireData = true;
    if (opaqWireList)
      unregisterList(opaqWireList);
  }
  if (outlineFound)
  {
    outlineData = true;
    if (outlineList)
      unregisterList(outlineList);
  }
  if (transSolidFound)
  {
    transSolidData = true;
    if (transSolidList)
      unregisterList(transSolidList);
  }
  if (transWireFound)
  {
    transWireData = true;
    if (transWireList)
      unregisterList(transWireList);
  }

  newMinMax(triaPtr->vect1);
  newMinMax(triaPtr->vect2);
  newMinMax(triaPtr->vect3);
  return triaPtr;
}

const quadri *SceneEntity::addQuadri(cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored)
{
  quadri *quadriPtr=0;

  bool opaqSolidFound = false;
  bool opaqWireFound = false;
  bool outlineFound = false;
  bool transSolidFound = false;
  bool transWireFound = false;

  try
  {
    if (!lstQuadri)
      lstQuadri = new Lessa_Quadri();

    quadriPtr = (*lstQuadri).grab();
  }
  catch(...)
  {
    return NULL;
  }

  quadriPtr->flags = flags;
  if ( !getCUVFloats(ioDeviceIn, quadriPtr->vect1, 3, bytesMirrored) ||
     !getCUVFloats(ioDeviceIn, quadriPtr->vect2, 3, bytesMirrored) ||
     !getCUVFloats(ioDeviceIn, quadriPtr->vect3, 3, bytesMirrored) ||
     !getCUVFloats(ioDeviceIn, quadriPtr->vect4, 3, bytesMirrored) )
  {
    (*lstQuadri).remove();
    return NULL;
  }

  if ( (quadriPtr->flags & SFILL) || (quadriPtr->flags & SOUTLINE) ) // only get colours when needed.
  {
    if (quadriPtr->flags & SMULTICOLOR) // finding out how many colours to read
    {
      if (quadriPtr->flags & STRANSPARENT) // read multicolour & transparent
      {
        try
        {
          if (!lstColQuadTrans)
            lstColQuadTrans = new Lessa_Cfloat_4x4();

          quadriPtr->color = (cuv_float *) (*lstColQuadTrans).grab();
        }
        catch(...)
        {
          (*lstQuadri).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, quadriPtr->color, 16, bytesMirrored) )
        {
          (*lstQuadri).remove();
          (*lstColQuadTrans).remove();
          return NULL;
        }

        // determine what to flag as existing if no failures
        if (quadriPtr->flags & SFILL)
          transSolidFound = true;
        if (quadriPtr->flags & SOUTLINE)
        {
          if (quadriPtr->flags & SFILL)
            outlineFound = true;
          else
            transWireFound = true;
        }
      }
      else // read multicolour & opaque
      {
        try
        {
          if (!lstColQuadOpaq)
            lstColQuadOpaq = new Lessa_Cfloat_4x3();

          quadriPtr->color = (cuv_float *) (*lstColQuadOpaq).grab();
        }
        catch(...)
        {
          (*lstQuadri).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, quadriPtr->color, 12, bytesMirrored) )
        {
          (*lstQuadri).remove();
          (*lstColQuadOpaq).remove();
          return NULL;
        }

        // determine what to flag as existing if no failures
        if (quadriPtr->flags & SFILL)
          opaqSolidFound = true;
        if (quadriPtr->flags & SOUTLINE)
        {
          if (quadriPtr->flags & SFILL)
            outlineFound = true;
          else
            opaqWireFound = true;
        }
      }
    }
    else
    {
      if (quadriPtr->flags & STRANSPARENT) // read single colour & transparent
      {
        try
        {
          if (!lstColSingTrans)
            lstColSingTrans = new Lessa_Cfloat_1x4();

          quadriPtr->color = (cuv_float *) (*lstColSingTrans).grab();
        }
        catch(...)
        {
          (*lstQuadri).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, quadriPtr->color, 4, bytesMirrored) )
        {
          (*lstQuadri).remove();
          (*lstColSingTrans).remove();
          return NULL;
        }

        // determine what to flag as existing if no failures
        if (quadriPtr->flags & SFILL)
          transSolidFound = true;
        if (quadriPtr->flags & SOUTLINE)
        {
          if (quadriPtr->flags & SFILL)
            outlineFound = true;
          else
            transWireFound = true;
        }
      }
      else // read single colour & opaque
      {
        try
        {
          if (!lstColSingOpaq)
            lstColSingOpaq = new Lessa_Cfloat_1x3();

          quadriPtr->color = (cuv_float *) (*lstColSingOpaq).grab();
        }
        catch(...)
        {
          (*lstQuadri).remove();
          return NULL;
        }

        if ( !getCUVFloats(ioDeviceIn, quadriPtr->color, 3, bytesMirrored) )
        {
          (*lstQuadri).remove();
          (*lstColSingOpaq).remove();
          return NULL;
        }

        // determine what to flag as existing if no failures
        if (quadriPtr->flags & SFILL)
          opaqSolidFound = true;
        if (quadriPtr->flags & SOUTLINE)
        {
          if (quadriPtr->flags & SFILL)
            outlineFound = true;
          else
            opaqWireFound = true;
        }
      }
    }
  }
  else
  {
    quadriPtr->color = NULL;

    // determine what to flag as existing if no failures
    outlineFound = true;
  }

  if (quadriPtr->flags & SNORMALS) // check for normals
  {
    try
    {
    if (!lstNormQuad)
      lstNormQuad = new Lessa_Cfloat_4x3();

      quadriPtr->norm = (cuv_float *) (*lstNormQuad).grab();
    }
    catch(...)
    {
      (*lstQuadri).remove();

      if (quadriPtr->flags & SMULTICOLOR)
      {
        if (quadriPtr->flags & STRANSPARENT)
          (*lstColQuadTrans).remove();
        else
          (*lstColQuadOpaq).remove();
      }
      else
      {
        if (quadriPtr->flags & STRANSPARENT)
          (*lstColSingTrans).remove();
        else
          (*lstColSingOpaq).remove();
      }

      return NULL;
    }

    if ( !getCUVFloats(ioDeviceIn, quadriPtr->norm, 12, bytesMirrored) )
    {
      (*lstQuadri).remove();

      if (quadriPtr->flags & SMULTICOLOR)
      {
        if (quadriPtr->flags & STRANSPARENT)
          (*lstColQuadTrans).remove();
        else
          (*lstColQuadOpaq).remove();
      }
      else
      {
        if (quadriPtr->flags & STRANSPARENT)
          (*lstColSingTrans).remove();
        else
          (*lstColSingOpaq).remove();
      }

      (*lstNormQuad).remove();

      return NULL;
    }
  }

  if (opaqSolidFound)
  {
    opaqSolidData = true;
    if (opaqSolidList)
      unregisterList(opaqSolidList);
  }
  if (opaqWireFound)
  {
    opaqWireData = true;
    if (opaqWireList)
      unregisterList(opaqWireList);
  }
  if (outlineFound)
  {
    outlineData = true;
    if (outlineList)
      unregisterList(outlineList);
  }
  if (transSolidFound)
  {
    transSolidData = true;
    if (transSolidList)
      unregisterList(transSolidList);
  }
  if (transWireFound)
  {
    transWireData = true;
    if (transWireList)
      unregisterList(transWireList);
  }

  newMinMax(quadriPtr->vect1);
  newMinMax(quadriPtr->vect2);
  newMinMax(quadriPtr->vect3);
  newMinMax(quadriPtr->vect4);
  return quadriPtr;
}

const sphere *SceneEntity::addSphere(cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored)
{
  sphere *spherePtr=0;

  GLfloat tempVect[3]; // for extremities

  try
  {
    if (!lstSphere)
      lstSphere = new Lessa_Sphere();

    spherePtr = (*lstSphere).grab();
  }
  catch(...)
  {
    return NULL;
  }

  spherePtr->flags = flags;
  if ( !getCUVFloats(ioDeviceIn, spherePtr->vect1, 3, bytesMirrored) ||
     !getCUVFloats(ioDeviceIn, &spherePtr->radius, 1, bytesMirrored) )
  {
    (*lstSphere).remove();
    return NULL;
  }

  if ( (spherePtr->flags & SFILL) || (spherePtr->flags & SOUTLINE) ) // only get colours when needed.
  {
    if (spherePtr->flags & STRANSPARENT) // read single colour & transparent
    {
      try
      {
        if (!lstColSingTrans)
          lstColSingTrans = new Lessa_Cfloat_1x4();

        spherePtr->color = (cuv_float *) (*lstColSingTrans).grab();
      }
      catch(...)
      {
        (*lstSphere).remove();
        return NULL;
      }

      if ( !getCUVFloats(ioDeviceIn, spherePtr->color, 4, bytesMirrored) )
      {
        (*lstSphere).remove();
        (*lstColSingTrans).remove();
        return NULL;
      }

      // determine what to flag as existing if no failures
      if (spherePtr->flags & SFILL)
      {
        transSolidData = true;
        if (transSolidList)
          unregisterList(transSolidList);
      }
      if (spherePtr->flags & SOUTLINE)
      {
        if (spherePtr->flags & SFILL)
        {
          outlineData = true;
          if (outlineList)
            unregisterList(outlineList);
        }
        else
        {
          transWireData = true;
          if (transWireList)
            unregisterList(transWireList);
        }
      }
    }
    else // read single colour & opaque
    {
      try
      {
        if (!lstColSingOpaq)
          lstColSingOpaq = new Lessa_Cfloat_1x3();

        spherePtr->color = (cuv_float *) (*lstColSingOpaq).grab();
      }
      catch(...)
      {
        (*lstSphere).remove();
        return NULL;
      }

      if ( !getCUVFloats(ioDeviceIn, spherePtr->color, 3, bytesMirrored) )
      {
        (*lstSphere).remove();
        (*lstColSingOpaq).remove();
        return NULL;
      }

      // determine what to flag as existing if no failures
      if (spherePtr->flags & SFILL)
      {
        opaqSolidData = true;
        if (opaqSolidList)
          unregisterList(opaqSolidList);
      }
      if (spherePtr->flags & SOUTLINE)
      {
        if (spherePtr->flags & SFILL)
        {
          outlineData = true;
          if (outlineList)
            unregisterList(outlineList);
        }
        else
        {
          opaqWireData = true;
          if (opaqWireList)
            unregisterList(opaqWireList);
        }
      }
    }
  }
  else
  {
    spherePtr->color = NULL;

    // determine what to flag as existing if no failures
    outlineData = true;
    if (outlineList)
      unregisterList(outlineList);
  }

  tempVect[0] = spherePtr->vect1[0] + spherePtr->radius;
  tempVect[1] = spherePtr->vect1[1] + spherePtr->radius;
  tempVect[2] = spherePtr->vect1[2] + spherePtr->radius;
  newMinMax(tempVect);
  tempVect[0] = spherePtr->vect1[0] - spherePtr->radius;
  tempVect[1] = spherePtr->vect1[1] - spherePtr->radius;
  tempVect[2] = spherePtr->vect1[2] - spherePtr->radius;
  newMinMax(tempVect);

  return spherePtr;
}

const sphoid *SceneEntity::addSphoid(cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored)
{
  sphoid *sphoidPtr=0;

  GLfloat tempVect[3]; // for extremities

  try
  {
    if (!lstSphoid)
      lstSphoid = new Lessa_Sphoid();

    sphoidPtr = (*lstSphoid).grab();
  }
  catch(...)
  {
    return NULL;
  }

  sphoidPtr->flags = flags;
  if ( !getCUVFloats(ioDeviceIn, sphoidPtr->vect1, 3, bytesMirrored) ||
     !getCUVFloats(ioDeviceIn, sphoidPtr->size, 3, bytesMirrored) ||
     !getCUVFloats(ioDeviceIn, sphoidPtr->rotate, 4, bytesMirrored) )
  {
    (*lstSphoid).remove();
    return NULL;
  }

  if ( (sphoidPtr->flags & SFILL) || (sphoidPtr->flags & SOUTLINE) ) // only get colours when needed.
  {
    if (sphoidPtr->flags & STRANSPARENT) // read single colour & transparent
    {
      try
      {
        if (!lstColSingTrans)
          lstColSingTrans = new Lessa_Cfloat_1x4();

        sphoidPtr->color = (cuv_float *) (*lstColSingTrans).grab();
      }
      catch(...)
      {
        (*lstSphoid).remove();
        return NULL;
      }

      if ( !getCUVFloats(ioDeviceIn, sphoidPtr->color, 4, bytesMirrored) )
      {
        (*lstSphoid).remove();
        (*lstColSingTrans).remove();
        return NULL;
      }

      // determine what to flag as existing if no failures
      if (sphoidPtr->flags & SFILL)
      {
        transSolidData = true;
        if (transSolidList)
          unregisterList(transSolidList);
      }
      if (sphoidPtr->flags & SOUTLINE)
      {
        if (sphoidPtr->flags & SFILL)
        {
          outlineData = true;
          if (outlineList)
            unregisterList(outlineList);
        }
        else
        {
          transWireData = true;
          if (transWireList)
            unregisterList(transWireList);
        }
      }
    }
    else // read single colour & opaque
    {
      try
      {
        if (!lstColSingOpaq)
          lstColSingOpaq = new Lessa_Cfloat_1x3();

        sphoidPtr->color = (cuv_float *) (*lstColSingOpaq).grab();
      }
      catch(...)
      {
        (*lstSphoid).remove();
        return NULL;
      }

      if ( !getCUVFloats(ioDeviceIn, sphoidPtr->color, 3, bytesMirrored) )
      {
        (*lstSphoid).remove();
        (*lstColSingOpaq).remove();
        return NULL;
      }

      // determine what to flag as existing if no failures
      if (sphoidPtr->flags & SFILL)
      {
        opaqSolidData = true;
        if (opaqSolidList)
          unregisterList(opaqSolidList);
      }
      if (sphoidPtr->flags & SOUTLINE)
      {
        if (sphoidPtr->flags & SFILL)
        {
          outlineData = true;
          if (outlineList)
            unregisterList(outlineList);
        }
        else
        {
          opaqWireData = true;
          if (opaqWireList)
            unregisterList(opaqWireList);
        }
      }
    }
  }
  else
  {
    sphoidPtr->color = NULL;

    // determine what to flag as existing if no failures
    outlineData = true;
    if (outlineList)
      unregisterList(outlineList);
  }

  // The following code is in no way perfect (with rotations), but it is quick and close enough.
  tempVect[0] = sphoidPtr->vect1[0] + sphoidPtr->size[0];
  tempVect[1] = sphoidPtr->vect1[1] + sphoidPtr->size[1];
  tempVect[2] = sphoidPtr->vect1[2] + sphoidPtr->size[2];
  newMinMax(tempVect);
  tempVect[0] = sphoidPtr->vect1[0] - sphoidPtr->size[0];
  tempVect[1] = sphoidPtr->vect1[1] - sphoidPtr->size[1];
  tempVect[2] = sphoidPtr->vect1[2] - sphoidPtr->size[2];
  newMinMax(tempVect);

  return  sphoidPtr;
}

const text *SceneEntity::addText(cuv_tag flags, QIODevice *ioDeviceIn, bool bytesMirrored)
{
  text *textPtr=0;

  try
  {
    if (!lstText)
      lstText = new Lessa_Text();

    textPtr = (*lstText).grab();
  }
  catch(...)
  {
    return NULL;
  }

  textPtr->flags = flags;
  if ( !getCUVFloats(ioDeviceIn, textPtr->vect1, 3, bytesMirrored) )
  {
    (*lstText).remove();
    return NULL;
  }

  if ( (!(textPtr->flags & SFILL) && (textPtr->flags & SOUTLINE)) ||
     ((textPtr->flags & SFILL) && !(textPtr->flags & SOUTLINE)) ) // only get colours when needed.
  {
    if (textPtr->flags & STRANSPARENT) // read single colour & transparent
    {
      try
      {
        if (!lstColSingTrans)
          lstColSingTrans = new Lessa_Cfloat_1x4();

        textPtr->color = (cuv_float *) (*lstColSingTrans).grab();
      }
      catch(...)
      {
        (*lstText).remove();
        return NULL;
      }

      if ( !getCUVFloats(ioDeviceIn, textPtr->color, 4, bytesMirrored) )
      {
        (*lstText).remove();
        (*lstColSingTrans).remove();
        return NULL;
      }

      transWireData = true;
      if (transWireList)
        unregisterList(transWireList);
    }
    else // read single colour & opaque
    {
      try
      {
        if (!lstColSingOpaq)
          lstColSingOpaq = new Lessa_Cfloat_1x3();

        textPtr->color = (cuv_float *) (*lstColSingOpaq).grab();
      }
      catch(...)
      {
        (*lstText).remove();
        return NULL;
      }

      if ( !getCUVFloats(ioDeviceIn, textPtr->color, 3, bytesMirrored) )
      {
        (*lstText).remove();
        (*lstColSingOpaq).remove();
        return NULL;
      }

      opaqWireData = true;
      if (opaqWireList)
        unregisterList(opaqWireList);
    }
  }
  else
  {
    textPtr->color = NULL;

    outlineData = true;
    if (outlineList)
      unregisterList(outlineList);
  }

  newMinMax(textPtr->vect1);

  cuv_uint charCount=0;
  if ( !getCUVUInt(ioDeviceIn, charCount, bytesMirrored) )
  {
    (*lstText).remove();
    return NULL;
  }

  cuv_ubyte *toFill=new cuv_ubyte(charCount);
  if( !getCUVUBytes(ioDeviceIn, toFill, charCount) )
  {
    (*lstText).remove();
    return NULL;
  }
  textPtr->maintext = QString((char *)toFill);

  return textPtr;
}

void SceneEntity::clear()
{
  if (lstPoint)
    delete lstPoint;
  if (lstLine)
    delete lstLine;
  if (lstTria)
    delete lstTria;
  if (lstQuadri)
    delete lstQuadri;
  if (lstSphere)
    delete lstSphere;
  if (lstSphoid)
    delete lstSphoid;
  if (lstText)
    delete lstText;

  if (lstColSingOpaq)
    delete lstColSingOpaq;
  if (lstColSingTrans)
    delete lstColSingTrans;
  if (lstColDoubOpaq)
    delete lstColDoubOpaq;
  if (lstColDoubTrans)
    delete lstColDoubTrans;
  if (lstColTriplOpaq)
    delete lstColTriplOpaq;
  if (lstColTriplTrans)
    delete lstColTriplTrans;
  if (lstColQuadOpaq)
    delete lstColQuadOpaq;
  if (lstColQuadTrans)
    delete lstColQuadTrans;

  if (lstNormTripl)
    delete lstNormTripl;
  if (lstNormQuad)
    delete lstNormQuad;

  opaqSolidData = false;
  opaqWireData = false;
  outlineData = false;
  transSolidData = false;
  transWireData = false;

  clearDispListExtrem();
}

void SceneEntity::clearDispListExtrem()
{
  unregisterLists();

  opaqSolidList = 0;
  opaqWireList = 0;
  outlineList = 0;
  transSolidList = 0;
  transWireList = 0;

  extrem.xmin = FLT_MAX; extrem.xmax = -FLT_MAX; // set to opposite extremities so that the real
  extrem.ymin = FLT_MAX; extrem.ymax = -FLT_MAX; // exremities can be found (the initialization
  extrem.zmin = FLT_MAX; extrem.zmax = -FLT_MAX; // value cannot be zero).
}
