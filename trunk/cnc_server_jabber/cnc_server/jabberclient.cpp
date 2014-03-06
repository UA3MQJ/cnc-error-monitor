#include "jabberclient.h"
#include <QDateTime>
#include <QDebug>
#include <QTextCodec>

QString JabberClient::nowStr()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}

JabberClient::JabberClient(QObject *parent)
    : QXmppClient(parent)
{
    file = new QFile("jabber.log");
    file->open(QIODevice::WriteOnly);

    QString toLog;
    toLog = QString("\"%1\";\"Start\"\n").arg(nowStr());
    file->write(toLog.toLocal8Bit());
    file->close();

    qDebug() << nowStr() << "Start jabber";

    connect(this, SIGNAL(connected()), SLOT(clientConnected()));
    connect(&this->rosterManager(), SIGNAL(rosterReceived()), SLOT(rosterReceived()));
    connect(&this->rosterManager(), SIGNAL(presenceChanged(QString,QString)), SLOT(presenceChanged(QString,QString)));
    connect(this, SIGNAL(messageReceived(QXmppMessage)), SLOT(onMessageReceived(QXmppMessage)));
}

void JabberClient::connectToJabber(QString user, QString pass)
{
    qDebug() << nowStr() << "CONNECT to server by" << user;
    file->open(QIODevice::WriteOnly|QIODevice::Append);
    QString toLog;
    toLog = QString("\"%1\";\"CONNECT to server\";\"%2\"\n").arg(nowStr()).arg(user);
    file->write(toLog.toLocal8Bit());
    file->close();

    this->connectToServer(user, pass);
}

void JabberClient::clientConnected()
{
    qDebug() << nowStr() << "CONNECTED";
    file->open(QIODevice::WriteOnly|QIODevice::Append);
    QString toLog;
    toLog = QString("\"%1\";\"CONNECTED\"\n").arg(nowStr());
    file->write(toLog.toLocal8Bit());
    file->close();
}

void JabberClient::rosterReceived()
{
    qDebug() << nowStr() << "Roster received";
    file->open(QIODevice::WriteOnly|QIODevice::Append);
    QString toLog;
    toLog = QString("\"%1\";\"Roster received\"\n").arg(nowStr());
    file->write(toLog.toLocal8Bit());

    foreach (const QString &bareJid, rosterManager().getRosterBareJids()) {
        QString name = rosterManager().getRosterEntry(bareJid).name();
        if(name.isEmpty())
            name = "-";
        qDebug("Roster received: %s [%s]", qPrintable(bareJid), qPrintable(name));
        toLog = QString(" %1 [%2]").arg(bareJid).arg(name);
        file->write(toLog.toLocal8Bit());
        file->write("\n");

        sendList << bareJid;

        this->sendMessage(bareJid, nowStr() + " Started");
    }

    file->close();
}

void JabberClient::presenceChanged(const QString& bareJid,
                                 const QString& resource)
{
    qDebug("example_2_rosterHandling:: Presence changed %s/%s", qPrintable(bareJid), qPrintable(resource));
}

void JabberClient::sendToAll(QByteArray msg, QString cncName, QDate errDate, QString errTime)
{
    QString messageToUser;

    QTextCodec *codec = QTextCodec::codecForName("Windows-1251");

    messageToUser = codec->toUnicode(msg);

    foreach(QString userName, sendList) {
        this->sendMessage(userName, "CNC_NAME: "+ cncName +  "\n" +
                                    errDate.toString("yyyy-MM-dd ")+ errTime + "\nMesage:\n" + messageToUser);
    }
}

void JabberClient::onMessageReceived(QXmppMessage msg)
{
    if(msg.body().length()>0) {
        qDebug() << nowStr() << "onMessageReceived" << msg.from() << msg.body();
        foreach(QString userName, sendList) {
            this->sendMessage(userName, msg.from() + ">\n" + msg.body());
        }
    }
}
