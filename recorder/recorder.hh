#ifndef __RECORDER_H__
#define __RECORDER_H__
#include <QObject>
#include <QHostAddress>
class QUdpSocket;
class UdpRecorder : public QObject {
    Q_OBJECT
public:
    UdpRecorder(QObject *app, QHostAddress listenAddr, quint16 listenPort, \
                QHostAddress relayAddr=QHostAddress::LocalHost, quint16 relayPort=0);
signals:
    void quit();
private slots:
    void processPendingDatagrams();
private:
    QUdpSocket *udpSocket;
    QHostAddress relayAddr;
    quint16      relayPort;
};
#endif
