/****************************************************************************
** $Id: helpwindow.cpp,v 1.15 2003/07/04 20:20:01 bwan Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "helpwindow.h"
#include <qstatusbar.h>
#include <qpixmap.h>
#include <menu.h>
#include <qmenubar.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qicon.h>
#include <qfile.h>
#include <qtextstream.h>
//#include <qstylesheet.h>
#include <qmessagebox.h>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qcombobox.h>
#include <qevent.h>
#include <qlineedit.h>
#include <qobject.h>
#include <qfileinfo.h>
#include <qfile.h>
#include <qdatastream.h>
#include <qprinter.h>
//#include <q3simplerichtext.h>
#include <qpainter.h>
//#include <q3paintdevicemetrics.h>
#include <QFrame>

#include <ctype.h>
using namespace Qt;

static const char* const home[]={
"16 16 4 1",
"# c #000000",
"a c #ffffff",
"b c #c0c0c0",
". c None",
"........... ....",
"   ....##.......",
"..#...####......",
"..#..#aabb#.....",
"..#.#aaaabb#....",
"..##aaaaaabb#...",
"..#aaaaaaaabb#..",
".#aaaaaaaaabbb#.",
"###aaaaaaaabb###",
"..#aaaaaaaabb#..",
"..#aaa###aabb#..",
"..#aaa#.#aabb#..",
"..#aaa#.#aabb#..",
"..#aaa#.#aabb#..",
"..#aaa#.#aabb#..",
"..#####.######..",
"................"};

static const char* const back[]={
"16 16 5 1",
"# c #000000",
"a c #ffffff",
"c c #808080",
"b c #c0c0c0",
". c None",
"................",
".......#........",
"......##........",
".....#a#........",
"....#aa########.",
"...#aabaaaaaaa#.",
"..#aabbbbbbbbb#.",
"...#abbbbbbbbb#.",
"...c#ab########.",
"....c#a#ccccccc.",
".....c##c.......",
"......c#c.......",
".......cc.......",
"........c.......",
"................",
"......................"};

static const char* const forward[]={
"16 16 5 1",
"# c #000000",
"a c #ffffff",
"c c #808080",
"b c #c0c0c0",
". c None",
"................",
"................",
".........#......",
".........##.....",
".........#a#....",
"..########aa#...",
"..#aaaaaaabaa#..",
"..#bbbbbbbbbaa#.",
"..#bbbbbbbbba#..",
"..########ba#c..",
"..ccccccc#a#c...",
"........c##c....",
"........c#c.....",
"........cc......",
"........c.......",
"................",
"................"};


HelpWindow::HelpWindow( const QString& home_, const QString& _path,
			QWidget* parent, const char *name )
    : QMainWindow( parent ),
      pathCombo( 0 ), selectedURL()
{
    readHistory();
    readBookmarks();

    browser = new QTextBrowser( this );
//    browser->mimeSourceFactory()->setFilePath( _path );
    browser->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    connect( browser, SIGNAL( textChanged() ),
	     this, SLOT( textChanged() ) );

//    setCentralWidget( browser );

    if ( !home_.isEmpty() )
      browser->setSource( home_ );

    connect( browser, SIGNAL( highlighted( const QString&) ),
              statusBar(), SLOT( message( const QString&)) );

    resize( 640,700 );

    QMenu* file = new QMenu(this);
    file->addAction("&New Window", this, SLOT(newWindow()), CTRL+Key_N);
    file->addAction("&Open File", this, SLOT( openFile() ), CTRL+Key_O );
    file->addAction("&Print", this, SLOT( print() ), CTRL+Key_P );
    file->addSeparator();
    file->addAction("&Close", this, SLOT( close() ), CTRL+Key_W );
    file->addAction("&Quit", qApp, SLOT( closeAllWindows() ), CTRL+Key_Q );
//    Q3PopupMenu* file = new Q3PopupMenu( this );
//    file->insertItem( tr("&New Window"), this, SLOT( newWindow() ), CTRL+Key_N );
//    file->insertItem( tr("&Open File"), this, SLOT( openFile() ), CTRL+Key_O );
//    file->insertItem( tr("&Print"), this, SLOT( print() ), CTRL+Key_P );
//    file->insertSeparator();
//    file->insertItem( tr("&Close"), this, SLOT( close() ), CTRL+Key_W );
//    file->insertItem( tr("&Quit"), qApp, SLOT( closeAllWindows() ), CTRL+Key_Q );

    // The same three icons are used twice each.
    QIcon icon_back( QPixmap(( const char** ) back) );
    QIcon icon_forward( QPixmap(( const char** ) forward) );
    QIcon icon_home( QPixmap(( const char** ) home) );

    QMenu* go = new QMenu(this);
    backwardId = go->addAction(icon_back, "&Backward", browser, SLOT(backward()), CTRL+Key_Left);
    forwardId = go->addAction(icon_forward, "&Forward", browser, SLOT(forward()), CTRL+Key_Right);
    go->addAction(icon_home, "&Home", browser, SLOT(home()));
//    Q3PopupMenu* go = new Q3PopupMenu( this );
//    backwardId = go->insertItem( icon_back,
//				 tr("&Backward"), browser, SLOT( backward() ),
//				 CTRL+Key_Left );
//    forwardId = go->insertItem( icon_forward,
//				tr("&Forward"), browser, SLOT( forward() ),
//				CTRL+Key_Right );
//    go->insertItem( icon_home, tr("&Home"), browser, SLOT( home() ) );

    QMenu* help = new QMenu(this);
    help->addAction("&About ...", this, SLOT(about()));
    help->addAction("About &Qt ...", this, SLOT(aboutQt()));
//    Q3PopupMenu* help = new Q3PopupMenu( this );
//    help->insertItem( tr("&About ..."), this, SLOT( about() ) );
//    help->insertItem( tr("About &Qt ..."), this, SLOT( aboutQt() ) );

    hist = new QMenu( this );
    QStringList::Iterator it = history.begin();
    for ( ; it != history.end(); ++it ){
      mHistory[ hist->addAction( *it ) ] = *it;
    }
    connect( hist, SIGNAL( activated( int ) ), this, SLOT( histChosen( int ) ) );

    bookm = new QMenu( this );
    bookm->insertAction( "Add Bookmark", this, SLOT( addBookmark() ) );
    bookm->insertSeparator();
//    bookm = new Q3PopupMenu( this );
//    bookm->insertItem( tr( "Add Bookmark" ), this, SLOT( addBookmark() ) );
//    bookm->insertSeparator();

    QStringList::Iterator it2 = bookmarks.begin();
    for ( ; it2 != bookmarks.end(); ++it2 ){
      mBookmarks[ bookm->insertItem( *it2 ) ] = *it2;
    }
    connect( bookm, SIGNAL( activated( int ) ), this, SLOT( bookmChosen( int ) ) );

    menuBar()->insertItem( tr("&File"), file );
    menuBar()->insertItem( tr("&Go"), go );
    menuBar()->insertItem( tr( "History" ), hist );
    menuBar()->insertItem( tr( "Bookmarks" ), bookm );
    menuBar()->insertSeparator();
    menuBar()->insertItem( tr("&Help"), help );

    menuBar()->setItemEnabled( forwardId, FALSE);
    menuBar()->setItemEnabled( backwardId, FALSE);
    connect( browser, SIGNAL( backwardAvailable( bool ) ),
	     this, SLOT( setBackwardAvailable( bool ) ) );
    connect( browser, SIGNAL( forwardAvailable( bool ) ),
	     this, SLOT( setForwardAvailable( bool ) ) );


    QToolBar* toolbar = new QToolBar( this );
    addToolBar( toolbar, "Toolbar");
    QToolButton* button;

    button = new QToolButton( icon_back, tr("Backward"), "", browser, SLOT(backward()), toolbar );
    connect( browser, SIGNAL( backwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
    button = new QToolButton( icon_forward, tr("Forward"), "", browser, SLOT(forward()), toolbar );
    connect( browser, SIGNAL( forwardAvailable(bool) ), button, SLOT( setEnabled(bool) ) );
    button->setEnabled( FALSE );
    button = new QToolButton( icon_home, tr("Home"), "", browser, SLOT(home()), toolbar );

    toolbar->addSeparator();

    pathCombo = new QComboBox( TRUE, toolbar );
    connect( pathCombo, SIGNAL( activated( const QString & ) ),
	     this, SLOT( pathSelected( const QString & ) ) );
    toolbar->setStretchableWidget( pathCombo );
    setRightJustification( TRUE );
    setDockEnabled( Left, FALSE );
    setDockEnabled( Right, FALSE );

    pathCombo->insertItem( home_ );
    browser->setFocus();

}


void HelpWindow::setBackwardAvailable( bool b)
{
    menuBar()->setItemEnabled( backwardId, b);
}

void HelpWindow::setForwardAvailable( bool b)
{
    menuBar()->setItemEnabled( forwardId, b);
}


void HelpWindow::textChanged()
{
    if ( browser->documentTitle().isNull() ) {
	setCaption( "Helpviewer - " + browser->context() );
	selectedURL = browser->context();
    }
    else {
	setCaption( "Helpviewer - " + browser->documentTitle() ) ;
	selectedURL = browser->documentTitle();
    }
    
    if ( !selectedURL.isEmpty() && pathCombo ) {
	bool exists = FALSE;
	int i;
	for ( i = 0; i < pathCombo->count(); ++i ) {
	    if ( pathCombo->text( i ) == selectedURL ) {
		exists = TRUE;
		break;
	    }
	}
	if ( !exists ) {
	    pathCombo->insertItem( selectedURL, 0 );
	    pathCombo->setCurrentItem( 0 );
	    mHistory[ hist->insertItem( selectedURL ) ] = selectedURL;
	} else
	    pathCombo->setCurrentItem( i );
	selectedURL = QString::null;
    }
}

HelpWindow::~HelpWindow()
{
    history.clear();
    QMap<int, QString>::Iterator it = mHistory.begin();
    for ( ; it != mHistory.end(); ++it )
	history.append( *it );

    QFile f( QDir::currentDirPath() + "/.history" );
    f.open( QIODevice::WriteOnly );
    QDataStream s( &f );
    s << history;
    f.close();

    bookmarks.clear();
    QMap<int, QString>::Iterator it2 = mBookmarks.begin();
    for ( ; it2 != mBookmarks.end(); ++it2 )
	bookmarks.append( *it2 );

    QFile f2( QDir::currentDirPath() + "/.bookmarks" );
    f2.open( QIODevice::WriteOnly );
    QDataStream s2( &f2 );
    s2 << bookmarks;
    f2.close();
}

void HelpWindow::about()
{
    QMessageBox::about( this, "CUViewer HelpViewer",
			"<p>This is based on Qt helpviewer example "
			"using Qt's rich text capabilities</p>"	);
}


void HelpWindow::aboutQt()
{
    QMessageBox::aboutQt( this, "QBrowser" );
}

void HelpWindow::openFile()
{
#ifndef QT_NO_FILEDIALOG
    QString fn = Q3FileDialog::getOpenFileName( QString::null, QString::null, this );
    if ( !fn.isEmpty() )
	browser->setSource( fn );
#endif
}

void HelpWindow::newWindow()
{
    ( new HelpWindow(browser->source(), "qbrowser") )->show();
}

void HelpWindow::print()
{
#ifndef QT_NO_PRINTER
    QPrinter printer;
    printer.setFullPage(TRUE);
    if ( printer.setup() ) {
	QPainter p( &printer );
	Q3PaintDeviceMetrics metrics(p.device());
	int dpix = metrics.logicalDpiX();
	int dpiy = metrics.logicalDpiY();
	const int margin = 72; // pt
	QRect body(margin*dpix/72, margin*dpiy/72,
		   metrics.width()-margin*dpix/72*2,
		   metrics.height()-margin*dpiy/72*2 );
	QFont font("times", 10);
	QStringList filePaths = browser->mimeSourceFactory()->filePath();
	QString file;
	QStringList::Iterator it = filePaths.begin();
	for ( ; it != filePaths.end(); ++it ) {
	    file = Q3Url( *it, Q3Url( browser->source() ).path() ).path();
	    if ( QFile::exists( file ) )
		break;
	    else
		file = QString::null;
	}
	if ( file.isEmpty() )
	    return;
	QFile f( file );
	if ( !f.open( QIODevice::ReadOnly ) )
	    return;
	Q3TextStream ts( &f );
	Q3SimpleRichText richText( ts.read(), font, browser->context(), browser->styleSheet(),
				  browser->mimeSourceFactory(), body.height() );
	richText.setWidth( &p, body.width() );
	QRect view( body );
	int page = 1;
	do {
	    richText.draw( &p, body.left(), body.top(), view, colorGroup() );
	    view.moveBy( 0, body.height() );
	    p.translate( 0 , -body.height() );
	    p.setFont( font );
	    p.drawText( view.right() - p.fontMetrics().width( QString::number(page) ),
			view.bottom() + p.fontMetrics().ascent() + 5, QString::number(page) );
	    if ( view.top()  >= richText.height() )
		break;
	    printer.newPage();
	    page++;
	} while (TRUE);
    }
#endif
}

void HelpWindow::pathSelected( const QString &_path )
{
    browser->setSource( _path );
    QMap<int, QString>::Iterator it = mHistory.begin();
    bool exists = FALSE;
    for ( ; it != mHistory.end(); ++it ) {
	if ( *it == _path ) {
	    exists = TRUE;
	    break;
	}
    }
    if ( !exists )
	mHistory[ hist->insertItem( _path ) ] = _path;
}

void HelpWindow::readHistory()
{
    if ( QFile::exists( QDir::currentDirPath() + "/.history" ) ) {
	QFile f( QDir::currentDirPath() + "/.history" );
	f.open( QIODevice::ReadOnly );
	QDataStream s( &f );
	s >> history;
	f.close();
	while ( history.count() > 20 )
	    history.remove( history.begin() );
    }
}

void HelpWindow::readBookmarks()
{
    if ( QFile::exists( QDir::currentDirPath() + "/.bookmarks" ) ) {
	QFile f( QDir::currentDirPath() + "/.bookmarks" );
	f.open( QIODevice::ReadOnly );
	QDataStream s( &f );
	s >> bookmarks;
	f.close();
    }
}

void HelpWindow::histChosen( int i )
{
    if ( mHistory.contains( i ) )
	browser->setSource( mHistory[ i ] );
}

void HelpWindow::bookmChosen( int i )
{
    if ( mBookmarks.contains( i ) )
	browser->setSource( mBookmarks[ i ] );
}

void HelpWindow::addBookmark()
{
    mBookmarks[ bookm->insertItem( caption() ) ] = caption();
}
