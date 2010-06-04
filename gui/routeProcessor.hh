#ifndef __ROUTE_PROCESSOR_H__
#define __ROUTE_PROCESSOR_H__
#include <QHostAddress>
#include <QStringList>
#include <QObject>

class RouteProcessor : public QObject {
public:
    RouteProcessor(QObject *parent = 0);
    ~RouteProcessor();
private:
    struct InterfaceInfo {
        QString name;
        QHostAddress ip;
        QHostAddress netmask;
        bool         isPPP;
        QHostAddress gateway;
        InterfaceInfo(QString name, QHostAddress ip, QHostAddress netmask, bool isPPP)
            :name(name), ip(ip), netmask(netmask), isPPP(isPPP) {
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
