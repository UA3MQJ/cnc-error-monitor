#include <QtCore/QCoreApplication>
#include <QObject>
#include "cncreceiver.h"
#include "jabberclient.h"
#include "json.h"
#include <QFile>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    CncReceiver cncReceiver;
    JabberClient jabberClient;

    QFile databaseFile("./config/jabber.json");
    databaseFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream dbFileIn(&databaseFile);
    QString jsonData = dbFileIn.readAll();
    databaseFile.close();

    bool ok = false;

    QVariantMap result = QtJson::parse(jsonData, ok).toMap();
    QString user;
    QString pass;

    if(ok) {
        foreach(QVariant key, result.keys()) {
            if(key.toString().toLower()=="user")
                user = result.value(key.toString()).toString();
            if(key.toString().toLower()=="pass")
                pass = result.value(key.toString()).toString();
        }
    }

    QObject::connect(&cncReceiver, SIGNAL(needSendToAll(QByteArray,QString,QDate,QString)),
                     &jabberClient, SLOT(sendToAll(QByteArray,QString,QDate,QString)));

    jabberClient.connectToJabber(user, pass);
    
    return a.exec();
}
