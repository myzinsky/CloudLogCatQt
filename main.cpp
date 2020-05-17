#include "cloudlogcatmacos.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    CloudLogCATQt w;
    w.show();
    return a.exec();
}
