#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__
#include <QtGui>
#include "listAddr.hh"
#include "flowrategraph.hh"
class Config;
class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(QList<Interface> infs, FlowrateDict *ratedict, double interval, Config *config);
private slots:
    void updateConfig();
public slots:
    void plot() {
        emit replot();
    }
    void startListener();
    
    void restartListener() {
        emit restart();
    }
    void exit() {
        emit quit();
    }
signals:
    void replot();
    void start();
    void restart();
    void quit();
private:
    Config* config;
    QLineEdit *listenPortInput;
    QLineEdit *outAddrInput; 
    QLineEdit *outPortInput;
    QLineEdit *bInput; 
    QLineEdit *kInput;
    QPushButton *startButton;
};

#endif
