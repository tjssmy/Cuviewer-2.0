#ifndef GETIMAGESIZE_H
#define GETIMAGESIZE_H

#include <qvariant.h>
#include "ui_getimagesize.h"

namespace Ui {
    class GetImageSize;
} // namespace Ui

class GetImageSize : public QDialog, public Ui::GetImageSize
{
    Q_OBJECT

public:
    GetImageSize(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~GetImageSize();

public slots:
    virtual void lowres();
    virtual void medres();
    virtual void hires();

protected slots:
    virtual void languageChange();

};

#endif // GETIMAGESIZE_H
