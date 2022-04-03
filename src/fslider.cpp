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

#include "fslider.h"

#include <QSlider>
#include <QString>
#include <QLineEdit>
#include <QHBoxLayout>

fSlider::fSlider(const  QString name, QWidget *parent) : QGroupBox(name, parent) {
    min = -10.0;
    max = 10.0;
    interval = 500;
    value = 0.0;

    slider = new QSlider( this );
    textval = new QLineEdit( this );
    QHBoxLayout *hbox = new QHBoxLayout(this);

    textval->setMinimumWidth(80);
    textval->setMaximumWidth(80);

    QString str;
    str.setNum(0);
    textval->setText(str);

    slider->setMinimumWidth(200);

    slider->setOrientation(Qt::Horizontal);
    slider->setTickPosition(QSlider::TicksAbove);

    slider->setValue(int(value));
    slider->setMinimum(int(min * interval));
    slider->setMaximum(int(max * interval));
    slider->setTickInterval(int(interval));
    
    connect(slider, SIGNAL(valueChanged(int)), SLOT(intValueChanges(int)));
    connect(textval, SIGNAL(textChanged(const QString&)), SLOT(textValueChanges(const QString&)));

    hbox->addWidget(slider);
    hbox->addWidget(textval);
    hbox->addStretch(1);
    setLayout(hbox);
}

fSlider::~fSlider() {}

void fSlider::intValueChanges(int ival) {
    float newval;

    newval = (float)ival / (float)interval;
    if(newval != value) {
        value = newval;
        emit(valueChanged(value));
    }

    QString str;
    str.setNum(value);
    textval->setText(str);
}

void fSlider::setMinValue(float n) {
    slider->setMinimum(int(interval * n));
    min = n;
}

void fSlider::setMaxValue(float n) {
    slider->setMaximum(int(interval * n));
    max = n;
}

void fSlider::setInterval(int n) {
    slider->setTickInterval(n);
    interval = n;
    slider->setMinimum(int(interval * min));
    slider->setMaximum(int(interval * max));
    setValue(value);
}

void fSlider::setValue(float n) {
    /* set slider */
    slider->setValue(int(n * interval));
    value = n;
}

float fSlider::getValue() {
    return value;
}

void fSlider::textValueChanges(const QString& str) {
    QString text;

    setValue(str.toFloat());

    /* force value and text in its boundaries! */
    if(value > max) {
        setValue(max);
        text.setNum(max);
        textval->setText(text);
    }
    if(value < min) {
        setValue(min);
        text.setNum(min);
        textval->setText(text);
    }
}
