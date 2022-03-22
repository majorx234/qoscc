#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string>
#include <mutex>
#include <chrono>
#include <math.h>

#ifndef M_SQRT2
#define M_SQRT2 1.41421356237309504880
#endif

#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

#include "stringlist.h"
#include "dbuffer.h"

#include "deviceinterface.h"
#include "controllerclass.h"
#include "traceclass.h"

TraceClass::TraceClass(ControllerClass* _parentController) 
   : parentController(_parentController), 
     running(false), 
     color("#55ff55"), 
     dev_channel(0), 
     name("NoNameTrace"),
     xshift(0.0),
     yshift(0.0),
     parentname("unknown"),
     parent(NULL),
     buffersize(2048),
     bufferPos(0),
     PerfectBuffer(true)
{
}

TraceClass::~TraceClass() {
}

// dump()
// print out all internal stored values
void TraceClass::dump(FILE *file) {
  fprintf(file, "name = %s\n", name.c_str());
  fprintf(file, "channel = %d\n", dev_channel);
  fprintf(file, "parent = %s\n", parentname.c_str());
  fprintf(file, "color = %s\n", color.c_str());
  fprintf(file, "xshift = %f\n", xshift);
  fprintf(file, "yshift = %f\n", yshift);
  fprintf(file, "buffersize = %d\n", buffersize);
  fprintf(file, "perfectbuffer = %s\n", PerfectBuffer ? "true" : "false");
  fprintf(file, "fftwin = ");
  switch(buf_fft.getFftWinType()) {
  case dbuffer::winRect:
    fprintf(file, "rectangular\n");
     break;
  case dbuffer::winHanning:
    fprintf(file, "hanning\n");
    break;
  case dbuffer::winHamming:
    fprintf(file, "hamming\n");
    break;
  case dbuffer::winBlackmanHarrisA:
    fprintf(file, "blackman-harris-a\n");
    break;
  case dbuffer::winBlackmanHarrisB:
    fprintf(file, "blackman-harris-b\n");
    break;
  default:
    fprintf(file, "rectangular\n");
    break;
  }
  fflush(file);
}

std::string TraceClass::getName() {
    return name;
}

void TraceClass::setname(std::string newname) {
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    name = newname;
}

// int setParentName(string)
// check and set parent name
int TraceClass::setParentName(std::string newname) {
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    if(parentController->getDevice(newname)){
        parent = parentController->getDevice(newname);
        parentname = newname;
    }
    return 0;
}

// char *getParentName()
// return parents name
std::string TraceClass::getParentName() {
    return parentname;
}

// deviceClass *getParent()
// return pointer to parent device
// FIXME: this should not be used. 
// this is used exclusively by traceControl to find out the samplerate.
// maybe write a replacement thatfor in TraceClass.
DeviceInterface *TraceClass::getParent() {
    return parent;
}

// void recalc_stringrefs()
// re-read string values from its objects (parent / other trace names)
void TraceClass::recalc_stringrefs() {
    // re-read parents name:
    if(parent)
        parentname = parent->getName();
}

// int setBuffer(float*, int)
// set internal buffer to specified one
int TraceClass::setBuffer(const dbuffer& newbuf) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    
    // copy over buffer
    if(bufferPos > 0)
    {  
      // append if position is greater than zero
      // implement the += operator in dbuffer!
      //buf += newbuf;
      buf = buf + newbuf;
    }  
    else
    {  
      // replace if buffpos is zero
      buf = newbuf;
    }
    // adjust/limit buffersize to correct value!
    if(buf.getNumSamples() > buffersize)
    {  
      buf.setSize(buffersize);
    }
    // adjust bufferpos value, eventually wrap around (later)...
    bufferPos = buf.getNumSamples();

    // check if our buffer is "perfect"
    if(bufferPos >= buffersize) {
        bufferPos = 0;
        // copy actual buffer to perfect buffer
        buf_perfect = buf;
        buf_fft = buf;
        buf_fft.applyFft();
        parentController->notifyTraceUpdate(getParentName());
    }
    else 
    {  
      if(!PerfectBuffer){
        // redraw affected scope wins.
        parentController->notifyTraceUpdate(getParentName());
      }  
    }  
  }    
  return 0;
}

const dbuffer& TraceClass::getFBuf(){
    return buf_fft;
}

const dbuffer& TraceClass::getTBuf(){
    if(PerfectBuffer)
  return buf_perfect;
    return buf;
}

double TraceClass::getXShift() {
    return xshift;
}

double TraceClass::getYShift() {
    return yshift;
}

unsigned int TraceClass::getDevChannel() {
    return dev_channel;
}

std::string TraceClass::getColor() {
    return color;
}

unsigned int TraceClass::getBufferSize() {
    return buffersize;
}

void TraceClass::setYZoom(double newzoom) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    // ToDo: implement zoom
  }
}

void TraceClass::setXShift(double newshift) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    xshift = newshift;
  }
}

void TraceClass::setYShift(double newshift) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    yshift = newshift;
  }
}

void TraceClass::setDevChannel(unsigned int newchannel) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    dev_channel = newchannel;
  }
}

void TraceClass::setColor(const std::string & newcolor) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    color = newcolor;
  }
}

// void setBufferSize(unsigned int)
// resize the buffer...
int TraceClass::setBufferSize(unsigned int newsize) {
  if(!newsize)
    return -1;
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    buffersize = newsize;
  }
  return 0;
}

void TraceClass::setPerfectBuffer(bool n) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    PerfectBuffer = n;
  }
}

bool TraceClass::getPerfectBuffer() {
  return PerfectBuffer;
}

double TraceClass::getDc() {
  if(running)
    return buf_fft[(unsigned int)0];
  return 0;
}

void TraceClass::setFftWinType(dbuffer::fftWinType type) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    buf_fft.setFftWinType(type);
  }
}

dbuffer::fftWinType TraceClass::getFftWinType() {
  return buf_fft.getFftWinType();
}