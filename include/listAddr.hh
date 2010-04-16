#ifndef __LIST_ADDR_H__
#define __LIST_ADDR_H__
#include <QNetworkInterface>
#include <QHostAddress>
#include <QDebug>

struct Interface {
    QString name;
    QHostAddress addr;
    Interface(const QString& name, const QHostAddress& addr)
        :name(name), addr(addr) {;}
};

namespace ListAddr {
QList<Interface> validIPv4Infs() {
    QList<Interface> out_infs;
    QList<QNetworkInterface> infs = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface inf, infs) {
        QNetworkInterface::InterfaceFlags iflags = inf.flags();
        if (iflags.testFlag(QNetworkInterface::IsLoopBack)) {
            continue; //ignore loopback address
        }
        if (iflags.testFlag(QNetworkInterface::IsUp) && \
                    iflags.testFlag(QNetworkInterface::IsRunning)) {
            QList<QNetworkAddressEntry> entrys = inf.addressEntries();
            foreach(QNetworkAddressEntry entry, entrys) {
                switch (entry.ip().protocol()) {
                case QAbstractSocket::IPv4Protocol:
                    out_infs.append(Interface(inf.name(), entry.ip()));
                    break;
                case QAbstractSocket::IPv6Protocol:
                    //ignore IPv6 address
                    break;
                default:
                    //ignore
                    break;
                }
            }
        }
    }
    return out_infs;
}

QList<QHostAddress> validIPv4Addrs() {
    QList<QHostAddress> addrs;
    QList<QNetworkInterface> infs = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface inf, infs) {
        QNetworkInterface::InterfaceFlags iflags = inf.flags();
        if (iflags.testFlag(QNetworkInterface::IsLoopBack)) {
            continue; //ignore loopback address
        }
        if (iflags.testFlag(QNetworkInterface::IsUp) && \
                    iflags.testFlag(QNetworkInterface::IsRunning)) {
            QList<QNetworkAddressEntry> entrys = inf.addressEntries();
            foreach(QNetworkAddressEntry entry, entrys) {
                switch (entry.ip().protocol()) {
                case QAbstractSocket::IPv4Protocol:
                    addrs.append(entry.ip());
                    break;
                case QAbstractSocket::IPv6Protocol:
                    //ignore IPv6 address
                    break;
                default:
                    //ignore
                    break;
                }
            }
        }
    }
    return addrs;
}
}
#endif
