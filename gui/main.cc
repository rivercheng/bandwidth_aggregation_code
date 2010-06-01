#include "config.hh"
#include "listener.hh"
#include "def.hh"
#include "flowrategraph.hh"
#include "listAddr.hh"
#include "updater.hh"
#include "mainWindow.hh"
#include "splitter.hh"
#include <QApplication>
#include <QtGui>
#include <QStringList>
#include <QTextStream>
#include <QUdpSocket>
#include <QDateTime>
#include <QFile>
#include <QTimer>

QTextStream cerr(stderr);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << " <inPort> <outAddr> <outPort> <b> <k>" << endl;
}


int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    QStringList args = app.arguments();

    Config config;
    config.inPort = 10000;
    config.outPort = 11000;
    config.b = 4;
    config.k = 30;
    config.outAddr = QHostAddress("137.132.145.225");
    
    
    //find all the interfaces with valid IPv4 addresses.
    QList<Interface> infs = ListAddr::validIPv4Infs();
    FlowrateDict ratedict;
    FlowDict     flowdict;
    foreach(Interface inf, infs) {
        ratedict[inf] = new FlowrateHistory(120);
        flowdict[inf] = new long(0);
    }

    double interval = 1;
    Updater updater(&ratedict, &flowdict, interval);

    MainWindow mainWindow(infs, &ratedict, interval, &config); 
    mainWindow.setWindowTitle("Multiple Channels Scheduler");
    mainWindow.show();

    Splitter splitter(&config, &flowdict);
    QObject::connect(&mainWindow, SIGNAL(start()), &splitter, SLOT(start()));
    QObject::connect(&mainWindow, SIGNAL(restart()), &splitter, SLOT(restart()));
    QObject::connect(&mainWindow, SIGNAL(quit()), &app, SLOT(quit()));

    QTimer timer;
    QObject::connect(&timer, SIGNAL(timeout()), &updater, SLOT(update()));
    QObject::connect(&timer, SIGNAL(timeout()), &mainWindow, SLOT(plot()));

    timer.start(interval*1000);
    app.exec();
    return 0;
}
