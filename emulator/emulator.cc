#include <QString>
#include <QStringList>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>
#include "udpRelay.hh"
#include "packetInfo.hh"

QTextStream cout(stdout, QIODevice::WriteOnly);
QTextStream cerr(stderr, QIODevice::WriteOnly);

void usage(QStringList args) {
    cerr << "Usage: " << args[0] << " <filename> " << endl;
}

PacketInfo processLine(QString line) {
    QStringList entries = line.split(" ");
    //if (entries.size() != 4) {
    if (entries.size() != 2) {
        cerr << "wrong line " << line << endl;
    }
    //bool ok1, ok2, ok3;
    bool ok1;
    int id = entries[0].toInt(&ok1);
    QString status = entries[1];
    //int sec = entries[2].toInt(&ok2);
    //int usec = entries[3].toInt(&ok3);
    //if (! (ok1 && ok2 && ok3)) {
    if (! ok1) {
        cerr << "wrong line " << line << endl;
    }
    //cout << QString("%1 %2 %3 %4").arg(id).arg(status).arg(sec).arg(usec) << endl;
    //PacketInfo info(id, status=="true", sec, usec);
    PacketInfo info(id, status=="1", 0, 0);
    return info;
}

    

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QStringList args = app.arguments();
    if (args.size() != 2) {
        usage(args);
        return 1;
    }
    QFile file(args[1]);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cerr << "Cannot open file " << args[1] << endl;
        return 1;
    }

    QTextStream in(&file);
    QVector<PacketInfo> infos;
    while ( !in.atEnd()) {
        QString line = in.readLine();
        PacketInfo info = processLine(line);
        infos.append(info);
    }

    QHostAddress addr=QHostAddress::LocalHost;
    UdpRelay relay(10000, addr, 10020, infos);
    app.exec();

    return 0;
}

