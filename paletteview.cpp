/** paletteview.cpp
  * Carleton University Department of Electronics
  * Copyright (C) 2001 Bryan Wan and Cliff Dugal
  */
/*struct bin_window_palette
{
	cuv_uint count;
	QString name;
	GLubyte *fromPaletteStart;
	GLubyte *fromPaletteEnd;
	GLubyte *toPalette;
};*/

#include "paletteview.h"
#include <qpainter.h>
#include <qgl.h>

#include <QMouseEvent>
#include <QPaintEvent>
#include "include/cuvcommonstructs.h" //contains bin_window_palette

// using namespace Qt;
//public:
PaletteView::PaletteView( QWidget *parent)
  :  QWidget( parent)
{
  minvalue = -1;
  maxvalue = -1;
  lowerbound = 0;
  upperbound = 100;
  postFix = "";
  this->setPalette( QPalette( Qt::black, QColor( 200, 200, 200) ) );
  bwp = NULL;
  cursorOn = TRUE;
  setBackgroundColor(Qt::lightGray);
}

PaletteView::~PaletteView()
{
  delete bwp->fromPaletteStart;
  delete bwp->fromPaletteEnd;
  delete bwp->toPalette;
}

void PaletteView::setBinPalette(const bin_window_palette* bin)
{
  bwp=new bin_window_palette;
  bwp->count=bin->count;
  bwp->name=bin->name;
  bwp->fromPaletteStart = new GLubyte[1024];
  bwp->fromPaletteEnd = new GLubyte[1024];
  bwp->toPalette = new GLubyte[1024];
  for(int i=0;i<3*((int)bwp->count);i++){
    bwp->fromPaletteStart[i]=bin->fromPaletteStart[i];
    bwp->fromPaletteEnd[i]=bin->fromPaletteEnd[i];
    bwp->toPalette[i]=bin->toPalette[i];
  }
}

/** @fn setBackgroundColor
    @brief Changes the color of the background.

    The setBackgroundColor method changes the color of the
    background to backgroundcolor and changes the text
    to either white or black. The color of the text depends
    on the brightness of the given background color.

    For example, given a brightness scale of 0 to 255 where 0 is
    the least bright and 255 is the most bright, if the
    brightness is between 0 and 128 inclusive then the text color
    will be white. Otherwise the text color is black.

    @param backgroundcolor
  */
void PaletteView::setBackgroundColor(const QColor& backgroundcolor)
{
  bg = backgroundcolor;
  int h,s,v;
  bg.getHsv(&h,&s,&v);
  QColor textcolor;
  if(0<=v && v<=128){
    textcolor = Qt::white;
    qDebug("text white");
  }
  else {
    textcolor = Qt::black;
    qDebug("text black");
  }
  
  QPalette p = palette();
//  p.setColor(QColorGroup::Foreground, textcolor);
  p.setColor(QPalette::Text, textcolor);
//  p.setColor(QColorGroup::ButtonText, textcolor);
  this->setPalette(p);
  repaint();
}

void PaletteView::paintPalette(QPaintDevice* paintDevice,int width,int height, int x, int y)
{
  QPainter p( paintDevice ); //text, lines, rects
  p.setBrush( bg );
  p.drawRect( QRect(x,y,width,height) );
  QPalette pal = palette();
  p.setPen( pal.color(QPalette::Active, QPalette::WindowText) );

//  QPainter cbins( paintDevice ); //color bins
//  cbins.setPen( Qt::NoPen );

  float STEPDY = height/(float)(bwp->count-4); //widget dy step in pixels
  float len=maxvalue-minvalue;
  // Range will be within minrange and maxrange
  float minrange = minvalue + len*lowerbound/100.0;
  float maxrange = maxvalue - len*(100-upperbound)/100.0;
  float stepunit = (maxrange-minrange)/(float)(bwp->count-4);
  float palettescale; //for the scale
  //for the spacing of the scale, always keep text away 10% of the height
  int skip=1+(int)(0.10*height/STEPDY);
  int dy;
  
  for(int i=0;i<(int)bwp->count-4;i++){
//    cbins.setBrush( QColor( bwp->toPalette[3*i   + 9],
//                            bwp->toPalette[3*i+1 + 9],
//                            bwp->toPalette[3*i+2 + 9] ) );
//    cbins.drawRect( QRect(x+35, y+(int)(STEPDY*i), width-35, (int)(STEPDY)+1 ) );
    p.setBrush( QColor( bwp->toPalette[3*i   + 9],
                        bwp->toPalette[3*i+1 + 9],
                        bwp->toPalette[3*i+2 + 9] ) );
    p.drawRect( QRect(x+35, y+(int)(STEPDY*i), width-35, (int)(STEPDY)+1 ) );
    p.drawLine( x+35, y+(int)(STEPDY*i), x+38, y+(int)(STEPDY*i) );
    p.drawLine( x+width-3, y+(int)(STEPDY*i), x+width-1, y+(int)(STEPDY*i) );

    //spaces automatically text in palette bar
    dy = 5+(int)(i*STEPDY);
    if(!(i%skip)){
      palettescale = maxrange - i*stepunit;
      if( i==0 ){ //pad of 8px
        dy += 10;
      }
      /*bottom margin of 20 px AND top margin of 30px*/
      if( height-dy > 20 && (!i||dy>30) ){  //pad of 3px
        p.drawText( x+3, y+dy, QString::number((int)palettescale) );
      }
      p.drawLine( x+30, y+(int)(i*STEPDY), x+35, y+(int)(i*STEPDY) );
    }
  }
  p.drawText( x+3, y+height - 4, QString::number((int)minrange) + postFix );

  //draw black line borders
  p.setBrush( Qt::NoBrush );
  p.drawRect( QRect(x,y,width,height) );
  p.drawRect( QRect(x+35,y,width-35,height) );
}

QSizePolicy PaletteView::sizePolicy() const
{
  return QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
}

//public slots:
void PaletteView::setPostFix(const QString& postfix)
{
  postFix=postfix;
  repaint();
}
void PaletteView::setMaxValue(const QString& maxval)
{ 
  maxvalue=maxval.toFloat();
  repaint();
}
void PaletteView::setMinValue(const QString& minval)
{ 
  minvalue=minval.toFloat();
  repaint();
}

void PaletteView::setUpperBoundPercent(int upper) //int range 4-100
{ 
  if(upper<4)
    upper=4;
  qDebug("upper %i",upper);
  upperbound=upper;
  repaint();
}
void PaletteView::setLowerBoundPercent(int lower) //int range 0-96
{ 
  if(lower>96)
    lower=96;
  lowerbound=lower;
  repaint();
}

void PaletteView::changeCursorFlash()
{
  cursorOn = !cursorOn;
}

float PaletteView::getCursorValue(int value)
{
  float len=maxvalue-minvalue;
  // Range will be within minrange and maxrange
  float minrange = minvalue + len*lowerbound/100.0;
  float maxrange = maxvalue - len*(100-upperbound)/100.0;
  return (height()-value)/((float)height())*(maxrange-minrange) + minrange;
}

void PaletteView::mousePressEvent(QMouseEvent *mouseEvent)
{
  QWidget::mousePressEvent( mouseEvent );
  mousePressed = true;
  emit selectedValueChanged((const QString&)QString::number(getCursorValue(mouseEvent->y()),'f',2));
    //vs->bpcolor->setBackgroundColor(QColor(r,g,b));

}
void PaletteView::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
  QWidget::mouseReleaseEvent( mouseEvent );
  
  if ( (mouseEvent->modifiers() & Qt::MouseButtonMask) == mouseEvent->button() )
    {
      mousePressed = false;
    }
}
void PaletteView::mouseMoveEvent(QMouseEvent *mouseEvent)
{
  QWidget::mouseMoveEvent( mouseEvent );
  int y = mouseEvent->y();
  if(mousePressed && ( y<=height() && y>=0 ) ){
    emit selectedValueChanged((const QString&)QString::number(getCursorValue(mouseEvent->y()),'f',2));
    //vs->bpcolor->setBackgroundColor(QColor(r,g,b));
  }
}

//protected:
void PaletteView::paintEvent( QPaintEvent * )
{
  if(!bwp||!(bwp->count))
     return;
  paintPalette(this,width(),height());
}
