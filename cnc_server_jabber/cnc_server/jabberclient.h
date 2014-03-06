#ifndef JABBERCLIENT_H
#define JABBERCLIENT_H

#include <QObject>
#include <QFile>
#include <QList>
#include "QXmppClient.h"
#include "QXmppMessage.h"
#include "QXmppRosterManager.h"

class JabberClient : public QXmppClient
{
    Q_OBJECT
public:
    JabberClient(QObject *parent = 0);

public slots:
    void connectToJabber(QString user, QString pass);
    void clientConnected();
    void rosterReceived();
    void presenceChanged(const QString& bareJid, const QString& resource);
    void sendToAll(QByteArray msg, QString cncName, QDate errDate, QString errTime);
    void onMessageReceived(QXmppMessage msg);

private:
    QString nowStr();
    QFile *file;
    QList<QString> sendList;
};

#endif // JABBERCLIENT_H
