#include <QFile>
#include <QUdpSocket>
#include "scheduler.hh"
#include "listener.hh"
Listener::Listener(QUdpSocket *socket, QHostAddress dstAddr, quint16 dstPort, int b, int k, QFile *f)
    :udpSocket_(socket), b_(b), k_(k), f_(f)
{
    scheduler_ = new Scheduler(dstAddr, dstPort);
    connect(udpSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    scheduler_->start();
}

void Listener::processPendingDatagrams()
{
    QTextStream cout(stdout);
    while (udpSocket_->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket_->pendingDatagramSize());
        QHostAddress inAddr;
        quint16      inPort;
        udpSocket_->readDatagram(datagram.data(), datagram.size(), &inAddr, &inPort);
        scheduler_->send(datagram);
    }
}
