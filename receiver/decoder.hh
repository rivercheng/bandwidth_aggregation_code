#ifndef __UDP_DECODER_H__
#define __UDP_DECODER_H__
#include <QObject>
#include <QVector>
#include <QHostAddress>
#include "chunk.hh"
#include "gettime.hh"
#include "header.hh"

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
class UdpDecoder : public QObject {
    Q_OBJECT
public:
    UdpDecoder(QUdpSocket *socket, QHostAddress outAddr, \
            quint16 outPort, int b, int k, unsigned int delay);
private slots:
    void processPendingDatagrams();
    void sendPacket();
signals:
    void send();
private:
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
    QUdpSocket *udpSocket_;
    QHostAddress outAddr_;
    quint16      outPort_;
    int b_;
    int k_;
    PacketID id_;
    QVector<FECChunk*> chunks_;
    QVector<PacketToSend> packetBuffer_;
    int nextIDToSend_;
    int lastIDToSend_;
    PreciseTime initialTime_;
    PreciseTime firstSentTime_;
    PreciseTime firstToSendTime_;
    unsigned int delay_;
};
#endif
