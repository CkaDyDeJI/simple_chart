#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    w.setGeometry(0, 0, 700, 300);
    w.show();
    w.doSomething();

    return a.exec();
}
