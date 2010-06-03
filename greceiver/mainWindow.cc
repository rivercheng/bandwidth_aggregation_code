#include "config.hh"
#include "decoder.hh"
#include "mainWindow.hh"
#include <QUdpSocket>
#include <QtGui>
#include <cstdlib>

MainWindow::MainWindow(Config *config)
    :config(config), listenPortInput(0), outAddrInput(0), bInput(0),kInput(0), startButton(0), decoder(0) 
{
    //Config input
    QVBoxLayout *layout = new QVBoxLayout(this);
    setupConfigInput(layout);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    startButton = new QPushButton("Start");
    connect(startButton, SIGNAL(clicked()), this, SLOT(startDecoder()));
    
    QPushButton *exitButton = new QPushButton("Exit");
    connect(exitButton, SIGNAL(clicked()), this, SLOT(exit()));

    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(exitButton);
    layout->addLayout(buttonLayout);

}

void MainWindow::createRecordingFile() {
    QString filename = "incoming_record_" + QDateTime::currentDateTime().toString("yy:MM:dd:hh:mm:ss");
    QFile *rf = new QFile(filename);
    if (! rf->open(QIODevice::WriteOnly)) {
        errMsg.showMessage(QString("cannot open recording file"));
        emit quit();
    }
    config->rf =  rf;
}

void MainWindow::startDecoder() {
    if (config->outAddr.isNull()) {
        return;
    }
    errMsg.showMessage(QString("to listen at %1 and relay to %2:%3 with b:%4 and k:%5. Delay is %6 second.").arg(config->inPort).arg(config->outAddr.toString()).arg(config->outPort).arg(config->b).arg(config->k).arg(config->delay));
    if (checkBox->isChecked()) {
        createRecordingFile();
    }

    sock = new QUdpSocket();
    if (!sock->bind(QHostAddress::Any, config->inPort)) {
        errMsg.showMessage(QString("Unable to bind to port %1").arg(config->inPort));
        //qDebug() << "unable to bind to port" << config->inPort;
        //emit quit();
    }
    inPort = config->inPort;
    decoder = new UdpDecoder(sock, config->outAddr, config->outPort, config->b, config->k, config->delay, config->rf);
    startButton->setText("Restart");
    disconnect(startButton, SIGNAL(clicked()), this, SLOT(startDecoder()));
    connect(startButton, SIGNAL(clicked()), this, SLOT(restartDecoder()));
}

void MainWindow::restartDecoder() {
    if (config->outAddr.isNull()) {
        return;
    }
    errMsg.showMessage(QString("to listen at %1 and relay to %2:%3 with b:%4 and k:%5. Delay is %6 second.").arg(config->inPort).arg(config->outAddr.toString()).arg(config->outPort).arg(config->b).arg(config->k).arg(config->delay));
    if (config->rf) {
        config->rf->close();
        delete config->rf;
        config->rf = 0;
    }
    if (checkBox->isChecked()) {
        createRecordingFile();
    }
    if (config->inPort == inPort) {
        decoder->restart(config->outAddr, config->outPort, config->b, config->k, config->delay, config->rf);
    } else {
        delete sock;
        sock = new QUdpSocket();
        if (!sock->bind(QHostAddress::Any, config->inPort)) {
            errMsg.showMessage(QString("Unable to bind to port %1").arg(config->inPort));
            //qDebug() << "unable to bind to port" << config->inPort;
            //emit quit();
        }
        inPort = config->inPort;
        delete decoder;
        decoder = new UdpDecoder(sock, config->outAddr, config->outPort, config->b, config->k, config->delay, config->rf);
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
    
    QDoubleValidator *validDelay = new QDoubleValidator(this);
    validDelay->setBottom(0);
    
    QHBoxLayout *delayLayout = new QHBoxLayout();
    delayInput = new QLineEdit(QString("%1").arg(config->delay));
    delayInput->setValidator(validDelay);
    connect(delayInput, SIGNAL(editingFinished()), this, SLOT(updateDelay()));
    
    QLabel *delayLabel = new QLabel("Delay (in seconds) ");
    delayLayout->addWidget(delayLabel);
    delayLayout->addWidget(delayInput);
    layout->addLayout(delayLayout);
    
    checkBox = new QCheckBox("Recording incoming information");
    checkBox->setCheckState(Qt::Checked);
    layout->addWidget(checkBox);
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

void MainWindow::updateDelay() {
    bool ok;
    config->delay = delayInput->text().toDouble(&ok);
}
        

void MainWindow::exit() {
    emit quit();
}


