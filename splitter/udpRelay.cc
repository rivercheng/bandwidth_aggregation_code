#include <QUdpSocket>
#include <QTextStream>
#include <cstdlib>
#include <QTimer>
#include <QFile>
#include <QProcess>
#include "udpRelay.hh"
#include "listAddr.hh"

static bool runCommand(QString command, QStringList arguments) {
    QTextStream cout(stdout);
    cout << command << " ";
    foreach (QString argument, arguments) {
        cout << argument << " ";
    }
    cout << endl;
            
    QProcess proc;
    proc.start(command, arguments);
    bool res = proc.waitForFinished();
    cout << proc.readAllStandardError();
    return res;
}

UdpRelay::UdpRelay(quint16 listenPort, QHostAddress sender, quint16 outPort)
    :index(0), sender_(sender), outPort_(outPort) {
    udpSocket = new QUdpSocket(this);
    if (!udpSocket->bind(listenPort)) {
        throw BindError();
    }

    QList<QHostAddress> addrs = ListAddr::validIPv4Addrs();
    foreach(QHostAddress addr, addrs) {
        QUdpSocket *sock = new QUdpSocket(this);
        if (!sock->bind(addr, 0)) {
            throw BindError();
        }
        qDebug() << addr.toString() <<" is used";
        outSockets.append(sock);
        addresses.append(addr.toString());
    }

    QString command = "ping";
    QStringList arguments;
    for (int i = 0; i < 3; i++) {
    	foreach(QString address, addresses) {
        	arguments << "-I" << address << sender_.toString() <<"-c" << "2";
        	runCommand(command, arguments);
            arguments.clear();
    	}
    }

    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));
    timer = new QTimer();
    connect(timer, SIGNAL(timeout()), this, SLOT(processTimer()));
    timer->start(1000);
}

void UdpRelay::processPendingDatagrams()
{
    timer->start(1000);
    QTextStream cout(stdout);
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());
        QUdpSocket *outSocket = selectSocket();
        outSocket->writeDatagram(datagram, sender_, outPort_);
    }
}

QUdpSocket *UdpRelay::selectSocket() {
    if (index >= outSockets.size()) {
        index = 0;
    }
    return outSockets[index++];
}


void UdpRelay::processTimer() {
    QString command = "ping";
    QStringList arguments;
    foreach(QString address, addresses) {
       arguments << "-I" << address << sender_.toString() <<"-c" << "2";
       runCommand(command, arguments);
       arguments.clear();
    }
}






