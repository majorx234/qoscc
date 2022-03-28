/****************************************************************************
                          tracecontrol.cpp  -  GUI part for traces
                             -------------------
    begin                : Wed Nov 27 2002
    copyright            : (C) 2001 - 2005 by Sven Queisser
                         : (C) 2022 by Majorx234
    email                : tincan@svenqueisser.de
                         : majorx234@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <string>

#include "tracecontrol.h"

#include <math.h>

#include <QCheckBox>
#include <QColor>
#include <QColorDialog>
#include <QComboBox>
#include <QCursor>
#include <QButtonGroup>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QMenu>
#include <QPushButton>
#include <QString>
#include <QTimer>
#include <QToolTip>
#include <QVBoxLayout>

//#include "fslider.h"

#include "traceclass.h"
#include "controllerclass.h"
#include "scopeclass.h"
#include "deviceclass.h"
#include "stringlist.h"
#include "misc.h"
#include "dbuffer.h"

TraceControl::TraceControl(TraceInterface *n, ControllerClass* parentController, QWidget *parent, const std::string ) 
  : QGroupBox(parent) 
  ,_parentController(parentController)
{
    trace = n;

    // context menu
    miscOptsMenu = new QMenu("traceMiscOptsMenu" ,this);
    QAction* setNameAction = new QAction("&Set Name", this);
    QAction* deleteTraceAction = new QAction("&Delete trace", this);
    QAction* setColorAction = new QAction("Set Color", this);

    connect(setNameAction, SIGNAL(triggered()), this, SLOT(getName()));
    connect(deleteTraceAction, SIGNAL(triggered()), this, SLOT(suicide()));
    connect(setColorAction, SIGNAL(triggered()), this, SLOT(setCol()));

    miscOptsMenu->addAction(setNameAction);
    miscOptsMenu->addAction(deleteTraceAction);
    miscOptsMenu->addSeparator();
    miscOptsMenu->addAction(setColorAction);

    QPushButton *btnPopupMenu = new QPushButton(tr("Settings"), this);
    connect(btnPopupMenu, SIGNAL(clicked()), SLOT(showMiscOptsMenu()));

    // parent selector
    QGroupBox *parentSel = new QGroupBox(tr("Device and Channel"), this);
    parentList = new QComboBox(parentSel);
    parentChannel = new QComboBox(parentSel);
    connect(parentList, SIGNAL(highlighted(const QString &)), SLOT(setParentDevice(const QString &)));
    connect(parentChannel, SIGNAL(highlighted(const QString &)), SLOT(setParentChannel(const QString &)));

    // slider for X-Shift of trace
    // TODO
    // sldXPos = new fSlider( this );
    // sldXPos->setInterval(10);
    // sldXPos->setTitle(tr("X-Position (DIVS)"));
    // connect(sldXPos, SIGNAL(valueChanged(float)), SLOT(setXPos(float)));

    // slider for Y-Shift of trace
    // sldYPos = new fSlider( this );
    // sldYPos->setInterval(10);
    // sldYPos->setTitle(tr("Y-Position (DIVS)"));
    // connect(sldYPos, SIGNAL(valueChanged(float)), SLOT(setYPos(float)));

    QGroupBox *box = new QGroupBox(tr("FFT Windowing"), this);
    fftWin = new QComboBox(box);
    fftWin->addItem(tr("Rectangular"), dbuffer::winRect);
    fftWin->addItem(tr("Hanning"), dbuffer::winHanning);
    fftWin->addItem(tr("Hamming"), dbuffer::winHamming);
    fftWin->addItem(tr("Blackman-Harris (var.A)"), dbuffer::winBlackmanHarrisA);
    fftWin->addItem(tr("Blackman-Harris (var.B)"), dbuffer::winBlackmanHarrisB);
    connect(fftWin, SIGNAL(highlighted(int)), SLOT(setFftWin(int)));

    // Buffer settings
    box = new QGroupBox(tr("Buffer settings"), this);
    perfectBuffer = new QCheckBox(tr("Perfect Buffer"), box);
    connect(perfectBuffer, SIGNAL(toggled(bool)), SLOT(setPerfectBuffer(bool)));
    bufferSize = new QComboBox(box);
    bufferSize->setToolTip(tr("Select processing buffer size. "
                        "Selecting a too high value may slow down the application dramatically."));
    bufferSize->setEditable(true);
    bufferSize->addItem("10 ms");
    bufferSize->addItem("100 ms");
    bufferSize->addItem("500 ms");
    bufferSize->addItem("1 s");
    bufferSize->addItem("5 s");
    QPushButton *btnApplyBuffersize = new QPushButton(tr("Set buffersize"), box);
    connect(btnApplyBuffersize, SIGNAL(clicked()), SLOT(setBuffersize()));
    btnApplyBuffersize->setToolTip(tr("Applying the buffersize. This may take a while"));

    update();
}

// ~TraceControl()
// do the suicide..... *g*
TraceControl::~TraceControl() {
    // remove trace from all other arrays
    // from _parentController...
    _parentController->removeTrace(trace);

    stringlist liste;
    _parentController->getScopeList(&liste);
    // from scopes
    for(unsigned int i = 0; i < liste.count(); i++)
        _parentController->getScope(liste.getString(i))->removeTrace(trace);

    // finally remove it
    delete trace;
    trace = 0;

    // tell the rest of the world of my death...
    emit hasChanged() ;
}

void TraceControl::setCol() {
    QColor col = QColorDialog::getColor(QColor(QString::fromStdString(trace->getColor())));
    if(col.isValid())
        trace->setColor(col.name().toStdString());
}

void TraceControl::setXPos(float val) {
    trace->setXShift(val);
}

void TraceControl::setYPos(float val) {
    trace->setYShift(val);
}

void TraceControl::getName() {
    bool ok = false;
    QString text = QInputDialog::getText(this, "QOscC", tr("Enter new name for trace"),
                                         QLineEdit::Normal, QString::fromStdString(trace->getName()), &ok);
    if(!ok || text.isEmpty())
        return;

    trace->setname(text.toLatin1().toStdString());
    updateTitle();
    emit hasChanged();
}

void TraceControl::update() {
    if(!trace)
        return;

    trace->recalc_stringrefs();

    updateTitle();
    updateXPos();
    updateYPos();
    updateParent();
    updateParentChannels();
    updatePerfectBuffer();
    updateBuffersize();
    updateFftWin();
}

void TraceControl::updateTitle() {
    QString title = QString(tr("Trace %1")).arg(QString::fromStdString(trace->getName()));
    emit labelChanged(this, title);
}

void TraceControl::updateXPos() {
    // TODO
    // sldXPos->setValue(trace->getXShift());
}

void TraceControl::updateYPos() {
    // TODO
    // sldYPos->setValue(trace->getYShift());
}

// delete this trace (see destructor...)
void TraceControl::suicide() {
    // remove gui...
    delete this;
}

// void setParentTrace()
// assign this trace to spec. Device
void TraceControl::setParentDevice(const QString &name) {
    // bail out if name is empty
    if(name.isEmpty())
        return;
    if(!_parentController->getDevice(name.toStdString())) {
        QMessageBox::critical(this, tr("Ooops.."), tr("No such Device:") + name);
        return;
    }

    // if selected name is the current, we do nothing.....
    if(trace->getParentName() == name.toStdString())
        return;

    trace->setParentName(name.toStdString());
    updateParentChannels();
}

// void setParentChannel()
// assign to specified channel number
void TraceControl::setParentChannel(const QString &name) {
    // check if correct channel is already set
    if(name.toUInt() == trace->getDevChannel())
        return;
    // else set to correct channel
    trace->setDevChannel(name.toUInt());
}

// void updateParent()
// update Parent selection combo boxes
void TraceControl::updateParent() {
    stringlist liste;
    // get list
    _parentController->getDeviceList(&liste);
    // clear list
    parentList->clear();
    // copy list to combobox
    for(unsigned int i = 0; i < liste.count(); i++) {
        parentList->addItem(QString::fromStdString(liste.getString(i)));
        if(liste.getString(i) == std::string(trace->getParentName()))
            parentList->setCurrentIndex(i);
    }
    // set parent if none is selected by now...
    setParentDevice(parentList->currentText());
}

// void updateParentChannels()
// update Parent selection combo boxes
void TraceControl::updateParentChannels() {
    if(!trace->getParent())
        return;
    
    unsigned int channels = trace->getParent()->getChNum();

    parentChannel->clear();

    for(unsigned int i = 0; i < channels; i++) {
        QString text = "";
        text.setNum(i);
        parentChannel->addItem(text);
        if(i == trace->getDevChannel())
            parentChannel->setCurrentIndex(i);
    }
    // set channel if none is selected by now..
    setParentChannel(parentChannel->currentText());
}

void TraceControl::updatePerfectBuffer() {
    perfectBuffer->setChecked(trace->getPerfectBuffer());
}

void TraceControl::updateBuffersize() {
    if(!trace->getParent())
        return;
    int i;
    for(i = 0; i < bufferSize->count(); i++) {
        if(isNear(stringToNum(bufferSize->itemText(i).toStdString()) * trace->getParent()->getDspRate(), trace->getBufferSize())) {
            bufferSize->setCurrentIndex(i);
            return;
        }
    }
    bufferSize->addItem(QString::fromStdString(numToString((double)trace->getBufferSize() / trace->getParent()->getDspRate())) + "s");
    bufferSize->setCurrentIndex(i);
}

void TraceControl::setBuffersize() {
    double value = stringToNum(bufferSize->currentText().toStdString());
    if(trace->getParent() && value >= 0) {
        emit(setStatus(tr("Setting buffersize. This may take a while")));
        if(trace->setBufferSize((unsigned int)(value * trace->getParent()->getDspRate() + 0.5)))
            emit(setStatus(tr("Setting buffersize to %1 samples failed").arg((unsigned int)(value * trace->getParent()->getDspRate() + 0.5))));
        else
            emit(setStatus(QString(tr("Set buffersize to %1s")).arg(QString::fromStdString(numToString(value)))));
        return;
    }
    emit(setStatus(tr("Cannot set buffersize!")));
}

void TraceControl::setPerfectBuffer(bool) {
    trace->setPerfectBuffer(perfectBuffer->isChecked());
}


void TraceControl::setFftWin(int index){
    dbuffer::fftWinType type = (dbuffer::fftWinType)index;
    trace->setFftWinType(type);
}

void TraceControl::updateFftWin() {
    // items in the combo-box are well-ordered, so this works:
    fftWin->setCurrentIndex(trace->getFftWinType());
}

void TraceControl::showMiscOptsMenu() {
    miscOptsMenu->exec(QCursor::pos());
}