#include <QtGui>
#include <QMessageBox>
#include <QTabWidget>
#include <QLabel>
#include <QInputDialog>

#include "qosccwindow.h"
#include "ui_qosccwindow.h"
#include "scopecontrol.h"
#include "devicecontrol.h"
#include "tracecontrol.h"
#include "deviceclass.h"
#include "traceclass.h"
#include "scopeclass.h"

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

/*
  //create a toolBart
  toolBarFile = addToolBar("File");
  //add the same action in the toolbar.
  toolBarFile->addAction(actionOpen);
 */
  initActions();
  initMenuBar();
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

    stringlist traceliste;
    controller->getTraceList(&traceliste);
    // create controls for traces
    for(unsigned int i = 0; i < traceliste.count(); i++) {
        TraceControl *tracectl = new TraceControl(controller->getTrace(traceliste.getString(i)), controller, groups, "trace");
        QString title = QString(tr("Trace %1")).arg(QString::fromStdString(traceliste.getString(i)));
        groups->addTab(tracectl, title);
        connect(this, SIGNAL(hasChanged()), tracectl, SLOT(update()));
        connect(tracectl, SIGNAL(hasChanged()), this, SLOT(update()));
        connect(tracectl, SIGNAL(setStatus(const QString& )), SLOT(setStatus(const QString&)));
        connect(tracectl, SIGNAL(labelChanged(QWidget*, const QString&)), SLOT(changeLabel(QWidget*, const QString&)));
    }
}

void QOscCWindow::destroyControls() {
    // hugh, im not sure this works....
    for(int i = 0; i < groups->count(); i++) {
        delete groups->widget(i);
    }
}

// initializes all QActions of the application
void QOscCWindow::initActions() {
    fileOpen = new QAction("&Open", this);
    fileOpen->setShortcut(QKeySequence::Open);
    connect(fileOpen, SIGNAL(triggered()), this, SLOT(slotFileOpen()));

    fileQuit = new QAction(tr("E&xit"), this);
    fileQuit->setShortcuts(QKeySequence::Close);
    connect(fileQuit, SIGNAL(triggered()), this, SLOT(slotFileQuit()));

    setSave = new QAction(tr("S&tore settings"), this);
    connect(setSave, SIGNAL(triggered()), this, SLOT(saveSettings()));

    setAddDevice = new QAction(tr("Add &device"), this);
    connect(setAddDevice, SIGNAL(triggered()), this, SLOT(addDevice()));

    setAddScope = new QAction(tr("Add &scope window"), this);
    connect(setAddScope, SIGNAL(triggered()), this, SLOT(addScope()));

    setAddTrace = new QAction(tr("Add &trace"), this);
    connect(setAddTrace, SIGNAL(triggered()), this, SLOT(addTrace()));

    helpAboutApp = new QAction(tr("&About..."), this);
    helpAboutApp->setShortcut(QKeySequence::HelpContents);
    connect(helpAboutApp, SIGNAL(triggered()), this, SLOT(slotHelpAbout()));
}

void QOscCWindow::initMenuBar() {
    fileMenu=new QMenu(tr("&File"), this);
    fileMenu->addAction(fileOpen);
    fileMenu->addAction(fileQuit);

    setMenu = new QMenu(tr("&Settings"), this);
    setMenu->addAction(setSave);
    setMenu->addAction(setAddDevice);
    setMenu->addAction(setAddScope);
    setMenu->addAction(setAddTrace);

    helpMenu=new QMenu(tr("&Help"), this);
    helpMenu->addAction(helpAboutApp);

    menuBar()->addMenu(fileMenu);
    menuBar()->addMenu(setMenu);
    menuBar()->addSeparator();
    menuBar()->addMenu(helpMenu);
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

// write config file
void QOscCWindow::saveSettings() {
    controller->writeconfig(conffilename);
    setStatus(tr("Configuration file written"));
}

// void addDevice()
// add a new device
void QOscCWindow::addDevice() {
    // first ask for a name for the new device...
    QString name = tr("Device");
    bool ok = false;
    do {
        name = QInputDialog::getText(this, "QOscC", tr("Enter new name for device"),
                                     QLineEdit::Normal, name, &ok );
        if(!ok) // cancel pressed, so we abort all this.....
            return;
        if(name.isEmpty()) {
            QMessageBox::warning(this, tr("QOscC -- Ooops.."), tr("You should specify a name"));
            ok = false;
        }
        if(controller->getDevice(name.toStdString())) {
            QMessageBox::warning(this, tr("QOscC -- Ooops.."), tr("This name already exists.\nPlease select another."));
            ok = false;
        }
    } while(!ok);

    DeviceClass *newdevice = new DeviceClass(controller);

    newdevice->setname(name.toLatin1().toStdString());

    controller->addDevice(newdevice);

    DeviceControl *devctl = new DeviceControl(newdevice, controller, groups, name.toStdString());
    groups->addTab(devctl, QString(tr("Device ")) + name);
    groups->hide();
    // select this page. it is the last available.
    groups->setCurrentIndex(groups->count()-1);
    groups->show();

    connect(this, SIGNAL(hasChanged()), devctl, SLOT(update()));
    connect(devctl, SIGNAL(hasChanged()), this, SLOT(update()));
    connect(devctl, SIGNAL(setStatus(const QString& )), SLOT(setStatus(const QString&)));
    connect(devctl, SIGNAL(labelChanged(QWidget*, const QString&)), SLOT(changeLabel(QWidget*, const QString&)));
    emit update();
    setStatus(tr("Device %1 added").arg(name));
}

// void addScope()
// adds a new scope
void QOscCWindow::addScope() {
    QString name = tr("Scope");
    bool ok = false;
    do {
        name = QInputDialog::getText(this, "QOscC", tr("Enter new name for this scope"),
                                     QLineEdit::Normal, name, &ok);
        if(!ok) // cancel pressed, so we abbort all this.....
            return;
        if(name.isEmpty()) {
            QMessageBox::warning(this, tr("QOscC -- Ooops.."), tr("You should specify a name"));
            ok = false;
        }
        if(controller->getScope(name.toStdString())) {
            QMessageBox::warning(this, tr("QOscC -- Ooops.."), tr("This name already exists.\nPlease select another."));
            ok = false;
        }
    } while(!ok);

    // add scope
    ScopeClass *newscope  = new ScopeClass(controller );
    newscope->setName(name.toStdString());
    controller->addScope(newscope);

    // add control
    ScopeControl *scopectl = new ScopeControl(newscope, controller, groups, "scope");
    QString title = QString(tr("Scope ")) + name;
    groups->addTab(scopectl, title);
    groups->hide();
    // select this page. it is the last available.
    groups->setCurrentIndex(groups->count()-1);
    groups->show();

    connect(this, SIGNAL(hasChanged()), scopectl, SLOT(update()));
    connect(scopectl, SIGNAL(hasChanged()), this, SLOT(update()));
    connect(scopectl, SIGNAL(setStatus(const QString& )), SLOT(setStatus(const QString&)));
    connect(scopectl, SIGNAL(labelChanged(QWidget*, const QString&)), SLOT(changeLabel(QWidget*, const QString&)));

    emit update();
    setStatus(tr("Scope window %1 added").arg(name));
}

// void addTrace()
// adds a new trace
void QOscCWindow::addTrace() {
    QString name = tr("Trace");
    bool ok = false;
    do {
        name = QInputDialog::getText(this, "QOscC", tr("Enter new name for trace"),
                                     QLineEdit::Normal, name, &ok);
        if(!ok) // cancel pressed, so we abbort all this.....
            return;
        if(name.isEmpty()) {
            QMessageBox::warning(this, tr("QOscC -- Ooops.."), tr("You should specify a name"));
            ok = false;
        }
        if(controller->getTrace(name.toStdString())) {
            QMessageBox::warning(this, tr("QOscC -- Ooops.."), tr("This name already exists.\nPlease select another."));
            ok = false;
        }
    } while(!ok);

    // add trace
    TraceClass *newtrace  = new TraceClass(controller);
    newtrace->setname(name.toStdString());
    controller->addTrace(newtrace);

    // add control
    TraceControl *tracectl = new TraceControl(newtrace, controller, groups, "trace");
    QString title = QString(tr("Trace ")) + name;
    groups->addTab(tracectl, title);
    groups->hide();
    // select this page. it is the last available.
    groups->setCurrentIndex(groups->count()-1);
    groups->show();

    connect(this, SIGNAL(hasChanged()), tracectl, SLOT(update()));
    connect(tracectl, SIGNAL(hasChanged()), this, SLOT(update()));
    connect(tracectl, SIGNAL(setStatus(const QString& )), SLOT(setStatus(const QString&)));
    connect(tracectl, SIGNAL(labelChanged(QWidget*, const QString&)), SLOT(changeLabel(QWidget*, const QString&)));
    emit(update());
    setStatus(tr("Trace %1 added").arg(name));
}

// void setStatus(const QString &)
// set the text in the Status bar
void QOscCWindow::setStatus(const QString & text) {
    status->showMessage(text);
}

// void update();
// udpate all widgets in this application.....
void QOscCWindow::update() {
    emit(hasChanged());
}

// void changeLabel(const QWidget&, const QString&)
// changes the widgets label...
void QOscCWindow::changeLabel(QWidget *w, const QString &n) {
    int index = groups->indexOf(w);
    groups->setTabText(index, n);
}
