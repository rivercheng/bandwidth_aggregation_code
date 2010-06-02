#ifndef __SENDER_H__
#define __SENDER_H__
#include <QThread>
#include <QSemaphore>
#include <QDataStream>
#include <QHostAddress>
#include <pcap.h>
#include "def.hh"
class QUdpSocket;
class Device;
class Sender : public QThread {
public: 
    Sender(const QString & devName, const QHostAddress & srcAddr, quint16 srcPort, const QHostAddress & dstAddr, \
            quint16 dstPort, QSemaphore *readyToSend, FlowDict *dict);
    
    virtual ~Sender(void);
    
    void run(void);

    QHostAddress srcAddr(void) {
        return srcAddr_;
    }
    
    void send(const QByteArray & packet);
    
    bool isAvailable(void) {
        return isAvailable_;
    }

    void changeSource(const QHostAddress& srcAddr, quint16 srcPort);
private:
    void   initPcap(const QString & devName);
    
    int    packetID(const QByteArray & packet) {
        QDataStream ds(packet);
        int id;
        ds >> id;
        return id;
    }
    
    int pcapPacketID(const u_char *p) {
        if (datalink_type_ == DLT_EN10MB || datalink_type_ == DLT_IEEE802_11) {
            p += 42;
        } else if (datalink_type_ == DLT_LINUX_SLL) {
            p += 44;
        } else {
            qDebug() << "unsupport interface type " << datalink_type_;
            exit(1);
        }
        QByteArray pdata((const char *)p, 4);
        QDataStream ds(pdata);
        int id;
        ds >> id;
        return id;
    }
    
    bool trySending();
    Sender(const Sender&);
    Sender& operator=(const Sender&);
private:
    QHostAddress srcAddr_;
    QHostAddress dstAddr_;
    quint16      dstPort_;
    QSemaphore   *readyToSend_;
    QSemaphore   waitingPacket_;
    bool isAvailable_;
    QUdpSocket   *sock_;
    pcap_t       *handle_;
    QByteArray   packet_;
    int          id_;
    int          datalink_type_;
    long         *dataflow_;
};
#endif
