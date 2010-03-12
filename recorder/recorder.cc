#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QUdpSocket>
#include "recorder.hh"
#include "gettime.hh"

UdpRecorder::UdpRecorder(QObject *app, QHostAddress listenAddr, quint16 listenPort, \
                         QHostAddress relayAddr, quint16 relayPort) 
             : relayAddr(relayAddr), relayPort(relayPort) {
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(listenAddr, listenPort);

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    connect(this, SIGNAL(quit()), app, SLOT(quit()));
}

void UdpRecorder::processPendingDatagrams() {
    QTextStream cout(stdout, QIODevice::WriteOnly);
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());

        QHostAddress fromAddr;
        quint16      fromPort;
        
        udpSocket->readDatagram(datagram.data(), datagram.size(), &fromAddr, &fromPort);
        
        QDataStream data(datagram);
        qint32 id;
        quint32 sec;
        quint32 usec;
        data >> id >> sec >> usec;

        PreciseTime currTime = getTime();
        if (relayPort != 0) {
            udpSocket->writeDatagram(datagram, relayAddr, relayPort);
        }
        cout << QString("%1 %2 %3 %4 %5\n").arg(id).arg(sec).arg(usec).arg(currTime.sec).arg(currTime.usec);
        if (id < 0)
        {
            QByteArray fakeData(1400, 0);
            udpSocket->writeDatagram(fakeData, fromAddr, fromPort);
            quit();
        }
    }
}
