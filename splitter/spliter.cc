#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include "udpRelay.hh"

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << " <inPort> <outPort> <outAddr>" << endl;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    if (args.size() < 4) {
        usage(args);
        return 1;
    }
    quint16 inPort = 10000;
    quint16 outPort = 12000;
    bool ok1, ok2;
    inPort = args[1].toInt(&ok1);
    outPort = args[2].toInt(&ok2);
    if (!ok1 || !ok2) {
        usage(args);
        return 1;
    }

    QHostAddress addr=QHostAddress::LocalHost;
    addr = QHostAddress(args[3]);
    
    UdpRelay relay(inPort, addr, outPort);
    app.exec();
    return 0;
}

