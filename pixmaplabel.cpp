/** pixmaplabel.cpp
  * Carleton University Department of Electronics
  * Copyright (C) 2001 Bryan Wan and Cliff Dugal
  */
#include <qcursor.h>
#include <qpainter.h>
#include <qimage.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QLabel>
#include <QMouseEvent>
#include "pixmaplabel.h"
using namespace Qt;
PixmapLabel::PixmapLabel(QWidget* parent, const char* name, Qt::WindowFlags f)
  : QLabel (parent, f)
{
  paletteView = 0;
  mousePressed = false;
  mouseSelect = false;
  handleSelect = false;
  handle = None;
	f=0;
}

PixmapLabel::~PixmapLabel()
{
}

void PixmapLabel::setPixmapLabel(QPixmap pm){
  tpm = pm;
  setPixmap(pm);

//  setBackgroundMode(Qt::FixedPixmap);
//  setPaletteBackgroundPixmap(pm);
}

void PixmapLabel::setPaletteView(PaletteView* palette)
{
  paletteView = palette;
}

QRect PixmapLabel::getPaletteRect()
{
  return moving;
}

void PixmapLabel::setPaletteRect(int x,int y,int w,int h)
{
  moving.setRect(x,y,w,h);
}

void PixmapLabel::update()
{
  QPixmap npm = tpm;
  if( paletteView ){
    paletteView->paintPalette( &npm, moving.width(), moving.height(),
       moving.x(), moving.y() );
      if(mouseSelect){
      QPainter p( &npm );
      p.setPen( NoPen );
      p.setBrush( blue );
      int sq = HANDLE_SIZE;
      QRect tl(moving.x(),moving.y(),sq,sq);
      QRect tc(moving.center().x()-sq/2,moving.y(),sq,sq);
      QRect tr(moving.right()-sq,moving.y(),sq,sq);
      QRect rc(moving.right()-sq,moving.center().y()-sq/2,sq,sq);
      QRect br(moving.right()-sq,moving.bottom()-sq,sq,sq);
      QRect bc(moving.center().x()-sq/2,moving.bottom()-sq,sq,sq);
      QRect bl(moving.x(),moving.bottom()-sq,sq,sq);
      QRect lc(moving.x(),moving.center().y()-sq/2,sq,sq);

      p.drawRect( tl );
      p.drawRect( tc );
      p.drawRect( tr );
      p.drawRect( rc );
      p.drawRect( br );
      p.drawRect( bc );
      p.drawRect( bl );
      p.drawRect( lc );
    }
  }
  setPixmap( npm );
}

void PixmapLabel::mousePressEvent(QMouseEvent* me)
{
  int mx=me->x();
  int my=me->y();
  
  if( mx>moving.x() && my>moving.y() && 
      mx<(moving.right()) &&
      my<(moving.bottom()) ) 
    {
      mousePressed = true;
      mouseSelect = true;
      moving_start.setX(mx);
      moving_start.setY(my);

      int sq = HANDLE_SIZE;
      QRect tl(moving.x(),moving.y(),sq,sq);
      QRect tc(moving.center().x()-sq/2,moving.y(),sq,sq);
      QRect tr(moving.right()-sq,moving.y(),sq,sq);
      QRect rc(moving.right()-sq,moving.center().y()-sq/2,sq,sq);
      QRect br(moving.right()-sq,moving.bottom()-sq,sq,sq);
      QRect bc(moving.center().x()-sq/2,moving.bottom()-sq,sq,sq);
      QRect bl(moving.x(),moving.bottom()-sq,sq,sq);
      QRect lc(moving.x(),moving.center().y()-sq/2,sq,sq);

      handleSelect = true;
      if (tl.contains(moving_start)) handle=TopLeft;
      else if (tc.contains(moving_start)) handle=TopCenter;
      else if (tr.contains(moving_start)) handle=TopRight;
      else if (rc.contains(moving_start)) handle=RightCenter;
      else if (br.contains(moving_start)) handle=BottomRight;
      else if (bc.contains(moving_start)) handle=BottomCenter;
      else if (bl.contains(moving_start)) handle=BottomLeft;
      else if (lc.contains(moving_start)) handle=LeftCenter;
      else {
        handleSelect = false;
        handle = None;
      }
      update();
    }
  else 
    if(mouseSelect){
      mouseSelect = false;
      update();
    }
}

void PixmapLabel::mouseReleaseEvent(QMouseEvent*)
{
  mousePressed = false;
}

void PixmapLabel::mouseMoveEvent(QMouseEvent* me)
{
  if(mousePressed){
    int minwidth = 40;
    int minheight = 100;
    int mx=me->x();
    int my=me->y();
    int dx=mx-moving_start.x();
    int dy=my-moving_start.y();

    switch(handle){
    case(None):
      if( moving.right()+dx<=width() && moving.x()+dx>=0 ){
        moving.translate( dx, 0 );
        moving_start.setX(mx);
      }
      if( moving.y()+dy>=0 && moving.bottom()+dy<=height() ){
        moving.translate( 0, dy );
        moving_start.setY(my);
      }
      update();
      break;
    case(TopLeft):
      if( moving.width() - dx>=minwidth ){
        moving.setX( mx );
        moving_start.setX(mx);
      }
      if( moving.height() - dy>=minheight ){
        moving.setY(my);
        moving_start.setY(my);
      }
      update();
      break;
    case(TopCenter):
      if( moving.height() - dy>=minheight ){
        moving.setY(my);
        moving_start.setY(my);
        update();
      }
      break;
    case(TopRight):
      if( moving.width() + dx>=minwidth ){
        moving.setRight( mx );
        moving_start.setX(mx);
      }
      if( moving.height() - dy>=minheight ){
        moving.setY(my);
        moving_start.setY(my);
      }
      update();
      break;
    case(RightCenter):
      if( moving.width() + dx>=minwidth )
        {
          moving.setRight(mx);
          moving_start.setX(mx);
          update();
        }
      break;
    case(BottomRight):
      if( moving.width() + dx>=minwidth ){
        moving.setRight( mx );
        moving_start.setX(mx);
      }
      if( moving.height() + dy>=minheight ){
        moving.setBottom( my );
        moving_start.setY(my);
      }
      update();
      break;
    case(BottomCenter):
      if( moving.height() + dy>=minheight ){
        moving.setBottom(my);
        moving_start.setY(my);
        update();
      }
      break;
    case(BottomLeft):
      if( moving.width() - dx>=minwidth ){
        moving.setX( mx );
        moving_start.setX(mx);
      }
      if( moving.height() + dy>=minheight ){
        moving.setBottom( my );
        moving_start.setY(my);
      }
      update();
      break;
    case(LeftCenter):
      if( moving.width() - dx>=minwidth ){
        moving.setX( mx );
        moving_start.setX(mx);
        update();
      }
      break;
    default:
      qDebug("invalid pixmaplabel handle");
      break;
    }
  }
  else{
    if ( mouseSelect ) {
      //set cursor
      int mx=me->x();
      int my=me->y();
      Selected ch=None;
      QPoint cp;
      cp.setX(mx);
      cp.setY(my);

      int sq = HANDLE_SIZE;
      QRect tl(moving.x(),moving.y(),sq,sq);
      QRect tc(moving.center().x()-sq/2,moving.y(),sq,sq);
      QRect tr(moving.right()-sq,moving.y(),sq,sq);
      QRect rc(moving.right()-sq,moving.center().y()-sq/2,sq,sq);
      QRect br(moving.right()-sq,moving.bottom()-sq,sq,sq);
      QRect bc(moving.center().x()-sq/2,moving.bottom()-sq,sq,sq);
      QRect bl(moving.x(),moving.bottom()-sq,sq,sq);
      QRect lc(moving.x(),moving.center().y()-sq/2,sq,sq);

      if(tl.contains(cp)) ch=TopLeft; 
      else if(tc.contains(cp)) ch=TopCenter;
      else if(tr.contains(cp)) ch=TopRight;
      else if(rc.contains(cp)) ch=RightCenter;
      else if(br.contains(cp)) ch=BottomRight;
      else if(bc.contains(cp)) ch=BottomCenter;
      else if(bl.contains(cp)) ch=BottomLeft;
      else if(lc.contains(cp)) ch=LeftCenter;
      else ch = None;

      switch(ch){
      case(TopCenter):
        setCursor( SizeVerCursor );
        break;
      case(TopRight):
        setCursor( SizeBDiagCursor );
        break;
      case(RightCenter):
        setCursor( SizeHorCursor );
        break;
      case(BottomRight):
        setCursor( SizeFDiagCursor );
        break;
      case(BottomCenter):
        setCursor( SizeVerCursor );
        break;
      case(BottomLeft):
        setCursor( SizeBDiagCursor );
        break;
      case(LeftCenter):
        setCursor( SizeHorCursor );
        break;
      case(TopLeft):
        setCursor( SizeFDiagCursor );
        break;
      case(None):
        unsetCursor();
        break;
      default:
        unsetCursor();
        break;
      };
    }
  }
}
