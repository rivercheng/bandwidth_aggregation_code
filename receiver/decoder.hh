#ifndef __UDP_DECODER_H__
#define __UDP_DECODER_H__
#include <QObject>
#include <QVector>
#include <QHostAddress>
#include "chunk.hh"
#include "gettime.hh"
#include "header.hh"

class QUdpSocket;
class QTimer;
class UdpDecoder : public QObject {
    Q_OBJECT
public:
    UdpDecoder(QUdpSocket *socket, QHostAddress outAddr, \
            quint16 outPort, int b, int k, unsigned int delay);
private slots:
    void processPendingDatagrams();
    void sendPacket();
    void reset();
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
    QVector<FECChunk*> chunks_;
    QVector<PacketToSend> packetBuffer_;
    int nextIDToSend_;
    int lastIDToSend_;
    PreciseTime initialTime_;
    PreciseTime firstSentTime_;
    PreciseTime firstToSendTime_;
    unsigned int delay_;
    QTimer *timer_;
    bool   allSent_;
};
#endif
