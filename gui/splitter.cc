#include "splitter.hh"
#include "listener.hh"
#include "config.hh"
#include <QUdpSocket>
Splitter::Splitter(Config* config, FlowDict* flowdict, QObject *parent) 
    :QObject(parent), config(config), flowdict(flowdict)
{
    sock = new QUdpSocket(this);
    if (!sock->bind(QHostAddress::Any, config->inPort)) {
        qDebug() << "Cannot listen at port " << config->inPort;
        exit(1);
    }
    inPort = config->inPort;
}

void Splitter::start() {
    listener = new Listener(sock, config->outAddr, config->outPort, config->b, config->k, flowdict);
}

void Splitter::restart() {
    if (config-> inPort !=  inPort) {
        delete sock;
        sock = new QUdpSocket();
        if (!sock->bind(QHostAddress::Any, config->inPort)) {
            exit(1);
        }
        inPort = config->inPort;
    }

    delete listener;
    listener = new Listener(sock, config->outAddr, config->outPort, config->b, config->k, flowdict);
}
