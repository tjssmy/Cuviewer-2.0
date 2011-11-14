#ifndef VIEWERSETTINGS_H
#define VIEWERSETTINGS_H

#include "ui_viewersettings.h"

#include <QTabWidget>
#include "paletteview.h"


namespace Ui {
    class ViewerSettings;
}

class ViewerSettings : public QTabWidget, public Ui::ViewerSettings
{
    Q_OBJECT

public:
    explicit ViewerSettings(QWidget *parent = 0);
    ~ViewerSettings();

protected slots:
    virtual void languageChange();
};

#endif // VIEWERSETTINGS_H
