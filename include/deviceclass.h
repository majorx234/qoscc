#ifndef DEVICECLASS_H
#define DEVICECLASS_H

#include <string>
#include <mutex>
#include <atomic>

#include "dbuffer.h"
#include "stringlist.h"
#include "dspcommon.h"
#include "deviceinterface.h"
#include "controllerclass.h"

class DeviceClass : public DeviceInterface
{
public:
    DeviceClass(ControllerClass* _parentController);
    ~DeviceClass();

    void dump(FILE *file);

    int start();   // open device and start reading
    int stop();    // close device and stop reading
    int setDspSize(int);
    int getDspSize();

    std::string getName();
    void setname(std::string);
    std::string getdevname();
    void setdevname(std::string);

    unsigned int getChNum();

    void setBuffersize(unsigned int);
    int setChannels(unsigned int);
    int setDspRate(double);
    double getAdjust();

    unsigned int getBuffersize();
    unsigned int getChannels();
    double getDspRate();
    int getDspType();

    void getDspChannelList(stringlist *);
    void getDspRateList(stringlist *);
    void getDspSizeList(stringlist *);
    void getDspNameList(stringlist *);

    void run();

    int setDeviceType(int);
    int setAdjust(double);

    bool isRunning();
private:
    ControllerClass* parentController;
    dspCommon *dsp;
    int type;

//    unsigned int buffersize;
//    double **fbuf;                   // buffers for all channels
    dbuffer *buf;

    std::string name;                  // symbolic name of device

    void readbuffers();

    std::shared_mutex rwMutex;
    std::thread* deviceThread;

    bool readerRunning;
    std::atomic<bool> close;

    // FIXME: we should get that running with a friend?!?
    void *deviceHandler(); // thread handling function...

};

#endif // DEVICECLASS_H

