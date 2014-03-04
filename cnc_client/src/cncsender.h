#ifndef REFRESHDB_H
#define REFRESHDB_H

#include <QObject>
#include <QList>
#include <QUdpSocket>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QTimer>

class CncDataSender : public QObject
{
    Q_OBJECT
public:
    explicit CncDataSender(QObject *parent = 0);
    ~CncDataSender();
    
signals:
    
public slots:

private slots:
    void onTimeout();

private:
    QString nowStr();
    void writeToConsole(QString logMsg);
    void checkToSend(QSqlQuery *query);
    QList<QString> msgList;
    QUdpSocket *udpSocket;
    QTimer *refreshTimer;
    QList<QByteArray> errorList;
    QString cncName;
};

#endif // REFRESHDB_H
