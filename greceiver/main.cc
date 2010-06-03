#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QApplication>
#include <QUdpSocket>
#include <QDateTime>
#include "mainWindow.hh"
#include "config.hh"

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    Config config;
    config.inPort = 11000;
    config.outPort = 12000;
    config.b = 4;
    config.k = 30;
    config.outAddr = QHostAddress("127.0.0.1");
    config.delay = 1;
    config.rf = 0;

    MainWindow mainWindow(&config);
    mainWindow.setWindowTitle("Multiple Channels Receiver");
    mainWindow.show();
    QObject::connect(&mainWindow, SIGNAL(quit()), &app, SLOT(quit()));

    app.exec();
    return 0;
}

