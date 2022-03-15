//
// C++ Interface: %{MODULE}
//
// Description:
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DSPNONE_H
#define DSPNONE_H

#include "dspcommon.h"

#include "dbuffer.h"

class dspNONE : public dspCommon {
public:
    dspNONE();

    ~dspNONE();

    int openDevice();
    int setDeviceName(const std::string&);
    int closeDevice();
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
};

#endif
