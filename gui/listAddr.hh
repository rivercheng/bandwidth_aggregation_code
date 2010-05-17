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
    bool operator==(const Interface& inf) const {
        return name == inf.name && addr == inf.addr;
    }
};

uint qHash(Interface k);

namespace ListAddr {
QList<Interface> validIPv4Infs();

QList<QHostAddress> validIPv4Addrs();
}
#endif
