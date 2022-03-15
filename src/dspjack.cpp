//
// C++ Implementation: dspjack.cpp
//
// Description:
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "dspjack.h"

#include <jack/jack.h>
#include <string.h>
#include <string>

#include "stringlist.h"
#include "misc.h"
#include "dbuffer.h"

dspJACK::dspJACK() : dspCommon() {
    frames = 0;
    buffer = NULL;
    jackdRunning = true;

    // init "buffer ready" condition
    pthread_cond_init(&data_ready, NULL);
    pthread_mutex_init(&data_ready_lock, NULL);

    // register as client
    if((client = jack_client_new("QOscC")) == 0) {
        MSG(MSG_ERROR, "Cannot register as Client. Jackd running?\n");
        jackdRunning = false;
        return;
    }

    // register the  callback functions
    if(jack_set_process_callback(client, dspJACK::jack_process, this)) {
        MSG(MSG_ERROR, "cannot register callback process!\n");
        return;
    }
    jack_on_shutdown(client, dspJACK::jack_shutdown, this);
    // get & store sample rate
    dspRate = (unsigned int) jack_get_sample_rate (client);
}


dspJACK::~dspJACK() {
    if(jackdRunning)
        jack_client_close(client);

    pthread_cond_destroy(&data_ready);
    pthread_mutex_destroy(&data_ready_lock);
}


int dspJACK::openDevice() {
    if(running)
        return 0;
    if(!jackdRunning)
        return -1;


    // get the buffer array and init it.
    buffer = new jack_default_audio_sample_t*[channels];
    memset(buffer, 0, sizeof(jack_default_audio_sample_t *) * channels);

    // prepare ports
    inports = new jack_port_t*[channels];
    outports = new jack_port_t*[channels];
    if(!inports || !outports) {
        MSG(MSG_ERROR, "Unable to allocate memory\n");
        exit(-1);
    }
    for(unsigned int ch = 0; ch < channels; ch++) {
        char name[80];
        sprintf(name, "input_%d", ch);
        if((inports[ch] = jack_port_register (client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsInput, 0)) == 0) {
            fprintf (stderr, "cannot register input port \"%s\"!\n", name);
            jack_client_close (client);
            return -1;
        }
        sprintf(name, "output_%d", ch);
        if((outports[ch] = jack_port_register (client, name, JACK_DEFAULT_AUDIO_TYPE, JackPortIsOutput, 0)) == 0) {
            fprintf (stderr, "cannot register input port \"%s\"!\n", name);
            jack_client_close (client);
            return -1;
        }
    }

    // activate this client
    if(jack_activate(client)) {
        MSG(MSG_ERROR, "Cannot activate client\n");
        return -1;
    }

    running = true;

    return 0;
}

int dspJACK::closeDevice() {
    if(!jackdRunning)
        return -1;

    if(running) {
        // deactivate client
        jack_deactivate(client);

        // remove buffer
        delete [] buffer;
        buffer = NULL;

        // detach from ports
        for(unsigned int ch = 0; ch < channels; ch++) {
            jack_port_unregister(client, inports[ch]);
            jack_port_unregister(client, outports[ch]);
        }
        delete [] inports;
        inports = NULL;
        delete [] outports;
        outports = NULL;
        running = false;
    }
    return 0;
}

int dspJACK::setDeviceName(const std::string&) {
    return 0;
}

int dspJACK::setDspRate(double) {
    // cannot be selected!
    return 0;
}

int dspJACK::setDspSize(int) {
    // jack has no selectable sample size
    return 0;
}

int dspJACK::type() {
    return PCM_JACK;
}

int dspJACK::readdsp(dbuffer *buf) {
    if(!running)
        return -1;

    // wait till data is ready...
    pthread_mutex_lock(&data_ready_lock);
    pthread_cond_wait(&data_ready, &data_ready_lock);

    //    printf("dspJACK::readdsp(): returning data!\n");
    // enlarge buffersize if necessary. else we'd loose samples......

    // process data!

    for(unsigned int ch = 0; ch < channels; ch++){
        buf[ch].setSize(frames);
	buf[ch].setUnit(dbuffer::vdc);
	buf[ch].setType(dbuffer::t);	
        for(unsigned int i = 0; i < frames; i++)
            buf[ch][i] = double(buffer[ch][i] * adjust);
    }

    pthread_mutex_unlock(&data_ready_lock);
    return frames;
}

int dspJACK::setChannels(unsigned int n) {
    if(!n)
        return -1;
    if(!running) {
        channels = n;
        return 0;
    }
    // device running: stop, reconfigure and start again
    // FIXME: do this runtime, so currently connected
    // channels do not get disconnected!
    closeDevice();
    channels = n;
    openDevice();
    return 0;
}

// not used in this api
void dspJACK::setBufferSize(unsigned int n) {
    bufferSize = n;
}

int dspJACK::setAdjust(double n) {
    adjust = n;
    return 0;
}


unsigned int dspJACK::getChannels() {
    return channels;
}

double dspJACK::getDspRate() {
    return dspRate;
}

unsigned int dspJACK::getDspSize() {
    return 0;
}

void dspJACK::getDspChannelList(stringlist *liste) {
    liste->addString("1");
    liste->addString("2");
    liste->addString("3");
    liste->addString("4");
}

void dspJACK::getDspRateList(stringlist *liste) {
    char rate[80];
    sprintf(rate, "%d Hz", int(dspRate + 0.5));
    liste->addString(rate);
}

void dspJACK::getDspSizeList(stringlist * liste) {
    liste->addString("float");
}

void dspJACK::getDspNameList(stringlist *liste) {
    liste->addString("not used");
}

int dspJACK::jack_process(jack_nframes_t nframes, void *arg) {
    if(!((dspJACK*)arg)->isRunning())
        return 0;
    jack_default_audio_sample_t *out;

    //    printf("dspJACK::jack_process(), nframes = %d\n", nframes);

    // read data
    for(unsigned int ch = 0; ch < ((dspJACK*)arg)->getChannels(); ch++) {
        ((dspJACK*)arg)->buffer[ch] = (jack_default_audio_sample_t*)jack_port_get_buffer( ((dspJACK*)arg)->inports[ch], nframes);
        // copy buffer to the monitor output
        out = (jack_default_audio_sample_t*)jack_port_get_buffer( ((dspJACK*)arg)->outports[ch], nframes);
        memcpy(out, ((dspJACK*)arg)->buffer[ch], sizeof(jack_default_audio_sample_t) * nframes);
    }

    ((dspJACK*)arg)->frames = nframes;

    // try to set the ready flag
    if(pthread_mutex_trylock(&(((dspJACK*)arg)->data_ready_lock)) == 0) {
        pthread_cond_signal(&(((dspJACK*)arg)->data_ready));
        pthread_mutex_unlock(&(((dspJACK*)arg)->data_ready_lock));
    }
    return 0;
}

void dspJACK::jack_shutdown(void *arg) {
    // notify that the device is not running any more....
    ((dspJACK*)arg)->running = false;
}
