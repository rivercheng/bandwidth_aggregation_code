#include "scheduler.hh"
#include "listAddr.hh"
#include "../include/header.hh"
#include "../include/gettime.hh"
#include "connection.hh"
#include "sender.hh"
#include "def.hh"
const int MAX_DELAY = 2;
const int MIN_DELAY = 1; 

Scheduler::Scheduler(const QHostAddress& dstAddr, quint16 dstPort, FlowDict *dict, QObject *parent) 
    :QThread(parent), inDropMode(false), toStop(false)
{
    QList<Interface> infs(ListAddr::validIPv4Infs());
    foreach(Interface inf, infs) {
        Sender *sender = new Sender(inf.name, inf.addr, 0, dstAddr, dstPort, &senderAvailable_, dict);
        sender->start();
        senders_.push_back(sender);
    }
    findActiveConnections();
}

void Scheduler::run() {
    while(!toStop) {
        while(!toStop) {
            havingPacket_.tryAcquire(1, 100);
        }
        if (toStop) break;
        QByteArray packet;
        {
            QMutexLocker locker(&bufferMutex_);
            packet = buffer_.dequeue();
        }

        //ignore the late packet
        //QByteArray packet(buffer_.dequeue());
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
        while(!toStop) {
            senderAvailable_.tryAcquire(1, 100);
        }
        if (toStop) break;
        Sender *sender = selectSender();

        sender->send(packet);
    }
    return;
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
            senderAvailable_.acquire(); //use an available sender
            sender->send(packet);
        }
    }
}

void Scheduler::findActiveConnections() {
    QString service("org.freedesktop.NetworkManager"); 
    QString path("/org/freedesktop/NetworkManager");
    QString iface("org.freedesktop.NetworkManager");

    QDBusInterface nm(service, path, iface, QDBusConnection::systemBus());
    if (!nm.isValid()) {
        qDebug() << "invalid interface";
        exit(1);
    }

    QVariant activeConns = nm.property("ActiveConnections");
    if (activeConns.isValid()) {
        QList<QDBusObjectPath> actives = activeConns.value<QList<QDBusObjectPath> >();
        foreach(QDBusObjectPath p, actives) {
            ActiveConnection* activeConnection = new ActiveConnection(p);
            connect(activeConnection, SIGNAL(IpChanged(QHostAddress, QHostAddress)), this, SLOT(updateSenderIp(QHostAddress, QHostAddress)));
            activeConnections_.push_back(activeConnection);
        }
    } else {
        qDebug() << "error in finding active connections";
        exit(1);
    }
}

void Scheduler::updateSenderIp(QHostAddress oldIp, QHostAddress newIp) {
    if (oldIp == newIp) return;
    qDebug() << "update ip";
    foreach (Sender* sender, senders_) {
        qDebug() << sender->srcAddr();
        if (sender->srcAddr() == oldIp) {
            sender->changeSource(newIp, 0);
        }
    }
}





