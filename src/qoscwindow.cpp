#include <QtGui>
#include <qmessagebox.h>
#include <QTabWidget>
#include <QLabel>

#include "qoscwindow.h"
#include "ui_qoscwindow.h"

QOscWindow::QOscWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QOscWindow)
{
  ui->setupUi(this);

  // read configuration
  conffilename = std::string(getenv("HOME")) + "/.qosccrc";

  setWindowTitle("QOscC " VERSION);
  groups = new QTabWidget(this);
  setCentralWidget(groups);

// later to initActions()
  fileOpen = new QAction("&Open", this);
  fileOpen->setShortcut(QKeySequence::Open);
  
  connect(fileOpen, SIGNAL(triggered()), this, SLOT(slotFileOpen()));

  fileQuit = new QAction("&Exit", this);
  fileQuit->setShortcut(QKeySequence::Close);
  
  connect(fileQuit, SIGNAL(triggered()), this, SLOT(slotFileQuit()));

  helpAboutApp = new QAction("&Help", this);
  helpAboutApp->setShortcut(QKeySequence::HelpContents);
  
  connect(helpAboutApp, SIGNAL(triggered()), this, SLOT(slotHelpAbout()));


// later to initMenuBar();
  fileMenu = new QMenu("&File");
  helpMenu = new QMenu("&Help");
  setMenu  = new QMenu("&Settings");
  fileMenu->addAction(fileOpen);
  fileMenu->addAction(fileQuit);
  helpMenu->addAction(helpAboutApp);  
  menuBar()->addMenu(fileMenu);
  menuBar()->addMenu(helpMenu);
/*
  //create a toolBart
  toolBarFile = addToolBar("File");
  //add the same action in the toolbar.
  toolBarFile->addAction(actionOpen);
 */
  

  

  statusBar()->addWidget(new QLabel(tr("Welcome to QOscC Version %1").arg(VERSION), this));
}

QOscWindow::~QOscWindow() {
  delete ui;
}

void QOscWindow::slotFileOpen()
{
     statusBar()->showMessage(tr("File Opened"),2000);
}

void QOscWindow::slotFileQuit() {
    // store configuration

    // then exit
    qApp->quit();
}

void QOscWindow::slotHelpAbout() {
    QMessageBox::about(this, tr("About..."), tr("QOscC\nVersion %1\n(c) 2001 - 2005 by Sven Queisser\n 2017 by MajorX234").arg(VERSION) );
}