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
//#include "datastore.h"

ScopeControl::ScopeControl(ScopeClass *scope, QTabWidget *parent, const char *name) : QGroupBox(QString(name),parent) {
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

   /*
    miscOptsMenu->insertItem(tr("Set name"), this, SLOT(setScopeName()));
    miscOptsMenu->insertItem(tr("Delete scope"),this, SLOT(suicide()));
    miscOptsMenu->insertSeparator();
    miscOptsMenu->insertItem(tr("Export data"), this, SLOT(saveDataFile()));
    miscOptsMenu->insertSeparator();
    miscOptsMenu->insertItem(tr("Set grid color"), this, SLOT(setColGrid()));
    miscOptsMenu->insertItem(tr("Set mark color"), this, SLOT(setColMark()));
    miscOptsMenu->insertItem(tr("Set background color"), this, SLOT(setColBg()));
    miscOptsMenu->insertItem(tr("Set text color"), this, SLOT(setColText()));
    miscOptsMenu->insertSeparator();
    miscOptsMenu->insertItem(tr("Set scope font"), this, SLOT(setScopeFont()));
   */
    QPushButton *btnPopupMenu = new QPushButton(tr("Settings"), this);
    //connect(btnPopupMenu, SIGNAL(clicked()), SLOT(showMiscOptsMenu()));

    // radio-buttons for selecting mode
    QButtonGroup *modebox = new QButtonGroup( this );
    //tr("Mode"),
    modebox->setExclusive(true);
    mode_none = new QRadioButton(tr("none"), this);
    mode_yt   = new QRadioButton(tr("Y-T"), this);
    mode_xy   = new QRadioButton(tr("X-Y"), this);
    mode_fft  = new QRadioButton(tr("FFT"), this);
    //connect(mode_none, SIGNAL(clicked()), SLOT(setModeNone()));
    //connect(mode_yt,   SIGNAL(clicked()), SLOT(setModeYt()));
    //connect(mode_xy,   SIGNAL(clicked()), SLOT(setModeXy()));
    //connect(mode_fft,  SIGNAL(clicked()), SLOT(setModeFft()));

    // Trace list
    QGroupBox *traceListBox = new QGroupBox(tr("Available Traces"), this);
    tracelist = new QListWidget(traceListBox); //"traceListBox"
    // Allow selection of multiple traces
    //tracelist->setSelectionMode(QListBox::Multi);
    //connect(tracelist, SIGNAL(selectionChanged()), SLOT(setTraceList()));

    // Hold - Checkbox
    hold = new QCheckBox(tr("Hold data"), this);
    //connect(hold, SIGNAL(toggled(bool)), SLOT(setHold(bool)));    
}

ScopeControl::~ScopeControl() {}