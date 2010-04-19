#include <QFile>
#include <QUdpSocket>
#include "scheduler.hh"
#include "listener.hh"
#include "header.hh"
Listener::Listener(QUdpSocket *socket, QHostAddress dstAddr, quint16 dstPort, int b, int k)
    :udpSocket_(socket), b_(b), k_(k), id_(0)
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
        if (b_ * k_ == 0) { 
            //no fec
            QByteArray wrappedPacket(wrapPacket(id_, datagram));
            scheduler_->send(datagram);
            id_++;
            continue;
        } 
        
        ChunkID cid = FECChunk::packetID2chunkID(id_, b_, k_);
        while (cid >= chunks_.size())
        {
            chunks_.resize( 2 * chunks_.size());
        }

        if (chunks_[cid] == 0) {
            chunks_[cid] = new FECChunk(cid, b_, k_);
        }
        chunks_[cid]->addPacket(id_, datagram);
        
        QByteArray wrappedPacket = wrapPacket(id_, datagram);
        scheduler_->send(wrappedPacket);

        if (chunks_[cid]->FECReady()) {
            QByteArray fecPacket = chunks_[cid]->packet();
            scheduler_->send(wrapFecPacket(cid, fecPacket));
            delete chunks_[cid];
            chunks_[cid] = 0;
        }
    }
}
