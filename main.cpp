//#include "mainwindow.h"
#include "marsminermainwin.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MarsMinerMainWin w;
    w.show();
    return a.exec();
}
