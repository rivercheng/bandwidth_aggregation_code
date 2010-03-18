#include <QNetworkInterface>
#include <QHostAddress>
#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <cstdlib>

//now support 4 non-loop interfaces
const int COUNT = 4;
QTextStream cerr(stderr);
QTextStream cout(stdout);

bool runCommand(QString command, QStringList arguments) {
    cout << command << " ";
    foreach (QString argument, arguments) {
        cout << argument << " ";
    }
    cout << endl;
            
    QProcess proc;
    proc.start(command, arguments);
    bool res = proc.waitForFinished();
    cout << proc.readAllStandardError();
    return res;
}

int main(void) {
    QStringList tables;
    tables << "t1" << "t2" << "t3" << "t4";
    
    QList<QNetworkInterface> infs = QNetworkInterface::allInterfaces();
    int index = 0;
    foreach(QNetworkInterface inf, infs) {
        QNetworkInterface::InterfaceFlags iflags = inf.flags();
        if (! iflags.testFlag(QNetworkInterface::IsLoopBack) && \
              iflags.testFlag(QNetworkInterface::IsUp)  && \
              iflags.testFlag(QNetworkInterface::IsRunning)) {

            if (index >= COUNT) {
                break;
            }

            //setup route tables
            QString command = "ip";
            QStringList argument;
            argument << "route" << "delete" << "default" << "table" << tables[index];
            runCommand(command, argument);

            argument.clear();
            argument << "route" << "add" << "default" << "dev" << inf.name();
            argument << "table" << tables[index];
            if (! runCommand(command, argument))
            {
                cerr << "Error in adding route table for interface "<< inf.name() << endl;
                exit(1);
            }

            
            QList<QNetworkAddressEntry> entrys = inf.addressEntries();
            foreach(QNetworkAddressEntry entry, entrys) {
                switch (entry.ip().protocol()) {
                    case QAbstractSocket::IPv4Protocol:
                        //adding rules
                        {
                            argument.clear();
                            argument << "rule" << "del" << "from" << entry.ip().toString();
                            runCommand(command, argument);

                            argument.clear();
                            argument << "rule" << "add" << "from" << entry.ip().toString();
                            argument << "table" << tables[index];
                            if (! runCommand(command, argument)) {
                                cerr << "Error in adding rules for address " << entry.ip().toString();
                                exit(1);
                            }
                        }
                        break;
                    case QAbstractSocket::IPv6Protocol:
                        break;
                    default:
                        break;
                }
            }
            index++;
        }
    }
}

