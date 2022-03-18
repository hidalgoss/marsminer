//#include "mainwindow.h"
#include "marsminermainwin.h"

#include <QApplication>

// NOTA:
// Generacion .exe sin dependencias con,
// qmake -nodepend -o Makefile MarsMiner.pro
//
// A partir de este qmake, hacer un build genera el .exe sin depend.

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MarsMinerMainWin w;
    w.show();
    
    return a.exec();
}
