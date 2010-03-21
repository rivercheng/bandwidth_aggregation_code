#include <QUdpSocket>
#include <QTextStream>
#include <cstdlib>
#include <QTimer>
#include <QFile>
#include <QProcess>
#include "splitter.hh"

UdpSplitter::UdpSplitter(QUdpSocket *socket, QList< Tuple> outTuples)
    :udpSocket_(socket), outTuples_(outTuples), index(0) {

    foreach(Tuple tuple, outTuples_) {
        QUdpSocket *sock = new QUdpSocket(this);
        if (!sock->bind(tuple.sourceAddr, tuple.sourcePort)) {
            throw BindError();
        }
        outSockets_.append(sock);
    }

    connect(udpSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
}

void UdpSplitter::processPendingDatagrams()
{
    QTextStream cout(stdout);
    while (udpSocket_->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket_->pendingDatagramSize());
        udpSocket_->readDatagram(datagram.data(), datagram.size());
        int i = selectOutputIndex();
        outSockets_[i]->writeDatagram(datagram, \
                outTuples_[i].destAddr, outTuples_[i].destPort);
        qDebug() << "split packet to " << i;
    }
}

int UdpSplitter::selectOutputIndex() {
    if (index >= outTuples_.size()) {
        index = 0;
    }
    return index++;
}
