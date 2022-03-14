//
// C++ Implementation: dspcommon.cpp
//
// Description:
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "dspcommon.h"

#include <pthread.h>

#include <string>

dspCommon::dspCommon() {
    deviceName = "unknown";
    dspRate = 0;
    dspSize = 0;
    channels = 0;
    running = false;
    bufferSize = 0;
    adjust = 1.0;
}

dspCommon::~dspCommon() {}

bool dspCommon::isRunning() {
    return running;
}

unsigned int dspCommon::getChannels() {
    return channels;
}

double dspCommon::getDspRate() {
    return dspRate;
}

unsigned int dspCommon::getDspSize() {
    return dspSize;
}

std::string dspCommon::getDeviceName() {
    return deviceName;
}

double dspCommon::getAdjust() {
    return adjust;
}

unsigned int dspCommon::getBufferSize(){
    return bufferSize;
}

