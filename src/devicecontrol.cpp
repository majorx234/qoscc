/***************************************************************************
                          devicecontrol.h  -  description
                             -------------------
    begin                : Wed Nov 27 2002
    copyright            : (C) 2002 by Sven Queisser
                         : (C) 2022 by Majorx234    
    email                : svenqueisser@t-online.de
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

#include "devicecontrol.h"

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
#include <QTabWidget>
#include <QTimer>
#include <QString>

#include <stdlib.h>

#include "dspcommon.h"
#include "stringlist.h"
#include "misc.h"
#include "tracecontrol.h"
#include "controllerclass.h"
#include "traceclass.h"
#include "deviceclass.h"

DeviceControl::DeviceControl(DeviceInterface *n, ControllerClass* parentController, QTabWidget *parent, const std::string name) 
  : QGroupBox(QString::fromStdString(name),parent)
    ,_parentController(parentController)
{
    device = n;

    // misc options context menu
    miscOptsMenu = new QMenu("deviceMiscOptsMenu",this);
    QAction* setNameAction = new QAction("&Set Name", this);
    QAction* deleteDeviceAction = new QAction("&Delete device", this);
    connect(setNameAction, SIGNAL(triggered()), this, SLOT(getName()));
    connect(deleteDeviceAction, SIGNAL(triggered()), this, SLOT(suicide()));
    miscOptsMenu->addAction(setNameAction);
    miscOptsMenu->addAction(deleteDeviceAction);

    QPushButton *btnPopupMenu = new QPushButton(tr("Settings"), this);
    connect(btnPopupMenu, SIGNAL(clicked()), SLOT(showMiscOptsMenu()));

    QGroupBox *box = new QGroupBox(tr("DSP Type"), this);
    dspType = new QComboBox(box);
    // the sequence of these inserts MUST be in order of the list in dspcommon.h
    dspType->addItem(tr("None"), PCM_NONE);
    dspType->addItem(tr("OSS"), PCM_OSS);
    dspType->addItem(tr("ALSA"), PCM_ALSA);
    dspType->addItem(tr("JACK"), PCM_JACK);
    dspType->addItem(tr("Serial Multimeter"), PCM_MM);

    box = new QGroupBox(tr("DSP Parameters"), this);
    QGroupBox *dspparam = new QGroupBox(tr("Sample rate"), box);
    dspRates = new QComboBox(dspparam);
    dspparam = new QGroupBox(tr("Sample Size"), box);
    dspSizes = new QComboBox(dspparam);
    dspparam = new QGroupBox(tr("Channels"), box);
    dspChannels = new QComboBox(dspparam);
    dspChannels->setEditable(true);

    box = new QGroupBox(tr("Device File / Identifier"), this);
    dspNames = new QComboBox(box);
    dspNames->setEditable(true);

    box = new QGroupBox(tr("Status"), this);
    status = new QLabel(tr("Idle"), box);
    QPushButton *btnStart = new QPushButton(tr("Start"), box);
    connect(btnStart, SIGNAL(clicked()), SLOT(tryStart()));

    box = new QGroupBox(tr("Buffersize"), this);
    bufferSize = new QComboBox(box);
    QPushButton *btnSetBufferSize = new QPushButton(tr("Apply"), box);
    connect(btnSetBufferSize, SIGNAL(clicked()), SLOT(setBufferSize()));
    bufferSize->addItem("10 ms");
    bufferSize->addItem("100 ms");
    bufferSize->addItem("500 ms");
    bufferSize->addItem("1 s");
    bufferSize->addItem("5 s");
    bufferSize->setEditable(true);

    // Adjust value line and button
    box = new QGroupBox(tr("Adjust value"), this);
    adjust = new QLineEdit(box);
    QPushButton *btnSetAdjust = new QPushButton(tr("Apply"), box);
    connect(btnSetAdjust, SIGNAL(clicked()), SLOT(setAdjust()));
    adjust->setToolTip(tr("The input signal is multiplied by that value to obtain correct level readings"));

    QTimer *dynamicUpdateTimer = new QTimer(this); //DynamicUpdateTimer-Device
    dynamicUpdateTimer->start(100);

    connect(dynamicUpdateTimer, SIGNAL(timeout()), SLOT(updateStatus()));

    update();

    connect(dspType, SIGNAL(activated(int)), SLOT(setDspType()));
    connect(dspRates, SIGNAL(activated(int)), SLOT(setDspRate()));
    connect(dspSizes, SIGNAL(activated(int)), SLOT(setDspSize()));
    connect(dspChannels, SIGNAL(activated(int)), SLOT(setDspChannel()));
    connect(dspNames, SIGNAL(activated(int)), SLOT(setDspName()));
}


DeviceControl::~DeviceControl() {
    // clean up....
    _parentController->removeDevice(device);
    emit setStatus(tr("Device %1 removed").arg(QString::fromStdString(device->getName())));
    delete device;
}

void DeviceControl::update() {
    updateTitle();
    updateDspType();
    updateDspRates();
    updateDspChannels();
    updateDspSizes();
    updateDspNames();
    updateStatus();
    updateBufferSize();
    updateAdjust();
}

void DeviceControl::updateTitle() {
    QString title = QString(tr("Device %1")).arg(QString::fromStdString(device->getName()));
    emit labelChanged(this, title);
}

// void getName()
// re-ask the name for this device
void DeviceControl::getName() {
    QString name = QString::fromStdString(device->getName());
    bool ok = false;
    do {
        name = QInputDialog::getText(this,"QOscC", tr("Enter new name for this device"),
                                     QLineEdit::Normal, QString::fromStdString(device->getName()), &ok);
    
        if(!ok) // cancel pressed, so we abbort all this.....
            return;
        if(name.isEmpty()) {
            QMessageBox::warning(this, tr("QOscC -- Ooops.."), tr("You should specify a name"));
            ok = false;
        }
        if(_parentController->getScope(name.toStdString())) {
            QMessageBox::warning(this, tr("QOscC -- Ooops.."), tr("This name already exists.\nPlease select another."));
            ok = false;
        }
    } while(!ok);

    device->setname(name.toStdString());
    updateTitle();
    emit hasChanged();
    setStatus(tr("Changed name to %1").arg(name));
}

void DeviceControl::suicide() {
    delete this;
}

void DeviceControl::updateDspType() {
    dspType->setCurrentIndex(device->getDspType());
}

void DeviceControl::updateDspRates() {
    stringlist liste;
    dspRates->clear();
    device->getDspRateList(&liste);
    for(unsigned int i = 0; i < liste.count(); i++) {
        dspRates->addItem(QString::fromStdString(liste.getString(i)));
        if(unsigned (atoi(liste.getString(i).c_str())) == device->getDspRate())
            dspRates->setCurrentIndex(i);
    }
}

void DeviceControl::updateDspChannels() {
    stringlist liste;
    dspChannels->clear();
    device->getDspChannelList(&liste);
    for(unsigned int i = 0; i < liste.count(); i++) {
        dspChannels->addItem(QString::fromStdString(liste.getString(i)));
        if(unsigned (atoi(liste.getString(i).c_str())) == device->getChannels())
            dspChannels->setCurrentIndex(i);
    }
}

void DeviceControl::updateDspSizes() {
    stringlist liste;
    dspSizes->clear();
    device->getDspSizeList(&liste);
    for(unsigned int i = 0; i < liste.count(); i++) {
        dspSizes->addItem(QString::fromStdString(liste.getString(i)));
        if(atoi(liste.getString(i).c_str()) == device->getDspSize())
            dspSizes->setCurrentIndex(i);
    }
}

void DeviceControl::updateStatus() {
    status->setText(device->isRunning() ? tr("Running") : tr("Idle"));
}

void DeviceControl::updateDspNames() {
    stringlist liste;
    dspNames->clear();
    device->getDspNameList(&liste);
    for(unsigned int i = 0; i < liste.count(); i++) {
        dspNames->addItem(QString::fromStdString(liste.getString(i)));
        if(liste.getString(i) == device->getdevname())
            dspNames->setCurrentIndex(i);
    }
}

void DeviceControl::setDspType() {
    device->setDeviceType(dspType->currentIndex());
    // re-set parameters, as their lists have changed and the
    // dsp-class in deviceClass is not yet initialized correct.
    //    update();
    emit hasChanged();
}

void DeviceControl::setDspRate() {
    double value = dspRates->currentText().toDouble();
    if(!device->setDspRate(value))
        emit setStatus(tr("Set samplingrate to %1Hz").arg(QString::fromStdString(numToString(value))));
    else
        emit setStatus(tr("Unable to set the Samplingrate to %1Hz").arg(QString::fromStdString(numToString(value))));

    /*    updateDspRates();
        updateBufferSize();*/
    emit hasChanged();
}

void DeviceControl::setDspChannel() {
    if(device->setChannels(dspChannels->currentText().toInt()))
        emit(setStatus(tr("Could not change number of channels")));
    else
        emit(setStatus(tr("Changed number of Channels")));
    /*    updateDspChannels();*/
    emit hasChanged();
}

void DeviceControl::setDspSize() {
    device->setDspSize(dspSizes->currentText().toInt());
    /*   updateDspSizes();*/
    emit hasChanged();
}

void DeviceControl::setDspName() {
    device->setdevname(dspNames->currentText().toStdString());
    //    update();
    emit hasChanged();
}

// void tryStart()
// tries to start the device
void DeviceControl::tryStart() {
    // first re-apply all settings...
    setDspName();
    setDspRate();
    setDspSize();
    setDspChannel();
    setBufferSize();

    if(!device->start())
        emit setStatus(tr("Device %1 succesfully started").arg(QString::fromStdString(device->getName())));
    else
        emit setStatus(tr("Device %1 could not be started!!").arg(QString::fromStdString(device->getName())));
    // update, since some parameters may change at start (buffer size, sample size...)
    //    update();
    emit hasChanged();
}

void DeviceControl::setBufferSize() {
    device->setBuffersize(int(stringToNum(bufferSize->currentText().toStdString()) * device->getDspRate() + 0.5));
    updateBufferSize();
    emit setStatus(tr("New buffersize: %1 seconds (%2 samples)")
                   .arg((double)device->getBuffersize() / device->getDspRate())
                   .arg(device->getBuffersize()));
    // re-read buffersize, for the case that it was not accepted as is
    updateBufferSize();
}

void DeviceControl::updateBufferSize() {
    bool selected = false;
    int i;
    for(i = 0; i < bufferSize->count(); i++) {
        if(isNear(stringToNum(bufferSize->itemText(i).toStdString()) * device->getDspRate(), device->getBuffersize())) {
            bufferSize->setCurrentIndex(i);
            selected = true;
        }
    }
    // add new item, if it is not yet in the list
    // do not add them if either buffersize or samplerate is zero, as this
    // would lead to nonsense values!
    if(!selected && device->getDspRate() != 0 && device->getBuffersize() != 0) {
        bufferSize->addItem(QString::fromStdString(numToString((double)device->getBuffersize() / device->getDspRate())) + "s");
        bufferSize->setCurrentIndex(i);
    }
}

void DeviceControl::setAdjust() {
    bool ok;

    double value = adjust->text().toDouble(&ok);
    if(!ok) {
        emit(setStatus(tr("Invalid expression for adjust value")));
        return;
    }
    device->setAdjust(value);
}

void DeviceControl::updateAdjust() {
    adjust->setText(QString("%1").arg(device->getAdjust()));
}

void DeviceControl::showMiscOptsMenu() {
    miscOptsMenu->exec(QCursor::pos());
}
