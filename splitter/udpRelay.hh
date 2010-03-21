#ifndef __UDP_RELAY_H__
#define __UDP_RELAY_H__
#include <QObject>
#include <QList>
#include <QHostAddress>
#include <QStringList>

class QUdpSocket;
class QTimer;
class UdpRelay : public QObject {
    Q_OBJECT
public:
    class NoAddressFile{};
    class BindError{};
    class WrongAddress{};
    UdpRelay(quint16 listenPort, QHostAddress sender, quint16 outPort);
private slots:
    void processPendingDatagrams();
    void processTimer();
private:
    QUdpSocket *selectSocket();
private:
    QUdpSocket *udpSocket;
    
    QList< QUdpSocket* > outSockets;
    QStringList  addresses;
    int index;
    
    QHostAddress sender_;
    quint16      outPort_;
    
    QTimer       *timer;
};
#endif
