#ifndef __FEC_ENCODER_H__
#define __FEC_ENCODER_H__
#include <QObject>
#include <QVector>
#include <QHostAddress>
#include "chunk.hh"
#include "gettime.hh"

class QUdpSocket;
class QTimer;
class UdpEncoder : public QObject {
    Q_OBJECT
public:
    UdpEncoder(QUdpSocket *socket, QHostAddress outAddr, \
            quint16 outPort, int b, int k);
private slots:
    void processPendingDatagrams();
    void processTimer();
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
    QTimer *timer;
};
#endif
