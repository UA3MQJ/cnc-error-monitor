#include "cncsender.h"
#include <QDateTime>
#include <QTextStream>
#include <iostream>
#include <cstdlib>
#include <QTextCodec>
#include <QtSql>
#include "json.h"

CncDataSender::CncDataSender(QObject *parent) :
    QObject(parent)
{

    udpSocket = new QUdpSocket(this);

    QString connecionString;
    QString logFileName;
    int refreshPeriod = 0;

    QFile configFile("./config/database.json");
    configFile.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream dbFileIn(&configFile);
    QString jsonData = dbFileIn.readAll();
    configFile.close();

    QFile fileIni("./config/errorlist.ini");
    fileIni.open(QFile::ReadOnly);
    QByteArray errorRaw = fileIni.readAll();
    fileIni.close();
    errorRaw = errorRaw.replace('\n', "");
    QList<QByteArray> errorAllList = errorRaw.split('\r');
    for(int i=0;i<errorAllList.count();i++) {
        if(errorAllList.at(i).count()>0) {
            errorList << errorAllList.at(i);
        }
    }

    refreshTimer = new QTimer(this);
    refreshTimer->setSingleShot(false);
    connect(refreshTimer, SIGNAL(timeout()), this, SLOT(onTimeout()));

    bool ok = false;

    QVariantMap result = QtJson::parse(jsonData, ok).toMap();

    if(ok) {       
        foreach(QVariant key, result.keys()) {
            if(key.toString().toLower()=="connectionstring")
                connecionString = result.value(key.toString()).toString();
            if(key.toString().toLower()=="logfilename")
                logFileName = result.value(key.toString()).toString();
            if(key.toString().toLower()=="refreshperiod")
                refreshPeriod = result.value(key.toString()).toString().toInt();
            if(key.toString().toLower()=="cncname")
                cncName = result.value(key.toString()).toString();
        }
    }


    writeToConsole("CNCName=\""+cncName+"\"");
    writeToConsole("ConnecionString=\""+connecionString+"\"");
    writeToConsole("LogFileName=\""+logFileName+"\"");
    writeToConsole(QString("Errol list count = %1").arg(errorList.count()));

    QFile file(logFileName);
    file.open(QFile::WriteOnly);

    file.write(QByteArray("Read Config\n\n"));
    file.write(QByteArray("Error list:\n"));

    foreach(QByteArray str, errorList) {
        file.write(str.append("\n"));
    }

    file.write("\n\nReadDatabase:\n");

    writeToConsole("Поехали(russian text test)!");

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");

    db.setDatabaseName(connecionString);

    if(db.open())
    {
       writeToConsole("Database opened!");

       QSqlQuery query;
       query.exec("SELECT data, format(ManLog.Time,'hh:mm:ss'), mes from ManLog");

       if(query.isActive()) {

           while ( query.next() ) {

               QString stringForConsole;
               QString logDate = query.value(0).toDate().toString("yyyy-MM-dd");
               QString logTime = query.value(1).toString();
               QString logMsg = query.value(2).toString();

               stringForConsole = logDate + " " + logTime + " " + logMsg;

               writeToConsole(stringForConsole);

               QByteArray msg = query.value(2).toByteArray();

               foreach(QByteArray err, errorList) {
                   if(msg.contains(err)) {
                       msg.append("(!ERROR!)");
                   }
               }

               msg.append("\n");

               file.write(msg);

               checkToSend(&query);
           }

           refreshTimer->start(refreshPeriod);

       } else {
           writeToConsole("query not active!" + query.lastError().text());
       }

    }
    else
    {
       writeToConsole("Database Error : " + db.lastError().text());
    }


    file.close();
}

CncDataSender::~CncDataSender()
{
    refreshTimer->stop();
    this->disconnect();
}

void CncDataSender::checkToSend(QSqlQuery *query)
{
    QString stringForConsole;
    QString logDate = query->value(0).toDate().toString("yyyy-MM-dd");
    QString logTime = query->value(1).toString();
    QString logMsg = query->value(2).toString();

    stringForConsole = logDate + " " + logTime + " " + logMsg;

    if(!msgList.contains(stringForConsole)) {
        msgList << stringForConsole;

        QByteArray msg = query->value(2).toByteArray();
        QString errText="ok";

        foreach(QByteArray err, errorList) {
            if(msg.contains(err)) {
                errText = "ERROR";
            }
        }

        QByteArray datagram;
        QDataStream out( &datagram, QIODevice::ReadWrite );
        out.setVersion(QDataStream::Qt_4_0);
        out << cncName;
        out << query->value(0).toDate(); //date
        out << logTime;//time
        out << errText;
        out << msg;//bytearray

        udpSocket->writeDatagram(datagram, QHostAddress::Broadcast, 45000);
    }
}

void CncDataSender::onTimeout()
{
    writeToConsole("check for new records");

    QSqlQuery query;
    query.exec("SELECT data, format(ManLog.Time,'hh:mm:ss'), mes from ManLog");

    if(query.isActive()) {

        while ( query.next() ) {

            QString stringForConsole;
            QString logDate = query.value(0).toDate().toString("yyyy-MM-dd");
            QString logTime = query.value(1).toString();
            QString logMsg = query.value(2).toString();

            stringForConsole = logDate + " " + logTime + " " + logMsg;

            if(msgList.contains(stringForConsole)) {
                //writeToConsole("no new data");
            } else {
                writeToConsole(stringForConsole);
            }

            checkToSend(&query);
        }

    } else {
        writeToConsole("query not active!" + query.lastError().text());
    }

}

QString CncDataSender::nowStr()
{
    return QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz");
}

void CncDataSender::writeToConsole(QString logMsg)
{
    QString msg = nowStr() + " " + logMsg;
    std::string current_locale_text = msg.toLatin1().constData();
    std::cout << current_locale_text <<std::endl;
}
