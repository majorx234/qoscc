//
// C++ Interface: dspcommon.h
//
// Description:
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef DSPCOMMON_H
#define DSPCOMMON_H

#include <string>

#include "dbuffer.h"
#include "stringlist.h"

class dspCommon {
public:
    dspCommon();
    virtual ~dspCommon();

    virtual int openDevice() = 0;
    virtual int setDeviceName(const std::string&) = 0;
    virtual int closeDevice() = 0;
    virtual int setDspRate(double) = 0;
    virtual int setDspSize(int) = 0;
    virtual int type() = 0;
    virtual int readdsp(dbuffer *) = 0;
    virtual int setChannels(unsigned int) = 0;

    virtual void getDspChannelList(stringlist *) = 0;
    virtual void getDspRateList(stringlist *) = 0;
    virtual void getDspSizeList(stringlist *) = 0;
    virtual void getDspNameList(stringlist *) = 0;

    virtual void setBufferSize(unsigned int) = 0;
    virtual int setAdjust(double) = 0;

    double getAdjust();
    unsigned int getChannels();
    double getDspRate();
    unsigned int getDspSize();
    std::string getDeviceName();
    unsigned int getBufferSize();

    bool isRunning();

protected:
    bool running;
    std::string deviceName;
    unsigned int dspSize;
    double dspRate;
    unsigned int channels;
    unsigned int bufferSize;
    double adjust;
};

#define PCM_NONE 0x00
#define PCM_OSS  0x01
#define PCM_ALSA 0x02
#define PCM_JACK 0x03
#define PCM_MM   0x04

#endif
