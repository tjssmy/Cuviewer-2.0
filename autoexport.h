#ifndef AUTOEXPORT_H
#define AUTOEXPORT_H

#include <qvariant.h>
#include "ui_autoexport.h"

namespace Ui {
    class AutoExport;
} // namespace Ui

class AutoExport : public QDialog, public Ui::AutoExport
{
    Q_OBJECT

public:
    AutoExport(QWidget* parent = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~AutoExport();

protected slots:
    virtual void languageChange();

};

#endif // AUTOEXPORT_H
