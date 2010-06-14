#include <QFile>
#include <QUdpSocket>
#include <QTimer>
#include "scheduler.hh"
#include "listener.hh"
#include "../include/header.hh"
Listener::Listener(QUdpSocket *socket, QHostAddress dstAddr, quint16 dstPort, int b, int k, FlowDict *dict, int max_delay, int min_delay, QObject *parent)
    :QObject(parent), udpSocket_(socket), b_(b), k_(k), id_(0), timer_(0), resetTimer_(0)
{
    timer_ = new QTimer(this);
    connect(timer_, SIGNAL(timeout()), this, SLOT(keeplive()));

    resetTimer_ = new QTimer(this);
    connect(resetTimer_, SIGNAL(timeout()), this, SLOT(reset()));

    chunks_.resize(10000);
    scheduler_ = new Scheduler(dstAddr, dstPort, dict, max_delay, min_delay, this);
    connect(udpSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    scheduler_->start();

    for (int i = 0; i < 10; i++) {
        keeplive();
        usleep(100000);
    }
    qDebug() << "ready";
    
    timer_->start(1000);

    resetTimer_->start(3000);
}

Listener::~Listener() {
    if (scheduler_) {
        scheduler_->stop();
        scheduler_->exit();
        scheduler_->wait();
    }
}

void Listener::processPendingDatagrams()
{
    QTextStream cout(stdout);
    while (udpSocket_->hasPendingDatagrams()) {
        //qDebug() << "received packet";
        timer_->start(1000);
        resetTimer_->start(3000);
        QByteArray datagram;
        datagram.resize(udpSocket_->pendingDatagramSize());
        QHostAddress inAddr;
        quint16      inPort;
        udpSocket_->readDatagram(datagram.data(), datagram.size(), &inAddr, &inPort);
        if (b_ * k_ == 0) { 
            //no fec
            QByteArray wrappedPacket(wrapPacket(id_, datagram));
            scheduler_->send(wrappedPacket);
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
        id_++;

        if (chunks_[cid]->FECReady()) {
            usleep(1000); //to reduce bursty.
            QByteArray fecPacket = chunks_[cid]->packet();
            scheduler_->send(wrapFecPacket(cid, fecPacket));
            delete chunks_[cid];
            chunks_[cid] = 0;
        }
    }
}

void Listener::reset() {
    id_ = 0;
    for (int i = 0; i < chunks_.size(); i++) {
        delete chunks_[i];
        chunks_[i] = 0;
    }
}

void Listener::keeplive() {
    QByteArray empty;
    scheduler_->sendAll(wrapPacket(0, empty));
}

