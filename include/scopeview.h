#ifndef SCOPEVIEW_H
#define SCOPEVIEW_H

#include <mutex>

#include <QWidget>
#include <QFont>
#include <QEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QPainter>
#include <QPixmap>

#include "scopeinterface.h"

class ScopeView : public QWidget, public ScopeObserver
{
  Q_OBJECT
public:  
  ScopeView(ScopeInterface* _modelScope, QWidget *parent = 0, const char *name = 0);
  ~ScopeView();
  ScopeView(const ScopeView&) = delete;

  void triggerRedrawScope();
  void redrawScope();
  unsigned int getMouseX();
  unsigned int getMouseY();
  void setMouseX(unsigned int);
  void setMouseY(unsigned int);  
  int getScreenPos(double);

  bool update(const int value) override;
private:
  ScopeInterface* modelScope;
  
  void mousePressEvent(QMouseEvent *e);
  void resizeEvent(QResizeEvent *e);
  void paintEvent(QPaintEvent *e);
  bool event(QEvent *e); 

  inline void drawmark(int x, int y);
  inline void draw_triglvl(int trace);
  inline void grid();
  inline void grid_fft();
  inline double calcx(int buffpos, int trace);  
  QPainter *painter;
  QPixmap *pm; 
  QFont font;    

  std::mutex bltMutex;

  // position of mouse pointer
  unsigned int mouse_x;
  unsigned int mouse_y;
};

#endif // SCOPEVIEW_H
