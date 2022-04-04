#include "scopeview.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <string.h>

//qt
#include <QtGlobal>
#include <QWidget>
#include <QApplication>
#include <QCursor>
#include <QEvent>
#include <QPainter>
#include <QPixmap>

ScopeView::ScopeView(ScopeInterface* _modelScope, QWidget *parent, const char *_name) : modelScope(_modelScope), QWidget(parent) {
    mouse_x = 0;
    mouse_y = 0;
    // makes double-buffering possible...
    setAttribute(Qt::WA_OpaquePaintEvent);

    // set a nice cursor
    setCursor(Qt::CrossCursor);

    // create new painter and pixmap
    painter = new QPainter;
    pm = new QPixmap(qMax(pm->width(), width()), qMax(pm->height(), height()));
    pm->fill(QColor(QString::fromStdString(modelScope->getBCol())));
    show();
}

ScopeView::~ScopeView() {
  delete painter;
  //maybe this too? 
  //delete pixmap;
}

// void mouseMoveEvent(QMouseMoveEvent)
// reimplementation that handles mouse movements
void ScopeView::mousePressEvent(QMouseEvent *e) {
    setMouseX(e->x());
    setMouseY(e->y());
    // redraw scope immediately...
    triggerRedrawScope();
}

unsigned int ScopeView::getMouseX() {
    return mouse_x;
}

unsigned int ScopeView::getMouseY() {
    return mouse_y;
}

void ScopeView::setMouseX(unsigned int mouse_x_new) {
    mouse_x = mouse_x_new;
}
void ScopeView::setMouseY(unsigned int mouse_y_new) {
    mouse_y = mouse_y_new;
}

bool ScopeView::update(const int value) {
    return true;
}