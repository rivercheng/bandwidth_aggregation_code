#ifndef __UDP_RELAY_H__
#define __UDP_RELAY_H__
#include <QObject>
#include <QVector>
#include <QHostAddress>
#include "chunk.hh"

inline QByteArray wrapPacket(PacketID pid, QByteArray datagram)
{
    QByteArray header;
    QDataStream dstr(&header, QIODevice::WriteOnly);
    dstr << pid << (quint16)datagram.size();
    return header + datagram;
}

inline QByteArray wrapFecPacket(ChunkID cid, QByteArray datagram)
{
    return wrapPacket(static_cast<PacketID>(-cid-1), datagram);
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
}

class QUdpSocket;
class FecUdpRelay : public QObject {
    Q_OBJECT
public:
    FecUdpRelay(QHostAddress listenAddr, quint16 listenPort, QHostAddress sender, quint16 outPort, int b, int k, bool encode);
private slots:
    void processPendingDatagrams();
private:
    QUdpSocket *udpSocket;
    QHostAddress sender_;
    quint16      outPort_;
    int b;
    int k;
    PacketID id;
    QVector<FECChunk*> chunks;
    bool encode;
};
#endif
