//
// C++ Implementation: globalClass.cpp
//
// Description:
//
//
// Author: Sven Queisser, <tincan@svenqueisser.de, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "globalclass.h"

#include <stdio.h>
#include <pthread.h>
#include <fftw3.h>
#include <errno.h>
#include <unistd.h>

#include "deviceClass.h"
#include "scopeClass.h"
#include "traceClass.h"
#include "stringlist.h"
#include "dspcommon.h"
#include "dbuffer.h"

globalClass::globalClass() {
    devnum = 0;
    tracenum = 0;
    scopenum = 0;
    delay = 40000; // that would be 25Hz at absolutely no load. good start value.

    // init rw-lock
    pthread_rwlock_init(&rwlock, NULL);
}

globalClass::~globalClass() {
    delete [] traces;
    delete [] devices;
    delete [] scopes;
    // this produces a sigsegv, so leave it!
    //    fftw_cleanup();
    //    int err = pthread_mutex_destroy(&mutex);
    int err = pthread_rwlock_destroy(&rwlock);
    if(err) {
        fprintf(stderr, "deviceClass::~deviceClass(): Mutex destroy failure (%s)\n", strerror(err));
        fflush(stderr);
    }
}

// addDevice
// ads specified device to array
int globalClass::addDevice(deviceClass *newdev) {
    // bail out if device exists already
    if(getDevice(newdev->getName()))
        return -1;
    writeLock();
    //    printf("globalClass::addDevice(%s): now have %d devices\n", newdev->getname(), devnum+1);
    //    fflush(stdout);
    deviceClass **newlist;  // create new, larger array
    newlist = new deviceClass*[devnum + 1];
    // copy over old devices
    for(unsigned int i = 0; i < devnum; i++)
        newlist[i] = devices[i];
    // add new device at end
    newlist[devnum] = newdev;

    // delete old array if it existed
    if(devnum)
        delete [] devices;
    // copy back new list
    devices = newlist;
    // increase number of devices
    devnum ++;
    unlock();
    return 0;
}

// removeDevice
// removes specified class from array and clears memory
int globalClass::removeDevice(deviceClass *devptr) {
    // bail out if we got a zero pointer
    if(!devptr || devnum < 1)
        return -1;
    writeLock();
    int removed = 0; // number of traces we removed ....
    for(unsigned int i = 0; i < devnum; i++) {
        if(devptr == devices[i]) {
            removed ++;
            //            delete devices[i];
        } else
            devices[i - removed] = devices[i];
    }

    devnum -= removed;

    unlock();
    return 0;
}

// deviceClass *getDevice(char*)
// looks for device with name and returns its pointer
// else it returns the NULL-pointer
deviceClass *globalClass::getDevice(const std::string &name) {
    readLock();
    for(unsigned int i = 0; i < devnum; i++)
        if(name == devices[i]->getName()) {
            unlock();
            return devices[i];
        }
    unlock();
    return NULL;
}

// int addTrace(traceClass *)
// adds new trace to global list
int globalClass::addTrace(traceClass *newtrace) {
    // bail out if trace name exists already
    if(getTrace(newtrace->getName()))
        return -1;
    writeLock();
    //    printf("globalClass::addTrace(%s): now have %d traces\n", newtrace->getname(), tracenum+1);
    //    fflush(stdout);
    traceClass **newlist;  // create new, larger array
    newlist = new traceClass*[tracenum + 1];
    // copy over old traces
    for(unsigned int i = 0; i < tracenum; i++)
        newlist[i] = traces[i];
    // add new device at end
    newlist[tracenum] = newtrace;

    // delete old array if it existed
    if(tracenum)
        delete [] traces;
    // copy back new list
    traces = newlist;
    // increase number of devices
    tracenum ++;
    unlock();
    return 0;
}

// int removeTrace(traceClass*)
// removes trace identified by pointer from class
int globalClass::removeTrace(traceClass *deltrace) {
    // bail out if we got a zero pointer
    if(!deltrace || tracenum < 1)
        return -1;
    writeLock();
    int removed = 0; // number of traces we removed ....
    for(unsigned int i = 0; i < tracenum; i++) {
        if(deltrace == traces[i]) {
            removed ++;
        } else
            traces[i - removed] = traces[i];
    }

    tracenum -= removed;

    unlock();
    return 0;
}

// traceClass *getTrace(char*)
// looks for trace with name and returns its pointer
// else it returns the NULL-pointer
traceClass *globalClass::getTrace(const std::string &name) {
    readLock();
    for(unsigned int i = 0; i < tracenum; i++)
        if(name == traces[i]->getName()) {
            unlock();
            return traces[i];
        }
    unlock();
    return NULL;
}

// int getTraceNum()
// returns number of existing traces
unsigned int globalClass::getTraceNum() {
    return tracenum;
}

// void lock()
// set mutex lock
void globalClass::writeLock() {
    //  pthread_mutex_lock(&mutex);
    pthread_rwlock_wrlock(&rwlock);
}

// void lock()
// set mutex lock
void globalClass::readLock() {
    //    pthread_mutex_lock(&mutex);
    pthread_rwlock_rdlock(&rwlock);
}

// void unlock()
// release mutex lock()
void globalClass::unlock() {
    //    pthread_mutex_unlock(&mutex);
    pthread_rwlock_unlock(&rwlock);
}

int globalClass::addScope(scopeClass *newscope) {
    // bail out if scope name exists already
    if(getScope(newscope->getName()))
        return -1;
    writeLock();
    //    printf("globalClass::addScope(%s): now have %d scopes\n", newscope->getName().c_str(), scopenum+1);
    //    fflush(stdout);
    scopeClass **newlist;  // create new, larger array
    newlist = new scopeClass*[scopenum + 1];
    // copy over old scopes
    for(unsigned int i = 0; i < scopenum; i++)
        newlist[i] = scopes[i];
    // add new scope at end
    newlist[scopenum] = newscope;

    // delete old array if it existed
    if(scopenum)
        delete [] scopes;
    // copy back new list
    scopes = newlist;
    // increase number of devices
    scopenum ++;
    unlock();
    return 0;
}

// int removeScope(scopeClass*)
// removes Scope of given name
int globalClass::removeScope(scopeClass *scope) {
    // bail out if we got a zero pointer
    if(!scope || scopenum < 1)
        return -1;

    writeLock();
    int removed = 0; // number of traces we removed ....
    for(unsigned int i = 0; i < scopenum; i++) {
        if(scope == scopes[i]) {
            removed ++;
            delete scopes[i];
        } else
            scopes[i - removed] = scopes[i];
    }

    scopenum -= removed;

    unlock();
    return 0;
}

// int removeScope(string)
// removes Scope of given name
int globalClass::removeScope(const std::string &name) {
    return removeScope(getScope(name));
}

// scope *getScope(string)
// returns pointer so scope of given name
// returns NULL if none matches
scopeClass *globalClass::getScope(const std::string &name) {
    readLock();
    for(unsigned int i = 0; i < scopenum; i++)
        if(name == scopes[i]->getName()) {
            unlock();
            return scopes[i];
        }
    unlock();
    return NULL;
}

// stringlist getDeviceList()
// return list of devices
void globalClass::getDeviceList(stringlist *liste) {
    readLock();
    // compose list...
    for(unsigned int i = 0; i < devnum; i++)
        liste->addString(devices[i]->getName());
    unlock();
}

// stringlist getTraceList()
// return list of Traces
void globalClass::getTraceList(stringlist *liste) {
    readLock();
    // compose list...
    for(unsigned int i = 0; i < tracenum; i++)
        liste->addString(traces[i]->getName());
    unlock();
}

// stringlist getScopeList()
// return list of scope wins
void globalClass::getScopeList(stringlist *liste) {
    readLock();
    // compose list...
    for(unsigned int i = 0; i < scopenum; i++)
        liste->addString(scopes[i]->getName());
    unlock();
}

int globalClass::readconfig(const std::string &file) {
    FILE *conffile;
    char *confline = NULL;
    size_t conflen = 0;
    int fstat;

    enum conftypes{C_NONE = 0, C_DEV, C_TRACE, C_SCOPE};
    conftypes conftype = C_NONE;

    char thelp[81];

    conffile = fopen(file.c_str(), "r");
    if(!conffile) {
        fprintf(stderr, "\nreadconfig(): could not open config file %s! (%s)\n", file.c_str(), strerror(errno));
        return 1;
    }

    traceClass *currtrace; // pointers for current device / trace / scope
    deviceClass *currdev;
    scopeClass *currscope;

    // read configfile, transfer parameters to structs
    do {
        fstat = getline(&confline, &conflen, conffile);
        if(fstat > 1 && strncmp("#", confline, 1)) {   /* 0 would mean EOF, 1 would mean an empty line, "#" is a comment */
            confline[fstat - 1] = 0;      /* close string properly by replacing LF by zero */
            //            printf("%s\n", confline);
            //            fflush(stdout);
            if(!strncmp("[dev]", confline, 5)) {
                currdev = new deviceClass;
                if(!currdev) {
                    fprintf(stderr, "readconfig: could not allocate memory (%s)\n", strerror(errno));
                    exit(-1);
                }
                global->addDevice(currdev);
                conftype = C_DEV;
            } else if(!strncmp("[trace]", confline, 7)) {
                currtrace = new traceClass;
                if(!currtrace) {
                    fprintf(stderr, "readconfig: could not allocate memory (%s)\n", strerror(errno));
                    exit(-1);
                }
                global->addTrace(currtrace);
                conftype = C_TRACE;
            } else if(!strncmp("[scope]", confline, 8)) {
                currscope = new scopeClass;
                if(!currscope) {
                    fprintf(stderr, "readconfig: could not allocate memory (%s)\n", strerror(errno));
                    exit(-1);
                }
                global->addScope(currscope);
                conftype = C_SCOPE;
            } else if(conftype == C_DEV) {
                if(!strncmp("name = ", confline, 7))
                    currdev->setname(getsparam(confline));
                else if(!strncmp("file = ", confline, 7))
                    currdev->setdevname(getsparam(confline));
                else if(!strncmp("channels = ", confline, 11))
                    currdev->setChannels(getiparam(confline));
                else if(!strncmp("dsp_rate = ", confline, 11))
                    currdev->setDspRate(getfparam(confline));
                else if(!strncmp("devicetype = ", confline, 13)) {
                    if(!strncmp("none", getsparam(confline), 4))
                        currdev->setDeviceType(PCM_NONE);
                    else if(!strncmp("oss", getsparam(confline), 3))
                        currdev->setDeviceType(PCM_OSS);
                    else if(!strncmp("alsa", getsparam(confline), 4))
                        currdev->setDeviceType(PCM_ALSA);
                    else if(!strncmp("jack", getsparam(confline), 4))
                        currdev->setDeviceType(PCM_JACK);
                    else if(!strncmp("mm", getsparam(confline), 4))
                        currdev->setDeviceType(PCM_MM);
                    else
                        fprintf(stderr, "readconfig(): no such option for devicetype for %s\n", currdev->getName().c_str());
                } else if(!strncmp("dsp_size = ", confline, 11)) {
                    if(currdev->setDspSize(getiparam(confline)))
                        fprintf(stderr, "readconfig(): invalid dsp_size for %s\n", currdev->getName().c_str());
                } else if(!strncmp("buffersize = ", confline, 13))
                    currdev->setBuffersize(getiparam(confline));
                else if(!strncmp("adjust = ", confline, 9))
                    currdev->setAdjust(getfparam(confline));
                else
                    printf("no such option in [dev]: %s\n", confline);

            } else if(conftype == C_TRACE) {
                if(!strncmp("name = ", confline, 7))
                    currtrace->setname(getsparam(confline));
                else if(!strncmp("channel = ", confline, 10))
                    currtrace->setDevChannel(getiparam(confline));
                else if(!strncmp("color = ", confline, 8))
                    currtrace->setColor(getsparam(confline));
                else if(!strncmp("xshift = ", confline, 9))
                    currtrace->setXShift(getfparam(confline));
                else if(!strncmp("yshift = ", confline, 9))
                    currtrace->setYShift(getfparam(confline));
                else if(!strncmp("buffersize = ", confline, 13))
                    currtrace->setBufferSize(getiparam(confline));
                else if(!strncmp("perfectbuffer = ", confline, 16)) {
                    if(!strncmp("true", getsparam(confline), 4))
                        currtrace->setPerfectBuffer(true);
                    else if(!strncmp("false", getsparam(confline), 5))
                        currtrace->setPerfectBuffer(false);
                    else
                        fprintf(stderr, "readconfig(): no such option for perfectbuffer: %s\n", getsparam(confline));
                } else if(!strncmp("parent = ", confline, 9)) {
                    if(currtrace->setParentName(getsparam(confline))) {
                        fprintf(stderr, "readconfig(): Unable to set parent name %s for trace %s\n", getsparam(confline), currtrace->getName().c_str());
                        fflush(stderr);
                    }
                } else if(!strncmp("fftwin = ", confline, 9)) {
                    if(!strncmp("rectangular", getsparam(confline), 11))
                        currtrace->setFftWinType(dbuffer::winRect);
                    else if(!strncmp("hanning", getsparam(confline), 7))
                        currtrace->setFftWinType(dbuffer::winHanning);
                    else if(!strncmp("hamming", getsparam(confline), 7))
                        currtrace->setFftWinType(dbuffer::winHamming);
                    else if(!strncmp("blackman-harris-a", getsparam(confline), 17))
                        currtrace->setFftWinType(dbuffer::winBlackmanHarrisA);
                    else if(!strncmp("blackman-harris-b", getsparam(confline), 17))
                        currtrace->setFftWinType(dbuffer::winBlackmanHarrisB);
                    else
                        printf("no such option for fftwin: %s\n", getsparam(confline));
                }
                else
                    printf("no such option in [trace]: %s\n", confline);

            } else if(conftype == C_SCOPE) {
                if(!strncmp("name = ", confline, 7))
                    currscope->setName(getsparam(confline));
                else if(!strncmp("mode = ", confline, 7)) {
                    strcpy(thelp, getsparam(confline));
                    if(!strncmp(thelp, "yt", 2))
                        currscope->setMode(M_YT);
                    else if(!strncmp(thelp, "xy", 2))
                        currscope->setMode(M_XY);
                    else if(!strncmp(thelp, "fft", 3))
                        currscope->setMode(M_FFT);
                    else
                        printf("no such parameter for mode in [scope]: %s\n", thelp);
                } else if(!strncmp("sweep = ", confline, 8))
                    currscope->setSweep(getfparam(confline));
                else if(!strncmp("bcol = ", confline, 7))
                    currscope->setBCol(getsparam(confline));
                else if(!strncmp("gridcol = ", confline, 10))
                    currscope->setGridCol(getsparam(confline));
                else if(!strncmp("markcol = ", confline, 10))
                    currscope->setMarkCol(getsparam(confline));
                else if(!strncmp("textcol = ", confline, 10))
                    currscope->setTextCol(getsparam(confline));
                else if(!strncmp("font = ", confline, 7))
                    currscope->setFont(getsparam(confline));
                else if(!strncmp("width = ", confline, 8))
                    currscope->setWidth(getiparam(confline));
                else if(!strncmp("height = ", confline, 9))
                    currscope->setHeight(getiparam(confline));
                else if(!strncmp("hdivs = ", confline, 8))
                    currscope->setHDivs(getiparam(confline));
                else if(!strncmp("vdivs = ", confline, 8))
                    currscope->setVDivs(getiparam(confline));
                else if(!strncmp("trace = ", confline, 8)) {
                    if(currscope->addTrace(global->getTrace(getsparam(confline))))
                        fprintf(stderr, "readconfig(): cannot add Trace %s\n", getsparam(confline));
                } else if(!strncmp("trigger_source = ", confline, 17))
                    currscope->setTriggerSource(getsparam(confline));
                else if(!strncmp("trigger_level = ", confline, 16))
                    currscope->setTriggerLevel(getfparam(confline));
                else if(!strncmp("trigger_edge = ", confline, 15)) {
                    strcpy(thelp, getsparam(confline));
                    if(!strncmp(thelp, "fall", 4))
                        currscope->setTriggerEdge(TE_FALL);
                    else if(!strncmp(thelp, "rise", 4))
                        currscope->setTriggerEdge(TE_RISE);
                    else if(!strncmp(thelp, "none", 4))
                        currscope->setTriggerEdge(TE_NONE);
                    else
                        printf("no such parameter for trigger_edge in [scope]: %s\n", thelp);
                } else if(!strncmp("xysource_x = ", confline, 13))
                    currscope->setXYSourceX(getsparam(confline));
                else if(!strncmp("xysource_y = ", confline, 13))
                    currscope->setXYSourceY(getsparam(confline));
                else if(!strncmp("disp_f_min = ", confline, 13))
                    currscope->setDispFMin(getiparam(confline));
                else if(!strncmp("disp_f_max = ", confline, 13))
                    currscope->setDispFMax(getiparam(confline));
                else if(!strncmp("infotrace = ", confline, 12))
                    currscope->setInfoTraceName(getsparam(confline));
                else if(!strncmp("disp_log = ", confline, 11)) {
                    if(!strcmp(getsparam(confline), "true"))
                        currscope->setDispLog(true);
                    else
                        currscope->setDispLog(false);
                } else if(!strncmp("disp_db = ", confline, 10)) {
                    if(!strcmp(getsparam(confline), "true"))
                        currscope->setDispDb(true);
                    else
                        currscope->setDispDb(false);
                } else if(!strncmp("disp_db_ref = ", confline, 14))
                    currscope->setDispDbRef(getfparam(confline));
                else if(!strncmp("v_div = ", confline, 8))
                    currscope->setVDiv(getfparam(confline));
                else if(!strncmp("db_min = ", confline, 9))
                    currscope->setDbMin(getfparam(confline));
                else if(!strncmp("db_max = ", confline, 9))
                    currscope->setDbMax(getfparam(confline));
                else if(!strncmp("x = ", confline, 4))
                    currscope->setXPos(getiparam(confline));
                else if(!strncmp("y = ", confline, 4))
                    currscope->setYPos(getiparam(confline));
                else
                    printf("no such option in [scope]: %s\n", confline);
            }
        }
    } while(fstat != -1);

    fclose(conffile);

    if(conflen > 0)  // there's no mem allocated else
        free(confline);

    // print out all config infos
    /*
    for(unsigned int i = 0; i < global->devnum; i++)
        global->devices[i]->dump(stdout);
    for(unsigned int i = 0; i < global->tracenum; i++)
        global->traces[i]->dump(stdout);
    for(unsigned int i = 0; i < global->scopenum; i++)
        global->scopes[i]->dump(stdout);
    */

    return 0;
}

int globalClass::getiparam(char *text) {
    unsigned int start;
    for(start = 0; strncmp("= ", text + start, 2) && start < strlen(text); start++)
        ;
    start += 2;
    return atoi(text + start);
}

char *globalClass::getsparam(char *text) {
    int start;

    for(start = 0; strncmp("= ", text + start, 2); start++)
        ;
    start += 2;

    return text + start;
}

float globalClass::getfparam(char *text) {
    unsigned int start;
    for(start = 0; strncmp("= ", text + start, 2) && start < strlen(text); start++)
        ;
    start += 2;
    return atof(text + start);
}

// writeconfig()
// writes configuration data to file
int globalClass::writeconfig(const std::string &filename) {
    unsigned int i;
    FILE *file;

    file = fopen(filename.c_str(), "w");
    if(!file) {
        fprintf(stderr, "writeconfig(): Could not open file %s (%s)\n", filename.c_str(), strerror(errno));
        return -1;
    }

    // print out devices
    for(i = 0; i < global->devnum; i++) {
        fprintf(file, "[dev]\n");
        global->devices[i]->dump(file);
        fprintf(file, "\n");
    }

    // print out traces
    for(i = 0; i < global->tracenum; i++) {
        fprintf(file, "[trace]\n");
        global->traces[i]->dump(file);
        fprintf(file, "\n");
    }

    // print out scope sections
    for(i = 0; i < global->scopenum; i++) {
        fprintf(file, "[scope]\n");
        global->scopes[i]->dump(file);
        fprintf(file, "\n");
    }
    return 0;
}

bool globalClass::hasTrace(traceClass *trace) {
    readLock();
    for(unsigned int i = 0; i < tracenum; i++) {
        if(traces[i] == trace) {
            unlock();
            return true;
        }
    }
    unlock();
    return false;
}

// void quit
void globalClass::quit() {
    writeLock();
    // kill scope-redrawer thread
    //    pthread_cancel(scope_thread);
    //    pthread_join(scope_thread, NULL);

    // destroy traces, scopes and devices
    for(unsigned int i = 0; i < scopenum; i++)
        delete scopes[i];
    scopenum = 0;
    unlock();

    // only set read lock while stopping traces and devices
    // they also need to read from us, so we cannot
    // set a write lock
    readLock();
    for(unsigned int i = 0; i < devnum; i++)
        devices[i]->stop();
    unlock();

    writeLock();
    for(unsigned int i = 0; i < devnum; i++)
        delete devices[i];
    devnum = 0;

    for(unsigned int i = 0; i < tracenum; i++)
        delete traces[i];
    tracenum = 0;

    unlock();
}

void globalClass::notifyTraceUpdate(const std::string& devicename){
    readLock();
    for(unsigned int i = 0; i < scopenum; i++)
	scopes[i]->notifyTraceUpdate(devicename);
    unlock();
}
