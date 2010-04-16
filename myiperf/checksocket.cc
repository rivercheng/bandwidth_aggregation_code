#include "checksocket.hh"
#include "device.hh"
#include "header.hh"
#include <QTimer>
CheckSocket::CheckSocket(const QString& name, const QHostAddress& srcAddr, quint16 srcPort, const QHostAddress& dstAddr, quint16 dstPort, QObject *parent)
    :QUdpSocket(parent), addr_(dstAddr), port_(dstPort), sentID_(-1), isAvailable_(true)
{
    //device_ = new Device(name, localPort(), addr, port);
    if(!bind(srcAddr, srcPort)) {
        qDebug() << "bind error" << endl;
        exit(1);
    }
    device_ = new Device(name, this, addr_, port_);
    timer_  = new QTimer(this);
    connect(device_, SIGNAL(received()), this, SLOT(recved()));
    connect(this, SIGNAL(sent()), device_, SLOT(capture()));
    connect(timer_, SIGNAL(timeout()), this, SLOT(resend()));
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
    qint64 res = writeDatagram(datagram, addr_, port_);
    int sendID = packetInfo(datagram).id;
    sentID_ = sendID;
    packet_ = datagram;
    while (res == -1) {
            usleep(100);
            //qDebug() << "retry";
            res = writeDatagram(datagram, addr_, port_);
    };
    //qDebug() << "sent " << res;
    timer_->start(1);
    emit sent();
    return res;
}

//void CheckSocket::recved(PacketID id) {
void CheckSocket::recved() {
    //if (id == sentID_) {
    //qDebug() << "recved";
    isAvailable_ = true;
    timer_->stop();
    emit ready();
}

void CheckSocket::resend() {
    if (isAvailable_) {
        return;
    }
    int res = writeDatagram(packet_, addr_, port_);
    while (res == -1) {
            usleep(100);
            qDebug() << "retry";
            res = writeDatagram(packet_, addr_, port_);
    };
    qDebug() << "resend " << res;
    timer_->start(1);
    emit sent();
}




