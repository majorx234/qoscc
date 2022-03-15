//
// C++ Implementation: dspmm.cpp
//
// Description: Backend for serial multimeters
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "dspmm.h"

#include <fcntl.h>
#include <sys/termios.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <string>
#include <cstring>
#include <unistd.h>  // NOLINT
                     // 
#include "stringlist.h"
#include "misc.h"
#include "dbuffer.h"

dspMM::dspMM() {
    adjust = 1.0;
    channels = 1;
    tz.tz_minuteswest = 0;
    tz.tz_dsttime = 0;
}

dspMM::~dspMM() {
    if(running)
        closeDevice();

}

int dspMM::openDevice() {
    if(running)
        return 0;

    struct termios settings;

    fd=open(deviceName.c_str(),O_RDWR|O_NOCTTY);
    if(fd==-1) {
        fprintf(stderr,"dspMM::openDevice(): Cannot open device %s!\n", deviceName.c_str());
        return -1;
    }

    if(!isatty(fd)) {
        fprintf(stderr,"dspMM::openDevice(): %s is not a valid device!\n", deviceName.c_str());
        close(fd);
        return -1;
    }

    settings.c_iflag = IGNBRK | IGNPAR;
    settings.c_oflag = 0;
    settings.c_cflag = CS7 | CSTOPB | CREAD | CLOCAL;
    settings.c_lflag = 0;
    settings.c_cc[VMIN] = 0;
    settings.c_cc[VTIME] = 30;

    // test speed....
    // the fastest speeds will be tested first...
    int speed[] = {B9600, B1200, B600, 0};
    int si = 0;
    do {
        printf("testing %d\n", speed[si]);
        fflush(stdout);
        // configure the device
        if(cfsetispeed(&settings, speed[si]) | cfsetospeed(&settings, speed[si])) {
            MSG(MSG_ERROR, "Unable to set terminal parameters!\n");
            continue;
        }
        if(tcsetattr(fd,TCSANOW,&settings)==-1) {
            fprintf(stderr,"dspMM::openDevice(): Cannot set terminal parameters for %s (%s)!\n", deviceName.c_str(), strerror(errno));
            close(fd);
            return -1;
        }

        int modelines=0;
        if(ioctl(fd,TIOCMGET,&modelines)==-1) {
            fprintf(stderr,"dspMM::openDevice(): Cannot set terminal parameters for %s (%s)!\n", deviceName.c_str(), strerror(errno));
            close(fd);
            return -1;
        }

        modelines &= ~TIOCM_RTS;
        if(ioctl(fd,TIOCMSET,&modelines)==-1) {
            fprintf(stderr,"dspMM::openDevice(): Cannot set terminal parameters for %s (%s)!\n", deviceName.c_str(), strerror(errno));
            close(fd);
            return -1;
        }

        // test multimeter...
        // flush input buffer
        char dump;
        while(read(fd, &dump, 1) == 1)
            ;
        // send request
        write(fd, "D\n", 2);
        // count returned characters
        int i = 0;
        while(read(fd, &dump, 1) == 1)
            i++;

        if(i == 14) {
            printf("using speed %d!\n", speed[si]);
            // init timer
            gettimeofday(&starttime, &tz);
            running = true;
            return 0;
        }

        // else test a lower speed...
        si ++;
    } while(speed[si]);

    close(fd);
    return -1;
}

int dspMM::closeDevice() {
    if(!running)
        return 0;
    running = false;
    return close(fd);
}

int dspMM::setDeviceName(const std::string& n) {
    if(!n.compare(""))
        return -1;

    if(!running)
        deviceName = n;

    closeDevice();
    deviceName = n;
    openDevice();
    return 0;
}

int dspMM::setDspRate(double n) {
    if(n == 0.0)
        return -1;
    
    dspRate = n;

    return 0;
}

int dspMM::setDspSize(int) {
    // nothing to set!!
    return 0 ;
}

int dspMM::type() {
    return PCM_MM;
}

int dspMM::readdsp(dbuffer *fbuf) {
    // int read_dmm(int fd,char buf[14])
    if(!running)
        return -1;
    
    // check if at least one buffer exists!
    if(!fbuf)
	return -1;

    char buf[14];
    int nbyte;

    // wait till we can read the next sample
    struct timeval currtime;
    gettimeofday(&currtime, &tz);
    // diffusec: modulo of delta t from the beginning and the period time
    // so this is the time from the last call to this function (approx.)
    // but absolute timing is precise, as everything is referenced back
    // to the start of this device (so we might have a low jitter due to
    // system load or so, but no continued errors that might sum up
    // with time.
    double diffusec = fmod((currtime.tv_sec * 1000000.0 + currtime.tv_usec) - (starttime.tv_sec * 1000000.0 + starttime.tv_usec), 1000000 / dspRate) ;

    // sleep if elapsed time was smaller than the period time
    if(diffusec > 1.0 / dspRate) {
        usleep((unsigned int)(1000000.0 / dspRate - diffusec + 0.5));
    } else
        MSG(MSG_WARN, "System is to slow to handle this sampling rate");

    // send request
    write(fd,"D\n",2);

    for(int i = 0; i < 14; i++) {
        nbyte = read(fd, &buf[i], 1);
        if(nbyte != 1) {
            MSG(MSG_ERROR, "Could not read from multimeter");
            sleep(1);
            return -1;
        }
    }
    // ensure valid string....
    buf[14] = '\0';
    
    // get unit...
    // These are the units supported by the ME-32
    // report me types for others.....
    dbuffer::sampleUnits unit = dbuffer::unone;
    if(!strncmp(buf, "DC", 2)){ // DC amp and volt
	if(buf[12] == 'V')
	    unit = dbuffer::vdc;
	else if(buf[12] == 'A')
	    unit = dbuffer::adc;
	else
	    unit = dbuffer::unone;
    }
    else if(!strncmp(buf, "AC", 2)){ // AC amp and volt
	if(buf[12] == 'V')
	    unit = dbuffer::vac;
	else if(buf[12] == 'A')
	    unit = dbuffer::aac;
	else
	    unit = dbuffer::unone;
    }
    else if(!strncmp(buf, "OH", 2)) // Ohms
        unit = dbuffer::ohm;
    else if(!strncmp(buf, "DI", 2)) // Uf in Volts dc
        unit = dbuffer::vdc;
    else if(!strncmp(buf, "TE", 2)) // Temperature in Celsius
        unit = dbuffer::celsius;
    else if(!strncmp(buf, "CA", 2)) // Capacity in Farad
        unit = dbuffer::farad;
    
    fbuf[0].setUnit(unit);
//    printf("%s\n", fbuf[0].getUnitString().c_str());fflush(stdout);    
    
    // "fill" the buffer
    fbuf[0].setSize(1);
    fbuf[0].setType(dbuffer::t);
    // strip '\r' and last character of unit (this
    // contains m of "Ohm" which confuses stringToNum)
    buf[12] = '\0';
    fbuf[0][(unsigned int)0] = stringToNum(buf + 3);

    return 1;
}

int dspMM::setChannels(unsigned int channels) {
    // do nothing. we have one channel!
    // but inform our caller....
    if(channels != 1)
        return -1;
    return 0;
}

void dspMM::setBufferSize(unsigned int) {
    // nothing to set!
}

int dspMM::setAdjust(double) {
    // do not accept adjusting!
    return -1;
}

void dspMM::getDspChannelList(stringlist *n) {
    // we have only one channel!
    n->addString("1");
}

void dspMM::getDspRateList(stringlist *n) {
    n->addString("0.5");
}

void dspMM::getDspSizeList(stringlist *n) {
    n->addString("not used");
}

void dspMM::getDspNameList(stringlist *n) {
    n->addString("/dev/ttyS0");
    n->addString("/dev/ttyS1");
}
