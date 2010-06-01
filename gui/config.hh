#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <QObject>
#include <QHostAddress>
struct Config{
    int b;
    int k;
    quint16 inPort;
    quint16 outPort;
    QHostAddress outAddr;
};

#endif
