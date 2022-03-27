//
// C++ Interface: scopecontrol.h
//
// Description: controls scopeClass
//
//
// Author: Sven Queisser <tincan@svenqueisser.de>, (C) 2001 - 2005
// Maintainer: MajorX234 <majorx234@googlemail.com>, (C) 2022               
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef SCOPECONTROL_H
#define SCOPECONTROL_H

#include <QGroupBox>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QTabWidget>

#include "scopeinterface.h"
#include "controllerclass.h"

//ToDo implement fslider
//#include "fslider.h"

class ScopeControl : public QGroupBox {
    Q_OBJECT
public:
    ScopeControl(ScopeInterface *scope,ControllerClass* parentController, QTabWidget *parent = 0, const std::string = "");
    ~ScopeControl();

public slots:
    void update();
signals:
    void hasChanged();
    void setStatus(const QString & text);
    void labelChanged(QWidget *, const QString &);

private:
    ScopeInterface *thisscope;
    ControllerClass *_parentController;

    QPushButton *btnColGrid;
    QPushButton *btnColMark;
    QPushButton *btnColBg;
    QListWidget *tracelist;
    QComboBox *ytSweep;
    QGroupBox *boxSweep;
    QLineEdit *freqLabel;
    QLabel *lblCursorFreq;
    QLabel *lblCursorTime;
    QRadioButton *mode_none;
    QRadioButton *mode_yt;
    QRadioButton *mode_xy;
    QRadioButton *mode_fft;

    QRadioButton *btnTedgeNone;
    QRadioButton *btnTedgePositive;
    QRadioButton *btnTedgeNegative;

//TODO
//    fSlider *sldLevel;
    QComboBox *triggerSource;
    QGroupBox *triggerbox;

    QComboBox  *lstXTrace;
    QComboBox  *lstYTrace;
    QGroupBox *globalXYBox;
    QCheckBox  *hold;
    QCheckBox  *dispLog;
    QComboBox  *dispFMin;
    QComboBox  *dispFMax;
    QComboBox  *infoTrace;
    QCheckBox *dispDb;
    QComboBox *dispDbRef;
    QComboBox *dispDbMin;
    QComboBox *dispDbMax;

    QGroupBox *boxDbRange;
    QGroupBox *boxFRange;
    QGroupBox *boxScaling;
    QComboBox  *vDiv;
    QGroupBox *boxVDiv;

    QMenu *miscOptsMenu;
private slots:
    void suicide();
    void showMiscOptsMenu();

    void saveDataFile();

    void setColGrid();
    void setColMark();
    void setColBg();
    void setColText();
    void setTraceList();
    void setScopeName();
    void setScopeFont();
    void setSweep(const std::string &);
    void setModeNone();
    void setModeYt();
    void setModeXy();
    void setModeFft();
    void setTedgeNone();
    void setTedgePositive();
    void setTedgeNegative();
    void setTriggerLevel(float val);
    void setTriggerSource(const QString &n);
    void setXSource(const QString &n);
    void setYSource(const QString &n);
    void setHold(bool);
    void setDispLog(bool);
    void setDispFMin(const QString &);
    void setDispFMax(const QString &);
    void setInfoTrace(const QString &);
    void setDispDb(bool);
    void setDispDbRef(const QString &);
    void setDispDbMin(const QString &);
    void setDispDbMax(const QString &);
    void setVDiv(const QString &);

    void updateLocal();
    void updateTraceList();
    void updateTitle();
    void updateSweep();
    void updateBoxSweep();
    void updateMode();
    void updateTLevel();
    void updateTEdge();
    void updateTriggerSource();
    void updateTrigger();
    void updateXYSource();
    void updateHold();
    void updateDispLog();
    void updateDispFMin();
    void updateDispFMax();
    void updateInfoTrace();
    void updateDispDb();
    void updateDispDbRef();
    void updateDispDbMin();
    void updateDispDbMax();
    void updateBoxFRange();
    void updateBoxDbRange();
    void updateBoxScaling();
    void updateVDiv();
    void updateBoxVDiv();
};

#endif // SCOPECONTROL_H