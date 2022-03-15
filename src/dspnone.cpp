//
// C++ Implementation: dspnone.cpp
//
// Description: Dummy for storing data and being able to create some object...
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "dspnone.h"
#include "dspcommon.h"
#include "stringlist.h"
#include "dbuffer.h"

dspNONE::dspNONE() : dspCommon() {}


dspNONE::~dspNONE() {}


int dspNONE::openDevice() {
    return -1;
}

int dspNONE::setDeviceName(const std::string& n) {
    deviceName = n;
    return 0;
}

int dspNONE::closeDevice() {
    return 0;
}

int dspNONE::setDspRate(double n) {
    dspRate = n;
    return 0;
}

int dspNONE::setDspSize(int n) {
    dspSize = n;
    return 0;
}

void dspNONE::setBufferSize(unsigned int size) {
    bufferSize = size;
}

int dspNONE::type() {
    return PCM_NONE;
}

int dspNONE::readdsp(dbuffer *) {
    return 0;
}

int dspNONE::setChannels(unsigned int n) {
    channels = n;
    return 0;
}

int dspNONE::setAdjust(double n) {
    adjust = n;
    return 0;
}

void dspNONE::getDspChannelList(stringlist *) {}
void dspNONE::getDspRateList(stringlist *) {}
void dspNONE::getDspSizeList(stringlist *) {}
void dspNONE::getDspNameList(stringlist *) {}
