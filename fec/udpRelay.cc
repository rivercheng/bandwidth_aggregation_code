#include <QUdpSocket>
#include <QTextStream>
#include <QDebug>
#include "udpRelay.hh"
FecUdpRelay::FecUdpRelay(QHostAddress listenAddr, quint16 listenPort, QHostAddress sender, quint16 outPort, \
        int b, int k, bool encode)
    :sender_(sender), outPort_(outPort), b(b), k(k), id(0), encode(encode) {
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(listenAddr, listenPort);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

    chunks.resize(10000);
}

/*inline QByteArray wrapPacket(PacketID pid, QByteArray datagram)
{
    QByteArray header;
    QDataStream dstr(&header, QIODevice::WriteOnly);
    dstr << pid << (quint16)datagram.size();
    return header + datagram;
}


inline QByteArray wrapFecPacket(ChunkID cid, QByteArray datagram)
{
    return wrapPacket(static_cast<PacketID>(-cid), datagram);
}

inline QByteArray dewrap(QByteArray datagram)
{
    QDataStream dstr(datagram);
    int id;
    quint16 len;
    dstr >> id >> len;
    datagram.remove(0, 6);
    datagram.resize(len);
    return datagram;
}*/


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
            chunks.resize( 2* chunks.size());
        }

        if (chunks[cid] == 0) {
            chunks[cid] = new FECChunk(cid, b, k);
        }
        chunks[cid]->addPacket(id, datagram);
        
        QByteArray wrappedPacket = wrapPacket(id, datagram);
        udpSocket->writeDatagram(wrappedPacket, sender_, outPort_);
        id++;

        if (chunks[cid]->FECReady()) {
            QByteArray fecPacket = chunks[cid]->packet();
            udpSocket->writeDatagram(wrapFecPacket(cid, fecPacket), sender_, outPort_);
            //qDebug() << "sent FEC for chunk " << cid << endl;
            delete chunks[cid];
            chunks[cid] = 0;
        }
      } else {
          PacketID pid = 0;
          quint16  len = 0;
          ChunkID  cid = 0;
          QDataStream data(datagram);
          data >> pid >> len;
          //qDebug() << "received " << pid << " len " << len << endl;
          if (pid < 0) { //FEC packet
            cid = -pid - 1;
            //qDebug() << "received a FEC " << cid << endl;
          } else {
            cid = FECChunk::packetID2chunkID(pid, b, k);
          }
          while (cid >= chunks.size())
          {
              chunks.resize(2 * chunks.size());
          }
          if (chunks[cid] == 0) {
              chunks[cid] = new FECChunk(cid, b, k);
          }
          if (pid < 0) {
              chunks[cid]->addFEC(dewrap(datagram));
              if (chunks[cid] -> full()) {
                    //qDebug() << "chunk " << cid << "is full" << endl;
                  //delete chunks[cid];
                  //chunks[cid] = 0;
              }
          } else {
            QByteArray dewrapped = dewrap(datagram);
            chunks[cid]->addPacket(pid, dewrapped);
            //qDebug() << "waiting " << chunks[cid]->waiting() << " packets " << endl;
            if (chunks[cid]->full() && chunks[cid]->FECreceived())
            {
                //qDebug() << "chunk " << cid << "is full" << endl;
                //delete chunks[cid];
                //chunks[cid] = 0;
            } else {
                udpSocket->writeDatagram(dewrapped, sender_, outPort_);
            }
          }
          
          if (chunks[cid]->recoverReady()) {
              QByteArray recoveredPacket = chunks[cid]->packet();
              qDebug() << "recovered 1 packet in chunk " << cid << endl;
              udpSocket->writeDatagram(recoveredPacket, sender_, outPort_);
          }
      }
    }
}
