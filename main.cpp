#include "mainwindow.h"
#include <QApplication>
#include <QFontDatabase>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFontDatabase::addApplicationFont(":/liberation-sans.ttf");
    MainWindow w;
    QStringList arguments = a.arguments();
    if (arguments.length() > 1) {
        w.open(arguments.at(1));
    }
    w.show();
    return a.exec();
}
