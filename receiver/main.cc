#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QUdpSocket>
#include "decoder.hh"
#include "listener.hh"

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << " <inPort> <outPort> <outAddr>" << endl;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    //Parse the command line arguments
    QStringList args = app.arguments();
    if (args.size() < 4) {
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

    QUdpSocket *sock = new QUdpSocket();
    if (!sock->bind(QHostAddress(QHostAddress::LocalHost), quint16(0))) {
        cerr << "bind error" << endl;
        return(1);
    }
    UdpDecoder decoder(sock, outAddr, outPort, 8, 10, 500);
    
    quint16 p = sock->localPort();
    
    QUdpSocket *inSock = new QUdpSocket();
    if (!inSock->bind(QHostAddress::Any, inPort)) {
        cerr << "bind error" << endl;
        return(1);
    }

    UdpListener listener(inSock, QHostAddress::LocalHost, p, 8, 10, 500);
    
    app.exec();
    return 0;
}

