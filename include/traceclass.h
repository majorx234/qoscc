#ifndef TRACECLASS_H
#define TRACECLASS_H

#include <string>
#include <mutex>

#include "dbuffer.h"
#include "deviceinterface.h"
#include "traceinterface.h"
#include "controllerclass.h"

class TraceClass : public TraceInterface
{
public:
  TraceClass(ControllerClass*);
  ~TraceClass();
  TraceClass(const TraceClass&) = delete; // block copying

  void dump(FILE *file);

  std::string getName();
  void setname(std::string newname);
  int setParentName(std::string newname);
  std::string getParentName();
  DeviceInterface *getParent();

  void recalc_stringrefs();

  int setBuffer(const dbuffer&);
  const dbuffer& getFBuf();
  const dbuffer& getTBuf();

  double getXShift();
  double getYShift();
  unsigned int getDevChannel();
  std::string getColor();
  unsigned int getBufferSize();
  bool getPerfectBuffer();
  double getDc();
  dbuffer::fftWinType getFftWinType();

  void setYZoom(double);
  void setXShift(double);
  void setYShift(double);
  void setDevChannel(unsigned int channelInDev);
  void setColor(const std::string & newcolor);
  int setBufferSize(unsigned int);
  void setPerfectBuffer(bool enablePerfectBuffer);
  void setFftWinType(dbuffer::fftWinType);

private:
  ControllerClass* parentController;
  double xshift, yshift;        // shift of trace in div's

  unsigned int dev_channel;     // who am i in parent device ?

  std::string color;                 // color of trace
  double peak;                  // peak value
  double peakpos;               // peak position

  std::string name;                  // symbolic name of trace
  DeviceInterface *parent;          // pointer to parent
  std::string parentname;            // parents name
  bool running;                 // indicates if trace is in use
  std::shared_mutex rwMutex;

  dbuffer buf;
  dbuffer buf_fft;
  dbuffer buf_perfect;
    
  unsigned int buffersize;
  unsigned int bufferPos;
  bool PerfectBuffer;
};

#endif // TRACECLASS_H
