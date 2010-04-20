#include "scheduler.hh"
#include "listAddr.hh"
#include "sender.hh"
Scheduler::Scheduler(const QHostAddress& dstAddr, quint16 dstPort) 
{
    havingPacket_    = new QSemaphore();
    senderAvailable_ = new QSemaphore();
    QList<Interface> infs(ListAddr::validIPv4Infs());
    foreach(Interface inf, infs) {
        Sender *sender = new Sender(inf.name, inf.addr, 0, dstAddr, dstPort, senderAvailable_);
        sender->start();
        senders_.push_back(sender);
    }
}

void Scheduler::run() {
    while(true) {
        havingPacket_->acquire();
        QMutexLocker lock(&sendingMutex_); //the selecting and sending cannot be interrupted by sendAll()
        senderAvailable_->acquire();
        Sender *sender = selectSender();
        QMutexLocker locker(&bufferMutex_);
        sender->send(buffer_.dequeue());
    }
}

Sender *Scheduler::selectSender() {
    QList<Sender *> availableSenders;
    foreach(Sender *sender, senders_) {
        if (sender->isAvailable()) {
            availableSenders.push_back(sender);
        }
    }
    int size = availableSenders.size();
    int randomNumber = qrand();
    int choice = randomNumber - randomNumber / size * size;
    return availableSenders[choice];
}

void Scheduler::sendAll(const QByteArray& packet){
    QMutexLocker locker(&sendingMutex_);
    foreach(Sender *sender, senders_) {
        if (sender->isAvailable()) {
            sender->send(packet);
        }
    }
}



