#include "mainwindow.h"
#include <QApplication>
#include <QTextStream>
#include <QStyle>
#include <QString>
#include <QStringList>
#include <QFile>
#include <QStyle>
#include <QStyleFactory>

#include "mainwindow.h"
#include "config.h"

bool useNativeStyle; //Use native look-and-feel if set, otherwise use Windows style.
bool scriptload;
QStringList* files;
QString scriptfile, imagefile, moviefile;

void createApp()
{
  MainWindow *mainwindow = new MainWindow();
  mainwindow->setWindowTitle("Carleton University Viewer");

  if (!scriptfile.isEmpty()){
    mainwindow->slotSetScriptFile(scriptfile, scriptload);
  }
  //Only first file executes scriptfile
  mainwindow->loadSeparately(*files);

  if (!useNativeStyle){
    qApp->setStyle("windows");
  }
  mainwindow->show();

  if (!imagefile.isEmpty()){
    mainwindow->slotExportImageFile(imagefile);
    QTimer::singleShot(mainwindow->terminateTime()+300, qApp, SLOT(quit()));
  }
  if (!moviefile.isEmpty()){
    mainwindow->slotReadMovieFile(moviefile);
  }

}

int main(int argc, char *argv[])
{
  useNativeStyle=true;
  scriptload=true;
  files = new QStringList;

  //open a output stream
  QFile *stdOutFile = new QFile();
  stdOutFile->open(stdout, QIODevice::WriteOnly | QIODevice::Unbuffered);
  QTextStream stdOutStream(stdOutFile);

  QApplication::setColorSpec(QApplication::ManyColor);
  //preprocess the command line before QApplication
  for(int i=1; i<argc ;i++) {
    if(QString(argv[i]) == "-lowcolor")
      QApplication::setColorSpec(QApplication::NormalColor);
    if(QString(argv[i]) == "-style")
      useNativeStyle=false;
  }

  QApplication app(argc, argv);

  //after Qt parses command line, removes options it recognizes, we then parse the command line
  for (int i=1 ; i < qApp->arguments().size() ; i++) {
    QString argument = qApp->arguments().at(i);

    //Look for options starting with '-'
    if (argument.at(0) == '-') {
      if ( QString(argument) == "-v" ||
           QString(argument) == "--version" ||
           QString(argument) == "-version" )
      {
        stdOutStream << qApp->tr(VERSION_MESSAGE);
        return 0;
      }
      else if(QString(argument) == "-s"){ //script
        scriptfile = QString(qApp->arguments().at(++i));
      }
      else if(QString(argument) == "-noload"){ //scriptload
        scriptload=false;
      }
      else if(QString(argument) == "-e"){ //export image
        imagefile = QString(qApp->arguments()[++i]);
      }
      else if(QString(argument) == "-m"){ //export movie
        moviefile = QString(qApp->arguments()[++i]);
      }
      else{
        stdOutStream << qApp->tr("Unrecognized Option: ");
        stdOutStream << argument;
        stdOutStream << "\a\n\n";
        stdOutStream << qApp->tr(HELP_MESSAGE);
        return -1;
      }
    }
    else{ //treat it as a file to open...
      files->push_back(QString(argument));
    }
  }

  stdOutStream.setDevice(0);
  stdOutFile->close();
  delete stdOutFile;

  createApp();

  qApp->connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));

  return app.exec();
}
