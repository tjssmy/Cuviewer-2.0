#include "autoexport.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

/**
 *  Constructs a AutoExport as a child of 'parent', with the
 *  widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
AutoExport::AutoExport(QWidget* parent, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    this->setModal(modal);
    setupUi(this);

}

/**
 *  Destroys the object and frees any allocated resources
 */
AutoExport::~AutoExport()
{
    // no need to delete child widgets, Qt does it all for us
}

/**
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void AutoExport::languageChange()
{
    retranslateUi(this);
}

