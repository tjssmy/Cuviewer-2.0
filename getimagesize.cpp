#include "getimagesize.h"

#include <qvariant.h>
#include "ui_getimagesize.h"
/*
 *  Constructs a GetImageSize as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
GetImageSize::GetImageSize(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    this->setWindowTitle(name);
    this->setModal(modal);
    setupUi(this);

}

/*
 *  Destroys the object and frees any allocated resources
 */
GetImageSize::~GetImageSize()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void GetImageSize::languageChange()
{
    retranslateUi(this);
}

void GetImageSize::lowres()
{
    xValueSpinBox->setValue(640);
    yValueSpinBox->setValue(480);
}

void GetImageSize::medres()
{
    xValueSpinBox->setValue(1024);
    yValueSpinBox->setValue(768);
}

void GetImageSize::hires()
{
    xValueSpinBox->setValue(2048);
    yValueSpinBox->setValue(1536);
}
