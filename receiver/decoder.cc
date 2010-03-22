#include <QUdpSocket>
#include <QTextStream>
#include <QDebug>
#include <QTimer>
#include <cstdlib>
#include "decoder.hh"
UdpDecoder::UdpDecoder(QUdpSocket *socket, QHostAddress outAddr, quint16 outPort, \
        int b, int k, unsigned int delay)
    :udpSocket_(socket), outAddr_(outAddr), outPort_(outPort), \
         b_(b), k_(k), \
         nextIDToSend_(0), lastIDToSend_(0), \
         initialTime_(0,0), firstSentTime_(0,0), \
         firstToSendTime_(0,0), delay_(delay), timer_(0) {

    connect(udpSocket_, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    
    chunks_.resize(10000);
    packetBuffer_.resize(10000);
    connect(this, SIGNAL(send()), this, SLOT(sendPacket()));

    timer_ = new QTimer();
    connect(timer_, SIGNAL(timeout()), this, SLOT(reset()));
    timer_->start(2000);

}

void UdpDecoder::reset() {
    for (int i = 0; i < chunks_.size(); i++) {
        delete chunks_[i];
        chunks_[i] = 0;
    }
    packetBuffer_.clear();
    packetBuffer_.resize(10000);
    nextIDToSend_ = 0;
    lastIDToSend_ = 0;
    initialTime_.sec = 0;
    initialTime_.usec = 0;
    firstSentTime_.sec = 0;
    firstSentTime_.usec = 0;
    firstToSendTime_.sec = 0;
    firstToSendTime_.usec = 0;
}

void UdpDecoder::processPendingDatagrams()
{
    QTextStream cout(stdout);
    timer_->start(2000);
    while (udpSocket_->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket_->pendingDatagramSize());
        udpSocket_->readDatagram(datagram.data(), datagram.size());

          // in the decoder
          PacketID pid = 0;
          unsigned int sec = 0;
          unsigned int usec = 0;
          quint16  len = 0;
          ChunkID  cid = 0;
          QDataStream data(datagram);
          data >> pid >> sec >> usec >> len;
          PreciseTime sentTime(sec, usec);
          qDebug() << udpSocket_->localPort() << " received " << pid << " at " << sec << " " << usec;

          
          if (pid < 0) { //FEC packet
            cid = -pid - 1;
          } else {
            cid = FECChunk::packetID2chunkID(pid, b_, k_);
          }

          PreciseTime currTime = PreciseTime::getTime();

          if (initialTime_.sec == 0 && initialTime_.usec == 0) { //First packet
              initialTime_ = currTime; 
              firstSentTime_ = sentTime;
          }

          
          while (cid >= chunks_.size())
          {
              chunks_.resize(2 * chunks_.size());
          }
          
          if (chunks_[cid] == 0) {
              chunks_[cid] = new FECChunk(cid, b_, k_);
          }
          
          if (pid < 0) {
              // FEC packet
              chunks_[cid]->addFEC(dewrap(datagram));
              if (chunks_[cid] -> full()) {
                  ; // TODO
              }
          } else {
              //Data Packet
            QByteArray dewrapped = dewrap(datagram);
            chunks_[cid]->addPacket(pid, dewrapped);
            if (chunks_[cid]->full() && chunks_[cid]->FECreceived())
            {
                // All packets are received.
                // TODO
            } else {
                PreciseTime d(0, delay_*1000);
                PreciseTime toSendTime = initialTime_ + d + sentTime - firstSentTime_; 
                insertPacket(pid, dewrapped, toSendTime);
            }
          }
          
          if (chunks_[cid]->recoverReady()) {
              QByteArray recoveredPacket = chunks_[cid]->packet();
              PacketID rpid = chunks_[cid]->recoverdID();
              qDebug() << udpSocket_->localPort() << " recovered packet "<< rpid << " in chunk " << cid << endl;
              
              //Since we cannot know the exact sent time of the recovered packets.
              //Recovered Packets are assumed to be sent immediatelly after their previous packets.
              insertPacket(rpid, recoveredPacket, PreciseTime(0, 0));
          }
    }
}

void
UdpDecoder::insertPacket(PacketID pid, QByteArray packet, PreciseTime sendTime) {
    qDebug() << udpSocket_->localPort() << " insert " << pid << "to send at " << sendTime.sec << " " << sendTime.usec;
    while (pid >= packetBuffer_.size()) {
        packetBuffer_.resize(2 * packetBuffer_.size());
    }
    packetBuffer_[pid] = PacketToSend(packet, sendTime);
    if (pid <= nextIDToSend_) {
        nextIDToSend_ = pid;
        firstToSendTime_ = sendTime;
        qDebug() << "emit send";
        emit send();
    }
    if (pid > lastIDToSend_) {
        lastIDToSend_ = pid;
    }
}

void 
UdpDecoder::sendPacket() {
    qDebug() << udpSocket_->localPort() << "in send";
    qDebug() << "next " << nextIDToSend_ << " last " << lastIDToSend_;
    PreciseTime currTime = PreciseTime::getTime();
    qDebug() << "currTime " << currTime.sec << " " << currTime.usec;
    PreciseTime threshold = currTime + PreciseTime(0, 1000);
        
    while (nextIDToSend_ <= lastIDToSend_ && \
                packetBuffer_[nextIDToSend_].sendTime <= threshold) {

            if (!packetBuffer_[nextIDToSend_].packet.isEmpty()) {
                udpSocket_->writeDatagram(packetBuffer_[nextIDToSend_].packet, outAddr_, outPort_);
                packetBuffer_[nextIDToSend_].packet.clear();
                qDebug() << "sent " << nextIDToSend_;
                usleep(100); //avoid bursty.
            }
            nextIDToSend_++;
        }
        if (nextIDToSend_ <= lastIDToSend_) {
            PreciseTime nextSendTime = packetBuffer_[nextIDToSend_].sendTime;
            currTime = PreciseTime::getTime();
            //qDebug() << "nextSendTime " << nextSendTime.sec << " " << nextSendTime.usec;
            //qDebug() << "currTime " << currTime.sec << " " << currTime.usec;
            PreciseTime waittingTime = nextSendTime - currTime;
            int waittingTime_ms = waittingTime.sec * 1000 + waittingTime.usec / 1000;
            //qDebug() << "waittingTime " << waittingTime;
            if (waittingTime_ms < 0) {
                waittingTime_ms = 0;
            }
            QTimer::singleShot(waittingTime_ms, this, SLOT(sendPacket()));
    }
}



