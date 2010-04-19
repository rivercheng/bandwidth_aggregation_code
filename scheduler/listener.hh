#ifndef __LISTENER_H__
#define __LISTENER_H__
#include <QObject>
#include <QHostAddress>
#include "chunk.hh"

class Scheduler;
class QFile;
class QUdpSocket;
class Listener : public QObject {
    Q_OBJECT
public:
    Listener(QUdpSocket *socket, QHostAddress dstAddr, quint16 dstPort, int b, int k);
private slots:
    void processPendingDatagrams();
private:
    QUdpSocket *udpSocket_;
    Scheduler *scheduler_;
    int b_;
    int k_;
    QFile *f_;
    PacketID id_;
    QVector<FECChunk*> chunks_;
};
#endif
