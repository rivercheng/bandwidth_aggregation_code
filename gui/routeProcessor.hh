#ifndef __ROUTE_PROCESSOR_H__
#define __ROUTE_PROCESSOR_H__
#include <QHostAddress>
#include <QStringList>
#include <QObject>

class RouteProcessor : public QObject {
public:
    RouteProcessor();
    ~RouteProcessor();
private:
    struct InterfaceInfo {
        QString name;
        QHostAddress ip;
        QHostAddress netmask;
        QHostAddress gateway;
        InterfaceInfo(QString name, QHostAddress ip, QHostAddress netmask)
            :name(name), ip(ip), netmask(netmask) {
            gateway = QHostAddress((ip.toIPv4Address() & netmask.toIPv4Address())+1);
        }
    };
    void findAllValidDevices();
    void setRouteAndRules();
    void removeRouteAndRules();
    bool runCommand(const QString&, const QStringList&);
private:
    QList<InterfaceInfo>  infos;
    QStringList tables;
};
#endif
