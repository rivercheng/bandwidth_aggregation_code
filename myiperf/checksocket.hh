#ifndef __CHECK_SOCKET_H__
#define __CHECK_SOCKET_H__
#include <QHostAddress>
#include <QUdpSocket>
#include "chunk.hh"
#include <pcap.h>
class QTimer;
class Device;
class CheckSocket : public QUdpSocket {
    Q_OBJECT
public:
    CheckSocket(const QString& name, const QHostAddress& srcAddr, quint16 srcPort, const QHostAddress& dstAddr,  quint16 dstPort,  QObject *parent=0);
    virtual ~CheckSocket(void);
    bool isAvailable(void);
    qint64 sendData(const QByteArray & datagram);
    void setAvailable(void) 
    {
        qDebug() << "available";
        isAvailable_ = true;
    }
public slots:
    //void recved(PacketID id);
    void recved(void);
    //void resend(void);
signals:
    void ready(void);
    void sent(void);
private:
    Device*  device_;
    QHostAddress addr_;
    quint16  port_;
    PacketID sentID_;
    bool     isAvailable_;
    QByteArray packet_;
    QTimer   *timer_;
    bool     isResend_;
};
#endif
