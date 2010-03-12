#include <QUdpSocket>
#include <QTextStream>
#include "udpRelay.hh"
UdpRelay::UdpRelay(quint16 listenPort, QHostAddress sender, quint16 outPort, \
                   QVector<PacketInfo> infos)
    :sender_(sender), outPort_(outPort), infos_(infos), id(0) {
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(listenPort);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

void UdpRelay::processPendingDatagrams()
{
    QTextStream cout(stdout);
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        //QDataStream data(datagram);
        //qint32 id = 0;
        //data >> id;
        //if (id >= 0) {
            if (id >= infos_.size())
            {
                id = id - id / infos_.size() * infos_.size();
            }
            PacketInfo info = infos_[id];
            id++;
            //cout << id << " " << info.id << " " << info.status << "\n";
            if (!info.status) {
                cout << id << " " << info.id << " " << info.status << "\n";
                continue;
            }
        //}
        udpSocket->writeDatagram(datagram, sender_, outPort_);
    }
}






