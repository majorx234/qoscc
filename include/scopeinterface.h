#ifndef SCOPEINTERFACE_H
#define SCOPEINTERFACE_H

#include <string>
#include <traceinterface.h>
#include "scopeobserver.h"

class ScopeInterface
{
public:
  // pure virtual function providing interface framework.
  virtual int setName(const std::string &) = 0;
  virtual std::string getName() = 0;

  virtual int addTrace(TraceInterface *) = 0;
  virtual void dump(FILE *file) = 0;

  virtual int setTriggerSource(std::string) = 0;
  virtual int setXYSourceX(std::string) = 0;
  virtual int setXYSourceY(std::string) = 0;

  virtual void setHeight(int) = 0;
  virtual void setWidth(int) = 0;
  virtual void setXPos(int) = 0;
  virtual void setYPos(int) = 0;

  virtual TraceInterface *getTrace(const char *) = 0;
  virtual TraceInterface *getTrace(const std::string &) = 0;
  virtual TraceInterface *getTrace(unsigned int) = 0;

  virtual int removeTrace(TraceInterface *) = 0;
  virtual int removeTrace(const std::string &) = 0;

  virtual void recalc_stringrefs() = 0;
  virtual std::string getTriggerName() = 0;
  virtual std::string getXYSourceXName() = 0;
  virtual std::string getXYSourceYName() = 0;
  virtual void getTraceList(stringlist *) = 0;
  virtual bool triggerLocked() = 0;

  virtual int getMode() = 0;
  virtual double getSweep() = 0;
  virtual std::string getGridCol() = 0;
  virtual std::string getBCol() = 0;
  virtual std::string getMarkCol() = 0;
  virtual std::string getTextCol() = 0;
  virtual unsigned int getHDivs() = 0;
  virtual unsigned int getVDivs() = 0;
  virtual int getTriggerEdge() = 0;
  virtual double getTriggerLevel() = 0;

  virtual bool getHold() = 0;
  virtual bool getDispLog() = 0;
  virtual unsigned int getDispFMin() = 0;
  virtual unsigned int getDispFMax() = 0;
  virtual std::string getInfoTraceName() = 0;
  virtual bool getDispDb() = 0;
  virtual double getDispDbRef() = 0;
  virtual double getVDiv() = 0; 
  virtual double getDbMin() = 0; 
  virtual double getDbMax() = 0; 
  virtual std::string getFont() = 0;

  virtual void setMode(int) = 0;
  virtual void setSweep(double) = 0;
  virtual void setGridCol(const std::string&) = 0;
  virtual void setBCol(const std::string&) = 0;
  virtual void setMarkCol(const std::string&) = 0;
  virtual void setTextCol(const std::string&) = 0;
  virtual void setHDivs(unsigned int) = 0;
  virtual void setVDivs(unsigned int) = 0;
  virtual void setTriggerEdge(int) = 0;
  virtual void setTriggerLevel(double) = 0;
  virtual void setHold(bool) = 0;
  virtual void setDispLog(bool) = 0;
  virtual void setDispFMin(unsigned int) = 0;
  virtual void setDispFMax(unsigned int) = 0;
  virtual void setInfoTraceName(const std::string&) = 0;
  virtual void setDispDb(bool) = 0;
  virtual void setDispDbRef(double) = 0;
  virtual void setVDiv(double) = 0;
  virtual void setDbMin(double) = 0;
  virtual void setDbMax(double) = 0;
  virtual void setFont(const std::string&) = 0;

  virtual int storeValues(FILE *, double, double, int) = 0;
  virtual double getMaxDspRate() = 0; 

  virtual void notifyTraceUpdate(const std::string&) = 0;

//subject functionality for Observer Pattern
  virtual void registerObserver(const ScopeObserver* newObserver) = 0;
  virtual void removeObserver(const ScopeObserver* delObserver) = 0;
  virtual void notifyObserver() = 0;  
};

#endif // SCOPEINTERFACE_H
