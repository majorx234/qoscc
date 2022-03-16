#ifndef CONTROLLER_CLASS_H
#define CONTROLLER_CLASS_H

#include <string>
#include <mutex>
#include <thread>
#include <shared_mutex>

#include "deviceinterface.h"
#include "traceinterface.h"
#include "scopeinterface.h"

#include "globalclass.h"

class ControllerClass
{
public:
    ControllerClass();
    ~ControllerClass();
    ControllerClass(const ControllerClass&) = delete; // block copying!

    globalClass* getGlobal();

    int addDevice(DeviceInterface *);
    int removeDevice(DeviceInterface *);  

    int addTrace(TraceInterface *);
    int removeTrace(TraceInterface *);    

    DeviceInterface *getDevice(const std::string &);
    TraceInterface *getTrace(const std::string &);

    int addScope(ScopeInterface *);
    int removeScope(const std::string &);
    int removeScope(ScopeInterface *scope);
    ScopeInterface *getScope(const std::string &);

    unsigned int getTraceNum();

    void getDeviceList(stringlist *);
    void getTraceList(stringlist *);
    void getScopeList(stringlist *);

    int readconfig(const std::string &);
    int writeconfig(const std::string &);
    bool hasTrace(TraceInterface *);
    void quit();    

    void notifyTraceUpdate(const std::string&);

private:  
  //todo: should use vector
  unsigned int devnum;           // number of all devices
  DeviceInterface **devices;         // list of devices
  unsigned int tracenum;         // number of all traces (all channels in all devices)
  TraceInterface **traces;           // list of all traces
  unsigned int scopenum;         // number of scopes
  ScopeInterface **scopes;           // list of scopes
                                   //       
  std::thread controllerThread;
  std::thread scopeThread;
  std::shared_mutex rwMutex;

  int getiparam(char *);
  char *getsparam(char *);
  float getfparam(char *);

  unsigned long delay;  
  globalClass *global;
};

#endif // CONTROLLER_CLASS_H
