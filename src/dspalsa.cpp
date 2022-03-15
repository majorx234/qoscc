//
// C++ Implementation: dspalsa.cpp
//
// Description:
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "dspalsa.h"
#include "dspcommon.h"

#include <alsa/asoundlib.h>

#include "stringlist.h"
#include "misc.h"
#include "dbuffer.h"

dspALSA::dspALSA() : dspCommon() {
    // some _basic_ inits for alsa
    stream = SND_PCM_STREAM_CAPTURE;
    deviceName = "hw:0,0";
    snd_pcm_hw_params_malloc(&hwparams);

    // read list of available cards...
    int card = -1;
    snd_ctl_t *ctl;
    snd_pcm_info_t *pcm_info;

    snd_pcm_info_malloc(&pcm_info);

    while(!snd_card_next(&card) && card > -1) {
        int dev = -1;
        char devname[80];
        sprintf(devname, "hw:%d", card);
        snd_ctl_open(&ctl, devname, 0);
        while(!snd_ctl_pcm_next_device(ctl, &dev) && dev > -1) {
            snd_pcm_info_set_device(pcm_info, dev);
            snd_pcm_info_set_subdevice(pcm_info, 0);
            snd_pcm_info_set_stream(pcm_info, SND_PCM_STREAM_CAPTURE);
            if(snd_ctl_pcm_info(ctl, pcm_info)<0)
                continue;
            char device[80];
            sprintf(device, "hw:%d,%d (%s)", card, dev, snd_pcm_info_get_name(pcm_info));
            devicelist.addString(device);
            //     printf("%s\n", device);
        }
        snd_ctl_close(ctl);
    }
    snd_pcm_info_free(pcm_info);

}

dspALSA::~dspALSA() {
    if(running)
        closeDevice();
    snd_pcm_hw_params_free(hwparams);
}

int dspALSA::openDevice() {
    if(running)
        return 0;

    if(!dspSize || !dspRate || !channels || !bufferSize) {
        MSG(MSG_WARN, "dspALSA::openDevice(): Device not correctly configured!\n");
        return -1;
    }

    int err;

    // copy name until the whitespace or the backets
    std::string devname;
    for(unsigned int i = 0; i < deviceName.size(); i++) {
        if(deviceName[i] == '(' || deviceName[i] == ' ')
            break;
        devname += deviceName[i];
    }

    if((err = snd_pcm_open(&pcm_handle, devname.c_str(), stream, 0 )) < 0) {
        fprintf(stderr, "dspALSA::openDevice(): unable to open device %s (%s)\n", devname.c_str(), snd_strerror(err));
        fflush(stderr);
        return -1;
    }

    if((err = snd_pcm_hw_params_any(pcm_handle, hwparams)) < 0) {
        closeDevice();
        fprintf(stderr, "dspALSA::openDevice(): unable to configure this device (%s)\n", snd_strerror(err));
        fflush(stderr);
        return -1;
    }

    if((err = snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED)) < 0) {
        closeDevice();
        fprintf(stderr, "dspALSA::openDevice(): unable to set format (%s)\n", snd_strerror(err));
        fflush(stderr);
        return -1;
    }

    _snd_pcm_format format;
    switch(dspSize) {
    case 24:
        format = SND_PCM_FORMAT_S24;
        break;
    case 32:
        format = SND_PCM_FORMAT_S32;
        break;
    case 16:
    default:
        format = SND_PCM_FORMAT_S16;
        break;
    }

    if((err = snd_pcm_hw_params_set_format(pcm_handle, hwparams, format)) < 0) {
        closeDevice();
        fprintf(stderr, "dspALSA::openDevice(): unable to set format (%d) for this device (%s)\n", dspSize, snd_strerror(err));
        fflush(stderr);
        return -1;
    }

    unsigned int rate = int(dspRate + 0.5);
    if((err = snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &rate, NULL)) < 0) {
        closeDevice();
        fprintf(stderr, "dspALSA::openDevice(): unable to set sampling rate (%d) for this device (%s)\n", dspRate, snd_strerror(err));
        fflush(stderr);
        return -1;
    }
    dspRate = rate;

    if((err = snd_pcm_hw_params_set_channels(pcm_handle, hwparams, channels)) < 0) {
        closeDevice();
        fprintf(stderr, "dspALSA::openDevice(): unable to set channels (%d) for this device (%s)\n", channels, snd_strerror(err));
        fflush(stderr);
        return -1;
    }

    // hugh, what matter are periods??
    // we leave it at some default by now....
    if((err = snd_pcm_hw_params_set_periods(pcm_handle, hwparams, 2, 0)) < 0) {
        closeDevice();
        fprintf(stderr, "dspALSA::openDevice(): unable to set number of periods for this device (%s)\n", snd_strerror(err));
        fflush(stderr);
        return -1;
    }

    if((err = snd_pcm_hw_params_set_buffer_size_near(pcm_handle, hwparams, (snd_pcm_uframes_t*)&bufferSize ) ) < 0) {
        closeDevice();
        fprintf(stderr, "dspALSA::openDevice(): unable to set buffer size (%d) for this device (%s)\n", bufferSize, snd_strerror(err));
        fflush(stderr);
        return -1;
    }

    // apply our settings....
    if((err = snd_pcm_hw_params(pcm_handle, hwparams)) < 0) {
        closeDevice();
        fprintf(stderr, "dspALSA::openDevice(): unable to set HW params for this device (%s)\n", snd_strerror(err));
        fflush(stderr);
        return -1;
    }

    running = true;

    return 0;
}

int dspALSA::closeDevice() {
    running = false;
    if(snd_pcm_close(pcm_handle)) {
        return -1;
    }
    return 0;
}

int dspALSA::setDeviceName(const std::string& n) {
    if(!n.compare(deviceName))
        return 0;
    if(!running) {
        deviceName = n;
        return 0;
    }

    deviceName = n;
    closeDevice();
    openDevice();


    return -1;
}

int dspALSA::setDspRate(double n) {
    if(n == dspRate)
        return 0;

    if(!running) {
        dspRate = n;
        return 0;
    }

    dspRate = n;
    closeDevice();
    openDevice();

    return 0;
}

int dspALSA::setDspSize(int n) {
    if(unsigned (n) == dspSize)
        return 0;
    if(!running) {
        dspSize = n;
        return 0;
    }
    _snd_pcm_format format;
    switch(n) {
    case 24:
        format = SND_PCM_FORMAT_S24;
        break;
    case 32:
        format = SND_PCM_FORMAT_S32;
        break;
    case 16:
    default:
        format = SND_PCM_FORMAT_S16;
        break;
    }

    dspSize = n;
    closeDevice();
    openDevice();
    return 0;
}

int dspALSA::setChannels(unsigned int n) {
    if(channels == n)
        return 0;
    if(!running) {
        channels = n;
        return 0;
    }
    channels = n;
    closeDevice();
    openDevice();
    return 0;
}

void dspALSA::setBufferSize(unsigned int size) {
    if(size == bufferSize)
        return;
    if(!running) {
        bufferSize = size;
        return;
    }
    bufferSize = size;
    closeDevice();
    openDevice();
}

int dspALSA::type() {
    return PCM_ALSA;
}

int dspALSA::readdsp(dbuffer *buf) {
    if(!running)
        return -1;

    int dsp_bytes;
    int read_samples;
    int samplemask = 0x00;

    switch(dspSize) {
    case 8:
        samplemask = 0xff;
        break;
    case 16:
        samplemask = 0xffff;
        break;
    case 24:
        samplemask = 0xffffff;
        break;
    case 32:
        samplemask = 0xffffffff;
        break;
    }

    // dsp_bytes: number of bytes per sample
    dsp_bytes = dspSize / 8;

    // aquire buffer. 4 additional bytes to prevent read from unallocated space, when sample size is smaller
    // than an int (which it is almost certainly) and trying to read the last bytes. though this should not matter,
    // as these are masked out, but now, valgrind cannot complain about it any more....
    char *buffer = new char[bufferSize * dsp_bytes * channels + 4];
    
    if(!buffer) {
        fprintf(stderr, "readbuffers(): Not enough memory (%s)\n", strerror(errno));
        exit(-1);
    }

    while ((read_samples = snd_pcm_readi(pcm_handle, buffer, bufferSize)) < 0) {
        snd_pcm_prepare(pcm_handle);
        fprintf(stderr, "%s: Buffer overrun.\n", deviceName.c_str());
    }

    // set correct parameters
    for(unsigned int ch = 0; ch < channels; ch++){
        buf[ch].setSize(read_samples);
        buf[ch].setUnit(dbuffer::vdc);
        buf[ch].setType(dbuffer::t);
    }
    
    // transscribe this buffer to the per-trace float buffer.
    for(unsigned int i = 0; i < unsigned(read_samples); i++) { // process each sample in this device (complete current buffer)
        for(unsigned int ch = 0; ch < channels; ch++) {  // process each channel in this sample
            // this is really cruel, but does the job
            // the algorithm for interleaved data
            // works for all 8, 16, 24 or 32 bit wide signed data
            buf[ch][i] = (double( (*(int*)(((buffer))+(i*channels+ch)*dsp_bytes) & samplemask) << (32-dspSize) ) / 2147483648.0) * adjust;
        }
    }

    delete [] buffer;

    return read_samples;
}

void dspALSA::getDspChannelList(stringlist *liste) {
    liste->addString("1");
    liste->addString("2");
}

void dspALSA::getDspRateList(stringlist *liste) {
    liste->addString("8000");
    liste->addString("11025");
    liste->addString("22050");
    liste->addString("44100");
    liste->addString("48000");
    liste->addString("96000");
    liste->addString("192000");
}

void dspALSA::getDspSizeList(stringlist *liste) {
    liste->addString("16");
    liste->addString("24");
    liste->addString("32");
}

void dspALSA::getDspNameList(stringlist *liste) {
    // just copy over our once created list of devices
    for(unsigned int i = 0; i < devicelist.count(); i++)
        liste->addString(devicelist.getString(i));
}

int dspALSA::setAdjust(double n) {
    adjust = n;
    return 0;
}
