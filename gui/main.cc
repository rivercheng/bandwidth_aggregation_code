#include "listener.hh"
#include "def.hh"
#include "flowrategraph.hh"
#include "listAddr.hh"
#include "updater.hh"
#include <QApplication>
#include <QCheckBox>
#include <QVBoxLayout>
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
    
    if (args.size() < 5) {
        usage(args);
        return 1;
    }
    quint16 inPort  = 10000;
    quint16 outPort = 12000;
    bool ok1, ok2;
    inPort = args[1].toInt(&ok1);
    outPort = args[3].toInt(&ok2);
    if (!ok1 || !ok2) {
        usage(args);
        cerr << args[1] << " or " << args[3] << " is not a valid port number" << endl;
        return 1;
    }
    
    QHostAddress outAddr = QHostAddress(args[2]);
    if (outAddr.isNull()) {
        usage(args);
        cerr << args[2] << " is not a valid address" << endl;
        return 1;
    }
    
    int b, k;
    b = args[4].toInt(&ok1);
    k = args[5].toInt(&ok2);
    if (!ok1 || !ok2 ) {
        usage(args);
        cerr << "enter the correct b, k values" << endl;
        return 1;
    }

    QUdpSocket *sock = new QUdpSocket();
    if (!sock->bind(QHostAddress::Any, inPort)) {
        cerr << "bind error" << endl;
        return(1);
    }
    
    QWidget window;
    window.setWindowTitle("Multiple Channels Scheduler");

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
    QTimer timer;

    FlowrateGraph graph(&ratedict, interval);
    QObject::connect(&timer, SIGNAL(timeout()), &updater, SLOT(update()));
    QObject::connect(&timer, SIGNAL(timeout()), &graph, SLOT(plot()));

    QVBoxLayout layout;
    layout.addWidget(&graph);
    
    QHBoxLayout infLayout;
    foreach (Interface inf, infs) {
        QCheckBox *checkbox = new QCheckBox(inf.name);
        infLayout.addWidget(checkbox);
    }
    layout.addLayout(&infLayout);
    
    window.setLayout(&layout);
    window.show();
    
    Listener listener(sock, outAddr, outPort, b, k, &flowdict);


    timer.start(interval*1000);
    app.exec();
    return 0;
}
