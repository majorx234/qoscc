#ifndef QOSCCWINDOW_H
#define QOSCCWINDOW_H

#include <QMainWindow>
#include "controllerclass.h"

#define VERSION "0.41"

namespace Ui {
    class QOscCWindow;
}

class QOscCWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QOscCWindow(QWidget *parent = 0);
    ~QOscCWindow();

 //   void initActions();    
 //   void initMenuBar();
    ControllerClass* controller;
public slots:  
    void slotFileOpen();  
    void slotFileQuit();
    void slotHelpAbout();
//    void slotSaveSettings();
//    void update();
signals:
    void hasChanged();   
private:
    QMenu *fileMenu;
    QMenu *helpMenu;
    QMenu *setMenu;  
    
    QAction *fileOpen;
    QAction *fileQuit;
    QAction *helpAboutApp;
    /* tool bar
    QToolBar *toolBarFile

    */
    QTabWidget *groups;

    void createControls();
    void destroyControls(); 

    std::string conffilename;

    Ui::QOscCWindow *ui;
};      
#endif // QOSCCWINDOW_H
