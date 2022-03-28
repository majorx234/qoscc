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

#ifndef DEVICECONTROL_H_
#define DEVICECONTROL_H_

#include <QGroupBox>

#include <QTabWidget>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>

#include "deviceinterface.h"
#include "controllerclass.h"

class DeviceControl : public QGroupBox  {
    Q_OBJECT

public:
    DeviceControl(DeviceInterface *n, ControllerClass* parentController, QTabWidget *parent=0, const std::string name="");
    ~DeviceControl();
signals:
    void hasChanged();
    void setStatus(const QString & text);
    void labelChanged(QWidget *, const QString &);

private:
    DeviceInterface *device;
    ControllerClass* _parentController;

    QComboBox *dspType;
    QComboBox *dspRates;
    QComboBox *dspChannels;
    QComboBox *dspSizes;
    QComboBox *dspNames;
    QComboBox *bufferSize;
    QLineEdit *adjust;

    QLabel *status;
    QMenu *miscOptsMenu;

private slots:
    void getName();
    void showMiscOptsMenu();
    void suicide();
    void tryStart();

    void updateTitle();
    void updateDspType();
    void updateDspRates();
    void updateDspChannels();
    void updateDspSizes();
    void updateStatus();
    void updateDspNames();
    void updateBufferSize();
    void updateAdjust();

    void setDspType();
    void setDspRate();
    void setDspChannel();
    void setDspSize();
    void setDspName();
    void setBufferSize();
    void setAdjust();

public slots:
    void update();
};

#endif // DEVICECONTROL_H_
