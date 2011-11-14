#include "printpreview.h"
#include "ui_printpreview.h"

#include <qvariant.h>
#include <qrect.h>
#include "pixmaplabel.h"
/*
 *  Constructs a PrintPreview as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
PrintPreview::PrintPreview(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    setupUi(this);

    init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
PrintPreview::~PrintPreview()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void PrintPreview::languageChange()
{
    retranslateUi(this);
}

void PrintPreview::init()
{

}

void PrintPreview::destroy()
{

}

/** @fn updatePreview
    @brief Updates the pixmap label.

  */
void PrintPreview::updatePreview()
{
  QImage img = pix.toImage();
  if ( checkInvert->isChecked() ) {
    img.invertPixels();
  }
  if ( checkMirror->isChecked() ) {
    img = img.mirrored( TRUE, FALSE );
  }
  if ( checkFlip->isChecked() ) {
    img = img.mirrored( FALSE, TRUE );
  }
  QPixmap pm;
  pm.convertFromImage( img );
  pixmapLabel->setPixmapLabel( pm );
  pixmapLabel->update();
}

void PrintPreview::flip( bool )
{
  updatePreview();
}

void PrintPreview::invertColors( bool )
{
  updatePreview();
}

void PrintPreview::mirror( bool )
{
  updatePreview();
}

void PrintPreview::setPixmap( const QPixmap & pm )
{
  pix = pm;
  updatePreview();
}

void PrintPreview::setPaletteView( PaletteView * paletteView )
{
  pixmapLabel->setPaletteView( paletteView );
}

void PrintPreview::setPaletteRect(int x,int y,int width,int height)
{
  pixmapLabel->setPaletteRect( x, y, width, height );
}

QRect PrintPreview::getPaletteRect()
{
  return pixmapLabel->getPaletteRect();
}
