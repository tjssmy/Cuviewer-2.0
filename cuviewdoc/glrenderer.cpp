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

#ifdef Q_OS_WIN
#  include <math.h>
#else
#include <cmath>
#include <qgl.h>
#include <QWheelEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QKeyEvent>
#endif

#include <QGLBuffer>
#include <QGLShaderProgram>

#include <QtGui>
#include <qapplication.h>
#include <qdatetime.h>
#include <qevent.h>
#include <qmap.h>
#include <qtimer.h>
#include <qstring.h>
#include <QGLWidget>
#include <QDesktopWidget>

#include <GL/glu.h>
#include <GL/gl.h>

#include "../include/cuvtags.h"
#include "glrenderer.h"
#include "../include/cuvcommonstructs.h"
#include "drawablescene.h"
#include "changeablescene.h"
#include "entityscene.h"
#include "sceneentity.h"
#include "../include/cuvvector.h"

#define WHEEL_DELTA 120
#define TIME_UNTIL_CACHE 250 // ms
#define POLY_OFF_FACTOR 1.0F
#define POLY_OFF_UNITS 1.0F

const unsigned int GLRenderer::ALIGN_ADD_AMT = BYTE_ALIGNMENT - 1;
const unsigned int GLRenderer::ALIGN_SHIFT_AMT = log2(BYTE_ALIGNMENT);

using namespace Qt;

/**
    @brief Constructs a GLRenderer object.
  */
GLRenderer::GLRenderer(QGLFormat * glformat, QWidget *parent, 
             const char *name, const QGLWidget *shareWidget, Qt::WindowFlags f)
  : QGLWidget(*glformat, parent, shareWidget, f)
{
  dispListRefs = new QMap<GLuint, unsigned long>();

  mouseDragging = false;

  scene = NULL;
  sceneHasChanged = false;
  cachedImageExists = false;

  renderingPixmap = false;

  cacheImageWidth = -1; // fist call to resizeGL will make this a valid value
  cacheImageHeight = -1; // fist call to resizeGL will make this a valid value
  cacheImageData = NULL;
  cacheImageLineLength = 0;

  glrendererOptions.useThreeButtons = true;
  glrendererOptions.useCachedImage = true;
  glrendererOptions.drawBoxWhileMoving = false;
  glrendererOptions.drawBoxAlways = false;
  glrendererOptions.drawAxis = true;

// begin default state setup (make sure echoed in resetViewState())
  viewState.twoSidedEnabled        = false;
  viewState.smoothShadedEnabled    = true;
  viewState.transparencyEnabled    = true;
  viewState.antialiasEnabled       = false;
  viewState.sphereTess             = 8;

  viewState.outlineEnabled         = true;
  viewState.wireframeEnabled       = false;
  viewState.opaqueWireframeEnabled = false;
  viewState.lineWidth              = 1.0F;
  viewState.outlineColor[0]        = 1.0F;
  viewState.outlineColor[1]        = 0.0F;
  viewState.outlineColor[2]        = 0.0F;

  viewState.lightSources           = 1;
  viewState.lightEnabled           = true;
  viewState.lightingEnabled        = true;
  viewState.ambient                = 0.2F;
  viewState.diffuse                = 0.8F;
  viewState.background             = 0.0F;

  viewState.perspectiveEnabled     = true;

  viewState.fogEnabled             = false;
  viewState.fogLevel               = 0.6F;

  viewState.fov                    = 20.0F;
  viewState.cameraPos[0]           = 0.0F;
  viewState.cameraPos[1]           = 0.0F;
  viewState.cameraPos[2]           = 0.0F;
  viewState.cameraAim[0]           = 0.0F;
  viewState.cameraAim[1]           = 0.0F;
  viewState.cameraAim[2]           = -1.0F;
  viewState.cameraUp[0]            = 0.0F;
  viewState.cameraUp[1]            = 1.0F;
  viewState.cameraUp[2]            = 0.0F;
  viewState.clipPerspNear          = 0.125;
  viewState.clipOrthoNear          = 0.0;

  viewState.gammaEnabled           = false;
  viewState.gammaExponent          = 1.0F;
  viewState.specular               = 0.0F;

  viewState.binWindowEnabled       = false;
  viewState.binWindowIndex         = 0;

  {
    GLubyte *gammaPointer = viewState.gammaLookup;

    for (GLubyte i = (GLubyte)'\x00' ; i < (GLubyte)'\xFF' ; i++)
      *(gammaPointer++) = i;
    *gammaPointer = (GLubyte)'\xFF';
  }

  {
    cuv_uint *countPointer = viewState.binWindowCount;
    GLubyte **palettePointer = viewState.binWindowPalette;

    for (int i=0 ; i<BIN_WINDOW_SIZE ; i++)
    {
      *(countPointer++) = 0U;
      *(palettePointer++) = NULL;
    }
  }

  viewState.lightSources=1;
  for(int i=0;i<MAX_LIGHTSOURCES;i++){
    viewState.fixedLightSource[i]=0;
    viewState.lightEditing[i]=0;
    viewState.lightXTranslate[i]=0;
    viewState.lightYTranslate[i]=0;
    viewState.lightZTranslate[i]=1;
    viewState.lightColor[i][0]=1;
    viewState.lightColor[i][1]=1;
    viewState.lightColor[i][2]=1;
  }
  viewState.showLightPosition = FALSE;

// end default state setup

  QTimer *counter = new QTimer(this);
  connect(counter, SIGNAL(timeout()), this, SLOT(pollForGlError()));
  counter->start(1000);
}

/**
    @brief Destructs the GLRenderer object.
  */
GLRenderer::~GLRenderer()
{
  if (cacheImageData)
    delete[] cacheImageData;

  // clean up any lists still around (just to be sure)
  if (dispListRefs->count())
  {
    qWarning("Not all lists were freed before destroying renderer.");

    DisplayListMap::Iterator it = dispListRefs->begin();
    do
    {
      glDeleteLists(it.key(), 1);
    } while (++it != dispListRefs->end());
  }

  if (solidSphereList)
    glDeleteLists(solidSphereList, 1);

  if (wireSphereList)
    glDeleteLists(wireSphereList, 1);
}

void GLRenderer::quadricError()
{
  qWarning ("Quadric Error");
}

/**
    @brief Updates the projection.
  */
void GLRenderer::updateProjection()
{
  if (scene) // need to know what's here first.
  {
    GLfloat pos[3];
    GLfloat halfSize[3];
    GLdouble approxSize;
    GLfloat objVect[3];

    // setup needed info
    scene->halfSize(halfSize);
    scene->pos(pos);
    approxSize = 2.0 * vlength(halfSize);
    if (!approxSize)
      approxSize = 1.0;
    vsub(objVect, pos, viewState.cameraPos);
    distToObj = vdot(objVect, viewState.cameraAim);

    // calculate the clipping
    actualClipFar = distToObj + approxSize * 0.6;

    if ( viewState.perspectiveEnabled)
    {
      if ( actualClipFar < (MIN_CLIP_SIZE * approxSize) )
      {
        // model is off the screen, so just grab a chunk of nothing close to the lens
        actualClipFar = 2.0 * MIN_CLIP_SIZE * approxSize;
        actualClipNear = MIN_CLIP_SIZE * approxSize;
      }
      else
      {
        // can see at least part of the model...
        if ( (viewState.clipPerspNear * approxSize) >
           (actualClipFar - approxSize * MIN_CLIP_SIZE) )
        {
          // umm, seems like the near may be furthur than the far; don't do that.
          actualClipNear = actualClipFar - approxSize * MIN_CLIP_SIZE;
        }
        else
        {
          // model on screen, and clip near is in a good spot...
          actualClipNear = distToObj - approxSize * 0.6;

          if ( actualClipNear < (viewState.clipPerspNear * approxSize) )
          {
            // start cutting into object
            actualClipNear = viewState.clipPerspNear * approxSize;
          }

          if ( actualClipNear < (approxSize * MIN_CLIP_SIZE) )
          {
            // make sure that near is not closer than it should be
            actualClipNear = approxSize * MIN_CLIP_SIZE;
          }
        }
      }
    }
    else
    {
      actualClipNear = distToObj - approxSize * 0.6;
      actualClipNear += viewState.clipOrthoNear * approxSize * 1.2;

      if (distToObj < MIN_CLIP_SIZE)
        distToObj = MIN_CLIP_SIZE;
    }

    newProjection = true; // the rest is done is display
  }
}

/**
    @brief Handles any keyboard events.
    @param keyEvent  the keys that were pressed.
  */
void GLRenderer::keyPressEvent(QKeyEvent *keyEvent){
  enum MouseFunction { MouseOrbit, MouseOrbitPerp, MouseWalk, MouseSlide,
           MouseRotate, MouseRotatePerp, MouseOther };
  MouseFunction mouseFunction = MouseOrbit;
  GLfloat tmp1, tmp2;
  int     tmp3;
  GLfloat temp1[3], temp2[3], temp3[3];
  GLfloat tmpRot[4];

  //Pressing direction keys rotates the object towards the direction given
  int dx=0, dy=0;
  if(keyEvent->key()==Key_Up)
    dy=10;
  else if(keyEvent->key()==Key_Down)
    dy=-10;
  if(keyEvent->key()==Key_Right)
    dx=10;
  else if(keyEvent->key()==Key_Left)
    dx=-10;

  //buttonState's value is Shift key, Control key (Command key for Mac), and Alt key OR'ed together.
  KeyboardModifiers keyModifierState = keyEvent->modifiers();
  int key = keyEvent->key();
  GLfloat sensitivity = 1.0;
  //Check if Control (Command key for Mac) or Shift button was pressed and adjust sensitivity as described
  if (keyModifierState & ControlModifier)
    sensitivity *= 0.1;
  if (keyModifierState & ShiftModifier)
    sensitivity *= 4.0;

  mouseFunction = MouseOrbit;
  switch (key)
  {
    case (Key_Home):
      dy=-20;
      mouseFunction = MouseWalk;
      break;
    case(Key_End):
      dy=20;
      mouseFunction = MouseWalk;
      break;
    case (Key_Slash):
      mouseFunction = MouseSlide;
      dx=-10;
      break;
    case (Key_Asterisk):
      mouseFunction = MouseSlide;
      dx=10;
      break;
    case (Key_Minus):
      mouseFunction = MouseSlide;
      dy=10;
      break;
    case (Key_Plus):
      mouseFunction = MouseSlide;
      dy=-10;
      break;
    case (Key_PageUp):
      mouseFunction = MouseRotatePerp;
      dy=1;
      break;
    case (Key_PageDown):
      mouseFunction = MouseRotatePerp;
      dy=-1;
      break;
    default:
      break;
  }
  switch (mouseFunction)
  {
    case (MouseOrbit):
      vcopy3(temp1, viewState.cameraUp);
      // will be unit length if 90deg and unit lengths
      vcross(temp2, viewState.cameraAim, viewState.cameraUp);
      vscale( temp1, (-dy) );
      vscale( temp2, (-dx) );
      vadd(temp1, temp1, temp2);
      // temp1 now holds a vector in the direction of the movement;
      // need to find axis of rotation now
      vcross(temp2, temp1, viewState.cameraAim);
      if (glWindowWidth > glWindowHeight)
        tmp3 = glWindowHeight;
      else
        tmp3 = glWindowWidth;
      tmpRot[0] = vlength(temp1) * 360.0 / tmp3 * sensitivity;
      vcopy3( (tmpRot+1), temp2);

      scene->pos(temp2);
      cameraOrbit(tmpRot, temp2);
      break;
    case (MouseWalk):
      vcopy3(temp1, viewState.cameraAim);
      scene->halfSize(temp2);
      tmp1 = vlength(temp2);
      if (!tmp1)
      tmp1 = 1.0;
      // This looks wierd, but works so that different FOVs seem to scale the same.
      vscale( temp1, ( tmp1 / tan(fov() * M_PI / 360.0) * (-dy) /
             glWindowHeight * sensitivity) );
      cameraTranslate(temp1);
      break;
    case (MouseSlide):
      vcopy3(temp1, viewState.cameraUp);
      vcross(temp2, viewState.cameraAim, viewState.cameraUp); // will be unit length if 90deg and unit lengths
      scene->pos(temp3);
      vsub(temp3, temp3, viewState.cameraPos);
      tmp1 = vlength(temp3);
      scene->halfSize(temp3);
      tmp2 = vlength(temp3) * MIN_CLIP_SIZE;
      if ( tmp1 < tmp2 )
      tmp1 = tmp2;
      tmp1 *= 2.0 * tan(fov() * M_PI / 360.0) / glWindowHeight;
      vscale( temp1, (tmp1 * (dy) * sensitivity) );
      vscale( temp2, (tmp1 * (-dx) * sensitivity) );
      vadd(temp1, temp1, temp2);
      cameraTranslate(temp1);
      break;
    case (MouseRotatePerp):
      tmpRot[0] = 5 * dy * sensitivity; //move 5 deg
      vcopy3( (tmpRot+1), viewState.cameraAim);
      cameraRotate(tmpRot);
      break;
    default:
      break;
  }
  updateGL();
}

/**
    @brief Handles any mouse press events.
    @param mouseEvent  the mouse buttons that were clicked
  */
void GLRenderer::mousePressEvent(QMouseEvent *mouseEvent)
{
  QGLWidget::mousePressEvent(mouseEvent);

  mouseDragging = true;

  oldMouseX = mouseEvent->x();
  oldMouseY = mouseEvent->y();
}

/**
    @brief Handles any mouse release events.
  */
void GLRenderer::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
  QGLWidget::mouseReleaseEvent(mouseEvent);

  if ( (mouseEvent->modifiers() & MouseButtonMask) == mouseEvent->button() )
  {
    mouseDragging = false;

    if (glrendererOptions.drawBoxWhileMoving)
      updateGL();
  }
}

/**
    @brief Handles any mouse move events.
  */
void GLRenderer::mouseMoveEvent(QMouseEvent *mouseEvent)
{
  QGLWidget::mouseMoveEvent(mouseEvent);
  enum MouseFunction { MouseOrbit, MouseOrbitPerp, MouseWalk, MouseSlide,
                             MouseRotate, MouseRotatePerp, MouseOther };
  MouseFunction mouseFunction;

  GLfloat tmp1, tmp2;
  int     tmp3;
  GLfloat temp1[3], temp2[3], temp3[3];
  GLfloat tmpRot[4];

  int newMouseX = mouseEvent->x();
  int newMouseY = mouseEvent->y();

  //buttonState's value is LeftButton, RightButton, and MidButton bitwise-OR'ed together.
  MouseButtons buttonState = mouseEvent->buttons();
  //keyState is ShiftModifer, ControlModifier (Command key for Mac), and AltModifier bitwise-OR'ed together.
  KeyboardModifiers keyState = mouseEvent->modifiers();

  GLfloat sensitivity = 1.0;

  if (keyState & ControlModifier)
    sensitivity *= 0.1;

  if (glrendererOptions.useThreeButtons)
  {
    if (keyState & ShiftModifier)
      sensitivity *= 10.0;

    switch (buttonState)
    {
      case (LeftButton):
        mouseFunction = MouseOrbit;
        break;
      case ((int)LeftButton | (int)MidButton):
        mouseFunction = MouseOrbitPerp;
        break;
      case (MidButton):
        mouseFunction = MouseWalk;
        break;
      case ((int) MidButton | (int)RightButton):
        mouseFunction = MouseSlide;
        break;
      case (RightButton):
        mouseFunction = MouseRotate;
        break;
      case ((int)RightButton | (int)LeftButton):
        mouseFunction = MouseRotatePerp;
        break;
      default: // could have all three buttons down...
        mouseFunction = MouseOther; // set to the ignored state
        break;
    }
  }
  else //using two button mouse
  {
    // use shift as the 3rd functional mouse button
    switch (buttonState)// & (MouseButtonMask | ShiftButton))
    {
      case (LeftButton):
        mouseFunction = MouseOrbit;
        break;
      case ((int)LeftButton | (int)ShiftModifier):
        mouseFunction = MouseOrbitPerp;
        break;
      case (MidButton):
      case ((int)LeftButton | (int)RightButton): // in case no mouse chord
        mouseFunction = MouseWalk;
        break;
      case ((int)MidButton | (int)ShiftModifier):
      case ((int)LeftButton | (int)RightButton | (int)ShiftModifier): // in case no mouse chord
        mouseFunction = MouseSlide;
        break;
      case (RightButton):
        mouseFunction = MouseRotate;
        break;
      case ((int)RightButton | (int)ShiftModifier):
        mouseFunction = MouseRotatePerp;
        break;
      default: // could have other combinations...
        mouseFunction = MouseOther; // set to the ignored state
        break;
    }
  }
  switch (mouseFunction)
  {
    case (MouseOrbit):
      vcopy3(temp1, viewState.cameraUp);
      // will be unit length if 90deg and unit lengths
      vcross(temp2, viewState.cameraAim, viewState.cameraUp);

      vscale( temp1, (newMouseY-oldMouseY) );
      vscale( temp2, (oldMouseX-newMouseX) );

      vadd(temp1, temp1, temp2);
      // temp1 now holds a vector in the direction of the movement;
      // need to find axis of rotation now

      vcross(temp2, temp1, viewState.cameraAim);

      if (glWindowWidth > glWindowHeight)
        tmp3 = glWindowHeight;
      else
        tmp3 = glWindowWidth;

      tmpRot[0] = vlength(temp1) * 360.0 / tmp3 * sensitivity;
      vcopy3( (tmpRot+1), temp2);
      scene->pos(temp2);
      cameraOrbit(tmpRot, temp2);
    break;

    case (MouseOrbitPerp):
      temp1[0] = oldMouseX - (glWindowWidth/2);
      temp1[1] = (glWindowHeight/2) - oldMouseY;
      temp1[2] = 0;

      temp2[0] = newMouseX - (glWindowWidth/2);
      temp2[1] = (glWindowHeight/2) - newMouseY;
      temp2[2] = 0;

      // Now to find out the angle between the two vectors...
      tmp1 = sqrt(temp1[0]*temp1[0] + temp1[1]*temp1[1]) *
             sqrt(temp2[0]*temp2[0] + temp2[1]*temp2[1]);

      if (tmp1) // make sure the divisor is not 0
      {
        tmp1 = (temp1[0]*temp2[0] + temp1[1]*temp2[1]) / tmp1;

        tmp1 = acos(tmp1) * 180.0 / M_PI;

        vcross(temp3, temp1, temp2);
        if (temp3[2] < 0)
          tmp1 *= -1.0;

        tmpRot[0] = tmp1 * sensitivity;
        vcopy3( (tmpRot+1), viewState.cameraAim);
        scene->pos(temp2);
        cameraOrbit(tmpRot, temp2);
      }
    break;

    case (MouseWalk):
      vcopy3(temp1, viewState.cameraAim);

      scene->halfSize(temp2);
      tmp1 = vlength(temp2);
      if (!tmp1)
        tmp1 = 1.0;

      // This looks wierd, but works so that different FOVs seem to scale the same.
      vscale( temp1, ( tmp1 / tan(fov() * M_PI / 360.0) * (oldMouseY-newMouseY) /
               glWindowHeight * sensitivity) );

      cameraTranslate(temp1);

    break;

    case (MouseSlide):
      vcopy3(temp1, viewState.cameraUp);
      vcross(temp2, viewState.cameraAim, viewState.cameraUp); // will be unit length if 90deg and unit lengths

      scene->pos(temp3);
      vsub(temp3, temp3, viewState.cameraPos);
      tmp1 = vlength(temp3);
      scene->halfSize(temp3);
      tmp2 = vlength(temp3) * MIN_CLIP_SIZE;
      if ( tmp1 < tmp2 )
        tmp1 = tmp2;
      tmp1 *= 2.0 * tan(fov() * M_PI / 360.0) / glWindowHeight;

      vscale( temp1, (tmp1 * (newMouseY-oldMouseY) * sensitivity) );
      vscale( temp2, (tmp1 * (oldMouseX-newMouseX) * sensitivity) );

      vadd(temp1, temp1, temp2);

      cameraTranslate(temp1);

    break;

    case (MouseRotate):
      vcopy3(temp1, viewState.cameraUp);
      vcross(temp2, viewState.cameraAim, viewState.cameraUp); // will be unit length if 90deg and unit lengths

      vscale( temp1, (newMouseY-oldMouseY) );
      vscale( temp2, (oldMouseX-newMouseX) );
      vadd(temp1, temp1, temp2);
      // temp1 now holds a vector in the direction of the movement; need to find axis of rotation now

      vcross(temp2, viewState.cameraAim, temp1);

      if (glWindowWidth > glWindowHeight)
        tmp3 = glWindowHeight;
      else
        tmp3 = glWindowWidth;

      tmpRot[0] = vlength(temp1) * 90.0 / tmp3 * sensitivity;
      vcopy3( (tmpRot+1), temp2);

      cameraRotate(tmpRot);

    break;

    case (MouseRotatePerp):
      temp1[0] = oldMouseX - (glWindowWidth/2);
      temp1[1] = (glWindowHeight/2) - oldMouseY;
      temp1[2] = 0;

      temp2[0] = newMouseX - (glWindowWidth/2);
      temp2[1] = (glWindowHeight/2) - newMouseY;
      temp2[2] = 0;

      // Now to find out the angle between the two vectors...
      tmp1 = sqrt(temp1[0]*temp1[0] + temp1[1]*temp1[1]) * sqrt(temp2[0]*temp2[0] + temp2[1]*temp2[1]);

      if (tmp1) // make sure the divisor is not 0
      {
        tmp1 = (temp1[0]*temp2[0] + temp1[1]*temp2[1]) / tmp1;

        tmp1 = acos(tmp1) * 180.0 / M_PI;

        vcross(temp3, temp1, temp2);
        if (temp3[2] < 0)
          tmp1 *= -1.0;

        tmpRot[0] = tmp1 * sensitivity;
        vcopy3( (tmpRot+1), viewState.cameraAim);

        cameraRotate(tmpRot);
      }
    break;

    default:
    break;
  }
  oldMouseX = newMouseX;
  oldMouseY = newMouseY;
  updateGL();
}

/**
    @brief Handles any mouse scroll event.
  */
void GLRenderer::wheelEvent(QWheelEvent *wheelEvent)
{
  wheelEvent->accept();
// having parent class process event only accomplishes wheelEvent->ignore(),
// which is exactly what I don't want.
//  QGLWidget::wheelEvent(wheelEvent);

  GLfloat tmp1;
  GLfloat temp1[3], temp2[3];

  KeyboardModifiers buttonState = wheelEvent->modifiers();

  GLfloat sensitivity = 1.0;

  if (buttonState & ControlModifier)
    sensitivity *= 0.1;

  if (buttonState & ShiftModifier)
    sensitivity *= 10.0;

  vcopy3(temp1, viewState.cameraAim);

  scene->halfSize(temp2);
  tmp1 = vlength(temp2);
  if (!tmp1)
    tmp1 = 1.0;

  // This looks weird, but works so that different FOVs seem to scale the same.
  vscale( temp1, ( tmp1 / tan(fov() * M_PI / 360.0) * wheelEvent->delta() /
           glWindowHeight * 0.15 * sensitivity) );

  cameraTranslate(temp1);

  updateGL();
}

/**
    @brief Initializes the GL object.
  */
void GLRenderer::initializeGL()
{
  GLfloat lightPosition[4] = {0.0F, 0.0F, 1.0F, 0.0F};

  glPixelStorei(GL_UNPACK_ALIGNMENT, BYTE_ALIGNMENT);
  glPixelStorei(GL_PACK_ALIGNMENT, BYTE_ALIGNMENT);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);
  glEnable(GL_DITHER);

  if (renderingPixmap) // a Qt hack since it decided to use back buffers for pixmaps
  {
    glReadBuffer(GL_FRONT);
    glDrawBuffer(GL_FRONT);
  }

  // reset sphere lists (need to do so for pixmap rendering)
  solidSphereList = 0;
  wireSphereList = 0;

  // now to setup the noraml GL state from what the variables hold

  glLoadIdentity();

  { // Two Sided
    if (viewState.twoSidedEnabled)
    {
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
      glDisable(GL_CULL_FACE);
    }
    else
    {
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
      glEnable(GL_CULL_FACE);
    }
  }
  { // Smooth Shaded
    if (viewState.smoothShadedEnabled)
      glShadeModel(GL_SMOOTH);
    else
      glShadeModel(GL_FLAT);
  }
  // transparency set has no GL calls
  // antialias set has no GL calls

  { // Sphere Tesselation
    setSphereTessQuietForce(viewState.sphereTess);
  }

  // outline set has no GL calls
  { // Wireframe
    if (viewState.wireframeEnabled)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  }
  // opaqueWireframe set has no GL calls
  { // Line Width
    glLineWidth(viewState.lineWidth);
    glPointSize(viewState.lineWidth);
  }
  // outlineColor has no GL calls

  { // Light
    if (viewState.lightEnabled)
      glEnable(GL_LIGHT0);
    else
      glDisable(GL_LIGHT0);
  }
  { // Lighting
    if (viewState.lightingEnabled)
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);
  }
  { // Ambient
    GLfloat tempAmb[4] = {viewState.ambient, viewState.ambient,
                viewState.ambient, 1.0F};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, tempAmb);
  }
  { // Diffuse
    GLfloat tempDiff[4] = {viewState.diffuse, viewState.diffuse,
                 viewState.diffuse, 1.0F};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, tempDiff);
  }
  { // Background
    glClearColor(viewState.background, viewState.background,
           viewState.background, 1.0);
  }
  { // Specular shininess
    //GLfloat mat_specular[] = {1.0F, 1.0F, 1.0F, 1.0F};
    //glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, 20.0F);
  }
  { // Fog, see pg. 239-244 Red Book
    if (viewState.fogEnabled)
      glEnable(GL_FOG);
    else
      glDisable(GL_FOG);
    float bg = viewState.background;
    GLfloat fogColor[4] = {bg, bg, bg, 1.0};
    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogfv(GL_FOG_COLOR, fogColor);
//    glFogf(GL_FOG_DENSITY, viewState.fogLevel); //no effect in linear mode
    glHint(GL_FOG_HINT, GL_DONT_CARE);
//TODO sets the fog depths, need to call these once in redraw()
    if(scene && scene->scenes()){
      glFogf(GL_FOG_START,7*viewState.fogLevel*scene->largestExtremUnit());
      glFogf(GL_FOG_END,(7*viewState.fogLevel+.4)*scene->largestExtremUnit());
    }
  }

  // perspective has no GL calls (need to call updateProjection() though)
  // FOV has no GL calls (need to call updateProjection() though)
  // camera info has no GL calls (need to call updateProjection() though)
  // clipping has no GL calls (need to call updateProjection() though)

  // gamma has no GL calls
  // gamma level has no GL calls

  // bin window has no GL calls
  // bin window index has no GL calls

  updateProjection();
}

int GLRenderer::getInitError()
{
  int toReturn = GNoError;

  if (!isValid())
    toReturn |= GErrorNoRendContext;
  if (!format().doubleBuffer())
    toReturn |= GErrorDubBuff;
  if (!format().depth())
    toReturn |= GErrorDephBuff;
  if (!format().rgba())
    toReturn |= GErrorRGBABuff;
  if (!format().accum())
    toReturn |= GErrorAccumBuff;

  return toReturn;
}

void GLRenderer::resizeGL(int w, int h)
{
  // have to watch out for line width offset
  GLint halfLine = (GLint) ceil(viewState.lineWidth/2.0);

  if (renderingPixmap)
  {
    glWindowWidth = w;
    glWindowHeight = h;
  }
  else
  {
    QDesktopWidget *desktop = QApplication::desktop();
    int desktopWidth = desktop->width();
    int desktopHeight = desktop->height();

    GLsizei maxGlWindowWidth = desktopWidth - 2 * halfLine;
    GLsizei maxGlWindowHeight = desktopHeight - 2 * halfLine;

    // Just make sure the what we're trying to render is now larger than
    // the screen. Buffers may not be larger than the screen (e.g. in hardware),
    // so don't risk problems.
    glWindowWidth  = (w > maxGlWindowWidth ) ? maxGlWindowWidth  : w;
    glWindowHeight = (h > maxGlWindowHeight) ? maxGlWindowHeight : h;

    if ( (glWindowWidth != cacheImageWidth) || (glWindowHeight != cacheImageHeight) )
    {
      cacheImageLineLength = alignedLength(3 * glWindowWidth, sizeof(GLubyte));
      cacheImageWidth = glWindowWidth;
      cacheImageHeight = glWindowHeight;

      if (glrendererOptions.useCachedImage)
      {
        try
        {
          if (cacheImageData)
            delete[] cacheImageData; // clean up the old data

          cacheImageData = new GLubyte[cacheImageLineLength * cacheImageHeight];
        }
        catch (...)
        {
          cacheImageData = 0;
          qWarning("Could not allocate array for additional buffer.");
        }
      }

      cachedImageExists = false;
    }
  }

  glViewport(-halfLine, -halfLine, (GLsizei) (glWindowWidth + 2 * halfLine),
         (GLsizei) (glWindowHeight + 2 * halfLine));

  newProjection = true; // paintGL() will figure out what to do
}

void GLRenderer::paintGL() //protected inherited from qglwidget
{
  if (scene) // make sure there's something to draw first...
  {
    if (cachedImageExists /*&& !renderingPixmap*/) // cachedImageExists depends on glrendererOptions.useCachedImage
    {
      bool lastLighting;
      GLint halfLine = (GLint) ceil(viewState.lineWidth/2.0);

      glMatrixMode(GL_PROJECTION);
      glPushMatrix();
      glLoadIdentity();
      gluOrtho2D((-halfLine), (GLdouble) (cacheImageWidth + halfLine),
                 (-halfLine), (GLdouble) (cacheImageHeight + halfLine));
      glMatrixMode(GL_MODELVIEW);

      glDisable(GL_DITHER);
      glDisable(GL_DEPTH_TEST);
      lastLighting = lighting(); //remember if lighting was enabled
      setLightingQuiet(false);

      glLoadIdentity();
      glRasterPos2i(0, 0);
      glDrawPixels(cacheImageWidth, cacheImageHeight, GL_RGB,
             GL_UNSIGNED_BYTE, cacheImageData);

      glEnable(GL_DITHER);
      glEnable(GL_DEPTH_TEST);
      setLightingQuiet(lastLighting); //restore last lighting mode

      glMatrixMode(GL_PROJECTION);
      glPopMatrix();
      glMatrixMode(GL_MODELVIEW);
    }
    else
    {
      QTime drawTimer; // to see if it's worthwhile caching the image

      if (newProjection) // update projection if needed (turned off in DrawAll)
      {
        if (viewState.perspectiveEnabled)
        {
          currentProjection = myPerspective(fov(),
                            ((GLdouble) glWindowWidth / (GLdouble) glWindowHeight),
                            actualClipNear, actualClipFar);
        }
        else
        {
          currentProjection = myOrthoFov(fov(), distToObj,
                           ((GLdouble) glWindowWidth / (GLdouble) glWindowHeight),
                           actualClipNear, actualClipFar);
        }
      }

      if ( glrendererOptions.useCachedImage && !renderingPixmap &&
         !(glrendererOptions.drawBoxWhileMoving && mouseDragging) )
        drawTimer.start();

      drawAll(currentProjection);

      if ( glrendererOptions.useCachedImage && !renderingPixmap &&
         !(glrendererOptions.drawBoxWhileMoving && mouseDragging) &&
         (drawTimer.elapsed() > TIME_UNTIL_CACHE) && cacheImageData )
      {
        glReadPixels(0, 0, cacheImageWidth, cacheImageHeight, GL_RGB,
               GL_UNSIGNED_BYTE, cacheImageData);

        cachedImageExists = true;
      }
    }
  }
  else
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }
}

QPixmap GLRenderer::renderPixmap(int w, int h, bool useContext)
{
  bool lastCachedImageExists = cachedImageExists; // set to false later on
  GLuint lastSolidSphereList = solidSphereList;
  GLuint lastWireSphereList = wireSphereList;

  renderingPixmap = true;
    scene->useDL(false);
  QPixmap toReturn = QGLWidget::renderPixmap(w, h, useContext);
    scene->useDL(true);
  renderingPixmap = false;

  // need to put previous state back that the rendering changed
  makeCurrent();
  resizeGL(width(), height());

  //  GLboolean* buffer;
  //  glGetBooleanv( GL_DRAW_BUFFER, buffer );
  //  glDrawBuffer( (*buffer==GL_FRONT) ? GL_BACK : GL_FRONT );
  glDrawBuffer( GL_BACK );

  // since sphere lists aren't 'registered', just put back the 'right' value.
  solidSphereList = lastSolidSphereList;
  wireSphereList = lastWireSphereList;
  cachedImageExists = lastCachedImageExists;

  return toReturn;
}

void GLRenderer::drawLightSources(){
  //draw as point sources
  glPointSize(15);
  cuv_float objPos[3];
  scene->pos(objPos);
  float size = scene->largestExtremUnit();
  for(int i=0;i<viewState.lightSources;i++){
    glBegin(GL_POINTS);
    float color = (viewState.background>0.5)?0:1;
    glColor3f(color,color,color);
    glVertex3f(objPos[0]+0.6*size*(viewState.lightXTranslate[i]),
               objPos[1]+0.6*size*(viewState.lightYTranslate[i]),
               objPos[2]+0.6*size*(viewState.lightZTranslate[i]-1));
    glEnd();
  }
  glPointSize(viewState.lineWidth);
}

/**
    @brief Draws a wireframe box with dimensions matching the object
  */
void GLRenderer::drawBoxModel()
{
  cuv_float objPos[3];
  cuv_float objHalfSize[3];

  scene->pos(objPos);
  scene->halfSize(objHalfSize);

  bool lastTwoSided = twoSided();
  bool lastSmoothShaded = smoothShaded();
  bool lastWireframe = wireframe();
  bool lastLighting = lighting();

  setTwoSidedQuiet(true);
  setSmoothShadedQuiet(true);
  setWireframeQuiet(true);
  setLightingQuiet(false);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(scene->getEditMode())
    drawLightSources();

  glBegin(GL_QUADS);
  //draw top
  glColor3f(0.0, 0.0, 0.0);
  glVertex3f(objPos[0]-objHalfSize[0], objPos[1]-objHalfSize[1], objPos[2]-objHalfSize[2]);
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(objPos[0]-objHalfSize[0], objPos[1]+objHalfSize[1], objPos[2]-objHalfSize[2]);
  glColor3f(1.0, 1.0, 0.0);
  glVertex3f(objPos[0]+objHalfSize[0], objPos[1]+objHalfSize[1], objPos[2]-objHalfSize[2]);
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(objPos[0]+objHalfSize[0], objPos[1]-objHalfSize[1], objPos[2]-objHalfSize[2]);
  //draw bottom
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(objPos[0]-objHalfSize[0], objPos[1]-objHalfSize[1], objPos[2]+objHalfSize[2]);
  glColor3f(1.0, 0.0, 1.0);
  glVertex3f(objPos[0]+objHalfSize[0], objPos[1]-objHalfSize[1], objPos[2]+objHalfSize[2]);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(objPos[0]+objHalfSize[0], objPos[1]+objHalfSize[1], objPos[2]+objHalfSize[2]);
  glColor3f(0.0, 1.0, 1.0);
  glVertex3f(objPos[0]-objHalfSize[0], objPos[1]+objHalfSize[1], objPos[2]+objHalfSize[2]);

  glEnd();
/*
//draw top
GLfloat top[] = { objPos[0]-objHalfSize[0], objPos[1]-objHalfSize[1], objPos[1]-objHalfSize[2],
                  objPos[0]-objHalfSize[0], objPos[1]+objHalfSize[1], objPos[1]-objHalfSize[2],
                  objPos[0]+objHalfSize[0], objPos[1]+objHalfSize[1], objPos[1]-objHalfSize[2],
                  objPos[0]+objHalfSize[0], objPos[1]-objHalfSize[1], objPos[1]-objHalfSize[2] };
GLfloat topColour[] = { 0.0, 0.0, 0.0,
                        0.0, 1.0, 0.0,
                        1.0, 1.0, 0.0,
                        1.0, 0.0, 0.0 };
//draw bottom
GLfloat bottom[] = { objPos[0]-objHalfSize[0], objPos[1]-objHalfSize[1], objPos[1]+objHalfSize[2],
                     objPos[0]+objHalfSize[0], objPos[1]-objHalfSize[1], objPos[1]+objHalfSize[2],
                     objPos[0]+objHalfSize[0], objPos[1]+objHalfSize[1], objPos[1]+objHalfSize[2],
                     objPos[0]-objHalfSize[0], objPos[1]+objHalfSize[1], objPos[1]+objHalfSize[2] };
GLfloat bottomColour[] = { 0.0, 0.0, 1.0,
                           1.0, 0.0, 1.0,
                           1.0, 1.0, 1.0,
                           0.0, 1.0, 1.0 };

QGLBuffer shaderProgram;
QGLBuffer vertexBuffer(QGLBuffer::VertexBuffer);
QGLBuffer colourBuffer(QGLBuffer::VertexBuffer);

vertexBuffer.create();
vertexBuffer.setUsagePattern( QGLBuffer::StaticDraw );
colourBuffer.create();
colourBuffer.setUsagePattern( QGLBuffer::StaticDraw );

if (!vertexBuffer.bind()) {
  qWarning("Could not bind vertex buffer to the context";
  return;
}
if (!colourBuffer.bind()) {
  qWarning("Could not bind vertex buffer to the context";
  return;
}
vertexBuffer.allocate(top, sizeof(top));
colourBuffer.allocate(topColour, sizeof(topColour));
if (shaderProgram.bind()) {
  qWarning("Could not bind shader program to context");
  return;
}

shaderProgram.setAttributeBuffer("vertex", GL_FLOAT, 0, 4);
shaderProgram.setAttributeBuffer("colour", GL_FLOAT, 0, 4);

glDrawArrays(GL_LINE_LOOP, 0, 4);
*/
  //draw corners
  glBegin(GL_LINES);

  glColor3f(0.0, 0.0, 0.0);
  glVertex3f(objPos[0]-objHalfSize[0], objPos[1]-objHalfSize[1], objPos[2]-objHalfSize[2]);
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(objPos[0]-objHalfSize[0], objPos[1]-objHalfSize[1], objPos[2]+objHalfSize[2]);

  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(objPos[0]+objHalfSize[0], objPos[1]-objHalfSize[1], objPos[2]-objHalfSize[2]);
  glColor3f(1.0, 0.0, 1.0);
  glVertex3f(objPos[0]+objHalfSize[0], objPos[1]-objHalfSize[1], objPos[2]+objHalfSize[2]);

  glColor3f(1.0, 1.0, 0.0);
  glVertex3f(objPos[0]+objHalfSize[0], objPos[1]+objHalfSize[1], objPos[2]-objHalfSize[2]);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(objPos[0]+objHalfSize[0], objPos[1]+objHalfSize[1], objPos[2]+objHalfSize[2]);

  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(objPos[0]-objHalfSize[0], objPos[1]+objHalfSize[1], objPos[2]-objHalfSize[2]);
  glColor3f(0.0, 1.0, 1.0);
  glVertex3f(objPos[0]-objHalfSize[0], objPos[1]+objHalfSize[1], objPos[2]+objHalfSize[2]);

  glEnd();

/*
//draw corners
GLfloat lines[] = { objPos[0]-objHalfSize[0], objPos[1]-objHalfSize[1], objPos[1]-objHalfSize[2],
                    objPos[0]-objHalfSize[0], objPos[1]-objHalfSize[1], objPos[1]+objHalfSize[2],
                    objPos[0]+objHalfSize[0], objPos[1]-objHalfSize[1], objPos[1]-objHalfSize[2],
                    objPos[0]+objHalfSize[0], objPos[1]-objHalfSize[1], objPos[1]+objHalfSize[2],
                    objPos[0]+objHalfSize[0], objPos[1]+objHalfSize[1], objPos[1]-objHalfSize[2],
                    objPos[0]+objHalfSize[0], objPos[1]+objHalfSize[1], objPos[1]+objHalfSize[2],
                    objPos[0]-objHalfSize[0], objPos[1]+objHalfSize[1], objPos[1]-objHalfSize[2],
                    objPos[0]-objHalfSize[0], objPos[1]+objHalfSize[1], objPos[1]+objHalfSize[2]  };
GLfloat vertexColour[] = { 0.0, 0.0, 0.0,
                           0.0, 0.0, 1.0,
                           1.0, 0.0, 0.0,
                           1.0, 0.0, 1.0,
                           1.0, 1.0, 0.0,
                           1.0, 1.0, 1.0,
                           0.0, 1.0, 0.0,
                           0.0, 1.0, 1.0, };

glDrawArrays(GL_LINES, 0, 4);
*/

  setTwoSidedQuiet(lastTwoSided);
  setSmoothShadedQuiet(lastSmoothShaded);
  setWireframeQuiet(lastWireframe);
  setLightingQuiet(lastLighting);
}

void GLRenderer::drawAll(proj_info project)
{
  static const int JITTER_SIZE = 8;
  static const int ACCUM_AMT = JITTER_SIZE + 1; // since it will use {0.0, 0.0}
  static const GLdouble jitter[JITTER_SIZE][2]
    = { { 0.4,  0.0}, {-0.4,  0.0}, { 0.0,  0.4}, { 0.0, -0.4},
      { 0.5,  0.5}, {-0.5, -0.5}, { 0.5, -0.5}, {-0.5,  0.5} }; // offsets in pixels
  GLdouble projHorizAdj = (project.right - project.left) /
    (GLdouble) glWindowWidth * (GLint) ceil(viewState.lineWidth/2.0);
  GLdouble projVertAdj = (project.top - project.bottom) /
    (GLdouble) glWindowHeight * (GLint) ceil(viewState.lineWidth/2.0);

  // adjust so that lines can always be visible
  project.left -= projHorizAdj;
  project.right += projHorizAdj;
  project.bottom -= projVertAdj;
  project.top += projVertAdj;

  if (newProjection) // update projection if needed
  {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (viewState.perspectiveEnabled)
      myFrustum(project);
    else
      myOrtho(project);

    glMatrixMode(GL_MODELVIEW);

    newProjection = false;
  }

  readyCameraForDrawing();

  if(glrendererOptions.drawBoxWhileMoving && mouseDragging){
    //don't draw scene, this is done to perform camera movement quickly
    drawBoxModel();
    drawAxis();
    if(viewState.showLightPosition)
      drawLightSources();
  }
  else
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(glrendererOptions.drawBoxAlways || scene->getEditMode())
      drawBoxModel();
    if(viewState.showLightPosition)
      drawLightSources();
    drawScene(); // Draw everything

    if (viewState.antialiasEnabled) // if antialiased (fix postProcess() if changed)
    {
      glAccum(GL_LOAD, 1.0F/ACCUM_AMT); // add current image to accum buffer

      for (int i = 0 ; i < JITTER_SIZE ; i++) // render a few times
      {
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();

        if (viewState.perspectiveEnabled)
          myFrustum(project, jitter[i][0], jitter[i][1]);
        else
          myOrtho(project, jitter[i][0], jitter[i][1]);

        glMatrixMode(GL_MODELVIEW);

        drawScene();

        glAccum(GL_ACCUM, 1.0F/ACCUM_AMT); // accumulate the images
      }

      glAccum(GL_RETURN, 1.0F); // return the image to the framebuffer

      glMatrixMode(GL_PROJECTION); // now set everything back to normal (unjittered)
      glLoadIdentity();

      if (viewState.perspectiveEnabled)
        myFrustum(project);
      else
        myOrtho(project);

      glMatrixMode(GL_MODELVIEW);
    }
  }
}

void GLRenderer::readyCameraForDrawing()
{
  GLfloat cameraEye[3];
  GLfloat cameraDist = vlength(viewState.cameraPos);

  glLoadIdentity();

  vcopy3(cameraEye, viewState.cameraAim);
  if (cameraDist > 1.0)
    vscale(cameraEye, cameraDist); // make sure that the Aim is significant
  cameraEye[0] += viewState.cameraPos[0];
  cameraEye[1] += viewState.cameraPos[1];
  cameraEye[2] += viewState.cameraPos[2];

  gluLookAt(viewState.cameraPos[0], viewState.cameraPos[1], viewState.cameraPos[2],
        cameraEye[0], cameraEye[1], cameraEye[2],
        viewState.cameraUp[0], viewState.cameraUp[1], viewState.cameraUp[2]);
}

void GLRenderer::drawScene()
{
  bool preprocessLighting = false; // turn off compiler complaining

  bool lastTwoSided;
  bool lastSmoothShaded;
  bool lastWireframe;
  bool lastLighting;

  unsigned int depthLineLength = 0;
  GLfloat *depthRefData = 0;

  if (binWindow())
  {
    preprocessLighting = lighting();
    setLightingQuiet(false);
    glDisable(GL_DITHER);
  }

  // need to draw all the depth first, then draw the wireframe as usual
  if (wireframe() && opaqueWireframe())
  {
    int origBuffer; // may change depending if rendering, etc. Be safe.
    glGetIntegerv(GL_DRAW_BUFFER, &origBuffer);

    glDrawBuffer(GL_NONE);

    setWireframeQuiet(false);

    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(POLY_OFF_FACTOR * lineWidth(), POLY_OFF_UNITS);

    if (scene->opaqSolidExists())
      scene->drawOpaqSolid();

    if (scene->transSolidExists())
      scene->drawTransSolid();

    glDisable(GL_POLYGON_OFFSET_FILL);

    setWireframeQuiet(true);

    glDrawBuffer((GLenum) origBuffer);

    if (binWindow()) // need to get a reference value for depth since 1.0F cannot be assumed now
    {
      // depthLineLength must match the number gotten in postProcess()
      depthLineLength = alignedLength(glWindowWidth, sizeof(GLfloat));
      depthRefData = new GLfloat[glWindowHeight*depthLineLength];
      glReadPixels(0, 0, glWindowWidth, glWindowHeight, GL_DEPTH_COMPONENT,
             GL_FLOAT, depthRefData);

    }
  }

  if (scene->opaqSolidExists())
  {
    if (wireframe())
    {
      lastTwoSided = twoSided();
      lastLighting = lighting();

      setTwoSidedQuiet(true);
      setLightingQuiet(false);

      scene->drawOpaqSolid();

      setTwoSidedQuiet(lastTwoSided);
      setLightingQuiet(lastLighting);
    }
    else
    {
      scene->drawOpaqSolid();
    }
  }

  if (scene->opaqWireExists())
  {
    lastTwoSided = twoSided();
    lastWireframe = wireframe();
    lastLighting = lighting();

    setTwoSidedQuiet(true);
    setWireframeQuiet(true);
    setLightingQuiet(false);
    scene->drawOpaqWire();

    setTwoSidedQuiet(lastTwoSided);
    setWireframeQuiet(lastWireframe);
    setLightingQuiet(lastLighting);
  }

  if (!wireframe() && outline() && scene->outlineExists())
  {
    lastTwoSided = twoSided();
    lastSmoothShaded = smoothShaded();
    lastWireframe = wireframe();
    lastLighting = lighting();

    setTwoSidedQuiet(true);
    setSmoothShadedQuiet(false);
    setWireframeQuiet(true);
    setLightingQuiet(false);
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-POLY_OFF_FACTOR * lineWidth(), -POLY_OFF_UNITS);

    glColor3fv(viewState.outlineColor);
    scene->drawOutline();

    setTwoSidedQuiet(lastTwoSided);
    setSmoothShadedQuiet(lastSmoothShaded);
    setWireframeQuiet(lastWireframe);
    setLightingQuiet(lastLighting);
    glDisable(GL_POLYGON_OFFSET_LINE);
  }

  if (scene->transSolidExists())
  {
    if (transparency() && !wireframe())
    {
      glEnable(GL_BLEND);
      glDepthMask(GL_FALSE);
    }

    if (wireframe())
    {
      lastTwoSided = twoSided();
      lastLighting = lighting();

      setTwoSidedQuiet(true);
      setLightingQuiet(false);

      scene->drawTransSolid();

      setTwoSidedQuiet(lastTwoSided);
      setLightingQuiet(lastLighting);
    }
    else
    {
      scene->drawTransSolid();
    }

    if (transparency() && !wireframe())
    {
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
    }
  }

  if (scene->transWireExists())
  {
    if (transparency() && !wireframe())
    {
      glEnable(GL_BLEND);
      glDepthMask(GL_FALSE);
    }

    lastTwoSided = twoSided();
    lastWireframe = wireframe();
    lastLighting = lighting();

    setTwoSidedQuiet(true);
    setWireframeQuiet(true);
    setLightingQuiet(false);

    scene->drawTransWire();

    setTwoSidedQuiet(lastTwoSided);
    setWireframeQuiet(lastWireframe);
    setLightingQuiet(lastLighting);

    if (transparency() && !wireframe())
    {
      glDepthMask(GL_TRUE);
      glDisable(GL_BLEND);
    }
  }

  // write depth buffer for trans stuff if needed
  if (binWindow() && transparency() && !wireframe())
  {
    int origBuffer; // may change depending if rendering, etc. Be safe.
    glGetIntegerv(GL_DRAW_BUFFER, &origBuffer);

    glDrawBuffer(GL_NONE);

    if (scene->transSolidExists())
      scene->drawTransSolid();

    if (scene->transWireExists())
    {
      lastTwoSided = twoSided();
      lastWireframe = wireframe();
      lastLighting = lighting();

      setTwoSidedQuiet(true);
      setWireframeQuiet(true);
      setLightingQuiet(false);

      scene->drawTransWire();

      setTwoSidedQuiet(lastTwoSided);
      setWireframeQuiet(lastWireframe);
      setLightingQuiet(lastLighting);
    }

    glDrawBuffer((GLenum) origBuffer);
  }
  scene->setSceneExtrems();
  if ( binWindow() || (gamma() && gammaLevel() != 1.0F) )
  {
    postProcess(depthRefData);
    if (viewState.binWindowEnabled)
    {
      setLightingQuiet(preprocessLighting);
      glEnable(GL_DITHER);
    }
  }

  if (wireframe() && opaqueWireframe() && binWindow())
  {
    delete[] depthRefData;
  }

  Lessa_Text* textscene = 0;
  text *textPtr = 0;
  int tsize = 0;
  bool hastext=false;
  for(int i=0;i<scene->scenes();i++){
    textscene = scene->getTextScene(i);
    if(textscene && textscene->size()){
      hastext=true;
      break;
    }
  }

  if(hastext){
    glPushAttrib( GL_LIGHTING_BIT | GL_TEXTURE_BIT );
    glDisable( GL_LIGHTING );
    glDisable( GL_TEXTURE_2D );
    QFont fnt( "helvetica", 12, QFont::Bold, TRUE );
    for(int i=0;i<scene->scenes();i++){
      textscene = scene->getTextScene(i);
      tsize = textscene->size();
      for(int j=0;j<tsize;j++){
        textPtr = (*textscene)[j];
        if(scene->isVisible(i) && tsize){
    for(int j=0;j<tsize;j++){
      qglColor( QColor( (int)(255*textPtr->color[0]),
            (int)(255*textPtr->color[1]),
            (int)(255*textPtr->color[2]) ) );
      renderText( textPtr->vect1[0], textPtr->vect1[1],
            textPtr->vect1[2], textPtr->maintext, fnt);
    }
        }
      }
    }
    glPopAttrib();
  }
  drawAxis();
}

void GLRenderer::drawAxis()
{
  if(glrendererOptions.drawAxis){
    glPushAttrib( GL_LIGHTING_BIT | GL_TEXTURE_BIT );
    glDisable( GL_LIGHTING );
    glDisable( GL_TEXTURE_2D );
    /*
      qglColor( white );
      QString str( "Rendering text in OpenGL is easy with Qt" );
      QFontMetrics fm( font() );
      renderText( (width() - fm.width( str )) / 2, 15, str );
      QFont f( "courier", 8 );
      QFontMetrics fmc( f );
      qglColor( QColor("skyblue") );
      int x, y, z;
      GLfloat[3] rot; scene->getRotatef(0,rot)
      float xRot=rot[0]; float yRot=rot[1]; float zRot=rot[2];
      x = (xRot >= 0) ? (int) xRot % 360 : 359 - (QABS((int) xRot) % 360);
      y = (yRot >= 0) ? (int) yRot % 360 : 359 - (QABS((int) yRot) % 360);
      z = (zRot >= 0) ? (int) zRot % 360 : 359 - (QABS((int) zRot) % 360);
      str.sprintf( "Rot X: %03d - Rot Y: %03d - Rot Z: %03d", x, y, z );
      renderText( (width() - fmc.width( str )) / 2, height() - 15, str, f );
    */
    QFont fnt( "helvetica", 12, QFont::Bold, TRUE );
    float unit = scene->largestExtremUnit()/2;
    const extremities extrem = scene->getExtremeties();
    
    if (glrendererOptions.drawAxisOrigin)
    { 
      /*
      axes[] = { 0.0,             0.0,             0.0,
                 extrem.xmax*1.1, 0.0,             0.0,
                 0.0,             0.0,             0.0,
                 0.0,             extrem.ymax*1.1, 0.0,
                 0.0,             0.0,             0.0,
                 0.0,             0.0,             extrem.zmax*1.1 };

      */
      glBegin( GL_LINES );
      {
          qglColor( green );
          glVertex3f( 0.0, 0.0, 0.0 );
          glVertex3f(  extrem.xmax*1.1, 0.0, 0.0 );
          glVertex3f( 0.0, 0.0, 0.0 );
          glVertex3f( 0.0, extrem.ymax*1.1, 0 );
          glVertex3f( 0.0, 0.0, 0.0 );
          glVertex3f( 0.0, 0.0, extrem.zmax*1.1 );
      }
      glEnd();

      renderText(  extrem.xmax*1.2, 0.0, 0.0, "X", fnt);
      renderText( 0.0, extrem.ymax*1.2, 0.0, "Y", fnt);
      renderText( 0.0, 0.0,  extrem.zmax*1.2, "Z", fnt);
    }
    else 
    { 
      /*
      redLines[] = { 0.0,       0.0,       0.0,
                     unit*0.05, 0.0,       0.0,
                     0.0,       0.0,       0.0,
                     0.0,       unit*0.05, 0.0,
                     0.0,       0.0,       0.0,
                     0.0,       0.0,       unit*0.05 };

      glDrawArrays("GL_LINES, 0, 3);
      greenLines[] =  { extrem.xmin,          0.0+extrem.ymin,      0.0+extrem.zmin
                        unit*1.1+extrem.xmin, 0.0+extrem.ymin,      0.0+extrem.zmin
                        extrem.xmin,          0.0+extrem.ymin,      0.0+extrem.zmin
                        0.0+extrem.xmin,      unit*1.1+extrem.ymin, 0.0+extrem.zmin
                        extrem.xmin,          0.0+extrem.ymin,      0.0+extrem.zmin
                        0.0+extrem.xmin,      0.0+extrem.ymin,      unit*1.1+extrem.zmin };
      glDrawArrays("GL_LINES, 0, 3);
        */
      glBegin( GL_LINES );
      {
        qglColor( red );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( unit*0.05, 0.0, 0.0 );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( 0.0, unit*0.05, 0 );
        glVertex3f( 0.0, 0.0, 0.0 );
        glVertex3f( 0.0, 0.0, unit*0.05 );

        qglColor( green );
        glVertex3f( extrem.xmin, 0.0+extrem.ymin, 0.0+extrem.zmin);
        glVertex3f( unit*1.1+extrem.xmin, 0.0+extrem.ymin, 0.0+extrem.zmin);
        glVertex3f( extrem.xmin, 0.0+extrem.ymin, 0.0+extrem.zmin);
        glVertex3f( 0.0+extrem.xmin, unit*1.1+extrem.ymin, 0.0+extrem.zmin);
        glVertex3f( extrem.xmin, 0.0+extrem.ymin, 0.0+extrem.zmin);
        glVertex3f( 0.0+extrem.xmin, 0.0+extrem.ymin, unit*1.1+extrem.zmin);
      }
      glEnd();

      renderText( unit*1.1+extrem.xmin, 0.0+extrem.ymin, 0.0+extrem.zmin, "X", fnt);
      renderText( 0.0+extrem.xmin, unit*1.1+extrem.ymin, 0.0+extrem.zmin, "Y", fnt);
      renderText( 0.0+extrem.xmin, 0.0+extrem.ymin, unit*1.1+extrem.zmin, "Z", fnt);
    }
    glPopAttrib();
  }
}

void GLRenderer::postProcess(GLfloat *depthRefData)
{
  GLubyte *imageData = 0;
  GLfloat *depthData = 0;
  unsigned int imageLineLength = 0;
  unsigned int depthLineLength = 0;
  bool lastLighting;
  GLint halfLine = (GLint) ceil(viewState.lineWidth/2.0);

  try
  {
    imageLineLength = alignedLength(3 * glWindowWidth, sizeof(GLubyte));
    imageData = new GLubyte[imageLineLength * glWindowHeight];
    glReadPixels(0, 0, glWindowWidth, glWindowHeight, GL_RGB,
           GL_UNSIGNED_BYTE, imageData);

    if (viewState.binWindowEnabled)
    {
      depthLineLength = alignedLength(glWindowWidth, sizeof(GLfloat));
      depthData = new GLfloat[glWindowHeight*depthLineLength];
      glReadPixels(0, 0, glWindowWidth, glWindowHeight, GL_DEPTH_COMPONENT,
             GL_FLOAT, depthData);
    }

    if (viewState.binWindowEnabled)
      binWindowProcess(imageData, imageLineLength, depthData, depthLineLength, depthRefData);

    if ( (viewState.gammaEnabled) && (viewState.gammaExponent != 1.0) )
      gammaProcess(imageData, imageLineLength);

    if (viewState.binWindowEnabled)
      delete[] depthData;

    // draw
    if (viewState.antialiasEnabled)
      glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D((-halfLine), (GLdouble) (glWindowWidth + halfLine),
           (-halfLine), (GLdouble) (glWindowHeight + halfLine));
    glMatrixMode(GL_MODELVIEW);

    glDisable(GL_DEPTH_TEST);
    lastLighting = lighting();
    setLightingQuiet(false);

    glLoadIdentity();
    glRasterPos2i(0, 0);
    glDrawPixels(glWindowWidth, glWindowHeight, GL_RGB, GL_UNSIGNED_BYTE, imageData);

    glEnable(GL_DEPTH_TEST);
    setLightingQuiet(lastLighting);

    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    if (viewState.antialiasEnabled)
      glPopMatrix();

    delete[] imageData;
  }
  catch (...)
  {
    if (imageData)
      delete[] imageData;

    qWarning("Out of memory trying to post process image.");
  }
}

void GLRenderer::gammaProcess(GLubyte *imageData, unsigned int imageLineLength)
{
  register GLubyte *gammaPointer = viewState.gammaLookup;
  register GLubyte *imageDataPointer = imageData;
  GLubyte padding = imageLineLength - glWindowWidth * 3;

  for (int i=0 ; i < glWindowHeight ; i++)
  {
    for (register int j=0 ; j < glWindowWidth ; j++)
    {
      *(imageDataPointer++) = *(gammaPointer + (*imageDataPointer));

      *(imageDataPointer++) = *(gammaPointer + (*imageDataPointer));
      *(imageDataPointer++) = *(gammaPointer + (*imageDataPointer));
    }

    imageDataPointer += padding;
  }

  return;
}

void GLRenderer::binWindowProcess(GLubyte *imageData, unsigned int imageLineLength,
                  GLfloat *depthData, unsigned int depthLineLength,
                  GLfloat *depthRefData)
{
  if (viewState.binWindowCount[viewState.binWindowIndex])
  {
    cuv_uint paletteCount = viewState.binWindowCount[viewState.binWindowIndex];
    GLubyte *fromPaletteStart = viewState.binWindowPalette[viewState.binWindowIndex];
    GLubyte *fromPaletteEnd = fromPaletteStart + 3 * paletteCount;
    GLubyte *toPalette = fromPaletteEnd + 3 * paletteCount;
    GLubyte imagePadding = imageLineLength - glWindowWidth * 3;
    GLubyte depthPadding = depthLineLength - glWindowWidth;

    register GLubyte *imageDataPointer = imageData;
    register GLubyte *fromPaletteStartWork;
    register GLubyte *fromPaletteEndWork;
    GLubyte *toPaletteWork;

    // match colours -- keep both code blocks parallel!!!!!
    if (depthRefData)
    {
      // need to compare depth to the reference value

      for (int i=0 ; i < glWindowHeight ; i++)
      {
        // iterating through height

        for (int j=0 ; j < glWindowWidth ;
           j++, imageDataPointer+=3, depthData++, depthRefData++)
        {
          // iterating through width

          // only change data if it is not part of the background
          if (*depthData < *depthRefData)
          {
            fromPaletteStartWork = fromPaletteStart;
            fromPaletteEndWork = fromPaletteEnd;
            toPaletteWork = toPalette;

            // search for a matching range
            for ( ; fromPaletteStartWork<fromPaletteEnd ; fromPaletteStartWork+=3,
                  fromPaletteEndWork+=3)
            {
              if ( !( (*fromPaletteStartWork     > *imageDataPointer    ) ||
                  (*fromPaletteEndWork       < *imageDataPointer    ) ||
                  (*(fromPaletteStartWork+1) > *(imageDataPointer+1)) ||
                  (*(fromPaletteEndWork+1)   < *(imageDataPointer+1)) ||
                  (*(fromPaletteStartWork+2) > *(imageDataPointer+2)) ||
                  (*(fromPaletteEndWork+2)   < *(imageDataPointer+2)) ) )
              {
                // matching range found; replace the colour

                toPaletteWork  = (fromPaletteStartWork - fromPaletteStart) +
                  toPalette;
                *imageDataPointer     = *toPaletteWork;
                *(imageDataPointer+1) = *(++toPaletteWork);
                *(imageDataPointer+2) = *(++toPaletteWork);

                break; // stop searching for matches
              }
            }
          }
        }

        imageDataPointer += imagePadding;
        depthData += depthPadding;
        depthRefData += depthPadding;
      }
    }
    else
    {
      // simply compare the depth to 1.0

      for (int i=0 ; i < glWindowHeight ; i++)
      {
        // iterating through height

        for (int j=0 ; j < glWindowWidth ; j++, imageDataPointer+=3, depthData++)
        {
          // iterating through width

          // only change data if it is not part of the background
          if (*depthData < 1.0F)
          {
            fromPaletteStartWork = fromPaletteStart;
            fromPaletteEndWork = fromPaletteEnd;
            toPaletteWork = toPalette;

            // search for a matching range
            for ( ; fromPaletteStartWork<fromPaletteEnd ; fromPaletteStartWork+=3,
                  fromPaletteEndWork+=3)
            {
              if ( !( (*fromPaletteStartWork     > *imageDataPointer    ) ||
                  (*fromPaletteEndWork       < *imageDataPointer    ) ||
                  (*(fromPaletteStartWork+1) > *(imageDataPointer+1)) ||
                  (*(fromPaletteEndWork+1)   < *(imageDataPointer+1)) ||
                  (*(fromPaletteStartWork+2) > *(imageDataPointer+2)) ||
                  (*(fromPaletteEndWork+2)   < *(imageDataPointer+2)) ) )
              {
                // matching range found; replace the colour

                toPaletteWork  = (fromPaletteStartWork - fromPaletteStart) +
                  toPalette;
                *imageDataPointer     = *toPaletteWork;
                *(imageDataPointer+1) = *(++toPaletteWork);
                *(imageDataPointer+2) = *(++toPaletteWork);

                break; // stop searching for matches
              }
            }
          }
        }

        imageDataPointer += imagePadding;
        depthData += depthPadding;
      }
    }

  }
}

bool GLRenderer::getEditMode(){
  return scene->getEditMode();
}

void GLRenderer::setEditMode(bool edit){
  scene->setEditMode(edit);
  redrawDoc();
}

// never register solidSphereList or wireSphereList (nor release for that matter)
void GLRenderer::registerList(GLuint toRegister)
{
  DisplayListMap::Iterator it = dispListRefs->find(toRegister);

  if (it == dispListRefs->end())
  {
    // doesn't already exist, so add it
    dispListRefs->insert(toRegister, 1);
  }
  else
  {
    // already exists--update the number
    it.value()++;
  }
}

void GLRenderer::releaseList(GLuint toRelease)
{
  DisplayListMap::Iterator it = dispListRefs->find(toRelease);

  if (it == dispListRefs->end())
  {
    qWarning("A non-existant display list was requested to be removed.");
  }
  else
  {
    it.value()--;

    if (it.value() == 0)
    {
      dispListRefs->remove(it.key());

      makeCurrent();
      glDeleteLists(toRelease, 1);
    }
  }
}

void GLRenderer::useEntityScene(EntityScene *toUse)
{
  scene = toUse;
  updateProjection();
//  redrawDoc();
}

void GLRenderer::resetViewState()
{
  // keep this current with the constructor!!!!

  GLfloat temp1[3], temp2[3];

  glLoadIdentity();

  setTwoSided(false);
  setSmoothShaded(true);
  setTransparency(true);
  setAntialias(false);
  setSphereTess(8);

  setOutline(true);
  setWireframe(false);
  setOpaqueWireframe(false);
  setLineWidth(1.0F);
  temp1[0] = 1.0F; temp1[1] = 0.0F; temp1[2] = 0.0F;
  setOutlineColor(temp1);

  setLight(true);
  setLighting(true);
  setAmbient(0.2F);
  setDiffuse(0.8F);
  setBackground(0.0F);

  setPerspective(true);
  setFov(20.0F);
  temp1[0] = 0.0F; temp1[1] = 0.0F; temp1[2] = 0.0F;
  cameraTranslate(temp1, true);
  temp1[0] = 0.0F; temp1[1] = 0.0F; temp1[2] = -1.0F;
  temp2[0] = 0.0F; temp2[1] = 1.0F; temp2[2] = 0.0F;
  setCameraAimAndUp(temp1, temp2);
  setClipPerspNear(0.125);
  setClipOrthoNear(0.0);

  setGamma(false);
  setGammaLevel(1.0F);

  setBinWindow(false);
  setBinWindowIndex(0);
}

void GLRenderer::setSphereTessQuietForce(cuv_ubyte sphereTess)
{
  makeCurrent();

  // make sure value is > 1
  if (sphereTess > 1)
    viewState.sphereTess = sphereTess;
  else
    viewState.sphereTess = 2;

  GLUquadricObj *qObj;
  qObj = gluNewQuadric();

  if (qObj) // if it created a Quadric object
  {
#ifndef Q_WS_WIN
//In win32, Borland 5.5 complains about type mismatch: 
//wanted 'void (__stdcall *)()
//    gluQuadricCallback(qObj, (GLenum) GLU_ERROR, quadricError);
#endif

    if (!solidSphereList)
      solidSphereList = glGenLists(1);
    if (!wireSphereList)
      wireSphereList = glGenLists(1);

    // lists are not 0 if an index can be allocated successfully
    if (solidSphereList)
    {
      gluQuadricDrawStyle(qObj, (GLenum) GLU_FILL);
      gluQuadricNormals(qObj, (GLenum) GLU_SMOOTH);

      glNewList(solidSphereList, GL_COMPILE);
      gluSphere(qObj, 1.0, 2*viewState.sphereTess, viewState.sphereTess); // unit sphere
      glEndList();
    }
    else
    {
      qWarning("Solid spheres cannot be allocated; solid spheres not available");
    }

    if (wireSphereList)
    {
      gluQuadricDrawStyle(qObj, (GLenum) GLU_LINE);
      gluQuadricNormals(qObj, (GLenum) GLU_NONE);

      glNewList(wireSphereList, GL_COMPILE);
      gluSphere(qObj, 1.0, 2*viewState.sphereTess, viewState.sphereTess); // unit sphere
      glEndList();
    }
    else
    {
      qWarning("Wire spheres cannot be allocated; wire spheres not available");
    }

    gluQuadricCallback(qObj, (GLenum) GLU_ERROR, NULL);
    gluDeleteQuadric(qObj);
  }
  else
  {
    qWarning("Quadrics object could not be created; all quadrics not available");
  }
}

void GLRenderer::setFovScale(GLfloat fov)
{
  if (viewState.fov != fov)
  {
    GLfloat oldFov = viewState.fov;
    GLfloat dist[3];

    setFov(fov);

    scene->pos(dist);
    vsub(dist, viewState.cameraPos, dist);
    vscale( dist, (tan(oldFov * M_PI / 360.0) /
             tan(viewState.fov * M_PI / 360.0) - 1) );

    cameraTranslate(dist);
  }
}

void GLRenderer::setGammaLevel(float exponent)
{
  if (viewState.gammaExponent != exponent)
  {
    GLubyte *gammaPointer = viewState.gammaLookup;

    if (exponent == 1.0F)
    {
      viewState.gammaExponent = exponent;

      // quick, linear setup
      for (GLubyte i = (GLubyte)'\x00' ; i < (GLubyte)'\xFF' ; i++)
        *(gammaPointer++) = i;
      *gammaPointer = (GLubyte)'\xFF';

    }
    else
    {
      if (exponent < MIN_GAMMA_AMT)
        viewState.gammaExponent = MIN_GAMMA_AMT;
      else if (exponent > MAX_GAMMA_AMT)
        viewState.gammaExponent = MAX_GAMMA_AMT;
      else
        viewState.gammaExponent = exponent;

      double gammaInvert = 1.0 / viewState.gammaExponent;

      for (int i=0 ; i < 256 ; i++)
      {
        *(gammaPointer++) = (GLubyte) (255.0 * pow(((double) i / 255.0),
                               gammaInvert ));
      }
    }

    cachedImageExists = false;
    emit gammaLevelChanged(exponent);
  }
}

void GLRenderer::setSpecular(float value)
{
  if (viewState.specular != value) {
    if (value < MIN_SPECULAR_AMT)
      viewState.specular = MIN_SPECULAR_AMT;
    else if (value > MAX_SPECULAR_AMT)
      viewState.specular = MAX_SPECULAR_AMT;
    else{
      viewState.specular = value;
      float color = value/MAX_SPECULAR_AMT;
      GLfloat mat_specular[] = {color,color,color,1.0F};
      glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    }
    cachedImageExists = false;
  }
}

void GLRenderer::setBinWindowPalette(cuv_ubyte index, const bin_window_palette* palData)
{
  if (viewState.binWindowCount[index] == palData->count)
  {
    // same count as before
    if (viewState.binWindowCount[index])
    {
      // update the palette of the same size
      GLubyte *current;
      GLubyte *dataSource;

      if (viewState.binWindowName[index])
      {
        // change the old name to the new one
        *viewState.binWindowName[index] = palData->name;
      }
      else
      {
        // should not occur unless mem is really, really low and previous attempt failed
        try
        {
          viewState.binWindowName[index] = new QString(palData->name);
        }
        catch (...)
        {
          viewState.binWindowName[index] = 0;
          qWarning("Out of memory trying to create a palette name");
        }
      }

      current = viewState.binWindowPalette[index];

      dataSource = palData->fromPaletteStart;
      for (GLubyte *ending = current + 3 * palData->count ; current < ending ; )
        *(current++) = *(dataSource++);

      dataSource = palData->fromPaletteEnd;
      for (GLubyte *ending = current + 3 * palData->count ; current < ending ; )
        *(current++) = *(dataSource++);

      dataSource = palData->toPalette;
      for (GLubyte *ending = current + 3 * palData->count ; current < ending ; )
        *(current++) = *(dataSource++);
    }
    // else clearing a cleared palette again (don't bother)
  }
  else
  {
    // different count from last and now
    if (viewState.binWindowCount[index])
    {
      // clear the old palette data first
      delete[] viewState.binWindowPalette[index];
    }
    // else creating a new palette--nothing to delete

    if (palData->count)
    {
      // new data to put into the palette
      GLubyte *current;
      GLubyte *dataSource;

      if (viewState.binWindowCount[index])
      {
        // just change the old QString (as long as it exists...)
        if (viewState.binWindowName[index])
        {
          *viewState.binWindowName[index] = palData->name;
        }
        else
        {
          // should not occur unless mem is really, really low and previous attempt failed
          try
          {
            viewState.binWindowName[index] = new QString(palData->name);
          }
          catch (...)
          {
            viewState.binWindowName[index] = 0;
            qWarning("Out of memory trying to create a palette name");
          }
        }
      }
      else
      {
        // create a new instance of the QString name since none exists
        try
        {
          viewState.binWindowName[index] = new QString(palData->name);
        }
        catch (...)
        {
          viewState.binWindowName[index] = 0; // just set the thing to null if problem
          qWarning("Out of memory trying to create a palette name");
        }
      }

      try
      {
        current = viewState.binWindowPalette[index] =
          new GLubyte[3*3*palData->count];

        viewState.binWindowCount[index] = palData->count;

        dataSource = palData->fromPaletteStart;
        for (GLubyte *ending = current + 3 * palData->count ; current < ending ; )
          *(current++) = *(dataSource++);

        dataSource = palData->fromPaletteEnd;
        for (GLubyte *ending = current + 3 * palData->count ; current < ending ; )
          *(current++) = *(dataSource++);

        dataSource = palData->toPalette;
        for (GLubyte *ending = current + 3 * palData->count ; current < ending ; )
          *(current++) = *(dataSource++);
      }
      catch (...)
      {
        // uh-oh, no more mem
        viewState.binWindowCount[index] = 0;
        qWarning("Out of memory trying to create a palette");
      }
    }
    else
    {
      // else the palette is being cleared; data cleared earlier, need to delete QString
      if (viewState.binWindowName[index])
      {
        delete viewState.binWindowName[index];
      }
    }
  }

  cachedImageExists = false;
  emit binWindowPaletteChanged(index, palData);
}

bin_window_palette *GLRenderer::binWindowPalette(cuv_ubyte index) const
{
  bin_window_palette *toReturn;

  try
  { toReturn = new bin_window_palette; }
  catch (...)
  {
    qWarning("Out of memory trying to create a palette");
    return 0;
  }

  toReturn->count = viewState.binWindowCount[index];

  if (toReturn->count)
  {
    GLubyte *current;
    GLubyte *dataSource;

    // make life simple, and only allocate once
    try
    {
      if (viewState.binWindowName[index]) // just in case it couldn't alloc
        toReturn->name = *viewState.binWindowName[index];

      toReturn->fromPaletteStart = new GLubyte[3*3*toReturn->count];
    }
    catch (...)
    {
      delete toReturn;
      qWarning("Out of memory trying to create a palette");
      return 0;
    }
    toReturn->fromPaletteEnd = toReturn->fromPaletteStart + 3 * toReturn->count;
    toReturn->toPalette = toReturn->fromPaletteEnd + 3 * toReturn->count;

    dataSource = viewState.binWindowPalette[index];

    current = toReturn->fromPaletteStart;
    for (GLubyte *ending = current + 3*3 * toReturn->count ; current < ending ; )
      *(current++) = *(dataSource++);
  }
  else
  {
    //toReturn->name = "";
    toReturn->fromPaletteStart = 0;
    toReturn->fromPaletteEnd = 0;
    toReturn->toPalette = 0;
  }

  return toReturn;
}

void GLRenderer::setFogLevel(float value)
{
  //value range 0.0,1.0
  if (viewState.fogLevel != value)
  {
    viewState.fogLevel = value;
    // glFogf(GL_FOG_DENSITY, viewState.fogLevel); //no effect in linear
    glFogf(GL_FOG_START,7*viewState.fogLevel*scene->largestExtremUnit());
    glFogf(GL_FOG_END,(7*viewState.fogLevel+.4)*scene->largestExtremUnit());

    cachedImageExists = false;
  }
}

void GLRenderer::cameraTranslate(const GLfloat vect[3], bool absolute)
{
  if(scene->getEditMode()){
    for(int i=0;i<scene->scenes();i++)
      if(scene->isEditing(i))
  scene->translateScene(i,-vect[0],-vect[1],-vect[2]);
    for(int i=0;i<viewState.lightSources;i++)
      if(getEditLightMode(i))
  translateLight(i,-vect[0],-vect[1],-vect[2]);

    updateProjection();
    cachedImageExists = false;
    emit cameraPosChanged(viewState.cameraPos);
    return;
  }

  if (absolute)
  {
    if (vunequal3(viewState.cameraPos, vect))
      vcopy3(viewState.cameraPos, vect);
    else
      return; // don't emit a change
  }
  else
  {
    if (vect[0] || vect[1] || vect[2])
    {
      viewState.cameraPos[0] += vect[0];
      viewState.cameraPos[1] += vect[1];
      viewState.cameraPos[2] += vect[2];
    }
    else
      return; // don't emit a change
  }

  updateProjection();
  cachedImageExists = false;
  emit cameraPosChanged(viewState.cameraPos);
}

void GLRenderer::cameraRotate(const GLfloat vect[4], bool absolute)
{
  if(scene->getEditMode()){
//    qDebug("scale %f",1-vect[1]/100.0);
/*    if ( vect[0] && (vect[1] || vect[2] || vect[3]) ) // check for no rotation or a (0, 0, 0) axis of rotation
    {
    vrotate(temp1, viewState.cameraAim, vect);
    vcopy3(viewState.cameraAim, temp1);
    vrotate(temp1, viewState.cameraUp, vect);
    vcopy3(viewState.cameraUp, temp1);
     }
*/
    for(int i=0;i<scene->scenes();i++)
      scene->scaleScene(i,1-vect[1]/100.0);
    return;
  }
  GLfloat temp1[3];

  GLfloat oldCameraAim[3];
  GLfloat oldCameraUp[3];

  vcopy3(oldCameraAim, viewState.cameraAim);
  vcopy3(oldCameraUp, viewState.cameraUp);

  if (absolute) // reset to defaults
  {
    viewState.cameraAim[0] = 0.0F;
    viewState.cameraAim[1] = 0.0F;
    viewState.cameraAim[2] = -1.0F;

    viewState.cameraUp[0] = 0.0F;
    viewState.cameraUp[1] = 1.0F;
    viewState.cameraUp[2] = 0.0F;
  }

  if ( vect[0] && (vect[1] || vect[2] || vect[3]) ) // check for no rotation or a (0, 0, 0) axis of rotation
  {
    vrotate(temp1, viewState.cameraAim, vect);
    vcopy3(viewState.cameraAim, temp1);
    vrotate(temp1, viewState.cameraUp, vect);
    vcopy3(viewState.cameraUp, temp1);

    cameraFix();
  }

  if (vunequal3(oldCameraAim, viewState.cameraAim))
  {
    updateProjection();
    cachedImageExists = false;
    emit cameraAimAndUpChanged(viewState.cameraAim, viewState.cameraUp);
  }
  else if (vunequal3(oldCameraUp, viewState.cameraUp))
  {
    cachedImageExists = false;
    emit cameraAimAndUpChanged(viewState.cameraAim, viewState.cameraUp);
  }
}

void GLRenderer::cameraOrbit(const GLfloat vect[4], const GLfloat center[3], bool absolute)
{
  if(scene->getEditMode()){
    for(int i=0;i<scene->scenes();i++)
      scene->rotateScene(i,vect);
    return;
  }

  GLfloat temp1[3], length;

  GLfloat oldCameraPos[3];
  GLfloat oldCameraAim[3];
  GLfloat oldCameraUp[3];
  bool unupdatedProjection = true;

  vcopy3(oldCameraPos, viewState.cameraPos);
  vcopy3(oldCameraAim, viewState.cameraAim);
  vcopy3(oldCameraUp, viewState.cameraUp);

  if (absolute) // reset to defaults
  {
    viewState.cameraPos[0] = 0.0F;
    viewState.cameraPos[1] = 0.0F;
    viewState.cameraPos[2] = 0.0F;

    viewState.cameraAim[0] = 0.0F;
    viewState.cameraAim[1] = 0.0F;
    viewState.cameraAim[2] = -1.0F;

    viewState.cameraUp[0] = 0.0F;
    viewState.cameraUp[1] = 1.0F;
    viewState.cameraUp[2] = 0.0F;
  }

  if ( vect[0] && (vect[1] || vect[2] || vect[3]) ) // check for no rotation or a (0, 0, 0) axis of rotation
  {
    viewState.cameraPos[0] -= center[0];
    viewState.cameraPos[1] -= center[1];
    viewState.cameraPos[2] -= center[2];

    // scale the vectors so that precision isn't lost (fixed by cameraFix())
    length = vlength(viewState.cameraPos);
    if (vlength(viewState.cameraPos) > 1.0)
    {
      vscale(viewState.cameraAim, length);
      vscale(viewState.cameraUp, length);
    }

    viewState.cameraAim[0] += viewState.cameraPos[0];
    viewState.cameraAim[1] += viewState.cameraPos[1];
    viewState.cameraAim[2] += viewState.cameraPos[2];

    viewState.cameraUp[0] += viewState.cameraPos[0];
    viewState.cameraUp[1] += viewState.cameraPos[1];
    viewState.cameraUp[2] += viewState.cameraPos[2];

    vrotate(temp1, viewState.cameraPos, vect);
    vcopy3(viewState.cameraPos, temp1);

    vrotate(temp1, viewState.cameraAim, vect);
    vcopy3(viewState.cameraAim, temp1);

    vrotate(temp1, viewState.cameraUp, vect);
    vcopy3(viewState.cameraUp, temp1);

    viewState.cameraAim[0] -= viewState.cameraPos[0];
    viewState.cameraAim[1] -= viewState.cameraPos[1];
    viewState.cameraAim[2] -= viewState.cameraPos[2];

    viewState.cameraUp[0] -= viewState.cameraPos[0];
    viewState.cameraUp[1] -= viewState.cameraPos[1];
    viewState.cameraUp[2] -= viewState.cameraPos[2];

    viewState.cameraPos[0] += center[0];
    viewState.cameraPos[1] += center[1];
    viewState.cameraPos[2] += center[2];

    cameraFix();
  }

  if (vunequal3(oldCameraPos, viewState.cameraPos))
  {
    updateProjection();
    unupdatedProjection = false;

    cachedImageExists = false;
    emit cameraPosChanged(viewState.cameraPos);
  }

  if (vunequal3(oldCameraAim, viewState.cameraAim))
  {
    if (unupdatedProjection)
      updateProjection();

    cachedImageExists = false;
    emit cameraAimAndUpChanged(viewState.cameraAim, viewState.cameraUp);
  }
  else if (vunequal3(oldCameraUp, viewState.cameraUp))
  {
    cachedImageExists = false;
    emit cameraAimAndUpChanged(viewState.cameraAim, viewState.cameraUp);
  }
}

void GLRenderer::setCameraAimAndUp(const GLfloat cameraAim[3], const GLfloat cameraUp[3])
{
  GLfloat absDotResult;
  GLfloat temp[3];
  GLfloat tempAim[3];
  GLfloat tempUp[3];

  vcopy3(tempAim, cameraAim);
  vcopy3(tempUp, cameraUp);

  if (vnormalize(tempAim) && vnormalize(tempUp))
  {
    absDotResult = vdot(tempAim, tempUp);
    absDotResult = (absDotResult > 0) ? absDotResult : -absDotResult;

    if (absDotResult < 0.9F) // make sure vects are at least ~26deg apart
    {
      // now, make vectors orthogonal
      vcross(temp, tempAim, tempUp);
      vcross(tempUp, temp, tempAim);
      vnormalize(tempUp);

      if (vunequal3(viewState.cameraAim, tempAim))
      {
        vcopy3(viewState.cameraAim, tempAim);
        vcopy3(viewState.cameraUp, tempUp); // quicker to just copy

        updateProjection();
        cachedImageExists = false;
        emit cameraAimAndUpChanged(viewState.cameraAim, viewState.cameraUp);
      }
      else if (vunequal3(viewState.cameraUp, tempUp))
      {
        vcopy3(viewState.cameraUp, tempUp);

        cachedImageExists = false;
        emit cameraAimAndUpChanged(viewState.cameraAim, viewState.cameraUp);
      }
    }
    // else vectors are near-parallel
  }
  // else (near) null vector(s)
}

void GLRenderer::gotoViewpoint(cuv_ubyte view)
{
  if (scene && (view < 10))
  {
    GLfloat tempVect[4];
    cuv_float objPos[3];
    cuv_float objSize;

    scene->pos(objPos);

    scene->halfSize(tempVect);
    objSize = vlength(tempVect);

    if (!objSize)
      objSize = 1.0F;

    cameraTranslate(objPos, true);
    tempVect[0] = 0.0;
    tempVect[1] = 0.0;
    tempVect[2] = 0.0;
    tempVect[3] = 0.0;
    cameraRotate(tempVect, true);
    tempVect[0] = 0.0;
    tempVect[1] = 0.0;
    tempVect[2] = objSize / tan(fov() * M_PI / 360.0);
    cameraTranslate(tempVect);

    switch (view)
    {
      case 1: // top
      break;
      case 2: // front
      tempVect[0] = 90.0;
      tempVect[1] = 1.0;
      tempVect[2] = 0.0;
      tempVect[3] = 0.0;
      cameraOrbit(tempVect, objPos);
      break;
      case 3: // right
      tempVect[0] = 90.0;
      tempVect[1] = 0.0;
      tempVect[2] = 0.0;
      tempVect[3] = 1.0;
      cameraRotate(tempVect);
      tempVect[0] = 90.0;
      tempVect[1] = 0.0;
      tempVect[2] = 1.0;
      tempVect[3] = 0.0;
      cameraOrbit(tempVect, objPos);
      break;
      case 4: // bottom
      tempVect[0] = 180.0;
      tempVect[1] = 0.0;
      tempVect[2] = 1.0;
      tempVect[3] = 0.0;
      cameraOrbit(tempVect, objPos);
      break;
      case 5: // back
      tempVect[0] = 180.0;
      tempVect[1] = 0.0;
      tempVect[2] = 0.0;
      tempVect[3] = 1.0;
      cameraRotate(tempVect);
      tempVect[0] = 90.0;
      tempVect[1] = -1.0;
      tempVect[2] = 0.0;
      tempVect[3] = 0.0;
      cameraOrbit(tempVect, objPos);
      break;
      case 6: // left
      tempVect[0] = 90.0;
      tempVect[1] = 0.0;
      tempVect[2] = 0.0;
      tempVect[3] = -1.0;
      cameraRotate(tempVect);
      tempVect[0] = 90.0;
      tempVect[1] = 0.0;
      tempVect[2] = -1.0;
      tempVect[3] = 0.0;
      cameraOrbit(tempVect, objPos);
      break;
      case 7: // top back-left
      tempVect[0] = 135.0;
      tempVect[1] = 0.0;
      tempVect[2] = 0.0;
      tempVect[3] = -1.0;
      cameraRotate(tempVect);
      tempVect[0] = 45.0;
      tempVect[1] = -1.0;
      tempVect[2] = -1.0;
      tempVect[3] = 0.0;
      cameraOrbit(tempVect, objPos);
      break;
      case 8: // top back-right
      tempVect[0] = 135.0;
      tempVect[1] = 0.0;
      tempVect[2] = 0.0;
      tempVect[3] = 1.0;
      cameraRotate(tempVect);
      tempVect[0] = 45.0;
      tempVect[1] = -1.0;
      tempVect[2] = 1.0;
      tempVect[3] = 0.0;
      cameraOrbit(tempVect, objPos);
      break;
      case 9: // top front-left
      tempVect[0] = 45.0;
      tempVect[1] = 0.0;
      tempVect[2] = 0.0;
      tempVect[3] = -1.0;
      cameraRotate(tempVect, objPos);
      tempVect[0] = 45.0;
      tempVect[1] = 1.0;
      tempVect[2] = -1.0;
      tempVect[3] = 0.0;
      cameraOrbit(tempVect, objPos);
      break;
      case 0: // top front-right
      tempVect[0] = 45.0;
      tempVect[1] = 0.0;
      tempVect[2] = 0.0;
      tempVect[3] = 1.0;
      cameraRotate(tempVect);
      tempVect[0] = 45.0;
      tempVect[1] = 1.0;
      tempVect[2] = 1.0;
      tempVect[3] = 0.0;
      cameraOrbit(tempVect, objPos);
      break;
      default:
      break;
    }
  }

  return;
}

void GLRenderer::pollForGlError()
{
  GLenum err;

  makeCurrent();

  while ((err = glGetError()) != (GLenum)GL_NO_ERROR)
  {
    qWarning("OpenGL Error: %s", err);

//      if ((err == (GLenum)GL_OUT_OF_MEMORY) || (err == (GLenum)GLU_OUT_OF_MEMORY))
//      {
//        cout << "Cannot continue without more memory, quitting." << endl;
//        exit(2);
//      }
  }
}
