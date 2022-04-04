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

    bcol    = "#000000";
    gridcol = "#009f00";
    markcol = "#ffffff";
    textcol = "#ffffff";
    // makes double-buffering possible...
    setAttribute(Qt::WA_OpaquePaintEvent);

    // set a nice cursor
    setCursor(Qt::CrossCursor);

    // create new painter and pixmap
    painter = new QPainter;
    pm = new QPixmap(qMax(pm->width(), width()), qMax(pm->height(), height()));
    pm->fill(QColor(QString::fromStdString(getBCol())));
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

std::string ScopeView::getGridCol() {
    return gridcol;
}

std::string ScopeView::getBCol() {
    return bcol;
}

std::string ScopeView::getTextCol() {
    return textcol;
}

std::string ScopeView::getMarkCol() {
    return markcol;
}


bool ScopeView::update(const int value) {
    return true;
}

void ScopeView::setGridCol(const std::string& color) {
//ToFix    std::lock_guard<std::shared_mutex> lock(mutex);
    gridcol = color;
}

void ScopeView::setBCol(const std::string& color) {
//ToFix    std::lock_guard<std::shared_mutex> lock(mutex);
    bcol = color;
}

void ScopeView::setMarkCol(const std::string& color) {
//ToFix    std::lock_guard<std::shared_mutex> lock(mutex);
    markcol = color;
}

void ScopeView::setTextCol(const std::string& color) {
//ToFix    std::lock_guard<std::shared_mutex> lock(mutex);
    textcol = color;
}

// void paintEvent(QPaintEvent*)
// refreshes the screen from the buffered pixmap
void ScopeView::paintEvent( QPaintEvent *) {
    //    MSG(MSG_DEBUG, "bitBlt\n");
    std::lock_guard<std::mutex> lock(bltMutex);

    //bitBlt(this, 0, 0, pm);
    //ToDo: exchange with pm->drawPixmap
    //https://stackoverflow.com/questions/31068064/qt-alternative-to-bitblt-in-qt5-windows
}

void ScopeView::resizeEvent(QResizeEvent *) {
    {
        std::lock_guard<std::mutex> lock(bltMutex);
        pm->scaled(qMax(pm->width(), width()), qMax(pm->height(), height()));
        pm->fill(QColor(QString::fromStdString(bcol)));
    }

    // redraw after resizing...
    triggerRedrawScope();
}

// void triggerRedrawScope()
// triggers the redrawing by setting the cond. variable
void ScopeView::triggerRedrawScope() {
    // post event to redrawScope
    //ToFix lock()
        ;
    //    printf("scope (%s) redraw triggered\n", name.c_str()); fflush(stdout);
    QEvent *e = new QEvent(QEvent::User);
    QApplication::postEvent(this, e);
    //ToFix unlock();
}

// bool event ( QEvent *)
// reimplementation to catch the event to redraw the scope content
bool ScopeView::event(QEvent *e) {
    if(e->type() == QEvent::User) {
        std::lock_guard<std::mutex> lock(bltMutex);
        //TofFix redrawScope();
        return true;
    }

    // let qt process if it was not the "user" event
    return(QWidget::event(e));
}
