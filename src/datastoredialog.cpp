/****************************************************************************
** datastoredialog.cpp file with implementation.
**
*****************************************************************************/
#include <QtWidgets/QPushButton>
#include <QtWidgets/QLineEdit>

#include "datastoredialog.h"

#include <qfiledialog.h>
#include <qstring.h>
#include <qmessagebox.h>

#include "misc.h"
#include "dbuffer.h"

DataStoreDialog::DataStoreDialog(QWidget *parent) 
	: QDialog(parent)
	, ui(new Ui::DataStoreDialog) 
{
	ui->setupUi(this);
    connect(ui->btnCancel, &QPushButton::clicked,
          this, &DataStoreDialog::reject);
    connect(ui->btnOk, &QPushButton::clicked,
          this, &DataStoreDialog::checkClose);
    connect(ui->startPos, &QLineEdit::textChanged,
          this, &DataStoreDialog::changedStart);
    connect(ui->endPos, &QLineEdit::textChanged,
          this, &DataStoreDialog::changedEnd);
    connect(ui->rate, &QLineEdit::textChanged,
          this, &DataStoreDialog::changedRate);
    connect(ui->duration, &QLineEdit::textChanged,
          this, &DataStoreDialog::changedDuration);    
    connect(ui->btnSelectFile, &QPushButton::clicked,
          this, &DataStoreDialog::popupFileDialog);
}


DataStoreDialog::~DataStoreDialog(){

}


void  DataStoreDialog::changedStart( const QString & t )
{
    double start = stringToNum(t.toStdString());
    double end = stringToNum(ui->endPos->text().toStdString());

    // change duration value
    setDuration(end - start);
//     duration->setText(numToString(end - start) + "s");
}



void  DataStoreDialog::changedEnd( const QString & t )
{
    double end = stringToNum(t.toStdString());
    double start = stringToNum(ui->startPos->text().toStdString());
    
//     duration->setText(numToString(end - start) + "s");
    setDuration(end - start);
}


void  DataStoreDialog::changedRate( const QString & t )
{
//     samples->setText(numToString(stringToNum(duration->text()) * stringToNum(t)));
    setSamples(stringToNum(ui->duration->text().toStdString()) * stringToNum(t.toStdString()));
}

void  DataStoreDialog::changedDuration( const QString &t )
{
    // suppose rate has changed to refresh nsamples
    changedRate(ui->rate->text());    
}

double  DataStoreDialog::getRate()
{
    return stringToNum(ui->rate->text().toStdString());
}


double  DataStoreDialog::getStart()
{
    return stringToNum(ui->startPos->text().toStdString());
}


int  DataStoreDialog::getNSamples()
{
    return int(stringToNum(ui->samples->text().toStdString()) + 0.5);
}

QString  DataStoreDialog::getFileName()
{
    return ui->fileName->text();
}

// void checkClose()
// check data and accept this dialog if ok.
void  DataStoreDialog::checkClose()
{
    if(getRate() <= 0 || 
        getNSamples() <= 0 ||
        getStart() < 0){
        QMessageBox::warning(this, tr("QOscC -- Invalid specification"),
                                   tr("The values you typed in do not make sense.\n"
                                      "Please check them and try again."));
        return;
    }
    if(ui->fileName->text().isEmpty()){
        QMessageBox::warning(this, tr("QOscC -- Invalid specification"),
                                   tr("Please specify a filename."));
        return;
    }
        
    
    accept(); // accept this dialog if all data is sensible
}


void  DataStoreDialog::setDuration( double v)
{
    QString text = QString::fromStdString(numToString(v));
    switch(type){
        case dbuffer::t:
            text.append("s");
            break;
        case dbuffer::f:
            text.append("Hz");
            break;
        case dbuffer::tnone:
            break;
    }

    ui->duration->setText(text);
}


void  DataStoreDialog::setEnd( double v)
{
    QString text = QString::fromStdString(numToString(v));
    switch(type){
        case dbuffer::t:
            text.append("s");
            break;
        case dbuffer::f:
            text.append("Hz");
            break;
        case dbuffer::tnone:
            break;
    }
    ui->endPos->setText(text);
}


void  DataStoreDialog::setRate( double v)
{
    QString text = QString::fromStdString(numToString(v));
    switch(type){
        case dbuffer::t:
            text.append("1/s");
            break;
        case dbuffer::f:
            text.append("1/Hz");
            break;
        case dbuffer::tnone:
            break;
    }

    ui->rate->setText(text);
}


void  DataStoreDialog::setSamples( double v)
{
    ui->samples->setText(QString::fromStdString(numToString(int(v + 0.5))));
}

void  DataStoreDialog::setStart(double v)
{
    QString text = QString::fromStdString(numToString(v));
    switch(type){
        case dbuffer::t:
            text.append("s");
            break;
        case dbuffer::f:
            text.append("Hz");
            break;
        case dbuffer::tnone:
            break;
    }

    ui->startPos->setText(text);
}

void  DataStoreDialog::setType( dbuffer::sampleTypes t)
{
    type = t;
}

void  DataStoreDialog::popupFileDialog()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("fileDialog"), tr("~"), tr("Data files (*.dat)"));
    if (!filename.isEmpty()) {
        if(!filename.contains(".")){
            ui->fileName->setText(filename + (".dat"));
        } else {
            ui->fileName->setText(filename);
        }
    }    
}