#ifndef SHOWSCENE_H
#define SHOWSCENE_H

#include "ui_showscene.h"

namespace Ui {
    class ShowScene;
} // namespace Ui

class ShowScene : public QDialog, public Ui::ShowScene
{
    Q_OBJECT

public:
    ShowScene(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~ShowScene();

protected slots:
    virtual void languageChange();

};

#endif // SHOWSCENE_H
