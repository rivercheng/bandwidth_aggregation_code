#include "config.hh"
#include "def.hh"
#include "looper.hh"
#include <QApplication>
#include <QtGui>
#include <QTextStream>
#include <QUdpSocket>
#include <QDate>
QTextStream cerr(stderr);
QTextStream cout(stdout);
QDate EXPIRED_DAY(2010,8,10);

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
    config.max_delay = 4;
    config.min_delay = 2;

    Looper looper(&config, &app);
    QObject::connect(&looper, SIGNAL(quit()), &app, SLOT(quit()));
    
    app.exec();
    return 0;
}
