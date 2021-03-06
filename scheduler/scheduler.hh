#ifndef __SCHDULER_H__
#define __SCHDULER_H__
#include <QThread>
#include <QMutexLocker>
#include <QList>
#include <QQueue>
#include <QHostAddress>
#include <QSemaphore>

class QTimer;
class Sender;
class Scheduler : public QThread {
public:
    Scheduler(const QHostAddress& dstAddr, quint16 dstPort);
    void run(void);
    void send(const QByteArray & packet) {
        QMutexLocker locker(&bufferMutex_);
        buffer_.enqueue(packet);
        havingPacket_->release();
    }
    void sendAll(const QByteArray& packet);
    void toStop() {
        stop = true;
    }
private:
    QQueue<QByteArray> buffer_;
    QList<Sender *>    senders_;
    QMutex             bufferMutex_;
    QMutex             sendingMutex_;
    QSemaphore         *havingPacket_;
    QSemaphore         *senderAvailable_;
    bool    inDropMode;
    bool    stop;
private:
    Sender *selectSender(void);
};
#endif
