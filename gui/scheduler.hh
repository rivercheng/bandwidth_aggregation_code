#ifndef __SCHDULER_H__
#define __SCHDULER_H__
#include <QThread>
#include <QMutexLocker>
#include <QList>
#include <QQueue>
#include <QHostAddress>
#include <QSemaphore>
#include "def.hh"

class QTimer;
class Sender;
class ActiveConnection;
class Scheduler : public QThread {
public:
    Scheduler(const QHostAddress& dstAddr, quint16 dstPort, FlowDict *dict, int max_delay, int min_delay, QObject *parent = 0);
    void run(void);
    void send(const QByteArray & packet) {
        QMutexLocker locker(&bufferMutex_);
        buffer_.enqueue(packet);
        havingPacket_.release();
    }
    void sendAll(const QByteArray& packet);
    void stop() {
        toStop = true;
    }
private:
    Sender *selectSender(void);
private:
    QQueue<QByteArray> buffer_;
    QList<Sender *>    senders_;
    QMutex             bufferMutex_;
    QMutex             sendingMutex_;
    QSemaphore         havingPacket_;
    QSemaphore         senderAvailable_;
    bool    inDropMode;
    bool    toStop;
    QList<ActiveConnection*>  activeConnections_;
    int     max_delay;
    int     min_delay;
};
#endif
