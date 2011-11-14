#include "viewersettings.h"
#include "ui_viewersettings.h"
#include "paletteview.h"

ViewerSettings::ViewerSettings(QWidget *parent) :
    QTabWidget(parent)
{
    setupUi(this);
}

ViewerSettings::~ViewerSettings()
{
}

void ViewerSettings::languageChange()
{
    retranslateUi(this);
}
