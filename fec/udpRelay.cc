#include <QUdpSocket>
#include <QTextStream>
#include <QDebug>
#include <QTimer>
#include <cstdlib>
#include "udpRelay.hh"
FecUdpRelay::FecUdpRelay(QHostAddress listenAddr, quint16 listenPort, QHostAddress sender, quint16 outPort, \
        int b, int k, unsigned int delay, bool encode)
    :sender_(sender), outPort_(outPort), b(b), k(k), id(0), encode(encode), \
         nextIDToSend(0), lastIDToSend(0), \
         initialTime(0,0), firstSentTime(0,0), firstToSendTime(0,0), delay(delay) {
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(listenAddr, listenPort);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

    chunks.resize(10000);
    if (!encode) {
        packetBuffer.resize(10000);
        connect(this, SIGNAL(send()), this, SLOT(sendPacket()));
    }
}


void FecUdpRelay::processPendingDatagrams()
{
    QTextStream cout(stdout);
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

      if (encode) {
        ChunkID cid = FECChunk::packetID2chunkID(id, b, k);
        while (cid >= chunks.size())
        {
            chunks.resize( 2 * chunks.size());
        }

        if (chunks[cid] == 0) {
            chunks[cid] = new FECChunk(cid, b, k);
        }
        chunks[cid]->addPacket(id, datagram);
        
        QByteArray wrappedPacket = wrapPacket(id, datagram);
        udpSocket->writeDatagram(wrappedPacket, sender_, outPort_);
        //qDebug() << "sent packet "<<id;
        id++;

        if (chunks[cid]->FECReady()) {
            QByteArray fecPacket = chunks[cid]->packet();
            udpSocket->writeDatagram(wrapFecPacket(cid, fecPacket), sender_, outPort_);
            //qDebug() << "sent FEC for chunk " << cid << endl;
            delete chunks[cid];
            chunks[cid] = 0;
        }
      } else {
          // in the decoder
          PacketID pid = 0;
          unsigned int sec = 0;
          unsigned int usec = 0;
          quint16  len = 0;
          ChunkID  cid = 0;
          QDataStream data(datagram);
          data >> pid >> sec >> usec >> len;
          PreciseTime sentTime(sec, usec);
          qDebug() << "received " << pid << " at " << sec << " " << usec;

          
          if (pid < 0) { //FEC packet
            cid = -pid - 1;
          } else {
            cid = FECChunk::packetID2chunkID(pid, b, k);
          }

          PreciseTime currTime = PreciseTime::getTime();

          if (initialTime.sec == 0 && initialTime.usec == 0) { //First packet
              initialTime = currTime; 
              firstSentTime = sentTime;
          }

          
          while (cid >= chunks.size())
          {
              chunks.resize(2 * chunks.size());
          }
          
          if (chunks[cid] == 0) {
              chunks[cid] = new FECChunk(cid, b, k);
          }
          
          if (pid < 0) {
              // FEC packet
              chunks[cid]->addFEC(dewrap(datagram));
              if (chunks[cid] -> full()) {
                  ; // TODO
              }
          } else {
              //Data Packet
            QByteArray dewrapped = dewrap(datagram);
            chunks[cid]->addPacket(pid, dewrapped);
            if (chunks[cid]->full() && chunks[cid]->FECreceived())
            {
                // All packets are received.
                // TODO
            } else {
                PreciseTime toSendTime = decideSendingTime(initialTime, delay, firstSentTime, sentTime); 
                insertPacket(pid, dewrapped, toSendTime);
            }
          }
          
          if (chunks[cid]->recoverReady()) {
              QByteArray recoveredPacket = chunks[cid]->packet();
              PacketID rpid = chunks[cid]->recoverdID();
              qDebug() << "recovered packet "<< rpid << " in chunk " << cid << endl;
              //Since we cannot know the exact sent time of the recovered packets.
              //Recovered Packets are assumed to be sent immediatelly after their previous packets.
              insertPacket(rpid, recoveredPacket, PreciseTime(0, 0));
          }
      }
    }
}

void
FecUdpRelay::insertPacket(PacketID pid, QByteArray packet, PreciseTime sendTime) {
    qDebug() << "insert " << pid << "to send at " << sendTime.sec << " " << sendTime.usec;
    while (pid >= packetBuffer.size()) {
        packetBuffer.resize(2 * packetBuffer.size());
    }
    packetBuffer[pid] = PacketToSend(packet, sendTime);
    if (pid <= nextIDToSend) {
        nextIDToSend = pid;
        firstToSendTime = sendTime;
        qDebug() << "emit send";
        emit send();
    }
    if (pid > lastIDToSend) {
        lastIDToSend = pid;
    }
}

void 
FecUdpRelay::sendPacket() {
    qDebug() << "in send";
    qDebug() << "next " << nextIDToSend << " last " << lastIDToSend;
    PreciseTime currTime = PreciseTime::getTime();
    qDebug() << "currTime " << currTime.sec << " " << currTime.usec;
    PreciseTime threshold(0,0);
    threshold.sec = currTime.sec;
    threshold.usec = currTime.usec + 1000;
    while (threshold.usec > 1000000) {
        threshold.usec -= 1000000;
        threshold.sec ++;
    }
        while (nextIDToSend <= lastIDToSend && \
                packetBuffer[nextIDToSend].sendTime <= threshold) {

            if (!packetBuffer[nextIDToSend].packet.isEmpty()) {
                udpSocket->writeDatagram(packetBuffer[nextIDToSend].packet, sender_, outPort_);
                packetBuffer[nextIDToSend].packet.clear();
                qDebug() << "sent " << nextIDToSend;
                usleep(100); //avoid bursty.
            }
            nextIDToSend++;
        }
        if (nextIDToSend <= lastIDToSend) {
            PreciseTime nextSendTime = packetBuffer[nextIDToSend].sendTime;
            currTime = PreciseTime::getTime();
            //qDebug() << "nextSendTime " << nextSendTime.sec << " " << nextSendTime.usec;
            //qDebug() << "currTime " << currTime.sec << " " << currTime.usec;
            int waittingTime = (nextSendTime.sec - currTime.sec) * 1000;
            if (nextSendTime.usec > currTime.usec) {
                waittingTime += (nextSendTime.usec - currTime.usec) / 1000;
            } else {
                waittingTime -= (currTime.usec - nextSendTime.usec) / 1000;
            }
            //qDebug() << "waittingTime " << waittingTime;
            if (waittingTime < 0) {
                waittingTime = 0;
            }
            QTimer::singleShot(waittingTime, this, SLOT(sendPacket()));
        }
}



