#ifndef TRACEINTERFACE_H
#define TRACEINTERFACE_H

#include <string>
#include "dbuffer.h"
#include "deviceinterface.h"

class TraceInterface
{
public:
  // pure virtual function providing interface framework.
  virtual void dump(FILE *file) = 0;

  virtual std::string getName() = 0;
  virtual void setname(std::string) = 0;

  virtual int setParentName(std::string) = 0;
  virtual std::string getParentName() = 0;
  virtual DeviceInterface *getParent() = 0;

  virtual void recalc_stringrefs() = 0; 

  virtual int setBuffer(const dbuffer&) = 0;
  virtual const dbuffer& getFBuf() = 0;
  virtual const dbuffer& getTBuf() = 0;

  virtual double getXShift() = 0;
  virtual double getYShift() = 0;
  virtual unsigned int getDevChannel() = 0;  
  virtual std::string getColor() = 0;
  virtual unsigned int getBufferSize() = 0;
  virtual bool getPerfectBuffer() = 0;
  virtual double getDc() = 0;
  virtual dbuffer::fftWinType getFftWinType() = 0;  

  virtual void setYZoom(double) = 0;
  virtual void setXShift(double) = 0;
  virtual void setYShift(double) = 0;
  virtual void setDevChannel(unsigned int) = 0;
  virtual void setColor(const std::string &) = 0;
  virtual int setBufferSize(unsigned int) = 0;
  virtual void setPerfectBuffer(bool) = 0;
  virtual void setFftWinType(dbuffer::fftWinType) = 0;
};    
#endif // TRACEINTERFACE_H
