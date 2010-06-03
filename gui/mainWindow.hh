#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__
#include <QtGui>
#include "listAddr.hh"
#include "flowrategraph.hh"
class Config;
class Splitter;
class InterfaceInfo;
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
    void startListener();
    void restartListener();
    void redraw();
    void exit();
signals:
    void quit();
private:
    void updateInterfaceDict();
    void setupConfigInput(QVBoxLayout *layout);
private:
    Config *config;
    QList<Interface> infs;
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
};

#endif
