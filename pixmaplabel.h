/** pixmaplabel.h
  * Carleton University Department of Electronics
  * Copyright (C) 2001 Bryan Wan and Cliff Dugal
  */
#ifndef PIXMAPLABEL_H
#define PIXMAPLABEL_H

#include <qimage.h>
#include <qpoint.h>
#include <qlabel.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPixmap>
#include "paletteview.h"

class PixmapLabel : public QLabel {
    Q_OBJECT

public:
    enum Selected {None, TopCenter, TopRight, RightCenter, BottomRight, 
		   BottomCenter, BottomLeft, LeftCenter, TopLeft};
    PixmapLabel(QWidget* parent=0, const char* name=0, Qt::WindowFlags f=0);
    ~PixmapLabel();
    void setPixmapLabel(QPixmap pm);
    void setPaletteView(PaletteView*);
    QRect getPaletteRect();
    void setPaletteRect(int x,int y,int w,int h);
    void update();
    
protected:
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent* me);
    void mouseMoveEvent(QMouseEvent*);
    static const int HANDLE_SIZE=10;

private:
    QRect moving;
    QPoint moving_start;
    PaletteView* paletteView;
    QPixmap tpm;
    bool mousePressed;
    bool mouseSelect;
    bool handleSelect;
    Selected handle;
};


#endif
