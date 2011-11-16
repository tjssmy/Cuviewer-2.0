#ifndef GLOBALPREFERENCES_H
#define GLOBALPREFERENCES_H

#include <qvariant.h>
#include "ui_globalpreferences.h"

namespace Ui {
    class GlobalPreferences;
} // namespace Ui

class GlobalPreferences : public QDialog, public Ui::GlobalPreferences
{
    Q_OBJECT

public:
    GlobalPreferences(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~GlobalPreferences();

public slots:
    virtual void init();
    virtual void destroy();
    virtual void changeImageFormat(int i);
    virtual void slotOk();
    virtual void changeDocPath();

protected slots:
    virtual void languageChange();
};

#endif // GLOBALPREFERENCES_H
