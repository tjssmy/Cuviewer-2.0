/** script.cpp
  * Carleton University Department of Electronics
  * Copyright (C) 2001 Bryan Wan and Cliff Dugal
  */
#include <qapplication.h>

#include <QPixmap>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QVariant>
#include <qgl.h>

#include "include/cuvcommonstructs.h"
#include "script.h"
#include "viewdata.h"

/**
    Constructor initilize Script
    @param: g  The number of gradations to generate
    @param: type  The bin pattern to produce
  */
Script::Script(QFile * file,CUViewDoc * cvd)
{
	scriptfile = file;
	cuviewDoc = cvd;
	allowLoad = true;
}

Script::~Script()
{
}

void Script::setFileLoad(bool allow)
{
  allowLoad = allow;
}

QStringList Script::getFilenameList()
{
  return filenamelist;
}

void Script::setFilenameList(QStringList filelist)
{
  filenamelist=filelist;
}

/**
    @brief Writes the script to a file

    The writeScript method writes all settings related to the cuviewDoc to a file.

    The settings written to the file are listed below in the order which it was written:
     - Camera position
     - Camera aim
     - Camera up
     - Outline colour
     - Outline mode
     - Twosided mode
     - Antialias mode
     - Perspective mode
     - Smooth shading mode
     - Transparency mode
     - Gamma mode
     - Wireframe mode
     - Opaque wireframe mode
     - Ambient lighting
     - Diffuse lighting
     -
  */
void Script::writeScript()
{
//write everything to scriptfile
  QString filename = scriptfile->fileName();
	scriptfile->remove();
  scriptfile->setFileName(filename);
  qDebug( "writing script file: %s", qPrintable(scriptfile->fileName()));
	if(scriptfile->open(QIODevice::ReadWrite)){
		GLfloat* toFill = new GLfloat[3];
		
    QTextStream ts(scriptfile);
		ts << QString("script data for cuviewer, do not edit.\n");
		cuviewDoc->cameraPos(toFill);
		ts << QString("camera_position_xyz %1 %2 %3\n")
			.arg((float)toFill[0]).arg((float)toFill[1]).arg((float)toFill[2]);
		cuviewDoc->cameraAim(toFill);
		ts << QString("camera_aim_xyz %1 %2 %3\n")
			.arg((float)toFill[0]).arg((float)toFill[1]).arg((float)toFill[2]);
		cuviewDoc->cameraUp(toFill);
		ts << QString("camera_up_xyz %1 %2 %3\n")
			.arg((float)toFill[0]).arg((float)toFill[1]).arg((float)toFill[2]);
		cuviewDoc->outlineColor(toFill);
		ts << QString("outlinecolor_rbg %1 %2 %3\n")
			.arg((float)toFill[0]).arg((float)toFill[1]).arg((float)toFill[2]);
		
		ts << QString("outline %1\n").arg((int)cuviewDoc->outline());
		ts << QString("twosided %1\n").arg((int)cuviewDoc->twoSided());
		ts << QString("aa %1\n").arg((int)cuviewDoc->antialias());
		ts << QString("view %1\n").arg((int)cuviewDoc->perspective());
		ts << QString("smoothshaded %1\n").arg((int)cuviewDoc->smoothShaded());
		ts << QString("transparency %1\n").arg((int)cuviewDoc->transparency());
//		ts << QString("fog %1\n").arg((int)cuviewDoc->fog());
		ts << QString("gamma %1\n").arg((int)cuviewDoc->gamma());
		ts << QString("wireframe %1\n").arg((int)cuviewDoc->wireframe());
		ts << QString("opaquewf %1\n").arg((int)cuviewDoc->opaqueWireframe());
		ts << QString("lighting %1\n").arg((int)cuviewDoc->lighting());
		ts << QString("diffuse %1\n").arg((int)cuviewDoc->light());
		ts << QString("binWindow %1\n").arg((int)cuviewDoc->binWindow());
		
		ts << QString("linewidth %1\n").arg((int)cuviewDoc->lineWidth());
		ts << QString("spheretess %1\n").arg((int)cuviewDoc->sphereTess());
		ts << QString("clipping %1\n").arg((double)cuviewDoc->clipPerspNear());
		ts << QString("fov %1\n").arg((float)cuviewDoc->fov());
		ts << QString("ambientv %1\n").arg((float)cuviewDoc->ambient());
		ts << QString("diffusev %1\n").arg((float)cuviewDoc->diffuse());
		ts << QString("background %1\n").arg((float)cuviewDoc->background());
		ts << QString("gammav %1\n").arg((float)cuviewDoc->gammaLevel());
		ts << QString("binwindowindex %1\n").arg((int)cuviewDoc->binWindowIndex());

		ts << "loadfilelist " << QString::number(filenamelist.count()) << "\n";
		for ( QStringList::Iterator it = filenamelist.begin(); 
		      it != filenamelist.end(); ++it ) {
		  ts << *it << "\n";
		}

		int translateCount=0,scaleCount=0,rotateCount=0 ,tileCount=0;

		for(int i=0;i<cuviewDoc->scenes();i++){
		  if(cuviewDoc->hasTiles(i))
		    ++tileCount;
		  if(cuviewDoc->hasTranslated(i))
		    ++translateCount;
		  if(cuviewDoc->hasScaled(i))
		    ++scaleCount;
		  if(cuviewDoc->hasRotated(i))
		    ++rotateCount;
		}

		int toFilli[3];
		ts << QString("tile_count_index_xyz %1 ").arg(tileCount);
		for(int i=0;i<cuviewDoc->scenes();i++)
		  if(cuviewDoc->hasTiles(i)){
		    cuviewDoc->getTiles(i,toFilli,toFill);
		    ts << QString("%1 %2 %3 %4 %5 %6 %7 ").arg((int)i)
		      .arg(toFilli[0]).arg(toFilli[1]).arg(toFilli[2])
		      .arg(toFill[0]).arg(toFill[1]).arg(toFill[2]);
		  }
		ts << "\n";

		ts << QString("translate_count_index_xyz %1 ").arg(translateCount);
		for(int i=0;i<cuviewDoc->scenes();i++)
		  if(cuviewDoc->hasTranslated(i)){
		    cuviewDoc->getTranslatef(i,toFill);
		    ts << QString("%1 %2 %3 %4 ").arg((int)i)
		      .arg(toFill[0]).arg(toFill[1]).arg(toFill[2]);
		  }
		ts << "\n";

		ts << QString("scale_count_index_xyz %1 ").arg(scaleCount);
		for(int i=0;i<cuviewDoc->scenes();i++)
		  if(cuviewDoc->hasScaled(i)){
		    cuviewDoc->getScalef(i,toFill);
		    ts << QString("%1 %2 %3 %4 ").arg((int)i)
		      .arg(toFill[0]).arg(toFill[1]).arg(toFill[2]);
		  }
		ts << "\n";

		ts << QString("rotate_count_index_xyz %1 ").arg(rotateCount);
		for(int i=0;i<cuviewDoc->scenes();i++)
		  if(cuviewDoc->hasRotated(i)){
		    cuviewDoc->getRotatef(i,toFill);
		    ts << QString("%1 %2 %3 %4 ").arg((int)i)
		      .arg(toFill[0]).arg(toFill[1]).arg(toFill[2]);
		  }
		ts << "\n";

		ts << QString("scene_count_visibility %1 ")
              .arg((int)cuviewDoc->scenes());
    for(int i=0;i<cuviewDoc->scenes();i++){
      ts << " " << QString::number((int)cuviewDoc->isVisible(i));
    }
		ts << "\n";

		ts << QString("light_sources %1 ")
		  .arg((int)cuviewDoc->getLightSources());
		for(int i=0;i<cuviewDoc->getLightSources();i++){
		  cuviewDoc->getTranslateLight(i,toFill);
		  ts << QString(" %1 %2 %3 %4 ").arg(cuviewDoc->specular())
		    .arg(toFill[0]).arg(toFill[1]).arg(toFill[2]-1);
		}
		ts << "\n";

		ts << QString("window_size %1 %2")
		  .arg((int)  ((QWidget*)cuviewDoc->parent())->width())
		  .arg((int)  ((QWidget*)cuviewDoc->parent())->height());
		
		delete toFill;
	}
}

/**
    @brief Reads a script

    The readScript method reads a script file that was generated from the writeScript method.
  */
void Script::readScript()
{  
  //Load script file
  if(scriptfile->exists()){
    if(scriptfile->open(QIODevice::ReadOnly)){
      QTextStream ts(scriptfile);
      GLfloat* toFill = new GLfloat[3], *toFill2 = new GLfloat[3];
      QString pad;
      double value,value1,value2;
      ts.readLine();
      if(scriptfile->atEnd()){
        qDebug("script file reached end to soon");
        return;
      }
			
      //Read all cuviewDoc settings.
      ts >> pad >> toFill[0] >> toFill[1] >> toFill[2];
      cuviewDoc->cameraTranslate(toFill,true);
			
      ts >> pad >> toFill[0] >> toFill[1] >> toFill[2];
      ts >> pad >> toFill2[0] >> toFill2[1] >> toFill2[2];
      cuviewDoc->setCameraAimAndUp(toFill,toFill2);
			
      ts >> pad >> toFill[0] >> toFill[1] >> toFill[2];
      cuviewDoc->setOutlineColor(toFill);
      ts >> pad >> value;
      cuviewDoc->setOutline((bool)value);
      ts >> pad >> value;
      cuviewDoc->setTwoSided((bool)value);
      ts >> pad >> value;
      cuviewDoc->setAntialias((bool)value);
      ts >> pad >> value;
      cuviewDoc->setPerspective((bool)value);
      ts >> pad >> value;
      cuviewDoc->setSmoothShaded((bool)value);
      ts >> pad >> value;
      cuviewDoc->setTransparency((bool)value);
      //			ts >> pad >> value.asInt();
      //			cuviewDoc->setFog(value.toBool());
      ts >> pad >> value;
      cuviewDoc->setGamma((bool)value);
      ts >> pad >> value;
      cuviewDoc->setWireframe((bool)value);
      ts >> pad >> value;
      cuviewDoc->setOpaqueWireframe((bool)value);
      ts >> pad >> value;
      cuviewDoc->setLighting((bool)value);
      ts >> pad >> value;
      cuviewDoc->setLight((bool)value);
      ts >> pad >> value;
      cuviewDoc->setBinWindow((bool)value);
      ts >> pad >> value;
      cuviewDoc->setLineWidth((int)value);
      ts >> pad >> value;
      cuviewDoc->setSphereTess((unsigned char)value);
      ts >> pad >> value;
      cuviewDoc->setClipPerspNear(value);
      cuviewDoc->setClipOrthoNear(value/5.0);
      ts >> pad >> value;
      cuviewDoc->setFov(value);
      ts >> pad >> value;
      cuviewDoc->setAmbient(value);
      ts >> pad >> value;
      cuviewDoc->setDiffuse(value);
      ts >> pad >> value;
      cuviewDoc->setBackground(value);
      ts >> pad >> value;
      cuviewDoc->setGammaLevel(value);
      ts >> pad >> value;
      cuviewDoc->setBinWindowIndex((unsigned char)value);
      if(!cuviewDoc->binWindowColors((unsigned char)value)){
        qDebug("Invalid bin palette index");
      }

      int count=0, scene=0;
      bool oldscriptformat=true;
      ts >> pad; // loadfilelist
      if (pad=="loadfilelist"){
        oldscriptformat=false;
        ts >> count;
        QString temp;
        temp = ts.readLine();

        if(!count)
          return;
        for(int i=0;i<count;i++){
          temp = ts.readLine();
          if(allowLoad)  filenamelist += temp;
        }

        count=0; //reset count
      } 
	  
      int toFilli[3];
      //read tiles
      cuviewDoc->setEditMode(true);
      if (!oldscriptformat){
        ts >> pad;
      }
      ts >> count;
      for(int i=0;i<count;i++){
        ts >> scene >> toFilli[0] >> toFilli[1] >> toFilli[2];
        ts >> value >> value1 >> value2;
        toFill[0] = value;
        toFill[1] = value1;
        toFill[2] = value2;
        cuviewDoc->setSceneEditing(scene,true);
        cuviewDoc->tileScenes(scene,toFilli[0],toFilli[1],toFilli[2],
                  toFill[0],toFill[1],toFill[2],false);
        cuviewDoc->setSceneEditing(scene,false);
      }
      //read translated scenes
      ts >> pad >> count;
      for(int i=0;i<count;i++){
        ts >> scene >> value >> value1 >> value2;
        toFill[0] = value;
        toFill[1] = value1;
        toFill[2] = value2;
        cuviewDoc->setSceneEditing(scene,true);
        cuviewDoc->translateScene(scene,toFill[0],toFill[1],toFill[2]);
        cuviewDoc->setSceneEditing(scene,false);
      }
      //read scaled scenes
      ts >> pad >> count;
      for(int i=0;i<count;i++){
        ts >> scene >> value >> value1 >> value2;
        toFill[0] = value;
        toFill[1] = value1;
        toFill[2] = value2;
        cuviewDoc->setSceneEditing(scene,true);
        cuviewDoc->scaleScene(scene,toFill[0],toFill[1],toFill[2]);
        cuviewDoc->setSceneEditing(scene,false);
      }
      //read rotated scenes
      ts >> pad >> count;
      for(int i=0;i<count;i++){
        ts >> scene >> value >> value1 >> value2;
        toFill[0] = value;
        toFill[1] = value1;
        toFill[2] = value2;
        cuviewDoc->setSceneEditing(scene,true);
        cuviewDoc->rotateScene(scene,toFill[0],toFill[1],toFill[2]);
        cuviewDoc->setSceneEditing(scene,false);
      }
      //read scene visibility
      ts >> pad >> value;
      int scenes = (int)value;
      for(int i=0;i<scenes;i++){
        ts >> value;
        cuviewDoc->setSceneVisible(i,(bool)value);
      }
      //read lightsources num specular_shine position[3]
      bool editing;
      ts >> pad >> value;
      int lightsources = (int)value;
      for(int i=0;i<lightsources;i++){
        ts >> value;
        cuviewDoc->setSpecular(value);
        ts >> value >> value1 >> value2;
        toFill[0] = value;
        toFill[1] = value1;
        toFill[2] = value2;
        editing = cuviewDoc->getEditLightMode(i);
        cuviewDoc->setEditLightMode(i,true);
        cuviewDoc->translateLight(i,toFill[0],toFill[1],toFill[2]);
        cuviewDoc->setEditLightMode(i,editing);
      }

      cuviewDoc->setEditMode(false);

      ts >> pad >> value;
      int width = (int)value;
      ts >> value;
      int height = (int)value;
      ((QWidget*)cuviewDoc->parent())->resize(width,height);

      delete toFill;
      delete toFill2;
    }
    else
      qDebug( "Can't open script file. Check file permissions");
  }  

}
