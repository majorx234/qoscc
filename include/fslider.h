/***************************************************************************
                          fslider.h  -  description
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

#ifndef FSLIDER_H
#define FSLIDER_H

#include <QWidget>
#include <QGroupBox>
#include <QSlider>
#include <QLineEdit>
#include <QString>

/**
  *@author Sven Queisser
  *@author MajorX234 
  */

class fSlider : public QGroupBox  {
    Q_OBJECT

public:
    fSlider(const QString name="", QWidget *parent=0);
    ~fSlider();
    float getValue();

public slots:
    void setMinValue(float n);
    void setMaxValue(float n);
    void setInterval(int n);
    void setValue(float n);

private:
    QSlider *slider;
    QLineEdit *textval;
    float min, max;
    float interval;
    float value;

private slots:
    void intValueChanges(int ival);
    void textValueChanges(const QString& str);

signals:
    void valueChanged(float);
};

#endif
