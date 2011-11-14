#include "showscene.h"
#include "ui_showscene.h"

#include <qvariant.h>
/*
 *  Constructs a ShowScene as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
ShowScene::ShowScene(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
  this->setModal(modal);
  this->setWindowTitle(name);
  setupUi(this);

}

/*
 *  Destroys the object and frees any allocated resources
 */
ShowScene::~ShowScene()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void ShowScene::languageChange()
{
  retranslateUi(this);
}

