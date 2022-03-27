#ifndef DATA_STORE_DIALOG_H_
#define DATA_STORE_DIALOG_H_

#include <QtWidgets>
#include "ui_datastoredialog.h"

#include "dbuffer.h"
#include "misc.h"

namespace Ui {
class DataStoreDialog;
}

class DataStoreDialog : public QDialog {
  Q_OBJECT
 private:
    dbuffer::sampleTypes type;
    Ui::DataStoreDialog *ui;
 public:
    explicit  DataStoreDialog(QWidget *parent = Q_NULLPTR);
    ~ DataStoreDialog();   
 private slots:
    void changedStart( const QString &t );
    void changedEnd( const QString &t );
    void changedRate( const QString &t );
    void changedDuration( const QString &t );
    void checkClose();
    void popupFileDialog();
public slots:
    double getRate();
    double getStart();
    int getNSamples();
    QString getFileName();
    void setDuration( double v );
    void setEnd( double v );
    void setRate( double v );
    void setSamples( double v );
    void setStart( double );
    void setType( dbuffer::sampleTypes t );
};

#endif // DATA_STORE_DIALOG_H_

