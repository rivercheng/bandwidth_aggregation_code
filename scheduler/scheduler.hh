#ifndef __SCHDULER_H__
#define __SCHDULER_H__
#include <QThread>
#include <QMutexLocker>
#include <QList>
#include <QQueue>
#include <QHostAddress>
#include <QSemaphore>

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
private:
    QQueue<QByteArray> buffer_;
    QList<Sender *>    senders_;
    QMutex             bufferMutex_;
    QSemaphore         *havingPacket_;
    QSemaphore         *senderAvailable_;
private:
    Sender *selectSender(void);
};
#endif
