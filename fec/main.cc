#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QUdpSocket>
#include <QCoreApplication>
#include <QObject>
#include <QHostAddress>

#include "udpRelay.hh"

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << "<e|d> <b> <k> [addr] [listenPort] [RelayPort] [RelayAddr] " << endl;
    cerr << "\t'e': encoder, 'd': decoder." << endl;
    cerr << "\tb: the width of FEC block, k: the height of FEC block. Both are integer." << endl;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    QHostAddress listenAddr = QHostAddress::Any;
    quint16      listenPort = 10000;
    QHostAddress relayAddr = QHostAddress::LocalHost;
    quint16      relayPort = 0;
    bool         isEncoder = true;

    if (args.size() < 4) {
        usage(args);
        return 1;
    }

    if (args[1][0] == 'e') {
        isEncoder = true;
    } else if (args[1][0] == 'd') {
        isEncoder = false;
    } else {
        usage(args);
        return 1;
    }

    bool ok1, ok2;
    int b = args[2].toInt(&ok1);
    int k = args[3].toInt(&ok2);
    if (!ok1 || !ok2) {
        usage(args);
        return 1;
    }

    if (args.size() > 4) {
        listenAddr = QHostAddress(args[4]);
        if (listenAddr.isNull()) {
            cerr << "Not a valid address: " << args[4] << endl;
            return 1;
        }
    }
    
    if (args.size() > 5) {
        bool ok;
        listenPort = args[5].toInt(&ok);
        if (!ok) {
            usage(args);
            return 1;
        }
    }

    if (args.size() > 6) {
        bool ok;
        relayPort = args[6].toInt(&ok);
        if (!ok) {
            usage(args);
            return 1;
        }
    }

    if (args.size() > 7) {
        relayAddr = QHostAddress(args[7]);
        if (relayAddr.isNull()) {
            cerr << "Not a valid address: " << args[7] << endl;
            return 1;
        }
    }

    FecUdpRelay relay(listenAddr, listenPort, relayAddr, relayPort, b, k, isEncoder);
    app.exec();
}








