#include <QtGui>
#include <qmessagebox.h>
#include <QTabWidget>
#include <QLabel>

#include "qosccwindow.h"
#include "ui_qosccwindow.h"
#include "scopecontrol.h"
#include "devicecontrol.h"

QOscCWindow::QOscCWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QOscCWindow)
{
  ui->setupUi(this);
  controller = new ControllerClass();
  // read configuration
  conffilename = std::string(getenv("HOME")) + "/.qosccrc";
  if(controller->readconfig(conffilename)) {
    QMessageBox::information(this, 
                             tr("QOscC -- No configuration file found"),
                             tr("No configuration file (%1) has been found. I'll use default settings.\n" 
                                "You first have to add at least a trace, a scope and a device and configure "
                                "them to your needs.").arg(QString::fromStdString(conffilename)));
  }
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
  
  createControls();

  

  statusBar()->addWidget(new QLabel(tr("Welcome to QOscC Version %1").arg(VERSION), this));
}

QOscCWindow::~QOscCWindow() {
  delete ui;
}

// add all traces and devices that exist by now
void QOscCWindow::createControls() {
    // create controls for scopes
    stringlist scopeliste;
    controller->getScopeList(&scopeliste);
    for(unsigned int i = 0; i < scopeliste.count(); i++) {
        ScopeControl *scopectl = new ScopeControl(controller->getScope(scopeliste.getString(i)), controller, groups, "scope");
        QString title = QString(tr("Scope %1")).arg(QString::fromStdString(scopeliste.getString(i)));
        groups->addTab(scopectl, title);
        connect(this, SIGNAL(hasChanged()), scopectl, SLOT(update()));
        connect(scopectl, SIGNAL(hasChanged()), this, SLOT(update()));
        connect(scopectl, SIGNAL(setStatus(const QString& )), SLOT(setStatus(const QString&)));
        connect(scopectl, SIGNAL(labelChanged(QWidget*, const QString&)), SLOT(changeLabel(QWidget*, const QString&)));
    }
    
    stringlist deviceliste;
    controller->getDeviceList(&deviceliste);
    // create controls for devices
    for(unsigned int i = 0; i < deviceliste.count(); i++) {
        DeviceControl *devctl = new DeviceControl(controller->getDevice(deviceliste.getString(i)), controller, groups, "device");
        QString title = QString(tr("Device %1")).arg(QString::fromStdString(deviceliste.getString(i)));
        groups->addTab(devctl, title);
        connect(this, SIGNAL(hasChanged()), devctl, SLOT(update()));
        connect(devctl, SIGNAL(hasChanged()), this, SLOT(update()));
        controller->getDevice(deviceliste.getString(i))->start();
        connect(devctl, SIGNAL(setStatus(const QString& )), SLOT(setStatus(const QString&)));
        connect(devctl, SIGNAL(labelChanged(QWidget*, const QString&)), SLOT(changeLabel(QWidget*, const QString&)));
    }
}

void QOscCWindow::destroyControls() {

}

void QOscCWindow::slotFileOpen()
{
    statusBar()->showMessage(tr("File Opened"),2000);
}

void QOscCWindow::slotFileQuit() {
    // store configuration

    // then exit
    qApp->quit();
}

void QOscCWindow::slotHelpAbout() {
    QMessageBox::about(this, tr("About..."), tr("QOscC\nVersion %1\n(c) 2001 - 2005 by Sven Queisser\n 2017 by MajorX234").arg(VERSION) );
}
