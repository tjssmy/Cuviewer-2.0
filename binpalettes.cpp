/** binpalettes.cpp
  * Carleton University Department of Electronics
  * Copyright (C) 2003 Bryan Wan
  *
  * Description of class: handles all the stuff for bin palettes
  * 
  */

#include <qgl.h>

#ifdef Q_OS_MAC
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#include <qcolordialog.h>
#include <qlineedit.h>
//#include <q3listview.h>
//#include <q3listbox.h>
#include <qspinbox.h>
#include <qslider.h>
#include <qstatusbar.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtoolbutton.h>
#include <qwidget.h>

#include "viewdata.h" 
#include "viewersettings.h"
#include "include/lessalloc.h"
#include "config.h" 
#include "binpalettes.h" 
#include "paletteview.h" 
#include "cuviewdoc/cuviewdoc.h" //the opengl window
#include "include/cuvtags.h"

BinPalettes::BinPalettes(QWidget *parentWidget, int prefBinPalettes,ViewerSettings *viewersettings)
  : QObject ( parentWidget )
{
  parent = parentWidget;
  binPalettes = prefBinPalettes;
  vs = viewersettings;
  cuviewDoc=NULL;
}

BinPalettes::~BinPalettes()
{
}

void BinPalettes::setCUViewDoc(CUViewDoc* cuviewdocument)
{
  cuviewDoc = cuviewdocument;
}

void BinPalettes::setBinPalettes(int maxNumber)
{
  binPalettes=maxNumber;
}

//tries to set palette index, otherwise iterate through list until 
//a valid palette is found
void BinPalettes::setPaletteIndex(int index){
  bool bwOn = index<=binPalettes;

  vs->colorBinPalette->blockSignals(TRUE);
  vs->bwBinPalette->blockSignals(TRUE);
  vs->colorBinPalette->setChecked(!bwOn);
  vs->bwBinPalette->setChecked(bwOn);
  vs->colorBinPalette->blockSignals(FALSE);
  vs->bwBinPalette->blockSignals(FALSE);

  if(cuviewDoc){
    int loop =0;
    for (int i=index; ; i++){
      if (i>256)
        i = 0;
      if (cuviewDoc->binWindowColors(i)){
        //found valid palette
        vs->paletteView->setBinPalette(cuviewDoc->binWindowPalette(i));
        cuviewDoc->setBinWindowIndex( i );
        cuviewDoc->redrawDoc();
        int bins = cuviewDoc->binWindowPalette( i )->count-2;
        ((QMainWindow*)parent)->statusBar()->showMessage(QString("Bin Palette index %1 with %2 bins")
                 .arg(i).arg(bins-2));
        vs->paletteIndexSpinBox->blockSignals(TRUE);
        vs->paletteIndexSpinBox->setValue(index);
        vs->paletteIndexSpinBox->blockSignals(FALSE);
        return;
      }
      if(loop>256){ //avoid infinite loop
        qWarning("didn't find any palettes");
        return;
      }
      loop++;
    }
    cuviewDoc->redrawDoc();
  }
}

//startvalue:int ranges from 4 to 100%
void BinPalettes::setStartPaletteValue(int startvalue)
{
  vs->bpStart->blockSignals(true);
  vs->bpStart->setValue((int)(startvalue*2.55)); //slider range 11-255
  vs->bpStart->blockSignals(false);
  setBinPaletteStart();
}

//endvalue:int ranges from 0 to 96%
void BinPalettes::setEndPaletteValue(int endvalue)
{
  vs->bpEnd->blockSignals(true);
  vs->bpEnd->setValue((int)(endvalue*2.55)); //slider range 0-244
  vs->bpEnd->blockSignals(false);
  setBinPaletteEnd();
}

//slider released
void BinPalettes::setBinPaletteStart()
{
  int start=vs->bpStart->value(); //slider range 11-255
  vs->bpStartValueSpinBox->blockSignals(true);
  float value255=start/2.55; bool roundup=(value255-(int)value255)*10>=5;
  vs->bpStartValueSpinBox->setValue((int)value255+roundup); //spinbox range 4-100
  vs->bpStartValueSpinBox->blockSignals(false);
  if(vs->bpEnd->value()>=start-11){
    vs->bpEnd->setValue(start-11); //end is distanced from start by 11
    value255=(start-11)/2.55; roundup=(value255-(int)value255)*10>=5;
    vs->bpEndValueSpinBox->setValue((int)value255+roundup); //range 0-100
  }
  ((QMainWindow*)parent)->statusBar()->showMessage(QString("Bin palette upperbound %1")
             .arg(vs->bpStart->value()));
  vs->paletteView->setUpperBoundPercent(vs->bpStartValueSpinBox->value()); //spinbox range 4-100
  if(cuviewDoc){
    insertbinpalette();
    cuviewDoc->redrawDoc();
    paintBinPalette();
  }
}

//slider released
void BinPalettes::setBinPaletteEnd()
{
  int end=vs->bpEnd->value(); //slider range 0-244
  vs->bpEndValueSpinBox->blockSignals(true);
  float value255=end/2.55; bool roundup=(value255-(int)value255)*10>=5;
  vs->bpEndValueSpinBox->setValue((int)value255+roundup); //spinbox 0-96
  vs->bpEndValueSpinBox->blockSignals(false);
  if(vs->bpStart->value()<=end+11){
    vs->bpStart->setValue(end+11);
    value255=(end+11)/2.55; roundup=(value255-(int)value255)*10>=5;
    vs->bpStartValueSpinBox->setValue((int)value255+roundup);
  }
  ((QMainWindow*)parent)->statusBar()->showMessage(QString("Bin palette lowerbound %1")
             .arg(vs->bpEnd->value()));
  vs->paletteView->setLowerBoundPercent(vs->bpEndValueSpinBox->value()); //spinbox range 0-96
  if(cuviewDoc){
    insertbinpalette();
    cuviewDoc->redrawDoc();
    paintBinPalette();
  }
}

//create color mapping processing definitions
void BinPalettes::insertbinpalette()
{
  if(cuviewDoc){
    //loads up default binpalettes
    bin_window_palette * bwp;
    int stepVal = 0;
    int r,g,b;
    QColor qc;
    bwp = new bin_window_palette; //array seq:|rgb|rgb|rgb|.etc..
    bwp->fromPaletteStart = new GLubyte[256];
    bwp->fromPaletteEnd = new GLubyte[256];
    bwp->toPalette = new GLubyte[256];
  //binPalettes:int is specified in global preferences, max palette default is 16.
  //create a black/white and red-thru-blue palette
  //if you want to know how this works, do a internet search on rgb colour
  //triangles or on OPENGL's documentation on colour maps.
    for (int i=1;i<=2*binPalettes;i++) {
      //2+5,2+10,2+15..., 2+5,2+10,2+15...
      bwp->count=4+5*(i-(i<=binPalettes?0:binPalettes));
  //add map for black -> black
      bwp->fromPaletteStart[0] = 0;
      bwp->fromPaletteStart[1] = 0;
      bwp->fromPaletteStart[2] = 0;
      bwp->fromPaletteEnd[0] = 1;
      bwp->fromPaletteEnd[1] = 1;
      bwp->fromPaletteEnd[2] = 1;
      bwp->toPalette[0]   = 0;
      bwp->toPalette[1] = 0;
      bwp->toPalette[2] = 0;
  //white - >white
      bwp->fromPaletteStart[3] = 254;
      bwp->fromPaletteStart[4] = 254;
      bwp->fromPaletteStart[5] = 254;
      bwp->fromPaletteEnd[3] = 255;
      bwp->fromPaletteEnd[4] = 255;
      bwp->fromPaletteEnd[5] = 255;
      bwp->toPalette[3] = 255;
      bwp->toPalette[4] = 255;
      bwp->toPalette[5] = 255;
      int start=vs->bpStart->value(), end=vs->bpEnd->value();
      //slider range 11-255,          slider range 0-244
      for(int j=6;j<(int)(bwp->count)*3;j+=3){
        if(j==(int)(bwp->count-1)*3) //last bin
          stepVal = 255;
        else //bins=count-2
          stepVal=(255-start)+(start-end)*(j/3-2)/(bwp->count-4);

        bwp->fromPaletteStart[j] = 255-stepVal;
        bwp->fromPaletteStart[j+1] = 0;
        bwp->fromPaletteStart[j+2] = 0;
        bwp->fromPaletteEnd[j] = 255;
        bwp->fromPaletteEnd[j+1] = 255;
        bwp->fromPaletteEnd[j+2] = stepVal;
        //flips between 20% and 80% grey
        if(i<=binPalettes){
          bwp->toPalette[j]   = 51+153*(j%2);
          bwp->toPalette[j+1] = 51+153*(j%2);
          bwp->toPalette[j+2] = 51+153*(j%2);
        } else { //color spectrum
          qc.setHsv((j-6)*100/(bwp->count-3),255,255);
          qc.getRgb(&r,&g,&b);
          bwp->toPalette[j]   = r;
          bwp->toPalette[j+1] = g;
          bwp->toPalette[j+2] = b;
        }
      }
      cuviewDoc->setBinWindowPalette(i,bwp);
    }
    delete bwp->fromPaletteStart;
    delete bwp->fromPaletteEnd;
    delete bwp->toPalette;
  }
}

void BinPalettes::setPalette(){
  if(cuviewDoc){
    vs->paletteIndexSpinBox->setValue(vs->paletteIndexSpinBox->value() +
             (vs->bwBinPalette->isChecked()?-1:1)*
             binPalettes);
  }
  /*  else
    {
      vs->bwBinPalette->setOn(FALSE);
      vs->colorBinPalette->setOn(FALSE);
    }
  */
}

void BinPalettes::paintBinPalette()
{
  if(cuviewDoc && cuviewDoc->binWindow()) {
    vs->paletteView->paintPalette(vs,60,350);
  }
}
