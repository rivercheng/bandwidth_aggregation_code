#ifndef __SPLITTER_HH__
#define __SPLITTER_HH__
#include <QObject>
#include "def.hh"

class Config;
class Listener;
class QUdpSocket;
class Splitter : public QObject
{
    Q_OBJECT
public:
    Splitter (Config* config, FlowDict* flowdict, QObject *parent = 0);
public slots:
    void start();
    void restart();
private:
    quint16  inPort;
    Config   *config;
    FlowDict *flowdict;
    Listener *listener;
    QUdpSocket *sock;
};
#endif
