#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString target = "10.157.6.141:5000";

    if (argc == 2)
        target = argv[1];

    MainWindow w(0, target);
    w.show();

    return a.exec();
}
