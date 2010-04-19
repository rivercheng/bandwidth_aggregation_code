#ifndef __DEVICE_H__
#define __DEVICE_H__
#include <QObject>
#include <QHostAddress>
#include <QString>
#include "chunk.hh"
#include <pcap.h>
class CheckSocket;
class Device : public QObject {
    Q_OBJECT
public:
    //Device(QString name, quint16 inPort, QHostAddress outAddr, quint16 outPort);
    Device(QString name, CheckSocket *sock, QHostAddress outAddr, quint16 outPort);
signals:
    //void received(PacketID id);
    void received(void);
    void resend(void);
public slots:
    void capture(void);
private:
    void init();
    CheckSocket *sock_;
    QByteArray name_;
    QByteArray filter_;
    pcap_t   *handle;
};



#endif
