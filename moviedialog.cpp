#include "moviedialog.h"
#include "ui_moviedialog.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a MovieDialog as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
MovieDialog::MovieDialog(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    this->setWindowTitle(name);
    this->setModal(modal);
    setupUi(this);

}

/*
 *  Destroys the object and frees any allocated resources
 */
MovieDialog::~MovieDialog()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void MovieDialog::languageChange()
{
    retranslateUi(this);
}

