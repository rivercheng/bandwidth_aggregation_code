#ifndef __UDP_RELAY_H__
#define __UDP_RELAY_H__
#include <QObject>
#include <QVector>
#include <QHostAddress>
#include "chunk.hh"
#include "gettime.hh"

inline QByteArray wrapPacket(PacketID pid, QByteArray datagram)
{
    QByteArray header;
    QDataStream dstr(&header, QIODevice::WriteOnly);
    PreciseTime t = PreciseTime::getTime();
    dstr << pid << t.sec << t.usec << (quint16)datagram.size();
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
    unsigned int sec;
    unsigned int usec;
    dstr >> id >> sec >> usec >> len;
    datagram.remove(0, 14);
    datagram.resize(len);
    return datagram;
}

inline PreciseTime decideSendingTime(PreciseTime initialTime, unsigned int delay, \
        PreciseTime firstSentTime, PreciseTime sentTime) {
    unsigned int sec;
    int msec;
    sec = sentTime.sec - firstSentTime.sec + initialTime.sec;
    msec = int(initialTime.usec/1000) + int(sentTime.usec/1000) - int(firstSentTime.usec/1000) + delay;
    while (msec < 0) {
        sec --;
        msec += 1000;
    }
    while (msec > 1000) {
        sec ++;
        msec -= 1000;
    }
    return PreciseTime(sec, msec*1000);
}

class QUdpSocket;
class FecUdpRelay : public QObject {
    Q_OBJECT
public:
    FecUdpRelay(QHostAddress listenAddr, quint16 listenPort, QHostAddress sender, \
            quint16 outPort, int b, int k, unsigned int delay, bool encode);
private slots:
    void processPendingDatagrams();
    void sendPacket();
signals:
    void send();
private:
    void sendUntil(PacketID pid);
    void insertPacket(PacketID pid, QByteArray packet, PreciseTime toSendTime);
private:
    struct PacketToSend {
        QByteArray packet;
        PreciseTime sendTime;
        PacketToSend()
            :sendTime(0, 0) {;}
        PacketToSend(QByteArray p, PreciseTime t)
            :packet(p), sendTime(t) {;}
    };
    QUdpSocket *udpSocket;
    QHostAddress sender_;
    quint16      outPort_;
    int b;
    int k;
    PacketID id;
    QVector<FECChunk*> chunks;
    bool encode;
    QVector<PacketToSend> packetBuffer;
    int nextIDToSend;
    int lastIDToSend;
    PreciseTime initialTime;
    PreciseTime firstSentTime;
    PreciseTime firstToSendTime;
    unsigned int delay;
};
#endif
