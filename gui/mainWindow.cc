#include "config.hh"
#include "flowrategraph.hh"
#include "mainWindow.hh"
#include <QApplication>
#include <QtGui>
MainWindow::MainWindow(QList<Interface> infs, FlowrateDict *ratedict, double interval, Config *config)
    :config(config) 
{
    setWindowTitle("Multiple Channels Scheduler");
    
    QVBoxLayout *layout = new QVBoxLayout();
    FlowrateGraph *graph = new FlowrateGraph(ratedict, interval);
    layout->addWidget(graph);
    
    QVBoxLayout *infLayout = new QVBoxLayout();
    foreach (Interface inf, infs) {
        QLabel *label = new QLabel(inf.name + ":  " + inf.addr.toString());
        infLayout->addWidget(label);
    }
    layout->addLayout(infLayout);

    QIntValidator *portValid = new QIntValidator(0, 255, 0);

    QHBoxLayout *listenPortLayout = new QHBoxLayout();
    listenPortInput = new QLineEdit(QString("%1").arg(config->inPort));
    listenPortInput->setValidator(portValid);
    QLabel *label = new QLabel("Listening at port (0-65535) ");
    listenPortLayout->addWidget(label);
    listenPortLayout->addWidget(listenPortInput);
    layout->addLayout(listenPortLayout);

    QHBoxLayout *outInfoLayout = new QHBoxLayout();
    outAddrInput = new QLineEdit(config->outAddr.toString());
    outPortInput = new QLineEdit(QString("%1").arg(config->outPort));
    outPortInput->setValidator(portValid);
    QLabel *outLabel1 = new QLabel("Send to IP ");
    QLabel *outLabel2 = new QLabel("port (0-65535) ");
    outInfoLayout->addWidget(outLabel1);
    outInfoLayout->addWidget(outAddrInput);
    outInfoLayout->addWidget(outLabel2);
    outInfoLayout->addWidget(outPortInput);
    layout->addLayout(outInfoLayout);


    QIntValidator *validBK = new QIntValidator(0);
    validBK->setBottom(0);
    
    QHBoxLayout *fecInfoLayout = new QHBoxLayout();
    bInput = new QLineEdit(QString("%1").arg(config->b));
    kInput = new QLineEdit(QString("%1").arg(config->k));
    bInput->setValidator(validBK);
    QLabel *bkLabel1 = new QLabel("With FEC   b: ");
    QLabel *bkLabel2 = new QLabel("k: ");
    fecInfoLayout->addWidget(bkLabel1);
    fecInfoLayout->addWidget(bInput);
    fecInfoLayout->addWidget(bkLabel2);
    fecInfoLayout->addWidget(kInput);
    layout->addLayout(fecInfoLayout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    startButton = new QPushButton("Start");
    QPushButton *exitButton = new QPushButton("Exit");
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(exitButton);
    layout->addLayout(buttonLayout);
    
    setLayout(layout);

    connect(this, SIGNAL(replot()), graph, SLOT(plot()));

    connect(startButton, SIGNAL(clicked()), this, SLOT(startListener()));
    connect(exitButton, SIGNAL(clicked()), this, SLOT(exit()));
}

void MainWindow::updateConfig() {
    bool ok;
    config->inPort = listenPortInput->text().toInt(&ok);
    config->outPort = outPortInput->text().toInt(&ok);
    config->outAddr = QHostAddress(outAddrInput->text());
    config->b = bInput->text().toInt(&ok);
    config->k = kInput->text().toInt(&ok);
}

void MainWindow::startListener() {
        emit start();
        qDebug() << "in start";
        startButton->setText("Restart");
        disconnect(startButton, SIGNAL(clicked()), this, SLOT(startListener()));
        connect(startButton, SIGNAL(clicked()), this, SLOT(restartListener()));
}
        

