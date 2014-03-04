#ifndef CNCRECEIVER_H
#define CNCRECEIVER_H

#include <QObject>
#include <QUdpSocket>
#include <QFile>
#include <QDate>

class CncReceiver : public QObject
{
    Q_OBJECT
public:
    explicit CncReceiver(QObject *parent = 0);
    ~CncReceiver();
    
signals:
    void needSendToAll(QByteArray msg, QString cncName, QDate errDate, QString errTime);

public slots:
private slots:
    void onReadyRead();

private:
    QString nowStr();
    QUdpSocket *udpSocket;
    QFile *file;
};

#endif // CNCRECEIVER_H
