#ifndef DEVICEINTERFACE_H
#define DEVICEINTERFACE_H

#include <string>
#include "stringlist.h"
#include "dbuffer.h"
// Base class
class DeviceInterface  {

public:
   virtual void dump(FILE *file) = 0;

   virtual int start() = 0;   // open device and start reading
   virtual int stop() = 0;    // close device and stop reading
   virtual int setDspSize(int) = 0;
   virtual int getDspSize() = 0;

   virtual std::string getName() = 0;
   virtual void setname(std::string) = 0;
   virtual std::string getdevname() = 0;
   virtual void setdevname(std::string) = 0;

   virtual unsigned int getChNum() = 0;

   virtual void setBuffersize(unsigned int) = 0;
   virtual int setChannels(unsigned int) = 0;
   virtual int setDspRate(double) = 0;
   virtual double getAdjust() = 0;

   virtual unsigned int getBuffersize() = 0;
   virtual unsigned int getChannels() = 0;
   virtual double getDspRate() = 0;
   virtual int getDspType() = 0;

   virtual void getDspChannelList(stringlist *) = 0;
   virtual void getDspRateList(stringlist *) = 0;
   virtual void getDspSizeList(stringlist *) = 0;
   virtual void getDspNameList(stringlist *) = 0;

   virtual void run() = 0;

   virtual int setDeviceType(int) = 0;
   virtual int setAdjust(double) = 0;

   virtual bool isRunning() = 0;

};

#endif // DEVICEINTERFACE_H

