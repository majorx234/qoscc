#ifndef SCOPECLASS_H
#define SCOPECLASS_H

#include <stdio.h>
#include <string>
#include <mutex>
#include <vector>


#include "traceinterface.h"
#include "scopeinterface.h"
#include "controllerclass.h"

class ScopeClass : public ScopeInterface 
{
public:
  ScopeClass(ControllerClass*);
  ~ScopeClass();  
  ScopeClass(const ScopeClass&) = delete;

  std::string getName();
  int setName(const std::string &);

  double getfreq(int);
  double gettime(int);
  double getValue(int y);


  int addTrace(TraceInterface *);

  void dump(FILE *file);

  unsigned int getTraceNum();

  void setHeight(int);
  void setWidth(int);
  void setXPos(int);
  void setYPos(int);

  TraceInterface *getTrace(const char *);
  TraceInterface *getTrace(const std::string &);
  TraceInterface *getTrace(unsigned int);

  int removeTrace(TraceInterface *);
  int removeTrace(const std::string &);

  double calcx(int, int); 

  int setTriggerSource(std::string);
  void recalc_stringrefs();
  int setXYSourceX(std::string);
  int setXYSourceY(std::string);
  std::string getTriggerName();
  std::string getXYSourceXName();
  std::string getXYSourceYName();
  void getTraceList(stringlist *);
  bool triggerLocked();

  int getMode();
  double getSweep();
  std::string getGridCol();
  std::string getBCol();
  std::string getMarkCol();
  std::string getTextCol();
  unsigned int getHDivs();
  unsigned int getVDivs();
  int getTriggerEdge();
  double getTriggerLevel();

  bool getHold();
  bool getDispLog();
  unsigned int getDispFMin();
  unsigned int getDispFMax();
  std::string getInfoTraceName();
  bool getDispDb();
  double getDispDbRef();
  double getVDiv() {
    return vDiv;
  }
  double getDbMin() {
    return dbMin;
  }
  double getDbMax() {
    return dbMax;
  }
  /* FIXME needs qfont */
  std::string getFont(){
   // return font.toString();
    return font;
  }

  void setMode(int);
  void setSweep(double);
  void setGridCol(const std::string&);
  void setBCol(const std::string&);
  void setMarkCol(const std::string&);
  void setTextCol(const std::string&);
  void setHDivs(unsigned int);
  void setVDivs(unsigned int);
  void setTriggerEdge(int);
  void setTriggerLevel(double);
  void setHold(bool);
  void setDispLog(bool);
  void setDispFMin(unsigned int);
  void setDispFMax(unsigned int);
  void setInfoTraceName(const std::string&);
  void setDispDb(bool);
  void setDispDbRef(double);
  void setVDiv(double);
  void setDbMin(double);
  void setDbMax(double);
  void setFont(const std::string&);

  int storeValues(FILE *, double, double, int);
  double getMaxDspRate();
    
  void notifyTraceUpdate(const std::string&);  
  void registerObserver(const ScopeObserver* newObserver);
  void removeObserver(const ScopeObserver* delObserver);
  void notifyObserver();  
private:
  std::vector<std::shared_ptr<ScopeObserver>> _scopeObserver;
  ControllerClass* parentController;
  int maxy(int a, int b, int trace);
  void getpeak(int index);
  void initVars(); 

  int do_trigger(unsigned int);   

  int getFftPeakNear(unsigned int trace, int x, int y, double *freq, double *value);
  int getYtPeakNear(unsigned int trace, int x, int y, double *time, double *value);  
  unsigned int tracenum;
//    double **fbuf;
  dbuffer *buf;
//    unsigned int *buflen;
  std::string name;

  TraceInterface **traces;           // list of all classes for this scope

  TraceInterface *trigger_source;
  std::string trigger_source_name;
  TraceInterface *xysource_x;       // the sources traces (indices) for the xy-mode
  std::string xysource_x_name;       // and the corresponding names...
  TraceInterface *xysource_y;
  std::string xysource_y_name;

  int trigger_pos;
    
  std::string bcol, gridcol, markcol, textcol;
  unsigned int hdivs, vdivs;     // horizontal and vertial divisions  

  double sweep;                  // sweep (time to pass one div, in secs)  

  int mode;

  int trigger_edge;              // settings for trigger
  double trigger_level;
  bool trigger_locked;           // true if triggering was sucessful....

  bool hold;

  //std::recursive_mutex 
  std::shared_mutex mutex;

  bool dispLog;
  bool dispDb;
  double dispDbRef; 

  unsigned int mouse_x, mouse_y; // position of mouse pointer
  unsigned int dispFMin, dispFMax;

  std::string infoTraceName;

  double infoPeakFT;
  double infoPeakVal;
  double infoDC;

  double vDiv;
  double dbMin, dbMax;

  std::string font;
};
//todo better as enum
// scope display modes:
#define M_NONE      0x00
#define M_YT        0x01
#define M_XY        0x02
#define M_FFT       0x03

// trigger edge selection:
#define TE_NONE   0x00 /* = disabled */
#define TE_RISE   0x01
#define TE_FALL   0x02
#endif // SCOPECLASS_H
