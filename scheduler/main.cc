#include "listener.hh"
#include <QCoreApplication>
#include <QStringList>
#include <QTextStream>
#include <QUdpSocket>
#include <QDateTime>
#include <QFile>

QTextStream cerr(stderr);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << " <inPort> <outAddr> <outPort> <b> <k>" << endl;
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    
    if (args.size() < 5) {
        usage(args);
        return 1;
    }
    quint16 inPort  = 10000;
    quint16 outPort = 12000;
    bool ok1, ok2;
    inPort = args[1].toInt(&ok1);
    outPort = args[3].toInt(&ok2);
    if (!ok1 || !ok2) {
        usage(args);
        cerr << args[1] << " or " << args[3] << " is not a valid port number" << endl;
        return 1;
    }
    
    QHostAddress outAddr = QHostAddress(args[2]);
    if (outAddr.isNull()) {
        usage(args);
        cerr << args[2] << " is not a valid address" << endl;
        return 1;
    }
    
    int b, k;
    b = args[4].toInt(&ok1);
    k = args[5].toInt(&ok2);
    if (!ok1 || !ok2 ) {
        usage(args);
        cerr << "enter the correct b, k values" << endl;
        return 1;
    }

    QUdpSocket *sock = new QUdpSocket();
    if (!sock->bind(QHostAddress::Any, inPort)) {
        cerr << "bind error" << endl;
        return(1);
    }

    /*
    QString filename;
    QTextStream fs(&filename);
    fs << "outgoing_record_" << QDateTime::currentDateTime().toString("yy:MM:dd:hh:mm:ss");

    QFile rf(filename);
    if (! rf.open(QIODevice::WriteOnly)) {
        cerr << "cannot open file " << filename << endl;
        return 1;
    }*/

    Listener listener(sock, outAddr, outPort, b, k);
    
    app.exec();
    return 0;
}
