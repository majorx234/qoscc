//
// C++ Interface: dspmm.h
//
// Description:
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef DSPMM_H
#define DSPMM_H

#include "dspcommon.h"

#include <sys/time.h>
#include <string>

#include "dbuffer.h"

class dspMM : public dspCommon {
public:
    dspMM();
    ~dspMM();

    int openDevice();
    int closeDevice();
    int setDeviceName(const std::string&);
    int setDspRate(double);
    int setDspSize(int);
    int type();
    int readdsp(dbuffer *);
    int setChannels(unsigned int channels);
    void setBufferSize(unsigned int size);
    int setAdjust(double);

    void getDspChannelList(stringlist *);
    void getDspRateList(stringlist *);
    void getDspSizeList(stringlist *);
    void getDspNameList(stringlist *liste);

private:

    int fd;
    struct timeval starttime;
    struct timezone tz;
};

#endif
