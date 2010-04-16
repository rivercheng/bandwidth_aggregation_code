#include "checksocket.hh"
#include "device.hh"
#include "header.hh"
#include <QTimer>
#include <QCoreApplication>
CheckSocket::CheckSocket(const QString& name, const QHostAddress& srcAddr, quint16 srcPort, const QHostAddress& dstAddr, quint16 dstPort, QObject *parent)
    :QUdpSocket(parent), addr_(dstAddr), port_(dstPort), sentID_(-1), isAvailable_(true), isResend_(false)
{
    //device_ = new Device(name, localPort(), addr, port);
    if(!bind(srcAddr, srcPort)) {
        qDebug() << "bind error" << endl;
        exit(1);
    }
    device_ = new Device(name, this, addr_, port_);
    //timer_  = new QTimer(this);
    connect(device_, SIGNAL(received()), this, SLOT(recved()));
    connect(this, SIGNAL(sent()), device_, SLOT(capture()));
    //connect(timer_, SIGNAL(timeout()), this, SLOT(resend()));
}

CheckSocket::~CheckSocket() {
    delete device_;
}

qint64 CheckSocket::sendData(const QByteArray & datagram)
{
    if (!isAvailable_) {
        return -1;
    }
    isAvailable_ = false;
    packet_ = datagram;
    int sendID = packetInfo(datagram).id;
    sentID_ = sendID;
    qint64 res = 0;
    while (!isAvailable_) {
        res = writeDatagram(datagram, addr_, port_);
        while (res == -1) {
                usleep(100);
                //qDebug() << "retry";
                res = writeDatagram(datagram, addr_, port_);
        };
        emit sent();
        usleep(2000);
        QCoreApplication::processEvents();
    }
    return res;
}

//void CheckSocket::recved(PacketID id) {
void CheckSocket::recved() {
    isAvailable_ = true;
    emit ready();
}



