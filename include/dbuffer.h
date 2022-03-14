//
// C++ Interface: dbuffer.h
//
// Description: 
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef DBUFFER_H
#define DBUFFER_H

//#include "../config.h"

#include <string>
#include <fftw3.h>

#ifdef QOSCC_DEBUG
#include <stdio.h>
#endif

class dbuffer{
  public:
    enum sampleTypes {tnone = 0, t, f}; // time or frequency domain
    enum sampleUnits {unone = 0, vdc, vac, adc, aac, ohm, watt, kelvin, celsius, fahrenheit, farad}; // unit of the data
    enum fftWinType {winRect = 0, winHanning, winHamming, winBlackmanHarrisA, winBlackmanHarrisB};
    
    // construction
    dbuffer();
    dbuffer(unsigned int, double, double = 0.0); // alloc buffer
    dbuffer(const dbuffer &); // create from a existing buffer
    dbuffer(double *, unsigned int, double, sampleTypes, sampleUnits); // create from existing double buffer
    ~dbuffer(); // destroy buffer
    
    // operations
    dbuffer operator+(const dbuffer &); // append buffer
    dbuffer& operator=(const dbuffer &); // copy/replace buffer
    double& operator[](unsigned int); // get value at index
    double getValue(double);  // get value at time / frequency
    
    // output
    double getSampleRate(){return samplerate;};
    unsigned int getNumSamples(){return fill;};
    sampleTypes getType(){return type;};
    sampleUnits getUnit(){return unit;};
    std::string getTypeString();
    std::string getUnitString();
    
    fftWinType getFftWinType(){return fftWin;};
    
    // input
    void setFftWinType(fftWinType t){fftWin = t;};
    void setSize(unsigned int); // set buffer to specified size
    void setSampleRate(double newrate){samplerate = newrate;};
    void setUnit(sampleUnits newUnit){unit = newUnit;};
    void setType(sampleTypes newType){type = newType;};
    
    // transform
    void applyFft();
    
    // misc / debug
#ifdef QOSCC_DEBUG
    int dump(FILE *file);
#endif
    
  private:
    unsigned int buflen;     // nr. of allocated samples
    unsigned int fill;       // nr. of valid samples
    double *buf;
    double samplerate;       // samples per <type>

    sampleTypes type;
    sampleUnits unit;
    
    fftWinType fftWin;
    fftw_plan fftPlan;
    bool planIsValid;        // plan gets invalid as the size changes
    
    void initvars();

};

#endif
