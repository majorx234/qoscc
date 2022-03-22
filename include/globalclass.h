//
// C++ Interface: globalClass.h
//
// Description:
//
//
// Author: Sven Queisser, <tincan@svenqueisser.de, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef GLOBALCLASS_H
#define GLOBALCLASS_H

#include <string>
#include <thread>
#include <shared_mutex>

#include "stringlist.h"
#include "deviceinterface.h"
#include "traceinterface.h"
#include "scopeinterface.h"

class globalClass {
    typedef std::lock_guard<std::shared_mutex> writeLock;
    typedef std::shared_lock<std::shared_mutex> readLock;
public:
    globalClass();
    ~globalClass();
    
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

    unsigned int devnum;           // number of all devices
    DeviceInterface **devices;         // list of devices
    unsigned int tracenum;         // number of all traces (all channels in all devices)
    TraceInterface **traces;           // list of all traces    
    unsigned int scopenum;         // number of scopes
    ScopeInterface **scopes;           // list of scopes

private:
    globalClass(const globalClass&){}; // block copying!

    std::thread scope_thread;        // thread handler for scope
    std::shared_mutex rw_mtx;     // shared mutex or read/write

    int getiparam(char *);
    char *getsparam(char *);
    float getfparam(char *);

    unsigned long delay;
};

//extern globalClass *global; 

#endif

