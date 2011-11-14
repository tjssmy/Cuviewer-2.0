#include <QtGui/QApplication>
#include <QTextStream>
#include <QWindowsStyle>
#include <QString>
#include <QStringList>
#include <QFile>

#include "mainwindow.h"
#include "config.h"

bool useDefaultStyle,scriptload;
QStringList* files;
QString scriptfile, imagefile, moviefile;

void createApp()
{
  MainWindow *mainwindow = new MainWindow();
  mainwindow->setWindowTitle("Carleton University Viewer");

  if (!scriptfile.isEmpty()){
    //Only first file executes scriptfile
    mainwindow->loadSeparately(*files);
  }

  if (useDefaultStyle){
    qApp->setStyle(new QWindowsStyle());
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
  useDefaultStyle=false;
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
      useDefaultStyle=false;
  }

  QApplication app(argc, argv);

  //after Qt parses command line, removes options it recognizes, we then parse the command line
  for (int i=1 ; i < qApp->arguments().size() ; i++) {
    //Look for options starting with '-'
    if (qApp->arguments().at(i).at(0) == '-') {
      if ( QString(qApp->arguments().at(i)) == "-v" ||
           QString(qApp->arguments().at(i)) == "--version" ||
           QString(qApp->arguments().at(i)) == "-version" )
      {
        stdOutStream << qApp->tr(VERSION_MESSAGE);
        return 0;
      }
      else if(QString(qApp->arguments().at(i)) == "-s"){ //script
        scriptfile = QString(qApp->arguments().at(++i));
      }
      else if(QString(qApp->arguments().at(i)) == "-noload"){ //scriptload
        scriptload=false;
      }
      else if(QString(qApp->arguments().at(i)) == "-e"){ //export image
        imagefile = QString(qApp->arguments()[++i]);
      }
      else if(QString(qApp->arguments().at(i)) == "-m"){ //export movie
        moviefile = QString(qApp->arguments()[++i]);
      }
      else{
        stdOutStream << qApp->tr("Unrecognized Option: ");
        stdOutStream << qApp->arguments().at(i);
        stdOutStream << "\a\n\n";
        stdOutStream << qApp->tr(HELP_MESSAGE);
        return -1;
      }
    }
    else{ //treat it as a file to open...
      files->push_back(QString(qApp->arguments().at(i)));
    }
  }

  stdOutStream.setDevice(0);
  stdOutFile->close();
  delete stdOutFile;

  createApp();

  qApp->connect(qApp, SIGNAL(lastWindowClosed()), qApp, SLOT(quit()));

  return app.exec();
}
