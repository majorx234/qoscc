//
// C++ Interface: scopecontrol.cpp
//
// Description: controls scopeClass
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
// Maintainer: MajorX234 <majorx234@googlemail.com>, (C) 2022               
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <string>
#include "scopecontrol.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QDialog>
#include <QComboBox>
#include <QFontDialog>
#include <QButtonGroup>
#include <QInputDialog>
#include <QLabel>
#include <QLayout>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QRadioButton>
#include <QTabWidget>
#include <QTimer>
#include <QCursor>

#include <thread>
#include <math.h>
#include <string>

// interface for controller?
#include "scopeclass.h"
#include "traceclass.h"
#include "stringlist.h"
#include "misc.h"
#include "datastoredialog.h"

ScopeControl::ScopeControl(ScopeInterface *scope,ControllerClass* parentController, QTabWidget *parent, const std::string name) 
  : QGroupBox(QString::fromStdString(name),parent)
  ,_parentController(parentController)
{
    thisscope = scope;

    miscOptsMenu = new QMenu("scopeMiscOptsMenu",this);
    QAction *set_name = new QAction("Set name",this);
    QAction *delete_scope = new QAction("Delete scope",this);
    QAction *export_data = new QAction("Export data",this);
    QAction *set_grid_color = new QAction("Set grid color",this);
    QAction *set_mark_color = new QAction("Set mark color",this);
    QAction *set_background_color = new QAction("Set background color",this);
    QAction *set_text_color = new QAction("Set text color",this);
    QAction *set_scope_font = new QAction("Set scope font",this);
    
    connect(set_name, SIGNAL(triggered()), this, SLOT(setScopeName()));
    connect(delete_scope, SIGNAL(triggered()), this, SLOT(suicide()));
    connect(export_data, SIGNAL(triggered()), this, SLOT(saveDataFile()));
    connect(set_grid_color, SIGNAL(triggered()), this, SLOT(setColGrid()));
    connect(set_mark_color, SIGNAL(triggered()), this, SLOT(setColMark()));
    connect(set_background_color, SIGNAL(triggered()), this, SLOT(setColBg()));
    connect(set_text_color, SIGNAL(triggered()), this, SLOT(setColText()));
    connect(set_scope_font, SIGNAL(triggered()), this, SLOT(setScopeFont()));

    miscOptsMenu->addAction(set_name);
    miscOptsMenu->addAction(delete_scope);
    miscOptsMenu->addSeparator();
    miscOptsMenu->addAction(export_data);
    miscOptsMenu->addSeparator();
    miscOptsMenu->addAction(set_grid_color);
    miscOptsMenu->addAction(set_mark_color);
    miscOptsMenu->addAction(set_background_color);
    miscOptsMenu->addAction(set_text_color);
    miscOptsMenu->addSeparator();
    miscOptsMenu->addAction(set_scope_font);

    QPushButton *btnPopupMenu = new QPushButton(tr("Settings"), this);
    connect(btnPopupMenu, SIGNAL(clicked()), SLOT(showMiscOptsMenu()));

    // radio-buttons for selecting mode
    QButtonGroup *modebox = new QButtonGroup( this );
    //tr("Mode"),
    modebox->setExclusive(true);
    mode_none = new QRadioButton(tr("none"), this);
    mode_yt   = new QRadioButton(tr("Y-T"), this);
    mode_xy   = new QRadioButton(tr("X-Y"), this);
    mode_fft  = new QRadioButton(tr("FFT"), this);
    connect(mode_none, SIGNAL(clicked()), SLOT(setModeNone()));
    connect(mode_yt,   SIGNAL(clicked()), SLOT(setModeYt()));
    connect(mode_xy,   SIGNAL(clicked()), SLOT(setModeXy()));
    connect(mode_fft,  SIGNAL(clicked()), SLOT(setModeFft()));

    // Trace list
    QGroupBox *traceListBox = new QGroupBox(tr("Available Traces"), this);
    tracelist = new QListWidget(traceListBox); //"traceListBox"
    // Allow selection of multiple traces
    tracelist->setSelectionMode(QAbstractItemView::ExtendedSelection);
    connect(tracelist, SIGNAL(itemSelectionChanged()), SLOT(setTraceList()));

    // Hold - Checkbox
    hold = new QCheckBox(tr("Hold data"), this);
    connect(hold, SIGNAL(toggled(bool)), SLOT(setHold(bool)));  

    // box for sweep
    boxSweep = new QGroupBox(tr("Sweep"), this);
    ytSweep = new QComboBox(boxSweep);
    ytSweep->setEditable(true);
    ytSweep->addItem("100 µs");
    ytSweep->addItem("0.5 ms");
    ytSweep->addItem("1 ms");
    ytSweep->addItem("2 ms");
    ytSweep->addItem("5 ms");
    ytSweep->addItem("10 ms");
    ytSweep->addItem("50 ms");
    ytSweep->addItem("100 ms");
    ytSweep->addItem("0.5 s");
    ytSweep->addItem("1 s");
    connect(ytSweep, SIGNAL(activated(const QString&)), SLOT(setSweep(const QString &)));

    // scaling box
    boxScaling = new QGroupBox(tr("Scaling"), this);
    dispLog = new QCheckBox(tr("logarithmic scaling"), boxScaling);
    connect(dispLog, SIGNAL(toggled(bool)), SLOT(setDispLog(bool)));
    // dB - Display - Checkbox
    dispDb = new QCheckBox(tr("dB scaling"), boxScaling);
    connect(dispDb, SIGNAL(toggled(bool)), SLOT(setDispDb(bool)));
    new QLabel(tr("Reference:"), boxScaling);
    // dB - Display Reference ComboBox
    dispDbRef = new QComboBox(boxScaling);
    dispDbRef->setEditable(true);
    dispDbRef->addItem("1 V");
    dispDbRef->addItem("774.6 mV");
    connect(dispDbRef, SIGNAL(activated(const QString&)), SLOT(setDispDbRef(const QString&)));

    // Combo Boxes for frequency range selection
    boxFRange = new QGroupBox(tr("Frequency ranges"), this);
    dispFMin = new QComboBox(boxFRange);
    dispFMin->setEditable(true);
    dispFMin->addItem("10 Hz");
    dispFMin->addItem("50 Hz");
    dispFMin->addItem("100 Hz");
    dispFMin->addItem("500 Hz");
    dispFMin->addItem("1 kHz");
    connect(dispFMin, SIGNAL(activated(const QString&)), SLOT(setDispFMin(const QString&)));
    dispFMax = new QComboBox(boxFRange);
    dispFMax->setEditable(true);
    dispFMax->addItem("100 Hz");
    dispFMax->addItem("500 Hz");
    dispFMax->addItem("1 kHz");
    dispFMax->addItem("5 kHz");
    dispFMax->addItem("20 kHz");
    connect(dispFMax, SIGNAL(activated(const QString&)), SLOT(setDispFMax(const QString&)));

    // combo box for vertical volts per div
    boxVDiv = new QGroupBox(tr("Volts per DIV"), this);
    vDiv = new QComboBox(boxVDiv);
    vDiv->setEditable(true);
    vDiv->addItem("10 mV");
    vDiv->addItem("50 mV");
    vDiv->addItem("100 mV");
    vDiv->addItem("500 mV");
    vDiv->addItem("1 V");
    vDiv->addItem("5 V");
    vDiv->addItem("10 V");
    connect(vDiv, SIGNAL(activated(const QString&)), SLOT(setVDiv(const QString&)));

    // combo boxes for db-range selection
    boxDbRange = new QGroupBox(tr("Level ranges"), this);
    dispDbMin = new QComboBox(boxDbRange);
    dispDbMin->setEditable(true);
    dispDbMin->addItem("-50 dB");
    dispDbMin->addItem("-90 dB");
    dispDbMin->addItem("-100 dB");
    dispDbMin->addItem("-150 dB");
    connect(dispDbMin, SIGNAL(activated(const QString&)), SLOT(setDispDbMin(const QString&)));
    dispDbMax = new QComboBox(boxDbRange);
    dispDbMax->setEditable(true);
    dispDbMax->addItem("-20 dB");
    dispDbMax->addItem("0 dB");
    dispDbMax->addItem("20 dB");
    connect(dispDbMax, SIGNAL(activated(const QString&)), SLOT(setDispDbMax(const QString&)));

    // trigger section
    triggerbox = new QGroupBox(tr("Trigger"), this );

    QHBoxLayout *miscoptsLayout = new QHBoxLayout;
    
    QButtonGroup *tedgebox = new QButtonGroup(this);

    tedgebox->setExclusive(true);
    
    btnTedgeNone  = new QRadioButton( tr("None"), this);
    btnTedgePositive  = new QRadioButton( tr("Positive"), this);
    btnTedgeNegative   = new QRadioButton( tr("Negative"), this);

    tedgebox->addButton(btnTedgeNone);
    tedgebox->addButton(btnTedgePositive);
    tedgebox->addButton(btnTedgeNegative);
    miscoptsLayout->addWidget(btnTedgeNone);
    miscoptsLayout->addWidget(btnTedgePositive);
    miscoptsLayout->addWidget(btnTedgeNegative);
    triggerbox->setLayout(miscoptsLayout);

    connect(btnTedgeNone,  SIGNAL(clicked()), SLOT(setTedgeNone()));
    connect(btnTedgePositive,  SIGNAL(clicked()), SLOT(setTedgePositive()));
    connect(btnTedgeNegative, SIGNAL(clicked()), SLOT(setTedgeNegative()));

    QGroupBox *trgSrcBox = new QGroupBox(tr("Source"), this);
    QHBoxLayout *trgSrcBoxLayout = new QHBoxLayout;
    // "Triggersource"
    triggerSource = new QComboBox(this);
    trgSrcBoxLayout->addWidget(triggerSource);
    trgSrcBox->setLayout(trgSrcBoxLayout);
    connect(triggerSource, SIGNAL(activated(const QString&)), SLOT(setTriggerSource(const QString&)));

    /*
    //TODO
    sldLevel = new fSlider( triggerbox );
    sldLevel->setTitle(tr("Level"));
    sldLevel->setMaxValue(1.0);
    sldLevel->setMinValue(-1.0);
    sldLevel->setInterval(80);
    connect(sldLevel, SIGNAL(valueChanged(float)), SLOT(setTriggerLevel(float)));
    */

    // XY-Mode trace selection
    globalXYBox = new QGroupBox(tr("YX - Settings"), this);
    QVBoxLayout* globalXYBoxLayout = new QVBoxLayout;
    QLabel* sourceX = new QLabel(tr("Source X"), this);
    lstXTrace = new QComboBox(this);
    connect(lstXTrace, SIGNAL(activated(const QString&)), SLOT(setXSource(const QString&)));
    QLabel* sourceY = new QLabel(tr("Source Y"), this);
    lstYTrace = new QComboBox(this);
    connect(lstYTrace, SIGNAL(activated(const QString&)), SLOT(setYSource(const QString&)));
    globalXYBoxLayout->addWidget(sourceX);
    globalXYBoxLayout->addWidget(lstXTrace);
    globalXYBoxLayout->addWidget(sourceY);
    globalXYBoxLayout->addWidget(lstXTrace);
    globalXYBox->setLayout(globalXYBoxLayout);

    // misc. info
    QGroupBox *groupMisc = new QGroupBox(tr("Display information on trace:"), this);
    QVBoxLayout *groupMiscLayout = new QVBoxLayout;
    infoTrace = new QComboBox(this);
    groupMiscLayout->addWidget(infoTrace);
    groupMisc->setLayout(groupMiscLayout);
    connect(infoTrace, SIGNAL(activated(const QString&)), SLOT(setInfoTrace(const QString&)));
    
    updateLocal();
}

ScopeControl::~ScopeControl() {}

void ScopeControl::setColGrid() {
    QColor col;
    col = QColorDialog::getColor(QString::fromStdString(thisscope->getGridCol()));
    if(col.isValid())
        thisscope->setGridCol(col.name().toStdString());
}

void ScopeControl::setColMark() {
    QColor col;
    col = QColorDialog::getColor(QString::fromStdString(thisscope->getMarkCol()));
    if(col.isValid())
        thisscope->setMarkCol(col.name().toStdString());
}

void ScopeControl::setColBg() {
    QColor col;
    col = QColorDialog::getColor(QString::fromStdString(thisscope->getBCol()));
    if(col.isValid())
        thisscope->setBCol(col.name().toStdString());
}

void ScopeControl::setColText() {
    QColor col;
    col = QColorDialog::getColor(QString::fromStdString(thisscope->getTextCol()));
    if(col.isValid())
        thisscope->setTextCol(col.name().toStdString());
}

void ScopeControl::updateLocal() {
    updateTraceList();
    updateTitle();
    updateSweep();
    updateBoxSweep();
    updateMode();
    updateTLevel();
    updateTEdge();
    updateTriggerSource();
    updateTrigger();
    updateXYSource();
    updateDispLog();
    updateDispFMin();
    updateDispFMax();
    updateInfoTrace();
    updateDispDb();
    updateDispDbRef();
    updateDispDbMin();
    updateDispDbMax();
    updateBoxFRange();
    updateBoxDbRange();
    updateBoxScaling();
    updateVDiv();
    updateBoxVDiv();
}

void ScopeControl::update() {
    thisscope->recalc_stringrefs();
    updateLocal();
}

// void updateTraceList()
// re-read trace list and fill to traceListBox
void ScopeControl::updateTraceList() {
    unsigned int i;
    stringlist liste;
    // clear list:
    tracelist->clear();

    _parentController->getTraceList(&liste);

    // add all from scopes list
    for(i = 0; i < liste.count(); i++) {
        tracelist->addItem(QString::fromStdString(liste.getString(i)));
        // check if this trace is in this scope...
        if(thisscope->getTrace(liste.getString(i)))
            tracelist->setCurrentRow(i); // set selected item true
    }
}

// void setTraceList()
// sets the trace list(thisscope->traces) to the selection of the tracelist widget
void ScopeControl::setTraceList() {
    for(unsigned int i = 0; i < tracelist->count(); i++) {
        TraceInterface *trace = _parentController->getTrace(tracelist->item(i)->text().toStdString());
        if(!trace) {  // skip if this trace does not exist
            printf("%s::setTraceList: trace %s does not exist\n",
                   thisscope->getName().c_str(), tracelist->item(i)->text().toStdString().c_str());
            fflush(stdout);
            continue;
        }
        if(tracelist->currentRow() == i) {
            // add this trace if it doesnt exist yet
            if(!thisscope->getTrace(trace->getName()))
                thisscope->addTrace(trace);
        } else { // remove this trace if it exists
            if(thisscope->getTrace(trace->getName()))
                thisscope->removeTrace(tracelist->item(i)->text().toStdString().c_str());
        }
    }
    updateXYSource();
    updateTrigger();
    updateInfoTrace();
}

// void suicide()
// remove according scope, then this control widget
void ScopeControl::suicide() {
    _parentController->removeScope(thisscope);
    delete this;
}

// void updateTitle()
// set title according to scope name...
void ScopeControl::updateTitle() {
    QString title = QString(tr("Scope %1")).arg(QString::fromStdString(thisscope->getName()));
    emit labelChanged(this, title);
}

// void setScopeName
// asks user for new name for this scope
void ScopeControl::setScopeName() {
    bool ok = false;
    QString text = QInputDialog::getText(this,"QOscC", tr("Enter new name for scope"),
                                         QLineEdit::Normal, QString::fromStdString(thisscope->getName()), &ok);
    if(ok && !text.isEmpty()) {
        thisscope->setName(std::string(text.toStdString()));
        updateTitle();
        emit hasChanged();
    }
}

// void setSweep(float)
// set new sweep value
void ScopeControl::setSweep(const QString & text) {
    double value = stringToNum(text.toStdString());
    if(!value) {
        emit setStatus(tr("Cannot set sweep to zero!"));
        return;
    }
    thisscope->setSweep(value);
}

// void updateSweep()
// update the sweep slider
void ScopeControl::updateSweep() {
    int i;
    for(i = 0 ; i < ytSweep->count(); i++) {
        if(isNear(thisscope->getSweep(), stringToNum(ytSweep->itemText(i).toStdString()))) {
            ytSweep->setCurrentIndex(i);
            return;
        }
    }

    ytSweep->addItem(QString("%1s").arg(thisscope->getSweep()));
    ytSweep->setCurrentIndex(i);
}

// void updateBoxSweep()
// set sweep groupbox to right visibility..
void ScopeControl::updateBoxSweep() {
    switch(thisscope->getMode()) {
    case M_NONE:
        boxSweep->hide();
        break;
    case M_YT:
        boxSweep->show();
        break;
    case M_FFT:
        boxSweep->hide();
        break;
    case M_XY:
        boxSweep->hide();
        break;
    }
}

void ScopeControl::setModeNone() {
    thisscope->setMode(M_NONE);
    updateLocal();
}

void ScopeControl::setModeYt() {
    thisscope->setMode(M_YT);
    updateLocal();
}

void ScopeControl::setModeXy() {
    thisscope->setMode(M_XY);
    updateLocal();
}

void ScopeControl::setModeFft() {
    thisscope->setMode(M_FFT);
    updateLocal();
}

void ScopeControl::updateMode() {
    switch(thisscope->getMode()) {
    case M_NONE:
        mode_none->setChecked(1);
        break;
    case M_YT:
        mode_yt->setChecked(1);
        break;
    case M_XY:
        mode_xy->setChecked(1);
        break;
    case M_FFT:
        mode_fft->setChecked(1);
        break;
    }
}

void ScopeControl::setTedgeNone() {
    thisscope->setTriggerEdge(TE_NONE);
}

void ScopeControl::setTedgePositive() {
    thisscope->setTriggerEdge(TE_RISE);
}

void ScopeControl::setTedgeNegative() {
    thisscope->setTriggerEdge(TE_FALL);
}

void ScopeControl::setTriggerLevel(float val) {
    thisscope->setTriggerLevel(val);
}

void ScopeControl::updateTLevel() {
	//to do Add/Fix slider
    //sldLevel->setValue(thisscope->getTriggerLevel());
}

void ScopeControl::updateTEdge() {
    switch(thisscope->getTriggerEdge()) {
    case TE_NONE:
        btnTedgeNone->setChecked(1);
        break;
    case TE_RISE:
        btnTedgePositive->setChecked(1);
        break;
    case TE_FALL:
        btnTedgeNegative->setChecked(1);
        break;
    }
}

void ScopeControl::setTriggerSource(const QString &n) {
    thisscope->setTriggerSource(n.toStdString());
}

void ScopeControl::updateTriggerSource() {
    unsigned int i;
    stringlist liste;

    thisscope->getTraceList(&liste);

    triggerSource->clear();

    for(i = 0; i < liste.count(); i++) {
        triggerSource->addItem(QString::fromStdString(liste.getString(i)));
        if(!liste.getString(i).compare(thisscope->getTriggerName()))
            triggerSource->setCurrentIndex(i);
    }
    // set trigger source, if none was selected
    setTriggerSource(triggerSource->currentText());
}

void ScopeControl::updateTrigger() {
    updateTriggerSource();
    switch(thisscope->getMode()) {
    case M_YT:
        triggerbox->show();
        break;
    default:
        triggerbox->hide();
    }
}

void ScopeControl::updateXYSource() {
    unsigned int i;
    stringlist liste;
    thisscope->getTraceList(&liste);

    lstXTrace->clear();
    lstYTrace->clear();

    for(i = 0; i < liste.count(); i++) {
        lstXTrace->addItem(QString::fromStdString(liste.getString(i)));
        if(!liste.getString(i).compare(thisscope->getXYSourceXName()))
            lstXTrace->setCurrentIndex(i);
        lstYTrace->addItem(QString::fromStdString(liste.getString(i)));
        if(!liste.getString(i).compare(thisscope->getXYSourceYName()))
            lstYTrace->setCurrentIndex(i);
    }

    switch(thisscope->getMode()) {
    case M_XY:
        globalXYBox->show();
        break;
    default:
        globalXYBox->hide();
    }

    if(lstXTrace->count())
        setXSource(lstXTrace->currentText());
    if(lstYTrace->count())
        setYSource(lstYTrace->currentText());
}

void ScopeControl::setXSource(const QString &n) {
    thisscope->setXYSourceX(n.toStdString());
}

void ScopeControl::setYSource(const QString &n) {
    thisscope->setXYSourceY(n.toStdString());
}

void ScopeControl::setHold(bool hold) {
    thisscope->setHold(hold);
}

void ScopeControl::updateHold() {
    hold->setChecked(thisscope->getHold());
}

void ScopeControl::setDispLog(bool n) {
    thisscope->setDispLog(n);
}

void ScopeControl::updateDispLog() {
    dispLog->setChecked(thisscope->getDispLog());
}

void ScopeControl::setDispFMin(const QString & text) {
    unsigned int f = int(stringToNum(text.toStdString()) + 0.5);
    if(f >= thisscope->getDispFMax()) {
        emit setStatus(tr("Cannot set low frequency above high frequency!"));
        updateDispFMin();
        return;
    }
    thisscope->setDispFMin(f);
}

void ScopeControl::setDispFMax(const QString & text) {
    unsigned int f = int(stringToNum(text.toStdString()) + 0.5);
    if(f <= thisscope->getDispFMin()) {
        emit setStatus(tr("Cannot set high frequency below low frequency!"));
        updateDispFMax();
        return;
    }
    thisscope->setDispFMax(f);
}

void ScopeControl::updateDispFMin() {
    int i;
    for(i = 0; i < dispFMin->count(); i++) {
        if(isNear(thisscope->getDispFMin(), stringToNum(dispFMin->itemText(i).toStdString()))) {
            dispFMin->setCurrentIndex(i);
            return;
        }
    }
    // item not yet in list...
    dispFMin->addItem(QString::fromStdString(numToString(thisscope->getDispFMin())) + tr("Hz"));
    dispFMin->setCurrentIndex(i);
}

void ScopeControl::updateDispFMax() {
    int i;
    for(i = 0; i < dispFMax->count(); i++) {
        if(isNear(thisscope->getDispFMax(), stringToNum(dispFMax->itemText(i).toStdString()))) {
            dispFMax->setCurrentIndex(i);
            return;
        }
    }
    // item not yet in list...
    dispFMax->addItem(QString::fromStdString(numToString(thisscope->getDispFMax())) + tr("Hz"));
    dispFMax->setCurrentIndex(i);
}

void ScopeControl::setInfoTrace(const QString &name) {
    if(name != "")
        thisscope->setInfoTraceName(name.toStdString());
}

void ScopeControl::updateInfoTrace() {
    stringlist liste;

    thisscope->getTraceList(&liste);

    infoTrace->clear();
    infoTrace->addItem(tr("none"));
    for(unsigned int i = 0; i < liste.count(); i++){
        infoTrace->addItem(QString::fromStdString(liste.getString(i)));
        if(liste.getString(i) == thisscope->getInfoTraceName()){
            infoTrace->setCurrentIndex(i + 1);
        }
    }
}

void ScopeControl::updateDispDb() {
    dispDb->setChecked(thisscope->getDispDb());
}

void ScopeControl::updateDispDbRef() {
    // search the list for a usable entry...
    int i; // uugh. doesnt work!!!!!!!!!!!!
    for(i = 0; i < dispDbRef->count(); i++) { // compare to millivolts
        if(isNear(stringToNum(dispDbRef->itemText(i).toStdString()), thisscope->getDispDbRef()) ) {
            dispDbRef->setCurrentIndex(i);
            return;
        }
    }
    // add one if none found and select it
    dispDbRef->addItem(QString::fromStdString(numToString(thisscope->getDispDbRef())) + tr("V"));
    dispDbRef->setCurrentIndex(i);
}

void ScopeControl::setDispDb(bool n) {
    thisscope->setDispDb(n);
    updateBoxDbRange();
    updateBoxVDiv();
}

void ScopeControl::setDispDbRef(const QString & text) {
    double number = stringToNum(text.toStdString());
    if(number == 0.0) {
        emit setStatus(tr("Cannot set zero as reference!"));
        updateDispDbRef();
        return;
    }
    thisscope->setDispDbRef(number);
}

void ScopeControl::updateDispDbMin() {
    int i;
    for(i = 0; i < dispDbMin->count(); i++) {
        if(isNear(stringToNum(dispDbMin->itemText(i).toStdString()), thisscope->getDbMin())) {
            dispDbMin->setCurrentIndex(i);
            return;
        }
    }
    dispDbMin->addItem(QString("%1dB").arg(QString::fromStdString(numToString(thisscope->getDbMin()))));
    dispDbMin->setCurrentIndex(i);
}

void ScopeControl::updateDispDbMax() {
    int i;
    for(i = 0; i < dispDbMax->count(); i++) {
        if(isNear(stringToNum(dispDbMax->itemText(i).toStdString()), thisscope->getDbMax())) {
            dispDbMax->setCurrentIndex(i);
            return;
        }
    }
    dispDbMax->addItem(QString("%1dB").arg(QString::fromStdString(numToString(thisscope->getDbMax()))));
    dispDbMax->setCurrentIndex(i);
}

void ScopeControl::setDispDbMin(const QString & t) {
    if(stringToNum(t.toStdString()) >= thisscope->getDbMax()) {
        emit setStatus(tr("Cannot set low limit above the high limit!"));
        updateDispDbMin();
        return;
    }
    thisscope->setDbMin(stringToNum(t.toStdString()));
}

void ScopeControl::setDispDbMax(const QString & t) {
    if(stringToNum(t.toStdString()) <= thisscope->getDbMin()) {
        emit setStatus(tr("Cannot set high limit below the low limit!"));
        updateDispDbMax();
        return;
    }
    thisscope->setDbMax(stringToNum(t.toStdString()));
}

void ScopeControl::updateBoxFRange() {
    switch(thisscope->getMode()) {
    case M_NONE:
        boxFRange->hide();
        break;
    case M_YT:
        boxFRange->hide();
        break;
    case M_XY:
        boxFRange->hide();
        break;
    case M_FFT:
        boxFRange->show();
        break;
    }
}

void ScopeControl::updateBoxDbRange() {
    switch(thisscope->getMode()) {
    case M_NONE:
        boxDbRange->hide();
        break;
    case M_YT:
        boxDbRange->hide();
        break;
    case M_XY:
        boxDbRange->hide();
        break;
    case M_FFT:
        if(thisscope->getDispDb())
            boxDbRange->show();
        else
            boxDbRange->hide();
        break;
    }
}

void ScopeControl::updateBoxScaling() {
    switch(thisscope->getMode()) {
    case M_NONE:
        boxScaling->hide();
        break;
    case M_YT:
        boxScaling->hide();
        break;
    case M_XY:
        boxScaling->hide();
        break;
    case M_FFT:
        boxScaling->show();
    }
}


void ScopeControl::setVDiv(const QString & text) {
    double value = stringToNum(text.toStdString());
    if(value == 0) {
        emit setStatus(tr("Cannot set scale to 0 volts per DIV!"));
        return;
    }
    thisscope->setVDiv(value);
}

void ScopeControl::updateVDiv() {
    int i;
    for(i = 0; i < vDiv->count(); i++) {
        if(isNear(stringToNum(vDiv->itemText(i).toStdString()), thisscope->getVDiv())) {
            vDiv->setCurrentIndex(i);
            return;
        }
    }
    vDiv->addItem(QString::fromStdString(numToString(thisscope->getVDiv())+"V"));
    vDiv->setCurrentIndex(i);
}

void ScopeControl::updateBoxVDiv() {
    switch(thisscope->getMode()) {
    case M_NONE:
        boxVDiv->hide();
        break;
    case M_YT:
        boxVDiv->show();
        break;
    case M_XY:
        boxVDiv->show();
        break;
    case M_FFT:
        if(thisscope->getDispDb())
            boxVDiv->hide();
        else
            boxVDiv->show();
    }
}

void ScopeControl::showMiscOptsMenu() {
    miscOptsMenu->exec(QCursor::pos());
}

// void saveDataFile()
// store displayed data to file
void ScopeControl::saveDataFile() {
    DataStoreDialog *dl = new DataStoreDialog(this);
    
    // set default values.... (visible data at optimum sampling rate)
    switch(thisscope->getMode()){
        case M_YT:
        case M_XY:
            dl->setType(dbuffer::t);        
            break;
        case M_FFT:
            dl->setType(dbuffer::f);
            break;
    }
    if(thisscope->getDispLog() && thisscope->getMode() == M_FFT){
        dl->setStart(thisscope->getDispFMin());
        dl->setEnd(thisscope->getDispFMax());
    }
    else{
        dl->setStart(0); // EEh, this does not respect any shifting in the trace....
        dl->setEnd(thisscope->getSweep() * thisscope->getHDivs());
    }
    dl->setRate(thisscope->getMaxDspRate());
    
    FILE *fd;
    if(dl->exec() == QDialog::Accepted){
        // open file
        fd = fopen(dl->getFileName().toStdString().c_str(), "w");
        if(!fd) {
            emit setStatus(tr("Could not write file!"));
            QMessageBox::warning(this, tr("QOscC - Could not open file"),
                                 tr("Could not open \"%1\" for writing.").arg(dl->getFileName()));
            return;
        }
    }
    else{  // if canceled
        emit setStatus(tr("Canceled"));
        return;
    }

    if(!thisscope->storeValues(fd, dl->getStart(), dl->getRate(), dl->getNSamples()))
        emit setStatus(tr("Datafile %1 successfully written").arg(dl->getFileName()));
    else
        emit setStatus(tr("Writing datafile %1 failed!").arg(dl->getFileName()));
    
    fclose(fd);
    
    delete dl; 
}

// void setScopeFont()
// poup dialog to select a font for the scope window
void ScopeControl::setScopeFont(){
    bool ok;
    QFont font;
    font.fromString(QString::fromStdString(thisscope->getFont()));
    font = QFontDialog::getFont(&ok, font);
    if(ok)
        thisscope->setFont(font.toString().toStdString());
}
