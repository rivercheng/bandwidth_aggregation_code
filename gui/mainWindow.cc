#include "config.hh"
#include "updater.hh"
#include "flowrategraph.hh"
#include "mainWindow.hh"
#include "splitter.hh"
#include "interfaceInfo.hh"
#include <QApplication>
#include <QtGui>
#include <QTimer>
#include <cstdlib>

//now support 4 non-loop interfaces
const int COUNT = 8;

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

MainWindow::~MainWindow() {
    removeRules();
}

void MainWindow::removeRules() {
    QStringList tables;
    tables << "t1" << "t2" << "t3" << "t4" << "t5" << "t6" << "t7" << "t8";
    //QList<QNetworkInterface> infs = QNetworkInterface::allInterfaces();
    int index = 0;
    foreach(QNetworkInterface inf, infs) {
        QNetworkInterface::InterfaceFlags iflags = inf.flags();
        if (! iflags.testFlag(QNetworkInterface::IsLoopBack) && \
              iflags.testFlag(QNetworkInterface::IsUp)  && \
              iflags.testFlag(QNetworkInterface::IsRunning)) {

            if (index >= COUNT) {
                break;
            }

            //remove route tables
            QString command = "ip";
            QStringList argument;
            argument << "route" << "delete" << "default" << "table" << tables[index];
            runCommand(command, argument);

            QList<QNetworkAddressEntry> entrys = inf.addressEntries();
            foreach(QNetworkAddressEntry entry, entrys) {
                switch (entry.ip().protocol()) {
                    case QAbstractSocket::IPv4Protocol:
                        //remove rules
                        {
                            argument.clear();
                            argument << "rule" << "del" << "from" << entry.ip().toString();
                            runCommand(command, argument);
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

void MainWindow::setRoute() {
    QStringList tables;
    tables << "t1" << "t2" << "t3" << "t4" << "t5" << "t6" << "t7" << "t8";
    
    //QList<QNetworkInterface> infs = QNetworkInterface::allInterfaces();
    infs = QNetworkInterface::allInterfaces();
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
            argument << "route" << "add" << "default" << "dev" << inf.name();
            argument << "table" << tables[index];
            if (! runCommand(command, argument))
            {
                cerr << "Error in adding route table for interface "<< inf.name() << endl;
                emit quit();
            }

            
            QList<QNetworkAddressEntry> entrys = inf.addressEntries();
            foreach(QNetworkAddressEntry entry, entrys) {
                switch (entry.ip().protocol()) {
                    case QAbstractSocket::IPv4Protocol:
                        //adding rules
                        {
                            argument.clear();
                            argument << "rule" << "add" << "from" << entry.ip().toString();
                            argument << "table" << tables[index];
                            if (! runCommand(command, argument)) {
                                cerr << "Error in adding rules for address " << entry.ip().toString();
                                emit quit();
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


void MainWindow::updateInterfaceDict()
{
    foreach(long *v, flowdict) {
        delete v;
        v=0;
    }
    QList<Interface> infs = ListAddr::validIPv4Infs();
    foreach(Interface inf, infs) {
        ratedict[inf] = new FlowrateHistory(120, 0, this);
        flowdict[inf] = new long(0);
    }
}

MainWindow::MainWindow(Config *config)
    :config(config), interfaceInfo(0), listenPortInput(0), outAddrInput(0), bInput(0),kInput(0), startButton(0), splitter(0) 
{
    double interval = 1;
    setRoute();
    updateInterfaceDict();
    Updater *updater = new Updater(&ratedict, &flowdict, interval, this);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    interfaceInfo = new InterfaceInfo(&ratedict, interval);
    layout->addLayout(interfaceInfo);

    //Config input
    setupConfigInput(layout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    startButton = new QPushButton("Start");
    connect(startButton, SIGNAL(clicked()), this, SLOT(startListener()));
    
    QPushButton *exitButton = new QPushButton("Exit");
    connect(exitButton, SIGNAL(clicked()), this, SLOT(exit()));

    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(exitButton);
    layout->addLayout(buttonLayout);
    

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), updater, SLOT(update()));
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(redraw()));
    
    timer->start(interval*1000);
}


void MainWindow::startListener() {
    if (config->outAddr.isNull()) {
        return;
    }
    splitter = new Splitter(config, &flowdict, this);
    splitter->start();
    startButton->setText("Restart");
    disconnect(startButton, SIGNAL(clicked()), this, SLOT(startListener()));
    connect(startButton, SIGNAL(clicked()), this, SLOT(restartListener()));
}

void MainWindow::restartListener() {
    if (config->outAddr.isNull()) {
        return;
    }
    removeRules();
    setRoute();
    updateInterfaceDict();
    interfaceInfo->refresh();
    splitter->restart();
}

void MainWindow::redraw() {
    if (interfaceInfo) {
        interfaceInfo->redraw();
    }
}

void MainWindow::setupConfigInput(QVBoxLayout *layout) {
    QIntValidator *portValid = new QIntValidator(0, 65535, this);

    QHBoxLayout *listenPortLayout = new QHBoxLayout();
    listenPortInput = new QLineEdit(QString("%1").arg(config->inPort));
    listenPortInput->setValidator(portValid);
    connect(listenPortInput, SIGNAL(editingFinished()), this, SLOT(updateListenPort()));
    
    QLabel *label = new QLabel("Listening at port (0-65535) ");
    listenPortLayout->addWidget(label);
    listenPortLayout->addWidget(listenPortInput);
    layout->addLayout(listenPortLayout);

    QHBoxLayout *outInfoLayout = new QHBoxLayout();
    outAddrInput = new QLineEdit(config->outAddr.toString());
    outPortInput = new QLineEdit(QString("%1").arg(config->outPort));
    outPortInput->setValidator(portValid);
    connect(outAddrInput, SIGNAL(editingFinished()), this, SLOT(updateOutAddr()));
    connect(outPortInput, SIGNAL(editingFinished()), this, SLOT(updateOutPort()));

    QLabel *outLabel1 = new QLabel("Send to IP ");
    QLabel *outLabel2 = new QLabel("port (0-65535) ");
    outInfoLayout->addWidget(outLabel1);
    outInfoLayout->addWidget(outAddrInput);
    outInfoLayout->addWidget(outLabel2);
    outInfoLayout->addWidget(outPortInput);
    layout->addLayout(outInfoLayout);

    QIntValidator *validBK = new QIntValidator(this);
    validBK->setBottom(0);
    
    QHBoxLayout *fecInfoLayout = new QHBoxLayout();
    bInput = new QLineEdit(QString("%1").arg(config->b));
    kInput = new QLineEdit(QString("%1").arg(config->k));
    bInput->setValidator(validBK);
    kInput->setValidator(validBK);
    connect(bInput, SIGNAL(editingFinished()), this, SLOT(updateB()));
    connect(kInput, SIGNAL(editingFinished()), this, SLOT(updateK()));

    QLabel *bkLabel1 = new QLabel("With FEC   b: ");
    QLabel *bkLabel2 = new QLabel("k: ");
    fecInfoLayout->addWidget(bkLabel1);
    fecInfoLayout->addWidget(bInput);
    fecInfoLayout->addWidget(bkLabel2);
    fecInfoLayout->addWidget(kInput);
    layout->addLayout(fecInfoLayout);
}

void MainWindow::updateListenPort() {
    bool ok;
    config->inPort = listenPortInput->text().toInt(&ok);
}
        
void MainWindow::updateOutPort() {
    bool ok;
    config->outPort = outPortInput->text().toInt(&ok);
}

void MainWindow::updateOutAddr() {
    config->outAddr = QHostAddress(outAddrInput->text());
    if (config->outAddr.isNull()) {
        errMsg.showMessage(QString("%1 is not a valid IPv4 address").arg(outAddrInput->text()));
    }
}

void MainWindow::updateB() {
    bool ok;
    config->b = bInput->text().toInt(&ok);
}

void MainWindow::updateK() {
    bool ok;
    config->k = kInput->text().toInt(&ok);
}

void MainWindow::exit() {
    emit quit();
}


