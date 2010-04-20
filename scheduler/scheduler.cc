#include "scheduler.hh"
#include "listAddr.hh"
#include "header.hh"
#include "gettime.hh"
#include "sender.hh"
const int MAX_DELAY = 2;
const int MIN_DELAY = 1; 

Scheduler::Scheduler(const QHostAddress& dstAddr, quint16 dstPort) 
    :inDropMode(false)
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
        QMutexLocker locker(&bufferMutex_);

        //ignore the late packet
        QByteArray packet(buffer_.dequeue());
        PacketInfo info = packetInfo(packet);
        PreciseTime diff =  PreciseTime::getTime() - PreciseTime(info.sec, info.usec);
        if (inDropMode) {
            if (diff > PreciseTime(MIN_DELAY, 0)) {
                continue;
            } else {
                inDropMode = false;
            }
        } else {
            if (diff > PreciseTime(MAX_DELAY, 0)) {
                inDropMode = true;
                continue;
            }
        }
        
        QMutexLocker lock(&sendingMutex_); //the selecting and sending cannot be interrupted by sendAll()
        senderAvailable_->acquire();
        Sender *sender = selectSender();

        sender->send(packet);
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
            senderAvailable_->acquire(); //use an available sender
            sender->send(packet);
        }
    }
}



