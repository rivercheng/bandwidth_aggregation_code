#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QUdpSocket>
#include <QDateTime>

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << " <outPort> <outAddr> " << endl;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    //Parse the command line arguments
    QStringList args = app.arguments();
    if (args.size() < 3) {
        usage(args);
        return 1;
    }
    bool ok1;
    quint16 outPort = args[1].toInt(&ok1);
    if (!ok1) {
        usage(args);
        cerr << args[1] << " is not a valid port number" << endl;
        return 1;
    }
    
    QHostAddress outAddr = QHostAddress(args[2]);
    if (outAddr.isNull()) {
        usage(args);
        cerr << args[2] << " is not a valid address" << endl;
        return 1;
    }

    QUdpSocket *sock = new QUdpSocket();

    double total = 0.;
    QTime  t;
    t.start();
    for (int i = 0; i < 10000; i++) {
        QByteArray packet(1360, 'a'); //dummy packet
        int res = sock->writeDatagram(packet, outAddr, outPort);
        while (res == -1) {
             usleep(100);
             res = sock->writeDatagram(packet, outAddr, outPort);
             //qDebug() << "send failure";
        } 
        if (res < packet.size()) {
             qDebug() << "Cannot send all";
        }
        total += res;
    }
    int ms = t.elapsed();
    qDebug() << total / ms * 8 << "Kbps per second";
    return 0;
}

