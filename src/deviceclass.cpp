#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <cstring>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

/* 
#include "qoscc.h"
*/
#include "misc.h"

#include "traceinterface.h"
//#include "scopeinterface.h"
#include "deviceinterface.h"
#include "controllerclass.h"
#include "deviceclass.h"
// CONDITIONALY include backend headers!
#include "dspnone.h"
#ifdef USE_OSS
#include "dsposs.h"
#endif
#ifdef USE_ALSA
#include "dspalsa.h"
#endif
#ifdef USE_JACK
#include "dspjack.h"
#endif
#ifdef USE_MM
#include "dspmm.h"
#endif

DeviceClass::DeviceClass(ControllerClass* _parentController):  parentController(_parentController), dsp(new dspNONE), name("NoNameDevice"), type(PCM_NONE), buf(NULL), readerRunning(false), close(false)
{
//    buffersize = 2048;
}

DeviceClass::~DeviceClass() 
{
  stop();
  delete dsp;
  dsp = NULL;
  delete [] buf;
  buf= NULL;
}

void DeviceClass::readbuffers() 
{
  if(!readerRunning)
    return;
  //pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
  {  // read lock
    std::shared_lock<std::shared_mutex> lock(rwMutex);
    stringlist traces;
    parentController->getTraceList(&traces);

    // read the buffer from the dsp
    int samples = dsp->readdsp(buf);
    if(samples < 1) // bail out if no samples were read
    {
      std::this_thread::sleep_for(std::chrono::microseconds(10));
      return;
    }  
        // copy buffer to children...
    for(unsigned int ch = 0; ch < dsp->getChannels(); ch++) 
    {
    //        printf("device: buffer of ch %d fill is: %d\n", ch, buf[ch].getNumSamples());fflush(stdout);
      buf[ch].setSampleRate(dsp->getDspRate());
        for(unsigned int i = 0; i < traces.count(); i++) {
          TraceInterface *trace = parentController->getTrace(traces.getString(i));
          if(!trace) // skip if trace does not exist
            continue;
          // transfer the buffer, if traces parent && channel matches this one
          if(trace->getParentName() == name && trace->getDevChannel() == ch)
            trace->setBuffer(buf[ch]);
        }
    }
  }
  //pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
}

// dump(): print status / configuration info about this device
void DeviceClass::dump(FILE *file) {
    fprintf(file, "devicetype = ");
    switch(dsp->type()) {
    case PCM_NONE:
    default:
        fprintf(file, "none\n");
        break;

    case PCM_OSS:
        fprintf(file, "oss\n");
        break;

    case PCM_ALSA:
        fprintf(file, "alsa\n");
        break;

    case PCM_JACK:
        fprintf(file, "jack\n");
        break;

    case PCM_MM:
        fprintf(file, "mm\n");
        break;
    }
    fprintf(file, "name = %s\n", getName().c_str());
    fprintf(file, "file = %s\n", dsp->getDeviceName().c_str());
    fprintf(file, "channels = %d\n", dsp->getChannels());
    fprintf(file, "dsp_rate = %f\n", dsp->getDspRate());
    fprintf(file, "dsp_size = %d\n", dsp->getDspSize());
    fprintf(file, "buffersize = %d\n", dsp->getBufferSize());
    fprintf(file, "adjust = %f\n", dsp->getAdjust());
    fflush(file);
}  

std::string DeviceClass::getName() {
    return name;
}

// void setname(string)
// set devices symbloic name in device and also in its children
void DeviceClass::setname(std::string newname) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);

    name = newname;
  }
}


// void setdevname(string)
// set name of audio device
void DeviceClass::setdevname(std::string newname) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    dsp->setDeviceName(newname);
  }
}

std::string DeviceClass::getdevname() {
    return dsp->getDeviceName();
}

// int start()
// open device and start reading
int DeviceClass::start() {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    if(!buf) {
      //printf("%s::start() starting device\n", getname());
      //fflush(stdout);
      // alloc local data buffer
      buf = new dbuffer[dsp->getChannels()];
      if(!buf) {
          MSG(MSG_ERROR, "Could not allocate memory!\n");
          exit(1);
      }
    }

    // bring up dsp
    if(!dsp->isRunning()) {
      if(dsp->openDevice()) {
        return -1;
      }
    }

    // create readbuffer-thread
    if(!readerRunning) {
      deviceThread = new std::thread(&DeviceClass::deviceHandler, this); 
      //pthread_create(&deviceThread, NULL, DeviceClass::deviceHandler, (void*) this);
      readerRunning = true;
    }
  }
  return 0;
}

// int stop()
// close device and stop reading
int DeviceClass::stop() {
    int ret = 0;

    // kill readbuffer-thread...
    if(readerRunning) {
        close = true;
        deviceThread->join();
        readerRunning = false;
        MSG(MSG_DEBUG, "device reader thread canceled.\n");
    }
    { //write lock
      std::lock_guard<std::shared_mutex> lock(rwMutex);

      // close dsp
      if(dsp->isRunning()) {
        dsp->closeDevice();
      }

      // free buffer
      if(buf) {
        delete [] buf;
        buf = NULL;
      }
    }
    return ret;
}

// int setDspSize(int newDspSize)
// set and check new dsp size
int DeviceClass::setDspSize(int newDspSize) {
  int err = 0;
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    err = dsp->setDspSize(newDspSize);
  }
  return err;
}

// int getDspSize()
// read out dsp size
int DeviceClass::getDspSize() {
    return dsp->getDspSize();
}

// unsigned int getChNum()
// return number of available channels
unsigned int DeviceClass::getChNum() {
    return dsp->getChannels();
}

// get / set functions
void DeviceClass::setBuffersize(unsigned int size) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    // set buffersize for dsp (this is not used by all dsp types.)
    dsp->setBufferSize(size);
/*
    // no buffer alocated, so just take the size and store it...
    if(!fbuf) {
        buffersize = size;
        unlock();
        return;
    }
    // delete old buffer....
    for(unsigned int i = 0; i < dsp->getChannels(); i++)
        delete [] fbuf[i];
    delete [] fbuf;
    fbuf = NULL;

    // allocate new buffer:
    fbuf = new double *[dsp->getChannels()];
    if(!fbuf) {
        fprintf(stderr, "%s: unable to alloc memory!\n", name.c_str());
        exit(-1);
    }

    for(unsigned int i = 0; i < dsp->getChannels(); i++) {
        fbuf[i] = new double[size];
        if(!fbuf[i]) {
            fprintf(stderr, "%s: unable to alloc memory!\n", name.c_str());
            exit(-1);
        }
    }

    buffersize = size;
    */
  }
}

int DeviceClass::setChannels(unsigned int n) {
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    if(buf) {
      // delete old buffer....
      delete [] buf;
      // allocate new buffer:
      buf = new dbuffer[n];
      if(!buf) {
        fprintf(stderr, "%s: unable to alloc memory!\n", name.c_str());
        exit(-1);
      }
    }

    if(dsp->setChannels(n)) {
        return -1;
    }
  }  
  return 0;
}

int DeviceClass::setDspRate(double rate) {
  int ret;
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    ret = dsp->setDspRate(rate);
  }
  return ret;
}


unsigned int DeviceClass::getBuffersize() {
  return dsp->getBufferSize();
}

unsigned int DeviceClass::getChannels() {
  return dsp->getChannels();
}

double DeviceClass::getDspRate() {
  return dsp->getDspRate();
}

// void run()
// subsequently refreshes the buffer...
void DeviceClass::run() {
  while(!close) {
    // slow down this loop and give other lock()ers time to do their work.
    // its only 10msecs, but the effect is enormous!!
    // hope this is more or less portable (on slower machines....?)
    std::this_thread::sleep_for(std::chrono::microseconds(10));;
    readbuffers();
  }
}

// int setDeviceType(int)
// set type of this device
int DeviceClass::setDeviceType(int newtype) {
  // check if device is even different
  if(newtype == dsp->type())
  {  
    return 0;
  }
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);

    // possibly stop device
    if(dsp->isRunning())
        dsp->closeDevice();

    // create - copy - delete
    dspCommon* newdsp;
    switch(newtype) {
    case PCM_NONE:
    default:
        newdsp = new dspNONE;
        break;
#ifdef USE_OSS

    case PCM_OSS:
        newdsp = new dspOSS;
        break;
#endif
#ifdef USE_ALSA

    case PCM_ALSA:
        newdsp = new dspALSA;
        break;
#endif
#ifdef USE_JACK

    case PCM_JACK:
        newdsp = new dspJACK;
        break;
#endif
#ifdef USE_MM

    case PCM_MM:
        newdsp = new dspMM;
        break;
#endif

    }

    // assert memory...
    if(!newdsp) {
        fprintf(stderr, "DeviceClass::setDeviceType(): unable to get memory (%s)\n", strerror(errno));
        exit(-1);
    }
    // copy values from old dsp / from device
    newdsp->setChannels(dsp->getChannels());
    //    newdsp->setDeviceName(dsp->getDeviceName());
    newdsp->setDspRate(dsp->getDspRate());
    newdsp->setDspSize(dsp->getDspSize());
    newdsp->setAdjust(dsp->getAdjust());
    newdsp->setBufferSize(dsp->getBufferSize());

    // replace old by new...
    delete dsp;
    dsp = newdsp;

  }
  return 0;
}

// stringlist getDspChannelList()
// returns a list with all valid settings for the channel number
void DeviceClass::getDspChannelList(stringlist *liste) {
  dsp->getDspChannelList(liste);
}

// stringlist getDspRateList()
// returns a list with all valid settings for the sample rate
void DeviceClass::getDspRateList(stringlist *liste) {
  dsp->getDspRateList(liste);
}

// stringlist getDspSizeList()
// returns a list with all valid settings for the sample size
void DeviceClass::getDspSizeList(stringlist *liste) {
  dsp->getDspSizeList(liste);
}

int DeviceClass::getDspType() {
  return dsp->type();
}

double DeviceClass::getAdjust() {
  return dsp->getAdjust();
}

int DeviceClass::setAdjust(double n) {
  int ret = 0;
  { //write lock
    std::lock_guard<std::shared_mutex> lock(rwMutex);
    ret = dsp->setAdjust(n);
  }
  return ret;
}

bool DeviceClass::isRunning() {
  return dsp->isRunning();
}

void DeviceClass::getDspNameList(stringlist *liste) {
  return dsp->getDspNameList(liste);
}

void *DeviceClass::deviceHandler() {
  //pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);
  run();
  printf("deviceHandler() exits\n");
  fflush(stdout);
  return NULL;
}
