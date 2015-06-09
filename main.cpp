#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QStringList arguments = a.arguments();
    if (arguments.length() > 1) {
        w.open(arguments.at(1));
    }
    w.show();
    return a.exec();
}
