#include <QUdpSocket>
#include <QTextStream>
#include <QDebug>
#include <QTimer>
#include <cstdlib>
#include "listener.hh"
#include "decoder.hh"

UdpListener::UdpListener(QUdpSocket *socket, QHostAddress outAddr, quint16 outPort, \
        int b, int k, unsigned int delay)
    :udpSocket_(socket), outAddr_(outAddr), outPort_(outPort), \
         b_(b), k_(k), delay_(delay) {

    connect(udpSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}


void UdpListener::processPendingDatagrams()
{
    QTextStream cout(stdout);
    while (udpSocket_->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket_->pendingDatagramSize());
        QHostAddress inAddr;
        quint16      inPort;
        udpSocket_->readDatagram(datagram.data(), datagram.size(), &inAddr, &inPort);
        qDebug() << "received packet from " << inAddr.toString();

        if (decoderPortHashTable_.contains(inAddr)) {
            quint16 port = decoderPortHashTable_[inAddr];
            udpSocket_->writeDatagram(datagram, QHostAddress::LocalHost, port);
            qDebug() << "dispatched to " << port;
        } else {
            QUdpSocket *socket = new QUdpSocket();
            socket->bind(QHostAddress(QHostAddress::LocalHost), 0);
            UdpDecoder *decoder = new UdpDecoder(socket, outAddr_, outPort_, b_, k_, delay_);
            quint16 port = socket->localPort();
            decoderPortHashTable_[inAddr] = port;
            decoders_.append(decoder);
            udpSocket_->writeDatagram(datagram, QHostAddress::LocalHost, port);
            qDebug() << "created and dispatched to " << port;
        }
    }
}

