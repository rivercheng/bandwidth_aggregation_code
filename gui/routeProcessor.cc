#include "routeProcessor.hh"
#include <QNetworkInterface>
#include <QProcess>

RouteProcessor::RouteProcessor() {
    tables << "t1" << "t2" << "t3" << "t4" << "t5" << "t6" << "t7" << "t8";
    findAllValidDevices();
    setRouteAndRules();
}

RouteProcessor::~RouteProcessor() {
    removeRouteAndRules();
}
    

void RouteProcessor::findAllValidDevices() {
  
    QList<QNetworkInterface> infs = QNetworkInterface::allInterfaces();
    foreach(QNetworkInterface inf, infs) {
        QNetworkInterface::InterfaceFlags iflags= inf.flags();
        if (! iflags.testFlag(QNetworkInterface::IsLoopBack) && \
              iflags.testFlag(QNetworkInterface::IsUp)  && \
              iflags.testFlag(QNetworkInterface::IsRunning)) {
            QList<QNetworkAddressEntry> addressEntries = inf.addressEntries();
            foreach(QNetworkAddressEntry entry, addressEntries) {

                if (entry.ip().toIPv4Address() != 0) {
                    InterfaceInfo info(inf.name(), entry.ip(), entry.netmask());
                    infos.push_back(info);
                }
            }
        }
    }
}

void RouteProcessor::setRouteAndRules() {
    int index = 0;
    foreach(InterfaceInfo info, infos) {
        QString command = "ip";
        QStringList argument;
        argument << "route" << "add" << "default" << "via" << info.gateway.toString() << "dev" << info.name;
        argument << "table" << tables[index];
        if (!runCommand(command, argument)) {
             qDebug() << "Error in adding routing table for interface " << info.name;
             exit(1);
        }
        
        argument.clear();
        argument << "rule" << "add" << "from" << info.ip.toString() << "table" << tables[index];
        if (!runCommand(command, argument)) {
             qDebug() << "Error in adding rule for address " << info.ip.toString();
             exit(1);
        }
        index++;
    }
}

void RouteProcessor::removeRouteAndRules() {
    int index = 0;
    foreach(InterfaceInfo info, infos) {
        QString command = "ip";
        QStringList argument;
        argument << "route" << "delete" << "default" << "table" << tables[index];
        runCommand(command, argument);
        
        argument.clear();
        argument << "rule" << "del" << "from"  << info.ip.toString();
        runCommand(command, argument);
        
        index++;
    }
}
    
bool RouteProcessor::runCommand(const QString& command, const QStringList& arguments) {
    QTextStream cout(stdout);
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
        

      
                    
                    


    
