#include <QtWidgets/QApplication>
#include "qoscwindow.h"
 
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QOscWindow w;
    w.show();
 
    return a.exec();
}