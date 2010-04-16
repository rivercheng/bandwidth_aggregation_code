#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QUdpSocket>
#include <QDateTime>
#include "listAddr.hh"
#include "checksocket.hh"
#include "encoder.hh"

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << " <inPort> <outAddr> <outPort> <b> <k>" << endl;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    //========================Parse the command line arguments==========================
    QStringList args = app.arguments();
    if (args.size() < 6) {
        usage(args);
        return 1;
    }

    // Parsing inPort and outPort.
    quint16 inPort  = 0;
    quint16 outPort = 0;
    bool ok1        = false;
    bool ok2        = false;
    inPort = args[1].toInt(&ok1);
    outPort = args[3].toInt(&ok2);
    if (!ok1 || !ok2) {
        usage(args);
        cerr << args[1] << " or " << args[3] << " is not a valid port number" << endl;
        return 1;
    }
    
    // Parsing the outgoing address
    QHostAddress outAddr = QHostAddress(args[2]);
    if (outAddr.isNull()) {
        usage(args);
        cerr << args[2] << " is not a valid address" << endl;
        return 1;
    }
    
    int b, k;
    b = args[4].toInt(&ok1);
    k = args[5].toInt(&ok2);
    if (!ok1 || !ok2) {
        usage(args);
        cerr << "enter the correct b, k values" << endl;
        return 1;
    }
    //===================================================================================
    
    //============================ creating log file====================================
    QString filename;
    QTextStream fs(&filename);
    fs << "outgoing_record_" << QDateTime::currentDateTime().toString("yy:MM:dd:hh:mm:ss");
    
    QFile rf(filename);
    if (! rf.open(QIODevice::WriteOnly)) {
        cerr << "cannot open file " << filename << endl;
        return 1;
    }
    //==================================================================================

    //===================================================================================
    //find out all the valid IP addresses in this computer and create UDP sockets for each address.
    //Only one IP address is used for one interface.
    QList<Interface> infs = ListAddr::validIPv4Infs();
    QList<CheckSocket *> socks;
    
    foreach(Interface inf, infs) {
        CheckSocket *sock = new CheckSocket(inf.name, inf.addr, 0, outAddr, outPort);
        qDebug() << inf.name << " " << inf.addr.toString() <<" is used";
        socks.push_back(sock);
    }
    //===================================================================================
    
    //==========================Create the listening Port=================================
    QUdpSocket *socket = new QUdpSocket();
    if (!socket->bind(QHostAddress::Any, inPort)) {
        cerr << "bind error" << endl;
        return 1;
    }
    //===================================================================================


    UdpEncoder encoder(socket, socks, b, k, &rf);
    app.exec();
    return 0;
}

