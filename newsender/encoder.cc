#include <QUdpSocket>
#include <QTextStream>
#include <QDebug>
#include <QTimer>
#include <QStringList>
#include <QProcess>
#include <QFile>
#include <QThreadPool>
#include <QCoreApplication>
#include "encoder.hh"
#include "header.hh"
#include "checksocket.hh"
class SendingThread : public QRunnable {
    public:
        SendingThread(CheckSocket* sock, const QByteArray & packet)
            :sock_(sock), packet_(packet) {;}
        void run();
    private:
        CheckSocket *sock_;
        QByteArray    packet_;
};

void SendingThread::run() {
    int res = sock_->sendData(packet_);
    if (res == -1) {
         qDebug() << "sent error";
    } 
    else if (res < packet_.size()) {
         qDebug() << "Cannot send all";
    }
}

UdpEncoder::UdpEncoder(QUdpSocket *sock, QList<CheckSocket *>socks, int b, int k, QFile *f)
    :udpSocket_(sock), socks_(socks), b_(b), k_(k), records_(f), id_(0)
{
    chunks_.resize(10000);
    connect(udpSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    
    reset_timer = new QTimer();
    connect(reset_timer, SIGNAL(timeout()), this, SLOT(reset()));
    reset_timer->start(10000);
}

void UdpEncoder::reset() {
    id_ = 0;
    for (int i=0; i < chunks_.size(); i++)
    {
        delete chunks_[i];
        chunks_[i] = 0;
    }
}

void UdpEncoder::sendPacket(const QByteArray & packet) {
    bool sent = false;
    while (!sent) {
        foreach(CheckSocket* sock, socks_) {
            if (sock->isAvailable()) {
                //SendingThread *thread = new SendingThread(sock, packet);
                //QThreadPool::globalInstance()->start(thread);
                sock->sendData(packet);
                sent = true;
            }
        }
        QCoreApplication::processEvents();
    }
}


void UdpEncoder::processPendingDatagrams()
{
    while (udpSocket_->hasPendingDatagrams()) {
        reset_timer->start(10000);
        QByteArray datagram;
        datagram.resize(udpSocket_->pendingDatagramSize());
        udpSocket_->readDatagram(datagram.data(), datagram.size());
        if (b_ * k_ == 0) { 
            //no fec
            QByteArray wrappedPacket(wrapPacket(id_, datagram));
            sendPacket(wrappedPacket);
            if (records_ != 0) {
                recording(id_);
            }
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
        sendPacket(wrappedPacket);
        if (records_ != 0) {
            recording(id_);
        }

        if (chunks_[cid]->FECReady()) {
            QByteArray fecPacket = chunks_[cid]->packet();
            sendPacket(wrapFecPacket(cid, fecPacket));
            if (records_ != 0) {
                recordingFEC(cid);
            }
            delete chunks_[cid];
            chunks_[cid] = 0;
        }
    }
}

void UdpEncoder::recording(PacketID id) {
    QTextStream ts(records_);
    PreciseTime currTime = PreciseTime::getTime();
    ts <<  currTime.sec << " " << currTime.usec << " " << udpSocket_->localPort() << " " << id << "\n";
    return;
}

void UdpEncoder::recordingFEC(ChunkID cid) {
    QTextStream ts(records_);
    PreciseTime currTime = PreciseTime::getTime();
    ts <<  currTime.sec << " " << currTime.usec << " " << udpSocket_->localPort() << " fec " << cid << "\n";
    return;
}



