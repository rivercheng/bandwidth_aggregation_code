#include "config.hh"
#include "updater.hh"
#include "flowrategraph.hh"
#include "mainWindow.hh"
#include "splitter.hh"
#include "interfaceInfo.hh"
#include <QApplication>
#include <QtGui>
#include <QTimer>

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
    
    splitter = new Splitter(config, &flowdict, this);

    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), updater, SLOT(update()));
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(redraw()));
    
    timer->start(interval*1000);
}


void MainWindow::startListener() {
    splitter->start();
    startButton->setText("Restart");
    disconnect(startButton, SIGNAL(clicked()), this, SLOT(startListener()));
    connect(startButton, SIGNAL(clicked()), this, SLOT(restartListener()));
}

void MainWindow::restartListener() {
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
    QIntValidator *portValid = new QIntValidator(0, 255, this);

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
}

void MainWindow::updateB() {
    bool ok;
    config->b = bInput->text().toInt(&ok);
}

void MainWindow::updateK() {
    bool ok;
    config->k = kInput->text().toInt(&ok);
}



