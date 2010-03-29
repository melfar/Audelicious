#include "audeliciousqt.h"

#include <QtGui>
#include <QtGui/QMainWindow>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(true);
    QMainWindow wnd;
    wnd.resize(800, 480);
    AudeliciousQT w(&wnd);
    w.resize(800, 480);
    wnd.setCentralWidget(&w);
    wnd.showFullScreen();
    wnd.show();

    return a.exec();
}
