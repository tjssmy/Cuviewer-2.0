#ifndef MOVIEDIALOG_H
#define MOVIEDIALOG_H

#include <qvariant.h>
#include "ui_moviedialog.h"

//#include <Qt3Support/Q3MimeSourceFactory>
//#include <QtCore/QVariant>
//#include <QtGui/QAction>
//#include <QtGui/QApplication>
//#include <QtGui/QButtonGroup>
//#include <QtGui/QCheckBox>
//#include <QtGui/QDialog>
//#include <QtGui/QFrame>
//#include <QtGui/QGridLayout>
//#include <QtGui/QHBoxLayout>
//#include <QtGui/QLabel>
//#include <QtGui/QLineEdit>
//#include <QtGui/QPushButton>
//#include <QtGui/QSpacerItem>
//#include <QtGui/QSpinBox>

//class Ui_MovieDialog
//{
//public:
//    QGridLayout *gridLayout;
//    QLabel *TextLabel1;
//    QLabel *TextLabel2;
//    QLineEdit *moviefile;
//    QSpinBox *fps;
//    QSpinBox *timeLimit;
//    QLabel *TextLabel1_2;
//    QCheckBox *exportImages;
//    QCheckBox *recordMovement;
//    QHBoxLayout *hboxLayout;
//    QSpacerItem *spacerItem;
//    QPushButton *buttonOk;
//    QPushButton *buttonCancel;
//    QFrame *Line1;

//    void setupUi(QDialog *MovieDialog)
//    {
//    if (MovieDialog->objectName().isEmpty())
//        MovieDialog->setObjectName(QString::fromUtf8("MovieDialog"));
//    MovieDialog->resize(289, 182);
//    MovieDialog->setSizeGripEnabled(true);
//    gridLayout = new QGridLayout(MovieDialog);
//    gridLayout->setSpacing(6);
//    gridLayout->setMargin(11);
//    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
//    TextLabel1 = new QLabel(MovieDialog);
//    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
//    TextLabel1->setWordWrap(false);

//    gridLayout->addWidget(TextLabel1, 0, 0, 1, 1);

//    TextLabel2 = new QLabel(MovieDialog);
//    TextLabel2->setObjectName(QString::fromUtf8("TextLabel2"));
//    TextLabel2->setWordWrap(false);

//    gridLayout->addWidget(TextLabel2, 1, 0, 1, 1);

//    moviefile = new QLineEdit(MovieDialog);
//    moviefile->setObjectName(QString::fromUtf8("moviefile"));

//    gridLayout->addWidget(moviefile, 1, 1, 1, 1);

//    fps = new QSpinBox(MovieDialog);
//    fps->setObjectName(QString::fromUtf8("fps"));
//    fps->setMaximum(40);
//    fps->setMinimum(1);
//    fps->setValue(20);

//    gridLayout->addWidget(fps, 0, 1, 1, 1);

//    timeLimit = new QSpinBox(MovieDialog);
//    timeLimit->setObjectName(QString::fromUtf8("timeLimit"));
//    timeLimit->setMaximum(1000);
//    timeLimit->setMinimum(1);
//    timeLimit->setValue(30);

//    gridLayout->addWidget(timeLimit, 2, 1, 1, 1);

//    TextLabel1_2 = new QLabel(MovieDialog);
//    TextLabel1_2->setObjectName(QString::fromUtf8("TextLabel1_2"));
//    TextLabel1_2->setWordWrap(false);

//    gridLayout->addWidget(TextLabel1_2, 2, 0, 1, 1);

//    exportImages = new QCheckBox(MovieDialog);
//    exportImages->setObjectName(QString::fromUtf8("exportImages"));
//    exportImages->setChecked(true);

//    gridLayout->addWidget(exportImages, 3, 1, 1, 1);

//    recordMovement = new QCheckBox(MovieDialog);
//    recordMovement->setObjectName(QString::fromUtf8("recordMovement"));
//    recordMovement->setChecked(true);

//    gridLayout->addWidget(recordMovement, 3, 0, 1, 1);

//    hboxLayout = new QHBoxLayout();
//    hboxLayout->setSpacing(6);
//    hboxLayout->setMargin(0);
//    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
//    spacerItem = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

//    hboxLayout->addItem(spacerItem);

//    buttonOk = new QPushButton(MovieDialog);
//    buttonOk->setObjectName(QString::fromUtf8("buttonOk"));
//    buttonOk->setShortcut(0);
//    buttonOk->setAutoDefault(true);
//    buttonOk->setDefault(true);

//    hboxLayout->addWidget(buttonOk);

//    buttonCancel = new QPushButton(MovieDialog);
//    buttonCancel->setObjectName(QString::fromUtf8("buttonCancel"));
//    buttonCancel->setShortcut(0);
//    buttonCancel->setAutoDefault(true);

//    hboxLayout->addWidget(buttonCancel);


//    gridLayout->addLayout(hboxLayout, 5, 0, 1, 2);

//    Line1 = new QFrame(MovieDialog);
//    Line1->setObjectName(QString::fromUtf8("Line1"));
//    Line1->setProperty("frameShape", QVariant(QFrame::HLine));
//    Line1->setFrameShadow(QFrame::Sunken);

//    gridLayout->addWidget(Line1, 4, 0, 1, 2);

//    QWidget::setTabOrder(fps, moviefile);
//    QWidget::setTabOrder(moviefile, timeLimit);
//    QWidget::setTabOrder(timeLimit, buttonOk);
//    QWidget::setTabOrder(buttonOk, buttonCancel);

//    retranslateUi(MovieDialog);
//    QObject::connect(buttonOk, SIGNAL(clicked()), MovieDialog, SLOT(accept()));
//    QObject::connect(buttonCancel, SIGNAL(clicked()), MovieDialog, SLOT(reject()));

//    QMetaObject::connectSlotsByName(MovieDialog);
//    } // setupUi

//    void retranslateUi(QDialog *MovieDialog)
//    {
//    MovieDialog->setWindowTitle(QApplication::translate("MovieDialog", "Create Movie", 0, QApplication::UnicodeUTF8));
//    TextLabel1->setText(QApplication::translate("MovieDialog", "Frames per second", 0, QApplication::UnicodeUTF8));
//    TextLabel2->setText(QApplication::translate("MovieDialog", "Movie filename", 0, QApplication::UnicodeUTF8));
//    TextLabel1_2->setText(QApplication::translate("MovieDialog", "Time Limit (sec)", 0, QApplication::UnicodeUTF8));
//    exportImages->setText(QApplication::translate("MovieDialog", "Export Images", 0, QApplication::UnicodeUTF8));
//    recordMovement->setText(QApplication::translate("MovieDialog", "Record Movement", 0, QApplication::UnicodeUTF8));
//    buttonOk->setText(QApplication::translate("MovieDialog", "OK", 0, QApplication::UnicodeUTF8));
//    buttonCancel->setText(QApplication::translate("MovieDialog", "Cancel", 0, QApplication::UnicodeUTF8));
//    Q_UNUSED(MovieDialog);
//    } // retranslateUi

//};

namespace Ui {
    class MovieDialog;
} // namespace Ui

class MovieDialog : public QDialog, public Ui::MovieDialog
{
    Q_OBJECT

public:
    MovieDialog(QWidget* parent = 0, const char* name = 0, bool modal = false, Qt::WindowFlags fl = 0);
    ~MovieDialog();

protected slots:
    virtual void languageChange();

};

#endif // MOVIEDIALOG_H
