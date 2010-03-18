#include <QNetworkInterface>
#include <QHostAddress>
#include <QDebug>

int main(void) {
    QList<QHostAddress> addrs = QNetworkInterface::allAddresses();
    foreach(QHostAddress addr, addrs) {
        qDebug() << addr;
    }
    QList<QNetworkInterface> infs = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface inf, infs) {
        QNetworkInterface::InterfaceFlags iflags = inf.flags();
        qDebug() << inf.name() << " " << iflags;
        if (iflags.testFlag(QNetworkInterface::IsLoopBack)) {
            qDebug() << "loopback device";
        }
        if (iflags.testFlag(QNetworkInterface::IsUp)) {
            qDebug() << "up";
        }
        if (iflags.testFlag(QNetworkInterface::IsRunning)) {
            qDebug() << "running";
        }
        QList<QNetworkAddressEntry> entrys = inf.addressEntries();
        foreach(QNetworkAddressEntry entry, entrys) {
            qDebug() << "\t" << entry.ip();
            switch (entry.ip().protocol()) {
                case QAbstractSocket::IPv4Protocol:
                    qDebug() << "IPv4";
                    qDebug() << entry.ip().toString();
                    break;
                case QAbstractSocket::IPv6Protocol:
                    qDebug() << "IPv6";
                    qDebug() << entry.ip().toString();
                    break;
                default:
                    qDebug() <<"unknown";
            }
        }
    }
}

