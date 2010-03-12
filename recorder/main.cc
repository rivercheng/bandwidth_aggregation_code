#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QUdpSocket>
#include <QCoreApplication>
#include <QObject>
#include <QHostAddress>

#include "recorder.hh"

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << " [addr] [listenPort] [RelayPort] [RelayAddr] " << endl;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    QHostAddress listenAddr = QHostAddress::Any;
    quint16      listenPort = 10000;
    QHostAddress relayAddr = QHostAddress::LocalHost;
    quint16      relayPort = 0;
    if (args.size() > 1) {
        listenAddr = QHostAddress(args[1]);
        if (listenAddr.isNull()) {
            cerr << "Not a valid address: " << args[1] << endl;
            return 1;
        }
    }
    
    if (args.size() > 2) {
        bool ok;
        listenPort = args[2].toInt(&ok);
        if (!ok) {
            usage(args);
            return 1;
        }
    }

    if (args.size() > 3) {
        bool ok;
        relayPort = args[3].toInt(&ok);
        if (!ok) {
            usage(args);
            return 1;
        }
    }

    if (args.size() > 4) {
        relayAddr = QHostAddress(args[4]);
        if (relayAddr.isNull()) {
            cerr << "Not a valid address: " << args[4] << endl;
            return 1;
        }
    }

    UdpRecorder rec(&app, listenAddr, listenPort, relayAddr, relayPort);
    app.exec();
}








