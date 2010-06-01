#include "splitter.hh"
#include "listener.hh"
#include "config.hh"
#include <QUdpSocket>
Splitter::Splitter(Config* config, FlowDict* flowdict) 
    :config(config), flowdict(flowdict)
{
    sock = new QUdpSocket();
    if (!sock->bind(QHostAddress::Any, config->inPort)) {
        //cerr << "cannot bind to port " << config->inPort << endl;
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
            //cerr << "cannot bind to port " << config->inPort << endl;
            exit(1);
        }
        inPort = config->inPort;
    }

    delete listener;
    listener = new Listener(sock, config->outAddr, config->outPort, config->b, config->k, flowdict);
}
