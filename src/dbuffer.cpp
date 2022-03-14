//
// C++ Implementation: dbuffer.cpp
//
// Description:
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "dbuffer.h"

#include <math.h>
#include <fftw3.h>

#ifdef QOSCC_DEBUG
#include <stdio.h>
#endif

#ifdef HAVE_CONFIG_H
#include "../config.h"
#endif

#include <memory.h>
#include "misc.h"

using namespace std;

dbuffer::dbuffer(){
    initvars();
}

void dbuffer::initvars(){
    // init to defaults
//    MSG(MSG_DEBUG, "buf = ....\n");
    buf = new double[2048];
    if(!buf){
        MSG(MSG_ERROR, "Could not allocate memory!\n");
        exit(1);
    }
    buflen = 2048;
    fill = 0;
    
    type = tnone;
    unit = unone;
    samplerate = 0;
    planIsValid = false;
}

    
dbuffer::dbuffer(unsigned int size, double samplerate, double init){
    initvars();
    if(size == 0)
        return;

    if(buflen < size){
        delete [] buf;
        MSG(MSG_DEBUG, "buf = ....\n");
        buf = new double[size];
        buflen = size;
    }
    if(!buf){
//        MSG(MSG_ERROR, "Could not allocate memory!\n");
        exit(1);
    }
    for(unsigned int i = 0; i < size; i++)
        buf[i] = init;
    fill = size;
}

dbuffer::dbuffer(const dbuffer& oldbuffer){
    buflen = oldbuffer.buflen;
    fill = oldbuffer.fill;
    type = oldbuffer.type;
    unit = oldbuffer.unit;
    samplerate = oldbuffer.samplerate;
    planIsValid = false;
    
//    MSG(MSG_DEBUG, "buf = ....\n");
    buf = new double[buflen];
    if(!buf){
        MSG(MSG_ERROR, "Could not allocate memory!\n");
        exit(1);
    }
    
    memcpy(buf, oldbuffer.buf, sizeof(*buf) * fill);
}

dbuffer::dbuffer(double *oldbuf, unsigned int oldsize, double oldrate, sampleTypes oldtype, sampleUnits oldunit){
    // init to defaults...
    initvars();
    
//    printf("oldbuf: %p, oldsize: %d, oldrate: %.3f\n", oldbuf, oldsize, oldrate);
    if(!oldsize || !oldbuf || !oldrate) // do nothing if values are nonsense
        return;

    samplerate = oldrate;

    // create new
    if(oldsize > buflen){
        delete [] buf;
//        MSG(MSG_DEBUG, "buf = ....\n");
        buf = new double[oldsize];
        if(!buf){
            MSG(MSG_ERROR, "Could not allocate memory!\n");
            exit(1);
        }
        buflen = oldsize;
    }

    // copy old..
    memcpy(buf, oldbuf, sizeof(*buf) * oldsize);
    fill = oldsize;
    type = oldtype;
    unit = oldunit;
}


dbuffer::~dbuffer(){
    if(planIsValid)
        fftw_destroy_plan(fftPlan);
    delete [] buf;
}

dbuffer dbuffer::operator +(const dbuffer &left){
    // create new object
    dbuffer newbuf;

//    printf("&newbuf = %p, &left = %p, this = %p\n", &newbuf, &left, this);
    
    if(left.fill + fill > newbuf.buflen){
        delete [] newbuf.buf;
        newbuf.buf = new double[left.fill + fill];
        if(!newbuf.buf){
            MSG(MSG_ERROR, "Could not allocate memory!\n");
            exit(1);
        }
        newbuf.buflen = left.fill + fill;
    }
    
    // copy buffer from first argument
    memcpy(newbuf.buf, buf, sizeof(*buf) * fill);
    // copy buffer from second argument
    memcpy(newbuf.buf + fill, left.buf, sizeof(*buf) * left.fill);

    // copy parameter types
    newbuf.fill = fill + left.fill;
    // take parameters from first argument
    // if it exists
    if(buflen){
        newbuf.type = type;
        newbuf.unit = unit;
        newbuf.samplerate = samplerate;
    }
    // otherwise take parameters from second argument
    else{
        newbuf.type = left.type;
        newbuf.unit = left.unit;
        newbuf.samplerate = left.samplerate;
    }
        
    return newbuf;
}

dbuffer& dbuffer::operator =(const dbuffer &oldbuffer){
    // avoid copying to self
    if(&oldbuffer == this)
        return *this;
    if(oldbuffer.fill > buflen){
        delete [] buf;
//        MSG(MSG_DEBUG, "buf = ....\n");
        buf = new double[oldbuffer.fill];
        if(!buf){
            MSG(MSG_ERROR, "Could not allocate memory!\n");
            exit(1);
        }
        buflen = oldbuffer.fill;
        
        // plan gets invalid by this relocation
        if(planIsValid){
            MSG(MSG_DEBUG, "FFTW-Plan gets invalid\n");
            fftw_destroy_plan(fftPlan);
            planIsValid = false;
        }
    }
    memcpy(buf, oldbuffer.buf, sizeof(*buf) * oldbuffer.fill);
    
    // FIXME: this is in principle bad, but avoids a recalculate of the plan every time....
    /*
    // plan remains valid if the size of the buffer hasn't changed
    if(planIsValid && fill != oldbuffer.fill){
//      MSG(MSG_DEBUG, "FFTW-Plan gets invalid\n");
        fftw_destroy_plan(fftPlan);
        planIsValid = false;
    }
    */
    
    // copy over info from old buffer
    fill = oldbuffer.fill;
    type = oldbuffer.type;
    unit = oldbuffer.unit;
    samplerate = oldbuffer.samplerate;

    return *this;
}

double& dbuffer::operator[](unsigned int index){
    return buf[index];
}

double dbuffer::getValue(double time){
    // linear interpolation used
    
    // index left of specified time (rounded down)
    unsigned int i = int(time * samplerate);

    // check if index is outside this buffer;
    if(i > buflen - 1)
            return INFINITY;
    if(i == buflen - 1)
        return buf[buflen - 1];
          // y_0  + dy        * dx
    return buf[i] + (buf[i+1] - buf[i]) * (time - i/samplerate) * samplerate;
}

std::string dbuffer::getTypeString(){
    std::string types[] = {"none", "s", "Hz"};
    return types[type];
}

std::string dbuffer::getUnitString(){
    std::string units[] = {"none", "V DC", "V AC", "A DC", "A AC", "Ohm", "W", "K", "°C", "Fahrenheit", "F"};
    return units[unit];
}

#ifdef QOSCC_DEBUG
int dbuffer::dump(FILE *file){
    
    fprintf(file, "Buffer dump\n------------\n");
    fprintf(file, "Unit: %s\n", getUnitString().c_str());
    fprintf(file, "samples:%d, rate:%6.3f*1/%s\n", fill, samplerate, getTypeString().c_str());
    for(unsigned int i = 0; i < fill; i++)
        fprintf(file, "%3d, %6.3f, %6.3f\n", i, i / samplerate,  buf[i]);

    return 0;
}
#endif

void dbuffer::applyFft(){
    // we do not want to convert from the frequency domain or from unknown
    // this will not result in sensible results
    if(type != t){
        return;
    }
    // do not run the fft if there is no data
    // this could lead to errors in fftw.
    if(!fill){
        return;
    }

    // window
    switch(fftWin) {
    case winRect:
        break;
    case winHanning:
        for(unsigned int i = 0; i < fill; i++)
            buf[i] *= 0.5 - 0.5 * cos(2*i*M_PI/fill);
        break;
    case winHamming:
        for(unsigned int i = 0; i < fill; i++)
            buf[i] *= 0.54 - 0.46 * cos(2*i*M_PI/fill);
        break;
    case winBlackmanHarrisA:
        for(unsigned int i = 0; i < fill; i++)
            buf[i] *= 0.355768 - 0.487396*cos(2*i*M_PI/fill) + 0.144232*cos(4*i*M_PI/fill) - 0.012604*cos(6*i*M_PI/fill);
        break;
    case winBlackmanHarrisB:
        for(unsigned int i = 0; i < fill; i++)
            buf[i] *= 10.0/32 - 15.0/32*cos(2*i*M_PI/fill) + 6.0/32*cos(4*i*M_PI/fill) - 1.0/32*cos(6*i*M_PI/fill);
        break;
    }
        
    // make plan if it does not exist;
    if(!planIsValid){
        MSG(MSG_DEBUG, "FFTW-Plan recalculated\n");
        // uuugh, fftw_plan destroys data, so tmp them......
        double *tmp = new double[fill];
        if(!tmp){
            MSG(MSG_ERROR, "Could not allocate memory!\n");
            exit(1);
        }
        memcpy(tmp, buf, sizeof(*buf) * fill);
        fftPlan = fftw_plan_r2r_1d(fill, buf, buf, FFTW_R2HC, FFTW_PRESERVE_INPUT);
        memcpy(buf, tmp, sizeof(*buf) * fill);
        delete [] tmp;
        planIsValid = true;
    }
    
    // run....
    fftw_execute(fftPlan);
    for(unsigned int j = 1; j < fill / 2; j++)
        buf[j] = hypot(buf[j], buf[fill - j])  / fill * 2 / M_SQRT2;
    // scale the first and last buffer item
    buf[0] /= fill; 
    // FIXME: is there sth wrong???? the last sample looks strange in the scope....
    buf[fill/2] /= fill * M_SQRT2;
    
    // we changed the type to frequency domain
    type = f;
    // and the "samplerate" to 1/Hz
    samplerate = 1 / (samplerate / fill);
    // and the number of samples......
    fill = fill / 2 +1;
}

// void setSize(unsigned int)
// resizes the buffer
// this may discard existing data!!!
void dbuffer::setSize(unsigned int newsize){ // set buffer to specified size
    if(buflen < newsize){
        // enlarge buffer
        delete [] buf;
//        MSG(MSG_DEBUG, "buf = ....\n");
        buf = new double[newsize];
        if(!buf){
            MSG(MSG_ERROR, "Could not allocate memory!\n");
            exit(1);
        }
        buflen = newsize;
    }
    fill = newsize;
}

