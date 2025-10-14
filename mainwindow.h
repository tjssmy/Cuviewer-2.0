#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qvariant.h>
#include <QScrollArea>

#include <QMainWindow>
#include <QTextStream>
#include <QPrinter>
#include "binpalettes.h"
#include "cuviewdoc/cuviewdoc.h"
#include "imageprocessing.h"
#include "showscene.h"
#include "viewdata.h"
#include "viewersettings.h"
#include "config.h"
#include "helpwindow.h"
#include "ui_mainwindow.h"

namespace Ui {
    class MainWindow;
} // namespace Ui

class MainWindow : public QMainWindow, public Ui::MainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = 0);
    ~MainWindow();

public slots:
    virtual void init();
    virtual void destroy();
    virtual void setViewerSettingsActions();
    virtual void connectSlots();
    virtual void filePrint( int width, int height );
    virtual void filePrintWindowRes();
    virtual void filePrintLowRes();
    virtual void filePrintMedRes();
    virtual void filePrintHighRes();
    virtual bool filePrintSetup();
    virtual void loadSeparately( QStringList files );
    virtual void loadAllInOne( QStringList files );
    virtual void loadNewView( QString * loadthisfile );
    virtual void load( QString * loadthisfile );
    virtual void fileOpenMerge( QString * loadthisfile );
    virtual void loadFileOpenMerge();
    virtual void reload();
    virtual void slotOpen();
    virtual void slotAbout();
    virtual void slotHelpContents();
    virtual void quickHelp();
    virtual void slotClose();
    virtual void closeFile();
    virtual void slotExit();
    virtual void slotCreateMovie();
    virtual void slotReadMovieFile( QString filename );
    virtual void slotWriteMovieFrame();
    virtual void slotGlobalPrefs();
    virtual PrefData * getPreferences();
    virtual void setPrefData( PrefData * pd );
    virtual void setOutlineColor( QColor c );
    virtual void setBoundingBox( bool c );
    virtual void setDrawBoundingBoxAlways( bool c );
    virtual void setOutlineChecked( bool c );
    virtual void setTwoSidedChecked( bool c );
    virtual void setShadingChecked( bool c );
    virtual void setAntialiasingChecked( bool c );
    virtual void setWireframeChecked( bool c );
    virtual void setOpaqueWireframeChecked( bool c );
    virtual void setPerspectiveChecked( bool c );
    virtual void setTransparencyChecked( bool c );
    virtual void setFogChecked( bool c );
    virtual void setGammaChecked( bool c );
    virtual void setLightingChecked( bool c );
    virtual void setDiffuseChecked( bool c );
    virtual void setBinnedPaletteChecked( bool c );
    virtual void setClippingValue( int v );
    virtual void setFOVValue( int v );
    virtual void setOutlineValue( int v );
    virtual void setTesselValue( int v );
    virtual void setAmbientValue( int v );
    virtual void setDiffuseValue( int v );
    virtual void setBackgroundValue( int v );
    virtual void setBackgroundBlack();
    virtual void setBackgroundWhite();
    virtual void setFogValue( int v );
    virtual void setGammaValue( int v );
    virtual void slotOutlineColor();
    virtual int terminateTime();
    virtual void slotExportImageFile( QString filename );
    virtual void slotSetScriptFile( QString file, bool scriptload );
    virtual void slotSavePreset();
    virtual void slotLoadPreset();
    virtual void slotViewerSettings( bool show );
    virtual void redrawDocument();
    virtual void setZeroClipping();
    virtual void goto1();
    virtual void goto2();
    virtual void goto3();
    virtual void goto4();
    virtual void goto5();
    virtual void goto6();
    virtual void goto7();
    virtual void goto8();
    virtual void goto9();
    virtual void goto0();
    virtual void gotoViewpoint( int key );
    virtual void showScenes();
    virtual void updateScenes();
    virtual void setSceneVisible();
    virtual void setDrawAxis( bool toggle );
    virtual void setEditMode( bool edit );
    virtual void setSceneEditing();
    virtual void reversePlayScene();
    virtual void forwardPlayScene();
    virtual void reverseStep();
    virtual void forwardStep();
    virtual void stopPlayScene();
    virtual void startScene();
    virtual void endScene();
    virtual void transformScene();
    virtual void rotateScene();
    virtual void setLightEditing();
    virtual void setSpecular( int value );
    virtual void resetLightPosition();
    virtual void setFixedLight( bool fixed );
    virtual void setLightPosition();
    virtual void slotLightColor();
    virtual void setLightColor( int index, QColor c );
    virtual void reversePlayEnd();
    virtual void forwardPlayEnd();
    virtual void stopPlay();
    virtual void loadScriptnoloadAction_activated();

protected:
    StartFolder startfolder;
    PrefData prefdata;
    CUViewDoc * cuviewDoc;
    QFile * moviefile;
    QPrinter * printer;
    QTimer * timer;
    QTextStream moviestream;
    ViewerSettings* vs;
    ImageProcessing* ip;
    BinPalettes* bp;
    ShowScene * showSceneDialog;
    MovieData moviedata;
    FileData filedata;
    bool filesOnStartup;
    int timeout;
    int terminatetime;
    int currentscene;

protected slots:
    virtual void languageChange();

private:
    bool isFirstTimeLoading; //Checks if the loaded cuviewDoc is the first time loading it in the program
};

#endif // MAINWINDOW_H
