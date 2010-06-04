#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include <QUdpSocket>
#include <QDateTime>
#include "listAddr.hh"
#include "header.hh"
#include "checksocket.hh"

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << " <outAddr> <outPort> <fromAddr> [packet size = 1360] [t = 10s]" << endl;
}

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    //========================Parse the command line arguments==========================
    QStringList args = app.arguments();
    if (args.size() < 4) {
        usage(args);
        return 1;
    }
    //Parse the fromAddr. Which source IP address to use
    QHostAddress fromAddr = QHostAddress(args[3]);
    if (fromAddr.isNull()) {
        usage(args);
        cerr << args[3] << " is not a valid IP address " << endl;
        return 1;
    }

    //Parse the packet size. By default 1360, used by TS video streaming format.
    int packetSize = 1360;
    if (args.size() > 4) {
        bool ok;
        packetSize = args[4].toInt(&ok);
        if (!ok) {
            usage(args);
            cerr << args[4] << " is not a valid number" << endl;
            return 1;
        }
    }

    //Parse the duration. By default 10 seconds.
    double duration = 10.;
    if (args.size() > 5) {
        bool ok;
        duration = args[5].toDouble(&ok);
        if (!ok) {
            usage(args);
            cerr << args[5] << " is not a valid number" << endl;
            return 1;
        }
    }

    //Parse the port number
    quint16 outPort = 10000;
    bool ok;
    outPort = args[2].toInt(&ok);
    if (!ok) {
        usage(args);
        cerr << args[2] << " is not a valid port number" << endl;
        return 1;
    }
    
    //Parse the address
    QHostAddress outAddr = QHostAddress(args[1]);
    if (outAddr.isNull()) {
        usage(args);
        cerr << args[1] << " is not a valid address" << endl;
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
    QTextStream outfs(&rf);
    //==================================================================================

    //===================================================================================
    //find out all the valid IP addresses in this computer and create UDP sockets for each address.
    //Only one IP address is used for one interface.
    QList<Interface> infs = ListAddr::validIPv4Infs();
    QList<CheckSocket *> socks;
    
    foreach(Interface inf, infs) {
        if (inf.addr == fromAddr) {
            CheckSocket *sock = new CheckSocket(inf.name, inf.addr, 0, outAddr, outPort);
            qDebug() << inf.name << " " << inf.addr.toString() <<" is used";
            socks.push_back(sock);
            break;
        }
    }
    if (socks.empty()) {
        qDebug() << argv[3] << " is not an available address.";
        return 1;
    }
    //===================================================================================
    //

    QTime t;
    t.start();
    PacketID index = 0;
    while(t.elapsed() < duration * 1000) {
        QByteArray packet(packetSize - 14, 'a');
        int res = 0;
        do {
            foreach (CheckSocket *sock, socks) {
                QByteArray wrappedPacket(wrapPacket(index, packet));
                res = sock->sendData(wrappedPacket);
                if (res != -1) {
                    index++;
                    PacketInfo info(packetInfo(wrappedPacket));
                    outfs <<  info.sec << " " << info.usec << " " << info.id << "\n";
                    usleep(300);
                    break;
                }
            }
            if (res == -1) {
                QCoreApplication::processEvents();
                usleep(100);
            }
        } while (res == -1 && t.elapsed() < duration * 1000);
    }
    for (int i = 0; i < 10; i++) {
        QByteArray packet(64, 'a');
        foreach (CheckSocket *sock, socks) {
            sock->sendData(wrapPacket(-index, packet));
            QCoreApplication::processEvents();
            usleep(1000000);
        }
    }

    return 0;
}

