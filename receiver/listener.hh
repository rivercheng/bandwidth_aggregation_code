#ifndef __UDP_LISTENER_H__
#define __UDP_LISTENER_H__
#include <QObject>
#include <QList>
#include <QHostAddress>
#include <QHash>

class QUdpSocket;
class UdpDecoder;
class QFile;
class UdpListener : public QObject {
    Q_OBJECT
public:
    UdpListener(QUdpSocket *socket, QHostAddress outAddr, \
            quint16 outPort, int b, int k, unsigned int delay, QFile *f=0);
private slots:
    void processPendingDatagrams();
    
private:
    QUdpSocket *udpSocket_;
    QHostAddress outAddr_;
    quint16      outPort_;
    int b_;
    int k_;
    unsigned int delay_;
    QFile *records_;
    QList<UdpDecoder *> decoders_;
    QHash<QHostAddress, quint16> decoderPortHashTable_;
};
#endif
