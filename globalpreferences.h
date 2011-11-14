#ifndef GLOBALPREFERENCES_H
#define GLOBALPREFERENCES_H

#include <qvariant.h>
#include "ui_globalpreferences.h"

//#include <Qt3Support/Q3ButtonGroup>
//#include <Qt3Support/Q3GroupBox>
//#include <Qt3Support/Q3MimeSourceFactory>
//#include <Qt3Support/Q3TextEdit>
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
//#include <QtGui/QRadioButton>
//#include <QtGui/QSpacerItem>
//#include <QtGui/QSpinBox>
//#include <QtGui/QToolButton>
//#include <QtGui/QVBoxLayout>

//class Ui_GlobalPreferences
//{
//public:
//    QGridLayout *gridLayout;
//    Q3GroupBox *GroupBox22;
//    QGridLayout *gridLayout1;
//    QCheckBox *openScriptfile;
//    QCheckBox *restoreWS;
//    QLineEdit *docPath;
//    QLabel *TextLabel6;
//    QCheckBox *showSplashScreen;
//    QLineEdit *scriptFilename;
//    Q3GroupBox *GroupBox23;
//    QGridLayout *gridLayout2;
//    Q3ButtonGroup *updateDisplay;
//    QVBoxLayout *vboxLayout;
//    QRadioButton *sliderMove;
//    QRadioButton *sliderRelease;
//    Q3TextEdit *TextEdit1;
//    QCheckBox *drawAxis;
//    QCheckBox *directRendering;
//    QCheckBox *drawBoxAlways;
//    QCheckBox *boxModel;
//    QCheckBox *startUpRefresh;
//    QCheckBox *drawAxisOrigin;
//    QHBoxLayout *hboxLayout;
//    QSpacerItem *spacerItem;
//    QPushButton *PushButton20_3;
//    QPushButton *PushButton20_3_2;
//    Q3GroupBox *GroupBox15;
//    QHBoxLayout *hboxLayout1;
//    QLineEdit *imageFormat;
//    QToolButton *imageFormatsButton;
//    QFrame *Line1;
//    Q3GroupBox *GroupBox25;
//    QVBoxLayout *vboxLayout1;
//    QSpinBox *mouseButtons;
//    QSpacerItem *spacerItem1;
//    QSpinBox *binPalettes;
//    QLabel *TextLabel1;

//    void setupUi(QDialog *GlobalPreferences)
//    {
//    if (GlobalPreferences->objectName().isEmpty())
//        GlobalPreferences->setObjectName(QString::fromUtf8("GlobalPreferences"));
//    GlobalPreferences->resize(615, 505);
//    gridLayout = new QGridLayout(GlobalPreferences);
//    gridLayout->setSpacing(6);
//    gridLayout->setMargin(11);
//    gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
//    GroupBox22 = new Q3GroupBox(GlobalPreferences);
//    GroupBox22->setObjectName(QString::fromUtf8("GroupBox22"));
//    GroupBox22->setFrameShape(Q3GroupBox::Box);
//    GroupBox22->setFrameShadow(Q3GroupBox::Sunken);
//    GroupBox22->setColumnLayout(0, Qt::Vertical);
//    GroupBox22->layout()->setSpacing(6);
//    GroupBox22->layout()->setMargin(11);
//    gridLayout1 = new QGridLayout();
//    QBoxLayout *boxlayout = qobject_cast<QBoxLayout *>(GroupBox22->layout());
//    if (boxlayout)
//        boxlayout->addLayout(gridLayout1);
//    gridLayout1->setAlignment(Qt::AlignTop);
//    gridLayout1->setObjectName(QString::fromUtf8("gridLayout1"));
//    openScriptfile = new QCheckBox(GroupBox22);
//    openScriptfile->setObjectName(QString::fromUtf8("openScriptfile"));

//    gridLayout1->addWidget(openScriptfile, 1, 0, 1, 2);

//    restoreWS = new QCheckBox(GroupBox22);
//    restoreWS->setObjectName(QString::fromUtf8("restoreWS"));
//    restoreWS->setChecked(true);

//    gridLayout1->addWidget(restoreWS, 0, 0, 1, 2);

//    docPath = new QLineEdit(GroupBox22);
//    docPath->setObjectName(QString::fromUtf8("docPath"));

//    gridLayout1->addWidget(docPath, 4, 1, 1, 1);

//    TextLabel6 = new QLabel(GroupBox22);
//    TextLabel6->setObjectName(QString::fromUtf8("TextLabel6"));
//    TextLabel6->setWordWrap(false);

//    gridLayout1->addWidget(TextLabel6, 4, 0, 1, 1);

//    showSplashScreen = new QCheckBox(GroupBox22);
//    showSplashScreen->setObjectName(QString::fromUtf8("showSplashScreen"));
//    showSplashScreen->setChecked(true);

//    gridLayout1->addWidget(showSplashScreen, 3, 0, 1, 2);

//    scriptFilename = new QLineEdit(GroupBox22);
//    scriptFilename->setObjectName(QString::fromUtf8("scriptFilename"));

//    gridLayout1->addWidget(scriptFilename, 2, 0, 1, 2);


//    gridLayout->addWidget(GroupBox22, 0, 0, 1, 3);

//    GroupBox23 = new Q3GroupBox(GlobalPreferences);
//    GroupBox23->setObjectName(QString::fromUtf8("GroupBox23"));
//    GroupBox23->setColumnLayout(0, Qt::Vertical);
//    GroupBox23->layout()->setSpacing(6);
//    GroupBox23->layout()->setMargin(11);
//    gridLayout2 = new QGridLayout();
//    QBoxLayout *boxlayout1 = qobject_cast<QBoxLayout *>(GroupBox23->layout());
//    if (boxlayout1)
//        boxlayout1->addLayout(gridLayout2);
//    gridLayout2->setAlignment(Qt::AlignTop);
//    gridLayout2->setObjectName(QString::fromUtf8("gridLayout2"));
//    updateDisplay = new Q3ButtonGroup(GroupBox23);
//    updateDisplay->setObjectName(QString::fromUtf8("updateDisplay"));
//    updateDisplay->setEnabled(false);
//    updateDisplay->setExclusive(false);
//    updateDisplay->setColumnLayout(0, Qt::Vertical);
//    updateDisplay->layout()->setSpacing(6);
//    updateDisplay->layout()->setMargin(11);
//    vboxLayout = new QVBoxLayout();
//    QBoxLayout *boxlayout2 = qobject_cast<QBoxLayout *>(updateDisplay->layout());
//    if (boxlayout2)
//        boxlayout2->addLayout(vboxLayout);
//    vboxLayout->setAlignment(Qt::AlignTop);
//    vboxLayout->setObjectName(QString::fromUtf8("vboxLayout"));
//    sliderMove = new QRadioButton(updateDisplay);
//    sliderMove->setObjectName(QString::fromUtf8("sliderMove"));
//    sliderMove->setChecked(true);
//    updateDisplay->insert(sliderMove, 0);

//    vboxLayout->addWidget(sliderMove);

//    sliderRelease = new QRadioButton(updateDisplay);
//    sliderRelease->setObjectName(QString::fromUtf8("sliderRelease"));
//    updateDisplay->insert(sliderRelease, 1);

//    vboxLayout->addWidget(sliderRelease);


//    gridLayout2->addWidget(updateDisplay, 0, 0, 1, 2);

//    TextEdit1 = new Q3TextEdit(GroupBox23);
//    TextEdit1->setObjectName(QString::fromUtf8("TextEdit1"));

//    gridLayout2->addWidget(TextEdit1, 6, 0, 1, 2);

//    drawAxis = new QCheckBox(GroupBox23);
//    drawAxis->setObjectName(QString::fromUtf8("drawAxis"));

//    gridLayout2->addWidget(drawAxis, 5, 0, 1, 1);

//    directRendering = new QCheckBox(GroupBox23);
//    directRendering->setObjectName(QString::fromUtf8("directRendering"));

//    gridLayout2->addWidget(directRendering, 4, 0, 1, 2);

//    drawBoxAlways = new QCheckBox(GroupBox23);
//    drawBoxAlways->setObjectName(QString::fromUtf8("drawBoxAlways"));

//    gridLayout2->addWidget(drawBoxAlways, 3, 0, 1, 2);

//    boxModel = new QCheckBox(GroupBox23);
//    boxModel->setObjectName(QString::fromUtf8("boxModel"));

//    gridLayout2->addWidget(boxModel, 2, 0, 1, 2);

//    startUpRefresh = new QCheckBox(GroupBox23);
//    startUpRefresh->setObjectName(QString::fromUtf8("startUpRefresh"));

//    gridLayout2->addWidget(startUpRefresh, 1, 0, 1, 2);

//    drawAxisOrigin = new QCheckBox(GroupBox23);
//    drawAxisOrigin->setObjectName(QString::fromUtf8("drawAxisOrigin"));
//    drawAxisOrigin->setEnabled(true);

//    gridLayout2->addWidget(drawAxisOrigin, 5, 1, 1, 1);


//    gridLayout->addWidget(GroupBox23, 0, 3, 4, 1);

//    hboxLayout = new QHBoxLayout();
//    hboxLayout->setSpacing(6);
//    hboxLayout->setMargin(0);
//    hboxLayout->setObjectName(QString::fromUtf8("hboxLayout"));
//    spacerItem = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

//    hboxLayout->addItem(spacerItem);

//    PushButton20_3 = new QPushButton(GlobalPreferences);
//    PushButton20_3->setObjectName(QString::fromUtf8("PushButton20_3"));

//    hboxLayout->addWidget(PushButton20_3);

//    PushButton20_3_2 = new QPushButton(GlobalPreferences);
//    PushButton20_3_2->setObjectName(QString::fromUtf8("PushButton20_3_2"));

//    hboxLayout->addWidget(PushButton20_3_2);


//    gridLayout->addLayout(hboxLayout, 5, 0, 1, 4);

//    GroupBox15 = new Q3GroupBox(GlobalPreferences);
//    GroupBox15->setObjectName(QString::fromUtf8("GroupBox15"));
//    GroupBox15->setColumnLayout(0, Qt::Vertical);
//    GroupBox15->layout()->setSpacing(6);
//    GroupBox15->layout()->setMargin(11);
//    hboxLayout1 = new QHBoxLayout();
//    QBoxLayout *boxlayout3 = qobject_cast<QBoxLayout *>(GroupBox15->layout());
//    if (boxlayout3)
//        boxlayout3->addLayout(hboxLayout1);
//    hboxLayout1->setAlignment(Qt::AlignTop);
//    hboxLayout1->setObjectName(QString::fromUtf8("hboxLayout1"));
//    imageFormat = new QLineEdit(GroupBox15);
//    imageFormat->setObjectName(QString::fromUtf8("imageFormat"));

//    hboxLayout1->addWidget(imageFormat);

//    imageFormatsButton = new QToolButton(GroupBox15);
//    imageFormatsButton->setObjectName(QString::fromUtf8("imageFormatsButton"));

//    hboxLayout1->addWidget(imageFormatsButton);


//    gridLayout->addWidget(GroupBox15, 1, 0, 1, 2);

//    Line1 = new QFrame(GlobalPreferences);
//    Line1->setObjectName(QString::fromUtf8("Line1"));
//    Line1->setFrameShape(QFrame::HLine);
//    Line1->setFrameShadow(QFrame::Sunken);

//    gridLayout->addWidget(Line1, 4, 0, 1, 4);

//    GroupBox25 = new Q3GroupBox(GlobalPreferences);
//    GroupBox25->setObjectName(QString::fromUtf8("GroupBox25"));
//    GroupBox25->setColumnLayout(0, Qt::Vertical);
//    GroupBox25->layout()->setSpacing(6);
//    GroupBox25->layout()->setMargin(11);
//    vboxLayout1 = new QVBoxLayout();
//    QBoxLayout *boxlayout4 = qobject_cast<QBoxLayout *>(GroupBox25->layout());
//    if (boxlayout4)
//        boxlayout4->addLayout(vboxLayout1);
//    vboxLayout1->setAlignment(Qt::AlignTop);
//    vboxLayout1->setObjectName(QString::fromUtf8("vboxLayout1"));
//    mouseButtons = new QSpinBox(GroupBox25);
//    mouseButtons->setObjectName(QString::fromUtf8("mouseButtons"));
//    mouseButtons->setMaximum(3);
//    mouseButtons->setMinimum(2);
//    mouseButtons->setValue(3);

//    vboxLayout1->addWidget(mouseButtons);


//    gridLayout->addWidget(GroupBox25, 1, 2, 1, 1);

//    spacerItem1 = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding);

//    gridLayout->addItem(spacerItem1, 3, 1, 1, 1);

//    binPalettes = new QSpinBox(GlobalPreferences);
//    binPalettes->setObjectName(QString::fromUtf8("binPalettes"));
//    binPalettes->setMaximum(16);
//    binPalettes->setValue(16);

//    gridLayout->addWidget(binPalettes, 2, 1, 1, 1);

//    TextLabel1 = new QLabel(GlobalPreferences);
//    TextLabel1->setObjectName(QString::fromUtf8("TextLabel1"));
//    TextLabel1->setWordWrap(false);

//    gridLayout->addWidget(TextLabel1, 2, 0, 1, 1);

//    QWidget::setTabOrder(restoreWS, openScriptfile);
//    QWidget::setTabOrder(openScriptfile, scriptFilename);
//    QWidget::setTabOrder(scriptFilename, showSplashScreen);
//    QWidget::setTabOrder(showSplashScreen, docPath);
//    QWidget::setTabOrder(docPath, imageFormat);
//    QWidget::setTabOrder(imageFormat, mouseButtons);
//    QWidget::setTabOrder(mouseButtons, binPalettes);
//    QWidget::setTabOrder(binPalettes, sliderMove);
//    QWidget::setTabOrder(sliderMove, startUpRefresh);
//    QWidget::setTabOrder(startUpRefresh, boxModel);
//    QWidget::setTabOrder(boxModel, directRendering);
//    QWidget::setTabOrder(directRendering, TextEdit1);
//    QWidget::setTabOrder(TextEdit1, PushButton20_3);
//    QWidget::setTabOrder(PushButton20_3, PushButton20_3_2);

//    retranslateUi(GlobalPreferences);
//    QObject::connect(PushButton20_3_2, SIGNAL(clicked()), GlobalPreferences, SLOT(reject()));
//    QObject::connect(PushButton20_3, SIGNAL(clicked()), GlobalPreferences, SLOT(slotOk()));

//    QMetaObject::connectSlotsByName(GlobalPreferences);
//    } // setupUi

//    void retranslateUi(QDialog *GlobalPreferences)
//    {
//    GlobalPreferences->setWindowTitle(QApplication::translate("GlobalPreferences", "Global Preferences", 0, QApplication::UnicodeUTF8));
//    GroupBox22->setTitle(QApplication::translate("GlobalPreferences", "General", 0, QApplication::UnicodeUTF8));
//    openScriptfile->setText(QApplication::translate("GlobalPreferences", "Always execute current scriptfile:", 0, QApplication::UnicodeUTF8));
//    openScriptfile->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "The current scriptfile being used will be executed next time", 0, QApplication::UnicodeUTF8)));
//    restoreWS->setText(QApplication::translate("GlobalPreferences", "Restore window size", 0, QApplication::UnicodeUTF8));
//    restoreWS->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Restores the last workspace window size", 0, QApplication::UnicodeUTF8)));
//    docPath->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Changes the path for where to look for progrom documentation.", 0, QApplication::UnicodeUTF8)));
//    TextLabel6->setText(QApplication::translate("GlobalPreferences", "Documentation:", 0, QApplication::UnicodeUTF8));
//    showSplashScreen->setText(QApplication::translate("GlobalPreferences", "Show splashscreen on startup", 0, QApplication::UnicodeUTF8));
//    showSplashScreen->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Click to show or hide the splashscreen", 0, QApplication::UnicodeUTF8)));
//    scriptFilename->setText(QString());
//    GroupBox23->setTitle(QApplication::translate("GlobalPreferences", "Viewing", 0, QApplication::UnicodeUTF8));
//    updateDisplay->setTitle(QApplication::translate("GlobalPreferences", "Update display on:", 0, QApplication::UnicodeUTF8));
//    sliderMove->setText(QApplication::translate("GlobalPreferences", "Slider move", 0, QApplication::UnicodeUTF8));
//    sliderMove->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Choosing this will update the display as the sliders are being moved.", 0, QApplication::UnicodeUTF8)));
//    sliderRelease->setText(QApplication::translate("GlobalPreferences", "Slider release", 0, QApplication::UnicodeUTF8));
//    sliderRelease->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Choosing this will improve performance on slow computers", 0, QApplication::UnicodeUTF8)));
//    TextEdit1->setText(QApplication::translate("GlobalPreferences", "Notes: Direct Rendering: \n"
//"fixes X Sun memory problem\n"
//"\n"
//"Box Model:\n"
//"replaces object with a box when mouse interacts with the screen; renders mouse movement faster.", 0, QApplication::UnicodeUTF8));
//    drawAxis->setText(QApplication::translate("GlobalPreferences", "Draw  Axis", 0, QApplication::UnicodeUTF8));
//    directRendering->setText(QApplication::translate("GlobalPreferences", "Direct Rendering", 0, QApplication::UnicodeUTF8));
//    directRendering->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Fix X server memory problem on Solaris", 0, QApplication::UnicodeUTF8)));
//    drawBoxAlways->setText(QApplication::translate("GlobalPreferences", "Bounding Box always", 0, QApplication::UnicodeUTF8));
//    boxModel->setText(QApplication::translate("GlobalPreferences", "Box model with mouse move", 0, QApplication::UnicodeUTF8));
//    boxModel->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Enabling this will improve rendering performance", 0, QApplication::UnicodeUTF8)));
//    startUpRefresh->setText(QApplication::translate("GlobalPreferences", "Refresh screen on startup", 0, QApplication::UnicodeUTF8));
//    startUpRefresh->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Check this if the screen doesn't refresh on startup", 0, QApplication::UnicodeUTF8)));
//    drawAxisOrigin->setText(QApplication::translate("GlobalPreferences", "Use Origin", 0, QApplication::UnicodeUTF8));
//    PushButton20_3->setText(QApplication::translate("GlobalPreferences", "Ok", 0, QApplication::UnicodeUTF8));
//    PushButton20_3->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Make changes to global preferences", 0, QApplication::UnicodeUTF8)));
//    PushButton20_3_2->setText(QApplication::translate("GlobalPreferences", "Cancel", 0, QApplication::UnicodeUTF8));
//    PushButton20_3_2->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Cancels all changes to global preferences.", 0, QApplication::UnicodeUTF8)));
//    GroupBox15->setTitle(QApplication::translate("GlobalPreferences", "Default image export format:", 0, QApplication::UnicodeUTF8));
//    imageFormatsButton->setText(QApplication::translate("GlobalPreferences", "...", 0, QApplication::UnicodeUTF8));
//    imageFormatsButton->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Click to select the default image export format from a list.", 0, QApplication::UnicodeUTF8)));
//    GroupBox25->setTitle(QApplication::translate("GlobalPreferences", "Mouse buttons", 0, QApplication::UnicodeUTF8));
//    mouseButtons->setProperty("toolTip", QVariant(QApplication::translate("GlobalPreferences", "Specifies how many buttons are on the mouse. Read manual on keyboard mapping with two/threee button mouse modes", 0, QApplication::UnicodeUTF8)));
//    TextLabel1->setText(QApplication::translate("GlobalPreferences", "Bin Palettes", 0, QApplication::UnicodeUTF8));
//    Q_UNUSED(GlobalPreferences);
//    } // retranslateUi

//};

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

protected slots:
    virtual void languageChange();
};

#endif // GLOBALPREFERENCES_H
