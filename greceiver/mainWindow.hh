#ifndef __RECEIVER_MAIN_WINDOW_H__
#define __RECEIVER_MAIN_WINDOW_H__
#include <QtGui>
class Config;
class UdpDecoder;
class QUdpSocket;
class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(Config *config);
private slots:
    void updateListenPort();
    void updateOutPort();
    void updateOutAddr();
    void updateB();
    void updateK();
    void updateDelay();
    void startDecoder();
    void restartDecoder();
    void exit();
signals:
    void quit();
private:
    void setupConfigInput(QVBoxLayout *layout);
    void createRecordingFile();
private:
    Config *config;
    QLineEdit *listenPortInput;
    QLineEdit *outAddrInput; 
    QLineEdit *outPortInput;
    QLineEdit *bInput; 
    QLineEdit *kInput;
    QLineEdit *delayInput;
    QCheckBox *checkBox;
    QPushButton *startButton;
    UdpDecoder  *decoder;
    QUdpSocket  *sock;
    quint16     inPort;
    QErrorMessage errMsg;
};

#endif
