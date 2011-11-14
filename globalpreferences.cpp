#include "globalpreferences.h"
#include "ui_globalpreferences.h"

#include <qvariant.h>
#include <qmessagebox.h>
#include <qimage.h>
#include <qdir.h>
#include <qfile.h>
#include <QImageWriter>
#include <QTextStream>

#include "config.h"
/*
 *  Constructs a GlobalPreferences as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
GlobalPreferences::GlobalPreferences(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
  this->setWindowTitle(name);
  this->setModal(modal);

  setupUi(this);

  init();
}

/*
 *  Destroys the object and frees any allocated resources
 */
GlobalPreferences::~GlobalPreferences()
{
    destroy();
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void GlobalPreferences::languageChange()
{
    retranslateUi(this);
}

void GlobalPreferences::init()
{
  QString str;
  for (int i=0; i< QImageWriter::supportedImageFormats().count(); i++){
    str = QImageWriter::supportedImageFormats().at(i);
    imageFormatComboBox->addItem(str);
  }
  connect( imageFormatComboBox, SIGNAL(activated(int)), this, SLOT(changeImageFormat(int)) );
}

void GlobalPreferences::destroy()
{
}

void GlobalPreferences::changeImageFormat(int i)
{
  imageFormatComboBox->setCurrentIndex(i);
}

/**
    @fn slotOk
    @brief Writes all global preferences seen on the dialog to a file when
           the OK button is clicked.

    The method slotOk writes the user's settings a file when the OK button
    on the dialog is clicked.

  */
void GlobalPreferences::slotOk()
{
  //make dir if it doesn't exist
  QDir cuview;
  if(!cuview.exists(QDir::homePath()+"/.cuviewer"))
    cuview.mkdir(QDir::homePath()+"/.cuviewer");

  //write everything to $home/.cuview/cuviewrc
  QFile *file = new QFile(QDir::home().path()+"/.cuviewer/cuviewrc-"+VERSION);
//  file->remove();
  file->setFileName(QDir::home().path()+"/.cuviewer/cuviewrc-"+VERSION);
  if(file->open(QIODevice::ReadWrite)){
    QTextStream ts(file);
    ts << QString("General preferences for cuviewer\n");
    ts << QString("restore_ws %1\n").arg((int)restoreWSCheck->isChecked());
    ts << QString("show_sscreen %1\n").arg((int)showSplashScreenCheck->isChecked());
    if(docPathLineEdit->text().isEmpty()){
      docPathLineEdit->setText(DOCUMENTATION);
    }
    ts << QString("doc_path %1\n").arg(docPathLineEdit->text());
    ts << QString("update_display %1\n").arg(updateDisplayButtonGroup->id(updateDisplayButtonGroup->checkedButton()));
    ts << QString("box_model %1\n").arg((int)boxModelCheck->isChecked());
    ts << QString("box_model_always %1\n").arg((int)drawBoxAlwaysCheck->isChecked());
    ts << QString("drawaxis %1\n").arg((int)drawAxisCheck->isChecked());
    ts << QString("drawaxisorigin %1\n").arg((int)drawAxisOriginCheck->isChecked());

    ts << QString("mouse_buttons %1\n").arg(mouseButtonsSpinBox->text());
    QString fmt = imageFormatComboBox->currentText();
    ts << QString("image_format %1\n").arg(fmt.isEmpty() ? QString("?"):fmt);
    ts << QString("direct_rendering %1\n").arg((int)directRenderingCheck->isChecked());
    ts << QString("bin_palettes %1\n").arg(binPalettesSpinBox->text());
    ts << QString("startUpRefresh %1\n").arg((int)startUpRefreshCheck->isChecked());
    ts << QString("execute_scriptfile %1\n").arg((int)openScriptFileCheck->isChecked());
    ts << QString("scriptfile %1\n").arg(scriptFileNameLineEdit->text());
    file->close();
  }
  else
    qDebug("Can't save prefs to file");

  delete file;
  accept();
}
