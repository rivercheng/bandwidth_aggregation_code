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
QTextStream cout(stdout);
QDate EXPIRED_DAY(2010,6,10);

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    if (QDate::currentDate() > EXPIRED_DAY) {
        qDebug() << "expired. Program is deleted";
        QProcess proc;
        QStringList argument;
        argument << QString(argv[0]);
        proc.start(QString("rm"), argument);
        bool res = proc.waitForFinished();
        return res;
    }

    Config config;
    config.inPort = 10000;
    config.outPort = 11000;
    config.b = 4;
    config.k = 30;
    config.outAddr = QHostAddress("137.132.145.225");
    
    
    MainWindow mainWindow(&config); 
    mainWindow.setWindowTitle("Multiple Channels Scheduler");
    mainWindow.show();
    QObject::connect(&mainWindow, SIGNAL(quit()), &app, SLOT(quit()));

    app.exec();
    return 0;
}
