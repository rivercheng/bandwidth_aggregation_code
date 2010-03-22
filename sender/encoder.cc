#include <QUdpSocket>
#include <QTextStream>
#include <QDebug>
#include <QTimer>
#include <QStringList>
#include <QProcess>
#include "encoder.hh"
#include "header.hh"

static bool runCommand(QString command, QStringList arguments) {
    QTextStream cout(stdout);
    cout << command << " ";
    foreach (QString argument, arguments) {
        cout << argument << " ";
    }
    cout << endl;
            
    QProcess proc;
    proc.start(command, arguments);
    bool res = proc.waitForFinished();
    cout << proc.readAllStandardError();
    return res;
}

UdpEncoder::UdpEncoder(QUdpSocket *socket, \
        QHostAddress outAddr, quint16 outPort, int b, int k)
    :udpSocket_(socket), outAddr_(outAddr), outPort_(outPort), b_(b), k_(k), id_(0), timer(0) 
{
    chunks_.resize(10000);
    connect(udpSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    
    if (outAddr_ != QHostAddress::LocalHost) {
        QString command = "ping";
        QStringList arguments;
        arguments << "-I" << socket->localAddress().toString() << outAddr_.toString() <<"-c" << "3";
        runCommand(command, arguments);
    
        timer = new QTimer();
        connect(timer, SIGNAL(timeout()), this, SLOT(processTimer()));
        timer->start(1000);
    }

    reset_timer = new QTimer();
    connect(reset_timer, SIGNAL(timeout()), this, SLOT(reset()));
    reset_timer->start(2000);
}

void UdpEncoder::reset() {
    id_ = 0;
    for (int i=0; i < chunks_.size(); i++)
    {
        delete chunks_[i];
        chunks_[i] = 0;
    }
}

void UdpEncoder::processPendingDatagrams()
{
    if (timer != 0) timer->start(1000);
    reset_timer->start(2000);
    QTextStream cout(stdout);
    while (udpSocket_->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket_->pendingDatagramSize());
        udpSocket_->readDatagram(datagram.data(), datagram.size());
        if (b_ * k_ == 0) { 
            //no fec
            udpSocket_->writeDatagram(wrapPacket(id_, datagram), outAddr_, outPort_);
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
        udpSocket_->writeDatagram(wrappedPacket, outAddr_, outPort_);
        qDebug() << "send packet " << id_;
        id_++;

        if (chunks_[cid]->FECReady()) {
            QByteArray fecPacket = chunks_[cid]->packet();
            udpSocket_->writeDatagram(wrapFecPacket(cid, fecPacket), outAddr_, outPort_);
            delete chunks_[cid];
            chunks_[cid] = 0;
        }
    }
}

void UdpEncoder::processTimer() {
    QString command = "ping";
    QStringList arguments;
    arguments << "-I" << udpSocket_->localAddress().toString() << outAddr_.toString() <<"-c" << "1";
    runCommand(command, arguments);
}



