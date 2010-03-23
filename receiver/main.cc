#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QUdpSocket>
#include <QDateTime>
#include "decoder.hh"
#include "listener.hh"

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << " <inPort> <outPort> <outAddr> <b1> <k1> <b2> <k2> [delay1: 500ms] [delay2: 500ms" \
        << endl;
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

    int delay1 = 500;
    if (args.size() > 8) {
        bool ok;
        delay1 = args[8].toInt(&ok);
        if (!ok) {
            usage(args);
            cerr << args[8] << " is not a valid delay value." <<endl;
            return 1;
        }
    }

    int delay2 = 500;
    if (args.size() > 9) {
        bool ok;
        delay2 = args[9].toInt(&ok);
        if (!ok) {
            usage(args);
            cerr << args[9] << " is not a valid delay value." <<endl;
            return 1;
        }
    }
    QUdpSocket *sock = new QUdpSocket();
    if (!sock->bind(QHostAddress(QHostAddress::LocalHost), quint16(0))) {
        cerr << "bind error" << endl;
        return(1);
    }
    UdpDecoder decoder(sock, outAddr, outPort, b1, k1, delay1);
    
    quint16 p = sock->localPort();
    
    QUdpSocket *inSock = new QUdpSocket();
    if (!inSock->bind(QHostAddress::Any, inPort)) {
        cerr << "bind error" << endl;
        return(1);
    }

    QString filename;
    QTextStream fs(&filename);
    fs << "incoming_record_" << QDateTime::currentDateTime().toString("yy:MM:dd:hh:mm:ss");
    
    QFile rf(filename);
    if (! rf.open(QIODevice::WriteOnly)) {
        cerr << "cannot open file " << filename << endl;
        return 1;
    }
    
    UdpListener listener(inSock, QHostAddress::LocalHost, p, b2, k2, delay2, &rf);
    
    app.exec();
    return 0;
}

