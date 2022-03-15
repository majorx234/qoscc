//
// C++ Interface: dspalsa.h
//
// Description:
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DSPALSA_H
#define DSPALSA_H

#include "dspcommon.h"

#include <alsa/asoundlib.h>

#include "stringlist.h"
#include "dbuffer.h"

class dspALSA : public dspCommon {
public:
    dspALSA();

    ~dspALSA();

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

    unsigned int getChannels();
    double getDspRate();
    unsigned int getDspSize();

    void getDspChannelList(stringlist *);
    void getDspRateList(stringlist *);
    void getDspSizeList(stringlist *);
    void getDspNameList(stringlist *liste);

private:
    snd_pcm_t *pcm_handle;
    snd_pcm_stream_t stream;
    snd_pcm_hw_params_t *hwparams;

    stringlist devicelist;
};

#endif

