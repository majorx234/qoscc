#include <QtWidgets/QApplication>
#include <QMainWindow>
#include <QSlider>
#include <QLayout>
#include "fslider.h"
 
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    fSlider* test_fslider = new fSlider("testslider");
    QMainWindow w;

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(test_fslider);

    // Set layout in QWidget
    QWidget *slider_container = new QWidget();
    slider_container->setLayout(layout);

    w.setCentralWidget(slider_container);
    w.show();
 
    return a.exec();
}