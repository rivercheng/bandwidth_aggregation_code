#ifndef __LOOPER_HH__
#define __LOOPER_HH__
#include <QObject>
#include "mainWindow.hh"
#include "config.hh"

class Looper : public QObject {
    Q_OBJECT
public:
    Looper(Config *config, QObject* parent = 0)
        :QObject(parent), config(config), mainWindow(0)
    {
        init();
    }
public slots:
    void restart() {
        delete mainWindow;
        mainWindow = 0;
        init();
    }
    void exit() {
        emit quit();
    }
signals:
    void quit();
private:
    void init() {
        mainWindow = new MainWindow(config); 
        mainWindow->setWindowTitle("Multiple Channels Scheduler");
        mainWindow->show();
        QObject::connect(mainWindow, SIGNAL(quit()), this, SLOT(exit()));
        QObject::connect(mainWindow, SIGNAL(restart()), this, SLOT(restart()));
        mainWindow->startListener();
    }
private:
    Config *config;
    MainWindow *mainWindow;
};

#endif
