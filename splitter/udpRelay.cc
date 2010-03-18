#include <QUdpSocket>
#include <QTextStream>
#include <cstdlib>
#include <QTimer>
#include <QFile>
#include "udpRelay.hh"
#include "listAddr.hh"
UdpRelay::UdpRelay(quint16 listenPort, QHostAddress sender, quint16 outPort)
    :index(0), sender_(sender), outPort_(outPort) {
    udpSocket = new QUdpSocket(this);
    if (!udpSocket->bind(listenPort)) {
        throw BindError();
    }

    //read source addresses
    /*
    QFile file("addresses");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw NoAddressFile();
    }
    QTextStream in(&file);
    while ( !in.atEnd()) {
        QString addr_str = in.readLine();
        QHostAddress addr(addr_str);
        if (addr.isNull()) {
            throw WrongAddress();
        }
        QUdpSocket *sock = new QUdpSocket(this);
        if (!sock->bind(addr, 0)) {
            throw BindError();
        }
        outSockets.append(sock);
    }
    */
    QList<QHostAddress> addrs = ListAddr::validIPv4Addrs();
    foreach(QHostAddress addr, addrs) {
        QUdpSocket *sock = new QUdpSocket(this);
        if (!sock->bind(addr, 0)) {
            throw BindError();
        }
        qDebug() << addr.toString() <<" is used";
        outSockets.append(sock);
    }

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(processTimer()));
}

void UdpRelay::processPendingDatagrams()
{
    QTextStream cout(stdout);
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        QUdpSocket *outSocket = selectSocket();
        outSocket->writeDatagram(datagram, sender_, outPort_);
    }
}

QUdpSocket *UdpRelay::selectSocket() {
    if (index >= outSockets.size()) {
        index = 0;
    }
    return outSockets[index++];
}


void UdpRelay::processTimer() {
    sleep(1);
    exit(0);
}






