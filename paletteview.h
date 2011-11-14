/** paletteview.h
  * Carleton University Department of Electronics
  * Copyright (C) 2001 Bryan Wan and Cliff Dugal
  */
#ifndef PALETTEVIEW_H
#define PALETTEVIEW_H

#include <qwidget.h>
#include <qgl.h>
//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>
#include "include/cuvcommonstructs.h" //contains bin_window_palette

class QMouseEvent;

class PaletteView : public QWidget
{
    Q_OBJECT
public:
  PaletteView( QWidget *parent=0);
  ~PaletteView();
  void setBinPalette(const bin_window_palette*);
  void paintPalette(QPaintDevice*,int,int, int x=0, int y=0);
  void setBackgroundColor(const QColor&);
  QSizePolicy sizePolicy() const;
  float getCursorValue(int value);
  virtual void mousePressEvent(QMouseEvent *mouseEvent);
  virtual void mouseReleaseEvent(QMouseEvent *mouseEvent);
  virtual void mouseMoveEvent(QMouseEvent *mouseEvent);

public slots:
  void setPostFix(const QString&);
  void setMaxValue(const QString&);
  void setMinValue(const QString&);
  void setUpperBoundPercent(int upper); //int range 4-100
  void setLowerBoundPercent(int lower); //int range 4-100
  void changeCursorFlash();
   
signals:
  void selectedValueChanged(float);
  void selectedValueChanged(const QString&);
  void selectedColourChanged(QColor);

protected:
  void paintEvent( QPaintEvent * );

private:
  float minvalue, maxvalue;
  int lowerbound, upperbound;
  QString postFix;
  bool cursorOn;
  bool mousePressed;
  bin_window_palette * bwp;
  QColor bg;
};

#endif // PALETTEVIEW_H
