#include "scopeclass.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <errno.h>
#include <string.h>

#include <fftw3.h>

#include "stringlist.h"
#include "misc.h"
#include "dbuffer.h"
#include "traceclass.h"

// FIXME: discard trigger_trace. should be done by trigger_trace_name

ScopeClass::ScopeClass(ControllerClass* _parentController) : parentController(_parentController) 
{
    //global = _parentController->getGlobal(); //toFix not needed parent controller should be used
    initVars();
}

ScopeClass::~ScopeClass() {
    delete [] buf;
}

// void initVars()
// set variables to default / safe settings
void ScopeClass::initVars() {
    hdivs = 10;
    vdivs = 8;
    name = "NoNameScope";
    tracenum = 0;
    bcol    = "#000000";
    gridcol = "#009f00";
    markcol = "#ffffff";
    textcol = "#ffffff";
    mode    = M_YT;
    sweep = 0.002;
    trigger_edge = TE_NONE;
    trigger_pos  = 0;
    trigger_level   = 0;
    trigger_locked = false;
    trigger_source_name = "unknown";
    xysource_x_name = "unknown";
    xysource_y_name = "unknown";
    hold = false;
    dispLog = false;
    dispFMin = 20;
    dispFMax = 20000;
    infoTraceName = "unknown";
    infoPeakFT = 0;
    infoPeakVal = 0;
    infoDC = 0.0;
    dispDb = false;
    dispDbRef = 0.7746; // corresp. dBu (sqrt(600 Ohm * 1mW))
    vDiv = 1.0;
    dbMin = -130.0;
    dbMax = 0.0;

    trigger_source = NULL;
    xysource_x = NULL;
    xysource_y = NULL;

    buf = NULL;
}
//*********************************************************last changes above todo

inline double ScopeClass::calcx(int buffpos, int trace) {
    return (double)(buffpos - trigger_pos) / (sweep * buf[trace].getSampleRate()) + traces[trace]->getXShift();
}

// string getName()
// returns string containing the name of this scope
std::string ScopeClass::getName() {
    return name;
}

// int setName(string)
// sets the name of this scope
int ScopeClass::setName(const std::string &newname) {
    if(parentController->getScope(newname))
        return -1;
    {
        std::lock_guard<std::shared_mutex> lock(mutex);
        // set local name.
        name = newname;
        // set window name
        // FIXME: need to port this to QT5
        //setCaption(newname);
    }
    return 0;
}

// int addTrace(traceClass *)
// adds new trace to list
// also adds a new buffer...
int ScopeClass::addTrace(TraceInterface *newtrace) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    //    printf("%s::addTrace(%s): now have %d traces\n", name.c_str(), newtrace->getname(), tracenum+1);
    //    fflush(stdout);
    TraceInterface **newlist;  // create new, larger array
    newlist = new TraceInterface*[tracenum + 1];
    // copy over old traces
    for(unsigned int i = 0; i < tracenum; i++)
        newlist[i] = traces[i];
    // add new device at end
    newlist[tracenum] = newtrace;

    // add a new buffer pointer

    dbuffer *newbuf = new dbuffer[tracenum + 1];

    //unsigned int *newbuflen = new unsigned int[tracenum + 1];
    for(unsigned int i = 0; i < tracenum; i++) {
        newbuf[i] = buf[i];
    }

    if(buf)
        delete [] buf;
    buf = newbuf;

    // delete old array if it existed
    if(tracenum)
        delete [] traces;
    // copy back new list
    traces = newlist;

    // increase number of devices
    tracenum ++;
    return 0;
}

// void dump()
// print out all important configuration data
void ScopeClass::dump(FILE *file) {
    fprintf(file, "name = %s\n", name.c_str());
    fprintf(file, "sweep = %f\n", sweep);
    fprintf(file, "hdivs = %d\n", hdivs);
    fprintf(file, "vdivs = %d\n", vdivs);
//    fprintf(file, "height = %d\n", height());
//    fprintf(file, "width = %d\n", width());
// TOFIX: x,y? from qt element?
//    fprintf(file, "x = %d\n", x());
//    fprintf(file, "y = %d\n", y());
    fprintf(file, "bcol = %s\n", bcol.c_str());
    fprintf(file, "gridcol = %s\n", gridcol.c_str());
    fprintf(file, "markcol = %s\n", markcol.c_str());
    fprintf(file, "textcol = %s\n", textcol.c_str());
//    fprintf(file, "font = %s\n", font.toString().ascii());
    for(unsigned int i = 0; i < tracenum; i++)
        fprintf(file, "trace = %s\n", traces[i]->getName().c_str());
    switch (mode) {
    case M_XY:
        fprintf(file, "mode = xy\n");
        break;
    case M_FFT:
        fprintf(file, "mode = fft\n");
        break;
    default:
        fprintf(file, "mode = yt\n");
        break;
    }
    fprintf(file, "trigger_level = %f\n", trigger_level);
    switch (trigger_edge) {
    case TE_FALL:
        fprintf(file, "trigger_edge = fall\n");
        break;
    case TE_RISE:
        fprintf(file, "trigger_edge = rise\n");
        break;
    default:
        fprintf(file, "trigger_edge = none\n");
        break;
    }
    fprintf(file, "trigger_source = %s\n", trigger_source_name.c_str());
    fprintf(file, "xysource_x = %s\n", xysource_x_name.c_str());
    fprintf(file, "xysource_y = %s\n", xysource_y_name.c_str());
    fprintf(file, "disp_f_min = %d\n", dispFMin);
    fprintf(file, "disp_f_max = %d\n", dispFMax);
    fprintf(file, "infotrace = %s\n", infoTraceName.c_str());
    fprintf(file, "disp_log = %s\n", dispLog ? "true" : "false");
    fprintf(file, "disp_db = %s\n", dispDb ? "true" : "false");
    fprintf(file, "disp_db_ref = %f\n", dispDbRef);
    fprintf(file, "v_div = %f\n", vDiv);
    fprintf(file, "db_min = %f\n", dbMin);
    fprintf(file, "db_max = %f\n", dbMax);
    fflush(file);
}


// unsigned int getTraceNum()
// return number of traces
unsigned int ScopeClass::getTraceNum() {
    return tracenum;
}

// void setHeight(int newheight)
// sets the new window height
void ScopeClass::setHeight(int newheight) {
    // ToDo notify View    
    //setGeometry(x(), y(), width(), newheight);
}

// void setWidth(int newwidth)
// sets the new window width
void ScopeClass::setWidth(int newwidth) {

    // ToDo notify View    
    //setGeometry(x(), y(), newwidth, height());
}

// void setXPos(int x)
// sets the new position of the window
void ScopeClass::setXPos(int x) {
    // ToDo notify View    
    //setGeometry(x, y(), width(), height());
}

// void setYPos(int y)
// sets the new position of the window
void ScopeClass::setYPos(int y) {
    // ToDo notify View    
    //setGeometry(x(), y, width(), height());
}

// traceClass *getTrace(char*)
// looks for trace with name and returns its pointer
// else it returns the NULL-pointer
TraceInterface *ScopeClass::getTrace(const char *name) {
    return getTrace(std::string(name));
}

// traceClass *getTrace(char*)
// looks for trace with name and returns its pointer
// else it returns the NULL-pointer
TraceInterface *ScopeClass::getTrace(const std::string &name) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    for(unsigned int i = 0; i < tracenum; i++)
        if(name == traces[i]->getName()) {
            return traces[i];
        }
    return NULL;
}

TraceInterface *ScopeClass::getTrace(unsigned int i)
{
  return traces[i];
}  
// int removeTrace(traceClass *trace)
// removes specified trace and its local buffer from list
int ScopeClass::removeTrace(TraceInterface *trace) {
    // bail out if we got a zero pointer
    if(!trace || tracenum < 1)
        return -1;
    {
        std::lock_guard<std::shared_mutex> lock(mutex);

        int removed = 0; // number of traces we removed ....
        for(unsigned int i = 0; i < tracenum; i++) {
            if(trace == traces[i]) {
                removed ++;
            } else {
                traces[i - removed] = traces[i];
                buf[i-removed] = buf[i];
            }
        }
        tracenum -= removed;
    }
    return 0;
}

// int removeTrace(string)
// remove trace of specified name
int ScopeClass::removeTrace(const std::string &tracename) {
    return removeTrace(parentController->getTrace(tracename));
}


// int setTriggerSource(string)
// check and set new trigger source
int ScopeClass::setTriggerSource(std::string newname) {
    TraceInterface *newsource = parentController->getTrace(newname.c_str());
    if(!newsource) {  // trace not found
        return -1;
    }
    {
        std::lock_guard<std::shared_mutex> lock(mutex);    
        trigger_source = newsource;
        trigger_source_name = newname;
    }
    return 0;
}

// void recalc_stringrefs()
// re-read string values from its objects (parent / other trace names)
void ScopeClass::recalc_stringrefs() {
    std::lock_guard<std::shared_mutex> lock(mutex);
    // trigger source:
    if(parentController->hasTrace(trigger_source))
        trigger_source_name = trigger_source->getName();
    // xy sources:
    if(parentController->hasTrace(xysource_x))
        xysource_x_name = xysource_x->getName();
    if(parentController->hasTrace(xysource_y))
        xysource_y_name = xysource_y->getName();
}

// do_trigger()
// find and set the trigger position in the trace
int ScopeClass::do_trigger(unsigned int trace) {
    bool done = false;
    unsigned int i;
    trigger_locked = false;

    trigger_pos = 0;

    switch(trigger_edge) {
    case TE_RISE:
        for(i = 0; i < buf[trace].getNumSamples() - 1 && !done; i++) {
            if(buf[trace][i] < trigger_level && buf[trace][i+1] > trigger_level) {
                done = true;
                trigger_pos = i;
                trigger_locked = true;
            }
        }
        break;
    case TE_FALL:
        for(i = 0; i < buf[trace].getNumSamples() - 1 && !done; i++) {
            if(buf[trace][i] > trigger_level && buf[trace][i+1] < trigger_level) {
                done = true;
                trigger_pos = i;
                trigger_locked = true;
            }
        }
        break;
    }
    return trigger_pos;
}

// int setXYSourceX(string)
// check and set new x source
int ScopeClass::setXYSourceX(std::string newname) {
    TraceInterface *newsource = getTrace(newname);
    if(!newsource)  // trace not found
        return -1;
    {
        std::lock_guard<std::shared_mutex> lock(mutex);
        xysource_x = newsource;
        xysource_x_name = newname;
    }
    return 0;
}

// int setXYSourceY(string)
// check and set new y source
int ScopeClass::setXYSourceY(std::string newname) {
    TraceInterface *newsource = getTrace(newname);
    if(!newsource)  // trace not found
        return -1;
    {
        std::lock_guard<std::shared_mutex> lock(mutex);
        xysource_y = newsource;
        xysource_y_name = newname;
    }
    return 0;
}

// string get??Name()
// return the requested names...
std::string ScopeClass::getTriggerName() {
    return trigger_source_name;
}

std::string ScopeClass::getXYSourceXName() {
    return xysource_x_name;
}

std::string ScopeClass::getXYSourceYName() {
    return xysource_y_name;
}

// stringlist getTraceList()
// return list of Traces
void ScopeClass::getTraceList(stringlist *liste) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    // compose list...
    for(unsigned int i = 0; i < tracenum; i++)
        liste->addString(traces[i]->getName());
}

// all following get / set functions.
// these set / get the corresp. values while locking the scope
// to keep thread safety...
int ScopeClass::getMode() {
    return mode;
}

double ScopeClass::getSweep() {
    return sweep;
}

std::string ScopeClass::getGridCol() {
    return gridcol;
}

std::string ScopeClass::getBCol() {
    return bcol;
}

std::string ScopeClass::getTextCol() {
    return textcol;
}

std::string ScopeClass::getMarkCol() {
    return markcol;
}

unsigned int ScopeClass::getHDivs() {
    return hdivs;
}

unsigned int ScopeClass::getVDivs() {
    return vdivs;
}

int ScopeClass::getTriggerEdge() {
    return trigger_edge;
}

double ScopeClass::getTriggerLevel() {
    return trigger_level;
}

void ScopeClass::setMode(int newmode) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    mode = newmode;
}

void ScopeClass::setSweep(double newsweep) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    sweep = newsweep;
}

void ScopeClass::setGridCol(const std::string& color) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    gridcol = color;
}

void ScopeClass::setBCol(const std::string& color) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    bcol = color;
}

void ScopeClass::setMarkCol(const std::string& color) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    markcol = color;
}

void ScopeClass::setTextCol(const std::string& color) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    textcol = color;
}

void ScopeClass::setHDivs(unsigned int n) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    hdivs = n;
}

void ScopeClass::setVDivs(unsigned int n) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    vdivs = n;
}

void ScopeClass::setTriggerEdge(int edge) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    trigger_edge = edge;
}

void ScopeClass::setTriggerLevel(double level) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    trigger_level = level;
}

void ScopeClass::setHold(bool newhold) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    hold = newhold;
}

bool ScopeClass::triggerLocked() {
    return trigger_locked;
}

bool ScopeClass::getHold() {
    return hold;
}

// void setDispLog(bool)
// set wether display should use logarithmic frequency scale in fft mode or not
void ScopeClass::setDispLog(bool n) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    dispLog = n;
}

// bool getDispLog()
// return wether the display is logarithmic or not
bool ScopeClass::getDispLog() {
    return dispLog;
}

// unsigned int getDispFMin()
unsigned int ScopeClass::getDispFMin() {
    return dispFMin;
}

// unsigned int getDispFMax()
unsigned int ScopeClass::getDispFMax() {
    return dispFMax;
}

// void setDispFMin(unsigned int)
void ScopeClass::setDispFMin(unsigned int n) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    dispFMin = n;
}

// void setDispFMax(unsigned int)
void ScopeClass::setDispFMax(unsigned int n) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    dispFMax = n;
}


void ScopeClass::setInfoTraceName(const std::string& newname) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    // check if trace exists
    if(parentController->getTrace(newname))
        infoTraceName = newname;
}

std::string ScopeClass::getInfoTraceName() {
    return infoTraceName;
}

bool ScopeClass::getDispDb() {
    return dispDb;
}

double ScopeClass::getDispDbRef() {
    return dispDbRef;
}

void ScopeClass::setDispDb(bool n) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    dispDb = n;
}

void ScopeClass::setDispDbRef(double reference) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    dispDbRef = reference;
}

void ScopeClass::setVDiv(double voltsperdiv) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    vDiv = voltsperdiv;
}

void ScopeClass::setDbMin(double minDbs) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    dbMin = minDbs;
}

void ScopeClass::setDbMax(double maxDbs) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    dbMax = maxDbs;
}

// int storeValues(FILE *, double, double, int)
// store current displayed data to file as text
// data shall be organized such that gnuplot can handle it!
int ScopeClass::storeValues(FILE *fd, double start, double rate, int frames) {

    if(tracenum <= 0)
        return -1;
    
      
    {
        std::lock_guard<std::shared_mutex> lock(mutex);
        // print some misc information
        // switch type from mode, not from buffer!
        fprintf(fd, "# rate: %f * 1/%s, %d samples\n", rate, buf[0].getTypeString().c_str(), frames);
        // print table header
        fprintf(fd, "# Traces:\n#\t");
        for(unsigned int i = 0; i < tracenum; i ++)
            fprintf(fd, "\t\"%s\"\t", traces[i]->getName().c_str());
        fprintf(fd, "\n");
    
        // write buffer to file
        for(int i = 0; i < frames; i++) {
            double t = (double)i / rate + start;
            // write timestamp
            fprintf(fd, "%f\t", t);
    
            // write the values...
            for(unsigned int tr = 0; tr < tracenum; tr++)
                fprintf(fd, "%f\t",  buf[tr].getValue(t));
            fprintf(fd, "\n");
        }
    }
    return 0;
}


// double getMaxDspRate()
// return the maximum dsp rate of all traces in this scope.
double ScopeClass::getMaxDspRate() {
    double maxRate = 0.0;
    for(unsigned int tr = 0; tr < tracenum; tr++)
        if(buf[tr].getSampleRate() > maxRate)
            maxRate = buf[tr].getSampleRate();
    return maxRate;
}

// ToDo:
void ScopeClass::registerObserver(const ScopeObserver* newObserver){

}
void ScopeClass::removeObserver(const ScopeObserver* delObserver){

}
void ScopeClass::notifyObserver(){

} 

void ScopeClass::setFont(const std::string& newfont) {
    std::lock_guard<std::shared_mutex> lock(mutex);
    font = newfont;
}

void ScopeClass::notifyTraceUpdate(const std::string& devicename) {
    // we can discard the notify if we are in hold mode.
    if(hold)
        return;

    std::shared_lock<std::shared_mutex> lock(mutex);
    {
        for(unsigned int i = 0; i < tracenum; i++)
            if(devicename == traces[i]->getParentName()) {
                // ToDo notify View
                //triggerRedrawScope();
                break;
            }
    }
}