#ifndef QOSCWINDOW_H
#define QOSCWINDOW_H

#include <QMainWindow>

#define VERSION "0.41"

namespace Ui {
    class QOscWindow;
}

class QOscWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit QOscWindow(QWidget *parent = 0);
    ~QOscWindow();

 //   void initActions();    
 //   void initMenuBar();

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

    //void createControls();
    //void destroyControls(); 

    std::string conffilename;
private:
    Ui::QOscWindow *ui;
};      
#endif // QOSCWINDOW_H
