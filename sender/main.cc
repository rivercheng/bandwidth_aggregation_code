#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QUdpSocket>
#include <QDateTime>
#include "encoder.hh"
#include "splitter.hh"
#include "listAddr.hh"

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << " <inPort> <outPort> <outAddr> <b1> <k1> <b2> <k2>" << endl;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    //Parse the command line arguments
    QStringList args = app.arguments();
    if (args.size() < 8) {
        usage(args);
        return 1;
    }
    quint16 inPort  = 10000;
    quint16 outPort = 12000;
    bool ok1, ok2;
    inPort = args[1].toInt(&ok1);
    outPort = args[2].toInt(&ok2);
    if (!ok1 || !ok2) {
        usage(args);
        cerr << args[1] << " or " << args[2] << " is not a valid port number" << endl;
        return 1;
    }
    
    QHostAddress outAddr = QHostAddress(args[3]);
    if (outAddr.isNull()) {
        usage(args);
        cerr << args[3] << " is not a valid address" << endl;
        return 1;
    }

    int b1, k1, b2, k2;
    bool ok3, ok4;
    b1 = args[4].toInt(&ok1);
    k1 = args[5].toInt(&ok2);
    b2 = args[6].toInt(&ok3);
    k2 = args[7].toInt(&ok4);
    if (!ok1 || !ok2 || !ok3 || !ok4) {
        usage(args);
        cerr << "enter the correct b, k values" << endl;
        return 1;
    }

    //find out all the valid IP addresses in this computer.
    //Only one IP address is used for one interface.
    QList<QHostAddress> addrs = ListAddr::validIPv4Addrs();

    QList<UdpEncoder *> encoders;
    QList<UdpSplitter::Tuple> tuples;
    
    QString filename;
    QTextStream fs(&filename);
    fs << "outgoing_record_" << QDateTime::currentDateTime().toString("yy:MM:dd:hh:mm:ss");
    
    QFile rf(filename);
    if (! rf.open(QIODevice::WriteOnly)) {
        cerr << "cannot open file " << filename << endl;
        return 1;
    }
    
    foreach(QHostAddress addr, addrs) {
        QUdpSocket *sock = new QUdpSocket();
        if (!sock->bind(addr, 0)) {
            cerr << "bind error" << endl;
            return 1;
        }
        qDebug() << addr.toString() <<" is used";
        UdpEncoder *encoder = new UdpEncoder(sock, outAddr, outPort, b2, k2, &rf);
        encoders.append(encoder);
        tuples.append(UdpSplitter::Tuple(QHostAddress::LocalHost, 0, \
                    addr, sock->localPort()));
    }

    QUdpSocket *sock = new QUdpSocket();
    if (!sock->bind(QHostAddress(QHostAddress::LocalHost), quint16(0))) {
        cerr << "bind error" << endl;
        return 1;
    }
    quint16 p = sock->localPort();
    UdpSplitter splitter(sock, tuples);
    
    QUdpSocket *inSock = new QUdpSocket();
    if (!inSock->bind(QHostAddress::Any, inPort)) {
        cerr << "bind error" << endl;
        return 1;
    }


    UdpEncoder encoder(inSock, QHostAddress::LocalHost, p, b1, k1);
    
    app.exec();
    return 0;
}

