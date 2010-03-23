#ifndef __FEC_ENCODER_H__
#define __FEC_ENCODER_H__
#include <QObject>
#include <QVector>
#include <QHostAddress>
#include "chunk.hh"
#include "gettime.hh"

class QUdpSocket;
class QTimer;
class QFile;
class UdpEncoder : public QObject {
    Q_OBJECT
public:
    UdpEncoder(QUdpSocket *socket, QHostAddress outAddr, \
            quint16 outPort, int b, int k, QFile *f = 0);
private slots:
    void processPendingDatagrams();
    void processTimer();
    void reset();
private:
    void recording(PacketID);
    void recordingFEC(ChunkID);
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
    QFile *records_;
    PacketID id_;
    QVector<FECChunk*> chunks_;
    QTimer *timer;
    QTimer *reset_timer;
};
#endif
