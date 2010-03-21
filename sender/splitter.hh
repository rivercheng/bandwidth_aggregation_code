#ifndef __UDP_SPLITTER_H__
#define __UDP_SPLITTER_H__
#include <QObject>
#include <QList>
#include <QHostAddress>

class QUdpSocket;
class BindError{};
class UdpSplitter : public QObject {
    Q_OBJECT
public:
    struct Tuple {
        QHostAddress sourceAddr;
        quint16      sourcePort;
        QHostAddress destAddr;
        quint16      destPort;
        Tuple(QHostAddress sourceAddr, \
                quint16 sourcePort, \
                QHostAddress destAddr, \
                quint16 destPort) 
            :sourceAddr(sourceAddr), sourcePort(sourcePort), \
             destAddr(destAddr), destPort(destPort) {;}
    };

    UdpSplitter(QUdpSocket *socket, QList< Tuple > outTuples);
private slots:
    void processPendingDatagrams();
private:
    int selectOutputIndex();
private:
    QUdpSocket *udpSocket_;
    QList< Tuple > outTuples_;
    QList< QUdpSocket* > outSockets_;
    
    int index;
};
#endif
