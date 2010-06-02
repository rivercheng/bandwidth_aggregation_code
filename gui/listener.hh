#ifndef __LISTENER_H__
#define __LISTENER_H__
#include <QObject>
#include <QHostAddress>
#include "../include/chunk.hh"
#include "def.hh"

class QTimer;
class Scheduler;
class QFile;
class QUdpSocket;
class Listener : public QObject {
    Q_OBJECT
public:
    Listener(QUdpSocket *socket, QHostAddress dstAddr, quint16 dstPort, int b, int k, FlowDict *dict);
    virtual ~Listener();
private slots:
    void processPendingDatagrams();
    void reset();
    void keeplive();
private:
    QUdpSocket *udpSocket_;
    Scheduler *scheduler_;
    int b_;
    int k_;
    PacketID id_;
    QTimer *timer_;
    QTimer *resetTimer_;
    QVector<FECChunk*> chunks_;
};
#endif
