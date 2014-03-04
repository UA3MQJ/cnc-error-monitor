#include "cncreceiver.h"
#include <QDebug>

QString CncReceiver::nowStr()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}

CncReceiver::CncReceiver(QObject *parent) :
    QObject(parent)
{
    //QFile file(logFileName);
    //file.open(QFile::WriteOnly);
    file = new QFile("rcvdata.log");
    file->open(QIODevice::WriteOnly);

    QString toLog;
    toLog = QString("\"%1\";\"Start receive\"\n").arg(nowStr());
    file->write(toLog.toLocal8Bit());
    file->close();

    qDebug() << nowStr() << "Start receive";

    udpSocket = new QUdpSocket(this);
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));

    udpSocket->bind(45000);
}

CncReceiver::~CncReceiver()
{
    file->close();
}

void CncReceiver::onReadyRead()
{
    QByteArray buffer;
    QString cncName;
    QDate date;
    QString time;
    QByteArray msg;
    QString errText;

    buffer.resize(udpSocket->pendingDatagramSize());

    udpSocket->readDatagram(buffer.data(), buffer.size());

    QDataStream in( &buffer, QIODevice::ReadWrite );
    in.setVersion(QDataStream::Qt_4_0);

    in >> cncName;
    in >> date;
    in >> time;
    in >> errText;
    in >> msg;

    qDebug() << "cncName: " << cncName;
    qDebug() << "date: " << date.toString("yyyy-MM-dd");
    qDebug() << "time: " << time;
    qDebug() << "errText: " << errText;
    qDebug() << "msg: " << msg.toHex();
    qDebug() << " ";

    file->open(QIODevice::WriteOnly|QIODevice::Append);

    QString toLog;
    toLog = QString("\"%5\";\"%1\";\"%2\";\"%3\";\"%4\";").arg(cncName)
                                           .arg(date.toString("yyyy-MM-dd"))
                                           .arg(time).arg(errText).arg(nowStr());
    file->write(toLog.toLocal8Bit());
    file->write("\"");
    file->write(msg);
    file->write("\"\n");

    file->close();

    if(errText!="ok")
        emit needSendToAll(msg, cncName, date, time);

}
