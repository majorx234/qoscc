/***************************************************************************
                          tracecontrol.h
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

#ifndef TRACECONTROL_H
#define TRACECONTROL_H

#include <string>
#include <QGroupBox>

#include "dbuffer.h"

#include <QGroupBox>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QString>
#include <QWidget>

 #include "fslider.h"
#include "traceinterface.h"
#include "controllerclass.h"

class TraceControl : public QGroupBox  {
    Q_OBJECT

public:
    TraceControl(TraceInterface *n, ControllerClass* parentController, QWidget *parent=0, const std::string=0);
    ~TraceControl();
public slots:
    void update();

signals:
    void hasChanged();
    void setStatus(const QString &);
    void labelChanged(QWidget *, const QString &);

private:
    TraceInterface *trace;
    ControllerClass* _parentController;

    QPushButton *btnTCol;
 
    fSlider *sldXPos;
    fSlider *sldYPos;
    QComboBox *parentList;
    QComboBox *parentChannel;

    QCheckBox *perfectBuffer;
    QComboBox *bufferSize;
    QComboBox *fftWin;

    QMenu *miscOptsMenu;

private slots:
    void getName();
    void showMiscOptsMenu();

    void setCol();
    void setXPos(float);
    void setYPos(float);
    void setParentDevice(const QString &);
    void setParentChannel(const QString &);
    void setBuffersize();
    void setPerfectBuffer(bool);
    void setFftWin(int);

    void updateTitle();
    void updateXPos();
    void updateYPos();
    void updateParent();
    void updateParentChannels();
    void updatePerfectBuffer();
    void updateBuffersize();
    void updateFftWin();

    void suicide();
};

#endif // TRACECONTROL_H