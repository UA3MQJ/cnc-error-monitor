#include <QCoreApplication>
#include "cncsender.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CncDataSender cncSender;

    return a.exec();
}
