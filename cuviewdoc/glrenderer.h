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

#ifndef GLRENDERER_H
#define GLRENDERER_H

#include <qgl.h>
#include <qmap.h>
#include <qpixmap.h>
#include <qstring.h>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QWheelEvent>

#ifdef Q_OS_MAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include "../include/cuvtags.h"
#include "../include/cuvcommonstructs.h"
#include "../include/cuvvector.h"

#define MIN_CLIP_SIZE 0.0001
#define MIN_GAMMA_AMT 0.0001
#define MAX_GAMMA_AMT 10000
#define MIN_SPECULAR_AMT 0.000
#define MAX_SPECULAR_AMT 25
#define BYTE_ALIGNMENT 4U
#define IMAGE_CACHE_THRESHOLD 500U // ms

class QMouseEvent;
class EntityScene;

class GLRenderer: public QGLWidget
{
  Q_OBJECT

  public:
  /**
      @brief Initializing error flags
    */
  enum GLInitError {
                    GNoError = 0,
                    GErrorNoRendContext = 1,
                    GErrorDubBuff = 2,
                    GErrorDephBuff = 4,
                    GErrorRGBABuff = 8,
                    GErrorAccumBuff = 16
                   };

  private:
  typedef QMap<GLuint, unsigned long> DisplayListMap;

  static const unsigned int ALIGN_ADD_AMT;
  static const unsigned int ALIGN_SHIFT_AMT;

  private:
  DisplayListMap *dispListRefs;
  GLuint solidSphereList;
  GLuint wireSphereList;

  proj_info currentProjection;
  view_state viewState;
  glrenderer_options glrendererOptions;
  //  font_draw fontDraw;
  EntityScene *scene;
  EntityScene *editingScene;
  bool newProjection;
  bool sceneHasChanged;
  bool cachedImageExists;

  int oldMouseX;
  int oldMouseY;
  bool mouseDragging;

  GLdouble actualClipFar;
  GLdouble actualClipNear;
  GLdouble distToObj;

  bool renderingPixmap;

  int cacheImageWidth;
  int cacheImageHeight;
  GLubyte *cacheImageData;
  unsigned int cacheImageLineLength;

  int glWindowWidth;
  int glWindowHeight;

  private:
  static unsigned int log2(unsigned int value);
  static unsigned int alignedLength(unsigned int dataLength, unsigned int sizeOfType);
  static void quadricError();

  void updateProjection();

  void drawLightSources();
  void drawBoxModel();
  void drawAxis();
  void drawAll(proj_info project);
  void readyCameraForDrawing();
  void drawScene();

  // need to have versions that emit no signals
  void setTwoSidedQuiet(bool toggle);
  void setSmoothShadedQuiet(bool toggle);
  void setSphereTessQuietForce(cuv_ubyte sphereTess);
  void setWireframeQuiet(bool toggle);
  void setLightingQuiet(bool toggle);

  void postProcess(GLfloat *depthRefData = 0);
  void gammaProcess(GLubyte *imageData, unsigned int imageLineLength);
  void binWindowProcess(GLubyte *imageData, unsigned int imageLineLength,
                        GLfloat *depthData, unsigned int depthLineLength,
                        GLfloat *depthRefData = 0);

  void cameraFix();

  public:
  GLRenderer(QGLFormat * glformat, QWidget *parent = 0, const char *name = 0, const QGLWidget *shareWidget = 0, Qt::WFlags f = 0);
   ~GLRenderer();
  virtual void keyPressEvent(QKeyEvent *keyEvent);
  virtual void mousePressEvent(QMouseEvent *mouseEvent);
  virtual void mouseReleaseEvent(QMouseEvent *mouseEvent);
  virtual void mouseMoveEvent(QMouseEvent *mouseEvent);
  virtual void wheelEvent(QWheelEvent *wheelEvent);
  virtual void initializeGL();
  int getInitError();
  virtual void resizeGL(int w, int h);
  virtual void paintGL();
  virtual QPixmap renderPixmap(int w=0, int h=0, bool useContext=false);

  inline GLuint getSolidSphereList()
  { return solidSphereList; }
  inline GLuint getWireSphereList()
  { return wireSphereList; }

  bool getEditMode();
  void setEditMode(bool);
  void registerList(GLuint toReserve);
  void releaseList(GLuint toRelease);
  void useEntityScene(EntityScene *toUse=NULL);

  void reloadSceneAttribs();

  private:
  void myFrustum(proj_info project, GLdouble pixdx = 0.0, GLdouble pixdy = 0.0);
  proj_info myPerspective(GLdouble fovy, GLdouble aspect, GLdouble nearp, GLdouble farp);
  void myOrtho(proj_info project, GLdouble pixdx = 0.0, GLdouble pixdy = 0.0);
  proj_info myOrthoFov(GLdouble fovy, GLdouble distz, GLdouble aspect,
                       GLdouble nearp, GLdouble farp);

  public: // public slots in CUViewDoc
  void redrawDoc();
  void setThreeButtonMouse(bool toggle);
  void setCachedImage(bool toggle);
  void setBoxWhileMoving(bool toggle);
  void setDrawBoxAlways(bool toggle);
  void setDrawAxis(bool toggle);
  void setDrawAxisOrigin(bool toggle);

  void setDrawScene(bool toggle);

  void resetViewState();

  void gotoViewpoint(cuv_ubyte view);

  void setTwoSided(bool toggle);
  void setSmoothShaded(bool toggle);
  void setTransparency(bool toggle);
  void setAntialias(bool toggle);
  void setSphereTess(cuv_ubyte sphereTess);

  void setOutline(bool toggle);
  void setWireframe(bool toggle);
  void setOpaqueWireframe(bool toggle);
  void setLineWidth(GLfloat lineWidth);
  void setOutlineColor(const GLfloat outlineColor[3]);

  void setLight(bool toggle);
  void setLighting(bool toggle);
  void setAmbient(GLfloat brightness);
  void setDiffuse(GLfloat brightness);
  void setBackground(GLfloat brightness);

  void setPerspective(bool toggle);
  void setFovScale(GLfloat fov);
  void setFov(GLfloat fov);
  void cameraTranslate(const GLfloat vect[3], bool absolute=false);
  void cameraRotate(const GLfloat vect[4], bool absolute=false);
  void cameraOrbit(const GLfloat vect[4], const GLfloat center[3], bool absolute=false);
  void setCameraAimAndUp(const GLfloat cameraAim[3], const GLfloat cameraUp[3]);
  void setClipPerspNear(GLdouble clip);
  void setClipOrthoNear(GLdouble clip);

  void setGamma(bool toggle);
  void setGammaLevel(float exponent);
  void setSpecular(float value);

  void setBinWindow(bool toggle);
  void setBinWindowIndex(cuv_ubyte index);
  void setBinWindowPalette(cuv_ubyte index, const bin_window_palette* palData);

  void setFog(bool toggle);
  void setFogLevel(float level);

  public: // public in CUViewDoc
  int getLightSources();
  bool fixedLight(int);
  void setFixedLight(int,bool);
  bool hasTranslatedLight(int);
  void getTranslateLightf(int,GLfloat[3]) const;
  void drawLight(int index);
  void resetTranslateLight(int index);
  void translateLight(int,cuv_float,cuv_float,cuv_float);
  bool getEditLightMode(int);
  void setEditLightMode(int,bool);
  void showLightPosition(bool);
  void lightColor(int index, GLfloat toFill[3]) const;
  void setLightColor(int index, const GLfloat lightColor[3]);

  bool threeButtonMouse() const;
  bool cachedImage() const;
  bool boxWhileMoving() const;
  bool drawBoxAlways() const;
  bool getDrawAxis() const;
  bool getDrawAxisOrigin() const;

  bool twoSided() const;
  bool smoothShaded() const;
  bool transparency() const;
  bool antialias() const;
  cuv_ubyte sphereTess() const;

  bool outline() const;
  bool wireframe() const;
  bool opaqueWireframe() const;
  GLfloat lineWidth() const;
  void outlineColor(GLfloat toFill[3]) const;

  bool light() const;
  bool lighting() const;
  GLfloat ambient() const;
  GLfloat diffuse() const;
  GLfloat background() const;

  bool perspective() const;
  GLfloat fov() const;
  void cameraPos(GLfloat toFill[3]) const;
  void cameraAim(GLfloat toFill[3]) const;
  void cameraUp(GLfloat toFill[3]) const;
  GLdouble clipPerspNear() const;
  GLdouble clipOrthoNear() const;

  bool gamma() const;
  float gammaLevel() const;
  float specular() const;

  bool fog();
  float fogLevel();

  bool binWindow() const;
  cuv_ubyte binWindowIndex() const;
  cuv_uint binWindowColors(cuv_ubyte index) const;
  QString binWindowName(cuv_ubyte index) const;
  // WARNING: make sure to free the data obtained when done!!!!
  bin_window_palette *binWindowPalette(cuv_ubyte index) const;
  void freeBinWindowPalette(bin_window_palette *toFree) const;

  private slots:
  void pollForGlError();

  signals:
  void threeButtonMouseChanged(bool state);
  void cachedImageChanged(bool state);
  void boxWhileMovingChanged(bool state);

  void twoSidedChanged(bool state);
  void smoothShadedChanged(bool state);
  void transparencyChanged(bool state);
  void antialiasChanged(bool state);
  void sphereTessChanged(cuv_ubyte state);

  void outlineChanged(bool state);
  void wireframeChanged(bool state);
  void opaqueWireframeChanged(bool state);
  void lineWidthChanged(GLfloat width);
  void outlineColorChanged(const GLfloat *color);

  void lightChanged(bool state);
  void lightingChanged(bool state);
  void ambientChanged(GLfloat brightness);
  void diffuseChanged(GLfloat brightness);
  void backgroundChanged(GLfloat brightness);

  void perspectiveChanged(bool state);
  void fovChanged(GLfloat fov);
  void cameraPosChanged(const GLfloat *cameraPos);
  void cameraAimAndUpChanged(const GLfloat *cameraAim, const GLfloat *cameraUp);
  void clipPerspNearChanged(GLdouble clip);
  void clipOrthoNearChanged(GLdouble clip);

  void gammaChanged(bool state);
  void gammaLevelChanged(float exponent);

  void binWindowChanged(bool state);
  void binWindowIndexChanged(cuv_ubyte index);
  void binWindowPaletteChanged(cuv_ubyte index, const bin_window_palette *palData);
};

inline unsigned int GLRenderer::log2(unsigned int value)
{
  int answer = -1;

  for ( ; value ; value >>= 1)
    answer++;

  return (unsigned int) answer;
}

inline unsigned int GLRenderer::alignedLength(unsigned int lineLength,
                        unsigned int sizeOfType)
{
  // used to increase read/write pixel ops in OpenGL

  return ( (((sizeOfType * lineLength + ALIGN_ADD_AMT)
         >> ALIGN_SHIFT_AMT) << ALIGN_SHIFT_AMT) /
       sizeOfType );
}

inline void GLRenderer::setTwoSidedQuiet(bool toggle)
{
  if (toggle)
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

inline void GLRenderer::setSmoothShadedQuiet(bool toggle)
{
  if (toggle)
    glShadeModel(GL_SMOOTH);
  else
    glShadeModel(GL_FLAT);
}

inline void GLRenderer::setWireframeQuiet(bool toggle)
{
  if (toggle)
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  else
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

inline void GLRenderer::setLightingQuiet(bool toggle)
{
  if (toggle)
    glEnable(GL_LIGHTING);
  else
    glDisable(GL_LIGHTING);
}

inline void GLRenderer::cameraFix()
{
  GLfloat temp[3];

  vnormalize(viewState.cameraAim);

  vcross(temp, viewState.cameraAim, viewState.cameraUp);
  vcross(viewState.cameraUp, temp, viewState.cameraAim);

  vnormalize(viewState.cameraUp);

  return;
}

inline void GLRenderer::reloadSceneAttribs()
{
  sceneHasChanged = true;
}

inline void GLRenderer::myFrustum(proj_info project, GLdouble pixdx, GLdouble pixdy)
{
  pixdx *= (project.left-project.right) / (GLdouble) glWindowWidth;
  pixdy *= (project.bottom-project.top) / (GLdouble) glWindowHeight;

  glFrustum((project.left+pixdx), (project.right+pixdx), (project.bottom+pixdy),
        (project.top+pixdy), project.nearp, project.farp);

  return;
}

inline proj_info GLRenderer::myPerspective(GLdouble fovy, GLdouble aspect, GLdouble nearp, GLdouble farp)
{
  proj_info project;

  project.top = nearp * tan(fovy * M_PI / 360.0);
  project.right = project.top * aspect;
  project.bottom = -project.top;
  project.left = -project.right;
  project.nearp = nearp;
  project.farp = farp;

  return project;
}

inline void GLRenderer::myOrtho(proj_info project, GLdouble pixdx, GLdouble pixdy)
{
  pixdx *= (project.left-project.right) / (GLdouble) glWindowWidth;
  pixdy *= (project.bottom-project.top) / (GLdouble) glWindowHeight;

  glOrtho((project.left+pixdx), (project.right+pixdx), (project.bottom+pixdy),
          (project.top+pixdy), project.nearp, project.farp);

  return;
}

inline proj_info GLRenderer::myOrthoFov(GLdouble fovy, GLdouble distz, GLdouble aspect,
                                        GLdouble nearp, GLdouble farp)
{
  proj_info project;

  project.top = distz * tan(fovy * M_PI / 360.0);
  project.right = project.top * aspect;
  project.bottom = -project.top;
  project.left = -project.right;
  project.nearp = nearp;
  project.farp = farp;

  return project;
}

inline void GLRenderer::redrawDoc()
{
  cachedImageExists = false;
  updateGL();
}

inline int GLRenderer::getLightSources(){
  return viewState.lightSources;
}

inline bool GLRenderer::fixedLight(int index){
  return viewState.fixedLightSource[index];
}

inline void GLRenderer::setFixedLight(int index,bool fixed){
  viewState.fixedLightSource[index] = fixed;
}

inline bool GLRenderer::hasTranslatedLight(int index){
  GLfloat toFill[3];
  toFill[0] = toFill[1]= toFill[2] = 0;
  getTranslateLightf(index,toFill);
  return (toFill[0]||toFill[1]||toFill[2]);
}

inline void GLRenderer::getTranslateLightf(int index,cuv_float toFill[3]) const
{
  if(index<MAX_LIGHTSOURCES){
    toFill[0]=viewState.lightXTranslate[index];
    toFill[1]=viewState.lightYTranslate[index];
    toFill[2]=viewState.lightZTranslate[index];
  }
}

inline void GLRenderer::drawLight(int index){
  GLfloat lightPosition[4] = {viewState.lightXTranslate[index],
            viewState.lightYTranslate[index],
            viewState.lightZTranslate[index],
            0.0F};
  glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

inline void GLRenderer::resetTranslateLight(int index){
  if(index<MAX_LIGHTSOURCES){
    viewState.lightXTranslate[index]=0;
    viewState.lightYTranslate[index]=0;
    viewState.lightZTranslate[index]=1;
    drawLight(index);
  }
}

inline void GLRenderer::translateLight(int index,cuv_float x,cuv_float y,cuv_float z){
  if( index<MAX_LIGHTSOURCES && (x||y||z) ){
    viewState.lightXTranslate[index]+=x;
    viewState.lightYTranslate[index]+=y;
    viewState.lightZTranslate[index]+=z;
    drawLight(index);
  }
}

inline bool GLRenderer::getEditLightMode(int index){
  return (index<MAX_LIGHTSOURCES) ? viewState.lightEditing[index]:false;
}

inline void GLRenderer::setEditLightMode(int index,bool edit){
  if(index<MAX_LIGHTSOURCES)
    viewState.lightEditing[index]=edit;
}

inline void GLRenderer::showLightPosition(bool show){
    viewState.showLightPosition=show;
}


inline void GLRenderer::setLightColor(int index, const GLfloat lightColor[3])
{
  if (vunequal3(viewState.lightColor[index], lightColor))
    {
      vcopy3(viewState.lightColor[index], lightColor);
      cachedImageExists = false;
    }
  GLenum light;
  switch(index){
    case 0: 
      light = GL_LIGHT0; break;
    case 1: 
      light = GL_LIGHT1; break;
    case 2: 
      light = GL_LIGHT2; break;
    case 3: 
      light = GL_LIGHT3; break;
    case 4: 
      light = GL_LIGHT4; break;
    case 5: 
      light = GL_LIGHT5; break;
    case 6: 
      light = GL_LIGHT6; break;
    case 7: 
      light = GL_LIGHT7; break;
    default:
      light = GL_LIGHT0; break;
  };

  GLfloat light_ambient[] = { viewState.ambient* lightColor[0],
                              viewState.ambient* lightColor[1],
                              viewState.ambient* lightColor[2], 1.0 };
  glLightfv(light, GL_AMBIENT, light_ambient);
  GLfloat light_diffuse[] = { viewState.diffuse* lightColor[0],
                              viewState.diffuse* lightColor[1],
                              viewState.diffuse* lightColor[2], 1.0 };
  glLightfv(light, GL_DIFFUSE, light_diffuse);
  GLfloat light_specular[] = { viewState.specular* lightColor[0],
                               viewState.specular* lightColor[1],
                               viewState.specular* lightColor[2], 1.0 };
  glLightfv(light, GL_SPECULAR, light_specular);
}

inline void GLRenderer::lightColor(int index, GLfloat toFill[3]) const
{
  vcopy3(toFill, viewState.lightColor[index]);
}


inline void GLRenderer::setThreeButtonMouse(bool toggle)
{
  if (glrendererOptions.useThreeButtons != toggle)
  {
    glrendererOptions.useThreeButtons = toggle;

    emit threeButtonMouseChanged(toggle);
  }
}

inline bool GLRenderer::threeButtonMouse() const
{
  return glrendererOptions.useThreeButtons;
}

inline void GLRenderer::setCachedImage(bool toggle)
{
  if (glrendererOptions.useCachedImage != toggle)
  {
    glrendererOptions.useCachedImage = toggle;

    if (toggle)
    {
      try
      {
        cacheImageData = new GLubyte[cacheImageLineLength * cacheImageHeight];
      }
      catch (...)
      {
        // cacheImageData was 0 and will stay that way
        qWarning("Could not allocate array for additional buffer.");
      }

      // cachedImageExists should already be false
    }
    else
    {
      cachedImageExists = false;

      if (cacheImageData)
      {
        delete[] cacheImageData; // clean up the old data
        cacheImageData = 0;
      }
    }

    emit cachedImageChanged(toggle);
  }
}

inline bool GLRenderer::cachedImage() const
{
  return glrendererOptions.useCachedImage;
}


inline void GLRenderer::setBoxWhileMoving(bool toggle)
{
  if (glrendererOptions.drawBoxWhileMoving != toggle)
  {
    glrendererOptions.drawBoxWhileMoving = toggle;
    emit boxWhileMovingChanged(toggle);
  }
}

inline void GLRenderer::setDrawAxis(bool toggle)
{
  if (glrendererOptions.drawAxis != toggle)
  {
    glrendererOptions.drawAxis = toggle;
  }
}

inline void GLRenderer::setDrawAxisOrigin(bool toggle)
{
  if (glrendererOptions.drawAxisOrigin != toggle)
  {
    glrendererOptions.drawAxisOrigin = toggle;
  }
}


inline bool GLRenderer::boxWhileMoving() const
{
  return glrendererOptions.drawBoxWhileMoving;
}

inline bool GLRenderer::getDrawAxis() const
{
  return glrendererOptions.drawAxis;
}

inline bool GLRenderer::getDrawAxisOrigin() const
{
  return glrendererOptions.drawAxisOrigin;
}


inline void GLRenderer::setDrawBoxAlways(bool toggle)
{
  if (glrendererOptions.drawBoxAlways != toggle)
    glrendererOptions.drawBoxAlways = toggle;
}

inline bool GLRenderer::drawBoxAlways() const
{
  return glrendererOptions.drawBoxAlways;
}

inline void GLRenderer::setTwoSided(bool toggle)
{
  makeCurrent();

  if (viewState.twoSidedEnabled != toggle)
  {
    viewState.twoSidedEnabled = toggle;

    if (toggle)
    {
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
      glDisable(GL_CULL_FACE);
    }
    else
    {
      glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
      glEnable(GL_CULL_FACE);
    }

    cachedImageExists = false;
    emit twoSidedChanged(toggle);
  }
}

inline bool GLRenderer::twoSided() const
{
  return viewState.twoSidedEnabled;
}


inline void GLRenderer::setSmoothShaded(bool toggle)
{
  makeCurrent();

  if (viewState.smoothShadedEnabled != toggle)
  {
    viewState.smoothShadedEnabled = toggle;

    if (toggle)
      glShadeModel(GL_SMOOTH);
    else
      glShadeModel(GL_FLAT);

    cachedImageExists = false;
    emit smoothShadedChanged(toggle);
  }
}

inline bool GLRenderer::smoothShaded() const
{
  return viewState.smoothShadedEnabled;
}


inline void GLRenderer::setTransparency(bool toggle)
{
  if (viewState.transparencyEnabled != toggle)
  {
    viewState.transparencyEnabled = toggle;

    cachedImageExists = false;
    emit transparencyChanged(toggle);
  }
}

inline bool GLRenderer::transparency() const
{
  return viewState.transparencyEnabled;
}


inline void GLRenderer::setAntialias(bool toggle)
{
  if (viewState.antialiasEnabled != toggle)
  {
    viewState.antialiasEnabled = toggle;

    cachedImageExists = false;
    emit antialiasChanged(toggle);
  }
}

inline bool GLRenderer::antialias() const
{
  return viewState.antialiasEnabled;
}


inline void GLRenderer::setSphereTess(cuv_ubyte sphereTess)
{
  if (viewState.sphereTess != sphereTess)
  {
    setSphereTessQuietForce(sphereTess);

    emit sphereTessChanged(sphereTess);
  }
}

inline cuv_ubyte GLRenderer::sphereTess() const
{
  return viewState.sphereTess;
}


inline void GLRenderer::setOutline(bool toggle)
{
  if (viewState.outlineEnabled != toggle)
  {
    viewState.outlineEnabled = toggle;

    cachedImageExists = false;
    emit outlineChanged(toggle);
  }
}

inline bool GLRenderer::outline() const
{
  return viewState.outlineEnabled;
}


inline void GLRenderer::setWireframe(bool toggle)
{
  makeCurrent();

  if (viewState.wireframeEnabled != toggle)
  {
    viewState.wireframeEnabled = toggle;

    if (toggle)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    cachedImageExists = false;
    emit wireframeChanged(toggle);
  }
}

inline bool GLRenderer::wireframe() const
{
  return viewState.wireframeEnabled;
}


inline void GLRenderer::setOpaqueWireframe(bool toggle)
{
  if (viewState.opaqueWireframeEnabled != toggle)
  {
    viewState.opaqueWireframeEnabled = toggle;

    cachedImageExists = false;
    emit opaqueWireframeChanged(toggle);
  }
}

inline bool GLRenderer::opaqueWireframe() const
{
  return viewState.opaqueWireframeEnabled;
}


inline void GLRenderer::setLineWidth(GLfloat lineWidth)
{
  makeCurrent();

  if (viewState.lineWidth != lineWidth)
  {
    if (lineWidth > 1.0F)
      viewState.lineWidth = lineWidth;
    else
      viewState.lineWidth = 1.0F;

    glLineWidth(viewState.lineWidth);
    glPointSize(viewState.lineWidth);
    resizeGL(width(), height());

    cachedImageExists = false;
    emit lineWidthChanged(viewState.lineWidth);
  }
}

inline GLfloat GLRenderer::lineWidth() const
{
  return viewState.lineWidth;
}


inline void GLRenderer::setOutlineColor(const GLfloat outlineColor[3])
{
  if (vunequal3(viewState.outlineColor, outlineColor))
  {
    vcopy3(viewState.outlineColor, outlineColor);

    cachedImageExists = false;
    emit outlineColorChanged(viewState.outlineColor);
  }
}

inline void GLRenderer::outlineColor(GLfloat toFill[3]) const
{
  vcopy3(toFill, viewState.outlineColor);
}


inline void GLRenderer::setLight(bool toggle)
{
  makeCurrent();

  if (viewState.lightEnabled != toggle)
  {
    viewState.lightEnabled = toggle;

    if (toggle)
      glEnable(GL_LIGHT0);
    else
      glDisable(GL_LIGHT0);

    cachedImageExists = false;
    emit lightChanged(toggle);
  }
}

inline bool GLRenderer::light() const
{
  return viewState.lightEnabled;
}


inline void GLRenderer::setLighting(bool toggle)
{
  makeCurrent();

  if (viewState.lightingEnabled != toggle)
  {
    viewState.lightingEnabled = toggle;

    if (toggle)
      glEnable(GL_LIGHTING);
    else
      glDisable(GL_LIGHTING);

    cachedImageExists = false;
    emit lightingChanged(toggle);
  }
}

inline bool GLRenderer::lighting() const
{
  return viewState.lightingEnabled;
}


inline void GLRenderer::setAmbient(GLfloat brightness)
{
  makeCurrent();

  if (viewState.ambient != brightness)
  {
    if (brightness < 0.0F)
      viewState.ambient = 0.0F;
    else
      viewState.ambient = brightness;

    GLfloat tempAmb[4] = {viewState.ambient, viewState.ambient,
                viewState.ambient, 1.0F};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, tempAmb);

    cachedImageExists = false;
    emit ambientChanged(viewState.ambient);
  }
}

inline GLfloat GLRenderer::ambient() const
{
  return viewState.ambient;
}


inline void GLRenderer::setDiffuse(GLfloat brightness)
{
  makeCurrent();

  if (viewState.diffuse != brightness)
  {
    if (brightness < 0.0F)
      viewState.diffuse = 0.0F;
    else
      viewState.diffuse = brightness;

    GLfloat tempDiff[4] = {viewState.diffuse, viewState.diffuse,
                 viewState.diffuse, 1.0F};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, tempDiff);

    cachedImageExists = false;
    emit diffuseChanged(viewState.diffuse);
  }
}

inline GLfloat GLRenderer::diffuse() const
{
  return viewState.diffuse;
}


inline void GLRenderer::setBackground(GLfloat brightness)
{
  makeCurrent();

  if (viewState.background != brightness)
  {
    if (brightness < 0.0F)
      viewState.background = 0.0F;
    else if (brightness > 1.0F)
      viewState.background = 1.0F;
    else
      viewState.background = brightness;

    glClearColor(viewState.background, viewState.background,
           viewState.background, 1.0);

    float bg = viewState.background;
    GLfloat fogColor[4] = {bg, bg, bg, 1.0};
    glFogfv(GL_FOG_COLOR, fogColor);

    cachedImageExists = false;
    emit backgroundChanged(viewState.background);
  }
}

inline GLfloat GLRenderer::background() const
{
  return viewState.background;
}


inline void GLRenderer::setPerspective(bool toggle)
{
  if (viewState.perspectiveEnabled != toggle)
  {
    viewState.perspectiveEnabled = toggle;
    updateProjection();

    cachedImageExists = false;
    emit perspectiveChanged(toggle);
  }
}

inline bool GLRenderer::perspective() const
{
  return viewState.perspectiveEnabled;
}


inline void GLRenderer::setFov(GLfloat fov)
{
  if (viewState.fov != fov)
  {
    if (fov < 1.0F)
      viewState.fov = 1.0F;
    else if (fov > 179.0F)
      viewState.fov = 179.0F;
    else
      viewState.fov = fov;

    newProjection = true;

    cachedImageExists = false;
    emit fovChanged(viewState.fov);
  }
}

inline GLfloat GLRenderer::fov() const
{
  return viewState.fov;
}


inline void GLRenderer::cameraPos(GLfloat toFill[3]) const
{
  vcopy3(toFill, viewState.cameraPos);
}

inline void GLRenderer::cameraAim(GLfloat toFill[3]) const
{
  vcopy3(toFill, viewState.cameraAim);
}

inline void GLRenderer::cameraUp(GLfloat toFill[3]) const
{
  vcopy3(toFill, viewState.cameraUp);
}


inline void GLRenderer::setClipPerspNear(GLdouble clipPerspNear)
{
  if (viewState.clipPerspNear != clipPerspNear)
  {
    viewState.clipPerspNear = clipPerspNear;
    if (viewState.perspectiveEnabled)
      updateProjection();

    cachedImageExists = false;
    emit clipPerspNearChanged(clipPerspNear);
  }
}

inline GLdouble GLRenderer::clipPerspNear() const
{
  return viewState.clipPerspNear;
}


inline void GLRenderer::setClipOrthoNear(GLdouble clipOrthoNear)
{
  if (viewState.clipOrthoNear != clipOrthoNear)
  {
    viewState.clipOrthoNear = clipOrthoNear;
    if (!viewState.perspectiveEnabled)
      updateProjection();

    cachedImageExists = false;
    emit clipOrthoNearChanged(clipOrthoNear);
  }
}

inline GLdouble GLRenderer::clipOrthoNear() const
{
  return viewState.clipOrthoNear;
}


inline void GLRenderer::setGamma(bool toggle)
{
  if (viewState.gammaEnabled != toggle)
  {
    viewState.gammaEnabled = toggle;

    cachedImageExists = false;
    emit gammaChanged(toggle);
  }
}

inline bool GLRenderer::gamma() const
{
  return viewState.gammaEnabled;
}

inline float GLRenderer::gammaLevel() const
{
  return viewState.gammaExponent;
}

inline float GLRenderer::specular() const
{
  return viewState.specular;
}

inline void GLRenderer::setFog(bool toggle)
{
  if (viewState.fogEnabled != toggle)
  {
    viewState.fogEnabled = toggle;

    if (toggle)
      glEnable(GL_FOG);
    else
      glDisable(GL_FOG);

    cachedImageExists = false;
  }
}

inline bool GLRenderer::fog()
{
  return viewState.fogEnabled;
}
//see glrenderer.cpp for set foglevel
inline float GLRenderer::fogLevel() 
{
  return viewState.fogLevel;
}


inline void GLRenderer::setBinWindow(bool toggle)
{
  if (viewState.binWindowEnabled != toggle)
  {
    viewState.binWindowEnabled = toggle;

    cachedImageExists = false;
    emit binWindowChanged(toggle);
  }
}

inline bool GLRenderer::binWindow() const
{
  return viewState.binWindowEnabled;
}

inline void GLRenderer::setBinWindowIndex(cuv_ubyte index)
{
  if (viewState.binWindowIndex != index)
  {
    viewState.binWindowIndex = index;

    cachedImageExists = false;
    emit binWindowIndexChanged(index);
  }
}

inline cuv_ubyte GLRenderer::binWindowIndex() const
{
  return viewState.binWindowIndex;
}

inline cuv_uint GLRenderer::binWindowColors(cuv_ubyte index) const
{
  return viewState.binWindowCount[index];
}

inline QString GLRenderer::binWindowName(cuv_ubyte index) const
{
  return *viewState.binWindowName[index];
}

inline void GLRenderer::freeBinWindowPalette(bin_window_palette *toFree) const
{
  if (toFree)
  {
    if (toFree->fromPaletteStart)
      delete[] toFree->fromPaletteStart;

    // need not touch the rest (look at the creation code)

    delete toFree;
  }
}

#endif
