#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__
#include <QtGui>
#include "listAddr.hh"
#include "flowrategraph.hh"
class Config;
class Splitter;
class InterfaceInfo;
class RouteProcessor;
class MainWindow : public QWidget {
    Q_OBJECT
public:
    MainWindow(Config *config);
public slots:
    void startListener();
private slots:
    void updateListenPort();
    void updateOutPort();
    void updateOutAddr();
    void updateB();
    void updateK();
    void restartListener();
    void redraw();
    void exit();
signals:
    void quit();
    void restart();
private:
    void updateInterfaceDict();
    void setupConfigInput(QVBoxLayout *layout);
    void setRoute();
    void removeRules();
private:
    Config *config;
    //QList<Interface> infs;
    FlowrateDict ratedict;
    FlowDict     flowdict;
    InterfaceInfo *interfaceInfo;
    QLineEdit *listenPortInput;
    QLineEdit *outAddrInput; 
    QLineEdit *outPortInput;
    QLineEdit *bInput; 
    QLineEdit *kInput;
    QPushButton *startButton;
    Splitter  *splitter;
    QErrorMessage errMsg;
    QList<QNetworkInterface> infs;
    RouteProcessor *routeP;
};

#endif
