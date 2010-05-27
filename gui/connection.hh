#ifndef __CONNECTION_H__
#define __CONNECTION_H__
#include <QObject>
#include <QtDBus>
#include <QHostAddress>
enum DEVICE_STATE {
   NM_DEVICE_STATE_DISCONNECTED = 3,
   NM_DEVICE_STATE_ACTIVATED = 8
};

class ConnectionInfo {
public:
    ConnectionInfo() {;}
    ConnectionInfo(const QString& serv, const QDBusObjectPath& c, const QDBusObjectPath& spec, const QDBusObjectPath& d)
        :serviceName(serv), connectionPath(c), specificObject(spec), device(d) { ; }
    QString serviceName;
    QDBusObjectPath connectionPath;
    QDBusObjectPath specificObject;
    QDBusObjectPath device;
};

class ActiveConnection : public QObject {
    Q_OBJECT
public:
    ActiveConnection(const QDBusObjectPath& p) {
        QString service("org.freedesktop.NetworkManager"); 
        QString iface("org.freedesktop.NetworkManager.Connection.Active");

        QDBusInterface activeConn(service, p.path(), iface, QDBusConnection::systemBus());
        if (activeConn.isValid()) {
                QString service = activeConn.property("ServiceName").toString();
                QDBusObjectPath conn = activeConn.property("Connection").value<QDBusObjectPath>();
                QDBusObjectPath specificObject = activeConn.property("SpecificObject").value<QDBusObjectPath>();
                QList<QDBusObjectPath> devList = activeConn.property("Devices").value<QList<QDBusObjectPath> >();
                QDBusInterface *device = new QDBusInterface(service, devList[0].path(), "org.freedesktop.NetworkManager.Device", QDBusConnection::systemBus());
                inf_ = device;
                connect(inf_, SIGNAL(StateChanged(uint, uint, uint)), this, SLOT(stateChangeHandle(uint, uint, uint)));
                if (device->isValid()) {
                        int ipv4Addr = device->property("Ip4Address").toInt();
                        ipv4Addr  = qFromBigEndian<int>((const uchar*)&ipv4Addr);
                        qDebug() << ipv4Addr;
                        IPv4Addr_ = QHostAddress(ipv4Addr);
                        qDebug() << IPv4Addr_;
                }
                info_ = ConnectionInfo(service, conn, specificObject, devList[0]);
        } else {
                qDebug() << "invalid interface to connection active";
                exit(1);
        }
    }
    void disconnect();
    bool isActive();
private slots:
    void stateChangeHandle(uint state, uint, uint) {
        qDebug() << "statechange: " <<  state;
        if (state == NM_DEVICE_STATE_DISCONNECTED) {
            qDebug() << "reconnecting";
            reconnect();
        } else if (state == NM_DEVICE_STATE_ACTIVATED) {
            qDebug() << "reconnected";
            updateIP();
        }
    }

    void updateIP() {
        QString service("org.freedesktop.NetworkManager"); 
        //delete inf_;
        //inf_ = 0;
        //QDBusInterface *device = new QDBusInterface(service, info_.connectionPath.path(), "org.freedesktop.NetworkManager.Device", QDBusConnection::systemBus());
        //if (device && device->isValid()) {
        //    inf_ = device;
            int ipv4Addr = inf_->property("Ip4Address").toInt();
            ipv4Addr = qFromBigEndian<int>((const uchar *)&ipv4Addr);
            qDebug() << ipv4Addr;
            emit IpChanged(IPv4Addr_, QHostAddress(ipv4Addr));
            IPv4Addr_ = QHostAddress(ipv4Addr);
            qDebug() << IPv4Addr_;
            //connect(inf_, SIGNAL(StateChanged(uint, uint, uint)), this, SLOT(stateChangeHandle(uint, uint, uint)));
        //} else {
        //    qDebug() << "cannot create interface to the device";
        //}
    }

private:
    void reconnect() {
        QString service("org.freedesktop.NetworkManager"); 
        QString path("/org/freedesktop/NetworkManager");
        QString iface("org.freedesktop.NetworkManager");
        
        QDBusInterface nm(service, path, iface, QDBusConnection::systemBus());
        if (nm.isValid()) {
            QDBusReply<QDBusObjectPath> ac = nm.call("ActivateConnection", info_.serviceName, qVariantFromValue(info_.connectionPath), \
                    qVariantFromValue(info_.device), qVariantFromValue(info_.specificObject));
            if (ac.isValid()) {
                ;
            } else {
                qDebug() << ac.error();
            }
        } else {
            qDebug() << "failed in create interface to networkmanager.";
        }
    }
signals:
    void IpChanged(QHostAddress oldIp, QHostAddress newIp);
private:
    QHostAddress IPv4Addr_;
    bool isActive_;
    ConnectionInfo info_;
    QDBusInterface *inf_;
};
#endif
