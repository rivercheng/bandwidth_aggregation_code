#ifndef __UDP_RELAY_H__
#define __UDP_RELAY_H__
#include <QObject>
#include <QList>
#include <QHostAddress>
#include "packetInfo.hh"

class QUdpSocket;
class UdpRelay : public QObject {
    Q_OBJECT
public:
    UdpRelay(quint16 listenPort, QHostAddress sender, quint16 outPort, QVector<PacketInfo> infos);
private slots:
    void processPendingDatagrams();
private:
    QUdpSocket *udpSocket;
    QHostAddress sender_;
    quint16      outPort_;
    QVector<PacketInfo> infos_;
    PacketList   pList_;
    qint32       id;
};
#endif
