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
        if(result.contains("user")) {
            user = result.value("user").toString();
        }
        if(result.contains("pass")) {
            pass = result.value("pass").toString();
        }
    }

    QObject::connect(&cncReceiver, SIGNAL(needSendToAll(QByteArray,QString,QDate,QString)),
                     &jabberClient, SLOT(sendToAll(QByteArray,QString,QDate,QString)));

    jabberClient.connectToJabber(user, pass);
    
    return a.exec();
}
